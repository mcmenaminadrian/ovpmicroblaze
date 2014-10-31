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

#ifndef MICROBLAZE_TLB_H
#define MICROBLAZE_TLB_H

#include "vmi/vmiTypes.h"
#include "microblazeStructure.h"

#define MIN_PAGE_SIZE (1<<10)

//
// Determine if the access using the passed virtual address misses the TLB,
// updating the processor state to take an appropriate exception if so.
//
Bool microblazeTLBMiss(
    microblazeP         microblaze,
    microblazeTLBAction action,
    Uns32               va,
    memAccessAttrs      attrs
);

void microblazeConstructMMU(microblazeP microblaze);
void microblazeDestructMMU(microblazeP microblaze);
void microblazePrintTLB(microblazeP microblaze, Bool onlyValid);
void microblazeWriteTLBLO(microblazeP microblaze);
void microblazeWriteTLBHI(microblazeP microblaze);
void microblazeReadTLBLO(microblazeP microblaze);
void microblazeReadTLBHI(microblazeP microblaze);
void microblazeSearchTLB(microblazeP microblaze);
void microblazeTLBRefreshZPR (microblazeP microblaze);

#endif // MICROBLAZE_TLB_H
