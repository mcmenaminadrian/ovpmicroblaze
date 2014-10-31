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

#ifndef MICROBLAZE_USER_H
#define MICROBLAZE_USER_H

#include "hostapi/impTypes.h"
#include "microblazeStructure.h"

//
// Routines that must be called for registers that require updating before read/write
//
void microblazeFillMSR(microblazeP);
void microblazeEmptyMSR(microblazeP);

void microblazeUpdateBlockMask(microblazeP);

void microblazeFillPC(microblazeP);

void microblazeFillFSR(microblazeP);
void microblazeEmptyFSR(microblazeP);

//
// Fill/Empty all registers that require it
//
void microblazeFillRegisters(microblazeP);
void microblazeEmptyRegisters(microblazeP);

#endif // MICROBLAZE_USER_H
