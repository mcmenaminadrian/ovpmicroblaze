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

#ifndef MICROBLAZE_EXCEPTIONS_H
#define MICROBLAZE_EXCEPTIONS_H
#include "vmi/vmiAttrs.h"
#include "microblazeStructure.h"

//
// Take an MMU storage exception
//
void microblazeMMUStorageException(
    microblazeP         microblaze,
    microblazeTLBAction action,
    Uns32               va,
    Bool                DIZ
);

void microblazeTLBMissException(
    microblazeP         microblaze,
    microblazeTLBAction action,
    Uns32               va
);

void microblazeStreamExceptionCB(vmiProcessorP processor, Uns32 insn, Uns32 e, Uns32 d, Uns32 fsl, Uns32 rB, Uns32 rD);
void microblazehwExceptionCB(vmiProcessorP processor, TYPE_SPR_ESR ESR);
void microblazeInstallPorts(microblazeP microblaze);
void microblazeRegisterInit (microblazeP microblaze);
void microblazeMaskEnable(Uns32 *mask, Bool Enable, Uns32 left, Uns32 right);
void microblazeCheckExceptions(microblazeP microblaze);
void microblazeProcessReset(vmiProcessorP processor);
void microblazeProcessInterrupt(vmiProcessorP processor);
void microblazeDoException(microblazeP microblaze);

#endif // MICROBLAZE_EXCEPTIONS_H
