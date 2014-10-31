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

#include "vmi/vmiRt.h"

#include "microblazeStructure.h"
#include "microblazeExceptions.h"
#include "microblazeFunctions.h"
#include "microblazeUser.h"
#include "microblazeMorphFP.h"
#include "microblazeUtils.h"

//
// Flags -> PSW
//
void microblazeFillMSR(microblazeP microblaze) {
    microblaze->SPR_MSR.bits.CC = microblaze->FLG_CC;
    microblaze->SPR_MSR.bits.C  = microblaze->FLG_CC;
}

//
// PC
//
void microblazeFillPC(microblazeP microblaze) {
    microblaze->SPR_PC = (Uns32)vmirtGetPC((vmiProcessorP)microblaze);
}

//
// PSW -> Flags
//
void microblazeEmptyMSR(microblazeP microblaze) {
    microblaze->FLG_CC = microblaze->SPR_MSR.bits.C;
}

void microblazeUpdateBlockMask(microblazeP microblaze) {
    //microblazeEmptyMSR(microblaze);

    microblazeUpdateFPControlWord(microblaze);

    vmirtSetMode((vmiProcessorP)microblaze, microblazeGetVMMode(microblaze));

    Uns32 BM = vmirtGetBlockMask((vmiProcessorP)microblaze);

    //
    // update simulated processor state for Exception Enable
    //
    if (microblaze->SPR_MSR.bits.EE) {
        BM |= BM_MSR_EE;
    } else {
        BM &= ~BM_MSR_EE;
    }

    //
    // update simulated processor state for User Mode
    //
    if (microblaze->SPR_MSR.bits.UM) {
        BM |= BM_MSR_UM;
    } else {
        BM &= ~BM_MSR_UM;
    }

    vmirtSetBlockMask((vmiProcessorP)microblaze, BM);

    microblazeCheckExceptions(microblaze);
}

//
// VMI sticky flags -> SPR_FSR
//
void microblazeFillFSR(microblazeP microblaze) {

    // copy VMI format sticky flags to  the FSR
    microblaze->SPR_FSR.bits.IO = microblaze->FLG_FP.f.I;  // invalid operation flag
    microblaze->SPR_FSR.bits.DZ = microblaze->FLG_FP.f.Z;  // divide-by-zero flag
    microblaze->SPR_FSR.bits.OF = microblaze->FLG_FP.f.O;  // overflow flag
    microblaze->SPR_FSR.bits.UF = microblaze->FLG_FP.f.U;  // underflow flag

    // Needs special handling - not a VMI flag
    microblaze->SPR_FSR.bits.DO = microblaze->FLG_DO == 0 ? 0 : 1;  // Denormal Operand
}

//
// SPR_FSR ->  VMI sticky flags
//
void microblazeEmptyFSR(microblazeP microblaze) {

    // Copy SPR_FSR sticky flags to VMI sticky flags
    microblaze->FLG_FP.f.I = microblaze->SPR_FSR.bits.IO;   // invalid operation flag
    microblaze->FLG_FP.f.Z = microblaze->SPR_FSR.bits.DZ;   // divide-by-zero flag
    microblaze->FLG_FP.f.O = microblaze->SPR_FSR.bits.OF;   // overflow flag
    microblaze->FLG_FP.f.U = microblaze->SPR_FSR.bits.UF;   // underflow flag

    // Needs special handling - not a VMI flag
    microblaze->FLG_DO = microblaze->SPR_FSR.bits.DO;       // Denormal Operand
}

//
// Fill all registers that require it
//
void microblazeFillRegisters(microblazeP microblaze) {
    microblazeFillPC(microblaze);
    microblazeFillMSR(microblaze);
    microblazeFillFSR(microblaze);
}

//
// Empty all registers that require it
//
void microblazeEmptyRegisters(microblazeP microblaze) {
    microblazeEmptyMSR(microblaze);
    microblazeEmptyFSR(microblaze);
}

