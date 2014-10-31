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

#include "vmi/vmiAttrs.h"
#include "vmi/vmiParameters.h"
#include "vmi/vmiMessage.h"

#include "microblazeFunctions.h"
#include "microblazeParameters.h"
#include "microblazeStructure.h"
#include "microblazeVariant.h"

static vmiEnumParameter C_RESET_MSRValues[] = {
    {"0x00", 0},
    {"0x20", 1},
    {"0x80", 2},
    {"0xa0", 3},
    {0},
};
static vmiEnumParameter C_ICACHE_LINE_LENValues[] = {
    {"4", 0},
    {"8", 1},
    {0},
};
static vmiEnumParameter C_ICACHE_INTERFACEValues[] = {
    {"IXCL", 0},
    {"IXCL2", 1},
    {0},
};
static vmiEnumParameter C_ICACHE_VICTIMSValues[] = {
    {"0", 0},
    {"2", 1},
    {"4", 2},
    {"8", 3},
    {0},
};
static vmiEnumParameter C_CACHE_BYTE_SIZEValues[] = {
    {"64", 0},
    {"128", 1},
    {"256", 2},
    {"512", 3},
    {"1024", 4},
    {"2048", 5},
    {"4096", 6},
    {"8192", 7},
    {"16384", 8},
    {"32768", 9},
    {"65536", 10},
    {0},
};
static vmiEnumParameter C_DCACHE_LINE_LENValues[] = {
    {"4", 0},
    {"8", 1},
    {0},
};
static vmiEnumParameter C_DCACHE_INTERFACEValues[] = {
    {"DXCL", 0},
    {"DXCL2", 1},
    {0},
};
static vmiEnumParameter C_DCACHE_VICTIMSValues[] = {
    {"0", 0},
    {"2", 1},
    {"4", 2},
    {"8", 3},
    {0},
};
static vmiEnumParameter C_DCACHE_BYTE_SIZEValues[] = {
    {"64", 0},
    {"128", 1},
    {"256", 2},
    {"512", 3},
    {"1024", 4},
    {"2048", 5},
    {"4096", 6},
    {"8192", 7},
    {"16384", 8},
    {"32768", 9},
    {"65536", 10},
    {0},
};

//
// Table of parameter specs
//
static vmiParameter formals[] = {
    VMI_ENUM_PARAM_SPEC(microblazeParamValues, variant, 0, "Selects variant (either a generic ISA or a specific model)"),
    VMI_BOOL_PARAM_SPEC(microblazeParamValues, verbose, 1, "Specify verbose output messages"),
    VMI_ENDIAN_PARAM_SPEC(microblazeParamValues, endian,   "Model endian"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_FAMILY, 0, 0, 255, "Target Family"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_AREA_OPTIMIZED, 0, 0, 1, "Select implementation to optimize area with lower instruction throughput"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_INTERCONNECT, 1, 1, 2, "Select interconnect 1 = PLBv46, 2 = AXI4"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_ENDIANNESS, 0, 0, 1, "Select endianness 0 = Big endian, 1 = Little endian"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_FAULT_TOLERANT, 0, 0, 1, "Implement fault tolerance"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_ECC_USE_CE_EXCEPTION, 0, 0, 1, "Generate Bus Error Exceptions for correctable errors"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_PVR, 0, 0, 2, "Processor version register mode selection"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_PVR_USER1, 0x0, 0x0, 0xff, "Processor version register USER1 constant"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_PVR_USER2, 0x0, 0x0, 0xffffffff, "Processor version register USER2 constant"),
    VMI_ENUM_PARAM_SPEC(microblazeParamValues, C_RESET_MSR, C_RESET_MSRValues, "Reset value for MSR register"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_D_PLB, 0, 0, 1, "Data side PLB interface"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_D_AXI, 0, 0, 1, "Data side AXI interface"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_D_LMB, 1, 0, 1, "Data side LMB interface"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_I_PLB, 0, 0, 1, "Instruction side PLB interface"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_I_AXI, 0, 0, 1, "Instruction side AXI interface"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_I_LMB, 1, 0, 1, "Instruction side LMB interface"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_USE_BARREL, 0, 0, 1, "Include barrel shifter"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_USE_DIV, 0, 0, 1, "Include hardware divider"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_USE_HW_MUL, 1, 0, 2, "Include hardware integer multiplier"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_USE_FPU, 0, 0, 2, "Include hardware floating integer point unit"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_USE_MSR_INSTR, 1, 0, 1, "Enable use of instructions: integer MSRSET and MSRCLR"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_USE_PCMP_INSTR, 1, 0, 1, "Enable use of instructions: integer CLZ, PCMPBF, PCMPEQ, and PCMPNE"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_UNALIGNED_EXCEPTIONS, 0, 0, 1, "Enable exception handling for unaligned data accesses"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_ILL_OPCODE_EXCEPTION, 0, 0, 1, "Enable exception handling for illegal op-code"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_IPLB_BUS_EXCEPTION, 0, 0, 1, "Enable exception handling for IPLB bus error"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_DPLB_BUS_EXCEPTION, 0, 0, 1, "Enable exception handling for DPLB bus error"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_M_AXI_I_BUS_EXCEPTION, 0, 0, 1, "Enable exception handling for M_AXI_I bus error"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_M_AXI_D_BUS_EXCEPTION, 0, 0, 1, "Enable exception handling for M_AXI_D bus error"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_DIV_ZERO_EXCEPTION, 0, 0, 1, "Enable exception handling for division by zero or division overflow"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_FPU_EXCEPTION, 0, 0, 1, "Enable exception handling for hardware floating point unit exceptions"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_OPCODE_0x0_ILLEGAL, 0, 0, 1, "Detect opcode 0x0 as an illegal instruction"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_FSL_EXCEPTION, 0, 0, 1, "Enable exception handling for Stream Links"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_USE_STACK_PROTECTION, 0, 0, 1, "Generate exception for stack overflow or stack underflow"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_DEBUG_ENABLED, 0, 0, 1, "MDM Debug interface"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_NUMBER_OF_PC_BRK, 1, 0, 8, "Number of hardware breakpoints"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_NUMBER_OF_RD_ADDR_BRK, 0, 0, 4, "Number of read address watchpoints"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_NUMBER_OF_WR_ADDR_BRK, 0, 0, 4, "Number of write address watchpoints"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_INTERRUPT_IS_EDGE, 0, 0, 1, "Level/Edge Interrupt"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_EDGE_IS_POSITIVE, 1, 0, 1, "Negative/Positive Edge integer Interrupt"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_FSL_LINKS, 0, 0, 16, "Number of stream interfaces (FSL or AXI)"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_USE_EXTENDED_FSL_INSTR, 0, 0, 1, "Enable use of extended integer stream instructions"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_ICACHE_BASEADDR, 0x00000000, 0x00000000, 0xFFFFFFFF, "Instruction cache base address"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_ICACHE_HIGHADDR, 0x3fffffff, 0x00000000, 0xFFFFFFFF, "Instruction cache high address"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_USE_ICACHE, 0, 0, 1, "Instruction cache"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_ALLOW_ICACHE_WR, 1, 0, 1, "Instruction cache write enable"),
    VMI_ENUM_PARAM_SPEC(microblazeParamValues, C_ICACHE_LINE_LEN, C_ICACHE_LINE_LENValues, "Instruction cache line length"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_ICACHE_ALWAYS_USED, 0, 0, 1, "Instruction cache CacheLink used for all memory accesses"),
    VMI_ENUM_PARAM_SPEC(microblazeParamValues, C_ICACHE_INTERFACE, C_ICACHE_INTERFACEValues, "Instruction cache CacheLink interface protocol"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_ICACHE_FORCE_TAG_LUTRAM, 0, 0, 1, "Instruction cache tag always implemented with distributed RAM"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_ICACHE_STREAMS, 0, 0, 1, "Instruction cache streams"),
    VMI_ENUM_PARAM_SPEC(microblazeParamValues, C_ICACHE_VICTIMS, C_ICACHE_VICTIMSValues, "Instruction cache victims"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_ICACHE_DATA_WIDTH, 0, 0, 2, "Instruction cache data width, 0 = 32 bits, 1 = Full cache line, 2 = 512 bits"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_ADDR_TAG_BITS, 17, 0, 25, "Instruction cache address tags"),
    VMI_ENUM_PARAM_SPEC(microblazeParamValues, C_CACHE_BYTE_SIZE, C_CACHE_BYTE_SIZEValues, "Instruction cache size"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_ICACHE_USE_FSL, 1, 1, 1, "Cache over CacheLink instead of peripheral bus for instructions"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_DCACHE_BASEADDR, 0x00000000, 0x00000000, 0xFFFFFFFF, "Data cache base address"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_DCACHE_HIGHADDR, 0x3fffffff, 0x00000000, 0xFFFFFFFF, "Data cache high address"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_USE_DCACHE, 0, 0, 1, "Data cache"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_ALLOW_DCACHE_WR, 1, 0, 1, "Data cache write enable"),
    VMI_ENUM_PARAM_SPEC(microblazeParamValues, C_DCACHE_LINE_LEN, C_DCACHE_LINE_LENValues, "Data cache line length"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_DCACHE_ALWAYS_USED, 0, 0, 1, "Data cache CacheLink used for all memory accesses"),
    VMI_ENUM_PARAM_SPEC(microblazeParamValues, C_DCACHE_INTERFACE, C_DCACHE_INTERFACEValues, "Data cache CacheLink interface protocol"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_DCACHE_FORCE_TAG_LUTRAM, 0, 0, 1, "Data cache tag always implemented with distributed RAM"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_DCACHE_USE_WRITEBACK, 0, 0, 1, "Data cache write-back storage policy used"),
    VMI_ENUM_PARAM_SPEC(microblazeParamValues, C_DCACHE_VICTIMS, C_DCACHE_VICTIMSValues, "Data cache victims"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_DCACHE_DATA_WIDTH, 0, 0, 2, "Data cache data width, 0 = 32 bits, 1 = Full cache line, 2 = 512 bits"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_DCACHE_ADDR_TAG, 17, 0, 25, "Data cache address tags"),
    VMI_ENUM_PARAM_SPEC(microblazeParamValues, C_DCACHE_BYTE_SIZE, C_DCACHE_BYTE_SIZEValues, "Data cache size"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_DCACHE_USE_FSL, 1, 1, 1, "Cache over CacheLink instead of peripheral bus for data"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_USE_MMU, 0, 0, 3, "0 = None, 1 = Usermode, 2 = Protection, 3 = Virtual"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_MMU_DTLB_SIZE, 4, 1, 8, "Data shadow Translation Look-Aside Buffer size 1,2,4,8"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_MMU_ITLB_SIZE, 2, 1, 8, "Instruction shadow Translation Look-Aside Buffer size 1,2,4,8"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_MMU_TLB_ACCESS, 3, 0, 3, "Access to memory management special registers: 0 = Minimal, 1 = Read, 2 = Write, 3 = Full"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_MMU_ZONES, 16, 0, 16, "Number of memory protection zones"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_MMU_PRIVILEGED_INSTR, 0, 0, 1, "Privileged instructions 0 = Full protection, 1 = Allow stream instrs"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_USE_INTERRUPT, 0, 0, 1, "Enable interrupt handling"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_USE_EXT_BRK, 0, 0, 1, "Enable external break handling"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_USE_EXT_NM_BRK, 0, 0, 1, "Enable external non-maskable break handling"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_USE_BRANCH_TARGET_CACHE, 0, 0, 1, "Enable Branch Target Cache"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_BRANCH_TARGET_CACHE_SIZE, 0, 0, 7, "Branch Target Cache size: 0 = Default, 1 = 8 entries, 2 = 16 entries, 3 = 32 entries, 4 = 64 entries, 5 = 512 entries, 6 = 1024 entries, 7 = 2048 entries"),
    VMI_UNS32_PARAM_SPEC(microblazeParamValues, C_STREAM_INTERCONNECT, 0, 0, 1, "Select AXI4-Stream integer interconnect"),
    VMI_END_PARAM
};

static Uns32 countVariants(void) {
    microblazeConfigCP cfg = microblazeConfigTable;
    Uns32 i = 0;
    while(cfg->name) {
        cfg++;
        i++;
    }
    return i;
}

//
// First time through, malloc and fill the variant list from the config table
//
static vmiEnumParameterP getVariantList() {
    static vmiEnumParameterP list = NULL;
    if (!list) {
        Uns32 v = 1 + countVariants();
        list = STYPE_CALLOC_N(vmiEnumParameter, v);
        vmiEnumParameterP prm;
        microblazeConfigCP cfg;
        Uns32 i;
        for (i = 0, cfg = microblazeConfigTable, prm = list;
             cfg->name;
             i++, cfg++, prm++) {
            prm->name = cfg->name;
            prm->value = i;
        }
    }
    return list;
}

//
// First time through, fill the formals table
//
static vmiParameterP getFormals(void) {
    static Bool first = True;
    if(first) {
        first = False;
        formals[0].u.enumParam.legalValues = getVariantList();
    }
    return formals;
}


//
// Function to iterate the parameter specs
//
VMI_PROC_PARAM_SPECS_FN(microblazeParamSpecs) {
    if(!prev) {
        return getFormals();
    } else {
        prev++;
        if (prev->name)
            return prev;
        else
            return 0;
    }
}

//
// Get the size of the parameter values table
//
VMI_PROC_PARAM_TABLE_SIZE_FN(microblazeParamValueSize) {
    return sizeof(microblazeParamValues);
}

