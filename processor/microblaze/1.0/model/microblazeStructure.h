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

#ifndef MICROBLAZE_STRUCTURE_H
#define MICROBLAZE_STRUCTURE_H

#include "vmi/vmiTypes.h"
#include "vmi/vmiPorts.h"

#include "microblazeTypeRefs.h"
#include "microblazeVariant.h"
#include "microblazeConfig.h"

#define MICROBLAZE_DISASSEMBLE_MASK 0x00000001
#define MICROBLAZE_DISASSEMBLE(_P) ((_P)->flags & MICROBLAZE_DISASSEMBLE_MASK)

typedef enum microblazeVMModeE {
    VM_MODE_REAL         = 0,
    VM_MODE_FIRST_VIRTUAL,
    VM_MODE_VIRTUAL_PRIV = VM_MODE_FIRST_VIRTUAL,
    VM_MODE_VIRTUAL_USER,
    VM_MODE_LAST_VIRTUAL = VM_MODE_VIRTUAL_USER,
    VM_MODE_LAST
} microblazeVMMode;

//
// Exception Vectors
//
#define RESET_VECTOR                            0x00000000
#define USER_VECTOR                             0x00000008
#define INTERRUPT_VECTOR                        0x00000010
#define BREAK_NMIHW_VECTOR                      0x00000018
#define BREAK_HW_VECTOR                         0x00000018
#define BREAK_SW_VECTOR                         0x00000018
#define HARDWARE_EXCEPTION_VECTOR               0x00000020

//
// Exception Cause Code
//
#define STREAM_EXCEPTION                        0x00
#define UNALIGNED_DATA_ACCESS                   0x01
#define ILLEGAL_OPCODE_EXCEPTION                0x02
#define INSTRUCTION_BUS_ERROR_EXCEPTION         0x03
#define DATA_BUS_ERROR_EXCEPTION                0x04
#define DIVIDE_EXCEPTION                        0x05
#define FLOATING_POINT_UNIT_EXCEPTION           0x06
#define PRIVILEGED_INSTRUCTION_EXCEPTION        0x07
#define STACK_PROTECTION_VIOLATION_EXCEPTION    0x07
#define DATA_STORAGE_EXCEPTION                  0x10
#define INSTRUCTION_STORAGE_EXCEPTION           0x11
#define DATA_TLB_MISS_EXCEPTION                 0x12
#define INSTRUCTION_TLB_MISS_EXCEPTION          0x13

typedef enum microblazeExceptionsS {
    //
    // Synchronous exceptions
    //
    microblaze_E_STREAM_EXCEPTION,
    microblaze_E_UNALIGNED_DATA_ACCESS,
    microblaze_E_ILLEGAL_OPCODE_EXCEPTION,
    microblaze_E_INSTRUCTION_BUS_ERROR_EXCEPTION,
    microblaze_E_DATA_BUS_ERROR_EXCEPTION,
    microblaze_E_DIVIDE_EXCEPTION,
    microblaze_E_FLOATING_POINT_UNIT_EXCEPTION,
    microblaze_E_PRIVILEGED_INSTRUCTION_EXCEPTION,
    microblaze_E_STACK_PROTECTION_VIOLATION_EXCEPTION,
    microblaze_E_DATA_STORAGE_EXCEPTION,
    microblaze_E_INSTRUCTION_STORAGE_EXCEPTION,
    microblaze_E_DATA_TLB_MISS_EXCEPTION,
    microblaze_E_INSTRUCTION_TLB_MISS_EXCEPTION,
    //
    // Asynchronous exceptions
    //
    microblaze_E_RESET,
    microblaze_E_INTERRUPT,
} microblazeException;

typedef struct microblazeNetPortS *microblazeNetPortP, **microblazeNetPortPP;

// this enumeration specifies modes enabling instruction subsets
typedef enum microblazeBlockMaskE {
    BM_NONE       = 0x000,  // no requirement
    BM_MSR_IE     = 0x001,  // Change state of the MSR_IE  (interrupt Enable)
    BM_MSR_BIP    = 0x002,  // Change state of the MSR_BIP (Break In Progress)
    BM_MSR_EE     = 0x004,  // Change state of the MSR_EE  (Exception Enable)
    BM_MSR_EIP    = 0x008,  // Change state of the MSR_EIP (Exception In Progress)
    BM_MSR_UM     = 0x010,  // Change state of the MSR_UM  (User Mode)
    BM_MSR_UMS    = 0x020,  // Change state of the MSR_UMS (User Mode Save)
    BM_MSR_VMS    = 0x040,  // Change state of the MSR_VMS (Virtual Protected Mode Save)
} microblazeBlockMask;

typedef union {
    Uns32 reg;
    struct {
        Uns32        RES : 1;
        Uns32         IE : 1;
        Uns32          C : 1;
        Uns32        BIP : 1;
        Uns32        FSL : 1;
        Uns32        ICE : 1;
        Uns32        DZO : 1;
        Uns32        DCE : 1;
        Uns32         EE : 1;
        Uns32        EIP : 1;
        Uns32        PVR : 1;
        Uns32         UM : 1;
        Uns32        UMS : 1;
        Uns32         VM : 1;
        Uns32        VMS : 1;
        Uns32  reserved0 : 16;
        Uns32         CC : 1;
    } bits;
} TYPE_SPR_MSR;

#define SPR_MSR_EIP_MASK 0x200

typedef union {
    Uns32 reg;
    struct {
    } bits;
} TYPE_SPR_EAR;

typedef union {
    Uns32 reg;
    struct {
        Uns32         EC : 5;
        Uns32        ESS : 7;
        Uns32         DS : 1;
    } bits;
    struct {
        Uns32   padding0 : 5;
        Uns32        FSL : 4;
        Uns32  reserved0 : 3;
    } bits_ESS_EC_00000;
    struct {
        Uns32   padding0 : 5;
        Uns32         Rx : 5;
        Uns32          S : 1;
        Uns32          W : 1;
    } bits_ESS_EC_00001;
    struct {
        Uns32   padding0 : 5;
        Uns32  reserved0 : 7;
    } bits_ESS_EC_00010;
    struct {
        Uns32   padding0 : 5;
        Uns32  reserved0 : 6;
        Uns32        ECC : 1;
    } bits_ESS_EC_00011;
    struct {
        Uns32   padding0 : 5;
        Uns32  reserved0 : 6;
        Uns32        ECC : 1;
    } bits_ESS_EC_00100;
    struct {
        Uns32   padding0 : 5;
        Uns32   padding1 : 6;
        Uns32        DEC : 1;
    } bits_ESS_EC_00101;
    struct {
        Uns32   padding0 : 5;
        Uns32  reserved0 : 7;
    } bits_ESS_EC_00110;
    struct {
        Uns32   padding0 : 5;
        Uns32  reserved0 : 7;
    } bits_ESS_EC_00111;
    struct {
        Uns32   padding0 : 5;
        Uns32  reserved0 : 5;
        Uns32          S : 1;
        Uns32        DIZ : 1;
    } bits_ESS_EC_10000;
    struct {
        Uns32   padding0 : 5;
        Uns32  reserved0 : 6;
        Uns32        DIZ : 1;
    } bits_ESS_EC_10001;
    struct {
        Uns32   padding0 : 5;
        Uns32  reserved0 : 5;
        Uns32          S : 1;
        Uns32  reserved1 : 1;
    } bits_ESS_EC_10010;
    struct {
        Uns32   padding0 : 5;
        Uns32  reserved0 : 7;
    } bits_ESS_EC_10011;
} TYPE_SPR_ESR;

typedef union {
    Uns32 reg;
    struct {
    } bits;
} TYPE_SPR_BTR;

typedef union {
    Uns32 reg;
    struct {
        Uns32         DO : 1;
        Uns32         UF : 1;
        Uns32         OF : 1;
        Uns32         DZ : 1;
        Uns32         IO : 1;
    } bits;
} TYPE_SPR_FSR;

typedef union {
    Uns32 reg;
    struct {
    } bits;
} TYPE_SPR_EDR;

typedef union {
    Uns32 reg;
    struct {
    } bits;
} TYPE_SPR_SLR;

typedef union {
    Uns32 reg;
    struct {
    } bits;
} TYPE_SPR_SHR;

typedef union {
    Uns32 reg;
    struct {
        Uns32        PID : 8;
        Uns32  reserved0 : 24;
    } bits;
} TYPE_SPR_PID;

typedef union {
    Uns32 reg;
    struct {
        Uns32       ZP15 : 2;
        Uns32       ZP14 : 2;
        Uns32       ZP13 : 2;
        Uns32       ZP12 : 2;
        Uns32       ZP11 : 2;
        Uns32       ZP10 : 2;
        Uns32        ZP9 : 2;
        Uns32        ZP8 : 2;
        Uns32        ZP7 : 2;
        Uns32        ZP6 : 2;
        Uns32        ZP5 : 2;
        Uns32        ZP4 : 2;
        Uns32        ZP3 : 2;
        Uns32        ZP2 : 2;
        Uns32        ZP1 : 2;
        Uns32        ZP0 : 2;
    } bits;
} TYPE_SPR_ZPR;

typedef union {
    Uns32 reg;
    struct {
        Uns32          G : 1;
        Uns32          M : 1;
        Uns32          I : 1;
        Uns32          W : 1;
        Uns32       ZSEL : 4;
        Uns32         WR : 1;
        Uns32         EX : 1;
        Uns32        RPN : 22;
    } bits;
} TYPE_SPR_TLBLO;

typedef union {
    Uns32 reg;
    struct {
        Uns32   reserved : 4;
        Uns32         U0 : 1;
        Uns32          E : 1;
        Uns32          V : 1;
        Uns32       SIZE : 3;
        Uns32        TAG : 22;
    } bits;
} TYPE_SPR_TLBHI;

typedef union {
    Uns32 reg;
    struct {
        Uns32      INDEX : 6;
        Uns32   reserved : 25;
        Uns32       MISS : 1;
    } bits;
} TYPE_SPR_TLBX;

typedef union {
    Uns32 reg;
    struct {
        Uns32   reserved : 10;
        Uns32        VPN : 22;
    } bits;
} TYPE_SPR_TLBSX;

typedef union {
    Uns32 reg;
    struct {
        Uns32       USR1 : 8;
        Uns32        MBV : 8;
        Uns32   reserved : 3;
        Uns32      SPROT : 1;
        Uns32         FT : 1;
        Uns32       ENDI : 1;
        Uns32        BTC : 1;
        Uns32        MMU : 1;
        Uns32        DCU : 1;
        Uns32        ICU : 1;
        Uns32        EXC : 1;
        Uns32        FPU : 1;
        Uns32        MUL : 1;
        Uns32        DIV : 1;
        Uns32         BS : 1;
        Uns32        CFG : 1;
    } bits;
} TYPE_SPR_PVR0;

typedef union {
    Uns32 reg;
    struct {
        Uns32       USR2 : 32;
    } bits;
} TYPE_SPR_PVR1;

typedef union {
    Uns32 reg;
    struct {
        Uns32     FPUEXC : 1;
        Uns32     DIVEXC : 1;
        Uns32    AXIDEXC : 1;
        Uns32    AXIIEXC : 1;
        Uns32      OPEXC : 1;
        Uns32      UNEXC : 1;
        Uns32     OP0EXC : 1;
        Uns32    DPLBEXC : 1;
        Uns32    IPLBEXC : 1;
        Uns32       FPU2 : 1;
        Uns32      MUL64 : 1;
        Uns32        FPU : 1;
        Uns32        MUL : 1;
        Uns32        DIV : 1;
        Uns32         BS : 1;
        Uns32       AREA : 1;
        Uns32       PCMP : 1;
        Uns32        MSR : 1;
        Uns32     FSLEXC : 1;
        Uns32        FSL : 1;
        Uns32   reserved : 2;
        Uns32     STREAM : 1;
        Uns32   INTERCON : 1;
        Uns32       IPLB : 1;
        Uns32       DPLB : 1;
        Uns32     IRQPOS : 1;
        Uns32    IRQEDGE : 1;
        Uns32       ILMB : 1;
        Uns32       IAXI : 1;
        Uns32       DLMB : 1;
        Uns32       DAXI : 1;
    } bits;
} TYPE_SPR_PVR2;

typedef union {
    Uns32 reg;
    struct {
        Uns32   BTC_SIZE : 3;
        Uns32  reserved0 : 4;
        Uns32        FSL : 5;
        Uns32  reserved1 : 1;
        Uns32     WRADDR : 3;
        Uns32  reserved2 : 3;
        Uns32     RDADDR : 3;
        Uns32  reserved3 : 3;
        Uns32      PCBRK : 4;
        Uns32  reserved4 : 2;
        Uns32     DEBUG_ : 1;
    } bits;
} TYPE_SPR_PVR3;

typedef union {
    Uns32 reg;
    struct {
        Uns32  reserved0 : 6;
        Uns32       ICDW : 1;
        Uns32       IFTL : 1;
        Uns32        ICS : 2;
        Uns32        ICV : 3;
        Uns32        ICI : 1;
        Uns32  reserved1 : 1;
        Uns32        IAU : 1;
        Uns32       ICBS : 5;
        Uns32       ICLL : 3;
        Uns32        ICW : 1;
        Uns32  reserved2 : 1;
        Uns32       ICTS : 5;
        Uns32        ICU : 1;
    } bits;
} TYPE_SPR_PVR4;

typedef union {
    Uns32 reg;
    struct {
        Uns32  reserved0 : 6;
        Uns32       DCDW : 1;
        Uns32       DFTL : 1;
        Uns32  reserved1 : 2;
        Uns32        DCV : 3;
        Uns32        DCI : 1;
        Uns32        DWB : 1;
        Uns32        DAU : 1;
        Uns32       DCBS : 5;
        Uns32       DCLL : 3;
        Uns32        DCW : 1;
        Uns32  reserved2 : 1;
        Uns32       DCTS : 5;
        Uns32        DCU : 1;
    } bits;
} TYPE_SPR_PVR5;

typedef union {
    Uns32 reg;
    struct {
        Uns32       ICBA : 32;
    } bits;
} TYPE_SPR_PVR6;

typedef union {
    Uns32 reg;
    struct {
        Uns32       ICHA : 32;
    } bits;
} TYPE_SPR_PVR7;

typedef union {
    Uns32 reg;
    struct {
        Uns32       DCBA : 32;
    } bits;
} TYPE_SPR_PVR8;

typedef union {
    Uns32 reg;
    struct {
        Uns32       DCHA : 32;
    } bits;
} TYPE_SPR_PVR9;

typedef union {
    Uns32 reg;
    struct {
        Uns32  reserved0 : 24;
        Uns32       ARCH : 8;
    } bits;
} TYPE_SPR_PVR10;

typedef union {
    Uns32 reg;
    struct {
        Uns32     RSTMSR : 15;
        Uns32  reserved0 : 1;
        Uns32    PRIVINS : 1;
        Uns32      ZONES : 5;
        Uns32     TLBACC : 2;
        Uns32       DTLB : 3;
        Uns32       ITLB : 3;
        Uns32        MMU : 2;
    } bits;
} TYPE_SPR_PVR11;

//
// This enumerates access actions for the TLB miss handler
//
typedef enum microblazeTLBActionE {
    TLBA_FETCH,         // access is an instruction fetch
    TLBA_LOAD,          // access is a data load
    TLBA_STORE,         // access is a data store
} microblazeTLBAction;

//
// TLB Table Definitions
//
#define UTLBENTRIES 64
typedef struct {
    TYPE_SPR_TLBLO  LO;
    TYPE_SPR_TLBHI  HI;
    Uns8            TID;
    Uns8            simPriv[VM_MODE_LAST];
} TLBentry, *TLBentryP;

typedef struct {
    TLBentryP       entry;
    Uns8            size;
} TLBtable, *TLBtableP;

typedef struct microblazeS {
    //
    // User Defined and Builtin Registers
    //
    Uns32 gpr[32];
    Uns32 SPR_PC;

    TYPE_SPR_MSR SPR_MSR, SPR_MSR_rmask, SPR_MSR_wmask;
    TYPE_SPR_EAR SPR_EAR, SPR_EAR_rmask, SPR_EAR_wmask;
    TYPE_SPR_ESR SPR_ESR, SPR_ESR_rmask, SPR_ESR_wmask;
    TYPE_SPR_BTR SPR_BTR, SPR_BTR_rmask, SPR_BTR_wmask;
    TYPE_SPR_FSR SPR_FSR, SPR_FSR_rmask, SPR_FSR_wmask;
    TYPE_SPR_EDR SPR_EDR, SPR_EDR_rmask, SPR_EDR_wmask;
    TYPE_SPR_SLR SPR_SLR, SPR_SLR_rmask, SPR_SLR_wmask;
    TYPE_SPR_SHR SPR_SHR, SPR_SHR_rmask, SPR_SHR_wmask;
    TYPE_SPR_PID SPR_PID, SPR_PID_rmask, SPR_PID_wmask;
    TYPE_SPR_ZPR SPR_ZPR, SPR_ZPR_rmask, SPR_ZPR_wmask;
    TYPE_SPR_TLBLO SPR_TLBLO, SPR_TLBLO_rmask, SPR_TLBLO_wmask;
    TYPE_SPR_TLBHI SPR_TLBHI, SPR_TLBHI_rmask, SPR_TLBHI_wmask;
    TYPE_SPR_TLBX SPR_TLBX, SPR_TLBX_rmask, SPR_TLBX_wmask;
    TYPE_SPR_TLBSX SPR_TLBSX, SPR_TLBSX_rmask, SPR_TLBSX_wmask;
    TYPE_SPR_PVR0 SPR_PVR0, SPR_PVR0_rmask, SPR_PVR0_wmask;
    TYPE_SPR_PVR1 SPR_PVR1, SPR_PVR1_rmask, SPR_PVR1_wmask;
    TYPE_SPR_PVR2 SPR_PVR2, SPR_PVR2_rmask, SPR_PVR2_wmask;
    TYPE_SPR_PVR3 SPR_PVR3, SPR_PVR3_rmask, SPR_PVR3_wmask;
    TYPE_SPR_PVR4 SPR_PVR4, SPR_PVR4_rmask, SPR_PVR4_wmask;
    TYPE_SPR_PVR5 SPR_PVR5, SPR_PVR5_rmask, SPR_PVR5_wmask;
    TYPE_SPR_PVR6 SPR_PVR6, SPR_PVR6_rmask, SPR_PVR6_wmask;
    TYPE_SPR_PVR7 SPR_PVR7, SPR_PVR7_rmask, SPR_PVR7_wmask;
    TYPE_SPR_PVR8 SPR_PVR8, SPR_PVR8_rmask, SPR_PVR8_wmask;
    TYPE_SPR_PVR9 SPR_PVR9, SPR_PVR9_rmask, SPR_PVR9_wmask;
    TYPE_SPR_PVR10 SPR_PVR10, SPR_PVR10_rmask, SPR_PVR10_wmask;
    TYPE_SPR_PVR11 SPR_PVR11, SPR_PVR11_rmask, SPR_PVR11_wmask;

    Uns32 tmp[4];
    Bool FLG_tmp;
    Bool FLG_reservation;

    // Psuedo-register to hold floating point sticky flags in VMI format
    vmiFPFlags FLG_FP;
    Uns8       FLG_DO;

    // Auto-generated psuedo-registers to hold VMI-format conditional flag values
    Bool FLG_CC;
    Bool FLG_PA;
    Bool FLG_ZR;
    Bool FLG_SI;
    Bool FLG_OV;

    //
    // TLB Table Pointers
    //
    TLBtable utlb;     // UTLBENTRIES deep

    //
    // Model Support Registers
    //
    Uns32 flags;
    microblazeConfig configInfo;
    Bool verbose;
    memEndian dendian;
    memEndian iendian;
    microblazeException exception;
    struct {
        Uns32 C_FAMILY;
        Uns32 C_AREA_OPTIMIZED;
        Uns32 C_INTERCONNECT;
        Uns32 C_ENDIANNESS;
        Uns32 C_FAULT_TOLERANT;
        Uns32 C_ECC_USE_CE_EXCEPTION;
        Uns32 C_PVR;
        Uns32 C_PVR_USER1;
        Uns32 C_PVR_USER2;
        Uns32 C_RESET_MSR; // Enumeration
        Uns32 C_D_PLB;
        Uns32 C_D_AXI;
        Uns32 C_D_LMB;
        Uns32 C_I_PLB;
        Uns32 C_I_AXI;
        Uns32 C_I_LMB;
        Uns32 C_USE_BARREL;
        Uns32 C_USE_DIV;
        Uns32 C_USE_HW_MUL;
        Uns32 C_USE_FPU;
        Uns32 C_USE_MSR_INSTR;
        Uns32 C_USE_PCMP_INSTR;
        Uns32 C_UNALIGNED_EXCEPTIONS;
        Uns32 C_ILL_OPCODE_EXCEPTION;
        Uns32 C_IPLB_BUS_EXCEPTION;
        Uns32 C_DPLB_BUS_EXCEPTION;
        Uns32 C_M_AXI_I_BUS_EXCEPTION;
        Uns32 C_M_AXI_D_BUS_EXCEPTION;
        Uns32 C_DIV_ZERO_EXCEPTION;
        Uns32 C_FPU_EXCEPTION;
        Uns32 C_OPCODE_0x0_ILLEGAL;
        Uns32 C_FSL_EXCEPTION;
        Uns32 C_USE_STACK_PROTECTION;
        Uns32 C_DEBUG_ENABLED;
        Uns32 C_NUMBER_OF_PC_BRK;
        Uns32 C_NUMBER_OF_RD_ADDR_BRK;
        Uns32 C_NUMBER_OF_WR_ADDR_BRK;
        Uns32 C_INTERRUPT_IS_EDGE;
        Uns32 C_EDGE_IS_POSITIVE;
        Uns32 C_FSL_LINKS;
        Uns32 C_USE_EXTENDED_FSL_INSTR;
        Uns32 C_ICACHE_BASEADDR;
        Uns32 C_ICACHE_HIGHADDR;
        Uns32 C_USE_ICACHE;
        Uns32 C_ALLOW_ICACHE_WR;
        Uns32 C_ICACHE_LINE_LEN; // Enumeration
        Uns32 C_ICACHE_ALWAYS_USED;
        Uns32 C_ICACHE_INTERFACE; // Enumeration
        Uns32 C_ICACHE_FORCE_TAG_LUTRAM;
        Uns32 C_ICACHE_STREAMS;
        Uns32 C_ICACHE_VICTIMS; // Enumeration
        Uns32 C_ICACHE_DATA_WIDTH;
        Uns32 C_ADDR_TAG_BITS;
        Uns32 C_CACHE_BYTE_SIZE; // Enumeration
        Uns32 C_ICACHE_USE_FSL;
        Uns32 C_DCACHE_BASEADDR;
        Uns32 C_DCACHE_HIGHADDR;
        Uns32 C_USE_DCACHE;
        Uns32 C_ALLOW_DCACHE_WR;
        Uns32 C_DCACHE_LINE_LEN; // Enumeration
        Uns32 C_DCACHE_ALWAYS_USED;
        Uns32 C_DCACHE_INTERFACE; // Enumeration
        Uns32 C_DCACHE_FORCE_TAG_LUTRAM;
        Uns32 C_DCACHE_USE_WRITEBACK;
        Uns32 C_DCACHE_VICTIMS; // Enumeration
        Uns32 C_DCACHE_DATA_WIDTH;
        Uns32 C_DCACHE_ADDR_TAG;
        Uns32 C_DCACHE_BYTE_SIZE; // Enumeration
        Uns32 C_DCACHE_USE_FSL;
        Uns32 C_USE_MMU;
        Uns32 C_MMU_DTLB_SIZE;
        Uns32 C_MMU_ITLB_SIZE;
        Uns32 C_MMU_TLB_ACCESS;
        Uns32 C_MMU_ZONES;
        Uns32 C_MMU_PRIVILEGED_INSTR;
        Uns32 C_USE_INTERRUPT;
        Uns32 C_USE_EXT_BRK;
        Uns32 C_USE_EXT_NM_BRK;
        Uns32 C_USE_BRANCH_TARGET_CACHE;
        Uns32 C_BRANCH_TARGET_CACHE_SIZE;
        Uns32 C_STREAM_INTERCONNECT;
    } params;

    vmiFifoPortP   fslPorts;            // fifo port descriptions

    //
    // An FSL is {32'ctl-id, 32.data}
    //
    vmiConnInputP  SFSL[16];        // input data FIFO connection
    vmiConnOutputP MFSL[16];        // output data FIFO connection
    vmiConnInputP  SFSLdyn;         // Placeholder for runtime callback using dynamic method
    vmiConnOutputP MFSLdyn;         // Placeholder for runtime callback using dynamic method

    //
    // Core signals
    //
    Bool        ResetPending;
    Uns32       Reset;
    Uns32       Interrupt;
    Bool        InterruptPos, InterruptNeg, InterruptActive;
    Uns32       MB_Reset;
    Uns32       Ext_BRK;
    Uns32       Ext_NM_BRK;

    //
    // Bus ports
    //
    vmiBusPortP busPorts;

    // Net ports
    microblazeNetPortP netPorts;

    //
    // Virtual memory
    //
    memDomainP pDomain;
    memDomainP vDomain[VM_MODE_LAST];

} microblaze;

#define MICROBLAZE_CPU_OFFSET(_F) VMI_CPU_OFFSET(microblazeP, _F)
#define MICROBLAZE_CPU_REG(_F) VMI_CPU_REG(microblazeP, _F)

#define MICROBLAZE_MFSL(_R) MICROBLAZE_CPU_REG(MFSL[_R])
#define MICROBLAZE_SFSL(_R) MICROBLAZE_CPU_REG(SFSL[_R])
#define MICROBLAZE_FSL_BITS (sizeof(Uns64) * 8)
#define MICROBLAZE_FSL_DATA 0
#define MICROBLAZE_FSL_CONTROL 1
#define MICROBLAZE_SFSL_DYN MICROBLAZE_CPU_REG(SFSLdyn)
#define MICROBLAZE_MFSL_DYN MICROBLAZE_CPU_REG(MFSLdyn)

#define MICROBLAZE_GPR_BITS (sizeof(Uns32) * 8)
#define MICROBLAZE_GPR_RD(_R) MICROBLAZE_CPU_REG(gpr[_R])
#define MICROBLAZE_GPR_WR(_R) ((_R==0)) ? VMI_NOREG : MICROBLAZE_CPU_REG(gpr[_R])

#define MICROBLAZE_SPR_PC_BITS (sizeof(Uns32) * 8)
#define MICROBLAZE_SPR_PC_RD MICROBLAZE_CPU_REG(SPR_PC.reg)
#define MICROBLAZE_SPR_PC_WR MICROBLAZE_CPU_REG(SPR_PC.reg)

#define MICROBLAZE_SPR_MSR_BITS (sizeof(Uns32) * 8)
#define MICROBLAZE_SPR_MSR_RD MICROBLAZE_CPU_REG(SPR_MSR.reg)
#define MICROBLAZE_SPR_MSR_WR MICROBLAZE_CPU_REG(SPR_MSR.reg)

#define MICROBLAZE_SPR_EAR_BITS (sizeof(Uns32) * 8)
#define MICROBLAZE_SPR_EAR_RD MICROBLAZE_CPU_REG(SPR_EAR.reg)
#define MICROBLAZE_SPR_EAR_WR MICROBLAZE_CPU_REG(SPR_EAR.reg)

#define MICROBLAZE_SPR_ESR_BITS (sizeof(Uns32) * 8)
#define MICROBLAZE_SPR_ESR_RD MICROBLAZE_CPU_REG(SPR_ESR.reg)
#define MICROBLAZE_SPR_ESR_WR MICROBLAZE_CPU_REG(SPR_ESR.reg)

#define MICROBLAZE_SPR_BTR_BITS (sizeof(Uns32) * 8)
#define MICROBLAZE_SPR_BTR_RD MICROBLAZE_CPU_REG(SPR_BTR.reg)
#define MICROBLAZE_SPR_BTR_WR MICROBLAZE_CPU_REG(SPR_BTR.reg)

#define MICROBLAZE_SPR_FSR_BITS (sizeof(Uns32) * 8)
#define MICROBLAZE_SPR_FSR_RD MICROBLAZE_CPU_REG(SPR_FSR.reg)
#define MICROBLAZE_SPR_FSR_WR MICROBLAZE_CPU_REG(SPR_FSR.reg)

#define MICROBLAZE_SPR_EDR_BITS (sizeof(Uns32) * 8)
#define MICROBLAZE_SPR_EDR_RD MICROBLAZE_CPU_REG(SPR_EDR.reg)
#define MICROBLAZE_SPR_EDR_WR MICROBLAZE_CPU_REG(SPR_EDR.reg)

#define MICROBLAZE_SPR_SLR_BITS (sizeof(Uns32) * 8)
#define MICROBLAZE_SPR_SLR_RD MICROBLAZE_CPU_REG(SPR_SLR.reg)
#define MICROBLAZE_SPR_SLR_WR MICROBLAZE_CPU_REG(SPR_SLR.reg)

#define MICROBLAZE_SPR_SHR_BITS (sizeof(Uns32) * 8)
#define MICROBLAZE_SPR_SHR_RD MICROBLAZE_CPU_REG(SPR_SHR.reg)
#define MICROBLAZE_SPR_SHR_WR MICROBLAZE_CPU_REG(SPR_SHR.reg)

#define MICROBLAZE_SPR_PID_BITS (sizeof(Uns32) * 8)
#define MICROBLAZE_SPR_PID_RD MICROBLAZE_CPU_REG(SPR_PID.reg)
#define MICROBLAZE_SPR_PID_WR MICROBLAZE_CPU_REG(SPR_PID.reg)

#define MICROBLAZE_SPR_ZPR_BITS (sizeof(Uns32) * 8)
#define MICROBLAZE_SPR_ZPR_RD MICROBLAZE_CPU_REG(SPR_ZPR.reg)
#define MICROBLAZE_SPR_ZPR_WR MICROBLAZE_CPU_REG(SPR_ZPR.reg)

#define MICROBLAZE_SPR_TLBLO_BITS (sizeof(Uns32) * 8)
#define MICROBLAZE_SPR_TLBLO_RD MICROBLAZE_CPU_REG(SPR_TLBLO.reg)
#define MICROBLAZE_SPR_TLBLO_WR MICROBLAZE_CPU_REG(SPR_TLBLO.reg)

#define MICROBLAZE_SPR_TLBHI_BITS (sizeof(Uns32) * 8)
#define MICROBLAZE_SPR_TLBHI_RD MICROBLAZE_CPU_REG(SPR_TLBHI.reg)
#define MICROBLAZE_SPR_TLBHI_WR MICROBLAZE_CPU_REG(SPR_TLBHI.reg)

#define MICROBLAZE_SPR_TLBX_BITS (sizeof(Uns32) * 8)
#define MICROBLAZE_SPR_TLBX_RD MICROBLAZE_CPU_REG(SPR_TLBX.reg)
#define MICROBLAZE_SPR_TLBX_WR MICROBLAZE_CPU_REG(SPR_TLBX.reg)

#define MICROBLAZE_SPR_TLBSX_BITS (sizeof(Uns32) * 8)
#define MICROBLAZE_SPR_TLBSX_RD MICROBLAZE_CPU_REG(SPR_TLBSX.reg)
#define MICROBLAZE_SPR_TLBSX_WR MICROBLAZE_CPU_REG(SPR_TLBSX.reg)
#define MICROBLAZE_SPR_TLBSX_VA_MASK 0x3ff

#define MICROBLAZE_SPR_PVR0_BITS (sizeof(Uns32) * 8)
#define MICROBLAZE_SPR_PVR0_RD MICROBLAZE_CPU_REG(SPR_PVR0.reg)
#define MICROBLAZE_SPR_PVR0_WR MICROBLAZE_CPU_REG(SPR_PVR0.reg)

#define MICROBLAZE_SPR_PVR1_BITS (sizeof(Uns32) * 8)
#define MICROBLAZE_SPR_PVR1_RD MICROBLAZE_CPU_REG(SPR_PVR1.reg)
#define MICROBLAZE_SPR_PVR1_WR MICROBLAZE_CPU_REG(SPR_PVR1.reg)

#define MICROBLAZE_SPR_PVR2_BITS (sizeof(Uns32) * 8)
#define MICROBLAZE_SPR_PVR2_RD MICROBLAZE_CPU_REG(SPR_PVR2.reg)
#define MICROBLAZE_SPR_PVR2_WR MICROBLAZE_CPU_REG(SPR_PVR2.reg)

#define MICROBLAZE_SPR_PVR3_BITS (sizeof(Uns32) * 8)
#define MICROBLAZE_SPR_PVR3_RD MICROBLAZE_CPU_REG(SPR_PVR3.reg)
#define MICROBLAZE_SPR_PVR3_WR MICROBLAZE_CPU_REG(SPR_PVR3.reg)

#define MICROBLAZE_SPR_PVR4_BITS (sizeof(Uns32) * 8)
#define MICROBLAZE_SPR_PVR4_RD MICROBLAZE_CPU_REG(SPR_PVR4.reg)
#define MICROBLAZE_SPR_PVR4_WR MICROBLAZE_CPU_REG(SPR_PVR4.reg)

#define MICROBLAZE_SPR_PVR5_BITS (sizeof(Uns32) * 8)
#define MICROBLAZE_SPR_PVR5_RD MICROBLAZE_CPU_REG(SPR_PVR5.reg)
#define MICROBLAZE_SPR_PVR5_WR MICROBLAZE_CPU_REG(SPR_PVR5.reg)

#define MICROBLAZE_SPR_PVR6_BITS (sizeof(Uns32) * 8)
#define MICROBLAZE_SPR_PVR6_RD MICROBLAZE_CPU_REG(SPR_PVR6.reg)
#define MICROBLAZE_SPR_PVR6_WR MICROBLAZE_CPU_REG(SPR_PVR6.reg)

#define MICROBLAZE_SPR_PVR7_BITS (sizeof(Uns32) * 8)
#define MICROBLAZE_SPR_PVR7_RD MICROBLAZE_CPU_REG(SPR_PVR7.reg)
#define MICROBLAZE_SPR_PVR7_WR MICROBLAZE_CPU_REG(SPR_PVR7.reg)

#define MICROBLAZE_SPR_PVR8_BITS (sizeof(Uns32) * 8)
#define MICROBLAZE_SPR_PVR8_RD MICROBLAZE_CPU_REG(SPR_PVR8.reg)
#define MICROBLAZE_SPR_PVR8_WR MICROBLAZE_CPU_REG(SPR_PVR8.reg)

#define MICROBLAZE_SPR_PVR9_BITS (sizeof(Uns32) * 8)
#define MICROBLAZE_SPR_PVR9_RD MICROBLAZE_CPU_REG(SPR_PVR9.reg)
#define MICROBLAZE_SPR_PVR9_WR MICROBLAZE_CPU_REG(SPR_PVR9.reg)

#define MICROBLAZE_SPR_PVR10_BITS (sizeof(Uns32) * 8)
#define MICROBLAZE_SPR_PVR10_RD MICROBLAZE_CPU_REG(SPR_PVR10)
#define MICROBLAZE_SPR_PVR10_WR MICROBLAZE_CPU_REG(SPR_PVR10)

#define MICROBLAZE_SPR_PVR11_BITS (sizeof(Uns32) * 8)
#define MICROBLAZE_SPR_PVR11_RD MICROBLAZE_CPU_REG(SPR_PVR11.reg)
#define MICROBLAZE_SPR_PVR11_WR MICROBLAZE_CPU_REG(SPR_PVR11.reg)

#define MICROBLAZE_TMP_BITS (sizeof(Uns32) * 8)
#define MICROBLAZE_TMP_RD(_R) MICROBLAZE_CPU_REG(tmp[_R])
#define MICROBLAZE_TMP_WR(_R) MICROBLAZE_CPU_REG(tmp[_R])

#define MICROBLAZE_FLG_TMP_BITS (sizeof(Bool) * 8)
#define MICROBLAZE_FLG_TMP_RD MICROBLAZE_CPU_REG(FLG_tmp)
#define MICROBLAZE_FLG_TMP_WR MICROBLAZE_CPU_REG(FLG_tmp)

#define MICROBLAZE_FLG_RESERVATION_BITS (sizeof(Bool) * 8)
#define MICROBLAZE_FLG_RESERVATION_RD MICROBLAZE_CPU_REG(FLG_reservation)
#define MICROBLAZE_FLG_RESERVATION_WR MICROBLAZE_CPU_REG(FLG_reservation)

#define MICROBLAZE_FLG_CC_BITS (sizeof(Bool) * 8)
#define MICROBLAZE_FLG_CC_RD MICROBLAZE_CPU_REG(FLG_CC)
#define MICROBLAZE_FLG_CC_WR MICROBLAZE_CPU_REG(FLG_CC)

#define MICROBLAZE_FLG_PA_BITS (sizeof(Bool) * 8)
#define MICROBLAZE_FLG_PA_RD MICROBLAZE_CPU_REG(FLG_PA)
#define MICROBLAZE_FLG_PA_WR MICROBLAZE_CPU_REG(FLG_PA)

#define MICROBLAZE_FLG_ZR_BITS (sizeof(Bool) * 8)
#define MICROBLAZE_FLG_ZR_RD MICROBLAZE_CPU_REG(FLG_ZR)
#define MICROBLAZE_FLG_ZR_WR MICROBLAZE_CPU_REG(FLG_ZR)

#define MICROBLAZE_FLG_SI_BITS (sizeof(Bool) * 8)
#define MICROBLAZE_FLG_SI_RD MICROBLAZE_CPU_REG(FLG_SI)
#define MICROBLAZE_FLG_SI_WR MICROBLAZE_CPU_REG(FLG_SI)

#define MICROBLAZE_FLG_OV_BITS (sizeof(Bool) * 8)
#define MICROBLAZE_FLG_OV_RD MICROBLAZE_CPU_REG(FLG_OV)
#define MICROBLAZE_FLG_OV_WR MICROBLAZE_CPU_REG(FLG_OV)

#define MICROBLAZE_FLG_FP_BITS (sizeof(Bool) * 8)
#define MICROBLAZE_FLG_FP_RD MICROBLAZE_CPU_REG(FLG_FP)
#define MICROBLAZE_FLG_FP_WR MICROBLAZE_CPU_REG(FLG_FP)

#define MICROBLAZE_FLG_DO_BITS (sizeof(Bool) * 8)
#define MICROBLAZE_FLG_DO_RD MICROBLAZE_CPU_REG(FLG_DO)
#define MICROBLAZE_FLG_DO_WR MICROBLAZE_CPU_REG(FLG_DO)

#define MICROBLAZE_CPU_REG_CONST(_F) VMI_CPU_REG_CONST(microblazeP, _F)
#define MICROBLAZE_GPR_CONST(_R) MICROBLAZE_CPU_REG_CONST(gpr[_R])

#define MICROBLAZE_SPR_PC_CONST MICROBLAZE_CPU_REG_CONST(SPR_PC)

#define MICROBLAZE_SPR_MSR_CONST MICROBLAZE_CPU_REG_CONST(SPR_MSR)

#define MICROBLAZE_SPR_EAR_CONST MICROBLAZE_CPU_REG_CONST(SPR_EAR)

#define MICROBLAZE_SPR_ESR_CONST MICROBLAZE_CPU_REG_CONST(SPR_ESR)

#define MICROBLAZE_SPR_BTR_CONST MICROBLAZE_CPU_REG_CONST(SPR_BTR)

#define MICROBLAZE_SPR_FSR_CONST MICROBLAZE_CPU_REG_CONST(SPR_FSR)

#define MICROBLAZE_SPR_EDR_CONST MICROBLAZE_CPU_REG_CONST(SPR_EDR)

#define MICROBLAZE_SPR_SLR_CONST MICROBLAZE_CPU_REG_CONST(SPR_SLR)

#define MICROBLAZE_SPR_SHR_CONST MICROBLAZE_CPU_REG_CONST(SPR_SHR)

#define MICROBLAZE_SPR_PID_CONST MICROBLAZE_CPU_REG_CONST(SPR_PID)

#define MICROBLAZE_SPR_ZPR_CONST MICROBLAZE_CPU_REG_CONST(SPR_ZPR)

#define MICROBLAZE_SPR_TLBLO_CONST MICROBLAZE_CPU_REG_CONST(SPR_TLBLO)

#define MICROBLAZE_SPR_TLBHI_CONST MICROBLAZE_CPU_REG_CONST(SPR_TLBHI)

#define MICROBLAZE_SPR_TLBX_CONST MICROBLAZE_CPU_REG_CONST(SPR_TLBX)

#define MICROBLAZE_SPR_TLBSX_CONST MICROBLAZE_CPU_REG_CONST(SPR_TLBSX)

#define MICROBLAZE_SPR_PVR0_CONST MICROBLAZE_CPU_REG_CONST(SPR_PVR0)

#define MICROBLAZE_SPR_PVR1_CONST MICROBLAZE_CPU_REG_CONST(SPR_PVR1)

#define MICROBLAZE_SPR_PVR2_CONST MICROBLAZE_CPU_REG_CONST(SPR_PVR2)

#define MICROBLAZE_SPR_PVR3_CONST MICROBLAZE_CPU_REG_CONST(SPR_PVR3)

#define MICROBLAZE_SPR_PVR4_CONST MICROBLAZE_CPU_REG_CONST(SPR_PVR4)

#define MICROBLAZE_SPR_PVR5_CONST MICROBLAZE_CPU_REG_CONST(SPR_PVR5)

#define MICROBLAZE_SPR_PVR6_CONST MICROBLAZE_CPU_REG_CONST(SPR_PVR6)

#define MICROBLAZE_SPR_PVR7_CONST MICROBLAZE_CPU_REG_CONST(SPR_PVR7)

#define MICROBLAZE_SPR_PVR8_CONST MICROBLAZE_CPU_REG_CONST(SPR_PVR8)

#define MICROBLAZE_SPR_PVR9_CONST MICROBLAZE_CPU_REG_CONST(SPR_PVR9)

#define MICROBLAZE_SPR_PVR10_CONST MICROBLAZE_CPU_REG_CONST(SPR_PVR10)

#define MICROBLAZE_SPR_PVR11_CONST MICROBLAZE_CPU_REG_CONST(SPR_PVR11)

#define MICROBLAZE_TMP_CONST(_R) MICROBLAZE_CPU_REG_CONST(tmp[_R])

#define MICROBLAZE_FLG_TMP_CONST MICROBLAZE_CPU_REG_CONST(FLG_tmp)

#define MICROBLAZE_FLG_RESERVATION_CONST MICROBLAZE_CPU_REG_CONST(FLG_reservation)

#define MICROBLAZE_FLG_CC_CONST MICROBLAZE_CPU_REG_CONST(FLG_CC)

#define MICROBLAZE_FLG_PA_CONST MICROBLAZE_CPU_REG_CONST(FLG_PA)

#define MICROBLAZE_FLG_ZR_CONST MICROBLAZE_CPU_REG_CONST(FLG_ZR)

#define MICROBLAZE_FLG_SI_CONST MICROBLAZE_CPU_REG_CONST(FLG_SI)

#define MICROBLAZE_FLG_OV_CONST MICROBLAZE_CPU_REG_CONST(FLG_OV)


#define MICROBLAZE_SPR_RPC 0x0000
#define MICROBLAZE_SPR_RMSR 0x0001
#define MICROBLAZE_SPR_REAR 0x0003
#define MICROBLAZE_SPR_RESR 0x0005
#define MICROBLAZE_SPR_RFSR 0x0007
#define MICROBLAZE_SPR_RBTR 0x000B
#define MICROBLAZE_SPR_REDR 0x000D
#define MICROBLAZE_SPR_RSLR 0x0800
#define MICROBLAZE_SPR_RSHR 0x0802
#define MICROBLAZE_SPR_RPID 0x1000
#define MICROBLAZE_SPR_RZPR 0x1001
#define MICROBLAZE_SPR_RTLBX 0x1002
#define MICROBLAZE_SPR_RTLBLO 0x1003
#define MICROBLAZE_SPR_RTLBHI 0x1004
#define MICROBLAZE_SPR_RTLBSX 0x1005
#define MICROBLAZE_SPR_RPVR0 0x2000
#define MICROBLAZE_SPR_RPVR1 0x2001
#define MICROBLAZE_SPR_RPVR2 0x2002
#define MICROBLAZE_SPR_RPVR3 0x2003
#define MICROBLAZE_SPR_RPVR4 0x2004
#define MICROBLAZE_SPR_RPVR5 0x2005
#define MICROBLAZE_SPR_RPVR6 0x2006
#define MICROBLAZE_SPR_RPVR7 0x2007
#define MICROBLAZE_SPR_RPVR8 0x2008
#define MICROBLAZE_SPR_RPVR9 0x2009
#define MICROBLAZE_SPR_RPVR10 0x200A
#define MICROBLAZE_SPR_RPVR11 0x200B

#define MICROBLAZE_MSR_IE  0x0002
#define MICROBLAZE_MSR_C   0x0004
#define MICROBLAZE_MSR_BIP 0x0008
#define MICROBLAZE_MSR_UMS 0x1000
#define MICROBLAZE_MSR_VMS 0x4000

#define CPU_PREFIX "MICROBLAZE"

typedef Uns32 microblazeAddr;

#endif // MICROBLAZE_RPVR11_H
