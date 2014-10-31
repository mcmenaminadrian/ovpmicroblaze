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


// VMI header files
#include "vmi/vmiAttrs.h"
#include "vmi/vmiModelInfo.h"
#include "vmi/vmiDoc.h"

#include "microblazeDoc.h"
#include "microblazeParameters.h"
#include "microblazeStructure.h"
#include "microblazeConfig.h"

void microblazeDoc(vmiProcessorP processor, microblazeParamValuesP parameters) {
    vmiDocNodeP root = vmidocAddSection(0, "Root");
    vmiDocNodeP desc = vmidocAddSection(root, "Description");
    vmidocAddText(desc, "Microblaze Processor Model");

    vmiDocNodeP lic  = vmidocAddSection(root, "Licensing");
    vmidocAddText(lic,  "Apache 2.0 Open Source License");

    vmiDocNodeP Features  = vmidocAddSection(root, "Features");
    vmidocAddText(Features, "Instruction Set: This model fully implements the instruction set upto and including V8.2.");
    vmidocAddText(Features, "Privileged Instructions: Implemented");
    vmidocAddText(Features, "Virtual-Memory Management: Implemented");
    vmidocAddText(Features, "Reset, Interrupts, Exceptions and Break: Implemented");
    vmidocAddText(Features, "Floating Point Unit: Implemented");
    vmidocAddText(Features, "Stream Link Interface: Implemented");

    vmiDocNodeP lim  = vmidocAddSection(root, "Limitations");
    vmidocAddText(lim, "No known limitations");

    vmiDocNodeP cfgfeatures = vmidocAddSection(root, "ConfigurationFeatures");

    microblazeConfigCP     match = &microblazeConfigTable[parameters->variant];
    microblazeArchitecture arch  = match->arch;
    if (arch & V7_00) {
        vmidocAddText(cfgfeatures, "Barrel Shifter.");
        vmidocAddText(cfgfeatures, "Hardware Divider.");
        vmidocAddText(cfgfeatures, "Machine Status Set/Clear Instructions.");
        vmidocAddText(cfgfeatures, "Hardware Exceptions.");
        vmidocAddText(cfgfeatures, "Pattern Compare Instructions.");
        vmidocAddText(cfgfeatures, "Floating Point Unit (FPU).");
        vmidocAddText(cfgfeatures, "Disable Hardware Multiplier.");
        vmidocAddText(cfgfeatures, "Processor Version Register (PVR).");
        vmidocAddText(cfgfeatures, "Hardware Multiplier 64-bit Result.");
        vmidocAddText(cfgfeatures, "Floating Point Conversion and Square Root Instructions.");
        vmidocAddText(cfgfeatures, "Memory Management Unit.");
        vmidocAddText(cfgfeatures, "Extended Stream Instructions .");
    }
    if (arch >= V8_00) {
        vmidocAddText(cfgfeatures, "Count Leading Zeros Instruction.");
        vmidocAddText(cfgfeatures, "Memory Barrier Instruction.");
        //vmidocAddText(cfgfeatures, "Stack Overflow and Underflow Detection.");
        vmidocAddText(cfgfeatures, "Allow Stream Instructions in User Mode.");
    }

    vmiDocNodeP verif = vmidocAddSection(root, "Verification");
    vmidocAddText(verif, "Models have been validated correct by "
                         "running through extensive tests using "
                         "test suites and technology provided by Xilinx");

    vmidocProcessor(processor, root);
}
