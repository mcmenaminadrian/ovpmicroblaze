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

#include "vmi/vmiCxt.h"
#include "vmi/vmiDecode.h"
#include "vmi/vmiMessage.h"
#include "microblazeDecode.h"
#include "microblazeInstructions.h"

//
// Generate the instruction opcode table
//
static const char *opcode[MICROBLAZE_IT_LAST+1] = {
    //
    // Instruction Opcode TypeA1 Insns
    //
    [MICROBLAZE_IT_ADD_TYPEA1] = "add",
    [MICROBLAZE_IT_ADDC_TYPEA1] = "addc",
    [MICROBLAZE_IT_ADDK_TYPEA1] = "addk",
    [MICROBLAZE_IT_ADDKC_TYPEA1] = "addkc",
    [MICROBLAZE_IT_AND_TYPEA1] = "and",
    [MICROBLAZE_IT_ANDN_TYPEA1] = "andn",
    [MICROBLAZE_IT_FADD_TYPEA1] = "fadd",
    [MICROBLAZE_IT_FCMPEQ_TYPEA1] = "fcmp.eq",
    [MICROBLAZE_IT_FCMPGE_TYPEA1] = "fcmp.ge",
    [MICROBLAZE_IT_FCMPGT_TYPEA1] = "fcmp.gt",
    [MICROBLAZE_IT_FCMPLE_TYPEA1] = "fcmp.le",
    [MICROBLAZE_IT_FCMPLT_TYPEA1] = "fcmp.lt",
    [MICROBLAZE_IT_FCMPNE_TYPEA1] = "fcmp.ne",
    [MICROBLAZE_IT_FCMPUN_TYPEA1] = "fcmp.un",
    [MICROBLAZE_IT_FDIV_TYPEA1] = "fdiv",
    [MICROBLAZE_IT_FMUL_TYPEA1] = "fmul",
    [MICROBLAZE_IT_FRSUB_TYPEA1] = "frsub",
    [MICROBLAZE_IT_LWX_TYPEA1] = "lwx",
    [MICROBLAZE_IT_MUL_TYPEA1] = "mul",
    [MICROBLAZE_IT_MULH_TYPEA1] = "mulh",
    [MICROBLAZE_IT_MULHSU_TYPEA1] = "mulhsu",
    [MICROBLAZE_IT_MULHU_TYPEA1] = "mulhu",
    [MICROBLAZE_IT_OR_TYPEA1] = "or",
    [MICROBLAZE_IT_PCMPBF_TYPEA1] = "pcmpbf",
    [MICROBLAZE_IT_PCMPEQ_TYPEA1] = "pcmpeq",
    [MICROBLAZE_IT_PCMPNE_TYPEA1] = "pcmpne",
    [MICROBLAZE_IT_RSUB_TYPEA1] = "rsub",
    [MICROBLAZE_IT_RSUBC_TYPEA1] = "rsubc",
    [MICROBLAZE_IT_RSUBK_TYPEA1] = "rsubk",
    [MICROBLAZE_IT_RSUBKC_TYPEA1] = "rsubkc",
    [MICROBLAZE_IT_SWX_TYPEA1] = "swx",
    [MICROBLAZE_IT_XOR_TYPEA1] = "xor",

    //
    // Instruction Opcode TypeA2 Insns
    //
    [MICROBLAZE_IT_BEQ_TYPEA2] = "beq",
    [MICROBLAZE_IT_BEQD_TYPEA2] = "beqd",
    [MICROBLAZE_IT_BGE_TYPEA2] = "bge",
    [MICROBLAZE_IT_BGED_TYPEA2] = "bged",
    [MICROBLAZE_IT_BGT_TYPEA2] = "bgt",
    [MICROBLAZE_IT_BGTD_TYPEA2] = "bgtd",
    [MICROBLAZE_IT_BLE_TYPEA2] = "ble",
    [MICROBLAZE_IT_BLED_TYPEA2] = "bled",
    [MICROBLAZE_IT_BLT_TYPEA2] = "blt",
    [MICROBLAZE_IT_BLTD_TYPEA2] = "bltd",
    [MICROBLAZE_IT_BNE_TYPEA2] = "bne",
    [MICROBLAZE_IT_BNED_TYPEA2] = "bned",

    //
    // Instruction Opcode TypeA3 Insns
    //
    [MICROBLAZE_IT_BR_TYPEA3] = "br",
    [MICROBLAZE_IT_BRA_TYPEA3] = "bra",
    [MICROBLAZE_IT_BRAD_TYPEA3] = "brad",
    [MICROBLAZE_IT_BRD_TYPEA3] = "brd",

    //
    // Instruction Opcode TypeA4 Insns
    //
    [MICROBLAZE_IT_BRALD_TYPEA4] = "brald",
    [MICROBLAZE_IT_BRLD_TYPEA4] = "brld",

    //
    // Instruction Opcode TypeA5 Insns
    //
    [MICROBLAZE_IT_BRK_TYPEA5] = "brk",

    //
    // Instruction Opcode TypeA6 Insns
    //
    [MICROBLAZE_IT_BSLL_TYPEA6] = "bsll",
    [MICROBLAZE_IT_BSRA_TYPEA6] = "bsra",
    [MICROBLAZE_IT_BSRL_TYPEA6] = "bsrl",

    //
    // Instruction Opcode TypeA7 Insns
    //
    [MICROBLAZE_IT_CMP_TYPEA7] = "cmp",
    [MICROBLAZE_IT_CMPU_TYPEA7] = "cmpu",
    [MICROBLAZE_IT_IDIV_TYPEA7] = "idiv",
    [MICROBLAZE_IT_IDIVU_TYPEA7] = "idivu",

    //
    // Instruction Opcode TypeA8 Insns
    //
    [MICROBLAZE_IT_LBU_TYPEA8] = "lbu",
    [MICROBLAZE_IT_LBUR_TYPEA8] = "lbur",
    [MICROBLAZE_IT_LHU_TYPEA8] = "lhu",
    [MICROBLAZE_IT_LHUR_TYPEA8] = "lhur",
    [MICROBLAZE_IT_LW_TYPEA8] = "lw",
    [MICROBLAZE_IT_LWR_TYPEA8] = "lwr",
    [MICROBLAZE_IT_SB_TYPEA8] = "sb",
    [MICROBLAZE_IT_SBR_TYPEA8] = "sbr",
    [MICROBLAZE_IT_SH_TYPEA8] = "sh",
    [MICROBLAZE_IT_SHR_TYPEA8] = "shr",
    [MICROBLAZE_IT_SW_TYPEA8] = "sw",
    [MICROBLAZE_IT_SWR_TYPEA8] = "swr",

    //
    // Instruction Opcode TypeA9 Insns
    //
    [MICROBLAZE_IT_MFS_TYPEA9] = "mfs",

    //
    // Instruction Opcode TypeAa Insns
    //
    [MICROBLAZE_IT_MTS_TYPEAA] = "mts",

    //
    // Instruction Opcode TypeAb Insns
    //
    [MICROBLAZE_IT_CLZ_TYPEAB] = "clz",
    [MICROBLAZE_IT_FINT_TYPEAB] = "fint",
    [MICROBLAZE_IT_FLT_TYPEAB] = "flt",
    [MICROBLAZE_IT_FSQRT_TYPEAB] = "fsqrt",
    [MICROBLAZE_IT_SEXT16_TYPEAB] = "sext16",
    [MICROBLAZE_IT_SEXT8_TYPEAB] = "sext8",
    [MICROBLAZE_IT_SRA_TYPEAB] = "sra",
    [MICROBLAZE_IT_SRC_TYPEAB] = "src",
    [MICROBLAZE_IT_SRL_TYPEAB] = "srl",

    //
    // Instruction Opcode TypeAc Insns
    //
    [MICROBLAZE_IT_WDC_TYPEAC] = "wdc",
    [MICROBLAZE_IT_WDCCLR_TYPEAC] = "wdc.clear",
    [MICROBLAZE_IT_WDCFL_TYPEAC] = "wdc.flush",
    [MICROBLAZE_IT_WIC_TYPEAC] = "wic",

    //
    // Instruction Opcode TypeB1 Insns
    //
    [MICROBLAZE_IT_ADDI_TYPEB1] = "addi",
    [MICROBLAZE_IT_ADDIC_TYPEB1] = "addic",
    [MICROBLAZE_IT_ADDIK_TYPEB1] = "addik",
    [MICROBLAZE_IT_ADDIKC_TYPEB1] = "addikc",
    [MICROBLAZE_IT_ANDI_TYPEB1] = "andi",
    [MICROBLAZE_IT_ANDNI_TYPEB1] = "andni",
    [MICROBLAZE_IT_LBUI_TYPEB1] = "lbui",
    [MICROBLAZE_IT_LHUI_TYPEB1] = "lhui",
    [MICROBLAZE_IT_LWI_TYPEB1] = "lwi",
    [MICROBLAZE_IT_MULI_TYPEB1] = "muli",
    [MICROBLAZE_IT_ORI_TYPEB1] = "ori",
    [MICROBLAZE_IT_RSUBI_TYPEB1] = "rsubi",
    [MICROBLAZE_IT_RSUBIC_TYPEB1] = "rsubic",
    [MICROBLAZE_IT_RSUBIK_TYPEB1] = "rsubik",
    [MICROBLAZE_IT_RSUBIKC_TYPEB1] = "rsubikc",
    [MICROBLAZE_IT_SBI_TYPEB1] = "sbi",
    [MICROBLAZE_IT_SHI_TYPEB1] = "shi",
    [MICROBLAZE_IT_SWI_TYPEB1] = "swi",
    [MICROBLAZE_IT_XORI_TYPEB1] = "xori",

    //
    // Instruction Opcode TypeB2 Insns
    //
    [MICROBLAZE_IT_BEQI_TYPEB2] = "beqi",
    [MICROBLAZE_IT_BEQID_TYPEB2] = "beqid",
    [MICROBLAZE_IT_BGEI_TYPEB2] = "bgei",
    [MICROBLAZE_IT_BGEID_TYPEB2] = "bgeid",
    [MICROBLAZE_IT_BGTI_TYPEB2] = "bgti",
    [MICROBLAZE_IT_BGTID_TYPEB2] = "bgtid",
    [MICROBLAZE_IT_BLEI_TYPEB2] = "blei",
    [MICROBLAZE_IT_BLEID_TYPEB2] = "bleid",
    [MICROBLAZE_IT_BLTI_TYPEB2] = "blti",
    [MICROBLAZE_IT_BLTID_TYPEB2] = "bltid",
    [MICROBLAZE_IT_BNEI_TYPEB2] = "bnei",
    [MICROBLAZE_IT_BNEID_TYPEB2] = "bneid",

    //
    // Instruction Opcode TypeB3 Insns
    //
    [MICROBLAZE_IT_BRAI_TYPEB3] = "brai",
    [MICROBLAZE_IT_BRAID_TYPEB3] = "braid",
    [MICROBLAZE_IT_BRI_TYPEB3] = "bri",
    [MICROBLAZE_IT_BRID_TYPEB3] = "brid",

    //
    // Instruction Opcode TypeB4 Insns
    //
    [MICROBLAZE_IT_BRALID_TYPEB4] = "bralid",
    [MICROBLAZE_IT_BRLID_TYPEB4] = "brlid",

    //
    // Instruction Opcode TypeB5 Insns
    //
    [MICROBLAZE_IT_BRKI_TYPEB5] = "brki",

    //
    // Instruction Opcode TypeB6 Insns
    //
    [MICROBLAZE_IT_BSLLI_TYPEB6] = "bslli",
    [MICROBLAZE_IT_BSRAI_TYPEB6] = "bsrai",
    [MICROBLAZE_IT_BSRLI_TYPEB6] = "bsrli",

    //
    // Instruction Opcode TypeB8 Insns
    //
    [MICROBLAZE_IT_MBAR_TYPEB8] = "mbar",

    //
    // Instruction Opcode TypeB9 Insns
    //
    [MICROBLAZE_IT_MSRCLR_TYPEB9] = "msrclr",
    [MICROBLAZE_IT_MSRSET_TYPEB9] = "msrset",

    //
    // Instruction Opcode TypeBa Insns
    //
    [MICROBLAZE_IT_RTBD_TYPEBA] = "rtbd",
    [MICROBLAZE_IT_RTED_TYPEBA] = "rted",
    [MICROBLAZE_IT_RTID_TYPEBA] = "rtid",
    [MICROBLAZE_IT_RTSD_TYPEBA] = "rtsd",

    //
    // Instruction Opcode TypeC1 Insns
    //
    [MICROBLAZE_IT_AGET_TYPEC1] = "aget",
    [MICROBLAZE_IT_CAGET_TYPEC1] = "caget",
    [MICROBLAZE_IT_CGET_TYPEC1] = "cget",
    [MICROBLAZE_IT_EAGET_TYPEC1] = "eaget",
    [MICROBLAZE_IT_ECAGET_TYPEC1] = "ecaget",
    [MICROBLAZE_IT_ECGET_TYPEC1] = "ecget",
    [MICROBLAZE_IT_EGET_TYPEC1] = "eget",
    [MICROBLAZE_IT_GET_TYPEC1] = "get",
    [MICROBLAZE_IT_NAGET_TYPEC1] = "naget",
    [MICROBLAZE_IT_NCAGET_TYPEC1] = "ncaget",
    [MICROBLAZE_IT_NCGET_TYPEC1] = "ncget",
    [MICROBLAZE_IT_NEAGET_TYPEC1] = "neaget",
    [MICROBLAZE_IT_NECAGET_TYPEC1] = "necaget",
    [MICROBLAZE_IT_NECGET_TYPEC1] = "necget",
    [MICROBLAZE_IT_NEGET_TYPEC1] = "neget",
    [MICROBLAZE_IT_NGET_TYPEC1] = "nget",
    [MICROBLAZE_IT_TAGET_TYPEC1] = "taget",
    [MICROBLAZE_IT_TCAGET_TYPEC1] = "tcaget",
    [MICROBLAZE_IT_TCGET_TYPEC1] = "tcget",
    [MICROBLAZE_IT_TEAGET_TYPEC1] = "teaget",
    [MICROBLAZE_IT_TECAGET_TYPEC1] = "tecaget",
    [MICROBLAZE_IT_TECGET_TYPEC1] = "tecget",
    [MICROBLAZE_IT_TEGET_TYPEC1] = "teget",
    [MICROBLAZE_IT_TGET_TYPEC1] = "tget",
    [MICROBLAZE_IT_TNAGET_TYPEC1] = "tnaget",
    [MICROBLAZE_IT_TNCAGET_TYPEC1] = "tncaget",
    [MICROBLAZE_IT_TNCGET_TYPEC1] = "tncget",
    [MICROBLAZE_IT_TNEAGET_TYPEC1] = "tneaget",
    [MICROBLAZE_IT_TNECAGET_TYPEC1] = "tnecaget",
    [MICROBLAZE_IT_TNECGET_TYPEC1] = "tnecget",
    [MICROBLAZE_IT_TNEGET_TYPEC1] = "tneget",
    [MICROBLAZE_IT_TNGET_TYPEC1] = "tnget",

    //
    // Instruction Opcode TypeC2 Insns
    //
    [MICROBLAZE_IT_AGETD_TYPEC2] = "agetd",
    [MICROBLAZE_IT_CAGETD_TYPEC2] = "cagetd",
    [MICROBLAZE_IT_CGETD_TYPEC2] = "cgetd",
    [MICROBLAZE_IT_EAGETD_TYPEC2] = "eagetd",
    [MICROBLAZE_IT_ECAGETD_TYPEC2] = "ecagetd",
    [MICROBLAZE_IT_ECGETD_TYPEC2] = "ecgetd",
    [MICROBLAZE_IT_EGETD_TYPEC2] = "egetd",
    [MICROBLAZE_IT_GETD_TYPEC2] = "getd",
    [MICROBLAZE_IT_NAGETD_TYPEC2] = "nagetd",
    [MICROBLAZE_IT_NCAGETD_TYPEC2] = "ncagetd",
    [MICROBLAZE_IT_NCGETD_TYPEC2] = "ncgetd",
    [MICROBLAZE_IT_NEAGETD_TYPEC2] = "neagetd",
    [MICROBLAZE_IT_NECAGETD_TYPEC2] = "necagetd",
    [MICROBLAZE_IT_NECGETD_TYPEC2] = "necgetd",
    [MICROBLAZE_IT_NEGETD_TYPEC2] = "negetd",
    [MICROBLAZE_IT_NGETD_TYPEC2] = "ngetd",
    [MICROBLAZE_IT_TAGETD_TYPEC2] = "tagetd",
    [MICROBLAZE_IT_TCAGETD_TYPEC2] = "tcagetd",
    [MICROBLAZE_IT_TCGETD_TYPEC2] = "tcgetd",
    [MICROBLAZE_IT_TEAGETD_TYPEC2] = "teagetd",
    [MICROBLAZE_IT_TECAGETD_TYPEC2] = "tecagetd",
    [MICROBLAZE_IT_TECGETD_TYPEC2] = "tecgetd",
    [MICROBLAZE_IT_TEGETD_TYPEC2] = "tegetd",
    [MICROBLAZE_IT_TGETD_TYPEC2] = "tgetd",
    [MICROBLAZE_IT_TNAGETD_TYPEC2] = "tnagetd",
    [MICROBLAZE_IT_TNCAGETD_TYPEC2] = "tncagetd",
    [MICROBLAZE_IT_TNCGETD_TYPEC2] = "tncgetd",
    [MICROBLAZE_IT_TNEAGETD_TYPEC2] = "tneagetd",
    [MICROBLAZE_IT_TNECAGETD_TYPEC2] = "tnecagetd",
    [MICROBLAZE_IT_TNECGETD_TYPEC2] = "tnecgetd",
    [MICROBLAZE_IT_TNEGETD_TYPEC2] = "tnegetd",
    [MICROBLAZE_IT_TNGETD_TYPEC2] = "tngetd",

    //
    // Instruction Opcode TypeC3 Insns
    //
    [MICROBLAZE_IT_APUT_TYPEC3] = "aput",
    [MICROBLAZE_IT_CAPUT_TYPEC3] = "caput",
    [MICROBLAZE_IT_CPUT_TYPEC3] = "cput",
    [MICROBLAZE_IT_NAPUT_TYPEC3] = "naput",
    [MICROBLAZE_IT_NCAPUT_TYPEC3] = "ncaput",
    [MICROBLAZE_IT_NCPUT_TYPEC3] = "ncput",
    [MICROBLAZE_IT_NPUT_TYPEC3] = "nput",
    [MICROBLAZE_IT_PUT_TYPEC3] = "put",

    //
    // Instruction Opcode TypeC3d Insns
    //
    [MICROBLAZE_IT_APUTD_TYPEC3D] = "aputd",
    [MICROBLAZE_IT_CAPUTD_TYPEC3D] = "caputd",
    [MICROBLAZE_IT_CPUTD_TYPEC3D] = "cputd",
    [MICROBLAZE_IT_NAPUTD_TYPEC3D] = "naputd",
    [MICROBLAZE_IT_NCAPUTD_TYPEC3D] = "ncaputd",
    [MICROBLAZE_IT_NCPUTD_TYPEC3D] = "ncputd",
    [MICROBLAZE_IT_NPUTD_TYPEC3D] = "nputd",
    [MICROBLAZE_IT_PUTD_TYPEC3D] = "putd",

    //
    // Instruction Opcode TypeC4 Insns
    //
    [MICROBLAZE_IT_TAPUT_TYPEC4] = "taput",
    [MICROBLAZE_IT_TCAPUT_TYPEC4] = "tcaput",
    [MICROBLAZE_IT_TCPUT_TYPEC4] = "tcput",
    [MICROBLAZE_IT_TNAPUT_TYPEC4] = "tnaput",
    [MICROBLAZE_IT_TNCAPUT_TYPEC4] = "tncaput",
    [MICROBLAZE_IT_TNCPUT_TYPEC4] = "tncput",
    [MICROBLAZE_IT_TNPUT_TYPEC4] = "tnput",
    [MICROBLAZE_IT_TPUT_TYPEC4] = "tput",

    //
    // Instruction Opcode TypeC4d Insns
    //
    [MICROBLAZE_IT_TAPUTD_TYPEC4D] = "taputd",
    [MICROBLAZE_IT_TCAPUTD_TYPEC4D] = "tcaputd",
    [MICROBLAZE_IT_TCPUTD_TYPEC4D] = "tcputd",
    [MICROBLAZE_IT_TNAPUTD_TYPEC4D] = "tnaputd",
    [MICROBLAZE_IT_TNCAPUTD_TYPEC4D] = "tncaputd",
    [MICROBLAZE_IT_TNCPUTD_TYPEC4D] = "tncputd",
    [MICROBLAZE_IT_TNPUTD_TYPEC4D] = "tnputd",
    [MICROBLAZE_IT_TPUTD_TYPEC4D] = "tputd",

    //
    // Instruction Opcode TypeI1 Insns
    //
    [MICROBLAZE_IT_IMM_TYPEI1] = "imm",

    //
    // Instruction Opcode TypeX Insns
    //
    [MICROBLAZE_IT_NOP_TYPEX] = "nop",

    [MICROBLAZE_IT_LAST] = "UNDEFINED"
};

#define DECODE_ENTRY(_PRIORITY, _NAME, _FMT) \
    vmidNewEntryFmtBin( \
        table, \
        #_NAME, \
        MICROBLAZE_IT_##_NAME, \
        _FMT, \
        _PRIORITY \
    )

//
// Decoder table for 32 bit instructions
//
vmidDecodeTableP microblazeCreateDecodeTable32(void) {
    vmidDecodeTableP table = vmidNewDecodeTable(32, MICROBLAZE_IT_LAST);
    // format                  TypeA1   |opcode|   rD|   rA|   rB|     subop1|     // 32 bits
    DECODE_ENTRY(17,       ADD_TYPEA1, "|000000|.....|.....|.....|00000000000|");
    DECODE_ENTRY(17,      ADDC_TYPEA1, "|000010|.....|.....|.....|00000000000|");
    DECODE_ENTRY(17,      ADDK_TYPEA1, "|000100|.....|.....|.....|00000000000|");
    DECODE_ENTRY(17,     ADDKC_TYPEA1, "|000110|.....|.....|.....|00000000000|");
    DECODE_ENTRY(17,       AND_TYPEA1, "|100001|.....|.....|.....|00000000000|");
    DECODE_ENTRY(17,      ANDN_TYPEA1, "|100011|.....|.....|.....|00000000000|");
    DECODE_ENTRY(17,      FADD_TYPEA1, "|010110|.....|.....|.....|00000000000|");
    DECODE_ENTRY(17,    FCMPEQ_TYPEA1, "|010110|.....|.....|.....|01000100000|");
    DECODE_ENTRY(17,    FCMPGE_TYPEA1, "|010110|.....|.....|.....|01001100000|");
    DECODE_ENTRY(17,    FCMPGT_TYPEA1, "|010110|.....|.....|.....|01001000000|");
    DECODE_ENTRY(17,    FCMPLE_TYPEA1, "|010110|.....|.....|.....|01000110000|");
    DECODE_ENTRY(17,    FCMPLT_TYPEA1, "|010110|.....|.....|.....|01000010000|");
    DECODE_ENTRY(17,    FCMPNE_TYPEA1, "|010110|.....|.....|.....|01001010000|");
    DECODE_ENTRY(17,    FCMPUN_TYPEA1, "|010110|.....|.....|.....|01000000000|");
    DECODE_ENTRY(17,      FDIV_TYPEA1, "|010110|.....|.....|.....|00110000000|");
    DECODE_ENTRY(17,      FMUL_TYPEA1, "|010110|.....|.....|.....|00100000000|");
    DECODE_ENTRY(17,     FRSUB_TYPEA1, "|010110|.....|.....|.....|00010000000|");
    DECODE_ENTRY(17,       LWX_TYPEA1, "|110010|.....|.....|.....|10000000000|");
    DECODE_ENTRY(17,       MUL_TYPEA1, "|010000|.....|.....|.....|00000000000|");
    DECODE_ENTRY(17,      MULH_TYPEA1, "|010000|.....|.....|.....|00000000001|");
    DECODE_ENTRY(17,    MULHSU_TYPEA1, "|010000|.....|.....|.....|00000000010|");
    DECODE_ENTRY(17,     MULHU_TYPEA1, "|010000|.....|.....|.....|00000000011|");
    DECODE_ENTRY(17,        OR_TYPEA1, "|100000|.....|.....|.....|00000000000|");
    DECODE_ENTRY(17,    PCMPBF_TYPEA1, "|100000|.....|.....|.....|10000000000|");
    DECODE_ENTRY(17,    PCMPEQ_TYPEA1, "|100010|.....|.....|.....|10000000000|");
    DECODE_ENTRY(17,    PCMPNE_TYPEA1, "|100011|.....|.....|.....|10000000000|");
    DECODE_ENTRY(17,      RSUB_TYPEA1, "|000001|.....|.....|.....|00000000000|");
    DECODE_ENTRY(17,     RSUBC_TYPEA1, "|000011|.....|.....|.....|00000000000|");
    DECODE_ENTRY(17,     RSUBK_TYPEA1, "|000101|.....|.....|.....|00000000000|");
    DECODE_ENTRY(17,    RSUBKC_TYPEA1, "|000111|.....|.....|.....|00000000000|");
    DECODE_ENTRY(17,       SWX_TYPEA1, "|110110|.....|.....|.....|10000000000|");
    DECODE_ENTRY(17,       XOR_TYPEA1, "|100010|.....|.....|.....|00000000000|");

    // format                  TypeA2   |opcode|D|opco|   rA|   rB|     subop1|     // 32 bits
    DECODE_ENTRY(22,       BEQ_TYPEA2, "|100111|0|0000|.....|.....|00000000000|");
    DECODE_ENTRY(22,      BEQD_TYPEA2, "|100111|1|0000|.....|.....|00000000000|");
    DECODE_ENTRY(22,       BGE_TYPEA2, "|100111|0|0101|.....|.....|00000000000|");
    DECODE_ENTRY(22,      BGED_TYPEA2, "|100111|1|0101|.....|.....|00000000000|");
    DECODE_ENTRY(22,       BGT_TYPEA2, "|100111|0|0100|.....|.....|00000000000|");
    DECODE_ENTRY(22,      BGTD_TYPEA2, "|100111|1|0100|.....|.....|00000000000|");
    DECODE_ENTRY(22,       BLE_TYPEA2, "|100111|0|0011|.....|.....|00000000000|");
    DECODE_ENTRY(22,      BLED_TYPEA2, "|100111|1|0011|.....|.....|00000000000|");
    DECODE_ENTRY(22,       BLT_TYPEA2, "|100111|0|0010|.....|.....|00000000000|");
    DECODE_ENTRY(22,      BLTD_TYPEA2, "|100111|1|0010|.....|.....|00000000000|");
    DECODE_ENTRY(22,       BNE_TYPEA2, "|100111|0|0001|.....|.....|00000000000|");
    DECODE_ENTRY(22,      BNED_TYPEA2, "|100111|1|0001|.....|.....|00000000000|");

    // format                  TypeA3   |opcode|   rD|D|A| c2|   rB|     subop1|     // 32 bits
    DECODE_ENTRY(22,        BR_TYPEA3, "|100110|.....|0|0|000|.....|00000000000|");
    DECODE_ENTRY(22,       BRA_TYPEA3, "|100110|.....|0|1|000|.....|00000000000|");
    DECODE_ENTRY(22,      BRAD_TYPEA3, "|100110|.....|1|1|000|.....|00000000000|");
    DECODE_ENTRY(22,       BRD_TYPEA3, "|100110|.....|1|0|000|.....|00000000000|");

    // format                  TypeA4   |opcode|   rD|D|A| c2|   rB|     subop1|     // 32 bits
    DECODE_ENTRY(22,     BRALD_TYPEA4, "|100110|.....|1|1|100|.....|00000000000|");
    DECODE_ENTRY(22,      BRLD_TYPEA4, "|100110|.....|1|0|100|.....|00000000000|");

    // format                  TypeA5   |opcode|   rD|opcod|   rB|     subop1|     // 32 bits
    DECODE_ENTRY(22,       BRK_TYPEA5, "|100110|.....|01100|.....|00000000000|");

    // format                  TypeA6   |opcode|   rD|   rA|   rB|ST|   subop1|     // 32 bits
    DECODE_ENTRY(17,      BSLL_TYPEA6, "|010001|.....|.....|.....|10|000000000|");
    DECODE_ENTRY(17,      BSRA_TYPEA6, "|010001|.....|.....|.....|01|000000000|");
    DECODE_ENTRY(17,      BSRL_TYPEA6, "|010001|.....|.....|.....|00|000000000|");

    // format                  TypeA7   |opcode|   rD|   rA|   rB|   opcode|U|o|     // 32 bits
    DECODE_ENTRY(17,       CMP_TYPEA7, "|000101|.....|.....|.....|000000000|0|1|");
    DECODE_ENTRY(17,      CMPU_TYPEA7, "|000101|.....|.....|.....|000000000|1|1|");
    DECODE_ENTRY(17,      IDIV_TYPEA7, "|010010|.....|.....|.....|000000000|0|0|");
    DECODE_ENTRY(17,     IDIVU_TYPEA7, "|010010|.....|.....|.....|000000000|1|0|");

    // format                  TypeA8   |opcode|   rD|   rA|   rB|z|R|    zero1|     // 32 bits
    DECODE_ENTRY(17,       LBU_TYPEA8, "|110000|.....|.....|.....|0|0|000000000|");
    DECODE_ENTRY(17,      LBUR_TYPEA8, "|110000|.....|.....|.....|0|1|000000000|");
    DECODE_ENTRY(17,       LHU_TYPEA8, "|110001|.....|.....|.....|0|0|000000000|");
    DECODE_ENTRY(17,      LHUR_TYPEA8, "|110001|.....|.....|.....|0|1|000000000|");
    DECODE_ENTRY(17,        LW_TYPEA8, "|110010|.....|.....|.....|0|0|000000000|");
    DECODE_ENTRY(17,       LWR_TYPEA8, "|110010|.....|.....|.....|0|1|000000000|");
    DECODE_ENTRY(17,        SB_TYPEA8, "|110100|.....|.....|.....|0|0|000000000|");
    DECODE_ENTRY(17,       SBR_TYPEA8, "|110100|.....|.....|.....|0|1|000000000|");
    DECODE_ENTRY(17,        SH_TYPEA8, "|110101|.....|.....|.....|0|0|000000000|");
    DECODE_ENTRY(17,       SHR_TYPEA8, "|110101|.....|.....|.....|0|1|000000000|");
    DECODE_ENTRY(17,        SW_TYPEA8, "|110110|.....|.....|.....|0|0|000000000|");
    DECODE_ENTRY(17,       SWR_TYPEA8, "|110110|.....|.....|.....|0|1|000000000|");

    // format                  TypeA9   |opcode|   rD|zero1|su|            rS|     // 32 bits
    DECODE_ENTRY(13,       MFS_TYPEA9, "|100101|.....|00000|10|..............|");

    // format                  TypeAa   |opcode|zero1|   rA|su|            rS|     // 32 bits
    DECODE_ENTRY(13,       MTS_TYPEAA, "|100101|00000|.....|11|..............|");

    // format                  TypeAb   |opcode|   rD|   rA|          subop1|     // 32 bits
    DECODE_ENTRY(22,       CLZ_TYPEAB, "|100100|.....|.....|0000000011100000|");
    DECODE_ENTRY(22,      FINT_TYPEAB, "|010110|.....|.....|0000001100000000|");
    DECODE_ENTRY(22,       FLT_TYPEAB, "|010110|.....|.....|0000001010000000|");
    DECODE_ENTRY(22,     FSQRT_TYPEAB, "|010110|.....|.....|0000001110000000|");
    DECODE_ENTRY(22,    SEXT16_TYPEAB, "|100100|.....|.....|0000000001100001|");
    DECODE_ENTRY(22,     SEXT8_TYPEAB, "|100100|.....|.....|0000000001100000|");
    DECODE_ENTRY(22,       SRA_TYPEAB, "|100100|.....|.....|0000000000000001|");
    DECODE_ENTRY(22,       SRC_TYPEAB, "|100100|.....|.....|0000000000100001|");
    DECODE_ENTRY(22,       SRL_TYPEAB, "|100100|.....|.....|0000000001000001|");

    // format                  TypeAc   |opcode|zero1|   rA|   rB|     subop1|     // 32 bits
    DECODE_ENTRY(22,       WDC_TYPEAC, "|100100|00000|.....|.....|00001100100|");
    DECODE_ENTRY(22,    WDCCLR_TYPEAC, "|100100|00000|.....|.....|00001100110|");
    DECODE_ENTRY(22,     WDCFL_TYPEAC, "|100100|00000|.....|.....|00001110100|");
    DECODE_ENTRY(22,       WIC_TYPEAC, "|100100|00000|.....|.....|00001101000|");

    // format                  TypeB1   |opcode|   rD|   rA|            simm|     // 32 bits
    DECODE_ENTRY( 6,      ADDI_TYPEB1, "|001000|.....|.....|................|");
    DECODE_ENTRY( 6,     ADDIC_TYPEB1, "|001010|.....|.....|................|");
    DECODE_ENTRY( 6,     ADDIK_TYPEB1, "|001100|.....|.....|................|");
    DECODE_ENTRY( 6,    ADDIKC_TYPEB1, "|001110|.....|.....|................|");
    DECODE_ENTRY( 6,      ANDI_TYPEB1, "|101001|.....|.....|................|");
    DECODE_ENTRY( 6,     ANDNI_TYPEB1, "|101011|.....|.....|................|");
    DECODE_ENTRY( 6,      LBUI_TYPEB1, "|111000|.....|.....|................|");
    DECODE_ENTRY( 6,      LHUI_TYPEB1, "|111001|.....|.....|................|");
    DECODE_ENTRY( 6,       LWI_TYPEB1, "|111010|.....|.....|................|");
    DECODE_ENTRY( 6,      MULI_TYPEB1, "|011000|.....|.....|................|");
    DECODE_ENTRY( 6,       ORI_TYPEB1, "|101000|.....|.....|................|");
    DECODE_ENTRY( 6,     RSUBI_TYPEB1, "|001001|.....|.....|................|");
    DECODE_ENTRY( 6,    RSUBIC_TYPEB1, "|001011|.....|.....|................|");
    DECODE_ENTRY( 6,    RSUBIK_TYPEB1, "|001101|.....|.....|................|");
    DECODE_ENTRY( 6,   RSUBIKC_TYPEB1, "|001111|.....|.....|................|");
    DECODE_ENTRY( 6,       SBI_TYPEB1, "|111100|.....|.....|................|");
    DECODE_ENTRY( 6,       SHI_TYPEB1, "|111101|.....|.....|................|");
    DECODE_ENTRY( 6,       SWI_TYPEB1, "|111110|.....|.....|................|");
    DECODE_ENTRY( 6,      XORI_TYPEB1, "|101010|.....|.....|................|");

    // format                  TypeB2   |opcode|D|opco|   rA|            simm|     // 32 bits
    DECODE_ENTRY(11,      BEQI_TYPEB2, "|101111|0|0000|.....|................|");
    DECODE_ENTRY(11,     BEQID_TYPEB2, "|101111|1|0000|.....|................|");
    DECODE_ENTRY(11,      BGEI_TYPEB2, "|101111|0|0101|.....|................|");
    DECODE_ENTRY(11,     BGEID_TYPEB2, "|101111|1|0101|.....|................|");
    DECODE_ENTRY(11,      BGTI_TYPEB2, "|101111|0|0100|.....|................|");
    DECODE_ENTRY(11,     BGTID_TYPEB2, "|101111|1|0100|.....|................|");
    DECODE_ENTRY(11,      BLEI_TYPEB2, "|101111|0|0011|.....|................|");
    DECODE_ENTRY(11,     BLEID_TYPEB2, "|101111|1|0011|.....|................|");
    DECODE_ENTRY(11,      BLTI_TYPEB2, "|101111|0|0010|.....|................|");
    DECODE_ENTRY(11,     BLTID_TYPEB2, "|101111|1|0010|.....|................|");
    DECODE_ENTRY(11,      BNEI_TYPEB2, "|101111|0|0001|.....|................|");
    DECODE_ENTRY(11,     BNEID_TYPEB2, "|101111|1|0001|.....|................|");

    // format                  TypeB3   |opcode|   rD|D|A| c2|            simm|     // 32 bits
    DECODE_ENTRY(11,      BRAI_TYPEB3, "|101110|.....|0|1|000|................|");
    DECODE_ENTRY(11,     BRAID_TYPEB3, "|101110|.....|1|1|000|................|");
    DECODE_ENTRY(11,       BRI_TYPEB3, "|101110|.....|0|0|000|................|");
    DECODE_ENTRY(11,      BRID_TYPEB3, "|101110|.....|1|0|000|................|");

    // format                  TypeB4   |opcode|   rD|D|A| c2|            simm|     // 32 bits
    DECODE_ENTRY(11,    BRALID_TYPEB4, "|101110|.....|1|1|100|................|");
    DECODE_ENTRY(11,     BRLID_TYPEB4, "|101110|.....|1|0|100|................|");

    // format                  TypeB5   |opcode|   rD|opcod|            simm|     // 32 bits
    DECODE_ENTRY(11,      BRKI_TYPEB5, "|101110|.....|01100|................|");

    // format                  TypeB6   |opcode|   rD|   rA|   c2|ST|subo| uimm|     // 32 bits
    DECODE_ENTRY(17,     BSLLI_TYPEB6, "|011001|.....|.....|00000|10|0000|.....|");
    DECODE_ENTRY(17,     BSRAI_TYPEB6, "|011001|.....|.....|00000|01|0000|.....|");
    DECODE_ENTRY(17,     BSRLI_TYPEB6, "|011001|.....|.....|00000|00|0000|.....|");

    // format                  TypeB8   |opcode| simm|subop|          subop1|     // 32 bits
    DECODE_ENTRY(27,      MBAR_TYPEB8, "|101110|.....|00010|0000000000000100|");

    // format                  TypeB9   |opcode|   rD|subop1|           uimm|     // 32 bits
    DECODE_ENTRY(12,    MSRCLR_TYPEB9, "|100101|.....|100010|...............|");
    DECODE_ENTRY(12,    MSRSET_TYPEB9, "|100101|.....|100000|...............|");

    // format                  TypeBa   |opcode|subop|   rA|            simm|     // 32 bits
    DECODE_ENTRY(11,      RTBD_TYPEBA, "|101101|10010|.....|................|");
    DECODE_ENTRY(11,      RTED_TYPEBA, "|101101|10100|.....|................|");
    DECODE_ENTRY(11,      RTID_TYPEBA, "|101101|10001|.....|................|");
    DECODE_ENTRY(11,      RTSD_TYPEBA, "|101101|10000|.....|................|");

    // format                  TypeC1   |opcode|   rD| zero1|nctae| zero1|FSLx|     // 32 bits
    DECODE_ENTRY(23,      AGET_TYPEC1, "|011011|.....|000000|00010|000000|....|");
    DECODE_ENTRY(23,     CAGET_TYPEC1, "|011011|.....|000000|01010|000000|....|");
    DECODE_ENTRY(23,      CGET_TYPEC1, "|011011|.....|000000|01000|000000|....|");
    DECODE_ENTRY(23,     EAGET_TYPEC1, "|011011|.....|000000|00011|000000|....|");
    DECODE_ENTRY(23,    ECAGET_TYPEC1, "|011011|.....|000000|01011|000000|....|");
    DECODE_ENTRY(23,     ECGET_TYPEC1, "|011011|.....|000000|01001|000000|....|");
    DECODE_ENTRY(23,      EGET_TYPEC1, "|011011|.....|000000|00001|000000|....|");
    DECODE_ENTRY(23,       GET_TYPEC1, "|011011|.....|000000|00000|000000|....|");
    DECODE_ENTRY(23,     NAGET_TYPEC1, "|011011|.....|000000|10010|000000|....|");
    DECODE_ENTRY(23,    NCAGET_TYPEC1, "|011011|.....|000000|11010|000000|....|");
    DECODE_ENTRY(23,     NCGET_TYPEC1, "|011011|.....|000000|11000|000000|....|");
    DECODE_ENTRY(23,    NEAGET_TYPEC1, "|011011|.....|000000|10011|000000|....|");
    DECODE_ENTRY(23,   NECAGET_TYPEC1, "|011011|.....|000000|11011|000000|....|");
    DECODE_ENTRY(23,    NECGET_TYPEC1, "|011011|.....|000000|11001|000000|....|");
    DECODE_ENTRY(23,     NEGET_TYPEC1, "|011011|.....|000000|10001|000000|....|");
    DECODE_ENTRY(23,      NGET_TYPEC1, "|011011|.....|000000|10000|000000|....|");
    DECODE_ENTRY(23,     TAGET_TYPEC1, "|011011|.....|000000|00110|000000|....|");
    DECODE_ENTRY(23,    TCAGET_TYPEC1, "|011011|.....|000000|01110|000000|....|");
    DECODE_ENTRY(23,     TCGET_TYPEC1, "|011011|.....|000000|01100|000000|....|");
    DECODE_ENTRY(23,    TEAGET_TYPEC1, "|011011|.....|000000|00111|000000|....|");
    DECODE_ENTRY(23,   TECAGET_TYPEC1, "|011011|.....|000000|01111|000000|....|");
    DECODE_ENTRY(23,    TECGET_TYPEC1, "|011011|.....|000000|01101|000000|....|");
    DECODE_ENTRY(23,     TEGET_TYPEC1, "|011011|.....|000000|00101|000000|....|");
    DECODE_ENTRY(23,      TGET_TYPEC1, "|011011|.....|000000|00100|000000|....|");
    DECODE_ENTRY(23,    TNAGET_TYPEC1, "|011011|.....|000000|10110|000000|....|");
    DECODE_ENTRY(23,   TNCAGET_TYPEC1, "|011011|.....|000000|11110|000000|....|");
    DECODE_ENTRY(23,    TNCGET_TYPEC1, "|011011|.....|000000|11100|000000|....|");
    DECODE_ENTRY(23,   TNEAGET_TYPEC1, "|011011|.....|000000|10111|000000|....|");
    DECODE_ENTRY(23,  TNECAGET_TYPEC1, "|011011|.....|000000|11111|000000|....|");
    DECODE_ENTRY(23,   TNECGET_TYPEC1, "|011011|.....|000000|11101|000000|....|");
    DECODE_ENTRY(23,    TNEGET_TYPEC1, "|011011|.....|000000|10101|000000|....|");
    DECODE_ENTRY(23,     TNGET_TYPEC1, "|011011|.....|000000|10100|000000|....|");

    // format                  TypeC2   |opcode|   rD|zero1|   rB|z|nctae|zero1|     // 32 bits
    DECODE_ENTRY(22,     AGETD_TYPEC2, "|010011|.....|00000|.....|0|00010|00000|");
    DECODE_ENTRY(22,    CAGETD_TYPEC2, "|010011|.....|00000|.....|0|01010|00000|");
    DECODE_ENTRY(22,     CGETD_TYPEC2, "|010011|.....|00000|.....|0|01000|00000|");
    DECODE_ENTRY(22,    EAGETD_TYPEC2, "|010011|.....|00000|.....|0|00011|00000|");
    DECODE_ENTRY(22,   ECAGETD_TYPEC2, "|010011|.....|00000|.....|0|01011|00000|");
    DECODE_ENTRY(22,    ECGETD_TYPEC2, "|010011|.....|00000|.....|0|01001|00000|");
    DECODE_ENTRY(22,     EGETD_TYPEC2, "|010011|.....|00000|.....|0|00001|00000|");
    DECODE_ENTRY(22,      GETD_TYPEC2, "|010011|.....|00000|.....|0|00000|00000|");
    DECODE_ENTRY(22,    NAGETD_TYPEC2, "|010011|.....|00000|.....|0|10010|00000|");
    DECODE_ENTRY(22,   NCAGETD_TYPEC2, "|010011|.....|00000|.....|0|11010|00000|");
    DECODE_ENTRY(22,    NCGETD_TYPEC2, "|010011|.....|00000|.....|0|11000|00000|");
    DECODE_ENTRY(22,   NEAGETD_TYPEC2, "|010011|.....|00000|.....|0|10011|00000|");
    DECODE_ENTRY(22,  NECAGETD_TYPEC2, "|010011|.....|00000|.....|0|11011|00000|");
    DECODE_ENTRY(22,   NECGETD_TYPEC2, "|010011|.....|00000|.....|0|11001|00000|");
    DECODE_ENTRY(22,    NEGETD_TYPEC2, "|010011|.....|00000|.....|0|10001|00000|");
    DECODE_ENTRY(22,     NGETD_TYPEC2, "|010011|.....|00000|.....|0|10000|00000|");
    DECODE_ENTRY(22,    TAGETD_TYPEC2, "|010011|.....|00000|.....|0|00110|00000|");
    DECODE_ENTRY(22,   TCAGETD_TYPEC2, "|010011|.....|00000|.....|0|01110|00000|");
    DECODE_ENTRY(22,    TCGETD_TYPEC2, "|010011|.....|00000|.....|0|01100|00000|");
    DECODE_ENTRY(22,   TEAGETD_TYPEC2, "|010011|.....|00000|.....|0|00111|00000|");
    DECODE_ENTRY(22,  TECAGETD_TYPEC2, "|010011|.....|00000|.....|0|01111|00000|");
    DECODE_ENTRY(22,   TECGETD_TYPEC2, "|010011|.....|00000|.....|0|01101|00000|");
    DECODE_ENTRY(22,    TEGETD_TYPEC2, "|010011|.....|00000|.....|0|00101|00000|");
    DECODE_ENTRY(22,     TGETD_TYPEC2, "|010011|.....|00000|.....|0|00100|00000|");
    DECODE_ENTRY(22,   TNAGETD_TYPEC2, "|010011|.....|00000|.....|0|10110|00000|");
    DECODE_ENTRY(22,  TNCAGETD_TYPEC2, "|010011|.....|00000|.....|0|11110|00000|");
    DECODE_ENTRY(22,   TNCGETD_TYPEC2, "|010011|.....|00000|.....|0|11100|00000|");
    DECODE_ENTRY(22,  TNEAGETD_TYPEC2, "|010011|.....|00000|.....|0|10111|00000|");
    DECODE_ENTRY(22, TNECAGETD_TYPEC2, "|010011|.....|00000|.....|0|11111|00000|");
    DECODE_ENTRY(22,  TNECGETD_TYPEC2, "|010011|.....|00000|.....|0|11101|00000|");
    DECODE_ENTRY(22,   TNEGETD_TYPEC2, "|010011|.....|00000|.....|0|10101|00000|");
    DECODE_ENTRY(22,    TNGETD_TYPEC2, "|010011|.....|00000|.....|0|10100|00000|");

    // format                  TypeC3   |opcode|init1|   rA|i|ncta|  init1|FSLx|     // 32 bits
    DECODE_ENTRY(23,      APUT_TYPEC3, "|011011|00000|.....|1|0001|0000000|....|");
    DECODE_ENTRY(23,     CAPUT_TYPEC3, "|011011|00000|.....|1|0101|0000000|....|");
    DECODE_ENTRY(23,      CPUT_TYPEC3, "|011011|00000|.....|1|0100|0000000|....|");
    DECODE_ENTRY(23,     NAPUT_TYPEC3, "|011011|00000|.....|1|1001|0000000|....|");
    DECODE_ENTRY(23,    NCAPUT_TYPEC3, "|011011|00000|.....|1|1101|0000000|....|");
    DECODE_ENTRY(23,     NCPUT_TYPEC3, "|011011|00000|.....|1|1100|0000000|....|");
    DECODE_ENTRY(23,      NPUT_TYPEC3, "|011011|00000|.....|1|1000|0000000|....|");
    DECODE_ENTRY(23,       PUT_TYPEC3, "|011011|00000|.....|1|0000|0000000|....|");

    // format                 TypeC3d   |opcode|zero1|   rA|   rB|z|ncta| zero1|     // 32 bits
    DECODE_ENTRY(22,    APUTD_TYPEC3D, "|010011|00000|.....|.....|1|0001|000000|");
    DECODE_ENTRY(22,   CAPUTD_TYPEC3D, "|010011|00000|.....|.....|1|0101|000000|");
    DECODE_ENTRY(22,    CPUTD_TYPEC3D, "|010011|00000|.....|.....|1|0100|000000|");
    DECODE_ENTRY(22,   NAPUTD_TYPEC3D, "|010011|00000|.....|.....|1|1001|000000|");
    DECODE_ENTRY(22,  NCAPUTD_TYPEC3D, "|010011|00000|.....|.....|1|1101|000000|");
    DECODE_ENTRY(22,   NCPUTD_TYPEC3D, "|010011|00000|.....|.....|1|1100|000000|");
    DECODE_ENTRY(22,    NPUTD_TYPEC3D, "|010011|00000|.....|.....|1|1000|000000|");
    DECODE_ENTRY(22,     PUTD_TYPEC3D, "|010011|00000|.....|.....|1|0000|000000|");

    // format                  TypeC4   |opcode|init1|   rA|i|ncta|  init1|FSLx|     // 32 bits
    DECODE_ENTRY(23,     TAPUT_TYPEC4, "|011011|00000|.....|1|0011|0000000|....|");
    DECODE_ENTRY(23,    TCAPUT_TYPEC4, "|011011|00000|.....|1|0111|0000000|....|");
    DECODE_ENTRY(23,     TCPUT_TYPEC4, "|011011|00000|.....|1|0110|0000000|....|");
    DECODE_ENTRY(23,    TNAPUT_TYPEC4, "|011011|00000|.....|1|1011|0000000|....|");
    DECODE_ENTRY(23,   TNCAPUT_TYPEC4, "|011011|00000|.....|1|1111|0000000|....|");
    DECODE_ENTRY(23,    TNCPUT_TYPEC4, "|011011|00000|.....|1|1110|0000000|....|");
    DECODE_ENTRY(23,     TNPUT_TYPEC4, "|011011|00000|.....|1|1010|0000000|....|");
    DECODE_ENTRY(23,      TPUT_TYPEC4, "|011011|00000|.....|1|0010|0000000|....|");

    // format                 TypeC4d   |opcode|zero1|   rA|   rB|z|ncta| zero1|     // 32 bits
    DECODE_ENTRY(22,   TAPUTD_TYPEC4D, "|010011|00000|.....|.....|1|0011|000000|");
    DECODE_ENTRY(22,  TCAPUTD_TYPEC4D, "|010011|00000|.....|.....|1|0111|000000|");
    DECODE_ENTRY(22,   TCPUTD_TYPEC4D, "|010011|00000|.....|.....|1|0110|000000|");
    DECODE_ENTRY(22,  TNAPUTD_TYPEC4D, "|010011|00000|.....|.....|1|1011|000000|");
    DECODE_ENTRY(22, TNCAPUTD_TYPEC4D, "|010011|00000|.....|.....|1|1111|000000|");
    DECODE_ENTRY(22,  TNCPUTD_TYPEC4D, "|010011|00000|.....|.....|1|1110|000000|");
    DECODE_ENTRY(22,   TNPUTD_TYPEC4D, "|010011|00000|.....|.....|1|1010|000000|");
    DECODE_ENTRY(22,    TPUTD_TYPEC4D, "|010011|00000|.....|.....|1|0010|000000|");

    // format                  TypeI1   |opcode|     zero1|          simmhi|     // 32 bits
    DECODE_ENTRY(16,       IMM_TYPEI1, "|101100|0000000000|................|");

    // format                   TypeX   |                          opcode|     // 32 bits
    DECODE_ENTRY(32,        NOP_TYPEX, "|10000000000000000000000000000000|");

    //
    // Drop through section to next table
    //
    return table;
}

//
// Initialization of the info structure per instruction
//
void microblazeGenInstructionInfo (microblazeInstructionInfoP info, Uns64 instruction) {
    info->opcode = opcode[info->type];
    switch (info->type) {
        case MICROBLAZE_IT_ADD_TYPEA1:
        case MICROBLAZE_IT_ADDC_TYPEA1:
        case MICROBLAZE_IT_ADDK_TYPEA1:
        case MICROBLAZE_IT_ADDKC_TYPEA1:
        case MICROBLAZE_IT_AND_TYPEA1:
        case MICROBLAZE_IT_ANDN_TYPEA1:
        case MICROBLAZE_IT_FADD_TYPEA1:
        case MICROBLAZE_IT_FCMPEQ_TYPEA1:
        case MICROBLAZE_IT_FCMPGE_TYPEA1:
        case MICROBLAZE_IT_FCMPGT_TYPEA1:
        case MICROBLAZE_IT_FCMPLE_TYPEA1:
        case MICROBLAZE_IT_FCMPLT_TYPEA1:
        case MICROBLAZE_IT_FCMPNE_TYPEA1:
        case MICROBLAZE_IT_FCMPUN_TYPEA1:
        case MICROBLAZE_IT_FDIV_TYPEA1:
        case MICROBLAZE_IT_FMUL_TYPEA1:
        case MICROBLAZE_IT_FRSUB_TYPEA1:
        case MICROBLAZE_IT_LWX_TYPEA1:
        case MICROBLAZE_IT_MUL_TYPEA1:
        case MICROBLAZE_IT_MULH_TYPEA1:
        case MICROBLAZE_IT_MULHSU_TYPEA1:
        case MICROBLAZE_IT_MULHU_TYPEA1:
        case MICROBLAZE_IT_OR_TYPEA1:
        case MICROBLAZE_IT_PCMPBF_TYPEA1:
        case MICROBLAZE_IT_PCMPEQ_TYPEA1:
        case MICROBLAZE_IT_PCMPNE_TYPEA1:
        case MICROBLAZE_IT_RSUB_TYPEA1:
        case MICROBLAZE_IT_RSUBC_TYPEA1:
        case MICROBLAZE_IT_RSUBK_TYPEA1:
        case MICROBLAZE_IT_RSUBKC_TYPEA1:
        case MICROBLAZE_IT_SWX_TYPEA1:
        case MICROBLAZE_IT_XOR_TYPEA1:
            info->rA = VECINDEX(instruction,20,16,0); // 20:16
            info->rB = VECINDEX(instruction,15,11,0); // 15:11
            info->rD = VECINDEX(instruction,25,21,0); // 25:21
            break;

        case MICROBLAZE_IT_BEQ_TYPEA2:
        case MICROBLAZE_IT_BEQD_TYPEA2:
        case MICROBLAZE_IT_BGE_TYPEA2:
        case MICROBLAZE_IT_BGED_TYPEA2:
        case MICROBLAZE_IT_BGT_TYPEA2:
        case MICROBLAZE_IT_BGTD_TYPEA2:
        case MICROBLAZE_IT_BLE_TYPEA2:
        case MICROBLAZE_IT_BLED_TYPEA2:
        case MICROBLAZE_IT_BLT_TYPEA2:
        case MICROBLAZE_IT_BLTD_TYPEA2:
        case MICROBLAZE_IT_BNE_TYPEA2:
        case MICROBLAZE_IT_BNED_TYPEA2:
            info->rA = VECINDEX(instruction,20,16,0); // 20:16
            info->rB = VECINDEX(instruction,15,11,0); // 15:11
            break;

        case MICROBLAZE_IT_BR_TYPEA3:
        case MICROBLAZE_IT_BRA_TYPEA3:
        case MICROBLAZE_IT_BRAD_TYPEA3:
        case MICROBLAZE_IT_BRD_TYPEA3:
            info->rB = VECINDEX(instruction,15,11,0); // 15:11
            info->rD = VECINDEX(instruction,25,21,0); // 25:21
            break;

        case MICROBLAZE_IT_BRALD_TYPEA4:
        case MICROBLAZE_IT_BRLD_TYPEA4:
            info->rB = VECINDEX(instruction,15,11,0); // 15:11
            info->rD = VECINDEX(instruction,25,21,0); // 25:21
            break;

        case MICROBLAZE_IT_BRK_TYPEA5:
            info->rB = VECINDEX(instruction,15,11,0); // 15:11
            info->rD = VECINDEX(instruction,25,21,0); // 25:21
            break;

        case MICROBLAZE_IT_BSLL_TYPEA6:
        case MICROBLAZE_IT_BSRA_TYPEA6:
        case MICROBLAZE_IT_BSRL_TYPEA6:
            info->rA = VECINDEX(instruction,20,16,0); // 20:16
            info->rB = VECINDEX(instruction,15,11,0); // 15:11
            info->rD = VECINDEX(instruction,25,21,0); // 25:21
            break;

        case MICROBLAZE_IT_CMP_TYPEA7:
        case MICROBLAZE_IT_CMPU_TYPEA7:
        case MICROBLAZE_IT_IDIV_TYPEA7:
        case MICROBLAZE_IT_IDIVU_TYPEA7:
            info->rA = VECINDEX(instruction,20,16,0); // 20:16
            info->rB = VECINDEX(instruction,15,11,0); // 15:11
            info->rD = VECINDEX(instruction,25,21,0); // 25:21
            break;

        case MICROBLAZE_IT_LBU_TYPEA8:
        case MICROBLAZE_IT_LBUR_TYPEA8:
        case MICROBLAZE_IT_LHU_TYPEA8:
        case MICROBLAZE_IT_LHUR_TYPEA8:
        case MICROBLAZE_IT_LW_TYPEA8:
        case MICROBLAZE_IT_LWR_TYPEA8:
        case MICROBLAZE_IT_SB_TYPEA8:
        case MICROBLAZE_IT_SBR_TYPEA8:
        case MICROBLAZE_IT_SH_TYPEA8:
        case MICROBLAZE_IT_SHR_TYPEA8:
        case MICROBLAZE_IT_SW_TYPEA8:
        case MICROBLAZE_IT_SWR_TYPEA8:
            info->rA = VECINDEX(instruction,20,16,0); // 20:16
            info->rB = VECINDEX(instruction,15,11,0); // 15:11
            info->rD = VECINDEX(instruction,25,21,0); // 25:21
            break;

        case MICROBLAZE_IT_MFS_TYPEA9:
            info->rD = VECINDEX(instruction,25,21,0); // 25:21
            info->rS = VECINDEX(instruction,13,0,0); // 13:0
            break;

        case MICROBLAZE_IT_MTS_TYPEAA:
            info->rA = VECINDEX(instruction,20,16,0); // 20:16
            info->rS = VECINDEX(instruction,13,0,0); // 13:0
            break;

        case MICROBLAZE_IT_CLZ_TYPEAB:
        case MICROBLAZE_IT_FINT_TYPEAB:
        case MICROBLAZE_IT_FLT_TYPEAB:
        case MICROBLAZE_IT_FSQRT_TYPEAB:
        case MICROBLAZE_IT_SEXT16_TYPEAB:
        case MICROBLAZE_IT_SEXT8_TYPEAB:
        case MICROBLAZE_IT_SRA_TYPEAB:
        case MICROBLAZE_IT_SRC_TYPEAB:
        case MICROBLAZE_IT_SRL_TYPEAB:
            info->rA = VECINDEX(instruction,20,16,0); // 20:16
            info->rD = VECINDEX(instruction,25,21,0); // 25:21
            break;

        case MICROBLAZE_IT_WDC_TYPEAC:
        case MICROBLAZE_IT_WDCCLR_TYPEAC:
        case MICROBLAZE_IT_WDCFL_TYPEAC:
        case MICROBLAZE_IT_WIC_TYPEAC:
            info->rA = VECINDEX(instruction,20,16,0); // 20:16
            info->rB = VECINDEX(instruction,15,11,0); // 15:11
            break;

        case MICROBLAZE_IT_ADDI_TYPEB1:
        case MICROBLAZE_IT_ADDIC_TYPEB1:
        case MICROBLAZE_IT_ADDIK_TYPEB1:
        case MICROBLAZE_IT_ADDIKC_TYPEB1:
        case MICROBLAZE_IT_ANDI_TYPEB1:
        case MICROBLAZE_IT_ANDNI_TYPEB1:
        case MICROBLAZE_IT_LBUI_TYPEB1:
        case MICROBLAZE_IT_LHUI_TYPEB1:
        case MICROBLAZE_IT_LWI_TYPEB1:
        case MICROBLAZE_IT_MULI_TYPEB1:
        case MICROBLAZE_IT_ORI_TYPEB1:
        case MICROBLAZE_IT_RSUBI_TYPEB1:
        case MICROBLAZE_IT_RSUBIC_TYPEB1:
        case MICROBLAZE_IT_RSUBIK_TYPEB1:
        case MICROBLAZE_IT_RSUBIKC_TYPEB1:
        case MICROBLAZE_IT_SBI_TYPEB1:
        case MICROBLAZE_IT_SHI_TYPEB1:
        case MICROBLAZE_IT_SWI_TYPEB1:
        case MICROBLAZE_IT_XORI_TYPEB1:
            info->rA = VECINDEX(instruction,20,16,0); // 20:16
            info->rD = VECINDEX(instruction,25,21,0); // 25:21
            info->simm = VECINDEX(instruction,15,0,0); // 15:0
            info->simm = SEXTEND(info->simm,15);
            break;

        case MICROBLAZE_IT_BEQI_TYPEB2:
        case MICROBLAZE_IT_BEQID_TYPEB2:
        case MICROBLAZE_IT_BGEI_TYPEB2:
        case MICROBLAZE_IT_BGEID_TYPEB2:
        case MICROBLAZE_IT_BGTI_TYPEB2:
        case MICROBLAZE_IT_BGTID_TYPEB2:
        case MICROBLAZE_IT_BLEI_TYPEB2:
        case MICROBLAZE_IT_BLEID_TYPEB2:
        case MICROBLAZE_IT_BLTI_TYPEB2:
        case MICROBLAZE_IT_BLTID_TYPEB2:
        case MICROBLAZE_IT_BNEI_TYPEB2:
        case MICROBLAZE_IT_BNEID_TYPEB2:
            info->rA = VECINDEX(instruction,20,16,0); // 20:16
            info->simm = VECINDEX(instruction,15,0,0); // 15:0
            info->simm = SEXTEND(info->simm,15);
            break;

        case MICROBLAZE_IT_BRAI_TYPEB3:
        case MICROBLAZE_IT_BRAID_TYPEB3:
        case MICROBLAZE_IT_BRI_TYPEB3:
        case MICROBLAZE_IT_BRID_TYPEB3:
            info->rD = VECINDEX(instruction,25,21,0); // 25:21
            info->simm = VECINDEX(instruction,15,0,0); // 15:0
            info->simm = SEXTEND(info->simm,15);
            break;

        case MICROBLAZE_IT_BRALID_TYPEB4:
        case MICROBLAZE_IT_BRLID_TYPEB4:
            info->rD = VECINDEX(instruction,25,21,0); // 25:21
            info->simm = VECINDEX(instruction,15,0,0); // 15:0
            info->simm = SEXTEND(info->simm,15);
            break;

        case MICROBLAZE_IT_BRKI_TYPEB5:
            info->rD = VECINDEX(instruction,25,21,0); // 25:21
            info->simm = VECINDEX(instruction,15,0,0); // 15:0
            info->simm = SEXTEND(info->simm,15);
            break;

        case MICROBLAZE_IT_BSLLI_TYPEB6:
        case MICROBLAZE_IT_BSRAI_TYPEB6:
        case MICROBLAZE_IT_BSRLI_TYPEB6:
            info->rA = VECINDEX(instruction,20,16,0); // 20:16
            info->rD = VECINDEX(instruction,25,21,0); // 25:21
            info->uimm = VECINDEX(instruction,4,0,0); // 4:0
            break;

        case MICROBLAZE_IT_MBAR_TYPEB8:
            info->simm = VECINDEX(instruction,25,21,0); // 25:21
            info->simm = SEXTEND(info->simm,4);
            break;

        case MICROBLAZE_IT_MSRCLR_TYPEB9:
        case MICROBLAZE_IT_MSRSET_TYPEB9:
            info->rD = VECINDEX(instruction,25,21,0); // 25:21
            info->uimm = VECINDEX(instruction,14,0,0); // 14:0
            break;

        case MICROBLAZE_IT_RTBD_TYPEBA:
        case MICROBLAZE_IT_RTED_TYPEBA:
        case MICROBLAZE_IT_RTID_TYPEBA:
        case MICROBLAZE_IT_RTSD_TYPEBA:
            info->rA = VECINDEX(instruction,20,16,0); // 20:16
            info->simm = VECINDEX(instruction,15,0,0); // 15:0
            info->simm = SEXTEND(info->simm,15);
            break;

        case MICROBLAZE_IT_AGET_TYPEC1:
        case MICROBLAZE_IT_CAGET_TYPEC1:
        case MICROBLAZE_IT_CGET_TYPEC1:
        case MICROBLAZE_IT_EAGET_TYPEC1:
        case MICROBLAZE_IT_ECAGET_TYPEC1:
        case MICROBLAZE_IT_ECGET_TYPEC1:
        case MICROBLAZE_IT_EGET_TYPEC1:
        case MICROBLAZE_IT_GET_TYPEC1:
        case MICROBLAZE_IT_NAGET_TYPEC1:
        case MICROBLAZE_IT_NCAGET_TYPEC1:
        case MICROBLAZE_IT_NCGET_TYPEC1:
        case MICROBLAZE_IT_NEAGET_TYPEC1:
        case MICROBLAZE_IT_NECAGET_TYPEC1:
        case MICROBLAZE_IT_NECGET_TYPEC1:
        case MICROBLAZE_IT_NEGET_TYPEC1:
        case MICROBLAZE_IT_NGET_TYPEC1:
        case MICROBLAZE_IT_TAGET_TYPEC1:
        case MICROBLAZE_IT_TCAGET_TYPEC1:
        case MICROBLAZE_IT_TCGET_TYPEC1:
        case MICROBLAZE_IT_TEAGET_TYPEC1:
        case MICROBLAZE_IT_TECAGET_TYPEC1:
        case MICROBLAZE_IT_TECGET_TYPEC1:
        case MICROBLAZE_IT_TEGET_TYPEC1:
        case MICROBLAZE_IT_TGET_TYPEC1:
        case MICROBLAZE_IT_TNAGET_TYPEC1:
        case MICROBLAZE_IT_TNCAGET_TYPEC1:
        case MICROBLAZE_IT_TNCGET_TYPEC1:
        case MICROBLAZE_IT_TNEAGET_TYPEC1:
        case MICROBLAZE_IT_TNECAGET_TYPEC1:
        case MICROBLAZE_IT_TNECGET_TYPEC1:
        case MICROBLAZE_IT_TNEGET_TYPEC1:
        case MICROBLAZE_IT_TNGET_TYPEC1:
            info->FSLx = VECINDEX(instruction,3,0,0); // 3:0
            info->rD = VECINDEX(instruction,25,21,0); // 25:21
            break;

        case MICROBLAZE_IT_AGETD_TYPEC2:
        case MICROBLAZE_IT_CAGETD_TYPEC2:
        case MICROBLAZE_IT_CGETD_TYPEC2:
        case MICROBLAZE_IT_EAGETD_TYPEC2:
        case MICROBLAZE_IT_ECAGETD_TYPEC2:
        case MICROBLAZE_IT_ECGETD_TYPEC2:
        case MICROBLAZE_IT_EGETD_TYPEC2:
        case MICROBLAZE_IT_GETD_TYPEC2:
        case MICROBLAZE_IT_NAGETD_TYPEC2:
        case MICROBLAZE_IT_NCAGETD_TYPEC2:
        case MICROBLAZE_IT_NCGETD_TYPEC2:
        case MICROBLAZE_IT_NEAGETD_TYPEC2:
        case MICROBLAZE_IT_NECAGETD_TYPEC2:
        case MICROBLAZE_IT_NECGETD_TYPEC2:
        case MICROBLAZE_IT_NEGETD_TYPEC2:
        case MICROBLAZE_IT_NGETD_TYPEC2:
        case MICROBLAZE_IT_TAGETD_TYPEC2:
        case MICROBLAZE_IT_TCAGETD_TYPEC2:
        case MICROBLAZE_IT_TCGETD_TYPEC2:
        case MICROBLAZE_IT_TEAGETD_TYPEC2:
        case MICROBLAZE_IT_TECAGETD_TYPEC2:
        case MICROBLAZE_IT_TECGETD_TYPEC2:
        case MICROBLAZE_IT_TEGETD_TYPEC2:
        case MICROBLAZE_IT_TGETD_TYPEC2:
        case MICROBLAZE_IT_TNAGETD_TYPEC2:
        case MICROBLAZE_IT_TNCAGETD_TYPEC2:
        case MICROBLAZE_IT_TNCGETD_TYPEC2:
        case MICROBLAZE_IT_TNEAGETD_TYPEC2:
        case MICROBLAZE_IT_TNECAGETD_TYPEC2:
        case MICROBLAZE_IT_TNECGETD_TYPEC2:
        case MICROBLAZE_IT_TNEGETD_TYPEC2:
        case MICROBLAZE_IT_TNGETD_TYPEC2:
            info->rB = VECINDEX(instruction,15,11,0); // 15:11
            info->rD = VECINDEX(instruction,25,21,0); // 25:21
            break;

        case MICROBLAZE_IT_APUT_TYPEC3:
        case MICROBLAZE_IT_CAPUT_TYPEC3:
        case MICROBLAZE_IT_CPUT_TYPEC3:
        case MICROBLAZE_IT_NAPUT_TYPEC3:
        case MICROBLAZE_IT_NCAPUT_TYPEC3:
        case MICROBLAZE_IT_NCPUT_TYPEC3:
        case MICROBLAZE_IT_NPUT_TYPEC3:
        case MICROBLAZE_IT_PUT_TYPEC3:
            info->FSLx = VECINDEX(instruction,3,0,0); // 3:0
            info->rA = VECINDEX(instruction,20,16,0); // 20:16
            break;

        case MICROBLAZE_IT_APUTD_TYPEC3D:
        case MICROBLAZE_IT_CAPUTD_TYPEC3D:
        case MICROBLAZE_IT_CPUTD_TYPEC3D:
        case MICROBLAZE_IT_NAPUTD_TYPEC3D:
        case MICROBLAZE_IT_NCAPUTD_TYPEC3D:
        case MICROBLAZE_IT_NCPUTD_TYPEC3D:
        case MICROBLAZE_IT_NPUTD_TYPEC3D:
        case MICROBLAZE_IT_PUTD_TYPEC3D:
            info->rA = VECINDEX(instruction,20,16,0); // 20:16
            info->rB = VECINDEX(instruction,15,11,0); // 15:11
            break;

        case MICROBLAZE_IT_TAPUT_TYPEC4:
        case MICROBLAZE_IT_TCAPUT_TYPEC4:
        case MICROBLAZE_IT_TCPUT_TYPEC4:
        case MICROBLAZE_IT_TNAPUT_TYPEC4:
        case MICROBLAZE_IT_TNCAPUT_TYPEC4:
        case MICROBLAZE_IT_TNCPUT_TYPEC4:
        case MICROBLAZE_IT_TNPUT_TYPEC4:
        case MICROBLAZE_IT_TPUT_TYPEC4:
            info->FSLx = VECINDEX(instruction,3,0,0); // 3:0
            info->rA = VECINDEX(instruction,20,16,0); // 20:16
            break;

        case MICROBLAZE_IT_TAPUTD_TYPEC4D:
        case MICROBLAZE_IT_TCAPUTD_TYPEC4D:
        case MICROBLAZE_IT_TCPUTD_TYPEC4D:
        case MICROBLAZE_IT_TNAPUTD_TYPEC4D:
        case MICROBLAZE_IT_TNCAPUTD_TYPEC4D:
        case MICROBLAZE_IT_TNCPUTD_TYPEC4D:
        case MICROBLAZE_IT_TNPUTD_TYPEC4D:
        case MICROBLAZE_IT_TPUTD_TYPEC4D:
            info->rA = VECINDEX(instruction,20,16,0); // 20:16
            info->rB = VECINDEX(instruction,15,11,0); // 15:11
            break;

        case MICROBLAZE_IT_IMM_TYPEI1:
            info->simmhi = VECINDEX(instruction,15,0,0); // 15:0
            info->simmhi = SHIFTL(info->simmhi,16);
            info->simmhi = SEXTEND(info->simmhi,31);
            break;

        case MICROBLAZE_IT_NOP_TYPEX:
            break;

        case MICROBLAZE_IT_INSTR32:
        case MICROBLAZE_IT_LAST:
            // Default to get the indices
            info->rA = VECINDEX(instruction,20,16,0); // 20:16
            info->rB = VECINDEX(instruction,15,11,0); // 15:11
            info->rD = VECINDEX(instruction,25,21,0); // 25:21
            info->simm = VECINDEX(instruction,15,0,0); // 15:0
            info->simm = SEXTEND(info->simm,15);
            break;
    }
    return;
}
