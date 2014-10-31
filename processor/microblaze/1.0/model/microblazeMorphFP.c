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
#include "microblazeMorph.h"
#include "microblazeMorphFP.h"
#include "microblazeFunctions.h"
#include "microblazeStructure.h"

////////////////////////////////////////////////////////////////////////////////
// FPU UTILITIES
////////////////////////////////////////////////////////////////////////////////

//
// Set simulator floating point control word
//
void microblazeUpdateFPControlWord(microblazeP microblaze) {

    // TODO: When exception support is added for microblaze the
    //       masks need to be set here when fp exceptions are enabled
    vmiFPControlWord cwNoExceptions = {
            .IM  = 1,
            .DM  = 1,
            .ZM  = 1,
            .OM  = 1,
            .UM  = 1,
            .PM  = 1,
            .RC  = vmi_FPR_NEAREST,
            .FZ  = 1,
            .DAZ = 0
    };

    vmiFPControlWord cwExceptions = {
            .IM  = 0,
            .DM  = 0,
            .ZM  = 0,
            .OM  = 0,
            .UM  = 0,
            .PM  = 1,
            .RC  = vmi_FPR_NEAREST,
            .FZ  = 0,
            .DAZ = 0
    };

    // update simulated processor state
    if (microblaze->SPR_MSR.bits.EE) {
        vmirtSetFPControlWord((vmiProcessorP)microblaze, cwExceptions);
    } else {
        vmirtSetFPControlWord((vmiProcessorP)microblaze, cwNoExceptions);
    }
}

//
// Return vmiFlags generating zero flag in temporary
//
static vmiFlags getZFFlags(vmiReg tf) {

    vmiFlags flags  = VMI_NOFLAGS;

    // define vmiFlags structure using the carry flag
    flags.f[vmi_ZF] = tf;

    // return vmiFlags structure
    return flags;
}

//
// Emit code to set the sticky flags
//
inline static void emitSetSticky(vmiReg flags) {
    vmimtBinopRR(8, vmi_OR, MICROBLAZE_FLG_FP_WR, flags, 0);
}

//
// Emit code executed when a denormal operand is found
//
static void denormalOperand(vmiReg rD, Uns32 denormalResult, vmiLabelP skipInstruction) {

    // TODO: will need to generate exception and set ESR.EC here when enabled
    vmimtMoveRC(32, rD, denormalResult);
    vmimtMoveRC( 8, MICROBLAZE_FLG_DO_WR, 1);
    vmimtUncondJumpLabel(skipInstruction);

}

//
// Emit code to check for denormal on a single  input.
//
static void checkDenormalOperand(
    microblazeP microblaze,
    vmiReg      rA,
    vmiReg      rD,
    vmiLabelP   skipInstruction,
    Uns32       denormalResult
) {

     vmiLabelP notDenormal = vmimtNewLabel();

     // rA is not denormal if Exponent is not 0
     vmimtTestRCJumpLabel(32, vmi_COND_NZ, rA, MICROBLAZE_EXP_32, notDenormal);

     // Here only if rA has exponent=0, rA is not denormal if fraction is zero
     vmimtTestRCJumpLabel(32, vmi_COND_Z, rA, MICROBLAZE_FRACT_32, notDenormal);

     // Here when a denormal operand has been found
     denormalOperand(rD, denormalResult, skipInstruction);

     // Here when no denormal operand found
     vmimtInsertLabel(notDenormal);

}

//
// Emit code to check for denormal on two inputs
//
static void checkDenormalOperands(
    microblazeMorphStateP state,
    vmiReg      rA,
    vmiReg      rB,
    vmiReg      rD,
    vmiLabelP   skipInstruction,
    Uns32       denormalResult
) {
    microblazeP microblaze = state->microblaze;

    // run-time assertion the floating point exception state is consistent
    vmimtValidateBlockMask(BM_MSR_EE);

    if(microblaze->SPR_MSR.bits.EE) {
        //
        // If Exception occurs, then target register unchanged
        // So squirrel register index and value to be retrieved
        // in the case of an exception
        //
        vmimtMoveRC(MICROBLAZE_TMP_BITS, MICROBLAZE_TMP_WR(0), state->info.rD);
        vmimtMoveRR(MICROBLAZE_TMP_BITS, MICROBLAZE_TMP_WR(1), rD);
        vmimtMoveRR(MICROBLAZE_TMP_BITS, MICROBLAZE_TMP_WR(2), rA);
        vmimtMoveRR(MICROBLAZE_TMP_BITS, MICROBLAZE_TMP_WR(3), rB);

    } else {
        vmiLabelP rBCheck     = vmimtNewLabel();
        vmiLabelP notDenormal = vmimtNewLabel();
        vmiLabelP isDenormal  = vmimtNewLabel();

        // rA is not denormal if Exponent is not 0
        vmimtTestRCJumpLabel(32, vmi_COND_NZ, rA, MICROBLAZE_EXP_32, rBCheck);

        // Here only if rA has exponent=0: rA is denormal if fraction is not zero
        vmimtTestRCJumpLabel(32, vmi_COND_NZ, rA, MICROBLAZE_FRACT_32, isDenormal);

        // Here when rA is not denormal
        vmimtInsertLabel(rBCheck);

        // rB is not denormal if Exponent is not 0
        vmimtTestRCJumpLabel(32, vmi_COND_NZ, rB, MICROBLAZE_EXP_32, notDenormal);

        // Here only if rB has exponent=0: rB is not denormal if fraction is zero
        vmimtTestRCJumpLabel(32, vmi_COND_Z, rB, MICROBLAZE_FRACT_32, notDenormal);

        // Here when a denormal operand has been found
        vmimtInsertLabel(isDenormal);

        denormalOperand(rD, denormalResult, skipInstruction);

        // Here when no denormal operand found
        vmimtInsertLabel(notDenormal);
    }
}


////////////////////////////////////////////////////////////////////////////////
// FPU ARITHMETIC INSTRUCTIONS
////////////////////////////////////////////////////////////////////////////////

//
// Emit code for generic FPU unop
//
static void emitFPUnop(microblazeMorphStateP state, vmiReg rA, vmiReg rD, vmiFUnop op) {

    microblazeP microblaze      = state->microblaze;
    vmiReg      fpuFlag         = MICROBLAZE_TMP_WR(0);
    vmiLabelP   skipInstruction = vmimtNewLabel();

    // Check for denormal operand and skip instruction if found
    checkDenormalOperand(microblaze, rA, rD, skipInstruction, MICROBLAZE_FIXED_NAN_32);

    // do the binary operation - following actions may not be done if there is
    // a floating-point exception
    vmimtFUnopRR(vmi_FT_32_IEEE_754, op, rD, rA, fpuFlag, 0);

    //  set sticky flags from temp
    emitSetSticky(fpuFlag);

    // Label to jump to when instruction is to be skipped
    vmimtInsertLabel(skipInstruction);
}

//
// Emit code for generic FPU binop
//
static void emitFPBinop(microblazeMorphStateP state, vmiReg rA, vmiReg rB, vmiReg rD, vmiFBinop op) {

    vmiReg      fpuFlag         = MICROBLAZE_TMP_WR(0);
    vmiLabelP   skipInstruction = vmimtNewLabel();

    // Check for denormal operands and skip instruction if found
    checkDenormalOperands(state, rA, rB, rD, skipInstruction, MICROBLAZE_FIXED_NAN_32);

    // do the binary operation - following actions may not be done if there is
    // a floating-point exception
    vmimtFBinopRRR(vmi_FT_32_IEEE_754, op, rD, rA, rB, fpuFlag, 0);

    //  set sticky flags from fpuFlag value
    emitSetSticky(fpuFlag);

    // Label to jump to when instruction is to be skipped
    vmimtInsertLabel(skipInstruction);
}

//
// Morph function for fadd instruction
//
MICROBLAZE_MORPH_FN(morphFADD_TYPEA1) {

    vmiReg rA = MICROBLAZE_GPR_RD(state->info.rA);
    vmiReg rB = MICROBLAZE_GPR_RD(state->info.rB);
    vmiReg rD = MICROBLAZE_GPR_WR(state->info.rD);

    emitFPBinop(state, rA, rB, rD, vmi_FADD);
}

//
// Morph function for fdiv instruction
//
MICROBLAZE_MORPH_FN(morphFDIV_TYPEA1) {

    vmiReg rA = MICROBLAZE_GPR_RD(state->info.rA);
    vmiReg rB = MICROBLAZE_GPR_RD(state->info.rB);
    vmiReg rD = MICROBLAZE_GPR_WR(state->info.rD);

    // Note: rA and rB are reversed to get rD = rB / rA
    emitFPBinop(state, rB, rA, rD, vmi_FDIV);
}

//
// Morph function for fmul instruction
//
MICROBLAZE_MORPH_FN(morphFMUL_TYPEA1) {

    vmiReg rA = MICROBLAZE_GPR_RD(state->info.rA);
    vmiReg rB = MICROBLAZE_GPR_RD(state->info.rB);
    vmiReg rD = MICROBLAZE_GPR_WR(state->info.rD);

    emitFPBinop(state, rA, rB, rD, vmi_FMUL);
}

//
// Morph function for frsub instruction
//
MICROBLAZE_MORPH_FN(morphFRSUB_TYPEA1) {

    vmiReg rA = MICROBLAZE_GPR_RD(state->info.rA);
    vmiReg rB = MICROBLAZE_GPR_RD(state->info.rB);
    vmiReg rD = MICROBLAZE_GPR_WR(state->info.rD);

    // Note: rA and rB are reversed to get rD = rB - rA
    emitFPBinop(state, rB, rA, rD, vmi_FSUB);

}

//
// Morph function for fsqrt instruction
//
MICROBLAZE_MORPH_FN(morphFSQRT_TYPEAB) {

    vmiReg rA = MICROBLAZE_GPR_RD(state->info.rA);
    vmiReg rD = MICROBLAZE_GPR_WR(state->info.rD);

    emitFPUnop(state, rA, rD, vmi_FSQRT);
}


////////////////////////////////////////////////////////////////////////////////
// FPU CONVERT INSTRUCTIONS
////////////////////////////////////////////////////////////////////////////////

//
// Emit code for integer to float conversion
// (no flags or exceptions)
//
MICROBLAZE_MORPH_FN(morphFLT_TYPEAB) {

    vmiReg rA = MICROBLAZE_GPR_RD(state->info.rA);
    vmiReg rD = MICROBLAZE_GPR_WR(state->info.rD);

    // TODO: must disable or ignore  exceptions when implemented
    vmimtFConvertRR(vmi_FT_32_IEEE_754, rD, vmi_FT_32_INT, rA, vmi_FPR_CURRENT, VMI_NOREG, 0);

}

//
// Emit code for float to integer conversion
//
MICROBLAZE_MORPH_FN(morphFINT_TYPEAB) {

    microblazeP microblaze      = state->microblaze;
    vmiReg      rA              = MICROBLAZE_GPR_RD(state->info.rA);
    vmiReg      rD              = MICROBLAZE_GPR_WR(state->info.rD);
    vmiReg      fpuFlag         = MICROBLAZE_TMP_WR(0);
    vmiLabelP   skipInstruction = vmimtNewLabel();

    // Check for denormal operand and skip instruction if found
    checkDenormalOperand(microblaze, rA, rD, skipInstruction, MICROBLAZE_FIXED_NAN_32);

    // Convert from FP to INT
    vmimtFConvertRR(vmi_FT_32_INT, rD, vmi_FT_32_IEEE_754, rA, vmi_FPR_ZERO, fpuFlag, 0);

    //  set sticky flags from fpuFlag value
    emitSetSticky(fpuFlag);

    // Label to jump to when instruction is to be skipped
    vmimtInsertLabel(skipInstruction);
}


////////////////////////////////////////////////////////////////////////////////
// FPU COMPARE INSTRUCTIONS
////////////////////////////////////////////////////////////////////////////////

//
// Emit code for generic FPU compare,
// if relationType is True,  set rD to 1 if relation result AND relation mask is not zero
// if relationType is False, set rD to 1 if relation result AND relation mask is zero
//
static void emitFPCompare(
    microblazeMorphStateP state,
    vmiReg                rA,
    vmiReg                rB,
    vmiReg                rD,
    Uns8                  relationMask,
    Uns8                  relationType,
    Bool                  allowQNaN
) {

    vmiReg      fpuFlag         = MICROBLAZE_TMP_WR(0);
    vmiReg      relation        = MICROBLAZE_TMP_WR(1);
    vmiReg      zFlag           = MICROBLAZE_TMP_WR(2);
    vmiFlags    flags           = getZFFlags(zFlag);
    vmiLabelP   skipInstruction = vmimtNewLabel();

    // Check for denormal operands and skip instruction if found
    checkDenormalOperands(state, rA, rB, rD, skipInstruction, 0);

    // do the compare
    vmimtFCompareRR(vmi_FT_32_IEEE_754, relation, rA, rB, fpuFlag, allowQNaN, 0);

    // If any bits in mask are set in relation then set result to 1, otherwise 0
    vmimtBinopRC(8, vmi_AND, relation, relationMask, &flags);
    vmimtCondMoveRCC(32, zFlag, relationType, rD, 0, 1);

    //  set sticky flags from fpuFlag value (Should just be IO flag)
    emitSetSticky(fpuFlag);

    // Label to jump to when instruction is to be skipped
    vmimtInsertLabel(skipInstruction);
}

//
// Morph function for fcmpun instruction
//
MICROBLAZE_MORPH_FN(morphFCMPUN_TYPEA1) {

    vmiReg rA = MICROBLAZE_GPR_RD(state->info.rA);
    vmiReg rB = MICROBLAZE_GPR_RD(state->info.rB);
    vmiReg rD = MICROBLAZE_GPR_WR(state->info.rD);

    // Note: rA and rB are reversed to compare rB - rA
    emitFPCompare(state, rB, rA, rD, vmi_FPRL_UNORDERED, True, True);
}

//
// Morph function for fcmpeq instruction
//
MICROBLAZE_MORPH_FN(morphFCMPEQ_TYPEA1) {

    vmiReg rA = MICROBLAZE_GPR_RD(state->info.rA);
    vmiReg rB = MICROBLAZE_GPR_RD(state->info.rB);
    vmiReg rD = MICROBLAZE_GPR_WR(state->info.rD);

    // Note: rA and rB are reversed to compare rB - rA
    emitFPCompare(state, rB, rA, rD, vmi_FPRL_EQUAL, True, True);
}

//
// Morph function for fcmpne instruction
//
MICROBLAZE_MORPH_FN(morphFCMPNE_TYPEA1) {

    vmiReg rA = MICROBLAZE_GPR_RD(state->info.rA);
    vmiReg rB = MICROBLAZE_GPR_RD(state->info.rB);
    vmiReg rD = MICROBLAZE_GPR_WR(state->info.rD);

    // Note: rA and rB are reversed to compare rB - rA
    emitFPCompare(state, rB, rA, rD, vmi_FPRL_EQUAL, False, True);
}

//
// Morph function for fcmpeq instruction
//
MICROBLAZE_MORPH_FN(morphFCMPGE_TYPEA1) {

    vmiReg rA = MICROBLAZE_GPR_RD(state->info.rA);
    vmiReg rB = MICROBLAZE_GPR_RD(state->info.rB);
    vmiReg rD = MICROBLAZE_GPR_WR(state->info.rD);

    // Note: rA and rB are reversed to compare rB - rA
    emitFPCompare(state, rB, rA, rD, vmi_FPRL_EQUAL|vmi_FPRL_GREATER, True, False);
}

//
// Morph function for fcmpgt instruction
//
MICROBLAZE_MORPH_FN(morphFCMPGT_TYPEA1) {

    vmiReg rA = MICROBLAZE_GPR_RD(state->info.rA);
    vmiReg rB = MICROBLAZE_GPR_RD(state->info.rB);
    vmiReg rD = MICROBLAZE_GPR_WR(state->info.rD);

    // Note: rA and rB are reversed to compare rB - rA
    emitFPCompare(state, rB, rA, rD, vmi_FPRL_GREATER, True, False);
}

//
// Morph function for fcmple instruction
//
MICROBLAZE_MORPH_FN(morphFCMPLE_TYPEA1) {

    vmiReg rA = MICROBLAZE_GPR_RD(state->info.rA);
    vmiReg rB = MICROBLAZE_GPR_RD(state->info.rB);
    vmiReg rD = MICROBLAZE_GPR_WR(state->info.rD);

    // Note: rA and rB are reversed to compare rB - rA
    emitFPCompare(state, rB, rA, rD, vmi_FPRL_EQUAL|vmi_FPRL_LESS, True, False);
}

//
// Morph function for fcmplt instruction
//
MICROBLAZE_MORPH_FN(morphFCMPLT_TYPEA1) {

    vmiReg rA = MICROBLAZE_GPR_RD(state->info.rA);
    vmiReg rB = MICROBLAZE_GPR_RD(state->info.rB);
    vmiReg rD = MICROBLAZE_GPR_WR(state->info.rD);

    // Note: rA and rB are reversed to compare rB - rA
    emitFPCompare(state, rB, rA, rD, vmi_FPRL_LESS, True, False);
}


////////////////////////////////////////////////////////////////////////////////
// FPU CONFIGURATION
////////////////////////////////////////////////////////////////////////////////

//
// Handle tiny result
//
static VMI_FP_TINY_RESULT_FN(handleTinyResult) {

    microblazeP microblaze = (microblazeP)processor;

    // Set underflow flag in VMI flags
    microblaze->FLG_FP.f.U = 1;

    // Result should be correctly-signed zero
    value.f80Parts.fraction = 0;
    value.f80Parts.exponent = 0;

    return value;
}

//
// Configure FPU, if implemented
//
void microblazeConfigureFPU(microblazeP microblaze) {

    if (microblaze->SPR_PVR0.bits.FPU) {

        // FPU configuration
        const static vmiFPConfig config = {
            .QNaN32             = MICROBLAZE_FIXED_NAN_32,
            .indeterminateUns32 = MICROBLAZE_FIXED_NAN_32,
            .tinyResultCB       = handleTinyResult,
            .fpArithExceptCB    = microblazeFPArithExcept,
        };

        // set up QNaN values and handlers
        vmirtConfigureFPU((vmiProcessorP)microblaze, &config);

        // initialize floating point control word
        microblazeUpdateFPControlWord(microblaze);

    }
}



