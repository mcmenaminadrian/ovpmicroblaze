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

// VMI area includes
#include "vmi/vmiMessage.h"
#include "vmi/vmiOSAttrs.h"
#include "vmi/vmiOSLib.h"
#include "vmi/vmiRt.h"
#include "vmi/vmiVersion.h"

// Message prefix
#define CPU_PREFIX "MICROBLAZE_NEWLIB"

// Number of arguments passed in regs as defined by ABI
#define REG_ARG_NUM  6

// Number of file descriptors supported
#define FILE_DES_NUM 128

// This is the offset from _impure_ptr to the errno field
// In microblaze toolchain, when compiled with _REENT_SMALL defined this is 12
// rather than the standard value of 0
#define ERRNO_OFFSET 12

////////////////////////////////////////////////////////////////////////////////
// TYPES
////////////////////////////////////////////////////////////////////////////////

typedef struct vmiosObjectS {

    // first three argument registers (standard ABI)
    vmiRegInfoCP args[REG_ARG_NUM];

    // return register (standard ABI)
    vmiRegInfoCP resultReg;

    // stack pointer register (standard ABI)
    vmiRegInfoCP argSP;
    char **argv, **envp;

    // Info for errno handling (set and used in newlibSemiHost.c.h)
    Addr         impurePtrAddr;
    memDomainP   impurePtrDomain;
    Addr         errnoAddr;
    memDomainP   errnoDomain;

    // file descriptor table (set and used in newlibSemiHost.c.h)
    Int32 fileDescriptors[FILE_DES_NUM];

} vmiosObject;

//
// microblaze/newlib stat structure
//
typedef struct {
    Uns32 _u1;      //  0:
    Uns32 mode;     //  4: mode
    Uns32 _u2;      //  8:
    Uns32 _u3;      // 12:
    Uns32 size;     // 16: size
    Uns32 atime;    // 20: atime
    Uns32 _u4;      // 24:
    Uns32 mtime;    // 28: mtime
    Uns32 _u5;      // 32:
    Uns32 ctime;    // 36: ctime
    Uns32 _u6;      // 40:
    Uns32 blksize;  // 44: blksize
    Uns32 blocks;   // 48: blocks
} newlibStat;


////////////////////////////////////////////////////////////////////////////////
// UTILITIES REQUIRED BY newlibSemiHost.c.h
////////////////////////////////////////////////////////////////////////////////

//
// Get the Constant pointer for the Stack Pointer
//
static vmiRegInfoCP getSPCP(
    vmiProcessorP processor,
    vmiosObjectP  object
) {
    return object->argSP;
}

//
// Get the Constant pointer for the register
//
static vmiRegInfoCP getArgCP(
    vmiProcessorP processor,
    vmiosObjectP  object,
    Uns32         index
) {
    vmiRegInfoCP regCP = 0;

    if(index>=REG_ARG_NUM) {
        vmiMessage("P", CPU_PREFIX"_ANS",
            "No more than %u function arguments supported",
            REG_ARG_NUM
        );
        vmirtFinish(-1);

    } else {
        regCP = object->args[index];
    }
    return regCP;
}

//
// Read a function argument using the standard ABI
//
static void getArg(
    vmiProcessorP processor,
    vmiosObjectP  object,
    Uns32         index,
    void         *result
) {
    if(index>=REG_ARG_NUM) {
        vmiMessage("P", CPU_PREFIX"_ANS",
            "No more than %u function arguments supported",
            REG_ARG_NUM
        );
        vmirtFinish(-1);
    } else {
        vmiRegInfoCP regCP = getArgCP(processor, object, index);
        vmiosRegRead(processor, regCP, result);
    }
}


////////////////////////////////////////////////////////////////////////////////
// Include code common to all newlib implementations
////////////////////////////////////////////////////////////////////////////////
#include "ovpworld.org/modelSupport/semihosting/1.0/model/newlib.c.h"

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR & DESTRUCTOR
////////////////////////////////////////////////////////////////////////////////

//
// Constructor
//
static VMIOS_CONSTRUCTOR_FN(constructor) {

    // first three argument registers (standard ABI)
    object->args[0] = vmiosGetRegDesc(processor, "R5");
    object->args[1] = vmiosGetRegDesc(processor, "R6");
    object->args[2] = vmiosGetRegDesc(processor, "R7");
    object->args[3] = vmiosGetRegDesc(processor, "R8");
    object->args[4] = vmiosGetRegDesc(processor, "R9");
    object->args[5] = vmiosGetRegDesc(processor, "R10");

    // return register (standard ABI)
    object->resultReg = vmiosGetRegDesc(processor, "R3");

    // Stack Pointer
    object->argSP = vmiosGetRegDesc(processor, "R1");
    Bool found;
    object->argv = (char **)(UnsPS)vmirtPlatformUns64Attribute(processor, "userargv", &found);
    object->envp = (char **)(UnsPS)vmirtPlatformUns64Attribute(processor, "userenvp", &found);

    // Sets up data common to all newlib implementations
    // Defined in newlibSemiHost.c.h
    setupNewlib(object, processor);

}

//
// Destructor
//
static VMIOS_DESTRUCTOR_FN(destructor) {
}


////////////////////////////////////////////////////////////////////////////////
// INTERCEPT ATTRIBUTES
////////////////////////////////////////////////////////////////////////////////

vmiosAttr modelAttrs = {

    ////////////////////////////////////////////////////////////////////////
    // VERSION
    ////////////////////////////////////////////////////////////////////////

    .versionString  = VMI_VERSION,            // version string (THIS MUST BE FIRST)
    .modelType      = VMI_INTERCEPT_LIBRARY,  // type
    .packageName    = "Newlib",               // description
    .objectSize     = sizeof(vmiosObject),    // size in bytes of OSS object

    ////////////////////////////////////////////////////////////////////////
    // CONSTRUCTOR/DESTRUCTOR ROUTINES
    ////////////////////////////////////////////////////////////////////////

    .constructorCB  = constructor,            // object constructor
    .destructorCB   = destructor,             // object destructor

    ////////////////////////////////////////////////////////////////////////
    // INSTRUCTION INTERCEPT ROUTINES
    ////////////////////////////////////////////////////////////////////////

    .morphCB        = 0,                      // morph callback
    .nextPCCB       = 0,                      // get next instruction address
    .disCB          = 0,                      // disassemble instruction

    ////////////////////////////////////////////////////////////////////////
    // ADDRESS INTERCEPT DEFINITIONS
    ////////////////////////////////////////////////////////////////////////

    .intercepts = {
        // ----------------- ------- ------ -----------------
        // Name              Address Opaque Callback
        // ----------------- ------- ------ -----------------
        //
        // newlib entries
        //
        { "exit",           0,      True,  exitCB         },
        { "open",           0,      True,  openCB         },
        { "close",          0,      True,  closeCB        },
        { "read",           0,      True,  readCB         },
        { "write",          0,      True,  writeCB        },
        { "lseek",          0,      True,  lseekCB        },
        { "fstat",          0,      True,  fstatCB        },
        { "lstat",          0,      True,  lstatCB        },
        { "stat",           0,      True,  statCB         },
        { "kill",           0,      True,  killCB         },
        { "gettimeofday",   0,      True,  gettimeofdayCB },
        { "times",          0,      True,  timesCB        },
        { "unlink",         0,      True,  unlinkCB       },
        { "time",           0,      True,  timeCB         },

        //
        // Intercept main for argc, argv, env
        //
        { "main",           0,      False, mainStackInitCB},

        { 0 },
    }
};

