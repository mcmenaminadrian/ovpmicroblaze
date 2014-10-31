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
#include "microblazeDecode.h"
#include "microblazeExceptions.h"
#include "microblazeInstructions.h"
#include "microblazeStructure.h"

//
// Write Morpher Table
//
const static microblazeMorphAttr dispatchTable[MICROBLAZE_IT_LAST+1] = {
    //
    // Format TypeA1 Insns
    //
    [MICROBLAZE_IT_ADD_TYPEA1] = {arch:V8_20, morphCB:morphADD_TYPEA1},
    [MICROBLAZE_IT_ADDC_TYPEA1] = {arch:V8_20, morphCB:morphADDC_TYPEA1},
    [MICROBLAZE_IT_ADDK_TYPEA1] = {arch:V8_20, morphCB:morphADDK_TYPEA1},
    [MICROBLAZE_IT_ADDKC_TYPEA1] = {arch:V8_20, morphCB:morphADDKC_TYPEA1},
    [MICROBLAZE_IT_AND_TYPEA1] = {arch:V8_20, morphCB:morphAND_TYPEA1},
    [MICROBLAZE_IT_ANDN_TYPEA1] = {arch:V8_20, morphCB:morphANDN_TYPEA1},
    [MICROBLAZE_IT_FADD_TYPEA1] = {arch:V8_20, morphCB:morphFADD_TYPEA1},
    [MICROBLAZE_IT_FCMPEQ_TYPEA1] = {arch:V8_20, morphCB:morphFCMPEQ_TYPEA1},
    [MICROBLAZE_IT_FCMPGE_TYPEA1] = {arch:V8_20, morphCB:morphFCMPGE_TYPEA1},
    [MICROBLAZE_IT_FCMPGT_TYPEA1] = {arch:V8_20, morphCB:morphFCMPGT_TYPEA1},
    [MICROBLAZE_IT_FCMPLE_TYPEA1] = {arch:V8_20, morphCB:morphFCMPLE_TYPEA1},
    [MICROBLAZE_IT_FCMPLT_TYPEA1] = {arch:V8_20, morphCB:morphFCMPLT_TYPEA1},
    [MICROBLAZE_IT_FCMPNE_TYPEA1] = {arch:V8_20, morphCB:morphFCMPNE_TYPEA1},
    [MICROBLAZE_IT_FCMPUN_TYPEA1] = {arch:V8_20, morphCB:morphFCMPUN_TYPEA1},
    [MICROBLAZE_IT_FDIV_TYPEA1] = {arch:V8_20, morphCB:morphFDIV_TYPEA1},
    [MICROBLAZE_IT_FMUL_TYPEA1] = {arch:V8_20, morphCB:morphFMUL_TYPEA1},
    [MICROBLAZE_IT_FRSUB_TYPEA1] = {arch:V8_20, morphCB:morphFRSUB_TYPEA1},
    [MICROBLAZE_IT_LWX_TYPEA1] = {arch:V8_20, morphCB:morphLWX_TYPEA1},
    [MICROBLAZE_IT_MUL_TYPEA1] = {arch:V8_20, morphCB:morphMUL_TYPEA1},
    [MICROBLAZE_IT_MULH_TYPEA1] = {arch:V8_20, morphCB:morphMULH_TYPEA1},
    [MICROBLAZE_IT_MULHSU_TYPEA1] = {arch:V8_20, morphCB:morphMULHSU_TYPEA1},
    [MICROBLAZE_IT_MULHU_TYPEA1] = {arch:V8_20, morphCB:morphMULHU_TYPEA1},
    [MICROBLAZE_IT_OR_TYPEA1] = {arch:V8_20, morphCB:morphOR_TYPEA1},
    [MICROBLAZE_IT_PCMPBF_TYPEA1] = {arch:V8_20, morphCB:morphPCMPBF_TYPEA1},
    [MICROBLAZE_IT_PCMPEQ_TYPEA1] = {arch:V8_20, morphCB:morphPCMPEQ_TYPEA1},
    [MICROBLAZE_IT_PCMPNE_TYPEA1] = {arch:V8_20, morphCB:morphPCMPNE_TYPEA1},
    [MICROBLAZE_IT_RSUB_TYPEA1] = {arch:V8_20, morphCB:morphRSUB_TYPEA1},
    [MICROBLAZE_IT_RSUBC_TYPEA1] = {arch:V8_20, morphCB:morphRSUBC_TYPEA1},
    [MICROBLAZE_IT_RSUBK_TYPEA1] = {arch:V8_20, morphCB:morphRSUBK_TYPEA1},
    [MICROBLAZE_IT_RSUBKC_TYPEA1] = {arch:V8_20, morphCB:morphRSUBKC_TYPEA1},
    [MICROBLAZE_IT_SWX_TYPEA1] = {arch:V8_20, morphCB:morphSWX_TYPEA1},
    [MICROBLAZE_IT_XOR_TYPEA1] = {arch:V8_20, morphCB:morphXOR_TYPEA1},

    //
    // Format TypeA2 Insns
    //
    [MICROBLAZE_IT_BEQ_TYPEA2] = {arch:V8_20, morphCB:morphBEQ_TYPEA2},
    [MICROBLAZE_IT_BEQD_TYPEA2] = {arch:V8_20, morphCB:morphBEQD_TYPEA2},
    [MICROBLAZE_IT_BGE_TYPEA2] = {arch:V8_20, morphCB:morphBGE_TYPEA2},
    [MICROBLAZE_IT_BGED_TYPEA2] = {arch:V8_20, morphCB:morphBGED_TYPEA2},
    [MICROBLAZE_IT_BGT_TYPEA2] = {arch:V8_20, morphCB:morphBGT_TYPEA2},
    [MICROBLAZE_IT_BGTD_TYPEA2] = {arch:V8_20, morphCB:morphBGTD_TYPEA2},
    [MICROBLAZE_IT_BLE_TYPEA2] = {arch:V8_20, morphCB:morphBLE_TYPEA2},
    [MICROBLAZE_IT_BLED_TYPEA2] = {arch:V8_20, morphCB:morphBLED_TYPEA2},
    [MICROBLAZE_IT_BLT_TYPEA2] = {arch:V8_20, morphCB:morphBLT_TYPEA2},
    [MICROBLAZE_IT_BLTD_TYPEA2] = {arch:V8_20, morphCB:morphBLTD_TYPEA2},
    [MICROBLAZE_IT_BNE_TYPEA2] = {arch:V8_20, morphCB:morphBNE_TYPEA2},
    [MICROBLAZE_IT_BNED_TYPEA2] = {arch:V8_20, morphCB:morphBNED_TYPEA2},

    //
    // Format TypeA3 Insns
    //
    [MICROBLAZE_IT_BR_TYPEA3] = {arch:V8_20, morphCB:morphBR_TYPEA3},
    [MICROBLAZE_IT_BRA_TYPEA3] = {arch:V8_20, morphCB:morphBRA_TYPEA3},
    [MICROBLAZE_IT_BRAD_TYPEA3] = {arch:V8_20, morphCB:morphBRAD_TYPEA3},
    [MICROBLAZE_IT_BRD_TYPEA3] = {arch:V8_20, morphCB:morphBRD_TYPEA3},

    //
    // Format TypeA4 Insns
    //
    [MICROBLAZE_IT_BRALD_TYPEA4] = {arch:V8_20, morphCB:morphBRALD_TYPEA4},
    [MICROBLAZE_IT_BRLD_TYPEA4] = {arch:V8_20, morphCB:morphBRLD_TYPEA4},

    //
    // Format TypeA5 Insns
    //
    [MICROBLAZE_IT_BRK_TYPEA5] = {arch:V8_20, morphCB:morphBRK_TYPEA5},

    //
    // Format TypeA6 Insns
    //
    [MICROBLAZE_IT_BSLL_TYPEA6] = {arch:V8_20, morphCB:morphBSLL_TYPEA6},
    [MICROBLAZE_IT_BSRA_TYPEA6] = {arch:V8_20, morphCB:morphBSRA_TYPEA6},
    [MICROBLAZE_IT_BSRL_TYPEA6] = {arch:V8_20, morphCB:morphBSRL_TYPEA6},

    //
    // Format TypeA7 Insns
    //
    [MICROBLAZE_IT_CMP_TYPEA7] = {arch:V8_20, morphCB:morphCMP_TYPEA7},
    [MICROBLAZE_IT_CMPU_TYPEA7] = {arch:V8_20, morphCB:morphCMPU_TYPEA7},
    [MICROBLAZE_IT_IDIV_TYPEA7] = {arch:V8_20, morphCB:morphIDIV_TYPEA7},
    [MICROBLAZE_IT_IDIVU_TYPEA7] = {arch:V8_20, morphCB:morphIDIVU_TYPEA7},

    //
    // Format TypeA8 Insns
    //
    [MICROBLAZE_IT_LBU_TYPEA8] = {arch:V8_20, morphCB:morphLBU_TYPEA8},
    [MICROBLAZE_IT_LBUR_TYPEA8] = {arch:V8_20, morphCB:morphLBUR_TYPEA8},
    [MICROBLAZE_IT_LHU_TYPEA8] = {arch:V8_20, morphCB:morphLHU_TYPEA8},
    [MICROBLAZE_IT_LHUR_TYPEA8] = {arch:V8_20, morphCB:morphLHUR_TYPEA8},
    [MICROBLAZE_IT_LW_TYPEA8] = {arch:V8_20, morphCB:morphLW_TYPEA8},
    [MICROBLAZE_IT_LWR_TYPEA8] = {arch:V8_20, morphCB:morphLWR_TYPEA8},
    [MICROBLAZE_IT_SB_TYPEA8] = {arch:V8_20, morphCB:morphSB_TYPEA8},
    [MICROBLAZE_IT_SBR_TYPEA8] = {arch:V8_20, morphCB:morphSBR_TYPEA8},
    [MICROBLAZE_IT_SH_TYPEA8] = {arch:V8_20, morphCB:morphSH_TYPEA8},
    [MICROBLAZE_IT_SHR_TYPEA8] = {arch:V8_20, morphCB:morphSHR_TYPEA8},
    [MICROBLAZE_IT_SW_TYPEA8] = {arch:V8_20, morphCB:morphSW_TYPEA8},
    [MICROBLAZE_IT_SWR_TYPEA8] = {arch:V8_20, morphCB:morphSWR_TYPEA8},

    //
    // Format TypeA9 Insns
    //
    [MICROBLAZE_IT_MFS_TYPEA9] = {arch:V8_20, morphCB:morphMFS_TYPEA9},

    //
    // Format TypeAa Insns
    //
    [MICROBLAZE_IT_MTS_TYPEAA] = {arch:V8_20, morphCB:morphMTS_TYPEAA},

    //
    // Format TypeAb Insns
    //
    [MICROBLAZE_IT_CLZ_TYPEAB] = {arch:V8_20, morphCB:morphCLZ_TYPEAB},
    [MICROBLAZE_IT_FINT_TYPEAB] = {arch:V8_20, morphCB:morphFINT_TYPEAB},
    [MICROBLAZE_IT_FLT_TYPEAB] = {arch:V8_20, morphCB:morphFLT_TYPEAB},
    [MICROBLAZE_IT_FSQRT_TYPEAB] = {arch:V8_20, morphCB:morphFSQRT_TYPEAB},
    [MICROBLAZE_IT_SEXT16_TYPEAB] = {arch:V8_20, morphCB:morphSEXT16_TYPEAB},
    [MICROBLAZE_IT_SEXT8_TYPEAB] = {arch:V8_20, morphCB:morphSEXT8_TYPEAB},
    [MICROBLAZE_IT_SRA_TYPEAB] = {arch:V8_20, morphCB:morphSRA_TYPEAB},
    [MICROBLAZE_IT_SRC_TYPEAB] = {arch:V8_20, morphCB:morphSRC_TYPEAB},
    [MICROBLAZE_IT_SRL_TYPEAB] = {arch:V8_20, morphCB:morphSRL_TYPEAB},

    //
    // Format TypeAc Insns
    //
    [MICROBLAZE_IT_WDC_TYPEAC] = {arch:V8_20, morphCB:morphWDC_TYPEAC},
    [MICROBLAZE_IT_WDCCLR_TYPEAC] = {arch:V8_20, morphCB:morphWDCCLR_TYPEAC},
    [MICROBLAZE_IT_WDCFL_TYPEAC] = {arch:V8_20, morphCB:morphWDCFL_TYPEAC},
    [MICROBLAZE_IT_WIC_TYPEAC] = {arch:V8_20, morphCB:morphWIC_TYPEAC},

    //
    // Format TypeB1 Insns
    //
    [MICROBLAZE_IT_ADDI_TYPEB1] = {arch:V8_20, morphCB:morphADDI_TYPEB1},
    [MICROBLAZE_IT_ADDIC_TYPEB1] = {arch:V8_20, morphCB:morphADDIC_TYPEB1},
    [MICROBLAZE_IT_ADDIK_TYPEB1] = {arch:V8_20, morphCB:morphADDIK_TYPEB1},
    [MICROBLAZE_IT_ADDIKC_TYPEB1] = {arch:V8_20, morphCB:morphADDIKC_TYPEB1},
    [MICROBLAZE_IT_ANDI_TYPEB1] = {arch:V8_20, morphCB:morphANDI_TYPEB1},
    [MICROBLAZE_IT_ANDNI_TYPEB1] = {arch:V8_20, morphCB:morphANDNI_TYPEB1},
    [MICROBLAZE_IT_LBUI_TYPEB1] = {arch:V8_20, morphCB:morphLBUI_TYPEB1},
    [MICROBLAZE_IT_LHUI_TYPEB1] = {arch:V8_20, morphCB:morphLHUI_TYPEB1},
    [MICROBLAZE_IT_LWI_TYPEB1] = {arch:V8_20, morphCB:morphLWI_TYPEB1},
    [MICROBLAZE_IT_MULI_TYPEB1] = {arch:V8_20, morphCB:morphMULI_TYPEB1},
    [MICROBLAZE_IT_ORI_TYPEB1] = {arch:V8_20, morphCB:morphORI_TYPEB1},
    [MICROBLAZE_IT_RSUBI_TYPEB1] = {arch:V8_20, morphCB:morphRSUBI_TYPEB1},
    [MICROBLAZE_IT_RSUBIC_TYPEB1] = {arch:V8_20, morphCB:morphRSUBIC_TYPEB1},
    [MICROBLAZE_IT_RSUBIK_TYPEB1] = {arch:V8_20, morphCB:morphRSUBIK_TYPEB1},
    [MICROBLAZE_IT_RSUBIKC_TYPEB1] = {arch:V8_20, morphCB:morphRSUBIKC_TYPEB1},
    [MICROBLAZE_IT_SBI_TYPEB1] = {arch:V8_20, morphCB:morphSBI_TYPEB1},
    [MICROBLAZE_IT_SHI_TYPEB1] = {arch:V8_20, morphCB:morphSHI_TYPEB1},
    [MICROBLAZE_IT_SWI_TYPEB1] = {arch:V8_20, morphCB:morphSWI_TYPEB1},
    [MICROBLAZE_IT_XORI_TYPEB1] = {arch:V8_20, morphCB:morphXORI_TYPEB1},

    //
    // Format TypeB2 Insns
    //
    [MICROBLAZE_IT_BEQI_TYPEB2] = {arch:V8_20, morphCB:morphBEQI_TYPEB2},
    [MICROBLAZE_IT_BEQID_TYPEB2] = {arch:V8_20, morphCB:morphBEQID_TYPEB2},
    [MICROBLAZE_IT_BGEI_TYPEB2] = {arch:V8_20, morphCB:morphBGEI_TYPEB2},
    [MICROBLAZE_IT_BGEID_TYPEB2] = {arch:V8_20, morphCB:morphBGEID_TYPEB2},
    [MICROBLAZE_IT_BGTI_TYPEB2] = {arch:V8_20, morphCB:morphBGTI_TYPEB2},
    [MICROBLAZE_IT_BGTID_TYPEB2] = {arch:V8_20, morphCB:morphBGTID_TYPEB2},
    [MICROBLAZE_IT_BLEI_TYPEB2] = {arch:V8_20, morphCB:morphBLEI_TYPEB2},
    [MICROBLAZE_IT_BLEID_TYPEB2] = {arch:V8_20, morphCB:morphBLEID_TYPEB2},
    [MICROBLAZE_IT_BLTI_TYPEB2] = {arch:V8_20, morphCB:morphBLTI_TYPEB2},
    [MICROBLAZE_IT_BLTID_TYPEB2] = {arch:V8_20, morphCB:morphBLTID_TYPEB2},
    [MICROBLAZE_IT_BNEI_TYPEB2] = {arch:V8_20, morphCB:morphBNEI_TYPEB2},
    [MICROBLAZE_IT_BNEID_TYPEB2] = {arch:V8_20, morphCB:morphBNEID_TYPEB2},

    //
    // Format TypeB3 Insns
    //
    [MICROBLAZE_IT_BRAI_TYPEB3] = {arch:V8_20, morphCB:morphBRAI_TYPEB3},
    [MICROBLAZE_IT_BRAID_TYPEB3] = {arch:V8_20, morphCB:morphBRAID_TYPEB3},
    [MICROBLAZE_IT_BRI_TYPEB3] = {arch:V8_20, morphCB:morphBRI_TYPEB3},
    [MICROBLAZE_IT_BRID_TYPEB3] = {arch:V8_20, morphCB:morphBRID_TYPEB3},

    //
    // Format TypeB4 Insns
    //
    [MICROBLAZE_IT_BRALID_TYPEB4] = {arch:V8_20, morphCB:morphBRALID_TYPEB4},
    [MICROBLAZE_IT_BRLID_TYPEB4] = {arch:V8_20, morphCB:morphBRLID_TYPEB4},

    //
    // Format TypeB5 Insns
    //
    [MICROBLAZE_IT_BRKI_TYPEB5] = {arch:V8_20, morphCB:morphBRKI_TYPEB5},

    //
    // Format TypeB6 Insns
    //
    [MICROBLAZE_IT_BSLLI_TYPEB6] = {arch:V8_20, morphCB:morphBSLLI_TYPEB6},
    [MICROBLAZE_IT_BSRAI_TYPEB6] = {arch:V8_20, morphCB:morphBSRAI_TYPEB6},
    [MICROBLAZE_IT_BSRLI_TYPEB6] = {arch:V8_20, morphCB:morphBSRLI_TYPEB6},

    //
    // Format TypeB8 Insns
    //
    [MICROBLAZE_IT_MBAR_TYPEB8] = {arch:V8_20, morphCB:morphMBAR_TYPEB8},

    //
    // Format TypeB9 Insns
    //
    [MICROBLAZE_IT_MSRCLR_TYPEB9] = {arch:V8_20, morphCB:morphMSRCLR_TYPEB9},
    [MICROBLAZE_IT_MSRSET_TYPEB9] = {arch:V8_20, morphCB:morphMSRSET_TYPEB9},

    //
    // Format TypeBa Insns
    //
    [MICROBLAZE_IT_RTBD_TYPEBA] = {arch:V8_20, morphCB:morphRTBD_TYPEBA},
    [MICROBLAZE_IT_RTED_TYPEBA] = {arch:V8_20, morphCB:morphRTED_TYPEBA},
    [MICROBLAZE_IT_RTID_TYPEBA] = {arch:V8_20, morphCB:morphRTID_TYPEBA},
    [MICROBLAZE_IT_RTSD_TYPEBA] = {arch:V8_20, morphCB:morphRTSD_TYPEBA},

    //
    // Format TypeC1 Insns
    //
    [MICROBLAZE_IT_AGET_TYPEC1] = {arch:V8_20, morphCB:morphAGET_TYPEC1},
    [MICROBLAZE_IT_CAGET_TYPEC1] = {arch:V8_20, morphCB:morphCAGET_TYPEC1},
    [MICROBLAZE_IT_CGET_TYPEC1] = {arch:V8_20, morphCB:morphCGET_TYPEC1},
    [MICROBLAZE_IT_EAGET_TYPEC1] = {arch:V8_20, morphCB:morphEAGET_TYPEC1},
    [MICROBLAZE_IT_ECAGET_TYPEC1] = {arch:V8_20, morphCB:morphECAGET_TYPEC1},
    [MICROBLAZE_IT_ECGET_TYPEC1] = {arch:V8_20, morphCB:morphECGET_TYPEC1},
    [MICROBLAZE_IT_EGET_TYPEC1] = {arch:V8_20, morphCB:morphEGET_TYPEC1},
    [MICROBLAZE_IT_GET_TYPEC1] = {arch:V8_20, morphCB:morphGET_TYPEC1},
    [MICROBLAZE_IT_NAGET_TYPEC1] = {arch:V8_20, morphCB:morphNAGET_TYPEC1},
    [MICROBLAZE_IT_NCAGET_TYPEC1] = {arch:V8_20, morphCB:morphNCAGET_TYPEC1},
    [MICROBLAZE_IT_NCGET_TYPEC1] = {arch:V8_20, morphCB:morphNCGET_TYPEC1},
    [MICROBLAZE_IT_NEAGET_TYPEC1] = {arch:V8_20, morphCB:morphNEAGET_TYPEC1},
    [MICROBLAZE_IT_NECAGET_TYPEC1] = {arch:V8_20, morphCB:morphNECAGET_TYPEC1},
    [MICROBLAZE_IT_NECGET_TYPEC1] = {arch:V8_20, morphCB:morphNECGET_TYPEC1},
    [MICROBLAZE_IT_NEGET_TYPEC1] = {arch:V8_20, morphCB:morphNEGET_TYPEC1},
    [MICROBLAZE_IT_NGET_TYPEC1] = {arch:V8_20, morphCB:morphNGET_TYPEC1},
    [MICROBLAZE_IT_TAGET_TYPEC1] = {arch:V8_20, morphCB:morphTAGET_TYPEC1},
    [MICROBLAZE_IT_TCAGET_TYPEC1] = {arch:V8_20, morphCB:morphTCAGET_TYPEC1},
    [MICROBLAZE_IT_TCGET_TYPEC1] = {arch:V8_20, morphCB:morphTCGET_TYPEC1},
    [MICROBLAZE_IT_TEAGET_TYPEC1] = {arch:V8_20, morphCB:morphTEAGET_TYPEC1},
    [MICROBLAZE_IT_TECAGET_TYPEC1] = {arch:V8_20, morphCB:morphTECAGET_TYPEC1},
    [MICROBLAZE_IT_TECGET_TYPEC1] = {arch:V8_20, morphCB:morphTECGET_TYPEC1},
    [MICROBLAZE_IT_TEGET_TYPEC1] = {arch:V8_20, morphCB:morphTEGET_TYPEC1},
    [MICROBLAZE_IT_TGET_TYPEC1] = {arch:V8_20, morphCB:morphTGET_TYPEC1},
    [MICROBLAZE_IT_TNAGET_TYPEC1] = {arch:V8_20, morphCB:morphTNAGET_TYPEC1},
    [MICROBLAZE_IT_TNCAGET_TYPEC1] = {arch:V8_20, morphCB:morphTNCAGET_TYPEC1},
    [MICROBLAZE_IT_TNCGET_TYPEC1] = {arch:V8_20, morphCB:morphTNCGET_TYPEC1},
    [MICROBLAZE_IT_TNEAGET_TYPEC1] = {arch:V8_20, morphCB:morphTNEAGET_TYPEC1},
    [MICROBLAZE_IT_TNECAGET_TYPEC1] = {arch:V8_20, morphCB:morphTNECAGET_TYPEC1},
    [MICROBLAZE_IT_TNECGET_TYPEC1] = {arch:V8_20, morphCB:morphTNECGET_TYPEC1},
    [MICROBLAZE_IT_TNEGET_TYPEC1] = {arch:V8_20, morphCB:morphTNEGET_TYPEC1},
    [MICROBLAZE_IT_TNGET_TYPEC1] = {arch:V8_20, morphCB:morphTNGET_TYPEC1},

    //
    // Format TypeC2 Insns
    //
    [MICROBLAZE_IT_AGETD_TYPEC2] = {arch:V8_20, morphCB:morphAGETD_TYPEC2},
    [MICROBLAZE_IT_CAGETD_TYPEC2] = {arch:V8_20, morphCB:morphCAGETD_TYPEC2},
    [MICROBLAZE_IT_CGETD_TYPEC2] = {arch:V8_20, morphCB:morphCGETD_TYPEC2},
    [MICROBLAZE_IT_EAGETD_TYPEC2] = {arch:V8_20, morphCB:morphEAGETD_TYPEC2},
    [MICROBLAZE_IT_ECAGETD_TYPEC2] = {arch:V8_20, morphCB:morphECAGETD_TYPEC2},
    [MICROBLAZE_IT_ECGETD_TYPEC2] = {arch:V8_20, morphCB:morphECGETD_TYPEC2},
    [MICROBLAZE_IT_EGETD_TYPEC2] = {arch:V8_20, morphCB:morphEGETD_TYPEC2},
    [MICROBLAZE_IT_GETD_TYPEC2] = {arch:V8_20, morphCB:morphGETD_TYPEC2},
    [MICROBLAZE_IT_NAGETD_TYPEC2] = {arch:V8_20, morphCB:morphNAGETD_TYPEC2},
    [MICROBLAZE_IT_NCAGETD_TYPEC2] = {arch:V8_20, morphCB:morphNCAGETD_TYPEC2},
    [MICROBLAZE_IT_NCGETD_TYPEC2] = {arch:V8_20, morphCB:morphNCGETD_TYPEC2},
    [MICROBLAZE_IT_NEAGETD_TYPEC2] = {arch:V8_20, morphCB:morphNEAGETD_TYPEC2},
    [MICROBLAZE_IT_NECAGETD_TYPEC2] = {arch:V8_20, morphCB:morphNECAGETD_TYPEC2},
    [MICROBLAZE_IT_NECGETD_TYPEC2] = {arch:V8_20, morphCB:morphNECGETD_TYPEC2},
    [MICROBLAZE_IT_NEGETD_TYPEC2] = {arch:V8_20, morphCB:morphNEGETD_TYPEC2},
    [MICROBLAZE_IT_NGETD_TYPEC2] = {arch:V8_20, morphCB:morphNGETD_TYPEC2},
    [MICROBLAZE_IT_TAGETD_TYPEC2] = {arch:V8_20, morphCB:morphTAGETD_TYPEC2},
    [MICROBLAZE_IT_TCAGETD_TYPEC2] = {arch:V8_20, morphCB:morphTCAGETD_TYPEC2},
    [MICROBLAZE_IT_TCGETD_TYPEC2] = {arch:V8_20, morphCB:morphTCGETD_TYPEC2},
    [MICROBLAZE_IT_TEAGETD_TYPEC2] = {arch:V8_20, morphCB:morphTEAGETD_TYPEC2},
    [MICROBLAZE_IT_TECAGETD_TYPEC2] = {arch:V8_20, morphCB:morphTECAGETD_TYPEC2},
    [MICROBLAZE_IT_TECGETD_TYPEC2] = {arch:V8_20, morphCB:morphTECGETD_TYPEC2},
    [MICROBLAZE_IT_TEGETD_TYPEC2] = {arch:V8_20, morphCB:morphTEGETD_TYPEC2},
    [MICROBLAZE_IT_TGETD_TYPEC2] = {arch:V8_20, morphCB:morphTGETD_TYPEC2},
    [MICROBLAZE_IT_TNAGETD_TYPEC2] = {arch:V8_20, morphCB:morphTNAGETD_TYPEC2},
    [MICROBLAZE_IT_TNCAGETD_TYPEC2] = {arch:V8_20, morphCB:morphTNCAGETD_TYPEC2},
    [MICROBLAZE_IT_TNCGETD_TYPEC2] = {arch:V8_20, morphCB:morphTNCGETD_TYPEC2},
    [MICROBLAZE_IT_TNEAGETD_TYPEC2] = {arch:V8_20, morphCB:morphTNEAGETD_TYPEC2},
    [MICROBLAZE_IT_TNECAGETD_TYPEC2] = {arch:V8_20, morphCB:morphTNECAGETD_TYPEC2},
    [MICROBLAZE_IT_TNECGETD_TYPEC2] = {arch:V8_20, morphCB:morphTNECGETD_TYPEC2},
    [MICROBLAZE_IT_TNEGETD_TYPEC2] = {arch:V8_20, morphCB:morphTNEGETD_TYPEC2},
    [MICROBLAZE_IT_TNGETD_TYPEC2] = {arch:V8_20, morphCB:morphTNGETD_TYPEC2},

    //
    // Format TypeC3 Insns
    //
    [MICROBLAZE_IT_APUT_TYPEC3] = {arch:V8_20, morphCB:morphAPUT_TYPEC3},
    [MICROBLAZE_IT_CAPUT_TYPEC3] = {arch:V8_20, morphCB:morphCAPUT_TYPEC3},
    [MICROBLAZE_IT_CPUT_TYPEC3] = {arch:V8_20, morphCB:morphCPUT_TYPEC3},
    [MICROBLAZE_IT_NAPUT_TYPEC3] = {arch:V8_20, morphCB:morphNAPUT_TYPEC3},
    [MICROBLAZE_IT_NCAPUT_TYPEC3] = {arch:V8_20, morphCB:morphNCAPUT_TYPEC3},
    [MICROBLAZE_IT_NCPUT_TYPEC3] = {arch:V8_20, morphCB:morphNCPUT_TYPEC3},
    [MICROBLAZE_IT_NPUT_TYPEC3] = {arch:V8_20, morphCB:morphNPUT_TYPEC3},
    [MICROBLAZE_IT_PUT_TYPEC3] = {arch:V8_20, morphCB:morphPUT_TYPEC3},

    //
    // Format TypeC3d Insns
    //
    [MICROBLAZE_IT_APUTD_TYPEC3D] = {arch:V8_20, morphCB:morphAPUTD_TYPEC3D},
    [MICROBLAZE_IT_CAPUTD_TYPEC3D] = {arch:V8_20, morphCB:morphCAPUTD_TYPEC3D},
    [MICROBLAZE_IT_CPUTD_TYPEC3D] = {arch:V8_20, morphCB:morphCPUTD_TYPEC3D},
    [MICROBLAZE_IT_NAPUTD_TYPEC3D] = {arch:V8_20, morphCB:morphNAPUTD_TYPEC3D},
    [MICROBLAZE_IT_NCAPUTD_TYPEC3D] = {arch:V8_20, morphCB:morphNCAPUTD_TYPEC3D},
    [MICROBLAZE_IT_NCPUTD_TYPEC3D] = {arch:V8_20, morphCB:morphNCPUTD_TYPEC3D},
    [MICROBLAZE_IT_NPUTD_TYPEC3D] = {arch:V8_20, morphCB:morphNPUTD_TYPEC3D},
    [MICROBLAZE_IT_PUTD_TYPEC3D] = {arch:V8_20, morphCB:morphPUTD_TYPEC3D},

    //
    // Format TypeC4 Insns
    //
    [MICROBLAZE_IT_TAPUT_TYPEC4] = {arch:V8_20, morphCB:morphTAPUT_TYPEC4},
    [MICROBLAZE_IT_TCAPUT_TYPEC4] = {arch:V8_20, morphCB:morphTCAPUT_TYPEC4},
    [MICROBLAZE_IT_TCPUT_TYPEC4] = {arch:V8_20, morphCB:morphTCPUT_TYPEC4},
    [MICROBLAZE_IT_TNAPUT_TYPEC4] = {arch:V8_20, morphCB:morphTNAPUT_TYPEC4},
    [MICROBLAZE_IT_TNCAPUT_TYPEC4] = {arch:V8_20, morphCB:morphTNCAPUT_TYPEC4},
    [MICROBLAZE_IT_TNCPUT_TYPEC4] = {arch:V8_20, morphCB:morphTNCPUT_TYPEC4},
    [MICROBLAZE_IT_TNPUT_TYPEC4] = {arch:V8_20, morphCB:morphTNPUT_TYPEC4},
    [MICROBLAZE_IT_TPUT_TYPEC4] = {arch:V8_20, morphCB:morphTPUT_TYPEC4},

    //
    // Format TypeC4d Insns
    //
    [MICROBLAZE_IT_TAPUTD_TYPEC4D] = {arch:V8_20, morphCB:morphTAPUTD_TYPEC4D},
    [MICROBLAZE_IT_TCAPUTD_TYPEC4D] = {arch:V8_20, morphCB:morphTCAPUTD_TYPEC4D},
    [MICROBLAZE_IT_TCPUTD_TYPEC4D] = {arch:V8_20, morphCB:morphTCPUTD_TYPEC4D},
    [MICROBLAZE_IT_TNAPUTD_TYPEC4D] = {arch:V8_20, morphCB:morphTNAPUTD_TYPEC4D},
    [MICROBLAZE_IT_TNCAPUTD_TYPEC4D] = {arch:V8_20, morphCB:morphTNCAPUTD_TYPEC4D},
    [MICROBLAZE_IT_TNCPUTD_TYPEC4D] = {arch:V8_20, morphCB:morphTNCPUTD_TYPEC4D},
    [MICROBLAZE_IT_TNPUTD_TYPEC4D] = {arch:V8_20, morphCB:morphTNPUTD_TYPEC4D},
    [MICROBLAZE_IT_TPUTD_TYPEC4D] = {arch:V8_20, morphCB:morphTPUTD_TYPEC4D},

    //
    // Format TypeI1 Insns
    //
    [MICROBLAZE_IT_IMM_TYPEI1] = {arch:V8_20, morphCB:morphIMM_TYPEI1},

    //
    // Format TypeX Insns
    //
    [MICROBLAZE_IT_NOP_TYPEX] = {arch:V8_20, morphCB:morphNOP_TYPEX},

};

//
// Default undecoded/badvariant/unimplemented instruction behavior
//
static void defaultMorphCallback(
    vmiProcessorP processor,
    microblazeAddr thisPC,
    Uns32 instruction,
    char *message
) {
    vmiPrintf(
        "CPU '%s' 0x%08x:0x%08x *** %s instruction: exiting ***\n",
        vmirtProcessorName(processor),
        thisPC,
        instruction,
        message
    );
    vmirtExit(processor);
}

//
// Default morphed code call
//
static MICROBLAZE_MORPH_FN(emitUndefined) {
    TYPE_SPR_ESR ESR = {0};
    ESR.bits.EC = ILLEGAL_OPCODE_EXCEPTION;
    ESR.bits_ESS_EC_00001.Rx = state->info.rD;
    vmimtArgProcessor();
    vmimtArgUns32(ESR.reg);
    vmimtCall((vmiCallFn)microblazehwExceptionCB);
}
static MICROBLAZE_MORPH_FN(emitNotVariant) {
    vmimtArgProcessor();
    vmimtArgUns32(state->info.thisPC);
    vmimtArgUns32(state->info.instruction);
    vmimtArgNatAddress("badvariant");
    vmimtCall((vmiCallFn)defaultMorphCallback);
}
static MICROBLAZE_MORPH_FN(emitAbsentFeature) {
    vmimtArgProcessor();
    vmimtArgUns32(state->info.thisPC);
    vmimtArgUns32(state->info.instruction);
    vmimtArgNatAddress("Absent Feature");
    vmimtCall((vmiCallFn)defaultMorphCallback);
}
static MICROBLAZE_MORPH_FN(emitUnimplemented) {
    vmimtArgProcessor();
    vmimtArgUns32(state->info.thisPC);
    vmimtArgUns32(state->info.instruction);
    vmimtArgNatAddress("unimplemented");
    vmimtCall((vmiCallFn)defaultMorphCallback);
}

static Bool disableMorph(microblazeMorphStateP state) {
    microblazeP microblaze = state->microblaze;
    if(!MICROBLAZE_DISASSEMBLE(microblaze)) {
        return False;
    } else {
        return True;
    }
}

//
// Call to implemented morph code instruction generator
//
static MICROBLAZE_MORPH_FN(emitImplemented) {
    state->attrs->morphCB(state);
}

//
// Return a boolean indicating whether the processor supports the required
// architecture
//
static Bool supportedOnVariant(microblazeP microblaze, microblazeMorphStateP state) {

    microblazeArchitecture configVariant = microblaze->configInfo.arch;
    microblazeArchitecture requiredVariant = state->attrs->arch;

    if (configVariant & requiredVariant) {
        return True;
    } else {
        return False;
    }
}

//
// Determine if this C_USE_* feature is enabled in the core
//
typedef enum microblazeFeatureE {
    FR_NONE = 0,                // no special feature requirement
    FR_C_USE_BARREL,            // Requires Barrel Shifter
    FR_C_USE_DIV,               // Requires hardware divider
    FR_C_USE_HW_MUL,            // Requires hardware multiplier
    FR_C_USE_HW_MUL64,          // Requires hardware 64 bit multiplier
    FR_C_USE_FPU,               // Requires FPU
    FR_C_USE_FPU2,              // Requires FPU (extended)
    FR_C_USE_MSR_INSTR,         // Requires MSRSET/MSRCLR
    FR_C_USE_PCMP_INSTR,        // Requires PCMP*/CLZ instructions
    FR_C_USE_FSL_LINKS,         // Requires FSL
    FR_C_USE_EXTENDED_FSL_INSTR // Requires Extended FSL
} microblazeFeature;

//
// This defines instructions that require particular features to be present
// (omitted instructions have no special feature requirements)
//
#define FEATURE_REQUIRE(_NAME, _FR) [MICROBLAZE_IT_##_NAME] = _FR
const static microblazeFeature featureRequired[MICROBLAZE_IT_LAST+1] = {

    // Barrel shifter functions C_USE_BARREL
    FEATURE_REQUIRE(BSLL_TYPEA6, FR_C_USE_BARREL),
    FEATURE_REQUIRE(BSLL_TYPEA6, FR_C_USE_BARREL),
    FEATURE_REQUIRE(BSRA_TYPEA6, FR_C_USE_BARREL),
    FEATURE_REQUIRE(BSRL_TYPEA6, FR_C_USE_BARREL),
    FEATURE_REQUIRE(BSLLI_TYPEB6, FR_C_USE_BARREL),
    FEATURE_REQUIRE(BSRAI_TYPEB6, FR_C_USE_BARREL),
    FEATURE_REQUIRE(BSRLI_TYPEB6, FR_C_USE_BARREL),

    // Divider check, C_USE_DIV
    FEATURE_REQUIRE(IDIV_TYPEA7, FR_C_USE_DIV),
    FEATURE_REQUIRE(IDIVU_TYPEA7, FR_C_USE_DIV),

    // HW Multiply check, C_USE_HW_MUL
    FEATURE_REQUIRE(MUL_TYPEA1, FR_C_USE_HW_MUL),
    FEATURE_REQUIRE(MULI_TYPEB1, FR_C_USE_HW_MUL),

    // HW Multiply check, C_USE_HW_MUL (64)
    FEATURE_REQUIRE(MULH_TYPEA1, FR_C_USE_HW_MUL64),
    FEATURE_REQUIRE(MULHSU_TYPEA1, FR_C_USE_HW_MUL64),
    FEATURE_REQUIRE(MULHU_TYPEA1, FR_C_USE_HW_MUL64),

    // FPU check, C_USE_FPU
    FEATURE_REQUIRE(FADD_TYPEA1, FR_C_USE_FPU),
    FEATURE_REQUIRE(FRSUB_TYPEA1, FR_C_USE_FPU),
    FEATURE_REQUIRE(FMUL_TYPEA1, FR_C_USE_FPU),
    FEATURE_REQUIRE(FDIV_TYPEA1, FR_C_USE_FPU),
    FEATURE_REQUIRE(FCMPEQ_TYPEA1, FR_C_USE_FPU),
    FEATURE_REQUIRE(FCMPGE_TYPEA1, FR_C_USE_FPU),
    FEATURE_REQUIRE(FCMPGT_TYPEA1, FR_C_USE_FPU),
    FEATURE_REQUIRE(FCMPLE_TYPEA1, FR_C_USE_FPU),
    FEATURE_REQUIRE(FCMPLT_TYPEA1, FR_C_USE_FPU),
    FEATURE_REQUIRE(FCMPNE_TYPEA1, FR_C_USE_FPU),
    FEATURE_REQUIRE(FCMPUN_TYPEA1, FR_C_USE_FPU),

    // FPU check, C_USE_FPU (2)
    FEATURE_REQUIRE(FLT_TYPEAB, FR_C_USE_FPU2),
    FEATURE_REQUIRE(FINT_TYPEAB, FR_C_USE_FPU2),
    FEATURE_REQUIRE(FSQRT_TYPEAB, FR_C_USE_FPU2),

    // MSRSET/MSRCLR, C_USE_MSR_INSTR
    FEATURE_REQUIRE(MSRCLR_TYPEB9, FR_C_USE_MSR_INSTR),
    FEATURE_REQUIRE(MSRSET_TYPEB9, FR_C_USE_MSR_INSTR),

    // PCMP/CLZ, C_USE_PCMP_INSTR
    FEATURE_REQUIRE(PCMPBF_TYPEA1, FR_C_USE_PCMP_INSTR),
    FEATURE_REQUIRE(PCMPEQ_TYPEA1, FR_C_USE_PCMP_INSTR),
    FEATURE_REQUIRE(PCMPNE_TYPEA1, FR_C_USE_PCMP_INSTR),
    FEATURE_REQUIRE(CLZ_TYPEAB, FR_C_USE_PCMP_INSTR),

    // GET/PUT, C_FSL_LINKS
    FEATURE_REQUIRE(CGET_TYPEC1, FR_C_USE_FSL_LINKS),
    FEATURE_REQUIRE(GET_TYPEC1, FR_C_USE_FSL_LINKS),
    FEATURE_REQUIRE(NCGET_TYPEC1, FR_C_USE_FSL_LINKS),
    FEATURE_REQUIRE(NGET_TYPEC1, FR_C_USE_FSL_LINKS),
    FEATURE_REQUIRE(CPUT_TYPEC3, FR_C_USE_FSL_LINKS),
    FEATURE_REQUIRE(NCPUT_TYPEC3, FR_C_USE_FSL_LINKS),
    FEATURE_REQUIRE(NPUT_TYPEC3, FR_C_USE_FSL_LINKS),
    FEATURE_REQUIRE(PUT_TYPEC3, FR_C_USE_FSL_LINKS),

    // C_USE_EXTENDED_FSL_INSTR
    FEATURE_REQUIRE(AGET_TYPEC1, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(CAGET_TYPEC1, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(EAGET_TYPEC1, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(ECAGET_TYPEC1, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(ECGET_TYPEC1, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(EGET_TYPEC1, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(NAGET_TYPEC1, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(NCAGET_TYPEC1, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(NEAGET_TYPEC1, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(NECAGET_TYPEC1, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(NECGET_TYPEC1, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(NEGET_TYPEC1, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(TAGET_TYPEC1, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(TCAGET_TYPEC1, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(TCGET_TYPEC1, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(TEAGET_TYPEC1, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(TECAGET_TYPEC1, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(TECGET_TYPEC1, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(TEGET_TYPEC1, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(TGET_TYPEC1, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(TNAGET_TYPEC1, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(TNCAGET_TYPEC1, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(TNCGET_TYPEC1, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(TNEAGET_TYPEC1, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(TNECAGET_TYPEC1, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(TNECGET_TYPEC1, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(TNEGET_TYPEC1, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(TNGET_TYPEC1, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(AGETD_TYPEC2, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(CAGETD_TYPEC2, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(CGETD_TYPEC2, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(EAGETD_TYPEC2, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(ECAGETD_TYPEC2, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(ECGETD_TYPEC2, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(EGETD_TYPEC2, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(GETD_TYPEC2, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(NAGETD_TYPEC2, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(NCAGETD_TYPEC2, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(NCGETD_TYPEC2, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(NEAGETD_TYPEC2, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(NECAGETD_TYPEC2, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(NECGETD_TYPEC2, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(NEGETD_TYPEC2, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(NGETD_TYPEC2, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(TAGETD_TYPEC2, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(TCAGETD_TYPEC2, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(TCGETD_TYPEC2, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(TEAGETD_TYPEC2, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(TECAGETD_TYPEC2, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(TECGETD_TYPEC2, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(TEGETD_TYPEC2, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(TGETD_TYPEC2, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(TNAGETD_TYPEC2, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(TNCAGETD_TYPEC2, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(TNCGETD_TYPEC2, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(TNEAGETD_TYPEC2, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(TNECAGETD_TYPEC2, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(TNECGETD_TYPEC2, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(TNEGETD_TYPEC2, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(TNGETD_TYPEC2, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(APUT_TYPEC3, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(CAPUT_TYPEC3, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(NAPUT_TYPEC3, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(NCAPUT_TYPEC3, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(APUTD_TYPEC3D, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(CAPUTD_TYPEC3D, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(NAPUTD_TYPEC3D, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(NCAPUTD_TYPEC3D, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(CPUTD_TYPEC3D, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(NCPUTD_TYPEC3D, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(NPUTD_TYPEC3D, FR_C_USE_EXTENDED_FSL_INSTR),
    FEATURE_REQUIRE(PUTD_TYPEC3D, FR_C_USE_EXTENDED_FSL_INSTR),

};

//
// A delay slot must not contain
// an IMM, branch or break instruction
//
static Bool illegalDelaySlotInstruction(microblazeP microblaze, microblazeMorphStateP state) {
    Bool status = False;

    // morphNOP_TYPEX

    //
    // If the instruction is 8 bits then this is an imm
    //
    if (state->info.instrsize == 8) {
        status = True;
        vmiMessage("W", "MB_DS_ILL", "Illegal Instruction 0x%08x: 'IMM' In Delay Slot", state->info.thisPC-4);
    } else {
        //
        // Illegal delay slot instructions branch & break
        //
        switch (state->info.type) {
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
            case MICROBLAZE_IT_BR_TYPEA3:
            case MICROBLAZE_IT_BRA_TYPEA3:
            case MICROBLAZE_IT_BRAD_TYPEA3:
            case MICROBLAZE_IT_BRD_TYPEA3:
            case MICROBLAZE_IT_BRALD_TYPEA4:
            case MICROBLAZE_IT_BRLD_TYPEA4:
            case MICROBLAZE_IT_BRK_TYPEA5:
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
            case MICROBLAZE_IT_BRAI_TYPEB3:
            case MICROBLAZE_IT_BRAID_TYPEB3:
            case MICROBLAZE_IT_BRI_TYPEB3:
            case MICROBLAZE_IT_BRID_TYPEB3:
            case MICROBLAZE_IT_BRALID_TYPEB4:
            case MICROBLAZE_IT_BRLID_TYPEB4:
            case MICROBLAZE_IT_BRKI_TYPEB5:
            case MICROBLAZE_IT_RTBD_TYPEBA:
            case MICROBLAZE_IT_RTED_TYPEBA:
            case MICROBLAZE_IT_RTID_TYPEBA:
            case MICROBLAZE_IT_RTSD_TYPEBA:
                status = True;
                vmiMessage("W", "MB_DS_ILL", "Illegal Instruction 0x%08x: '%s' In Delay Slot", state->info.thisPC, state->info.opcode);
                break;

            default:
                status = False;
                break;
        }
    }

    if (status) {
        //
        // Morph a NOP instruction
        //
    }
    return status;
}

static Bool absentFeature(microblazeP microblaze, microblazeMorphStateP state) {

    microblazeFeature fr = featureRequired[state->info.type];
    Bool        isAbsent = False;

    // determine whether required feature is absent
    if(fr) {
        switch(fr) {
            case FR_NONE:
                break;
            case FR_C_USE_BARREL:
                isAbsent = !microblaze->SPR_PVR0.bits.BS;
                break;
            case FR_C_USE_DIV:
                isAbsent = !microblaze->SPR_PVR0.bits.DIV;
                break;
            case FR_C_USE_HW_MUL:
                isAbsent = !microblaze->SPR_PVR0.bits.MUL;
                break;
            case FR_C_USE_HW_MUL64:
                isAbsent = !microblaze->SPR_PVR2.bits.MUL64;
                break;
            case FR_C_USE_FPU:
                isAbsent = !microblaze->SPR_PVR0.bits.FPU;
                break;
            case FR_C_USE_FPU2:
                isAbsent = !microblaze->SPR_PVR2.bits.FPU;
                break;
            case FR_C_USE_MSR_INSTR:
                isAbsent = !microblaze->SPR_PVR2.bits.MSR;
                break;
            case FR_C_USE_PCMP_INSTR:
                isAbsent = !microblaze->SPR_PVR2.bits.PCMP;
                break;
            case FR_C_USE_FSL_LINKS:
                isAbsent = !microblaze->SPR_PVR3.bits.FSL;
                break;
            case FR_C_USE_EXTENDED_FSL_INSTR:
                isAbsent = !microblaze->SPR_PVR2.bits.FSL;
                break;
            default:
                VMI_ABORT("unhandled feature %u\n", fr);
                break;
        }
    }

    return isAbsent;
}

//
// Instruction Morpher
//
VMI_MORPH_FN(microblazeMorph) {
    microblazeP microblaze = (microblazeP)processor;
    microblazeMorphState state;

    // get instruction and instruction type
    microblazeDecode(microblaze, thisPC, &state.info);

    state.attrs = &dispatchTable[state.info.type];
    state.microblaze = microblaze;

    if(disableMorph(&state)) {
        // no action if in disassembly mode

    } else if(state.info.type==MICROBLAZE_IT_LAST) {
        // take UndefinedInstruction exception
        emitUndefined(&state);

    } else if(!supportedOnVariant(microblaze, &state)) {
        // instruction not supported on this variant
        emitNotVariant(&state);

    } else if(absentFeature(microblaze, &state)) {
        // instruction not supported on this configuration
        emitAbsentFeature(&state);

    } else if(inDelaySlot && illegalDelaySlotInstruction(microblaze, &state)) {
        // no action - effectively an NOP

    } else if(state.attrs->morphCB) {
        // translate the instruction
        emitImplemented(&state);

    } else {
        // here if no morph callback specified
        emitUnimplemented(&state);

    }
}

VMI_FETCH_SNAP_FN(microblazeFetchSnap) {
    return thisPC & -4;
}
