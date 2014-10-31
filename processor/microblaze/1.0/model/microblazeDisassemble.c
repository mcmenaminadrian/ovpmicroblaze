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

#include <stdio.h>
#include "vmi/vmiCxt.h"
#include "microblazeDecode.h"
#include "microblazeInstructions.h"
#include "microblazeFunctions.h"

//
// Generate the disassembler alias lookup table for GPR
//
const char *GPR[32] = {
};

//
// Generate the disassembler alias lookup table for SPR
//
const char *SPR[16384] = {
    [0x0000] = "rpc",
    [0x0001] = "rmsr",
    [0x0003] = "rear",
    [0x0005] = "resr",
    [0x0007] = "rfsr",
    [0x000B] = "rbtr",
    [0x000D] = "redr",
    [0x0800] = "rslr",
    [0x0802] = "rshr",
    [0x1000] = "rpid",
    [0x1001] = "rzpr",
    [0x1002] = "rtlbx",
    [0x1003] = "rtlblo",
    [0x1004] = "rtlbhi",
    [0x1005] = "rtlbsx",
    [0x2000] = "rpvr0",
    [0x2001] = "rpvr1",
    [0x2002] = "rpvr2",
    [0x2003] = "rpvr3",
    [0x2004] = "rpvr4",
    [0x2005] = "rpvr5",
    [0x2006] = "rpvr6",
    [0x2007] = "rpvr7",
    [0x2008] = "rpvr8",
    [0x2009] = "rpvr9",
    [0x200A] = "rpvr10",
    [0x200B] = "rpvr11",
};

//
// Generate the disassembler statemachine definitions
//
#define EMITFSLX      "\200"
#define EMITFSLX_CASE '\200'
#define EMITIM      "\201"
#define EMITIM_CASE '\201'
#define EMITOP      "\202"
#define EMITOP_CASE '\202'
#define EMITOP0      "\203"
#define EMITOP0_CASE '\203'
#define EMITRA      "\204"
#define EMITRA_CASE '\204'
#define EMITRB      "\205"
#define EMITRB_CASE '\205'
#define EMITRD      "\206"
#define EMITRD_CASE '\206'
#define EMITRS      "\207"
#define EMITRS_CASE '\207'
#define EMITSI      "\210"
#define EMITSI_CASE '\210'
#define EMITSIH      "\211"
#define EMITSIH_CASE '\211'
#define EMITUI      "\212"
#define EMITUI_CASE '\212'

//
// Generate the instruction formatting table
//
#define FMT_NONE ""
const char *formats[MICROBLAZE_IT_LAST+1] = {
    //
    // Instruction Format TypeA1 Insns
    //
    [MICROBLAZE_IT_ADD_TYPEA1] = EMITOP " " EMITRD ", " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_ADDC_TYPEA1] = EMITOP " " EMITRD ", " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_ADDK_TYPEA1] = EMITOP " " EMITRD ", " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_ADDKC_TYPEA1] = EMITOP " " EMITRD ", " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_AND_TYPEA1] = EMITOP " " EMITRD ", " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_ANDN_TYPEA1] = EMITOP " " EMITRD ", " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_FADD_TYPEA1] = EMITOP " " EMITRD ", " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_FCMPEQ_TYPEA1] = EMITOP " " EMITRD ", " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_FCMPGE_TYPEA1] = EMITOP " " EMITRD ", " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_FCMPGT_TYPEA1] = EMITOP " " EMITRD ", " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_FCMPLE_TYPEA1] = EMITOP " " EMITRD ", " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_FCMPLT_TYPEA1] = EMITOP " " EMITRD ", " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_FCMPNE_TYPEA1] = EMITOP " " EMITRD ", " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_FCMPUN_TYPEA1] = EMITOP " " EMITRD ", " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_FDIV_TYPEA1] = EMITOP " " EMITRD ", " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_FMUL_TYPEA1] = EMITOP " " EMITRD ", " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_FRSUB_TYPEA1] = EMITOP " " EMITRD ", " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_LWX_TYPEA1] = EMITOP " " EMITRD ", " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_MUL_TYPEA1] = EMITOP " " EMITRD ", " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_MULH_TYPEA1] = EMITOP " " EMITRD ", " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_MULHSU_TYPEA1] = EMITOP " " EMITRD ", " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_MULHU_TYPEA1] = EMITOP " " EMITRD ", " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_OR_TYPEA1] = EMITOP " " EMITRD ", " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_PCMPBF_TYPEA1] = EMITOP " " EMITRD ", " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_PCMPEQ_TYPEA1] = EMITOP " " EMITRD ", " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_PCMPNE_TYPEA1] = EMITOP " " EMITRD ", " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_RSUB_TYPEA1] = EMITOP " " EMITRD ", " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_RSUBC_TYPEA1] = EMITOP " " EMITRD ", " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_RSUBK_TYPEA1] = EMITOP " " EMITRD ", " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_RSUBKC_TYPEA1] = EMITOP " " EMITRD ", " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_SWX_TYPEA1] = EMITOP " " EMITRD ", " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_XOR_TYPEA1] = EMITOP " " EMITRD ", " EMITRA ", " EMITRB,

    //
    // Instruction Format TypeA2 Insns
    //
    [MICROBLAZE_IT_BEQ_TYPEA2] = EMITOP " " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_BEQD_TYPEA2] = EMITOP " " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_BGE_TYPEA2] = EMITOP " " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_BGED_TYPEA2] = EMITOP " " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_BGT_TYPEA2] = EMITOP " " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_BGTD_TYPEA2] = EMITOP " " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_BLE_TYPEA2] = EMITOP " " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_BLED_TYPEA2] = EMITOP " " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_BLT_TYPEA2] = EMITOP " " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_BLTD_TYPEA2] = EMITOP " " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_BNE_TYPEA2] = EMITOP " " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_BNED_TYPEA2] = EMITOP " " EMITRA ", " EMITRB,

    //
    // Instruction Format TypeA3 Insns
    //
    [MICROBLAZE_IT_BR_TYPEA3] = EMITOP " " EMITRB,
    [MICROBLAZE_IT_BRA_TYPEA3] = EMITOP " " EMITRB,
    [MICROBLAZE_IT_BRAD_TYPEA3] = EMITOP " " EMITRB,
    [MICROBLAZE_IT_BRD_TYPEA3] = EMITOP " " EMITRB,

    //
    // Instruction Format TypeA4 Insns
    //
    [MICROBLAZE_IT_BRALD_TYPEA4] = EMITOP " " EMITRD ", " EMITRB,
    [MICROBLAZE_IT_BRLD_TYPEA4] = EMITOP " " EMITRD ", " EMITRB,

    //
    // Instruction Format TypeA5 Insns
    //
    [MICROBLAZE_IT_BRK_TYPEA5] = EMITOP " " EMITRD ", " EMITRB,

    //
    // Instruction Format TypeA6 Insns
    //
    [MICROBLAZE_IT_BSLL_TYPEA6] = EMITOP " " EMITRD ", " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_BSRA_TYPEA6] = EMITOP " " EMITRD ", " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_BSRL_TYPEA6] = EMITOP " " EMITRD ", " EMITRA ", " EMITRB,

    //
    // Instruction Format TypeA7 Insns
    //
    [MICROBLAZE_IT_CMP_TYPEA7] = EMITOP " " EMITRD ", " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_CMPU_TYPEA7] = EMITOP " " EMITRD ", " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_IDIV_TYPEA7] = EMITOP " " EMITRD ", " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_IDIVU_TYPEA7] = EMITOP " " EMITRD ", " EMITRA ", " EMITRB,

    //
    // Instruction Format TypeA8 Insns
    //
    [MICROBLAZE_IT_LBU_TYPEA8] = EMITOP " " EMITRD ", " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_LBUR_TYPEA8] = EMITOP " " EMITRD ", " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_LHU_TYPEA8] = EMITOP " " EMITRD ", " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_LHUR_TYPEA8] = EMITOP " " EMITRD ", " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_LW_TYPEA8] = EMITOP " " EMITRD ", " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_LWR_TYPEA8] = EMITOP " " EMITRD ", " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_SB_TYPEA8] = EMITOP " " EMITRD ", " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_SBR_TYPEA8] = EMITOP " " EMITRD ", " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_SH_TYPEA8] = EMITOP " " EMITRD ", " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_SHR_TYPEA8] = EMITOP " " EMITRD ", " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_SW_TYPEA8] = EMITOP " " EMITRD ", " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_SWR_TYPEA8] = EMITOP " " EMITRD ", " EMITRA ", " EMITRB,

    //
    // Instruction Format TypeA9 Insns
    //
    [MICROBLAZE_IT_MFS_TYPEA9] = EMITOP " " EMITRD ", " EMITRS,

    //
    // Instruction Format TypeAa Insns
    //
    [MICROBLAZE_IT_MTS_TYPEAA] = EMITOP " " EMITRS ", " EMITRA,

    //
    // Instruction Format TypeAb Insns
    //
    [MICROBLAZE_IT_CLZ_TYPEAB] = EMITOP " " EMITRD ", " EMITRA,
    [MICROBLAZE_IT_FINT_TYPEAB] = EMITOP " " EMITRD ", " EMITRA,
    [MICROBLAZE_IT_FLT_TYPEAB] = EMITOP " " EMITRD ", " EMITRA,
    [MICROBLAZE_IT_FSQRT_TYPEAB] = EMITOP " " EMITRD ", " EMITRA,
    [MICROBLAZE_IT_SEXT16_TYPEAB] = EMITOP " " EMITRD ", " EMITRA,
    [MICROBLAZE_IT_SEXT8_TYPEAB] = EMITOP " " EMITRD ", " EMITRA,
    [MICROBLAZE_IT_SRA_TYPEAB] = EMITOP " " EMITRD ", " EMITRA,
    [MICROBLAZE_IT_SRC_TYPEAB] = EMITOP " " EMITRD ", " EMITRA,
    [MICROBLAZE_IT_SRL_TYPEAB] = EMITOP " " EMITRD ", " EMITRA,

    //
    // Instruction Format TypeAc Insns
    //
    [MICROBLAZE_IT_WDC_TYPEAC] = EMITOP " " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_WDCCLR_TYPEAC] = EMITOP " " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_WDCFL_TYPEAC] = EMITOP " " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_WIC_TYPEAC] = EMITOP " " EMITRA ", " EMITRB,

    //
    // Instruction Format TypeB1 Insns
    //
    [MICROBLAZE_IT_ADDI_TYPEB1] = EMITOP " " EMITRD ", " EMITRA ", " EMITSI " // " EMITIM,
    [MICROBLAZE_IT_ADDIC_TYPEB1] = EMITOP " " EMITRD ", " EMITRA ", " EMITSI " // " EMITIM,
    [MICROBLAZE_IT_ADDIK_TYPEB1] = EMITOP " " EMITRD ", " EMITRA ", " EMITSI " // " EMITIM,
    [MICROBLAZE_IT_ADDIKC_TYPEB1] = EMITOP " " EMITRD ", " EMITRA ", " EMITSI " // " EMITIM,
    [MICROBLAZE_IT_ANDI_TYPEB1] = EMITOP " " EMITRD ", " EMITRA ", " EMITSI " // " EMITIM,
    [MICROBLAZE_IT_ANDNI_TYPEB1] = EMITOP " " EMITRD ", " EMITRA ", " EMITSI " // " EMITIM,
    [MICROBLAZE_IT_LBUI_TYPEB1] = EMITOP " " EMITRD ", " EMITRA ", " EMITSI " // " EMITIM,
    [MICROBLAZE_IT_LHUI_TYPEB1] = EMITOP " " EMITRD ", " EMITRA ", " EMITSI " // " EMITIM,
    [MICROBLAZE_IT_LWI_TYPEB1] = EMITOP " " EMITRD ", " EMITRA ", " EMITSI " // " EMITIM,
    [MICROBLAZE_IT_MULI_TYPEB1] = EMITOP " " EMITRD ", " EMITRA ", " EMITSI " // " EMITIM,
    [MICROBLAZE_IT_ORI_TYPEB1] = EMITOP " " EMITRD ", " EMITRA ", " EMITSI " // " EMITIM,
    [MICROBLAZE_IT_RSUBI_TYPEB1] = EMITOP " " EMITRD ", " EMITRA ", " EMITSI " // " EMITIM,
    [MICROBLAZE_IT_RSUBIC_TYPEB1] = EMITOP " " EMITRD ", " EMITRA ", " EMITSI " // " EMITIM,
    [MICROBLAZE_IT_RSUBIK_TYPEB1] = EMITOP " " EMITRD ", " EMITRA ", " EMITSI " // " EMITIM,
    [MICROBLAZE_IT_RSUBIKC_TYPEB1] = EMITOP " " EMITRD ", " EMITRA ", " EMITSI " // " EMITIM,
    [MICROBLAZE_IT_SBI_TYPEB1] = EMITOP " " EMITRD ", " EMITRA ", " EMITSI " // " EMITIM,
    [MICROBLAZE_IT_SHI_TYPEB1] = EMITOP " " EMITRD ", " EMITRA ", " EMITSI " // " EMITIM,
    [MICROBLAZE_IT_SWI_TYPEB1] = EMITOP " " EMITRD ", " EMITRA ", " EMITSI " // " EMITIM,
    [MICROBLAZE_IT_XORI_TYPEB1] = EMITOP " " EMITRD ", " EMITRA ", " EMITSI " // " EMITIM,

    //
    // Instruction Format TypeB2 Insns
    //
    [MICROBLAZE_IT_BEQI_TYPEB2] = EMITOP " " EMITRA ", " EMITSI " // " EMITIM,
    [MICROBLAZE_IT_BEQID_TYPEB2] = EMITOP " " EMITRA ", " EMITSI " // " EMITIM,
    [MICROBLAZE_IT_BGEI_TYPEB2] = EMITOP " " EMITRA ", " EMITSI " // " EMITIM,
    [MICROBLAZE_IT_BGEID_TYPEB2] = EMITOP " " EMITRA ", " EMITSI " // " EMITIM,
    [MICROBLAZE_IT_BGTI_TYPEB2] = EMITOP " " EMITRA ", " EMITSI " // " EMITIM,
    [MICROBLAZE_IT_BGTID_TYPEB2] = EMITOP " " EMITRA ", " EMITSI " // " EMITIM,
    [MICROBLAZE_IT_BLEI_TYPEB2] = EMITOP " " EMITRA ", " EMITSI " // " EMITIM,
    [MICROBLAZE_IT_BLEID_TYPEB2] = EMITOP " " EMITRA ", " EMITSI " // " EMITIM,
    [MICROBLAZE_IT_BLTI_TYPEB2] = EMITOP " " EMITRA ", " EMITSI " // " EMITIM,
    [MICROBLAZE_IT_BLTID_TYPEB2] = EMITOP " " EMITRA ", " EMITSI " // " EMITIM,
    [MICROBLAZE_IT_BNEI_TYPEB2] = EMITOP " " EMITRA ", " EMITSI " // " EMITIM,
    [MICROBLAZE_IT_BNEID_TYPEB2] = EMITOP " " EMITRA ", " EMITSI " // " EMITIM,

    //
    // Instruction Format TypeB3 Insns
    //
    [MICROBLAZE_IT_BRAI_TYPEB3] = EMITOP " " EMITSI " // " EMITIM,
    [MICROBLAZE_IT_BRAID_TYPEB3] = EMITOP " " EMITSI " // " EMITIM,
    [MICROBLAZE_IT_BRI_TYPEB3] = EMITOP " " EMITSI " // " EMITIM,
    [MICROBLAZE_IT_BRID_TYPEB3] = EMITOP " " EMITSI " // " EMITIM,

    //
    // Instruction Format TypeB4 Insns
    //
    [MICROBLAZE_IT_BRALID_TYPEB4] = EMITOP " " EMITRD ", " EMITSI " // " EMITIM,
    [MICROBLAZE_IT_BRLID_TYPEB4] = EMITOP " " EMITRD ", " EMITSI " // " EMITIM,

    //
    // Instruction Format TypeB5 Insns
    //
    [MICROBLAZE_IT_BRKI_TYPEB5] = EMITOP " " EMITRD ", " EMITSI " // " EMITIM,

    //
    // Instruction Format TypeB6 Insns
    //
    [MICROBLAZE_IT_BSLLI_TYPEB6] = EMITOP " " EMITRD ", " EMITRA ", " EMITUI,
    [MICROBLAZE_IT_BSRAI_TYPEB6] = EMITOP " " EMITRD ", " EMITRA ", " EMITUI,
    [MICROBLAZE_IT_BSRLI_TYPEB6] = EMITOP " " EMITRD ", " EMITRA ", " EMITUI,

    //
    // Instruction Format TypeB8 Insns
    //
    [MICROBLAZE_IT_MBAR_TYPEB8] = EMITOP " " EMITSI " // " EMITIM,

    //
    // Instruction Format TypeB9 Insns
    //
    [MICROBLAZE_IT_MSRCLR_TYPEB9] = EMITOP " " EMITRD ", " EMITUI,
    [MICROBLAZE_IT_MSRSET_TYPEB9] = EMITOP " " EMITRD ", " EMITUI,

    //
    // Instruction Format TypeBa Insns
    //
    [MICROBLAZE_IT_RTBD_TYPEBA] = EMITOP " " EMITRA ", " EMITSI " // " EMITIM,
    [MICROBLAZE_IT_RTED_TYPEBA] = EMITOP " " EMITRA ", " EMITSI " // " EMITIM,
    [MICROBLAZE_IT_RTID_TYPEBA] = EMITOP " " EMITRA ", " EMITSI " // " EMITIM,
    [MICROBLAZE_IT_RTSD_TYPEBA] = EMITOP " " EMITRA ", " EMITSI " // " EMITIM,

    //
    // Instruction Format TypeC1 Insns
    //
    [MICROBLAZE_IT_AGET_TYPEC1] = EMITOP " " EMITRD ", " EMITFSLX,
    [MICROBLAZE_IT_CAGET_TYPEC1] = EMITOP " " EMITRD ", " EMITFSLX,
    [MICROBLAZE_IT_CGET_TYPEC1] = EMITOP " " EMITRD ", " EMITFSLX,
    [MICROBLAZE_IT_EAGET_TYPEC1] = EMITOP " " EMITRD ", " EMITFSLX,
    [MICROBLAZE_IT_ECAGET_TYPEC1] = EMITOP " " EMITRD ", " EMITFSLX,
    [MICROBLAZE_IT_ECGET_TYPEC1] = EMITOP " " EMITRD ", " EMITFSLX,
    [MICROBLAZE_IT_EGET_TYPEC1] = EMITOP " " EMITRD ", " EMITFSLX,
    [MICROBLAZE_IT_GET_TYPEC1] = EMITOP " " EMITRD ", " EMITFSLX,
    [MICROBLAZE_IT_NAGET_TYPEC1] = EMITOP " " EMITRD ", " EMITFSLX,
    [MICROBLAZE_IT_NCAGET_TYPEC1] = EMITOP " " EMITRD ", " EMITFSLX,
    [MICROBLAZE_IT_NCGET_TYPEC1] = EMITOP " " EMITRD ", " EMITFSLX,
    [MICROBLAZE_IT_NEAGET_TYPEC1] = EMITOP " " EMITRD ", " EMITFSLX,
    [MICROBLAZE_IT_NECAGET_TYPEC1] = EMITOP " " EMITRD ", " EMITFSLX,
    [MICROBLAZE_IT_NECGET_TYPEC1] = EMITOP " " EMITRD ", " EMITFSLX,
    [MICROBLAZE_IT_NEGET_TYPEC1] = EMITOP " " EMITRD ", " EMITFSLX,
    [MICROBLAZE_IT_NGET_TYPEC1] = EMITOP " " EMITRD ", " EMITFSLX,
    [MICROBLAZE_IT_TAGET_TYPEC1] = EMITOP " " EMITRD ", " EMITFSLX,
    [MICROBLAZE_IT_TCAGET_TYPEC1] = EMITOP " " EMITRD ", " EMITFSLX,
    [MICROBLAZE_IT_TCGET_TYPEC1] = EMITOP " " EMITRD ", " EMITFSLX,
    [MICROBLAZE_IT_TEAGET_TYPEC1] = EMITOP " " EMITRD ", " EMITFSLX,
    [MICROBLAZE_IT_TECAGET_TYPEC1] = EMITOP " " EMITRD ", " EMITFSLX,
    [MICROBLAZE_IT_TECGET_TYPEC1] = EMITOP " " EMITRD ", " EMITFSLX,
    [MICROBLAZE_IT_TEGET_TYPEC1] = EMITOP " " EMITRD ", " EMITFSLX,
    [MICROBLAZE_IT_TGET_TYPEC1] = EMITOP " " EMITRD ", " EMITFSLX,
    [MICROBLAZE_IT_TNAGET_TYPEC1] = EMITOP " " EMITRD ", " EMITFSLX,
    [MICROBLAZE_IT_TNCAGET_TYPEC1] = EMITOP " " EMITRD ", " EMITFSLX,
    [MICROBLAZE_IT_TNCGET_TYPEC1] = EMITOP " " EMITRD ", " EMITFSLX,
    [MICROBLAZE_IT_TNEAGET_TYPEC1] = EMITOP " " EMITRD ", " EMITFSLX,
    [MICROBLAZE_IT_TNECAGET_TYPEC1] = EMITOP " " EMITRD ", " EMITFSLX,
    [MICROBLAZE_IT_TNECGET_TYPEC1] = EMITOP " " EMITRD ", " EMITFSLX,
    [MICROBLAZE_IT_TNEGET_TYPEC1] = EMITOP " " EMITRD ", " EMITFSLX,
    [MICROBLAZE_IT_TNGET_TYPEC1] = EMITOP " " EMITRD ", " EMITFSLX,

    //
    // Instruction Format TypeC2 Insns
    //
    [MICROBLAZE_IT_AGETD_TYPEC2] = EMITOP " " EMITRD ", " EMITRB,
    [MICROBLAZE_IT_CAGETD_TYPEC2] = EMITOP " " EMITRD ", " EMITRB,
    [MICROBLAZE_IT_CGETD_TYPEC2] = EMITOP " " EMITRD ", " EMITRB,
    [MICROBLAZE_IT_EAGETD_TYPEC2] = EMITOP " " EMITRD ", " EMITRB,
    [MICROBLAZE_IT_ECAGETD_TYPEC2] = EMITOP " " EMITRD ", " EMITRB,
    [MICROBLAZE_IT_ECGETD_TYPEC2] = EMITOP " " EMITRD ", " EMITRB,
    [MICROBLAZE_IT_EGETD_TYPEC2] = EMITOP " " EMITRD ", " EMITRB,
    [MICROBLAZE_IT_GETD_TYPEC2] = EMITOP " " EMITRD ", " EMITRB,
    [MICROBLAZE_IT_NAGETD_TYPEC2] = EMITOP " " EMITRD ", " EMITRB,
    [MICROBLAZE_IT_NCAGETD_TYPEC2] = EMITOP " " EMITRD ", " EMITRB,
    [MICROBLAZE_IT_NCGETD_TYPEC2] = EMITOP " " EMITRD ", " EMITRB,
    [MICROBLAZE_IT_NEAGETD_TYPEC2] = EMITOP " " EMITRD ", " EMITRB,
    [MICROBLAZE_IT_NECAGETD_TYPEC2] = EMITOP " " EMITRD ", " EMITRB,
    [MICROBLAZE_IT_NECGETD_TYPEC2] = EMITOP " " EMITRD ", " EMITRB,
    [MICROBLAZE_IT_NEGETD_TYPEC2] = EMITOP " " EMITRD ", " EMITRB,
    [MICROBLAZE_IT_NGETD_TYPEC2] = EMITOP " " EMITRD ", " EMITRB,
    [MICROBLAZE_IT_TAGETD_TYPEC2] = EMITOP " " EMITRD ", " EMITRB,
    [MICROBLAZE_IT_TCAGETD_TYPEC2] = EMITOP " " EMITRD ", " EMITRB,
    [MICROBLAZE_IT_TCGETD_TYPEC2] = EMITOP " " EMITRD ", " EMITRB,
    [MICROBLAZE_IT_TEAGETD_TYPEC2] = EMITOP " " EMITRD ", " EMITRB,
    [MICROBLAZE_IT_TECAGETD_TYPEC2] = EMITOP " " EMITRD ", " EMITRB,
    [MICROBLAZE_IT_TECGETD_TYPEC2] = EMITOP " " EMITRD ", " EMITRB,
    [MICROBLAZE_IT_TEGETD_TYPEC2] = EMITOP " " EMITRD ", " EMITRB,
    [MICROBLAZE_IT_TGETD_TYPEC2] = EMITOP " " EMITRD ", " EMITRB,
    [MICROBLAZE_IT_TNAGETD_TYPEC2] = EMITOP " " EMITRD ", " EMITRB,
    [MICROBLAZE_IT_TNCAGETD_TYPEC2] = EMITOP " " EMITRD ", " EMITRB,
    [MICROBLAZE_IT_TNCGETD_TYPEC2] = EMITOP " " EMITRD ", " EMITRB,
    [MICROBLAZE_IT_TNEAGETD_TYPEC2] = EMITOP " " EMITRD ", " EMITRB,
    [MICROBLAZE_IT_TNECAGETD_TYPEC2] = EMITOP " " EMITRD ", " EMITRB,
    [MICROBLAZE_IT_TNECGETD_TYPEC2] = EMITOP " " EMITRD ", " EMITRB,
    [MICROBLAZE_IT_TNEGETD_TYPEC2] = EMITOP " " EMITRD ", " EMITRB,
    [MICROBLAZE_IT_TNGETD_TYPEC2] = EMITOP " " EMITRD ", " EMITRB,

    //
    // Instruction Format TypeC3 Insns
    //
    [MICROBLAZE_IT_APUT_TYPEC3] = EMITOP " " EMITRA ", " EMITFSLX,
    [MICROBLAZE_IT_CAPUT_TYPEC3] = EMITOP " " EMITRA ", " EMITFSLX,
    [MICROBLAZE_IT_CPUT_TYPEC3] = EMITOP " " EMITRA ", " EMITFSLX,
    [MICROBLAZE_IT_NAPUT_TYPEC3] = EMITOP " " EMITRA ", " EMITFSLX,
    [MICROBLAZE_IT_NCAPUT_TYPEC3] = EMITOP " " EMITRA ", " EMITFSLX,
    [MICROBLAZE_IT_NCPUT_TYPEC3] = EMITOP " " EMITRA ", " EMITFSLX,
    [MICROBLAZE_IT_NPUT_TYPEC3] = EMITOP " " EMITRA ", " EMITFSLX,
    [MICROBLAZE_IT_PUT_TYPEC3] = EMITOP " " EMITRA ", " EMITFSLX,

    //
    // Instruction Format TypeC3d Insns
    //
    [MICROBLAZE_IT_APUTD_TYPEC3D] = EMITOP " " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_CAPUTD_TYPEC3D] = EMITOP " " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_CPUTD_TYPEC3D] = EMITOP " " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_NAPUTD_TYPEC3D] = EMITOP " " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_NCAPUTD_TYPEC3D] = EMITOP " " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_NCPUTD_TYPEC3D] = EMITOP " " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_NPUTD_TYPEC3D] = EMITOP " " EMITRA ", " EMITRB,
    [MICROBLAZE_IT_PUTD_TYPEC3D] = EMITOP " " EMITRA ", " EMITRB,

    //
    // Instruction Format TypeC4 Insns
    //
    [MICROBLAZE_IT_TAPUT_TYPEC4] = EMITOP " " EMITFSLX,
    [MICROBLAZE_IT_TCAPUT_TYPEC4] = EMITOP " " EMITFSLX,
    [MICROBLAZE_IT_TCPUT_TYPEC4] = EMITOP " " EMITFSLX,
    [MICROBLAZE_IT_TNAPUT_TYPEC4] = EMITOP " " EMITFSLX,
    [MICROBLAZE_IT_TNCAPUT_TYPEC4] = EMITOP " " EMITFSLX,
    [MICROBLAZE_IT_TNCPUT_TYPEC4] = EMITOP " " EMITFSLX,
    [MICROBLAZE_IT_TNPUT_TYPEC4] = EMITOP " " EMITFSLX,
    [MICROBLAZE_IT_TPUT_TYPEC4] = EMITOP " " EMITFSLX,

    //
    // Instruction Format TypeC4d Insns
    //
    [MICROBLAZE_IT_TAPUTD_TYPEC4D] = EMITOP " " EMITRB,
    [MICROBLAZE_IT_TCAPUTD_TYPEC4D] = EMITOP " " EMITRB,
    [MICROBLAZE_IT_TCPUTD_TYPEC4D] = EMITOP " " EMITRB,
    [MICROBLAZE_IT_TNAPUTD_TYPEC4D] = EMITOP " " EMITRB,
    [MICROBLAZE_IT_TNCAPUTD_TYPEC4D] = EMITOP " " EMITRB,
    [MICROBLAZE_IT_TNCPUTD_TYPEC4D] = EMITOP " " EMITRB,
    [MICROBLAZE_IT_TNPUTD_TYPEC4D] = EMITOP " " EMITRB,
    [MICROBLAZE_IT_TPUTD_TYPEC4D] = EMITOP " " EMITRB,

    //
    // Instruction Format TypeI1 Insns
    //
    [MICROBLAZE_IT_IMM_TYPEI1] = EMITOP " " EMITSIH,

    //
    // Instruction Format TypeX Insns
    //
    [MICROBLAZE_IT_NOP_TYPEX] = EMITOP0,

    [MICROBLAZE_IT_LAST] = "UNDEFINED"
};

//
// generate builtin disassembler writer functions
//
static void putChar(char **result, char ch) {
    char *tail = *result;
    *tail++ = ch;
    *tail = 0;
    *result = tail;
}

static void putString(char **result, const char *string) {
    char *tail = *result;
    char  ch;
    while((ch=*string++)) {
        *tail++ = ch;
    }
    *tail = 0;
    *result = tail;
}

//
// generate generated disassembler writer functions
//

//
// Function to format the info field #FSLx#
//
static void EMITFSLX_FN(char **result, microblazeInstructionInfoP info) {
    char tmp[32];
    const char *fmt = "rfsl%d";
    sprintf(tmp, fmt, info->FSLx);
    putString(result, tmp);
}
//
// Function to format the info field ( #simmhi# != 0xffff ) ? ( #simmhi# | ( #simm# & 0xffff ) ) : #simm#
//
static void EMITIM_FN(char **result, microblazeInstructionInfoP info) {
    char tmp[32];
    const char *fmt = "IMM32=%d";
    sprintf(tmp, fmt, ( info->simmhi != 0xffff ) ? ( info->simmhi | ( info->simm & 0xffff ) ) : info->simm);
    putString(result, tmp);
}
//
// Function to format the info field #opcode#
//
static void EMITOP_FN(char **result, microblazeInstructionInfoP info) {
    char tmp[32];
    const char *fmt = "%-8s";
    sprintf(tmp, fmt, info->opcode);
    putString(result, tmp);
}
//
// Function to format the info field #opcode#
//
static void EMITOP0_FN(char **result, microblazeInstructionInfoP info) {
    char tmp[32];
    const char *fmt = "%s";
    sprintf(tmp, fmt, info->opcode);
    putString(result, tmp);
}
//
// Function to format the info field #rA#
//
static void EMITRA_FN(char **result, microblazeInstructionInfoP info) {
    char tmp[32];
    const char *fmt = (GPR[info->rA] ? GPR[info->rA] : "r%d");
    sprintf(tmp, fmt, info->rA);
    putString(result, tmp);
}
//
// Function to format the info field #rB#
//
static void EMITRB_FN(char **result, microblazeInstructionInfoP info) {
    char tmp[32];
    const char *fmt = (GPR[info->rB] ? GPR[info->rB] : "r%d");
    sprintf(tmp, fmt, info->rB);
    putString(result, tmp);
}
//
// Function to format the info field #rD#
//
static void EMITRD_FN(char **result, microblazeInstructionInfoP info) {
    char tmp[32];
    const char *fmt = (GPR[info->rD] ? GPR[info->rD] : "r%d");
    sprintf(tmp, fmt, info->rD);
    putString(result, tmp);
}
//
// Function to format the info field #rS#
//
static void EMITRS_FN(char **result, microblazeInstructionInfoP info) {
    char tmp[32];
    const char *fmt = (SPR[info->rS] ? SPR[info->rS] : "r%d");
    sprintf(tmp, fmt, info->rS);
    putString(result, tmp);
}
//
// Function to format the info field #simm#
//
static void EMITSI_FN(char **result, microblazeInstructionInfoP info) {
    char tmp[32];
    const char *fmt = "%d";
    sprintf(tmp, fmt, info->simm);
    putString(result, tmp);
}
//
// Function to format the info field #simmhi#
//
static void EMITSIH_FN(char **result, microblazeInstructionInfoP info) {
    char tmp[32];
    const char *fmt = "%d";
    sprintf(tmp, fmt, info->simmhi);
    putString(result, tmp);
}
//
// Function to format the info field #uimm#
//
static void EMITUI_FN(char **result, microblazeInstructionInfoP info) {
    char tmp[32];
    const char *fmt = "%u";
    sprintf(tmp, fmt, info->uimm);
    putString(result, tmp);
}

//
// generate disassembler state machine
//
static void disassembleFormat(
    microblazeP microblaze,
    microblazeInstructionInfoP info,
    char **result,
    const char *format) {

    //
    // Default disassembler output
    //
    char ifmt[64];
    Uns8 ifmtchars = 2 * info->instrsize;
    sprintf(ifmt, "??? instruction(%d bytes)=0x%%0%0dx ", info->instrsize, ifmtchars);
    sprintf(*result, ifmt, info->instruction);

    char ch;
    while((ch=*format++)) {
         switch(ch) {
             case EMITFSLX_CASE:
                 EMITFSLX_FN(result, info);
                 break;

             case EMITIM_CASE:
                 EMITIM_FN(result, info);
                 break;

             case EMITOP_CASE:
                 EMITOP_FN(result, info);
                 break;

             case EMITOP0_CASE:
                 EMITOP0_FN(result, info);
                 break;

             case EMITRA_CASE:
                 EMITRA_FN(result, info);
                 break;

             case EMITRB_CASE:
                 EMITRB_FN(result, info);
                 break;

             case EMITRD_CASE:
                 EMITRD_FN(result, info);
                 break;

             case EMITRS_CASE:
                 EMITRS_FN(result, info);
                 break;

             case EMITSI_CASE:
                 EMITSI_FN(result, info);
                 break;

             case EMITSIH_CASE:
                 EMITSIH_FN(result, info);
                 break;

             case EMITUI_CASE:
                 EMITUI_FN(result, info);
                 break;

             default:
                 putChar(result, ch);
                 break;
        }
    }
}

//
// microblaze disassembler, decoded instruction interface
//
const char *microblazeDisassembleInfo(microblazeP microblaze, microblazeInstructionInfoP info) {
    static char result[256];
    const char *format;
    char *tail = result;

    //
    // disassemble using the format for the type
    //
    if((format=formats[info->type])) {
        disassembleFormat(microblaze, info, &tail, format);
    } else {
        sprintf(result, "??? instruction:0x%08x", info->instruction);
    }
    return result;
}

//
// microblaze disassembler
//
VMI_DISASSEMBLE_FN(microblazeDis) {
    // static buffer to hold disassembly result
    microblazeP microblaze = (microblazeP)processor;
    microblazeInstructionInfo info;
    // get instruction and instruction type
    microblazeDecode(microblaze, thisPC, &info);
    // return disassembled instruction
    return microblazeDisassembleInfo(microblaze, &info);
}
