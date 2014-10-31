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

#include "vmi/vmiCxt.h"
#include "vmi/vmiDecode.h"
#include "vmi/vmiMessage.h"
#include "microblazeDecode.h"
#include "microblazeDecodeUser.h"
#include "microblazeInstructions.h"

//
// Return the size of the instruction at the passed address in bytes
//
Uns32 microblazeGetInstructionBytes(
        microblazeP microblaze,
        microblazeAddr thisPC
) {
    vmiProcessorP processor = (vmiProcessorP)microblaze;

    //
    // Initialize the 32 bit decoder table
    //
    static vmidDecodeTableP decodeTable32;
    if(!decodeTable32) {
        decodeTable32 = microblazeCreateDecodeTable32();
    }

    Uns64 instr4 = vmicxtFetch4Byte(processor, thisPC);
    Uns32 type = MICROBLAZE_IT_LAST;
    type = vmidDecode(decodeTable32, instr4);

    if (type == MICROBLAZE_IT_IMM_TYPEI1) {
        return 8;
    } else {
        return 4;
    }
}

//
// Decoder function for a given address
//
void microblazeDecode(
    microblazeP microblaze,
    microblazeAddr thisPC,
    microblazeInstructionInfoP info
) {
    //
    // Initialize the 32 bit decoder table
    //
    static vmidDecodeTableP decodeTable32;
    if(!decodeTable32) {
        decodeTable32 = microblazeCreateDecodeTable32();
    }

    vmiProcessorP processor = (vmiProcessorP)microblaze;

    //
    // Initialize the following info fields before lookup
    //
    info->type = MICROBLAZE_IT_INSTR32;
    info->thisPC = thisPC;
    info->arch = microblaze->configInfo.arch;

    Uns64 instruction = 0;
    info->instrsize = 0;
    //
    // Attempt to match a 32 bit instruction
    //
    if (info->type == MICROBLAZE_IT_INSTR32) {
        Uns64 instr4 = vmicxtFetch4Byte(processor, thisPC);
        info->type = MICROBLAZE_IT_LAST;
        info->type = vmidDecode(decodeTable32, instr4);
        info->nextPC = info->thisPC + 4;
        instruction = instr4;
        info->instrsize = 4;
        info->instruction = instr4;
    }

    //
    // Fatal if there was a match failure
    //
    if (info->type == MICROBLAZE_IT_LAST && MICROBLAZE_DISASSEMBLE(microblaze)) {
        vmiMessage("F", "DECODE_FAIL", "Address=0x" FMT_640Nx " Instruction = 0x" FMT_640Nx, (Uns64)thisPC, instruction);
    }

    microblazeGenInstructionInfo(info, instruction);

    //
    // If this is a decode of an IMM instruction, we decode the next instruction
    //
    if (info->type == MICROBLAZE_IT_IMM_TYPEI1) {
        Uns64 instr4 = vmicxtFetch4Byte(processor, info->nextPC);
        info->type = MICROBLAZE_IT_LAST;
        info->type = vmidDecode(decodeTable32, instr4);
        info->thisPC = info->nextPC;
        info->nextPC = info->thisPC + 4;
        instruction = instr4;
        info->instrsize = 8;
        info->instruction = instr4;

        //
        // Fatal if there was a match failure
        //
        if (info->type == MICROBLAZE_IT_LAST && MICROBLAZE_DISASSEMBLE(microblaze)) {
            vmiMessage("F", "DECODE_FAIL", "Address=0x" FMT_640Nx " Instruction = 0x" FMT_640Nx, (Uns64)info->nextPC, instruction);
        }

        //
        // get the fields for the TYPEB instruction
        //
        microblazeGenInstructionInfo(info, instruction);
    } else {
        // Indicate that the simmhi is unset
        info->simmhi = 0x0000ffff;
    }
}
