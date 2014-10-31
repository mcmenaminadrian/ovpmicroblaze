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

#include "microblazeConfig.h"
#include "microblazeVariant.h"

const struct microblazeConfigS microblazeConfigTable[] = {
    ////////////////////////////////////////////////////////////////////////////
    // ISA CONFIGURATIONS
    ////////////////////////////////////////////////////////////////////////////
    {.name = "V7_00", .arch = V7_00},
    {.name = "V7_10", .arch = V7_10},
    {.name = "V7_20", .arch = V7_20},
    {.name = "V7_30", .arch = V7_30},
    {.name = "V8_00", .arch = V8_00},
    {.name = "V8_10", .arch = V8_10},
    {.name = "V8_20", .arch = V8_20},
    {.name = "ISA",   .arch = MICROBLAZE_ISA_ALL},

    {0} // null terminator
};

