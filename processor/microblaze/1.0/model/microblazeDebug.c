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

#include "vmi/vmiAttrs.h"
#include "vmi/vmiDbg.h"
#include "vmi/vmiMessage.h"
#include "vmi/vmiOSLib.h"
#include "vmi/vmiRt.h"
#include "microblazeStructure.h"
#include "microblazeFunctions.h"
#include "microblazeUser.h"

//
// Macro to specify basic register attributes
//
#define MICROBLAZE_REG_INFO(_NAME, _INDEX, _USAGE, _BITS, _GROUP, _DETAIL) { \
    name     : _NAME,    \
    gdbIndex : _INDEX,   \
    usage    : _USAGE,   \
    bits     : _BITS,    \
    group    : _GROUP,   \
    _DETAIL              \
}

//
// Macro to specify a register accessible raw for read/write
//
#define MICROBLAZE_RAW_REG_RW(_R) \
    access   : vmi_RA_RW,               \
    raw      : MICROBLAZE_CPU_REG(_R)

//
// Macro to specify a register accessible raw for read only
//
#define MICROBLAZE_RAW_REG_R(_R) \
    access   : vmi_RA_R,                \
    raw      : MICROBLAZE_CPU_REG(_R)

//
// Macro to specify a register accessible with callbacks for read/write
//
#define MICROBLAZE_CB_REG_RW(_RCB, _WCB) \
    access   : vmi_RA_RW,           \
    readCB   : _RCB,                \
    writeCB  : _WCB


static VMI_REG_READ_FN(readPC) {
    *(microblazeAddr*)buffer = (Uns32)vmirtGetPC(processor);
    return True;
}

static VMI_REG_WRITE_FN(writePC) {
    vmirtSetPC(processor, *(microblazeAddr*)buffer);
    return True;
}

static VMI_REG_READ_FN(readMSR) {
    microblazeP microblaze = (microblazeP) processor;
    microblazeFillMSR(microblaze);
    *(Uns32*)buffer = microblaze->SPR_MSR.reg;
    return True;
}

static VMI_REG_WRITE_FN(writeMSR) {
    microblazeP microblaze = (microblazeP) processor;
    microblaze->SPR_MSR.reg = *(Uns32*)buffer;
    microblazeEmptyMSR(microblaze);
    microblazeUpdateBlockMask(microblaze);
    return True;
}

static VMI_REG_READ_FN(readFSR) {
    microblazeP microblaze = (microblazeP) processor;
    microblazeFillFSR(microblaze);
    *(Uns32*)buffer = microblaze->SPR_FSR.reg;
    return True;
}

static VMI_REG_WRITE_FN(writeFSR) {
    microblazeP microblaze = (microblazeP) processor;
    microblaze->SPR_FSR.reg = *(Uns32*)buffer;
    microblazeEmptyFSR(microblaze);
    return True;
}


typedef enum microblazeRegGroupIdE {
    MICROBLAZE_RG_USER,
    MICROBLAZE_RG_SYSTEM,
    MICROBLAZE_RG_LAST
} microblazeRegGroupId;

static const vmiRegGroup groups[MICROBLAZE_RG_LAST+1] = {
    [MICROBLAZE_RG_USER]   = {name: "User"          },
    [MICROBLAZE_RG_SYSTEM] = {name: "System"        },
};

//
// Return next supported group on this processor
//
static vmiRegGroupCP getNextGroup(vmiRegGroupCP group) {

    if(!group) {
        group = groups;
    } else if((group+1)->name) {
        group = group+1;
    } else {
        group = 0;
    }

    return group;
}

static const vmiRegInfo registers[] = {
    MICROBLAZE_REG_INFO("R0",         0, vmi_REG_NONE, 32, &groups[MICROBLAZE_RG_USER],   MICROBLAZE_RAW_REG_R(gpr[0])),
    MICROBLAZE_REG_INFO("R1",         1, vmi_REG_NONE, 32, &groups[MICROBLAZE_RG_USER],   MICROBLAZE_RAW_REG_RW(gpr[1])),
    MICROBLAZE_REG_INFO("R2",         2, vmi_REG_NONE, 32, &groups[MICROBLAZE_RG_USER],   MICROBLAZE_RAW_REG_RW(gpr[2])),
    MICROBLAZE_REG_INFO("R3",         3, vmi_REG_NONE, 32, &groups[MICROBLAZE_RG_USER],   MICROBLAZE_RAW_REG_RW(gpr[3])),
    MICROBLAZE_REG_INFO("R4",         4, vmi_REG_NONE, 32, &groups[MICROBLAZE_RG_USER],   MICROBLAZE_RAW_REG_RW(gpr[4])),
    MICROBLAZE_REG_INFO("R5",         5, vmi_REG_NONE, 32, &groups[MICROBLAZE_RG_USER],   MICROBLAZE_RAW_REG_RW(gpr[5])),
    MICROBLAZE_REG_INFO("R6",         6, vmi_REG_NONE, 32, &groups[MICROBLAZE_RG_USER],   MICROBLAZE_RAW_REG_RW(gpr[6])),
    MICROBLAZE_REG_INFO("R7",         7, vmi_REG_NONE, 32, &groups[MICROBLAZE_RG_USER],   MICROBLAZE_RAW_REG_RW(gpr[7])),
    MICROBLAZE_REG_INFO("R8",         8, vmi_REG_NONE, 32, &groups[MICROBLAZE_RG_USER],   MICROBLAZE_RAW_REG_RW(gpr[8])),
    MICROBLAZE_REG_INFO("R9",         9, vmi_REG_NONE, 32, &groups[MICROBLAZE_RG_USER],   MICROBLAZE_RAW_REG_RW(gpr[9])),
    MICROBLAZE_REG_INFO("R10",       10, vmi_REG_NONE, 32, &groups[MICROBLAZE_RG_USER],   MICROBLAZE_RAW_REG_RW(gpr[10])),
    MICROBLAZE_REG_INFO("R11",       11, vmi_REG_NONE, 32, &groups[MICROBLAZE_RG_USER],   MICROBLAZE_RAW_REG_RW(gpr[11])),
    MICROBLAZE_REG_INFO("R12",       12, vmi_REG_NONE, 32, &groups[MICROBLAZE_RG_USER],   MICROBLAZE_RAW_REG_RW(gpr[12])),
    MICROBLAZE_REG_INFO("R13",       13, vmi_REG_NONE, 32, &groups[MICROBLAZE_RG_USER],   MICROBLAZE_RAW_REG_RW(gpr[13])),
    MICROBLAZE_REG_INFO("R14",       14, vmi_REG_NONE, 32, &groups[MICROBLAZE_RG_USER],   MICROBLAZE_RAW_REG_RW(gpr[14])),
    MICROBLAZE_REG_INFO("R15",       15, vmi_REG_NONE, 32, &groups[MICROBLAZE_RG_USER],   MICROBLAZE_RAW_REG_RW(gpr[15])),
    MICROBLAZE_REG_INFO("R16",       16, vmi_REG_NONE, 32, &groups[MICROBLAZE_RG_USER],   MICROBLAZE_RAW_REG_RW(gpr[16])),
    MICROBLAZE_REG_INFO("R17",       17, vmi_REG_NONE, 32, &groups[MICROBLAZE_RG_USER],   MICROBLAZE_RAW_REG_RW(gpr[17])),
    MICROBLAZE_REG_INFO("R18",       18, vmi_REG_NONE, 32, &groups[MICROBLAZE_RG_USER],   MICROBLAZE_RAW_REG_RW(gpr[18])),
    MICROBLAZE_REG_INFO("R19",       19, vmi_REG_NONE, 32, &groups[MICROBLAZE_RG_USER],   MICROBLAZE_RAW_REG_RW(gpr[19])),
    MICROBLAZE_REG_INFO("R20",       20, vmi_REG_NONE, 32, &groups[MICROBLAZE_RG_USER],   MICROBLAZE_RAW_REG_RW(gpr[20])),
    MICROBLAZE_REG_INFO("R21",       21, vmi_REG_NONE, 32, &groups[MICROBLAZE_RG_USER],   MICROBLAZE_RAW_REG_RW(gpr[21])),
    MICROBLAZE_REG_INFO("R22",       22, vmi_REG_NONE, 32, &groups[MICROBLAZE_RG_USER],   MICROBLAZE_RAW_REG_RW(gpr[22])),
    MICROBLAZE_REG_INFO("R23",       23, vmi_REG_NONE, 32, &groups[MICROBLAZE_RG_USER],   MICROBLAZE_RAW_REG_RW(gpr[23])),
    MICROBLAZE_REG_INFO("R24",       24, vmi_REG_NONE, 32, &groups[MICROBLAZE_RG_USER],   MICROBLAZE_RAW_REG_RW(gpr[24])),
    MICROBLAZE_REG_INFO("R25",       25, vmi_REG_NONE, 32, &groups[MICROBLAZE_RG_USER],   MICROBLAZE_RAW_REG_RW(gpr[25])),
    MICROBLAZE_REG_INFO("R26",       26, vmi_REG_NONE, 32, &groups[MICROBLAZE_RG_USER],   MICROBLAZE_RAW_REG_RW(gpr[26])),
    MICROBLAZE_REG_INFO("R27",       27, vmi_REG_NONE, 32, &groups[MICROBLAZE_RG_USER],   MICROBLAZE_RAW_REG_RW(gpr[27])),
    MICROBLAZE_REG_INFO("R28",       28, vmi_REG_NONE, 32, &groups[MICROBLAZE_RG_USER],   MICROBLAZE_RAW_REG_RW(gpr[28])),
    MICROBLAZE_REG_INFO("R29",       29, vmi_REG_NONE, 32, &groups[MICROBLAZE_RG_USER],   MICROBLAZE_RAW_REG_RW(gpr[29])),
    MICROBLAZE_REG_INFO("R30",       30, vmi_REG_NONE, 32, &groups[MICROBLAZE_RG_USER],   MICROBLAZE_RAW_REG_RW(gpr[30])),
    MICROBLAZE_REG_INFO("R31",       31, vmi_REG_NONE, 32, &groups[MICROBLAZE_RG_USER],   MICROBLAZE_RAW_REG_RW(gpr[31])),
    MICROBLAZE_REG_INFO("SPR_PC",    32, vmi_REG_PC,   32, &groups[MICROBLAZE_RG_SYSTEM], MICROBLAZE_CB_REG_RW(readPC, writePC)),
    MICROBLAZE_REG_INFO("SPR_MSR",   33, vmi_REG_NONE, 32, &groups[MICROBLAZE_RG_SYSTEM], MICROBLAZE_CB_REG_RW(readMSR, writeMSR)),
    MICROBLAZE_REG_INFO("SPR_EAR",   34, vmi_REG_NONE, 32, &groups[MICROBLAZE_RG_SYSTEM], MICROBLAZE_RAW_REG_RW(SPR_EAR)),
    MICROBLAZE_REG_INFO("SPR_ESR",   35, vmi_REG_NONE, 32, &groups[MICROBLAZE_RG_SYSTEM], MICROBLAZE_RAW_REG_RW(SPR_ESR)),
    MICROBLAZE_REG_INFO("SPR_FSR",   36, vmi_REG_NONE, 32, &groups[MICROBLAZE_RG_SYSTEM], MICROBLAZE_CB_REG_RW(readFSR, writeFSR)),
    MICROBLAZE_REG_INFO("SPR_BTR",   37, vmi_REG_NONE, 32, &groups[MICROBLAZE_RG_SYSTEM], MICROBLAZE_RAW_REG_RW(SPR_BTR)),
    MICROBLAZE_REG_INFO("SPR_PVR0",  38, vmi_REG_NONE, 32, &groups[MICROBLAZE_RG_SYSTEM], MICROBLAZE_RAW_REG_RW(SPR_PVR0)),
    MICROBLAZE_REG_INFO("SPR_PVR1",  39, vmi_REG_NONE, 32, &groups[MICROBLAZE_RG_SYSTEM], MICROBLAZE_RAW_REG_RW(SPR_PVR1)),
    MICROBLAZE_REG_INFO("SPR_PVR2",  40, vmi_REG_NONE, 32, &groups[MICROBLAZE_RG_SYSTEM], MICROBLAZE_RAW_REG_RW(SPR_PVR2)),
    MICROBLAZE_REG_INFO("SPR_PVR3",  41, vmi_REG_NONE, 32, &groups[MICROBLAZE_RG_SYSTEM], MICROBLAZE_RAW_REG_RW(SPR_PVR3)),
    MICROBLAZE_REG_INFO("SPR_PVR4",  42, vmi_REG_NONE, 32, &groups[MICROBLAZE_RG_SYSTEM], MICROBLAZE_RAW_REG_RW(SPR_PVR4)),
    MICROBLAZE_REG_INFO("SPR_PVR5",  43, vmi_REG_NONE, 32, &groups[MICROBLAZE_RG_SYSTEM], MICROBLAZE_RAW_REG_RW(SPR_PVR5)),
    MICROBLAZE_REG_INFO("SPR_PVR6",  44, vmi_REG_NONE, 32, &groups[MICROBLAZE_RG_SYSTEM], MICROBLAZE_RAW_REG_RW(SPR_PVR6)),
    MICROBLAZE_REG_INFO("SPR_PVR7",  45, vmi_REG_NONE, 32, &groups[MICROBLAZE_RG_SYSTEM], MICROBLAZE_RAW_REG_RW(SPR_PVR7)),
    MICROBLAZE_REG_INFO("SPR_PVR8",  46, vmi_REG_NONE, 32, &groups[MICROBLAZE_RG_SYSTEM], MICROBLAZE_RAW_REG_RW(SPR_PVR8)),
    MICROBLAZE_REG_INFO("SPR_PVR9",  47, vmi_REG_NONE, 32, &groups[MICROBLAZE_RG_SYSTEM], MICROBLAZE_RAW_REG_RW(SPR_PVR9)),
    MICROBLAZE_REG_INFO("SPR_PVR10", 48, vmi_REG_NONE, 32, &groups[MICROBLAZE_RG_SYSTEM], MICROBLAZE_RAW_REG_RW(SPR_PVR10)),
    MICROBLAZE_REG_INFO("SPR_PVR11", 49, vmi_REG_NONE, 32, &groups[MICROBLAZE_RG_SYSTEM], MICROBLAZE_RAW_REG_RW(SPR_PVR11)),
    MICROBLAZE_REG_INFO("SPR_EDR",   50, vmi_REG_NONE, 32, &groups[MICROBLAZE_RG_SYSTEM], MICROBLAZE_RAW_REG_RW(SPR_EDR)),
    MICROBLAZE_REG_INFO("SPR_PID",   51, vmi_REG_NONE, 32, &groups[MICROBLAZE_RG_SYSTEM], MICROBLAZE_RAW_REG_RW(SPR_PID)),
    MICROBLAZE_REG_INFO("SPR_ZPR",   52, vmi_REG_NONE, 32, &groups[MICROBLAZE_RG_SYSTEM], MICROBLAZE_RAW_REG_RW(SPR_ZPR)),
    MICROBLAZE_REG_INFO("SPR_TLBX",  53, vmi_REG_NONE, 32, &groups[MICROBLAZE_RG_SYSTEM], MICROBLAZE_RAW_REG_RW(SPR_TLBX)),
    MICROBLAZE_REG_INFO("SPR_TLBSX", 54, vmi_REG_NONE, 32, &groups[MICROBLAZE_RG_SYSTEM], MICROBLAZE_RAW_REG_RW(SPR_TLBSX)),
    //MICROBLAZE_REG_INFO("SPR_SLR",   55, vmi_REG_NONE, 32, &groups[MICROBLAZE_RG_SYSTEM], MICROBLAZE_RAW_REG_RW(SPR_SLR)),
    //MICROBLAZE_REG_INFO("SPR_SHR",   56, vmi_REG_NONE, 32, &groups[MICROBLAZE_RG_SYSTEM], MICROBLAZE_RAW_REG_RW(SPR_SHR)),
    MICROBLAZE_REG_INFO("SPR_TLBLO", 55, vmi_REG_NONE, 32, &groups[MICROBLAZE_RG_SYSTEM], MICROBLAZE_RAW_REG_RW(SPR_TLBLO)),
    MICROBLAZE_REG_INFO("SPR_TLBHI", 56, vmi_REG_NONE, 32, &groups[MICROBLAZE_RG_SYSTEM], MICROBLAZE_RAW_REG_RW(SPR_TLBHI)),
    //
    // Not Real registers
    //
//    MICROBLAZE_REG_INFO("tmp0", 59, vmi_REG_NONE, 32, &groups[MICROBLAZE_RG_SYSTEM], MICROBLAZE_RAW_REG_RW(tmp[0])),
//    MICROBLAZE_REG_INFO("tmp1", 60, vmi_REG_NONE, 32, &groups[MICROBLAZE_RG_SYSTEM], MICROBLAZE_RAW_REG_RW(tmp[1])),
//    MICROBLAZE_REG_INFO("tmp2", 61, vmi_REG_NONE, 32, &groups[MICROBLAZE_RG_SYSTEM], MICROBLAZE_RAW_REG_RW(tmp[2])),
//    MICROBLAZE_REG_INFO("tmp3", 62, vmi_REG_NONE, 32, &groups[MICROBLAZE_RG_SYSTEM], MICROBLAZE_RAW_REG_RW(tmp[3])),
    {0}
};

VMI_REG_GROUP_FN(microblazeRegGroup) {
    return getNextGroup(prev);
}

VMI_REG_INFO_FN(microblazeRegInfo) {
    if(!prev) {
        return registers;
    } else if((prev+1)->name) {
        return prev+1;
    } else {
        return 0;
    }
}

VMI_DEBUG_FN(microblazeDebug) {
    microblazeP microblaze = (microblazeP)processor;
    Int32 iter;

    // TODO: should use vmi interface so this isn't necessary?
    microblazeFillRegisters(microblaze);

    for (iter=0; iter<32; iter++) {
        vmiPrintf("%8s%02d : %08x", "R", iter, microblaze->gpr[iter]);
        if(!((iter+1)&3)) {
            vmiPrintf("\n");
        }
    }
//    for (iter=0; iter<4; iter++) {
//        vmiPrintf("%8s%02d : %08x", "tmp", iter, microblaze->tmp[iter]);
//        if(!((iter+1)&3)) {
//            vmiPrintf("\n");
//        }
//    }
    vmiPrintf("%10s : %08x", "SPR_PC",  microblaze->SPR_PC);
    vmiPrintf("%10s : %08x", "SPR_MSR", microblaze->SPR_MSR.reg);
    vmiPrintf("%10s : %08x", "SPR_EAR", microblaze->SPR_EAR.reg);
    vmiPrintf("%10s : %08x", "SPR_ESR", microblaze->SPR_ESR.reg);
    vmiPrintf("\n");
    vmiPrintf("%10s : %08x", "SPR_BTR", microblaze->SPR_BTR.reg);
    vmiPrintf("%10s : %08x", "SPR_FSR", microblaze->SPR_FSR.reg);
    vmiPrintf("%10s : %08x", "SPR_EDR", microblaze->SPR_EDR.reg);
    vmiPrintf("%10s : %08x", "SPR_SLR", microblaze->SPR_SLR.reg);
    vmiPrintf("\n");
    vmiPrintf("%10s : %08x", "SPR_SHR", microblaze->SPR_SHR.reg);
    vmiPrintf("%10s : %08x", "SPR_PID", microblaze->SPR_PID.reg);
    vmiPrintf("%10s : %08x", "SPR_ZPR", microblaze->SPR_ZPR.reg);
    vmiPrintf("%10s : %08x", "SPR_TLBLO", microblaze->SPR_TLBLO.reg);
    vmiPrintf("\n");
    vmiPrintf("%10s : %08x", "SPR_TLBHI", microblaze->SPR_TLBHI.reg);
    vmiPrintf("%10s : %08x", "SPR_TLBX", microblaze->SPR_TLBX.reg);
    vmiPrintf("%10s : %08x", "SPR_TLBSX", microblaze->SPR_TLBSX.reg);
    vmiPrintf("%10s : %08x", "SPR_PVR0", microblaze->SPR_PVR0.reg);
    vmiPrintf("\n");
    vmiPrintf("%10s : %08x", "SPR_PVR1", microblaze->SPR_PVR1.reg);
    vmiPrintf("%10s : %08x", "SPR_PVR2", microblaze->SPR_PVR2.reg);
    vmiPrintf("%10s : %08x", "SPR_PVR3", microblaze->SPR_PVR3.reg);
    vmiPrintf("%10s : %08x", "SPR_PVR4", microblaze->SPR_PVR4.reg);
    vmiPrintf("\n");
    vmiPrintf("%10s : %08x", "SPR_PVR5", microblaze->SPR_PVR5.reg);
    vmiPrintf("%10s : %08x", "SPR_PVR6", microblaze->SPR_PVR6.reg);
    vmiPrintf("%10s : %08x", "SPR_PVR7", microblaze->SPR_PVR7.reg);
    vmiPrintf("%10s : %08x", "SPR_PVR8", microblaze->SPR_PVR8.reg);
    vmiPrintf("\n");
    vmiPrintf("%10s : %08x", "SPR_PVR9", microblaze->SPR_PVR9.reg);
    vmiPrintf("%10s : %08x", "SPR_PVR10", microblaze->SPR_PVR10.reg);
    vmiPrintf("%10s : %08x", "SPR_PVR11", microblaze->SPR_PVR11.reg);
    vmiPrintf("\n");

    //
    // Not real Registers
    //

//    vmiPrintf("%10s :        %01x", "FLG_CC", microblaze->FLG_CC);
//    vmiPrintf("\n");
//    vmiPrintf("%10s :        %01x", "FLG_PA", microblaze->FLG_PA);
//    vmiPrintf("%10s :        %01x", "FLG_ZR", microblaze->FLG_ZR);
//    vmiPrintf("%10s :        %01x", "FLG_SI", microblaze->FLG_SI);
//    vmiPrintf("%10s :        %01x", "FLG_OV", microblaze->FLG_OV);
//    vmiPrintf("\n");
//    vmiPrintf("%10s : %08x\n", "PC", (microblazeAddr)vmirtGetPC(processor));

    vmiPrintf("\n");
}

