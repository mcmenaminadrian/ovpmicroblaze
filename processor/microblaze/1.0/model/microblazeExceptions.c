/*
 * Copyright (c) 2005-2014 Imperas Software Ltd., www.imperas.com
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied.
 *
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "hostapi/impAlloc.h"

#include "vmi/vmiMessage.h"
#include "vmi/vmiRt.h"
#include "microblazeExceptions.h"
#include "microblazeStructure.h"
#include "microblazeMorphFP.h"
#include "microblazeUser.h"
#include "microblazeUtils.h"
#include "microblazeInstructions.h"
#include "microblazeDecodeUser.h"
#include "microblazeTLB.h"

//
// NB
// Reset, interrupts, exceptions, and breaks (including the BRK and BRKI instructions)
// all clear the reservation.
//

//ESR[DS] <- exception in delay slot
//if ESR[DS] then
//   BTR <- branch target PC
//   if MMU exception then
//       if branch preceded by IMM then
//          r17 <- PC - 8
//       else
//          r17 <- PC - 4
//   else
//       r17 <- invalid value
//else if MMU exception then
//   if instruction preceded by IMM then
//       r17 <- PC - 4
//   else
//       r17 <- PC
//else
//   r17 <- PC + 4
//PC <- 0x00000020
//MSR[EE] <- 0, MSR[EIP]<- 1
//MSR[UMS] <- MSR[UM], MSR[UM] <- 0, MSR[VMS] <- MSR[VM], MSR[VM] <- 0
//ESR[EC] <- exception specific value
//ESR[ESS] <- exception specific value
//EAR <- exception specific value
//FSR <- exception specific value
//Reservation <- 0

//static void mmuException (microblazeP microblaze) {
//    if (delaySlotOffset) {
//        microblaze->SPR_ESR.bits.DS = 1;
//        microblaze->SPR_BTR.reg = simPC + delaySlotOffset;
//        // if (branch preceeded by imm)
//        //  r17 = PC - 8
//        // else
//        //  r17 = PC - 4
//    } else {
//        // if (branch preceeded by imm)
//        //  r17 = PC - 4
//        // else
//        //  r17 = PC
//    }
////}

static void commonException(microblazeP microblaze, Bool MMU_exception) {

    Uns8          delaySlotOffset;
    vmiProcessorP processor = (vmiProcessorP)microblaze;

    microblazeExceptionEnter(microblaze);

    Uns32 simPC = (Uns32) vmirtGetPCDS(processor, &delaySlotOffset);

    if (delaySlotOffset) {
        microblaze->SPR_ESR.bits.DS = 1;
        // microblaze->SPR_BTR.reg; set at morph time
        if (MMU_exception) {
            microblaze->gpr[17] = simPC;
        } else {
            microblaze->gpr[17] = microblaze->SPR_BTR.reg;
        }
    } else {
        if (MMU_exception) {
            microblaze->gpr[17] = simPC;
        } else {
            //
            // Get the next PC from the Utility, this ensures we handle the imm
            // instruction coalescing
            //
            simPC = (Uns32)microblazeNextInstructionUser(processor, simPC);
            microblaze->gpr[17] = simPC;
        }
    }

    //
    // save the VM mode
    //
    microblazeSaveVMMode(microblaze);
    vmirtSetPCException((vmiProcessorP)microblaze, HARDWARE_EXCEPTION_VECTOR);
}

//
// Take an MMU storage exception
//
void microblazeMMUStorageException(
    microblazeP         microblaze,
    microblazeTLBAction action,
    Uns32               va,
    Bool                DIZ
) {
    commonException(microblaze, True);

    microblaze->SPR_EAR.reg = va;

    if(action==TLBA_FETCH) {
        microblaze->SPR_ESR.bits.EC = INSTRUCTION_STORAGE_EXCEPTION;
        microblaze->SPR_ESR.bits_ESS_EC_10001.DIZ = DIZ;
        microblaze->exception = microblaze_E_INSTRUCTION_STORAGE_EXCEPTION;
    } else {
        microblaze->SPR_ESR.bits.EC = DATA_STORAGE_EXCEPTION;
        microblaze->SPR_ESR.bits_ESS_EC_10000.DIZ = DIZ;
        microblaze->SPR_ESR.bits_ESS_EC_10000.S = (action==TLBA_STORE);
        microblaze->exception = microblaze_E_DATA_STORAGE_EXCEPTION;
    }

    microblazeUpdateBlockMask(microblaze);
}

//
// Take an MMU storage exception
//
void microblazeTLBMissException(
    microblazeP         microblaze,
    microblazeTLBAction action,
    Uns32               va
) {
    commonException(microblaze, True);

    microblaze->SPR_EAR.reg = va;

    if(action==TLBA_FETCH) {
        microblaze->SPR_ESR.bits.EC = INSTRUCTION_TLB_MISS_EXCEPTION;
        microblaze->exception = microblaze_E_INSTRUCTION_TLB_MISS_EXCEPTION;
    } else {
        microblaze->SPR_ESR.bits.EC = DATA_TLB_MISS_EXCEPTION;
        microblaze->SPR_ESR.bits_ESS_EC_10010.S = (action==TLBA_STORE);
        microblaze->exception = microblaze_E_DATA_TLB_MISS_EXCEPTION;
    }

    microblazeUpdateBlockMask(microblaze);
}


static void nonmmuException (microblazeP microblaze) {

    commonException(microblaze, False);

    microblazeUpdateBlockMask(microblaze);

    //
    // Clear the reservation
    //
    microblaze->FLG_reservation = 0;
}

//
// Runtime callback to set the FSL bit ERROR
//
void microblazeStreamExceptionCB(vmiProcessorP processor, Uns32 insn, Uns32 e, Uns32 d, Uns32 fsl, Uns32 rB, Uns32 rD) {
    microblazeP microblaze = (microblazeP)processor;

    microblaze->SPR_MSR.bits.FSL = 1;

    //
    // Are exceptions enabled ?
    //
    if (microblaze->SPR_MSR.bits.EE) {
        Uns8 linkIndex;

        //
        // Is this an exception command ?
        // if so call the exception handler
        //
        if (e) {
            if (d) {
                //
                // Direct
                //
                linkIndex = microblaze->gpr[rB];
            } else {
                //
                // Immediate
                //
                linkIndex = fsl;
            }

            microblaze->SPR_ESR.bits.EC = STREAM_EXCEPTION;
            microblaze->SPR_ESR.bits_ESS_EC_00000.FSL = linkIndex;
            microblaze->SPR_EDR.reg = microblaze->gpr[rD];
            microblaze->exception = microblaze_E_STREAM_EXCEPTION;


            //
            // Restore the old rD saved in tmp[2]
            //
            microblaze->gpr[rD] = microblaze->tmp[2];

            //
            // This register is invalid, indicate by appending EC to 0xdead00..
            //
            microblaze->SPR_EAR.reg = 0xdeadea00 | microblaze->SPR_ESR.bits.EC;

            //
            // Call the general Hardware Exception routine
            //
            nonmmuException(microblaze);
        }
    }
}

void microblazehwExceptionCB(vmiProcessorP processor, TYPE_SPR_ESR ESR) {
    microblazeP microblaze = (microblazeP)processor;

    if (microblaze->SPR_MSR.bits.EE) {

        microblaze->SPR_ESR = ESR;

        //
        // This register is invalid, indicate by appending EC to 0xdead00..
        //
        microblaze->SPR_EAR.reg = 0xdeadea00 | microblaze->SPR_ESR.bits.EC;

        //
        // Call the general Hardware Exception routine
        //
        nonmmuException(microblaze);
    }
}

VMI_RD_PRIV_EXCEPT_FN(microblazeRdPrivExcept) {

    microblazeP microblaze = (microblazeP)processor;
    Uns32       faultAddr  = address;

    //  microblaze->exception = microblaze_E_RdPriv;

    if(!microblazeTLBMiss(microblaze, TLBA_LOAD, faultAddr, attrs)) {
        *action = VMI_LOAD_STORE_CONTINUE;
    }
}

VMI_WR_PRIV_EXCEPT_FN(microblazeWrPrivExcept) {

    microblazeP microblaze = (microblazeP)processor;
    Uns32       faultAddr  = address;

    //  microblaze->exception = microblaze_E_WrPriv;

    if(!microblazeTLBMiss(microblaze, TLBA_STORE, faultAddr, attrs)) {
        *action = VMI_LOAD_STORE_CONTINUE;
    }
}

VMI_RD_ABORT_EXCEPT_FN(microblazeRdAbortExcept) {
    microblazeP microblaze = (microblazeP)processor;

    nonmmuException(microblaze);

    microblaze->SPR_EAR.reg = address;

    if (isFetch) {
        // Instruction Read Abort
        microblaze->SPR_ESR.bits.EC = INSTRUCTION_BUS_ERROR_EXCEPTION;
        microblaze->SPR_ESR.bits_ESS_EC_00011.ECC = 0;
        microblaze->exception = microblaze_E_INSTRUCTION_BUS_ERROR_EXCEPTION;
    } else {
        // Data Read Abort
        microblaze->SPR_ESR.bits.EC = DATA_BUS_ERROR_EXCEPTION;
        microblaze->SPR_ESR.bits_ESS_EC_00100.ECC = 0;
        microblaze->exception = microblaze_E_DATA_BUS_ERROR_EXCEPTION;
    }
}

VMI_WR_ABORT_EXCEPT_FN(microblazeWrAbortExcept) {
    microblazeP microblaze = (microblazeP)processor;

    nonmmuException(microblaze);

    microblaze->SPR_EAR.reg = address;

    // Data Write Abort
    microblaze->SPR_ESR.bits.EC = DATA_BUS_ERROR_EXCEPTION;
    microblaze->SPR_ESR.bits_ESS_EC_00100.ECC = 0;
    microblaze->exception = microblaze_E_DATA_BUS_ERROR_EXCEPTION;
}

//
//      ESR[EC] = 00001; ESR[W] = isWord; ESR[S] = isWrite; ESR[Rx] = rD
//
static void machineAlignError(vmiProcessorP processor, Uns32 bytes, Uns32 rDidx, Uns32 isWrite) {
    microblazeP microblaze = (microblazeP)processor;

    microblaze->SPR_ESR.reg     = 0x0;
    microblaze->SPR_ESR.bits.EC = UNALIGNED_DATA_ACCESS;
    microblaze->exception = microblaze_E_UNALIGNED_DATA_ACCESS;
    switch (bytes) {
    case 4:
        microblaze->SPR_ESR.bits_ESS_EC_00001.W  = 1;
        break;
    case 2:
    case 1:
        microblaze->SPR_ESR.bits_ESS_EC_00001.W  = 0;
        break;
    default:
        vmiMessage("F", "machineAlignError", "%s bytes=%d",
                (isWrite ? "Write" : "Read"), bytes);
                /* no break */
    }
    microblaze->SPR_ESR.bits_ESS_EC_00001.S  = isWrite;
    microblaze->SPR_ESR.bits_ESS_EC_00001.Rx = rDidx;

    //
    // Call the general Hardware Exception routine
    //
    nonmmuException(microblaze);
}

VMI_RD_ALIGN_EXCEPT_FN(microblazeRdAlignExcept) {
    microblazeP microblaze = (microblazeP)processor;
    Uns32 rDidx = microblaze->tmp[0];

    Bool TLB_Miss = False;
    Bool Access_Protected = False;

    //
    // The address of the load
    //
    microblaze->SPR_EAR.reg = address;

//    Addr = (rA) + sext(IMM)
//    if TLB_Miss(Addr) and MSR[VM] = 1 then
//      ESR[EC]=10010;ESR[S]= 0
//      MSR[UMS]=MSR[UM]; MSR[VMS]=MSR[VM]; MSR[UM]=0; MSR[VM]=0
//    else if Access_Protected(Addr) and MSR[UM]=1 and MSR[VM]=1 then
//      ESR[EC]=10000; ESR[S]=0; ESR[DIZ]=1
//      MSR[UMS]=MSR[UM]; MSR[VMS]=MSR[VM]; MSR[UM]=0; MSR[VM]=0
//    else if Addr[31] != 0 then
//      ESR[EC]=00001; ESR[W]=0; ESR[S]=0; ESR[Rx]=rD

    if        (TLB_Miss && microblaze->SPR_MSR.bits.VM) {
    } else if (Access_Protected && microblaze->SPR_MSR.bits.UM && microblaze->SPR_MSR.bits.VM) {
    } else {
        machineAlignError(processor, bytes, rDidx, 0);
    }

    return 0;
}

VMI_WR_ALIGN_EXCEPT_FN(microblazeWrAlignExcept) {
    microblazeP microblaze = (microblazeP)processor;
    Uns32 rDidx = microblaze->tmp[0];

    Bool TLB_Miss = False;
    Bool Access_Protected = False;

    //
    // The address of the load
    //
    microblaze->SPR_EAR.reg = address;

//    Addr = (rA) + sext(IMM)
//    if TLB_Miss(Addr) and MSR[VM] = 1 then
//      ESR[EC]= 10010;ESR[S]= 1
//      MSR[UMS] = MSR[UM]; MSR[VMS] = MSR[VM]; MSR[UM] = 0; MSR[VM] = 0
//    else if Access_Protected(Addr) and MSR[VM] = 1 then
//      ESR[EC] = 10000;ESR[S]= 1; ESR[DIZ] = No-access-allowed
//      MSR[UMS] = MSR[UM]; MSR[VMS]= MSR[VM]; MSR[UM] = 0; MSR[VM] = 0
//    else if Addr[31]  0 then
//      ESR[EC] = 00001; ESR[W] = 0; ESR[S] = 1; ESR[Rx] = rD

    if        (TLB_Miss && microblaze->SPR_MSR.bits.VM) {
    } else if (Access_Protected && microblaze->SPR_MSR.bits.UM && microblaze->SPR_MSR.bits.VM) {
    } else {
        machineAlignError(processor, bytes, rDidx, 1);
    }

    return 0;
}

static void setDefaultESR(microblazeP microblaze, Uns32 rDidx, Uns32 EC, microblazeException exception) {
    microblaze->SPR_ESR.reg                   = 0x0;
    microblaze->SPR_ESR.bits.EC               = EC;
    microblaze->SPR_ESR.bits_ESS_EC_00001.Rx  = rDidx;
    microblaze->exception                     = exception;
}

VMI_ARITH_EXCEPT_FN(microblazeArithExcept) {
    microblazeP microblaze = (microblazeP)processor;
    Uns32 rDidx, rDval;

    rDidx = microblaze->tmp[0];
    rDval = microblaze->tmp[1];

    switch(exceptionType) {

//    Uns32 I  : 1;   // invalid operation flag
//    Uns32 D  : 1;   // denormal flag
//    Uns32 Z  : 1;   // divide-by-zero flag
//    Uns32 O  : 1;   // overflow flag
//    Uns32 U  : 1;   // underflow flag
//    Uns32 P  : 1;   // precision flag
//    Uns32 R1 : 1;   // (reserved for internal use)
//    Uns32 R2 : 1;   // (reserved for internal use)

        case VMI_INTEGER_DIVIDE_BY_ZERO:
            //
            // Need to check if we have EE bit set in MSR
            // and the core is configured for exceptions
            // icmAddUns64Attr(userAttrs, "C_DIV_ZERO_EXCEPTION", 1);
            //
            microblaze->gpr[rDidx] = 0x00000000;
            microblaze->SPR_MSR.bits.DZO = 1;

            if (microblaze->SPR_PVR2.bits.DIVEXC && microblaze->SPR_MSR.bits.EE) {
                //
                // Restore the overwritten register
                //
                microblaze->gpr[rDidx] = rDval;

                //
                // ESR Register
                // Default is to behave as though an unaligned address exception
                // Other conditions loaded afterwards
                //
                setDefaultESR(microblaze, rDidx, DIVIDE_EXCEPTION, microblaze_E_DIVIDE_EXCEPTION);
                microblaze->SPR_ESR.bits_ESS_EC_00101.DEC = 0;

                //
                // This register is invalid, indicate by appending EC to 0xdead00..
                //
                microblaze->SPR_EAR.reg = 0xdeadea00 | microblaze->SPR_ESR.bits.EC;

                //
                // Call the general Hardware Exception routine
                //
                nonmmuException(microblaze);
            } else {
                //
                // No exception generated
                //
            }
            return VMI_INTEGER_ABORT;

        case VMI_INTEGER_OVERFLOW:
            //
            // Need to check if we have EE bit set in MSR
            // and the core is configured for exceptions
            // icmAddUns64Attr(userAttrs, "C_DIV_ZERO_EXCEPTION", 1);
            // rDidx is held in tmp0, rDprev in tmp1
            //
            microblaze->SPR_MSR.bits.DZO = 1;
            if (microblaze->SPR_PVR2.bits.DIVEXC && microblaze->SPR_MSR.bits.EE) {
                //
                // Restore the overwritten register
                //
                microblaze->gpr[rDidx] = rDval;

                //
                // ESR Register
                // Default is to behave as though an unaligned address exception
                // Other conditions loaded afterwards
                //
                setDefaultESR(microblaze, rDidx, DIVIDE_EXCEPTION, microblaze_E_DIVIDE_EXCEPTION);
                microblaze->SPR_ESR.bits_ESS_EC_00101.DEC = 1;

                //
                // This register is invalid, indicate by appending EC to 0xdead00..
                //
                microblaze->SPR_EAR.reg = 0xdeadea00 | microblaze->SPR_ESR.bits.EC;

                //
                // Call the general Hardware Exception routine
                //
                nonmmuException(microblaze);
            } else {
                //
                // No exception generated
                //
                microblaze->gpr[rDidx] = 0x80000000;
            }
            return VMI_INTEGER_ABORT;

            // not expecting any other arithmetic exception types
        default:
            vmiPrintf("Exception UNKNOWN\n");
            return VMI_INTEGER_UNHANDLED;
    }

    return VMI_INTEGER_UNHANDLED;
}

VMI_FP_ARITH_EXCEPT_FN(microblazeFPArithExcept) {
    microblazeP microblaze = (microblazeP)processor;
    Uns32 rDidx;

    rDidx = microblaze->tmp[0];

    //
    // ESR Register
    // Default is to behave as though an unaligned address exception
    // Other conditions loaded afterwards
    //
    setDefaultESR(microblaze, rDidx, FLOATING_POINT_UNIT_EXCEPTION, microblaze_E_FLOATING_POINT_UNIT_EXCEPTION);

    if       (flags->f.I) {
        //
        // invalid operation
        //
        microblaze->SPR_FSR.bits.IO = 1;
    } else if(flags->f.D) {
        //
        // denormal flag
        //
        microblaze->SPR_FSR.bits.DO = 1;
    } else if(flags->f.Z) {
        //
        // divide-by-zero flag
        //
        microblaze->SPR_FSR.bits.DZ = 1;
    } else if(flags->f.O) {
        //
        // overflow flag
        //
        microblaze->SPR_FSR.bits.OF = 1;
    } else if(flags->f.U) {
        //
        // underflow flag
        //
        microblaze->SPR_FSR.bits.UF = 1;
    } else {
        //
        // Unknown, or Masked Exception Flag
        //
        vmiMessage("F", "MB_FLTEXC", "Exception VMI_FLOAT_EXCEPTION UNKNOWN flags = 0x%02x\n", flags->bits);
        return VMI_FLOAT_UNHANDLED;
    }

    microblazeEmptyFSR(microblaze);

    //
    // This register is invalid, indicate by appending EC to 0xdead00..
    //
    microblaze->SPR_EAR.reg = 0xdeadea00 | microblaze->SPR_ESR.bits.EC;

    nonmmuException(microblaze);

    return VMI_FLOAT_CONTINUE;
}

static Bool isIntActive(microblazeP microblaze) {

    if (microblaze->ResetPending) {
        // Held in reset
        return False;
    }

    //
    // Test Level / Edge against Enable
    //
    Bool irqedge   = microblaze->SPR_PVR2.bits.IRQEDGE;
    Bool irqpos    = microblaze->SPR_PVR2.bits.IRQPOS;
    Bool ie        = microblaze->SPR_MSR.bits.IE;
    Bool eip       = microblaze->SPR_MSR.bits.EIP;
    Bool bip       = microblaze->SPR_MSR.bits.BIP;
    Bool intlevel  = microblaze->Interrupt;
    Bool intedge   = (irqpos && microblaze->InterruptPos) || (!irqpos && microblaze->InterruptNeg);

    if (ie && !eip && !bip) {
        if (irqedge && intedge) {
            //
            // Test EDGE
            //
            microblaze->InterruptActive = True;

        } else if (irqpos == intlevel) {
            //
            // Test LEVEL Active
            //
            microblaze->InterruptActive = True;
        }
    }

    return microblaze->InterruptActive;
}

void microblazeProcessReset(vmiProcessorP processor) {

    microblazeP  microblaze = (microblazeP)processor;
    //
    // Perform reset actions, then jump to reset vector
    //
    microblaze->exception = microblaze_E_RESET;

    microblazeRegisterInit(microblaze);

    // jump to reset vector
    vmirtSetPCException((vmiProcessorP)microblaze, RESET_VECTOR);
}

void microblazeProcessInterrupt(vmiProcessorP processor) {

    microblazeP microblaze = (microblazeP)processor;
    microblaze->exception  = microblaze_E_INTERRUPT;

    Uns32 simPC;
    Uns8 delaySlotOffset;
    simPC = (Uns32) vmirtGetPCDS(processor, &delaySlotOffset);

    // Save the PC in R14
    microblaze->gpr[14] = simPC;

    microblazeInterruptEnter(microblaze);
    microblazeSaveVMMode(microblaze);
    microblazeUpdateBlockMask(microblaze);

    microblaze->FLG_reservation  = 0;

    // jump to interrupt vector
    vmirtSetPCException((vmiProcessorP)microblaze, INTERRUPT_VECTOR);

    //
    // Clear the edge detection bits
    //
    microblaze->InterruptPos    = False;
    microblaze->InterruptNeg    = False;
    microblaze->InterruptActive = False;
}

//
// Validate that the passed address is a mapped fetch address
//
static Bool validateFetchAddress(microblazeP microblaze, Uns32 simPC, Bool complete) {

    memAccessAttrs attrs = complete ? MEM_AA_TRUE : MEM_AA_FALSE;

    if(vmirtIsExecutable((vmiProcessorP)microblaze, simPC)) {

        // no exception pending
        return True;

    } else if(microblazeTLBMiss(microblaze, TLBA_FETCH, simPC, attrs)) {

        // TLB miss exception of some kind, handled by mipsTLBMiss, so no
        // further action required here.
        return False;

    } else {

        // no exception pending
        return True;
    }
}

VMI_IFETCH_FN(microblazeIfetchExcept) {

    microblazeAddr simPC      = address;
    microblazeP    microblaze = (microblazeP)processor;
    Bool           fetchOK    = False;

    // priority-ordered list of exception states
    if(isIntActive(microblaze)) {

        // update registers to complete interrupt exception if required
        if(complete) {
            microblazeProcessInterrupt(processor);
        }

    } else if(!validateFetchAddress(microblaze, simPC, complete)) {

        // mapping error - (handled in validateFetchAddress)

    } else if((simPC+4) & (MIN_PAGE_SIZE-1)) {

        // simPC isn't two bytes before page end - success
        fetchOK = True;

    } else if(microblazeGetInstructionBytes(microblaze, simPC) == 4) {

        // instruction at simPC is a four-byte instruction - success
        fetchOK = True;

    } else if(!validateFetchAddress(microblaze, simPC+4, complete)) {

        // mapping error for second instruction halfword (handled in
        // validateFetchAddress)

    } else {

        // no exception pending
        fetchOK = True;
    }

    // return appropriate result
    if(fetchOK) {
        return VMI_FETCH_NONE;
    } else if(complete) {
        return VMI_FETCH_EXCEPTION_COMPLETE;
    } else {
        return VMI_FETCH_EXCEPTION_PENDING;
    }
}

VMI_ICOUNT_FN(microblazeIcountExcept) {
}

//
// Table of exception mode descriptors
//
static const vmiExceptionInfo exceptions[] = {
    //
    // Synchronous exceptions
    //
    { "STREAM_EXCEPTION",                       microblaze_E_STREAM_EXCEPTION                      },
    { "UNALIGNED_DATA_ACCESS",                  microblaze_E_UNALIGNED_DATA_ACCESS                 },
    { "ILLEGAL_OPCODE_EXCEPTION",               microblaze_E_ILLEGAL_OPCODE_EXCEPTION              },
    { "INSTRUCTION_BUS_ERROR_EXCEPTION",        microblaze_E_INSTRUCTION_BUS_ERROR_EXCEPTION       },
    { "DATA_BUS_ERROR_EXCEPTION",               microblaze_E_DATA_BUS_ERROR_EXCEPTION              },
    { "DIVIDE_EXCEPTION",                       microblaze_E_DIVIDE_EXCEPTION                      },
    { "FLOATING_POINT_UNIT_EXCEPTION",          microblaze_E_FLOATING_POINT_UNIT_EXCEPTION         },
    { "PRIVILEGED_INSTRUCTION_EXCEPTION",       microblaze_E_PRIVILEGED_INSTRUCTION_EXCEPTION      },
    { "STACK_PROTECTION_VIOLATION_EXCEPTION",   microblaze_E_STACK_PROTECTION_VIOLATION_EXCEPTION  },
    { "DATA_STORAGE_EXCEPTION",                 microblaze_E_DATA_STORAGE_EXCEPTION                },
    { "INSTRUCTION_STORAGE_EXCEPTION",          microblaze_E_INSTRUCTION_STORAGE_EXCEPTION         },
    { "DATA_TLB_MISS_EXCEPTION",                microblaze_E_DATA_TLB_MISS_EXCEPTION               },
    { "INSTRUCTION_TLB_MISS_EXCEPTION",         microblaze_E_INSTRUCTION_TLB_MISS_EXCEPTION        },
    //
    // Asynchronous exceptions
    //
    { "RESET",                                  microblaze_E_RESET,                                },
    { "INTERRUPT",                              microblaze_E_INTERRUPT,                            },
    { 0 },
};

VMI_GET_EXCEPTION_FN(microblazeGetException) {
    microblazeP microblaze = (microblazeP)processor;
    return &exceptions[microblaze->exception];
}

VMI_EXCEPTION_INFO_FN(microblazeExceptionInfo) {

    // on the first call, start with the first member of the table
    if(!prev) {
        return exceptions;
    }
    prev++;
    return prev->name ? prev : 0;
}

void microblazeDoException(microblazeP microblaze) {
    vmiProcessorP processor = (vmiProcessorP)microblaze;

    vmirtRestartNext(processor);
    vmirtDoSynchronousInterrupt(processor);
}

//
// mask is a pointer to the mask register in the structure
// left and right are the indices 0..31, so reverse format
// similar to the PPC
//
void microblazeMaskEnable(Uns32 *mask, Bool Enable, Uns32 left, Uns32 right) {
    //
    // set bits left downto right (inclusive) to be the value '1' in the mask
    //
    Uns32 l = 31 - left;
    Uns32 r = 31 - right;
    Uns32 addmask = 0xffffffff;

    if (Enable) {
        *mask |= VECINDEX(addmask, l, r, r);
    } else {
        *mask &= ~(VECINDEX(addmask, l, r, r));
    }
}

void microblazeRegisterInit (microblazeP microblaze) {
    microblaze->SPR_PC          = 0;
    microblaze->SPR_MSR.reg     = 0;
    microblaze->SPR_EAR.reg     = 0;
    microblaze->SPR_ESR.reg     = 0;
    microblaze->SPR_BTR.reg     = 0;
    microblaze->SPR_FSR.reg     = 0;
    microblaze->SPR_EDR.reg     = 0;
    microblaze->SPR_SLR.reg     = 0;
    microblaze->SPR_SHR.reg     = 0xffffffff;
    microblaze->SPR_PID.reg     = 0;
    microblaze->SPR_ZPR.reg     = 0;
    microblaze->SPR_TLBLO.reg   = 0;
    microblaze->SPR_TLBHI.reg   = 0;
    microblaze->SPR_TLBX.reg    = 0;
    microblaze->SPR_TLBSX.reg   = 0;
    microblaze->FLG_reservation = 0;

    //
    // Some compound Booleans
    //
    Bool ANY_EXCEPTION =    microblaze->params.C_UNALIGNED_EXCEPTIONS  ||
                            microblaze->params.C_ILL_OPCODE_EXCEPTION  ||
                            microblaze->params.C_IPLB_BUS_EXCEPTION    ||
                            microblaze->params.C_DPLB_BUS_EXCEPTION    ||
                            microblaze->params.C_M_AXI_I_BUS_EXCEPTION ||
                            microblaze->params.C_M_AXI_D_BUS_EXCEPTION ||
                            microblaze->params.C_DIV_ZERO_EXCEPTION    ||
                            microblaze->params.C_FPU_EXCEPTION         ||
                            microblaze->params.C_USE_MMU;

    //
    // MSR
    //
    microblazeMaskEnable(&microblaze->SPR_MSR_rmask.reg, True, 0, 31);
    microblaze->SPR_MSR.reg      = microblaze->params.C_RESET_MSR;
    microblaze->SPR_MSR.bits.PVR = (microblaze->params.C_PVR) ? 1 : 0;
    microblazeMaskEnable(&microblaze->SPR_MSR_wmask.reg, False, 0, 31);         // Disable All
    if (microblaze->params.C_USE_MMU > 1) {
        microblazeMaskEnable(&microblaze->SPR_MSR_wmask.reg, True, 17, 18);     // VMS VM
    }
    if (microblaze->params.C_USE_MMU > 0) {
        microblazeMaskEnable(&microblaze->SPR_MSR_wmask.reg, True, 19, 20);     // UMS UM
    }
    if (microblaze->params.C_USE_MMU || ANY_EXCEPTION) {
        microblazeMaskEnable(&microblaze->SPR_MSR_wmask.reg, True, 22, 23);     // EIP EE
    }
    if (microblaze->params.C_USE_DCACHE == 1) {
        microblazeMaskEnable(&microblaze->SPR_MSR_wmask.reg, True, 24, 24);     // DCE
    }
    if (microblaze->params.C_USE_DIV == 1) {
        microblazeMaskEnable(&microblaze->SPR_MSR_wmask.reg, True, 25, 25);     // DZO
    }
    if (microblaze->params.C_USE_ICACHE == 1) {
        microblazeMaskEnable(&microblaze->SPR_MSR_wmask.reg, True, 26, 26);     // ICE
    }
    if (microblaze->params.C_FSL_LINKS > 0) {
        microblazeMaskEnable(&microblaze->SPR_MSR_wmask.reg, True, 27, 27);     // FSL
    }
    microblazeMaskEnable(&microblaze->SPR_MSR_wmask.reg, True, 28, 30);         // BIP C IE

    // EAR
    microblazeMaskEnable(&microblaze->SPR_EAR_rmask.reg, True, 0, 31);
    microblazeMaskEnable(&microblaze->SPR_EAR_wmask.reg, True, 0, 31);

    // ESR
    microblazeMaskEnable(&microblaze->SPR_ESR_rmask.reg, True, 0, 31);
    microblazeMaskEnable(&microblaze->SPR_ESR_wmask.reg, True, 0, 31);

    // BTR
    microblazeMaskEnable(&microblaze->SPR_BTR_rmask.reg, True, 0, 31);
    microblazeMaskEnable(&microblaze->SPR_BTR_wmask.reg, True, 0, 31);

    // FSR
    microblazeMaskEnable(&microblaze->SPR_FSR_rmask.reg, True, 0, 31);
    microblazeMaskEnable(&microblaze->SPR_FSR_wmask.reg, True, 0, 31);

    // EDR
    microblazeMaskEnable(&microblaze->SPR_EDR_rmask.reg, True, 0, 31);
    microblazeMaskEnable(&microblaze->SPR_EDR_wmask.reg, True, 0, 31);

    // SLR
    microblazeMaskEnable(&microblaze->SPR_SLR_rmask.reg, True, 0, 31);
    microblazeMaskEnable(&microblaze->SPR_SLR_wmask.reg, True, 0, 31);

    // SHR
    microblazeMaskEnable(&microblaze->SPR_SHR_rmask.reg, True, 0, 31);
    microblazeMaskEnable(&microblaze->SPR_SHR_wmask.reg, True, 0, 31);

    //
    // TLB Access to PID, ZPR, TLBLO, TLBHI registers is defined as follows
    //
    Bool tlbAccess1 =   (microblaze->params.C_USE_MMU > 1)      &&
                        (microblaze->params.C_MMU_TLB_ACCESS==1 ||
                         microblaze->params.C_MMU_TLB_ACCESS==3);

    // only available if C_USE_MMU > 1, PID, 0..23 is reserved
    if (tlbAccess1) {
        microblazeMaskEnable(&microblaze->SPR_PID_rmask.reg, True, 24, 31);
        microblazeMaskEnable(&microblaze->SPR_PID_wmask.reg, True, 24, 31);
    }

    // only available if C_USE_MMU > 1, ZPR
    // C_MMU_ZONES indicates number of bits available
    if (tlbAccess1 && microblaze->params.C_MMU_ZONES) {
        Uns32 r = (microblaze->params.C_MMU_ZONES * 2) - 1;
        //
        // Disabled ZPR bits are treated as '11'
        //
        microblazeMaskEnable(&microblaze->SPR_ZPR_rmask.reg, True, 0, r);
        microblazeMaskEnable(&microblaze->SPR_ZPR_wmask.reg, True, 0, r);
        microblaze->SPR_ZPR.reg = -1 & ~(microblaze->SPR_ZPR_wmask.reg);
    }

    // TLBLO
    // RPN [0:21] only defined when MMU == 3
    if (tlbAccess1) {
        microblazeMaskEnable(&microblaze->SPR_TLBLO_rmask.reg, True, 0, 31);
        microblazeMaskEnable(&microblaze->SPR_TLBLO_wmask.reg, True, 0, 31);

        //
        // TLBLO[RPN] only defined when C_USE_MMU == 3
        //
        if (microblaze->params.C_USE_MMU != 3) {
            microblazeMaskEnable(&microblaze->SPR_TLBLO_rmask.reg, False, 0, 21);
            microblazeMaskEnable(&microblaze->SPR_TLBLO_wmask.reg, False, 0, 21);
        }

        //
        // C_DCACHE_USE_WRITEBACK policy
        //
        if (microblaze->params.C_DCACHE_USE_WRITEBACK) {
            microblaze->SPR_TLBLO.bits.W = 1;
            microblazeMaskEnable(&microblaze->SPR_TLBLO_wmask.reg, False, 3, 3);
        }
        //
        // M bit read only
        //
        microblazeMaskEnable(&microblaze->SPR_TLBLO_wmask.reg, True, 30, 30);
    }

    // TLBHI
    if (tlbAccess1) {
        //
        // Field [27]    is user defined read only 0
        // Field [28:31] is reserved
        //
        microblazeMaskEnable(&microblaze->SPR_TLBHI_rmask.reg, True, 0, 26);
        microblazeMaskEnable(&microblaze->SPR_TLBHI_wmask.reg, True, 0, 26);
    }

    //
    // TLB Access to TLBX register is defined as follows
    //
    Bool tlbAccess2 =   (microblaze->params.C_USE_MMU > 1) &&
                        (microblaze->params.C_MMU_TLB_ACCESS > 0);

    // TLBX
    if (tlbAccess2) {
        //
        // MISS (bit 0)      read only, and only if C_MMU_TLB_ACCESS > 0
        // INDEX (bit 26-31) read/write only if C_MMU_TLB_ACCESS > 0
        // 1-25 is reserved
        //
        microblazeMaskEnable(&microblaze->SPR_TLBX_rmask.reg, True,  0, 31);
        microblazeMaskEnable(&microblaze->SPR_TLBX_rmask.reg, False, 1, 25);

        //
        // INDEX is writable
        //
        microblazeMaskEnable(&microblaze->SPR_TLBX_wmask.reg, True,  26, 31);
    }

    // TLBSX
    if (tlbAccess2) {
        //
        // VPN (bit 0-21) write only
        //
        microblazeMaskEnable(&microblaze->SPR_TLBSX_wmask.reg, True,  0, 21);
    }
}

void microblazeCheckExceptions(microblazeP microblaze) {

    if(isIntActive(microblaze)) {
        microblazeDoException(microblaze);
    }
}

