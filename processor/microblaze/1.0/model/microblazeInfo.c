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
#include "vmi/vmiModelInfo.h"
#include "hostapi/impAlloc.h"

#include "microblazeFunctions.h"
#include "microblazeTypeRefs.h"
#include "microblazeStructure.h"

#define VENDOR      "xilinx.ovpworld.org"
#define MDL_LIBRARY "processor"
#define MDL_NAME    "microblaze"
#define SHL_LIBRARY "semihosting"
#define SHL_NAME    "microblazeNewlib"
#define VERSION     "1.0"

VMI_PROC_INFO_FN(microblazeProcInfo) {

    static const Uns32 alternativeElfCodes[] = {
        47787,  // gcc Linux compiler
        0
    };

    static const vmiProcessorInfo info = {
        .vlnv.vendor         = VENDOR,
        .vlnv.library        = MDL_LIBRARY,
        .vlnv.name           = MDL_NAME,
        .vlnv.version        = VERSION,

        .semihost.vendor     = VENDOR,
        .semihost.library    = SHL_LIBRARY,
        .semihost.name       = SHL_NAME,
        .semihost.version    = VERSION,

        .helper.vendor       = "imperas.com",
        .helper.library      = "intercept",
        .helper.name         = "microblazeCpuHelper",
        .helper.version      = "1.0",

        .elfCode             = 189, // gcc bare metal compiler
        .alternativeElfCodes = alternativeElfCodes,
        .endianFixed         = False,
       //info.endian         = MEM_ENDIAN_BIG,
        .gdbPath             = "$IMPERAS_HOME/lib/$IMPERAS_ARCH/gdb/microblaze-xilinx-elf-gdb" VMI_EXE_SUFFIX,
        .family              = "xilinx_microblaze",
        .defaultModel        = True,
        .QLQualified         = True,
    };

    return &info;
}

