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

// Imperas header files
#include "hostapi/impAlloc.h"
#include "vmi/vmiMessage.h"
#include "vmi/vmiRt.h"

// Model header files
#include "microblazeStructure.h"
#include "microblazeFunctions.h"
#include "microblazeExceptions.h"
#include "microblazePorts.h"

#define NUM_MEMBERS(_A) (sizeof(_A)/sizeof((_A)[0]))

static Bool posedge(Uns32 old, Uns32 new) {
    if (old == 0 && new == 1) {
        return True;
    }
    return False;
}

static Bool negedge(Uns32 old, Uns32 new) {
    if (old == 1 && new == 0) {
        return True;
    }
    return False;
}

static VMI_NET_CHANGE_FN(signalReset) {

    microblazeP microblaze = (microblazeP)processor;

    //
    // If we have a rising edge, and we are edge triggered and
    //
    if (posedge(microblaze->Reset, newValue)) {
        microblaze->ResetPending = True;
        // halt the processor while reset is active
        vmirtHalt((vmiProcessorP)microblaze);

    } else {

        microblaze->ResetPending = False;

        // reset the processor
        vmirtRestartNext((vmiProcessorP)microblaze);

        microblazeProcessReset(processor);

        microblazeDoException(microblaze);

    }

    microblaze->Reset = newValue;
}

static VMI_NET_CHANGE_FN(signalInterrupt) {

    microblazeP microblaze = (microblazeP)processor;

    if (posedge(microblaze->Interrupt, newValue)) {
        microblaze->InterruptPos = True;
    }
    if (negedge(microblaze->Interrupt, newValue)) {
        microblaze->InterruptNeg = True;
    }

    microblaze->Interrupt = newValue;

    // process the interrupt
    microblazeCheckExceptions(microblaze);
}

typedef struct microblazeNetPortS {
    vmiNetPort desc;
    Uns32      variantMask;
} microblazeNetPort;

void microblazeNewNetPorts(microblazeP microblaze) {
    microblazeNetPort template[] = {
        { {"Interrupt",  vmi_NP_INPUT, (void*)0, signalInterrupt },  V7_00 },
        { {"Reset",      vmi_NP_INPUT, (void*)0, signalReset     },  V7_00 },
        { {"MB_Reset",   vmi_NP_INPUT, (void*)0, 0               },  V7_00 },
        { {"Ext_BRK",    vmi_NP_INPUT, (void*)0, 0               },  V7_00 },
        { {"Ext_NM_BRK", vmi_NP_INPUT, (void*)0, 0               },  V7_00 },
        { { 0 } }
    };

    // count members
    Uns32 numMembers = NUM_MEMBERS(template);
    Uns32 i;

    // allocate permanent port structure (including terminator)
    microblazeNetPortP result = STYPE_CALLOC_N(microblazeNetPort, numMembers);

    // fill permanent port structure
    for(i=0; i<numMembers; i++) {
        result[i] = template[i];
    }

    // save ports on processor
    microblaze->netPorts = result;
}

//
// Free ports
//
void microblazeFreeNetPorts(microblazeP microblaze) {
    if(microblaze->netPorts) {
        STYPE_FREE(microblaze->netPorts);
    }
}

//
// Skip port descriptions not used on this variant
//
static void skipUnusedPorts(microblazeNetPortPP p, Uns32 variant) {
    while(*p && (*p)->desc.name && !((*p)->variantMask & variant)) {
        (*p)++;
    }
}

//
// Get the next net port
//
VMI_NET_PORT_SPECS_FN(microblazeNetPortSpecs) {
    microblazeP microblaze = (microblazeP)processor;
    microblazeNetPortP c = (microblazeNetPortP)prev;
    Uns32 variant = microblaze->configInfo.arch;

    if(!c) {
        c = microblaze->netPorts;
    } else {
        c++;
    }
    skipUnusedPorts(&c, variant);

    return (vmiNetPortP)(c && (c->desc.name) ? c : 0);
}
