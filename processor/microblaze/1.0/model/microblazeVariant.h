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

#ifndef MICROBLAZE_VARIANT_H
#define MICROBLAZE_VARIANT_H

typedef enum microblazeArchitectureE {
    //
    // Base Enumerated Architecture types
    //
    V7_00 = 0x00000001,

    //
    // Compound Enumerated Architecture types
    //
    V7_10 = 0x00000002 | V7_00,
    V7_20 = 0x00000004 | V7_00 | V7_10,
    V7_30 = 0x00000008 | V7_00 | V7_10 | V7_20,
    V8_00 = 0x00000010 | V7_00 | V7_10 | V7_20 | V7_30,
    V8_10 = 0x00000020 | V7_00 | V7_10 | V7_20 | V7_30 | V8_00,
    V8_20 = 0x00000040 | V7_00 | V7_10 | V7_20 | V7_30 | V8_00 | V8_10,

    //
    // catch ALL Enumerated Architecture type
    //
    MICROBLAZE_ISA_ALL = 0xffffffff
} microblazeArchitecture;

#endif // MICROBLAZE_VARIANT_H
