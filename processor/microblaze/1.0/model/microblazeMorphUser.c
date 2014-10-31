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

#include "vmi/vmiMessage.h"
#include "vmi/vmiMt.h"
#include "vmi/vmiRt.h"
#include "microblazeInstructions.h"
#include "microblazeExceptions.h"
#include "microblazeMorph.h"
#include "microblazeUser.h"
#include "microblazeTLB.h"
#include "microblazeUtils.h"

//
// Write Flag Const structures
//
static const vmiFlags flags_Zr = {
    VMI_NOFLAG_CONST,
    {
        VMI_NOFLAG_CONST,
        VMI_NOFLAG_CONST,
        MICROBLAZE_FLG_ZR_CONST,
        VMI_NOFLAG_CONST,
        VMI_NOFLAG_CONST
    }
};
static const vmiFlags flags_Co = {
    VMI_NOFLAG_CONST,
    {
        MICROBLAZE_FLG_CC_CONST,
        VMI_NOFLAG_CONST,
        VMI_NOFLAG_CONST,
        VMI_NOFLAG_CONST,
        VMI_NOFLAG_CONST
    }
};
static const vmiFlags negflags_Co = {
    VMI_NOFLAG_CONST,
    {
        MICROBLAZE_FLG_CC_CONST,
        VMI_NOFLAG_CONST,
        VMI_NOFLAG_CONST,
        VMI_NOFLAG_CONST,
        VMI_NOFLAG_CONST
    },
    vmi_FN_CF_OUT
};
static const vmiFlags flags_Ci = {
    MICROBLAZE_FLG_CC_CONST,
    {
        VMI_NOFLAG_CONST,
        VMI_NOFLAG_CONST,
        VMI_NOFLAG_CONST,
        VMI_NOFLAG_CONST,
        VMI_NOFLAG_CONST
    }
};
static const vmiFlags negflags_Ci = {
    MICROBLAZE_FLG_CC_CONST,
    {
        VMI_NOFLAG_CONST,
        VMI_NOFLAG_CONST,
        VMI_NOFLAG_CONST,
        VMI_NOFLAG_CONST,
        VMI_NOFLAG_CONST
    },
    vmi_FN_CF_IN
};
static const vmiFlags flags_CiCo = {
    MICROBLAZE_FLG_CC_CONST,
    {
        MICROBLAZE_FLG_CC_CONST,
        VMI_NOFLAG_CONST,
        VMI_NOFLAG_CONST,
        VMI_NOFLAG_CONST,
        VMI_NOFLAG_CONST
    }
};
static const vmiFlags negflags_CiCo = {
    MICROBLAZE_FLG_CC_CONST,
    {
        MICROBLAZE_FLG_CC_CONST,
        VMI_NOFLAG_CONST,
        VMI_NOFLAG_CONST,
        VMI_NOFLAG_CONST,
        VMI_NOFLAG_CONST
    },
    vmi_FN_CF_IN | vmi_FN_CF_OUT
};

#define BINOPRRR(_OP,_FLAGS) \
    vmimtBinopRRR(MICROBLAZE_GPR_BITS, _OP, gpr_rD, gpr_rA, gpr_rB, _FLAGS)

#define BINOPRRR_REVERSE(_OP,_FLAGS) \
    vmimtBinopRRR(MICROBLAZE_GPR_BITS, _OP, gpr_rD, gpr_rB, gpr_rA, _FLAGS)

#define BINOPRR(_OP,_FLAGS) \
    vmimtBinopRR(MICROBLAZE_GPR_BITS, _OP, gpr_rD, gpr_rA, _FLAGS)

#define BINOPRRC(_OP,_IMM,_FLAGS) \
    vmimtBinopRRC(MICROBLAZE_GPR_BITS, _OP, gpr_rD, gpr_rA, _IMM, _FLAGS)

#define BINOPRC(_OP,_IMM,_FLAGS) \
    vmimtBinopRC(MICROBLAZE_GPR_BITS, _OP, gpr_rD, _IMM, _FLAGS)

#define UNOPRR(_OP,_FLAGS) \
    vmimtUnopRR(MICROBLAZE_GPR_BITS, _OP, gpr_rD, gpr_rA, _FLAGS);

#define MFSREG(GPR, SREG) \
    microblaze->gpr[GPR] = microblaze->SPR_ ## SREG .reg & microblaze->SPR_ ## SREG ## _rmask.reg;

#define MTSREG(SREG, GPR) \
    mask    = microblaze->SPR_ ## SREG ## _wmask.reg; \
    prevREG = microblaze->SPR_ ## SREG .reg & ~mask; \
    nextREG = microblaze->gpr[GPR]          &  mask; \
    microblaze->SPR_ ## SREG .reg = prevREG | nextREG;

static Uns32 getImm32 (microblazeMorphStateP state) {
    Uns32 imm32;
    //
    // If the upper immediate is 0x80000000, then the lower immediate is extended
    //
    if (state->info.simmhi == 0x0000ffff) {
        imm32 = state->info.simm;
    } else {
        imm32 = state->info.simmhi | (state->info.simm & 0xffff);
    }
    return imm32;
}

//static Uns32 bitlit(const char *literal) {
//    Uns32 value = 0;
//
//    const char *p = literal;
//    while (*p) {
//       if (*p == '0') {
//           value = (value << 1) | 0x0;
//       } else if (*p == '1') {
//           value = (value << 1) | 0x1;
//       } else {
//       }
//       p++;
//    }
//    return value;
//}

static void saveArgsIfExceptionsEnabled(microblazeMorphStateP state, Uns8 rD, vmiReg rDreg, vmiReg rAreg, vmiReg rBreg) {
    microblazeP microblaze = state->microblaze;

    vmimtValidateBlockMask(BM_MSR_EE);

    //
    // Arg index rD is always required whether exceptions are enabled or not
    //
    vmimtMoveRC(MICROBLAZE_TMP_BITS, MICROBLAZE_TMP_WR(0), rD);
    if(microblaze->SPR_MSR.bits.EE) {
        vmimtMoveRR(MICROBLAZE_TMP_BITS, MICROBLAZE_TMP_WR(1), rDreg);
        vmimtMoveRR(MICROBLAZE_TMP_BITS, MICROBLAZE_TMP_WR(2), rAreg);
        vmimtMoveRR(MICROBLAZE_TMP_BITS, MICROBLAZE_TMP_WR(3), rBreg);
    }
}

static memEndian getEndian(microblazeMorphStateP state) {
    memEndian endian;
    endian = (state->microblaze->SPR_PVR0.bits.ENDI == 0) ? MEM_ENDIAN_BIG : MEM_ENDIAN_LITTLE;
    return endian;
}

static memEndian invertEndian(memEndian endian) {
    if (endian == MEM_ENDIAN_BIG) {
        return MEM_ENDIAN_LITTLE;
    } else {
        return MEM_ENDIAN_BIG;
    }
}

static void branchOnCondReg (vmiCondition cond, vmiReg condReg, vmiReg addReg, Uns32 thisPC, Bool delaySlot) {
    vmimtCompareRC(MICROBLAZE_GPR_BITS, cond, condReg, 0, MICROBLAZE_TMP_WR(0));
    vmimtBinopRRC(MICROBLAZE_TMP_BITS, vmi_ADD, MICROBLAZE_TMP_WR(1), addReg, thisPC, 0);

    if (delaySlot) {
        vmiLabelP label1 = vmimtNewLabel();
        vmimtTestRCJumpLabel(MICROBLAZE_GPR_BITS, vmi_COND_NZ, MICROBLAZE_SPR_MSR_RD, SPR_MSR_EIP_MASK, label1);
        vmimtMoveRR(MICROBLAZE_GPR_BITS, MICROBLAZE_SPR_BTR_WR, MICROBLAZE_TMP_RD(1));
        vmimtInsertLabel(label1);

        vmimtCondJumpRegDelaySlot(1, MICROBLAZE_TMP_RD(0), 1, 0, MICROBLAZE_TMP_RD(1), VMI_NOREG, vmi_JH_RELATIVE, 0);
    } else {
        vmimtCondJumpReg(MICROBLAZE_TMP_RD(0), 1, 0, MICROBLAZE_TMP_RD(1), VMI_NOREG, vmi_JH_RELATIVE);
    }
}

static void branchOnCondImm (vmiCondition cond, vmiReg condReg, Uns32 branchPC, Bool delaySlot) {

    vmimtCompareRC(MICROBLAZE_GPR_BITS, cond, condReg, 0, MICROBLAZE_TMP_WR(0));

    if (delaySlot) {
        vmiLabelP label1 = vmimtNewLabel();
        vmimtTestRCJumpLabel(MICROBLAZE_GPR_BITS, vmi_COND_NZ, MICROBLAZE_SPR_MSR_RD, SPR_MSR_EIP_MASK, label1);
        vmimtMoveRC(MICROBLAZE_GPR_BITS, MICROBLAZE_SPR_BTR_WR, branchPC);
        vmimtInsertLabel(label1);

        vmimtCondJumpDelaySlot(1, MICROBLAZE_TMP_RD(0), 1, 0, branchPC, VMI_NOREG, vmi_JH_RELATIVE, 0);
    } else {
        vmimtCondJump(MICROBLAZE_TMP_RD(0), 1, 0, branchPC, VMI_NOREG, vmi_JH_RELATIVE);
    }
}

static void branchReg (vmiReg addReg, Bool delaySlot, vmiJumpHint hint) {
    if (delaySlot) {
        vmiLabelP label1 = vmimtNewLabel();
        vmimtTestRCJumpLabel(MICROBLAZE_GPR_BITS, vmi_COND_NZ, MICROBLAZE_SPR_MSR_RD, SPR_MSR_EIP_MASK, label1);
        vmimtMoveRR(MICROBLAZE_GPR_BITS, MICROBLAZE_SPR_BTR_WR, addReg);
        vmimtInsertLabel(label1);

        vmimtUncondJumpRegDelaySlot(1, 0, addReg, VMI_NOREG, hint, 0);
    } else {
        vmimtUncondJumpReg(0, addReg, VMI_NOREG, hint);
    }
}

static void branchImm (Uns32 branchPC, Bool delaySlot, vmiJumpHint hint, vmiPostSlotFn slotCB) {
    if (delaySlot) {
        vmiLabelP label1 = vmimtNewLabel();
        vmimtTestRCJumpLabel(MICROBLAZE_GPR_BITS, vmi_COND_NZ, MICROBLAZE_SPR_MSR_RD, SPR_MSR_EIP_MASK, label1);
        vmimtMoveRC(MICROBLAZE_GPR_BITS, MICROBLAZE_SPR_BTR_WR, branchPC);
        vmimtInsertLabel(label1);

        vmimtUncondJumpDelaySlot(1, 0, branchPC, VMI_NOREG, hint, slotCB);
    } else {
        vmimtUncondJump(0, branchPC, VMI_NOREG, hint);
    }
}

static void setReturnPC(vmiReg gpr_rD, microblazeMorphStateP state) {
    vmimtMoveRSimPC(MICROBLAZE_GPR_BITS, gpr_rD);
    //
    // Adjust return address if instruction preceded by an imm instruction
    //
    vmimtBinopRC(MICROBLAZE_GPR_BITS, vmi_ADD, gpr_rD, (state->info.instrsize - 4), 0);
}

//
// Called on some action on a connection on which this processor is waiting
//
static VMI_CONN_UPDATE_FN(fslRestartAfterBlock) {
    vmirtRestartNow(processor);
}

//
// Exception run time callback
//
static void emit_hwExceptionConst (TYPE_SPR_ESR ESR) {
    vmimtArgProcessor();
    vmimtArgUns32(ESR.reg);
    vmimtCall((vmiCallFn)microblazehwExceptionCB);
}

static Bool validatePrivMode(microblazeP microblaze) {
    vmimtValidateBlockMask(BM_MSR_UM);
    if (microblaze->SPR_MSR.bits.UM) {
        TYPE_SPR_ESR ESR = {0};
        ESR.bits.EC  = PRIVILEGED_INSTRUCTION_EXCEPTION;
        ESR.bits.ESS = 0;
        emit_hwExceptionConst(ESR);
        return False;
    }
    return True;
}

static Bool validateFSLPrivMode(microblazeP microblaze) {
    vmimtValidateBlockMask(BM_MSR_UM);
    if (microblaze->SPR_MSR.bits.UM && microblazeMMUPrivileged(microblaze)) {
        TYPE_SPR_ESR ESR = {0};
        ESR.bits.EC  = PRIVILEGED_INSTRUCTION_EXCEPTION;
        ESR.bits.ESS = 0;
        emit_hwExceptionConst(ESR);
        return False;
    }
    return True;
}

//
// Write Default Morpher stub Functions
//

MICROBLAZE_MORPH_FN(morphNOP_TYPEX) {
    //
    // Do Nothing
    //
}

MICROBLAZE_MORPH_FN(morphADD_TYPEA1) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rB = state->info.rB;
    vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);

    if (rD == 0 && rA == 0 && rB == 0) {
        //
        // Illegal Op, if PVR2.OP0EXC, otherwise a nop
        //
        if (state->microblaze->SPR_PVR2.bits.OP0EXC) {
            TYPE_SPR_ESR ESR = {0};
            ESR.bits.EC = ILLEGAL_OPCODE_EXCEPTION;
            ESR.bits_ESS_EC_00001.Rx = state->info.rD;
            emit_hwExceptionConst(ESR);
        }
    } else {
        BINOPRRR(vmi_ADD, &flags_Co);
    }
}

MICROBLAZE_MORPH_FN(morphADDC_TYPEA1) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rB = state->info.rB;
    vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);

    BINOPRRR(vmi_ADC, &flags_CiCo);
}

MICROBLAZE_MORPH_FN(morphADDK_TYPEA1) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rB = state->info.rB;
    vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);

    BINOPRRR(vmi_ADD, 0);
}

MICROBLAZE_MORPH_FN(morphADDKC_TYPEA1) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rB = state->info.rB;
    vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);

    BINOPRRR(vmi_ADC, &flags_Ci);
}

MICROBLAZE_MORPH_FN(morphAND_TYPEA1) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rB = state->info.rB;
    vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);
    BINOPRRR(vmi_AND, 0);
}

MICROBLAZE_MORPH_FN(morphANDN_TYPEA1) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rB = state->info.rB;
    vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);
    BINOPRRR(vmi_ANDN, 0);
}

MICROBLAZE_MORPH_FN(morphLWX_TYPEA1) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rB = state->info.rB;
    vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);

    //
    // Store rD for exception
    //
    vmimtMoveRC(MICROBLAZE_GPR_BITS, MICROBLAZE_TMP_WR(0), rD);
    vmimtBinopRRR(MICROBLAZE_TMP_BITS, vmi_ADD, MICROBLAZE_TMP_WR(1), gpr_rA, gpr_rB, 0);
    vmimtLoadRRO(MICROBLAZE_GPR_BITS, MICROBLAZE_GPR_BITS, 0, gpr_rD, MICROBLAZE_TMP_RD(1), getEndian(state), False, True);

    //
    // Set the reservation bit
    //
    vmimtMoveRC(MICROBLAZE_FLG_RESERVATION_BITS, MICROBLAZE_FLG_RESERVATION_WR, 1);

    //
    // Clear MSR[C] - this needs to be conditional on an AXI4 interface being present
    //
    vmimtMoveRC(MICROBLAZE_FLG_CC_BITS, MICROBLAZE_FLG_CC_WR, 0);

}

MICROBLAZE_MORPH_FN(morphMUL_TYPEA1) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rB = state->info.rB;
    vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);

    BINOPRRR(vmi_MUL, 0);
}

MICROBLAZE_MORPH_FN(morphMULH_TYPEA1) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rB = state->info.rB;
    vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);

    vmimtMulopRRR(MICROBLAZE_GPR_BITS, vmi_IMUL, gpr_rD, VMI_NOREG, gpr_rA, gpr_rB, 0);
}

static void vmic_MULHSU_TYPEA1(microblazeP microblaze, Uns32 rD, Int32 rA, Uns32 rB) {
    Int64 result64;
    Uns32 result32;
    if (rD) {
        result64 = (Int64)((Int64)rA * (Uns64)rB);
        result64 = result64 >> 32;
        result32 = (Uns32) (result64 & 0xffffffff);
        microblaze->gpr[rD] = result32;
    }
}

MICROBLAZE_MORPH_FN(morphMULHSU_TYPEA1) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rB = state->info.rB;
    vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    Uns8 rD = state->info.rD;

    vmimtArgProcessor();
    vmimtArgUns32(rD);
    vmimtArgReg(MICROBLAZE_GPR_BITS, gpr_rA);
    vmimtArgReg(MICROBLAZE_GPR_BITS, gpr_rB);
    vmimtCall((vmiCallFn)vmic_MULHSU_TYPEA1);
}

MICROBLAZE_MORPH_FN(morphMULHU_TYPEA1) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rB = state->info.rB;
    vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);

    vmimtMulopRRR(MICROBLAZE_GPR_BITS, vmi_MUL, gpr_rD, VMI_NOREG, gpr_rA, gpr_rB, 0);
}

MICROBLAZE_MORPH_FN(morphOR_TYPEA1) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rB = state->info.rB;
    vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_RD(rD);

    BINOPRRR(vmi_OR, 0);
}

MICROBLAZE_MORPH_FN(morphPCMPBF_TYPEA1) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rB = state->info.rB;
    vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);

    vmiLabelP L1 = vmimtNewLabel();

    // compare rA/rB [byte 3]
    vmimtMoveRC(MICROBLAZE_TMP_BITS, MICROBLAZE_TMP_WR(0), 1);
    vmimtCompareRR(8, vmi_COND_EQ, VMI_REG_DELTA(gpr_rA, 3), VMI_REG_DELTA(gpr_rB, 3), MICROBLAZE_FLG_TMP_WR);
    vmimtCondJumpLabel(MICROBLAZE_FLG_TMP_RD, True, L1);

    // compare rA/rB [byte 2]
    vmimtMoveRC(MICROBLAZE_TMP_BITS, MICROBLAZE_TMP_WR(0), 2);
    vmimtCompareRR(8, vmi_COND_EQ, VMI_REG_DELTA(gpr_rA, 2), VMI_REG_DELTA(gpr_rB, 2), MICROBLAZE_FLG_TMP_WR);
    vmimtCondJumpLabel(MICROBLAZE_FLG_TMP_RD, True, L1);

    // compare rA/rB [byte 1]
    vmimtMoveRC(MICROBLAZE_TMP_BITS, MICROBLAZE_TMP_WR(0), 3);
    vmimtCompareRR(8, vmi_COND_EQ, VMI_REG_DELTA(gpr_rA, 1), VMI_REG_DELTA(gpr_rB, 1), MICROBLAZE_FLG_TMP_WR);
    vmimtCondJumpLabel(MICROBLAZE_FLG_TMP_RD, True, L1);

    // compare rA/rB [byte 0]
    vmimtMoveRC(MICROBLAZE_TMP_BITS, MICROBLAZE_TMP_WR(0), 4);
    vmimtCompareRR(8, vmi_COND_EQ, VMI_REG_DELTA(gpr_rA, 0), VMI_REG_DELTA(gpr_rB, 0), MICROBLAZE_FLG_TMP_WR);
    vmimtCondJumpLabel(MICROBLAZE_FLG_TMP_RD, True, L1);

    // else
    vmimtMoveRC(MICROBLAZE_TMP_BITS, MICROBLAZE_TMP_WR(0), 0);

    vmimtInsertLabel(L1);
    vmimtMoveRR(MICROBLAZE_GPR_BITS, gpr_rD, MICROBLAZE_TMP_RD(0));

}

MICROBLAZE_MORPH_FN(morphPCMPEQ_TYPEA1) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rB = state->info.rB;
    vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);

    vmimtCompareRR(MICROBLAZE_GPR_BITS, vmi_COND_EQ, gpr_rA, gpr_rB, MICROBLAZE_FLG_TMP_WR);
    vmimtMoveExtendRR(MICROBLAZE_GPR_BITS, gpr_rD, 8, MICROBLAZE_FLG_TMP_RD, False);
}

MICROBLAZE_MORPH_FN(morphPCMPNE_TYPEA1) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rB = state->info.rB;
    vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);

    vmimtCompareRR(MICROBLAZE_GPR_BITS, vmi_COND_NE, gpr_rA, gpr_rB, MICROBLAZE_FLG_TMP_WR);
    vmimtMoveExtendRR(MICROBLAZE_GPR_BITS, gpr_rD, 8, MICROBLAZE_FLG_TMP_RD, False);
}

MICROBLAZE_MORPH_FN(morphRSUB_TYPEA1) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rB = state->info.rB;
    vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);

    BINOPRRR(vmi_RSUB, &negflags_Co);
}

MICROBLAZE_MORPH_FN(morphRSUBC_TYPEA1) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rB = state->info.rB;
    vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);

    BINOPRRR(vmi_RSBB, &negflags_CiCo);
}

MICROBLAZE_MORPH_FN(morphRSUBK_TYPEA1) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rB = state->info.rB;
    vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);

    BINOPRRR(vmi_RSUB, 0);
}

MICROBLAZE_MORPH_FN(morphRSUBKC_TYPEA1) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rB = state->info.rB;
    vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);

    BINOPRRR(vmi_RSBB, &negflags_Ci);
}

MICROBLAZE_MORPH_FN(morphSWX_TYPEA1) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rB = state->info.rB;
    vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);

    vmiLabelP FAIL = vmimtNewLabel();
    vmiLabelP END  = vmimtNewLabel();

    //
    // Test the state of the reservation bit, before attempting the store
    //
    // if reservation == 0
    //    MSR[C] = 1
    // else
    //    MEM[ADDR] = R
    //    MSR[C] = 0
    // endif
    vmimtCompareRC(MICROBLAZE_FLG_TMP_BITS, vmi_COND_EQ, MICROBLAZE_FLG_RESERVATION_RD, 0, MICROBLAZE_FLG_TMP_WR);
    vmimtCondJumpLabel(MICROBLAZE_FLG_TMP_RD, True, FAIL);

    //
    // reservation == 1; MEM[ADDR] = R; MSR[C] = 0
    //
    //
    // Store rD for exception
    //
    vmimtMoveRC(MICROBLAZE_GPR_BITS, MICROBLAZE_TMP_WR(0), rD);
    vmimtBinopRRR(MICROBLAZE_TMP_BITS, vmi_ADD, MICROBLAZE_TMP_WR(1), gpr_rA, gpr_rB, 0);
    vmimtStoreRRO(MICROBLAZE_GPR_BITS, 0, MICROBLAZE_TMP_RD(1), gpr_rD, getEndian(state), True);
    //
    // Clear MSR[C] - this needs to be conditional on an AXI4 interface being present
    //
    vmimtMoveRC(MICROBLAZE_FLG_CC_BITS, MICROBLAZE_FLG_CC_WR, 0);
    vmimtUncondJumpLabel(END);

    //
    // reservation == 0; FLG_CC <= 1
    //
    vmimtInsertLabel(FAIL);
    vmimtMoveRC(MICROBLAZE_FLG_CC_BITS, MICROBLAZE_FLG_CC_WR, 1);

    vmimtInsertLabel(END);

}

MICROBLAZE_MORPH_FN(morphXOR_TYPEA1) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rB = state->info.rB;
    vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_RD(rD);

    BINOPRRR(vmi_XOR, 0);
}

MICROBLAZE_MORPH_FN(morphBEQ_TYPEA2) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rB = state->info.rB;
    vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);

    branchOnCondReg(vmi_COND_EQ, gpr_rA, gpr_rB, state->info.thisPC, False);
}

MICROBLAZE_MORPH_FN(morphBEQD_TYPEA2) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rB = state->info.rB;
    vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);

    branchOnCondReg(vmi_COND_EQ, gpr_rA, gpr_rB, state->info.thisPC, True);
}

MICROBLAZE_MORPH_FN(morphBGE_TYPEA2) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rB = state->info.rB;
    vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);

    branchOnCondReg(vmi_COND_NS, gpr_rA, gpr_rB, state->info.thisPC, False);
}

MICROBLAZE_MORPH_FN(morphBGED_TYPEA2) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rB = state->info.rB;
    vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);

    branchOnCondReg(vmi_COND_NS, gpr_rA, gpr_rB, state->info.thisPC, True);
}

MICROBLAZE_MORPH_FN(morphBGT_TYPEA2) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rB = state->info.rB;
    vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);

    branchOnCondReg(vmi_COND_NLE, gpr_rA, gpr_rB, state->info.thisPC, False);
}

MICROBLAZE_MORPH_FN(morphBGTD_TYPEA2) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rB = state->info.rB;
    vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);

    branchOnCondReg(vmi_COND_NLE, gpr_rA, gpr_rB, state->info.thisPC, True);
}

MICROBLAZE_MORPH_FN(morphBLE_TYPEA2) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rB = state->info.rB;
    vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);

    branchOnCondReg(vmi_COND_LE, gpr_rA, gpr_rB, state->info.thisPC, False);
}

MICROBLAZE_MORPH_FN(morphBLED_TYPEA2) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rB = state->info.rB;
    vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);

    branchOnCondReg(vmi_COND_LE, gpr_rA, gpr_rB, state->info.thisPC, True);
}

MICROBLAZE_MORPH_FN(morphBLT_TYPEA2) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rB = state->info.rB;
    vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);

    branchOnCondReg(vmi_COND_L, gpr_rA, gpr_rB, state->info.thisPC, False);
}

MICROBLAZE_MORPH_FN(morphBLTD_TYPEA2) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rB = state->info.rB;
    vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);

    branchOnCondReg(vmi_COND_L, gpr_rA, gpr_rB, state->info.thisPC, True);
}

MICROBLAZE_MORPH_FN(morphBNE_TYPEA2) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rB = state->info.rB;
    vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);

    branchOnCondReg(vmi_COND_NZ, gpr_rA, gpr_rB, state->info.thisPC, False);
}

MICROBLAZE_MORPH_FN(morphBNED_TYPEA2) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rB = state->info.rB;
    vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);

    branchOnCondReg(vmi_COND_NZ, gpr_rA, gpr_rB, state->info.thisPC, True);
}

MICROBLAZE_MORPH_FN(morphBR_TYPEA3) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rB = state->info.rB;
    vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);

    vmimtBinopRRC(MICROBLAZE_GPR_BITS, vmi_ADD, MICROBLAZE_TMP_WR(0), gpr_rB, state->info.thisPC, 0);
    branchReg(MICROBLAZE_TMP_RD(0), False, vmi_JH_RELATIVE);
}

MICROBLAZE_MORPH_FN(morphBRA_TYPEA3) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rB = state->info.rB;
    vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);

    branchReg(gpr_rB, False, vmi_JH_NONE);
}

MICROBLAZE_MORPH_FN(morphBRAD_TYPEA3) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rB = state->info.rB;
    vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);

    branchReg(gpr_rB, True, vmi_JH_NONE);
}

MICROBLAZE_MORPH_FN(morphBRD_TYPEA3) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rB = state->info.rB;
    vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);

    vmimtBinopRRC(MICROBLAZE_GPR_BITS, vmi_ADD, MICROBLAZE_TMP_WR(0), gpr_rB, state->info.thisPC, 0);
    branchReg(MICROBLAZE_TMP_RD(0), True, vmi_JH_RELATIVE);
}

MICROBLAZE_MORPH_FN(morphBRALD_TYPEA4) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rB = state->info.rB;
    vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);

    setReturnPC(gpr_rD, state);
    branchReg(gpr_rB, True, vmi_JH_CALL);

}

MICROBLAZE_MORPH_FN(morphBRLD_TYPEA4) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rB = state->info.rB;
    vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);

    vmimtBinopRRC(MICROBLAZE_GPR_BITS, vmi_ADD, MICROBLAZE_TMP_WR(0), gpr_rB, state->info.thisPC, 0);
    setReturnPC(gpr_rD, state);
    branchReg(MICROBLAZE_TMP_RD(0), True, (vmi_JH_CALL | vmi_JH_RELATIVE));
}

// Need to handle user mode case
static void vmic_BRK_TYPEA5(microblazeP microblaze) {
    microblaze->SPR_MSR.bits.BIP = 1;
    microblaze->FLG_reservation = 0;
}
MICROBLAZE_MORPH_FN(morphBRK_TYPEA5) {
    if (validatePrivMode(state->microblaze)) {
        Uns8 rB = state->info.rB;
        vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
        Uns8 rD = state->info.rD;
        vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);

        setReturnPC(gpr_rD, state);
        vmimtUncondJumpReg(0, gpr_rB, VMI_NOREG, vmi_JH_CALL);

        vmimtArgProcessor();
        vmimtCall((vmiCallFn)vmic_BRK_TYPEA5);
    }
}

MICROBLAZE_MORPH_FN(morphBSLL_TYPEA6) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rB = state->info.rB;
    vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);

    BINOPRRR(vmi_SHL, 0);
}

MICROBLAZE_MORPH_FN(morphBSRA_TYPEA6) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rB = state->info.rB;
    vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);

    BINOPRRR(vmi_SAR, 0);
}

MICROBLAZE_MORPH_FN(morphBSRL_TYPEA6) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rB = state->info.rB;
    vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);

    BINOPRRR(vmi_SHR, 0);
}

MICROBLAZE_MORPH_FN(morphCMP_TYPEA7) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rB = state->info.rB;
    vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);

    //
    // Test that rB < rA (signed)
    //
    vmimtCompareRR(MICROBLAZE_GPR_BITS, vmi_COND_L, gpr_rB, gpr_rA, MICROBLAZE_FLG_TMP_WR);

    BINOPRRR(vmi_RSUB, 0); // signed

    // If the flag is 1, set the MSB, if it is 0 clear the MSB
    BINOPRC(vmi_AND, 0x7fffffff, 0); // clear the MSB

    vmimtMoveExtendRR(MICROBLAZE_TMP_BITS, MICROBLAZE_TMP_WR(0), 8, MICROBLAZE_FLG_TMP_RD, False);
    vmimtBinopRC(MICROBLAZE_TMP_BITS, vmi_SHL, MICROBLAZE_TMP_WR(0), 31, 0);
    vmimtBinopRR(MICROBLAZE_GPR_BITS, vmi_OR, gpr_rD, MICROBLAZE_TMP_RD(0), 0);
}

MICROBLAZE_MORPH_FN(morphCMPU_TYPEA7) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rB = state->info.rB;
    vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);

    //
    // Test that rB < rA (Unsigned)
    //
    vmimtCompareRR(MICROBLAZE_GPR_BITS, vmi_COND_B, gpr_rB, gpr_rA, MICROBLAZE_FLG_TMP_WR);

    BINOPRRR(vmi_RSUB, 0); // signed

    // If the flag is 1, set the MSB, if it is 0 clear the MSB
    BINOPRC(vmi_AND, 0x7fffffff, 0); // clear the MSB

    vmimtMoveExtendRR(MICROBLAZE_TMP_BITS, MICROBLAZE_TMP_WR(0), 8, MICROBLAZE_FLG_TMP_RD, False);
    vmimtBinopRC(MICROBLAZE_TMP_BITS, vmi_SHL, MICROBLAZE_TMP_WR(0), 31, 0);
    vmimtBinopRR(MICROBLAZE_GPR_BITS, vmi_OR, gpr_rD, MICROBLAZE_TMP_RD(0), 0);
}

// IDIV pseudo code
//if (rA) = 0 then
//  (rD)     <- 0
//  MSR[DZO] <- 1 (divide Zero | Overflow)
//  ESR[EC] <- 00101 (divide exception)
//  ESR[DEC] <- 0 (Divide exception Cause / Divide by Zero)
//else if (signed - IDIV) and (rA) = -1 and (rB) = -2147483648 then
//  (rD) <- -2147483648
//  MSR[DZO] <- 1 (divide Zero | Overflow)
//  ESR[EC] <- 00101 (divide exception)
//  ESR[DEC] <- 1 (Divide exception Cause / Division overflow)
//else
//  (rD) = (rB) / (rA)
//
MICROBLAZE_MORPH_FN(morphIDIV_TYPEA7) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rB = state->info.rB;
    vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);

    saveArgsIfExceptionsEnabled(state, rD, gpr_rD, gpr_rA, gpr_rB);
    BINOPRRR_REVERSE(vmi_IDIV, 0);
}

MICROBLAZE_MORPH_FN(morphIDIVU_TYPEA7) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rB = state->info.rB;
    vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);

    saveArgsIfExceptionsEnabled(state, rD, gpr_rD, gpr_rA, gpr_rB);
    BINOPRRR_REVERSE(vmi_DIV, 0);
}

MICROBLAZE_MORPH_FN(morphLBU_TYPEA8) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rB = state->info.rB;
    vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);

    //
    // Store rD for exception
    //
    vmimtMoveRC(MICROBLAZE_GPR_BITS, MICROBLAZE_TMP_WR(0), rD);
    vmimtBinopRRR(MICROBLAZE_TMP_BITS, vmi_ADD, MICROBLAZE_TMP_WR(1), gpr_rA, gpr_rB, 0);
    vmimtLoadRRO(MICROBLAZE_GPR_BITS, MICROBLAZE_GPR_BITS/4, 0, gpr_rD, MICROBLAZE_TMP_RD(1), getEndian(state), False, True);
}

MICROBLAZE_MORPH_FN(morphLBUR_TYPEA8) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    // Uns8 rA = state->info.rA;
    // vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    // vmiReg gpr_rA = MICROBLAZE_GPR_WR(rA);
    // Uns8 rB = state->info.rB;
    // vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    // vmiReg gpr_rB = MICROBLAZE_GPR_WR(rB);
    // Uns8 rD = state->info.rD;
    // vmiReg gpr_rD = MICROBLAZE_GPR_RD(rD);
    // vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);
    vmiMessage("F", "MORPH_UNIMP", "Instruction LBUR_TYPEA8 unimplemented");
}

MICROBLAZE_MORPH_FN(morphLHU_TYPEA8) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rB = state->info.rB;
    vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);

    //
    // Store rD for exception
    //
    vmimtMoveRC(MICROBLAZE_GPR_BITS, MICROBLAZE_TMP_WR(0), rD);
    vmimtBinopRRR(MICROBLAZE_TMP_BITS, vmi_ADD, MICROBLAZE_TMP_WR(1), gpr_rA, gpr_rB, 0);
    vmimtLoadRRO(MICROBLAZE_GPR_BITS, MICROBLAZE_GPR_BITS/2, 0, gpr_rD, MICROBLAZE_TMP_RD(1), getEndian(state), False, True);
}

MICROBLAZE_MORPH_FN(morphLHUR_TYPEA8) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    // Uns8 rA = state->info.rA;
    // vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    // vmiReg gpr_rA = MICROBLAZE_GPR_WR(rA);
    // Uns8 rB = state->info.rB;
    // vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    // vmiReg gpr_rB = MICROBLAZE_GPR_WR(rB);
    // Uns8 rD = state->info.rD;
    // vmiReg gpr_rD = MICROBLAZE_GPR_RD(rD);
    // vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);
    vmiMessage("F", "MORPH_UNIMP", "Instruction LHUR_TYPEA8 unimplemented");
}

MICROBLAZE_MORPH_FN(morphLW_TYPEA8) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rB = state->info.rB;
    vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);

    //
    // Store rD for exception
    //
    vmimtMoveRC(MICROBLAZE_GPR_BITS, MICROBLAZE_TMP_WR(0), rD);
    vmimtBinopRRR(MICROBLAZE_TMP_BITS, vmi_ADD, MICROBLAZE_TMP_WR(1), gpr_rA, gpr_rB, 0);
    vmimtLoadRRO(MICROBLAZE_GPR_BITS, MICROBLAZE_GPR_BITS, 0, gpr_rD, MICROBLAZE_TMP_RD(1), getEndian(state), False, True);
}

MICROBLAZE_MORPH_FN(morphLWR_TYPEA8) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rB = state->info.rB;
    vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);

    //
    // Store rD for exception
    //
    vmimtMoveRC(MICROBLAZE_GPR_BITS, MICROBLAZE_TMP_WR(0), rD);
    vmimtBinopRRR(MICROBLAZE_TMP_BITS, vmi_ADD, MICROBLAZE_TMP_WR(1), gpr_rA, gpr_rB, 0);
    vmimtLoadRRO(MICROBLAZE_GPR_BITS, MICROBLAZE_GPR_BITS, 0, gpr_rD, MICROBLAZE_TMP_RD(1), invertEndian(getEndian(state)), False, True);
}

MICROBLAZE_MORPH_FN(morphSB_TYPEA8) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rB = state->info.rB;
    vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);

    //
    // Store rD for exception
    //
    vmimtMoveRC(MICROBLAZE_GPR_BITS, MICROBLAZE_TMP_WR(0), rD);
    vmimtBinopRRR(MICROBLAZE_TMP_BITS, vmi_ADD, MICROBLAZE_TMP_WR(1), gpr_rA, gpr_rB, 0);
    vmimtStoreRRO(MICROBLAZE_GPR_BITS/4, 0, MICROBLAZE_TMP_RD(1), gpr_rD, getEndian(state), True);
}

MICROBLAZE_MORPH_FN(morphSBR_TYPEA8) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    // Uns8 rA = state->info.rA;
    // vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    // vmiReg gpr_rA = MICROBLAZE_GPR_WR(rA);
    // Uns8 rB = state->info.rB;
    // vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    // vmiReg gpr_rB = MICROBLAZE_GPR_WR(rB);
    // Uns8 rD = state->info.rD;
    // vmiReg gpr_rD = MICROBLAZE_GPR_RD(rD);
    // vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);
    vmiMessage("F", "MORPH_UNIMP", "Instruction SBR_TYPEA8 unimplemented");
}

MICROBLAZE_MORPH_FN(morphSH_TYPEA8) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rB = state->info.rB;
    vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);

    //
    // Store rD for exception
    //
    vmimtMoveRC(MICROBLAZE_GPR_BITS, MICROBLAZE_TMP_WR(0), rD);
    vmimtBinopRRR(MICROBLAZE_TMP_BITS, vmi_ADD, MICROBLAZE_TMP_WR(1), gpr_rA, gpr_rB, 0);
    vmimtStoreRRO(MICROBLAZE_GPR_BITS/2, 0, MICROBLAZE_TMP_RD(1), gpr_rD, getEndian(state), True);
}

MICROBLAZE_MORPH_FN(morphSHR_TYPEA8) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    // Uns8 rA = state->info.rA;
    // vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    // vmiReg gpr_rA = MICROBLAZE_GPR_WR(rA);
    // Uns8 rB = state->info.rB;
    // vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    // vmiReg gpr_rB = MICROBLAZE_GPR_WR(rB);
    // Uns8 rD = state->info.rD;
    // vmiReg gpr_rD = MICROBLAZE_GPR_RD(rD);
    // vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);
    vmiMessage("F", "MORPH_UNIMP", "Instruction SHR_TYPEA8 unimplemented");
}

MICROBLAZE_MORPH_FN(morphSW_TYPEA8) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rB = state->info.rB;
    vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);

    //
    // Store rD for exception
    //
    vmimtMoveRC(MICROBLAZE_GPR_BITS, MICROBLAZE_TMP_WR(0), rD);
    vmimtBinopRRR(MICROBLAZE_TMP_BITS, vmi_ADD, MICROBLAZE_TMP_WR(1), gpr_rA, gpr_rB, 0);
    vmimtStoreRRO(MICROBLAZE_GPR_BITS, 0, MICROBLAZE_TMP_RD(1), gpr_rD, getEndian(state), True);
}

MICROBLAZE_MORPH_FN(morphSWR_TYPEA8) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    // Uns8 rA = state->info.rA;
    // vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    // vmiReg gpr_rA = MICROBLAZE_GPR_WR(rA);
    // Uns8 rB = state->info.rB;
    // vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    // vmiReg gpr_rB = MICROBLAZE_GPR_WR(rB);
    // Uns8 rD = state->info.rD;
    // vmiReg gpr_rD = MICROBLAZE_GPR_RD(rD);
    // vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);
    vmiMessage("F", "MORPH_UNIMP", "Instruction SWR_TYPEA8 unimplemented");
}

//
// Copy SPR using mask
//
static void emit_MFS_SPR_TYPEA9(vmiReg rD, vmiReg rS, vmiReg rM) {
    vmimtBinopRRR(MICROBLAZE_GPR_BITS, vmi_AND, rD, rS, rM, 0);
}

static void vmic_MFS_TYPEA9(microblazeP microblaze, Uns32 rD, Uns32 rS) {
    switch (rS) {
        case 0x0001 :
            microblazeFillMSR(microblaze);
            MFSREG(rD, MSR);
            break;
        case 0x0007 :
            microblazeFillFSR(microblaze);
            MFSREG(rD, FSR);
            break;
        case 0x1002 :
            microblazeSearchTLB(microblaze);
            MFSREG(rD, TLBX);
            break;
        case 0x1003 :
            microblazeReadTLBLO(microblaze);
            MFSREG(rD, TLBLO);
            break;
        case 0x1004 :
            microblazeReadTLBHI(microblaze);
            MFSREG(rD, TLBHI);
            break;
        default :
            // Illegal instruction
            break;
    }
}

#define SET_SPR(_R) \
        spr_rS       = MICROBLAZE_CPU_REG(SPR_ ## _R .reg); \
        spr_rS_rmask = MICROBLAZE_CPU_REG(SPR_## _R ## _rmask.reg);

MICROBLAZE_MORPH_FN(morphMFS_TYPEA9) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8  rD = state->info.rD;
    Uns16 rS = state->info.rS;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);
    vmiReg spr_rS, spr_rS_rmask;

    switch (rS) {
        //
        // These routines generate Morph time calls
        //
        case 0x0003 :
            SET_SPR(EAR);
            break;
        case 0x0005 :
            SET_SPR(ESR);
            break;
        case 0x000B :
            SET_SPR(BTR);
            break;
        case 0x000D :
            SET_SPR(EDR);
            break;
        case 0x0800 :
            SET_SPR(SLR);
            break;
        case 0x0802 :
            SET_SPR(SHR);
            break;
        case 0x1000 :
            SET_SPR(PID);
            break;
        case 0x1001 :
            SET_SPR(ZPR);
            break;
        case 0x2000 :
            SET_SPR(PVR0);
            break;
        case 0x2001 :
            SET_SPR(PVR1);
            break;
        case 0x2002 :
            SET_SPR(PVR2);
            break;
        case 0x2003 :
            SET_SPR(PVR3);
            break;
        case 0x2004 :
            SET_SPR(PVR4);
            break;
        case 0x2005 :
            SET_SPR(PVR5);
            break;
        case 0x2006 :
            SET_SPR(PVR6);
            break;
        case 0x2007 :
            SET_SPR(PVR7);
            break;
        case 0x2008 :
            SET_SPR(PVR8);
            break;
        case 0x2009 :
            SET_SPR(PVR9);
            break;
        case 0x200a :
            SET_SPR(PVR10);
            break;
        case 0x200b :
            SET_SPR(PVR11);
            break;

        case 0x0000 :   // PC
            vmimtMoveRSimPC(MICROBLAZE_GPR_BITS, gpr_rD);
            return;
            break;

        case 0x0001 :   // MSR
        case 0x0007 :   // FSR
        case 0x1002 :   // TLBX
        case 0x1003 :   // TLBLO
        case 0x1004 :   // TLBHI
            vmimtArgProcessor();
            vmimtArgUns32(rD);
            vmimtArgUns32(rS);
            vmimtCall((vmiCallFn)vmic_MFS_TYPEA9);
            return;
            break;

        default:
            return;
            break;
    }

    //
    // If we have dropped through
    //
    emit_MFS_SPR_TYPEA9(gpr_rD, spr_rS, spr_rS_rmask);
}

/*
 * mts rS, rA
 */

static void vmic_MTS_TYPEAA(microblazeP microblaze, Uns32 rS, Uns32 rA) {
    Uns32 mask, prevREG, nextREG;
    switch (rS) {
        case 0x0001 :
            MTSREG(MSR, rA);
            microblazeEmptyMSR(microblaze);
            microblazeUpdateBlockMask(microblaze);
            break;
        case 0x0007 :
            MTSREG(FSR, rA);
            microblazeEmptyFSR(microblaze);
            break;
        case 0x0800 :
            MTSREG(SLR, rA);
            break;
        case 0x0802 :
            MTSREG(SHR, rA);
            break;
        case 0x1000 :
            MTSREG(PID, rA);
            vmirtSetProcessorASID((vmiProcessorP)microblaze, microblaze->SPR_PID.bits.PID);
            break;
        case 0x1001 : {
            Uns32 oldZPR = microblaze->SPR_ZPR.reg;
            MTSREG(ZPR, rA);
            if (oldZPR != microblaze->SPR_ZPR.reg) {
                microblazeTLBRefreshZPR(microblaze);
            }
            break;
        }
        case 0x1002 :
            MTSREG(TLBX, rA);
            //
            // Clear Write only Miss register on TLBX write
            //
            microblaze->SPR_TLBX.bits.MISS = 0;
            break;
        case 0x1003 :
            MTSREG(TLBLO, rA);
            if (!microblaze->SPR_PVR0.bits.MMU) {
                vmiMessage("F", "MB_TLB", "Attempting Write to TLBLO when no MMU available");
            }
            microblazeWriteTLBLO(microblaze);
            break;
        case 0x1004 :
            MTSREG(TLBHI, rA);
            if (!microblaze->SPR_PVR0.bits.MMU) {
                vmiMessage("F", "MB_TLB", "Attempting Write to TLBHI when no MMU available");
            }
            microblazeWriteTLBHI(microblaze);
            break;
        case 0x1005 :
            MTSREG(TLBSX, rA);
            if (!microblaze->SPR_PVR0.bits.MMU) {
                vmiMessage("F", "MB_TLB", "Attempting Write to TLBSX when no MMU available");
            }
            microblazeSearchTLB(microblaze);
            break;
        default:
            // Illegal instruction
            // vmiMessage("F", "MTS RS, RA", "Illegal Target SPR value 0x%08x\n", rS);
            break;
    }
}

MICROBLAZE_MORPH_FN(morphMTS_TYPEAA) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    Uns16 rS = state->info.rS;

    if (validatePrivMode(state->microblaze)) {
        vmimtArgProcessor();
        vmimtArgUns32(rS);
        vmimtArgUns32(rA);
        vmimtCall((vmiCallFn)vmic_MTS_TYPEAA);
        vmimtEndBlock();
    }
}

MICROBLAZE_MORPH_FN(morphCLZ_TYPEAB) {
    //
    // Supported Architecture=V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);

    UNOPRR(vmi_CLZ, 0);
}

MICROBLAZE_MORPH_FN(morphSEXT16_TYPEAB) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);

    vmimtMoveExtendRR(MICROBLAZE_GPR_BITS, gpr_rD, MICROBLAZE_GPR_BITS/2, gpr_rA, True);
}

MICROBLAZE_MORPH_FN(morphSEXT8_TYPEAB) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);

    vmimtMoveExtendRR(MICROBLAZE_GPR_BITS, gpr_rD, MICROBLAZE_GPR_BITS/4, gpr_rA, True);
}

//    Shifts arithmetically the contents of register rA, one bit to the right, and places the result in rD. The
//    most significant bit of rA (that is, the sign bit) placed in the most significant bit of rD. The least
//    significant bit coming out of the shift chain is placed in the Carry flag.
//
// (rD)[31] = (rA)[31]
// (rD)[30:0] = (rA)[31:1]
// MSR[C] =(rA)[0]
//
MICROBLAZE_MORPH_FN(morphSRA_TYPEAB) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);

    BINOPRRC(vmi_SAR, 0x1, &flags_Co);
}

//    Shifts the contents of register rA, one bit to the right, and places the result in rD. The Carry flag is
//    shifted in the shift chain and placed in the most significant bit of rD. The least significant bit coming
//    out of the shift chain is placed in the Carry flag.
//
//    (rD)[31] = MSR[C]
//    (rD)[30:0] = (rA)[0:30]
//    MSR[C] = (rA)[0]
//
MICROBLAZE_MORPH_FN(morphSRC_TYPEAB) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);

    BINOPRRC(vmi_RCR, 0x1, &flags_CiCo);
}

MICROBLAZE_MORPH_FN(morphSRL_TYPEAB) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);

    BINOPRRC(vmi_SHR, 0x1, &flags_Co);
}

MICROBLAZE_MORPH_FN(morphWDC_TYPEAC) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    // Uns8 rA = state->info.rA;
    // vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    // vmiReg gpr_rA = MICROBLAZE_GPR_WR(rA);
    // Uns8 rB = state->info.rB;
    // vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    // vmiReg gpr_rB = MICROBLAZE_GPR_WR(rB);

    if (validatePrivMode(state->microblaze)) {
        // NOP
    }
}

MICROBLAZE_MORPH_FN(morphWDCCLR_TYPEAC) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    // Uns8 rA = state->info.rA;
    // vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    // vmiReg gpr_rA = MICROBLAZE_GPR_WR(rA);
    // Uns8 rB = state->info.rB;
    // vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    // vmiReg gpr_rB = MICROBLAZE_GPR_WR(rB);
    vmiMessage("F", "MORPH_UNIMP", "Instruction WDCCLR_TYPEAC unimplemented");
}

MICROBLAZE_MORPH_FN(morphWDCFL_TYPEAC) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    // Uns8 rA = state->info.rA;
    // vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    // vmiReg gpr_rA = MICROBLAZE_GPR_WR(rA);
    // Uns8 rB = state->info.rB;
    // vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    // vmiReg gpr_rB = MICROBLAZE_GPR_WR(rB);
    vmiMessage("F", "MORPH_UNIMP", "Instruction WDCFL_TYPEAC unimplemented");
}

MICROBLAZE_MORPH_FN(morphWIC_TYPEAC) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    // Uns8 rA = state->info.rA;
    // vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    // vmiReg gpr_rA = MICROBLAZE_GPR_WR(rA);
    // Uns8 rB = state->info.rB;
    // vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    // vmiReg gpr_rB = MICROBLAZE_GPR_WR(rB);

    if (validatePrivMode(state->microblaze)) {
        // NOP
    }
}

MICROBLAZE_MORPH_FN(morphADDI_TYPEB1) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);

    BINOPRRC(vmi_ADD, getImm32(state), &flags_Co);
}

MICROBLAZE_MORPH_FN(morphADDIC_TYPEB1) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);

    BINOPRRC(vmi_ADC, getImm32(state), &flags_CiCo);
}

MICROBLAZE_MORPH_FN(morphADDIK_TYPEB1) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);

    BINOPRRC(vmi_ADD, getImm32(state), 0);
}

MICROBLAZE_MORPH_FN(morphADDIKC_TYPEB1) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);

    BINOPRRC(vmi_ADC, getImm32(state), &flags_Ci);
}

MICROBLAZE_MORPH_FN(morphANDI_TYPEB1) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);

    BINOPRRC(vmi_AND, getImm32(state), 0);
}

MICROBLAZE_MORPH_FN(morphANDNI_TYPEB1) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);

    BINOPRRC(vmi_ANDN, getImm32(state), 0);
}

MICROBLAZE_MORPH_FN(morphLBUI_TYPEB1) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);

    //
    // Store rD for exception
    //
    vmimtMoveRC(MICROBLAZE_GPR_BITS, MICROBLAZE_TMP_WR(0), rD);
    vmimtLoadRRO(MICROBLAZE_GPR_BITS, MICROBLAZE_GPR_BITS/4, getImm32(state), gpr_rD, gpr_rA, getEndian(state), False, True);
}

MICROBLAZE_MORPH_FN(morphLHUI_TYPEB1) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);

    //
    // Store rD for exception
    //
    vmimtMoveRC(MICROBLAZE_GPR_BITS, MICROBLAZE_TMP_WR(0), rD);
    vmimtLoadRRO(MICROBLAZE_GPR_BITS, MICROBLAZE_GPR_BITS/2, getImm32(state), gpr_rD, gpr_rA, getEndian(state), False, True);
}

MICROBLAZE_MORPH_FN(morphLWI_TYPEB1) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);

    //
    // Store rD for exception
    //
    vmimtMoveRC(MICROBLAZE_GPR_BITS, MICROBLAZE_TMP_WR(0), rD);
    vmimtLoadRRO(MICROBLAZE_GPR_BITS, MICROBLAZE_GPR_BITS, getImm32(state), gpr_rD, gpr_rA, getEndian(state), False, True);
}

MICROBLAZE_MORPH_FN(morphMULI_TYPEB1) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);

    BINOPRRC(vmi_IMUL, getImm32(state), 0);
}

MICROBLAZE_MORPH_FN(morphORI_TYPEB1) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);

    BINOPRRC(vmi_OR, getImm32(state), 0);
}

MICROBLAZE_MORPH_FN(morphRSUBI_TYPEB1) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);

    BINOPRRC(vmi_RSUB, getImm32(state), &negflags_Co);
}

MICROBLAZE_MORPH_FN(morphRSUBIC_TYPEB1) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);

    BINOPRRC(vmi_RSBB, getImm32(state), &negflags_CiCo);
}

MICROBLAZE_MORPH_FN(morphRSUBIK_TYPEB1) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);

    BINOPRRC(vmi_RSUB, getImm32(state), 0);
}

MICROBLAZE_MORPH_FN(morphRSUBIKC_TYPEB1) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);

    BINOPRRC(vmi_RSBB, getImm32(state), &negflags_Ci);
}

MICROBLAZE_MORPH_FN(morphSBI_TYPEB1) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);

    //
    // Store rD for exception
    //
    vmimtMoveRC(MICROBLAZE_GPR_BITS, MICROBLAZE_TMP_WR(0), rD);
    vmimtStoreRRO(MICROBLAZE_GPR_BITS/4, getImm32(state), gpr_rA, gpr_rD, getEndian(state), True);
}

MICROBLAZE_MORPH_FN(morphSHI_TYPEB1) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);

    //
    // Store rD for exception
    //
    vmimtMoveRC(MICROBLAZE_GPR_BITS, MICROBLAZE_TMP_WR(0), rD);
    vmimtStoreRRO(MICROBLAZE_GPR_BITS/2, getImm32(state), gpr_rA, gpr_rD, getEndian(state), True);
}

MICROBLAZE_MORPH_FN(morphSWI_TYPEB1) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);

    //
    // Store rD for exception
    //
    vmimtMoveRC(MICROBLAZE_GPR_BITS, MICROBLAZE_TMP_WR(0), rD);
    vmimtStoreRRO(MICROBLAZE_GPR_BITS, getImm32(state), gpr_rA, gpr_rD, getEndian(state), True);
}

MICROBLAZE_MORPH_FN(morphXORI_TYPEB1) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_RD(rD);

    BINOPRRC(vmi_XOR, getImm32(state), 0);
}

MICROBLAZE_MORPH_FN(morphBEQI_TYPEB2) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);

    Uns32 target = state->info.thisPC + getImm32(state);
    branchOnCondImm (vmi_COND_EQ, gpr_rA, target, False);
}

MICROBLAZE_MORPH_FN(morphBEQID_TYPEB2) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);

    Uns32 target = state->info.thisPC + getImm32(state);
    branchOnCondImm (vmi_COND_EQ, gpr_rA, target, True);
}

MICROBLAZE_MORPH_FN(morphBGEI_TYPEB2) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);

    Uns32 target = state->info.thisPC + getImm32(state);
    branchOnCondImm (vmi_COND_NS, gpr_rA, target, False);
}

MICROBLAZE_MORPH_FN(morphBGEID_TYPEB2) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);

    Uns32 target = state->info.thisPC + getImm32(state);
    branchOnCondImm (vmi_COND_NS, gpr_rA, target, True);
}

MICROBLAZE_MORPH_FN(morphBGTI_TYPEB2) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);

    Uns32 target = state->info.thisPC + getImm32(state);
    branchOnCondImm (vmi_COND_NLE, gpr_rA, target, False);
}

MICROBLAZE_MORPH_FN(morphBGTID_TYPEB2) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);

    Uns32 target = state->info.thisPC + getImm32(state);
    branchOnCondImm (vmi_COND_NLE, gpr_rA, target, True);
}

MICROBLAZE_MORPH_FN(morphBLEI_TYPEB2) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);

    Uns32 target = state->info.thisPC + getImm32(state);
    branchOnCondImm (vmi_COND_LE, gpr_rA, target, False);
}

MICROBLAZE_MORPH_FN(morphBLEID_TYPEB2) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);

    Uns32 target = state->info.thisPC + getImm32(state);
    branchOnCondImm (vmi_COND_LE, gpr_rA, target, True);
}

MICROBLAZE_MORPH_FN(morphBLTI_TYPEB2) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);

    Uns32 target = state->info.thisPC + getImm32(state);
    branchOnCondImm (vmi_COND_L, gpr_rA, target, False);
}

MICROBLAZE_MORPH_FN(morphBLTID_TYPEB2) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);

    Uns32 target = state->info.thisPC + getImm32(state);
    branchOnCondImm (vmi_COND_L, gpr_rA, target, True);
}

MICROBLAZE_MORPH_FN(morphBNEI_TYPEB2) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);

    Uns32 target = state->info.thisPC + getImm32(state);
    branchOnCondImm (vmi_COND_NZ, gpr_rA, target, False);
}

MICROBLAZE_MORPH_FN(morphBNEID_TYPEB2) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);

    Uns32 target = state->info.thisPC + getImm32(state);
    branchOnCondImm (vmi_COND_NZ, gpr_rA, target, True);
}

MICROBLAZE_MORPH_FN(morphBRAI_TYPEB3) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    branchImm(getImm32(state), False, vmi_JH_NONE, 0);
}

MICROBLAZE_MORPH_FN(morphBRAID_TYPEB3) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    branchImm(getImm32(state), True, vmi_JH_NONE, 0);
}

MICROBLAZE_MORPH_FN(morphBRI_TYPEB3) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns32 toAddress = state->info.thisPC + getImm32(state);
    branchImm(toAddress, False, vmi_JH_RELATIVE, 0);
}

MICROBLAZE_MORPH_FN(morphBRID_TYPEB3) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns32 toAddress = state->info.thisPC + getImm32(state);
    branchImm(toAddress, True, vmi_JH_RELATIVE, 0);
}

static VMI_POST_SLOT_FN(postDelaySlotBRALID_TYPEB4) {
    microblazeP microblaze = (microblazeP)processor;

    microblazeSaveVMMode(microblaze);
    microblazeUpdateBlockMask(microblaze);
}

MICROBLAZE_MORPH_FN(morphBRALID_TYPEB4) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);

    Int32 simm = getImm32(state);

    setReturnPC(gpr_rD, state);
    branchImm(simm, True, vmi_JH_CALL, simm == 0x8 ? postDelaySlotBRALID_TYPEB4 : 0);
}

MICROBLAZE_MORPH_FN(morphBRLID_TYPEB4) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);

    Uns32 toAddress = state->info.thisPC + getImm32(state);

    setReturnPC(gpr_rD, state);
    branchImm(toAddress, True, (vmi_JH_CALL | vmi_JH_RELATIVE), 0);
}

// Need to handle user mode case
static void vmic_BRKI_TYPEB5(microblazeP microblaze, Int32 simm) {
    microblaze->SPR_MSR.bits.BIP = 1;
    microblaze->FLG_reservation = 0;
    microblazeSaveVMMode(microblaze);
    microblazeUpdateBlockMask(microblaze);
}
MICROBLAZE_MORPH_FN(morphBRKI_TYPEB5) {

    Int32 simm = getImm32(state);

    if ((simm == 0x8) || (simm == 0x18) || validatePrivMode(state->microblaze)) {

        Uns8 rD = state->info.rD;
        vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);

        vmimtArgProcessor();
        vmimtArgUns32(simm);
        vmimtCall((vmiCallFn)vmic_BRKI_TYPEB5);

        setReturnPC(gpr_rD, state);
        vmimtUncondJump(0, simm, VMI_NOREG, vmi_JH_CALL);
    }
}

MICROBLAZE_MORPH_FN(morphBSLLI_TYPEB6) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);
    Uns8 uimm = state->info.uimm;

    BINOPRRC(vmi_SHL, uimm, 0);
}

MICROBLAZE_MORPH_FN(morphBSRAI_TYPEB6) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);
    Uns8 uimm = state->info.uimm;

    BINOPRRC(vmi_SAR, uimm, 0);
}

MICROBLAZE_MORPH_FN(morphBSRLI_TYPEB6) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    Uns8 rD = state->info.rD;
    vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);
    Uns8 uimm = state->info.uimm;

    BINOPRRC(vmi_SHR, uimm, 0);
}

//
// This is now a pseudo Instr which is merged with the following TYPEB Instr
//
MICROBLAZE_MORPH_FN(morphIMM_TYPEI1) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    // Int16 simm = state->info.simm;
    // vmiMessage("F", "MORPH_UNIMP", "Instruction IMM_TYPEI1 unimplemented");
}

MICROBLAZE_MORPH_FN(morphMBAR_TYPEB8) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    // Int16 simm = state->info.simm;
    vmiMessage("F", "MORPH_UNIMP", "Instruction MBAR_TYPEB8 unimplemented");
}

/*
 * msrset rD, imm
 */
static void vmic_MSR_SET_CLR_TYPEB9(microblazeP microblaze, Uns32 rD, Uns32 imm, Uns32 set) {
    microblazeFillMSR(microblaze);

    if (rD != 0) {
        microblaze->gpr[rD] = microblaze->SPR_MSR.reg;
    }
    if (set) {
        // Set action (applied to mask)
        microblaze->SPR_MSR.reg |= (imm & microblaze->SPR_MSR_wmask.reg);    // bits affected by set
    } else {
        // Clear action (applied to mask)
        microblaze->SPR_MSR.reg &= ~(imm & microblaze->SPR_MSR_wmask.reg);   // bits affected by clr
    }

    //
    // Check carry flag, copy to CC and internal FLG_CC
    //
    microblazeEmptyMSR(microblaze);
    microblazeUpdateBlockMask(microblaze);
}

static inline void vmic_setclr_common(microblazeP microblaze, Uns32 rD) {
    if (rD != 0) {
        microblazeFillMSR(microblaze);
        microblaze->gpr[rD] = microblaze->SPR_MSR.reg;
    }
}
static void vmic_setclr_ie(microblazeP microblaze, Uns32 rD, Uns32 imm, Uns32 set) {
    vmic_setclr_common(microblaze, rD);
    microblaze->SPR_MSR.bits.IE = set;
    microblazeCheckExceptions(microblaze);
}
static void vmic_setclr_bip(microblazeP microblaze, Uns32 rD, Uns32 imm, Uns32 set) {
    vmic_setclr_common(microblaze, rD);
    microblaze->SPR_MSR.bits.BIP = set;
    microblazeCheckExceptions(microblaze);
}
static void vmic_setclr_ums(microblazeP microblaze, Uns32 rD, Uns32 imm, Uns32 set) {
    vmic_setclr_common(microblaze, rD);
    microblaze->SPR_MSR.bits.UMS = set;
}
static void vmic_setclr_vms(microblazeP microblaze, Uns32 rD, Uns32 imm, Uns32 set) {
    vmic_setclr_common(microblaze, rD);
    microblaze->SPR_MSR.bits.VMS = set;
}
static Bool Morph_MSR_SET_CLR_TYPEB9(Uns32 rD, Uns32 imm, Uns32 set) {
    Bool Status;
//    return False;
    //
    // Attempt to optimize some common operations
    //
    vmimtArgProcessor();
    vmimtArgUns32(rD);
    vmimtArgUns32(imm);
    vmimtArgUns32(set);
    switch (imm) {
        case MICROBLAZE_MSR_IE:
            vmimtCall((vmiCallFn)vmic_setclr_ie);
            Status = True;
            break;

        case MICROBLAZE_MSR_BIP:
            vmimtCall((vmiCallFn)vmic_setclr_bip);
            Status = True;
            break;

        case MICROBLAZE_MSR_UMS:
            vmimtCall((vmiCallFn)vmic_setclr_ums);
            Status = True;
            break;

        case MICROBLAZE_MSR_VMS:
            vmimtCall((vmiCallFn)vmic_setclr_vms);
            Status = True;
            break;

        case (MICROBLAZE_MSR_UMS | MICROBLAZE_MSR_VMS):
            vmimtCall((vmiCallFn)vmic_setclr_ums);
            vmimtCall((vmiCallFn)vmic_setclr_vms);
            Status = True;
            break;

        default:
            // Cannot Morph
            Status = False;
            break;
    }
    return Status;
}

MICROBLAZE_MORPH_FN(morphMSRCLR_TYPEB9) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rD = state->info.rD;
    Uns16 uimm = state->info.uimm;

    if ((uimm == 0x4) || validatePrivMode(state->microblaze)) {
        //
        // Try and do some ops at morph time
        //
        if (Morph_MSR_SET_CLR_TYPEB9(rD, uimm, 0)) {
        } else {
            vmimtArgProcessor();
            vmimtArgUns32(rD);
            vmimtArgUns32(uimm);
            vmimtArgUns32(0);
            vmimtCall((vmiCallFn)vmic_MSR_SET_CLR_TYPEB9);
        }
    }
}

MICROBLAZE_MORPH_FN(morphMSRSET_TYPEB9) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rD = state->info.rD;
    Uns16 uimm = state->info.uimm;

    if ((uimm == 0x4) || validatePrivMode(state->microblaze)) {
        if (Morph_MSR_SET_CLR_TYPEB9(rD, uimm, 1)) {
        } else {
            vmimtArgProcessor();
            vmimtArgUns32(rD);
            vmimtArgUns32(uimm);
            vmimtArgUns32(1);
            vmimtCall((vmiCallFn)vmic_MSR_SET_CLR_TYPEB9);
        }
    }
}

static VMI_POST_SLOT_FN(postDelaySlotRTBD_TYPEBA) {
    microblazeP microblaze = (microblazeP)processor;

    microblaze->SPR_MSR.bits.BIP = 0;
    microblazeRestoreVMMode(microblaze);
    microblazeUpdateBlockMask(microblaze);
}

// Need to handle modal behavior
MICROBLAZE_MORPH_FN(morphRTBD_TYPEBA) {
    if (validatePrivMode(state->microblaze)) {

        //
        // Supported Architecture=MICROBLAZE_V8_20
        //
        Uns8 rA = state->info.rA;
        vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);

        vmimtBinopRRC(MICROBLAZE_GPR_BITS, vmi_ADD, MICROBLAZE_TMP_WR(0), gpr_rA, getImm32(state), 0);
        vmimtUncondJumpRegDelaySlot(1, 0, MICROBLAZE_TMP_RD(0), VMI_NOREG, vmi_JH_RETURN, postDelaySlotRTBD_TYPEBA);
    }
}

static VMI_POST_SLOT_FN(postDelaySlotRTED_TYPEBA) {
    microblazeP microblaze = (microblazeP)processor;

    microblazeExceptionReturn(microblaze);
    microblazeRestoreVMMode(microblaze);
    microblaze->SPR_ESR.reg = 0;
    microblazeUpdateBlockMask(microblaze);
}

// Need to handle modal behavior
MICROBLAZE_MORPH_FN(morphRTED_TYPEBA) {
    if (validatePrivMode(state->microblaze)) {
        //
        // Supported Architecture=MICROBLAZE_V8_20
        //
        Uns8 rA = state->info.rA;
        vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);

        vmimtBinopRRC(MICROBLAZE_GPR_BITS, vmi_ADD, MICROBLAZE_TMP_WR(0), gpr_rA, getImm32(state), 0);
        vmimtUncondJumpRegDelaySlot(1, 0, MICROBLAZE_TMP_RD(0), VMI_NOREG, vmi_JH_RETURN, postDelaySlotRTED_TYPEBA);
    }
}

static VMI_POST_SLOT_FN(postDelaySlotRTID_TYPEBA) {
    microblazeP microblaze = (microblazeP)processor;

    microblazeInterruptReturn(microblaze);
    microblazeRestoreVMMode(microblaze);
    microblazeUpdateBlockMask(microblaze);
}

// Need to handle modal behavior
MICROBLAZE_MORPH_FN(morphRTID_TYPEBA) {
    if (validatePrivMode(state->microblaze)) {
        //
        // Supported Architecture=MICROBLAZE_V8_20
        //
        Uns8 rA = state->info.rA;
        vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);

        vmimtBinopRRC(MICROBLAZE_GPR_BITS, vmi_ADD, MICROBLAZE_TMP_WR(0), gpr_rA, getImm32(state), 0);
        vmimtUncondJumpRegDelaySlot(1, 0, MICROBLAZE_TMP_RD(0), VMI_NOREG, vmi_JH_RETURN, postDelaySlotRTID_TYPEBA);
    }
}

MICROBLAZE_MORPH_FN(morphRTSD_TYPEBA) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);

    vmimtBinopRRC(MICROBLAZE_GPR_BITS, vmi_ADD, MICROBLAZE_TMP_WR(0), gpr_rA, getImm32(state), 0);
    vmimtUncondJumpRegDelaySlot(1, 0, MICROBLAZE_TMP_RD(0), VMI_NOREG, vmi_JH_RETURN, 0);
}

//
// Get the target vmiReg of the fifo write
//
static vmiConnInputP vmic_FSL_slave(microblazeP microblaze, Uns32 index) {
    return microblaze->SFSL[index & 0xf];
}
static vmiReg targetFifoGet(microblazeMorphStateP state, Bool d) {
    Uns8 FSLx, rB;
    vmiReg gpr_rB, fifo;

    //
    // Decide which FSL to use
    //
    if (d) {
        // Use Dymnamic method (indirect)
        rB = state->info.rB;
        gpr_rB = MICROBLAZE_GPR_RD(rB);
        vmimtArgProcessor();
        vmimtArgReg(MICROBLAZE_GPR_BITS, gpr_rB);
        vmimtCallResult((vmiCallFn)vmic_FSL_slave, sizeof(vmiConnInputP)*8, MICROBLAZE_SFSL_DYN);
        fifo = MICROBLAZE_SFSL_DYN;
    } else {
        // Use Static method (direct)
        FSLx = state->info.FSLx;
        fifo = MICROBLAZE_SFSL(FSLx);
    }
    return fifo;
}

//
// GET xxxxx functions
//
static void emit_get(microblazeMorphStateP state, Bool n, Bool c, Bool t, Bool a, Bool e, Bool d) {
    Uns8      rD        = state->info.rD;
    vmiReg    gpr_rD    = MICROBLAZE_GPR_WR(rD);
    vmiReg    SFSL      = targetFifoGet(state, d);
    vmiLabelP TYPE_GOOD = vmimtNewLabel();
    vmiLabelP TYPE_BAD  = vmimtNewLabel();
    vmiLabelP READ_GOOD = vmimtNewLabel();
    vmiLabelP READ_BAD  = vmimtNewLabel();
    vmiLabelP BLOCK_END = vmimtNewLabel();
    Bool      peek;

    if (!validateFSLPrivMode(state->microblaze)) {
        return;
    }

    //
    // Construct Register Value to put
    //
    if (t) {
        // Test (peek)
        peek = True;
    } else {
        // ! Test (READ & POP)
        peek = False;
    }

    //
    // store rD in tmp[1] in case of an exception
    //
    vmimtMoveRR(MICROBLAZE_GPR_BITS, MICROBLAZE_TMP_WR(2), gpr_rD);

    //
    // get the Data & Control TMP_WR(0) <= Data, TMP_WR(1) <= Control
    //
    if (n) {
        // non-blocking
        vmimtConnGetRNB(MICROBLAZE_FSL_BITS,  MICROBLAZE_TMP_WR(0), SFSL, peek, MICROBLAZE_FLG_TMP_WR);
        vmimtBinopRRC(MICROBLAZE_FLG_CC_BITS, vmi_XOR, MICROBLAZE_FLG_CC_WR, MICROBLAZE_FLG_TMP_WR, 0x1, 0);

        // Test to see if READ_BAD, if jump to label to invalidate target register
        vmimtCompareRCJumpLabel(MICROBLAZE_GPR_BITS, vmi_COND_EQ, MICROBLAZE_FLG_TMP_RD, 0, READ_BAD);
    } else {
        // blocking
        vmimtConnGetRB(MICROBLAZE_FSL_BITS, MICROBLAZE_TMP_WR(0), SFSL, peek, fslRestartAfterBlock);

        // always assert that the read was OK
        vmimtMoveRC(MICROBLAZE_FLG_TMP_BITS, MICROBLAZE_FLG_TMP_WR, 0x1);
    }

    //
    // Update the target register from the temporary, in the case of a successful read
    //
    vmimtInsertLabel(READ_GOOD);
    vmimtMoveRR(MICROBLAZE_GPR_BITS, gpr_rD, MICROBLAZE_TMP_RD(0));

    //
    // Assert that the message (if READ_OK) is of the correct TYPE CONTROL/DATA
    //
    if (c) {
        // Control
        vmimtCompareRCJumpLabel(MICROBLAZE_GPR_BITS, vmi_COND_EQ, MICROBLAZE_TMP_RD(1), MICROBLAZE_FSL_CONTROL, TYPE_GOOD);
    } else {
        // Data
        vmimtCompareRCJumpLabel(MICROBLAZE_GPR_BITS, vmi_COND_EQ, MICROBLAZE_TMP_RD(1), MICROBLAZE_FSL_DATA, TYPE_GOOD);
    }

    // BEGIN if (TYPE_BAD) set ERROR bit, and pass in the exception, direct bits
    vmimtInsertLabel(TYPE_BAD);
    vmimtArgProcessor();
    vmimtArgUns32(state->info.instruction); // insn
    vmimtArgUns32(e);                       // Exception flag
    vmimtArgUns32(d);                       // Direct flag
    vmimtArgUns32(state->info.FSLx);        // FSL
    vmimtArgUns32(state->info.rB);          // gpr_rB
    vmimtArgUns32(state->info.rD);          // gpr_rD
    vmimtCall((vmiCallFn)microblazeStreamExceptionCB);
    // END

    vmimtInsertLabel(TYPE_GOOD);

    //
    // Atomic operation sequence
    //
    if (a) {
        // Atomic
    } else {
        // ! Atomic
    }

    vmimtUncondJumpLabel(BLOCK_END);

    //
    // Read was bad so we skipped the CONTROL/DATA bit check
    //
    vmimtInsertLabel(READ_BAD);
    vmimtMoveRC(MICROBLAZE_GPR_BITS, gpr_rD, 0xbaddf1f0);

    vmimtInsertLabel(BLOCK_END);
}

MICROBLAZE_MORPH_FN(morphAGET_TYPEC1) {
    emit_get(state, 0, 0, 0, 1, 0, 0);
}

MICROBLAZE_MORPH_FN(morphCAGET_TYPEC1) {
    emit_get(state, 0, 1, 0, 1, 0, 0);
}

MICROBLAZE_MORPH_FN(morphCGET_TYPEC1) {
    emit_get(state, 0, 1, 0, 0, 0, 0);
}

MICROBLAZE_MORPH_FN(morphEAGET_TYPEC1) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    // Uns8 FSLx = state->info.FSLx;
    // Uns8 rD = state->info.rD;
    // vmiReg gpr_rD = MICROBLAZE_GPR_RD(rD);
    // vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);
    vmiMessage("F", "MORPH_UNIMP", "Instruction EAGET_TYPEC1 unimplemented");
}

MICROBLAZE_MORPH_FN(morphECAGET_TYPEC1) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    // Uns8 FSLx = state->info.FSLx;
    // Uns8 rD = state->info.rD;
    // vmiReg gpr_rD = MICROBLAZE_GPR_RD(rD);
    // vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);
    vmiMessage("F", "MORPH_UNIMP", "Instruction ECAGET_TYPEC1 unimplemented");
}

MICROBLAZE_MORPH_FN(morphECGET_TYPEC1) {
    emit_get(state, 0, 1, 0, 0, 1, 0);
}

MICROBLAZE_MORPH_FN(morphEGET_TYPEC1) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    // Uns8 FSLx = state->info.FSLx;
    // Uns8 rD = state->info.rD;
    // vmiReg gpr_rD = MICROBLAZE_GPR_RD(rD);
    // vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);
    vmiMessage("F", "MORPH_UNIMP", "Instruction EGET_TYPEC1 unimplemented");
}


MICROBLAZE_MORPH_FN(morphGET_TYPEC1) {
    emit_get(state, 0, 0, 0, 0, 0, 0);
}

MICROBLAZE_MORPH_FN(morphNAGET_TYPEC1) {
    emit_get(state, 1, 0, 0, 1, 0, 0);
}

MICROBLAZE_MORPH_FN(morphNCAGET_TYPEC1) {
    emit_get(state, 1, 1, 0, 1, 0, 0);
}

MICROBLAZE_MORPH_FN(morphNCGET_TYPEC1) {
    emit_get(state, 1, 1, 0, 0, 0, 0);
}

MICROBLAZE_MORPH_FN(morphNEAGET_TYPEC1) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    // Uns8 FSLx = state->info.FSLx;
    // Uns8 rD = state->info.rD;
    // vmiReg gpr_rD = MICROBLAZE_GPR_RD(rD);
    // vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);
    vmiMessage("F", "MORPH_UNIMP", "Instruction NEAGET_TYPEC1 unimplemented");
}

MICROBLAZE_MORPH_FN(morphNECAGET_TYPEC1) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    // Uns8 FSLx = state->info.FSLx;
    // Uns8 rD = state->info.rD;
    // vmiReg gpr_rD = MICROBLAZE_GPR_RD(rD);
    // vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);
    vmiMessage("F", "MORPH_UNIMP", "Instruction NECAGET_TYPEC1 unimplemented");
}

MICROBLAZE_MORPH_FN(morphNECGET_TYPEC1) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    // Uns8 FSLx = state->info.FSLx;
    // Uns8 rD = state->info.rD;
    // vmiReg gpr_rD = MICROBLAZE_GPR_RD(rD);
    // vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);
    vmiMessage("F", "MORPH_UNIMP", "Instruction NECGET_TYPEC1 unimplemented");
}

MICROBLAZE_MORPH_FN(morphNEGET_TYPEC1) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    // Uns8 FSLx = state->info.FSLx;
    // Uns8 rD = state->info.rD;
    // vmiReg gpr_rD = MICROBLAZE_GPR_RD(rD);
    // vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);
    vmiMessage("F", "MORPH_UNIMP", "Instruction NEGET_TYPEC1 unimplemented");
}

MICROBLAZE_MORPH_FN(morphNGET_TYPEC1) {
    emit_get(state, 1, 0, 0, 0, 0, 0);
}

MICROBLAZE_MORPH_FN(morphTAGET_TYPEC1) {
    emit_get(state, 0, 0, 1, 1, 0, 0);
}

MICROBLAZE_MORPH_FN(morphTCAGET_TYPEC1) {
    emit_get(state, 0, 1, 1, 1, 0, 0);
}

MICROBLAZE_MORPH_FN(morphTCGET_TYPEC1) {
    emit_get(state, 0, 1, 1, 0, 0, 0);
}

MICROBLAZE_MORPH_FN(morphTEAGET_TYPEC1) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    // Uns8 FSLx = state->info.FSLx;
    // Uns8 rD = state->info.rD;
    // vmiReg gpr_rD = MICROBLAZE_GPR_RD(rD);
    // vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);
    vmiMessage("F", "MORPH_UNIMP", "Instruction TEAGET_TYPEC1 unimplemented");
}

MICROBLAZE_MORPH_FN(morphTECAGET_TYPEC1) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    // Uns8 FSLx = state->info.FSLx;
    // Uns8 rD = state->info.rD;
    // vmiReg gpr_rD = MICROBLAZE_GPR_RD(rD);
    // vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);
    vmiMessage("F", "MORPH_UNIMP", "Instruction TECAGET_TYPEC1 unimplemented");
}

MICROBLAZE_MORPH_FN(morphTECGET_TYPEC1) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    // Uns8 FSLx = state->info.FSLx;
    // Uns8 rD = state->info.rD;
    // vmiReg gpr_rD = MICROBLAZE_GPR_RD(rD);
    // vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);
    vmiMessage("F", "MORPH_UNIMP", "Instruction TECGET_TYPEC1 unimplemented");
}

MICROBLAZE_MORPH_FN(morphTEGET_TYPEC1) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    // Uns8 FSLx = state->info.FSLx;
    // Uns8 rD = state->info.rD;
    // vmiReg gpr_rD = MICROBLAZE_GPR_RD(rD);
    // vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);
    vmiMessage("F", "MORPH_UNIMP", "Instruction TEGET_TYPEC1 unimplemented");
}

MICROBLAZE_MORPH_FN(morphTGET_TYPEC1) {
    emit_get(state, 0, 0, 1, 0, 0, 0);
}

MICROBLAZE_MORPH_FN(morphTNAGET_TYPEC1) {
    emit_get(state, 1, 0, 1, 0, 0, 0);
}

MICROBLAZE_MORPH_FN(morphTNCAGET_TYPEC1) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    // Uns8 FSLx = state->info.FSLx;
    // Uns8 rD = state->info.rD;
    // vmiReg gpr_rD = MICROBLAZE_GPR_RD(rD);
    // vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);
    vmiMessage("F", "MORPH_UNIMP", "Instruction TNCAGET_TYPEC1 unimplemented");
}

MICROBLAZE_MORPH_FN(morphTNCGET_TYPEC1) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    // Uns8 FSLx = state->info.FSLx;
    // Uns8 rD = state->info.rD;
    // vmiReg gpr_rD = MICROBLAZE_GPR_RD(rD);
    // vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);
    vmiMessage("F", "MORPH_UNIMP", "Instruction TNCGET_TYPEC1 unimplemented");
}

MICROBLAZE_MORPH_FN(morphTNEAGET_TYPEC1) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    // Uns8 FSLx = state->info.FSLx;
    // Uns8 rD = state->info.rD;
    // vmiReg gpr_rD = MICROBLAZE_GPR_RD(rD);
    // vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);
    vmiMessage("F", "MORPH_UNIMP", "Instruction TNEAGET_TYPEC1 unimplemented");
}

MICROBLAZE_MORPH_FN(morphTNECAGET_TYPEC1) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    // Uns8 FSLx = state->info.FSLx;
    // Uns8 rD = state->info.rD;
    // vmiReg gpr_rD = MICROBLAZE_GPR_RD(rD);
    // vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);
    vmiMessage("F", "MORPH_UNIMP", "Instruction TNECAGET_TYPEC1 unimplemented");
}

MICROBLAZE_MORPH_FN(morphTNECGET_TYPEC1) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    // Uns8 FSLx = state->info.FSLx;
    // Uns8 rD = state->info.rD;
    // vmiReg gpr_rD = MICROBLAZE_GPR_RD(rD);
    // vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);
    vmiMessage("F", "MORPH_UNIMP", "Instruction TNECGET_TYPEC1 unimplemented");
}

MICROBLAZE_MORPH_FN(morphTNEGET_TYPEC1) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    // Uns8 FSLx = state->info.FSLx;
    // Uns8 rD = state->info.rD;
    // vmiReg gpr_rD = MICROBLAZE_GPR_RD(rD);
    // vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);
    vmiMessage("F", "MORPH_UNIMP", "Instruction TNEGET_TYPEC1 unimplemented");
}

MICROBLAZE_MORPH_FN(morphTNGET_TYPEC1) {
    emit_get(state, 1, 0, 1, 0, 0, 0);
}

MICROBLAZE_MORPH_FN(morphAGETD_TYPEC2) {
    emit_get(state, 0, 0, 0, 1, 0, 1);
}

MICROBLAZE_MORPH_FN(morphCAGETD_TYPEC2) {
    emit_get(state, 0, 1, 0, 1, 0, 1);
}

MICROBLAZE_MORPH_FN(morphCGETD_TYPEC2) {
    emit_get(state, 0, 1, 0, 0, 0, 1);
}

MICROBLAZE_MORPH_FN(morphEAGETD_TYPEC2) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    // Uns8 rB = state->info.rB;
    // vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    // vmiReg gpr_rB = MICROBLAZE_GPR_WR(rB);
    // Uns8 rD = state->info.rD;
    // vmiReg gpr_rD = MICROBLAZE_GPR_RD(rD);
    // vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);
    vmiMessage("F", "MORPH_UNIMP", "Instruction EAGETD_TYPEC2 unimplemented");
}

MICROBLAZE_MORPH_FN(morphECAGETD_TYPEC2) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    // Uns8 rB = state->info.rB;
    // vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    // vmiReg gpr_rB = MICROBLAZE_GPR_WR(rB);
    // Uns8 rD = state->info.rD;
    // vmiReg gpr_rD = MICROBLAZE_GPR_RD(rD);
    // vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);
    vmiMessage("F", "MORPH_UNIMP", "Instruction ECAGETD_TYPEC2 unimplemented");
}

MICROBLAZE_MORPH_FN(morphECGETD_TYPEC2) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    emit_get(state, 0, 1, 0, 0, 1, 1);
}

MICROBLAZE_MORPH_FN(morphEGETD_TYPEC2) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    // Uns8 rB = state->info.rB;
    // vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    // vmiReg gpr_rB = MICROBLAZE_GPR_WR(rB);
    // Uns8 rD = state->info.rD;
    // vmiReg gpr_rD = MICROBLAZE_GPR_RD(rD);
    // vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);
    vmiMessage("F", "MORPH_UNIMP", "Instruction EGETD_TYPEC2 unimplemented");
}

MICROBLAZE_MORPH_FN(morphGETD_TYPEC2) {
    emit_get(state, 0, 0, 0, 0, 0, 1);
}

MICROBLAZE_MORPH_FN(morphNAGETD_TYPEC2) {
    emit_get(state, 1, 0, 0, 1, 0, 1);
}

MICROBLAZE_MORPH_FN(morphNCAGETD_TYPEC2) {
    emit_get(state, 1, 1, 0, 1, 0, 1);
}

MICROBLAZE_MORPH_FN(morphNCGETD_TYPEC2) {
    emit_get(state, 1, 1, 0, 0, 0, 1);
}

MICROBLAZE_MORPH_FN(morphNEAGETD_TYPEC2) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    // Uns8 rB = state->info.rB;
    // vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    // vmiReg gpr_rB = MICROBLAZE_GPR_WR(rB);
    // Uns8 rD = state->info.rD;
    // vmiReg gpr_rD = MICROBLAZE_GPR_RD(rD);
    // vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);
    vmiMessage("F", "MORPH_UNIMP", "Instruction NEAGETD_TYPEC2 unimplemented");
}

MICROBLAZE_MORPH_FN(morphNECAGETD_TYPEC2) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    // Uns8 rB = state->info.rB;
    // vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    // vmiReg gpr_rB = MICROBLAZE_GPR_WR(rB);
    // Uns8 rD = state->info.rD;
    // vmiReg gpr_rD = MICROBLAZE_GPR_RD(rD);
    // vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);
    vmiMessage("F", "MORPH_UNIMP", "Instruction NECAGETD_TYPEC2 unimplemented");
}

MICROBLAZE_MORPH_FN(morphNECGETD_TYPEC2) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    // Uns8 rB = state->info.rB;
    // vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    // vmiReg gpr_rB = MICROBLAZE_GPR_WR(rB);
    // Uns8 rD = state->info.rD;
    // vmiReg gpr_rD = MICROBLAZE_GPR_RD(rD);
    // vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);
    vmiMessage("F", "MORPH_UNIMP", "Instruction NECGETD_TYPEC2 unimplemented");
}

MICROBLAZE_MORPH_FN(morphNEGETD_TYPEC2) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    // Uns8 rB = state->info.rB;
    // vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    // vmiReg gpr_rB = MICROBLAZE_GPR_WR(rB);
    // Uns8 rD = state->info.rD;
    // vmiReg gpr_rD = MICROBLAZE_GPR_RD(rD);
    // vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);
    vmiMessage("F", "MORPH_UNIMP", "Instruction NEGETD_TYPEC2 unimplemented");
}

MICROBLAZE_MORPH_FN(morphNGETD_TYPEC2) {
    emit_get(state, 1, 0, 0, 0, 0, 1);
}

MICROBLAZE_MORPH_FN(morphTAGETD_TYPEC2) {
    emit_get(state, 0, 0, 1, 1, 0, 1);
}

MICROBLAZE_MORPH_FN(morphTCAGETD_TYPEC2) {
    emit_get(state, 0, 1, 1, 1, 0, 1);
}

MICROBLAZE_MORPH_FN(morphTCGETD_TYPEC2) {
    emit_get(state, 0, 1, 1, 0, 0, 1);
}

MICROBLAZE_MORPH_FN(morphTEAGETD_TYPEC2) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    // Uns8 rB = state->info.rB;
    // vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    // vmiReg gpr_rB = MICROBLAZE_GPR_WR(rB);
    // Uns8 rD = state->info.rD;
    // vmiReg gpr_rD = MICROBLAZE_GPR_RD(rD);
    // vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);
    vmiMessage("F", "MORPH_UNIMP", "Instruction TEAGETD_TYPEC2 unimplemented");
}

MICROBLAZE_MORPH_FN(morphTECAGETD_TYPEC2) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    // Uns8 rB = state->info.rB;
    // vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    // vmiReg gpr_rB = MICROBLAZE_GPR_WR(rB);
    // Uns8 rD = state->info.rD;
    // vmiReg gpr_rD = MICROBLAZE_GPR_RD(rD);
    // vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);
    vmiMessage("F", "MORPH_UNIMP", "Instruction TECAGETD_TYPEC2 unimplemented");
}

MICROBLAZE_MORPH_FN(morphTECGETD_TYPEC2) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    // Uns8 rB = state->info.rB;
    // vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    // vmiReg gpr_rB = MICROBLAZE_GPR_WR(rB);
    // Uns8 rD = state->info.rD;
    // vmiReg gpr_rD = MICROBLAZE_GPR_RD(rD);
    // vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);
    vmiMessage("F", "MORPH_UNIMP", "Instruction TECGETD_TYPEC2 unimplemented");
}

MICROBLAZE_MORPH_FN(morphTEGETD_TYPEC2) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    // Uns8 rB = state->info.rB;
    // vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    // vmiReg gpr_rB = MICROBLAZE_GPR_WR(rB);
    // Uns8 rD = state->info.rD;
    // vmiReg gpr_rD = MICROBLAZE_GPR_RD(rD);
    // vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);
    vmiMessage("F", "MORPH_UNIMP", "Instruction TEGETD_TYPEC2 unimplemented");
}

MICROBLAZE_MORPH_FN(morphTGETD_TYPEC2) {
    emit_get(state, 0, 0, 1, 0, 0, 1);
}

MICROBLAZE_MORPH_FN(morphTNAGETD_TYPEC2) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    // Uns8 rB = state->info.rB;
    // vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    // vmiReg gpr_rB = MICROBLAZE_GPR_WR(rB);
    // Uns8 rD = state->info.rD;
    // vmiReg gpr_rD = MICROBLAZE_GPR_RD(rD);
    // vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);
    vmiMessage("F", "MORPH_UNIMP", "Instruction TNAGETD_TYPEC2 unimplemented");
}

MICROBLAZE_MORPH_FN(morphTNCAGETD_TYPEC2) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    // Uns8 rB = state->info.rB;
    // vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    // vmiReg gpr_rB = MICROBLAZE_GPR_WR(rB);
    // Uns8 rD = state->info.rD;
    // vmiReg gpr_rD = MICROBLAZE_GPR_RD(rD);
    // vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);
    vmiMessage("F", "MORPH_UNIMP", "Instruction TNCAGETD_TYPEC2 unimplemented");
}

MICROBLAZE_MORPH_FN(morphTNCGETD_TYPEC2) {
    emit_get(state, 1, 1, 0, 0, 0, 1);
}

MICROBLAZE_MORPH_FN(morphTNEAGETD_TYPEC2) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    // Uns8 rB = state->info.rB;
    // vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    // vmiReg gpr_rB = MICROBLAZE_GPR_WR(rB);
    // Uns8 rD = state->info.rD;
    // vmiReg gpr_rD = MICROBLAZE_GPR_RD(rD);
    // vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);
    vmiMessage("F", "MORPH_UNIMP", "Instruction TNEAGETD_TYPEC2 unimplemented");
}

MICROBLAZE_MORPH_FN(morphTNECAGETD_TYPEC2) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    // Uns8 rB = state->info.rB;
    // vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    // vmiReg gpr_rB = MICROBLAZE_GPR_WR(rB);
    // Uns8 rD = state->info.rD;
    // vmiReg gpr_rD = MICROBLAZE_GPR_RD(rD);
    // vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);
    vmiMessage("F", "MORPH_UNIMP", "Instruction TNECAGETD_TYPEC2 unimplemented");
}

MICROBLAZE_MORPH_FN(morphTNECGETD_TYPEC2) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    // Uns8 rB = state->info.rB;
    // vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    // vmiReg gpr_rB = MICROBLAZE_GPR_WR(rB);
    // Uns8 rD = state->info.rD;
    // vmiReg gpr_rD = MICROBLAZE_GPR_RD(rD);
    // vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);
    vmiMessage("F", "MORPH_UNIMP", "Instruction TNECGETD_TYPEC2 unimplemented");
}

MICROBLAZE_MORPH_FN(morphTNEGETD_TYPEC2) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    // Uns8 rB = state->info.rB;
    // vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    // vmiReg gpr_rB = MICROBLAZE_GPR_WR(rB);
    // Uns8 rD = state->info.rD;
    // vmiReg gpr_rD = MICROBLAZE_GPR_RD(rD);
    // vmiReg gpr_rD = MICROBLAZE_GPR_WR(rD);
    vmiMessage("F", "MORPH_UNIMP", "Instruction TNEGETD_TYPEC2 unimplemented");
}

MICROBLAZE_MORPH_FN(morphTNGETD_TYPEC2) {
    emit_get(state, 1, 0, 1, 0, 0, 1);
}

//
// Get the FSL vmiConnOutputP for the PUT
//
static vmiConnOutputP vmic_FSL_master(microblazeP microblaze, Uns32 index) {
    return microblaze->MFSL[index & 0xf];
}

//
// Get the target vmiReg of the fifo write
//
static vmiReg targetFifoPut(microblazeMorphStateP state, Bool d) {
    Uns8 FSLx, rB;
    vmiReg gpr_rB, fifo;

    //
    // Decide which FSL to use
    //
    if (d) {
        // Use Dymnamic method (indirect)
        rB = state->info.rB;
        gpr_rB = MICROBLAZE_GPR_RD(rB);
        vmimtArgProcessor();
        vmimtArgReg(MICROBLAZE_GPR_BITS, gpr_rB);
        vmimtCallResult((vmiCallFn)vmic_FSL_master, sizeof(vmiConnOutputP)*8, MICROBLAZE_MFSL_DYN);
        fifo = MICROBLAZE_MFSL_DYN;
    } else {
        // Use Static method (direct)
        FSLx = state->info.FSLx;
        fifo = MICROBLAZE_MFSL(FSLx);
    }
    return fifo;
}

//
// PUT(D) NCTA functions
//
static void emit_put(microblazeMorphStateP state, Bool n, Bool c, Bool t, Bool a, Bool d) {

    if (!validateFSLPrivMode(state->microblaze)) {
        return;
    }

    Uns8 rA = state->info.rA;
    vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);

    vmiReg MFSL = targetFifoPut(state, d);
    vmiReg SOURCE_REG;

    //
    // Construct Type (control/data) to put
    //
    if (c) {
        // Control
        vmimtMoveRC(MICROBLAZE_GPR_BITS, MICROBLAZE_TMP_WR(1), MICROBLAZE_FSL_CONTROL);
    } else {
        // Data
        vmimtMoveRC(MICROBLAZE_GPR_BITS, MICROBLAZE_TMP_WR(1), MICROBLAZE_FSL_DATA);
    }

    //
    // Construct Register Value to put
    //
    if (t) {
        // Test
        SOURCE_REG = VMI_NOREG;
    } else {
        // ! Test
        vmimtMoveRR(MICROBLAZE_GPR_BITS, MICROBLAZE_TMP_WR(0), gpr_rA);
        SOURCE_REG = MICROBLAZE_TMP_WR(0);
    }

    //
    // put the Data & Control bit
    //
    if (n) {
        // non-blocking
        vmimtConnPutRNB(MICROBLAZE_FSL_BITS, MFSL, SOURCE_REG, MICROBLAZE_FLG_TMP_WR);
        vmimtBinopRRC(MICROBLAZE_FLG_CC_BITS, vmi_XOR, MICROBLAZE_FLG_CC_WR, MICROBLAZE_FLG_TMP_WR, 0x1, 0);
    } else {
        // blocking
        vmimtConnPutRB(MICROBLAZE_FSL_BITS, MFSL, SOURCE_REG, fslRestartAfterBlock);
    }

    //
    // Atomic
    //
    if (a) {
        // Atomic
    } else {
        // ! Atomic
    }
}

MICROBLAZE_MORPH_FN(morphAPUT_TYPEC3) {
    emit_put(state, 0, 0, 0, 1, 0);
}

MICROBLAZE_MORPH_FN(morphCAPUT_TYPEC3) {
    emit_put(state, 0, 1, 0, 1, 0);
}

MICROBLAZE_MORPH_FN(morphCPUT_TYPEC3) {
    emit_put(state, 0, 1, 0, 0, 0);
}

MICROBLAZE_MORPH_FN(morphNAPUT_TYPEC3) {
    emit_put(state, 1, 0, 0, 1, 0);
}

MICROBLAZE_MORPH_FN(morphNCAPUT_TYPEC3) {
    emit_put(state, 1, 1, 0, 1, 0);
}

MICROBLAZE_MORPH_FN(morphNCPUT_TYPEC3) {
    emit_put(state, 1, 1, 0, 0, 0);
}

MICROBLAZE_MORPH_FN(morphNPUT_TYPEC3) {
    emit_put(state, 1, 0, 0, 0, 0);
}

MICROBLAZE_MORPH_FN(morphPUT_TYPEC3) {
    emit_put(state, 0, 0, 0, 0, 0);
}

MICROBLAZE_MORPH_FN(morphAPUTD_TYPEC3D) {
    emit_put(state, 0, 0, 0, 1, 1);
}

MICROBLAZE_MORPH_FN(morphCAPUTD_TYPEC3D) {
    emit_put(state, 0, 1, 0, 1, 1);
}

MICROBLAZE_MORPH_FN(morphCPUTD_TYPEC3D) {
    emit_put(state, 0, 1, 0, 0, 1);
}

MICROBLAZE_MORPH_FN(morphNAPUTD_TYPEC3D) {
    emit_put(state, 1, 0, 0, 1, 1);
}

MICROBLAZE_MORPH_FN(morphNCAPUTD_TYPEC3D) {
    emit_put(state, 1, 1, 0, 1, 1);
}

MICROBLAZE_MORPH_FN(morphNCPUTD_TYPEC3D) {
    emit_put(state, 1, 1, 0, 0, 1);
}

MICROBLAZE_MORPH_FN(morphNPUTD_TYPEC3D) {
    emit_put(state, 1, 0, 0, 0, 1);
}

MICROBLAZE_MORPH_FN(morphPUTD_TYPEC3D) {
    emit_put(state, 0, 0, 0, 0, 1);
}

MICROBLAZE_MORPH_FN(morphTAPUT_TYPEC4) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    // Uns8 FSLx = state->info.FSLx;
    // Uns8 rA = state->info.rA;
    // vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    // vmiReg gpr_rA = MICROBLAZE_GPR_WR(rA);
    vmiMessage("F", "MORPH_UNIMP", "Instruction TAPUT_TYPEC4 unimplemented");
}

MICROBLAZE_MORPH_FN(morphTCAPUT_TYPEC4) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    // Uns8 FSLx = state->info.FSLx;
    // Uns8 rA = state->info.rA;
    // vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    // vmiReg gpr_rA = MICROBLAZE_GPR_WR(rA);
    vmiMessage("F", "MORPH_UNIMP", "Instruction TCAPUT_TYPEC4 unimplemented");
}

MICROBLAZE_MORPH_FN(morphTCPUT_TYPEC4) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    // Uns8 FSLx = state->info.FSLx;
    // Uns8 rA = state->info.rA;
    // vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    // vmiReg gpr_rA = MICROBLAZE_GPR_WR(rA);
    vmiMessage("F", "MORPH_UNIMP", "Instruction TCPUT_TYPEC4 unimplemented");
}

MICROBLAZE_MORPH_FN(morphTNAPUT_TYPEC4) {
    emit_put(state, 1, 0, 1, 1, 0);
}

MICROBLAZE_MORPH_FN(morphTNCAPUT_TYPEC4) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    // Uns8 FSLx = state->info.FSLx;
    // Uns8 rA = state->info.rA;
    // vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    // vmiReg gpr_rA = MICROBLAZE_GPR_WR(rA);
    vmiMessage("F", "MORPH_UNIMP", "Instruction TNCAPUT_TYPEC4 unimplemented");
}

MICROBLAZE_MORPH_FN(morphTNCPUT_TYPEC4) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    // Uns8 FSLx = state->info.FSLx;
    // Uns8 rA = state->info.rA;
    // vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    // vmiReg gpr_rA = MICROBLAZE_GPR_WR(rA);
    vmiMessage("F", "MORPH_UNIMP", "Instruction TNCPUT_TYPEC4 unimplemented");
}

MICROBLAZE_MORPH_FN(morphTNPUT_TYPEC4) {
    emit_put(state, 1, 0, 1, 0, 0);
}

MICROBLAZE_MORPH_FN(morphTPUT_TYPEC4) {
    emit_put(state, 0, 0, 1, 0, 0);
}

MICROBLAZE_MORPH_FN(morphTAPUTD_TYPEC4D) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    // Uns8 rA = state->info.rA;
    // vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    // vmiReg gpr_rA = MICROBLAZE_GPR_WR(rA);
    // Uns8 rB = state->info.rB;
    // vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    // vmiReg gpr_rB = MICROBLAZE_GPR_WR(rB);
    vmiMessage("F", "MORPH_UNIMP", "Instruction TAPUTD_TYPEC4D unimplemented");
}

MICROBLAZE_MORPH_FN(morphTCAPUTD_TYPEC4D) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    // Uns8 rA = state->info.rA;
    // vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    // vmiReg gpr_rA = MICROBLAZE_GPR_WR(rA);
    // Uns8 rB = state->info.rB;
    // vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    // vmiReg gpr_rB = MICROBLAZE_GPR_WR(rB);
    vmiMessage("F", "MORPH_UNIMP", "Instruction TCAPUTD_TYPEC4D unimplemented");
}

MICROBLAZE_MORPH_FN(morphTCPUTD_TYPEC4D) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    // Uns8 rA = state->info.rA;
    // vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    // vmiReg gpr_rA = MICROBLAZE_GPR_WR(rA);
    // Uns8 rB = state->info.rB;
    // vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    // vmiReg gpr_rB = MICROBLAZE_GPR_WR(rB);
    vmiMessage("F", "MORPH_UNIMP", "Instruction TCPUTD_TYPEC4D unimplemented");
}

MICROBLAZE_MORPH_FN(morphTNAPUTD_TYPEC4D) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    // Uns8 rA = state->info.rA;
    // vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    // vmiReg gpr_rA = MICROBLAZE_GPR_WR(rA);
    // Uns8 rB = state->info.rB;
    // vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    // vmiReg gpr_rB = MICROBLAZE_GPR_WR(rB);
    vmiMessage("F", "MORPH_UNIMP", "Instruction TNAPUTD_TYPEC4D unimplemented");
}

MICROBLAZE_MORPH_FN(morphTNCAPUTD_TYPEC4D) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    // Uns8 rA = state->info.rA;
    // vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    // vmiReg gpr_rA = MICROBLAZE_GPR_WR(rA);
    // Uns8 rB = state->info.rB;
    // vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    // vmiReg gpr_rB = MICROBLAZE_GPR_WR(rB);
    vmiMessage("F", "MORPH_UNIMP", "Instruction TNCAPUTD_TYPEC4D unimplemented");
}

MICROBLAZE_MORPH_FN(morphTNCPUTD_TYPEC4D) {
    //
    // Supported Architecture=MICROBLAZE_V8_20
    //
    // Uns8 rA = state->info.rA;
    // vmiReg gpr_rA = MICROBLAZE_GPR_RD(rA);
    // vmiReg gpr_rA = MICROBLAZE_GPR_WR(rA);
    // Uns8 rB = state->info.rB;
    // vmiReg gpr_rB = MICROBLAZE_GPR_RD(rB);
    // vmiReg gpr_rB = MICROBLAZE_GPR_WR(rB);
    vmiMessage("F", "MORPH_UNIMP", "Instruction TNCPUTD_TYPEC4D unimplemented");
}

MICROBLAZE_MORPH_FN(morphTNPUTD_TYPEC4D) {
    emit_put(state, 1, 0, 1, 0, 1);
}

MICROBLAZE_MORPH_FN(morphTPUTD_TYPEC4D) {
    emit_put(state, 0, 0, 1, 0, 1);
}

static void savePostSlotCB(vmiProcessorP processor, vmiSaveContextP cxt) {

    vmiPostSlotFn slotCB   = vmirtGetPostSlotCB(processor);
    Uns32         slotCBId = 0;

    // convert from slotCB to slotCBId
    if(!slotCB) {
        // no action
    } else if(slotCB==postDelaySlotBRALID_TYPEB4) {
        slotCBId = 1;
    } else if(slotCB==postDelaySlotRTBD_TYPEBA) {
        slotCBId = 2;
    } else if(slotCB==postDelaySlotRTED_TYPEBA) {
        slotCBId = 3;
    } else if(slotCB==postDelaySlotRTID_TYPEBA) {
        slotCBId = 4;
    } else {
        VMI_ABORT("unimplemented slotCB %p", slotCB); // LCOV_EXCL_LINE
    }

    // save slotCBId
    VMIRT_SAVE_REG(cxt, "slotCB", &slotCBId);
}

static void restorePostSlotCB(vmiProcessorP processor, vmiRestoreContextP cxt) {

    vmiPostSlotFn slotCB = 0;
    Uns32         slotCBId;

    // restore slotCBId
    VMIRT_RESTORE_REG(cxt, "slotCB", &slotCBId);

    // convert from slotCBId to slotCB
    if(!slotCBId) {
        // no action
    } else if(slotCBId==1) {
        slotCB = postDelaySlotBRALID_TYPEB4;
    } else if(slotCBId==2) {
        slotCB = postDelaySlotRTBD_TYPEBA;
    } else if(slotCBId==3) {
        slotCB = postDelaySlotRTED_TYPEBA;
    } else if(slotCBId==4) {
        slotCB = postDelaySlotRTID_TYPEBA;
    } else {
        VMI_ABORT("unimplemented slotCBId %u", slotCBId); // LCOV_EXCL_LINE
    }

    // restore slotCB
    vmirtSetPostSlotCB(processor, slotCB);
}

VMI_SAVE_STATE_FN(microblazeSaveStateCB) {

    switch(phase) {

        case SRT_BEGIN:
            // start of SMP cluster
            break;

        case SRT_BEGIN_CORE:
            // start of individual core - save post-delay-slot callback
            savePostSlotCB(processor, cxt);
            break;

        case SRT_END_CORE:
            // end of individual core
            break;

        case SRT_END:
            // end of SMP cluster
            break;

        default:
            // not reached
            VMI_ABORT("unimplemented case"); // LCOV_EXCL_LINE
            break;
    }
}

VMI_RESTORE_STATE_FN(microblazeRestoreStateCB) {

    switch(phase) {

        case SRT_BEGIN:
            // start of SMP cluster
            break;

        case SRT_BEGIN_CORE:
            // start of individual core - restore post-delay-slot callback
            restorePostSlotCB(processor, cxt);
            break;

        case SRT_END_CORE:
            // end of individual core
            break;

        case SRT_END:
            // end of SMP cluster
            break;

        default:
            // not reached
            VMI_ABORT("unimplemented case"); // LCOV_EXCL_LINE
            break;
    }
}

