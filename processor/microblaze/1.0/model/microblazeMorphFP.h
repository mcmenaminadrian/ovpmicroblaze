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

#ifndef MICROBLAZE_MORPH_FP_H
#define MICROBLAZE_MORPH_FP_H

// VMI header files
#include "vmi/vmiTypes.h"
#include "vmi/vmiDbg.h"

//
// Set simulator floating point control word
//
void microblazeUpdateFPControlWord(microblazeP microblaze);

//
// Configure FPU
//
void microblazeConfigureFPU(microblazeP microblaze);

//
// Floating point masks
//
#define MICROBLAZE_PLUS_ZERO          0x00000000
#define MICROBLAZE_MINUS_ZERO         0x80000000
#define MICROBLAZE_FIXED_NAN_32       0xffc00000
// Value for Quiet NaN is a guess as this is not documented!
#define MICROBLAZE_QUIET_NAN_32       0x7fc00000
#define MICROBLAZE_INDETERMINATE_32   0x7fffffff
#define MICROBLAZE_SNAN_32            0x00400000
#define MICROBLAZE_SIGN_32            0x80000000
#define MICROBLAZE_EXP_32             0x7f800000
#define MICROBLAZE_FRACT_32           0x007fffff

#endif

