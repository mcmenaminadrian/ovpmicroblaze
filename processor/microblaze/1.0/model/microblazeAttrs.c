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
#include "microblazeStructure.h"
#include "microblazeFunctions.h"

static const char *dictNames[] = {"REAL", "VIRTUAL PRIV", "VIRTUAL USER", 0};

const vmiIASAttr modelAttrs = {
    ////////////////////////////////////////////////////////////////////////
    // VERSION & SIZE ATTRIBUTES
    ////////////////////////////////////////////////////////////////////////

    .versionString       = VMI_VERSION,         // version string (THIS MUST BE 1ST)
    .modelType           = VMI_PROCESSOR_MODEL, // model type (THIS MUST BE 2ND)
    .dictNames           = dictNames,           // null-terminated dictionary name list
    .cpuSize             = sizeof(microblaze),    // full size of CPU object

    ////////////////////////////////////////////////////////////////////////
    // SAVE/RESTORE ROUTINES
    ////////////////////////////////////////////////////////////////////////

    .saveCB             = microblazeSaveStateCB,
    .restoreCB          = microblazeRestoreStateCB,

    ////////////////////////////////////////////////////////////////////////
    // CONSTRUCTOR/DESTRUCTOR ROUTINES
    ////////////////////////////////////////////////////////////////////////

    .constructorCB      = microblazeConstructor, // constructor
    .vmInitCB           = microblazeVMInit, // virtual memory initialization
    .destructorCB       = microblazeDestructor, // destructor

    ////////////////////////////////////////////////////////////////////////
    // MORPHER CORE ROUTINES
    ////////////////////////////////////////////////////////////////////////

    .morphCB            = microblazeMorph,// morph callback
    .fetchSnapCB        = microblazeFetchSnap, // fetch address snap callback

    ////////////////////////////////////////////////////////////////////////
    // SIMULATION SUPPORT ROUTINES
    ////////////////////////////////////////////////////////////////////////

    .getEndianCB        = microblazeGetEndian, // get processor endianness
    .nextPCCB           = microblazenextPC, // get next instruction address
    .disCB              = microblazeDis, // disassemble instruction
    .switchCB           = microblazeSwitch, // call on simulator context switch

    ////////////////////////////////////////////////////////////////////////
    // EXCEPTION ROUTINES
    ////////////////////////////////////////////////////////////////////////

    .rdPrivExceptCB     = microblazeRdPrivExcept, // read privilege exception
    .wrPrivExceptCB     = microblazeWrPrivExcept, // write privilege exception
    .rdAlignExceptCB    = microblazeRdAlignExcept, // read alignment exception
    .wrAlignExceptCB    = microblazeWrAlignExcept, // write alignment exception
    .rdAbortExceptCB    = microblazeRdAbortExcept, // read abort exception
    .wrAbortExceptCB    = microblazeWrAbortExcept, // write abort exception
    .ifetchExceptCB     = microblazeIfetchExcept, // execution privilege exception
    .arithExceptCB      = microblazeArithExcept, // arithmetic exception
    .icountExceptCB     = microblazeIcountExcept, // instruction count exception

    ////////////////////////////////////////////////////////////////////////
    // PARAMETER SUPPORT ROUTINES
    ////////////////////////////////////////////////////////////////////////

    .paramSpecsCB     = microblazeParamSpecs,
    .paramValueSizeCB = microblazeParamValueSize,

    ////////////////////////////////////////////////////////////////////////
    // PORT ROUTINES
    ////////////////////////////////////////////////////////////////////////

    .busPortSpecsCB   = microblazeBusPortSpecs,
    .netPortSpecsCB   = microblazeNetPortSpecs,
    .fifoPortSpecsCB  = microblazeFifoPortSpecs,

    ////////////////////////////////////////////////////////////////////////
    // DEBUGGER INTEGRATION SUPPORT ROUTINES
    ////////////////////////////////////////////////////////////////////////

    .regGroupCB         = microblazeRegGroup, // regGroup callback
    .regInfoCB          = microblazeRegInfo, // regInfo callback
    .exceptionInfoCB    = microblazeExceptionInfo, // exceptionInfo callback
    .modeInfoCB         = microblazeModeInfo, // modeInfo callback
    .getExceptionCB     = microblazeGetException, // return current processor exception
    .getModeCB          = microblazeGetMode, // return current processor mode
    .regReadCB          = 0, // default regRead callback
    .regWriteCB         = 0, // default regWrite callback
    .debugCB            = microblazeDebug, // user-defined trace callback

    ////////////////////////////////////////////////////////////////////////
    // IMPERAS INTERCEPTED FUNCTION SUPPORT ROUTINES
    ////////////////////////////////////////////////////////////////////////

    .intReturnCB        = microblazeIntReturn, // morph return from opaque intercept
    .intResultCB        = microblazeIntResult, // set intercepted function result
    .intParCB           = microblazeIntPar,    // push function arguments

    ////////////////////////////////////////////////////////////////////////
    // PROCESSOR INFO ROUTINE
    ////////////////////////////////////////////////////////////////////////

    .procInfoCB  = microblazeProcInfo

};
