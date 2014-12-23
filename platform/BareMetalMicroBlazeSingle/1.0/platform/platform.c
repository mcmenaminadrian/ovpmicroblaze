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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "icm/icmCpuManager.h"

// Function Prototypes early declaration
static void parseArgs(int argc, char **argv);

// valid command line
const char *usage = "<application name> [GDB port]";
// Variables set by arguments
char  *application;                    // the application to load
Bool  enableDebug     = False;         // set True when debugging selected
Bool  autoGDBConsole  = False;         // set True when auto start of GDB console selected
Uns32 portNum         = 0;             // set to a port number for a debug connection

icmProcessorP createPlatform(void) {

////////////////////////////////////////////////////////////////////////////////

    // initialize OVPsim, enabling verbose mode to get statistics at end of execution
    // and
    // Imperas Intercepts to utilise specific builtin simulator functions
    unsigned int icmAttrs = ICM_VERBOSE | ICM_STOP_ON_CTRLC;

    if(autoGDBConsole) {
        icmAttrs |= ICM_GDB_CONSOLE;
    }

    icmInitPlatform(ICM_VERSION, icmAttrs, enableDebug ? "rsp" : 0, portNum, "platform");

////////////////////////////////////////////////////////////////////////////////
//                                 Bus bus1_b                                 //
////////////////////////////////////////////////////////////////////////////////

    icmBusP bus1_b = icmNewBus( "bus1_b", 32);


////////////////////////////////////////////////////////////////////////////////
//                               Processor cpu1                               //
////////////////////////////////////////////////////////////////////////////////
#define SIM_ATTRS (ICM_ATTR_DEFAULT|ICM_ATTR_SIMEX)

    icmAttrListP cpu1_attr = icmNewAttrList();

    icmAddStringAttr(cpu1_attr, "endian", "big");
    icmAddDoubleAttr(cpu1_attr, "mips", 100.000000);
    icmAddStringAttr(cpu1_attr, "variant", "V8_20");
    icmAddBoolAttr(cpu1_attr, "verbose", "true");
    icmAddUns32Attr(cpu1_attr, "C_PVR", 2);
    icmAddUns32Attr(cpu1_attr, "C_USE_MMU", 3);
    icmAddStringAttr(cpu1_attr, "C_USE_BARREL", "1");
    icmAddStringAttr(cpu1_attr, "C_USE_DIV", "1");
    icmAddUns32Attr(cpu1_attr, "C_USE_INTERRUPT", 1);
    icmAddUns32Attr(cpu1_attr, "C_MMU_TLB_ACCESS", 3);
    icmAddUns32Attr(cpu1_attr, "C_UNALIGNED_EXCEPTIONS", 1);
    icmAddUns32Attr(cpu1_attr, "C_ILL_OPCODE_EXCEPTION", 1);
    icmAddUns32Attr(cpu1_attr, "C_DIV_ZERO_EXCEPTION", 1);
    icmAddUns32Attr(cpu1_attr, "C_OPCODE_0x0_ILLEGAL", 1);
    icmAddUns32Attr(cpu1_attr, "C_DEBUG_ENABLED", 1);
    icmAddUns32Attr(cpu1_attr, "C_D_LMB", 1);
    icmAddUns32Attr(cpu1_attr, "C_I_LMB", 1);
    icmAddUns32Attr(cpu1_attr, "C_MMU_DTLB_SIZE", 4);
    icmAddUns32Attr(cpu1_attr, "C_MMU_ITLB_SIZE", 2);
    icmAddUns32Attr(cpu1_attr, "C_MMU_ZONES", 16);
    icmAddUns32Attr(cpu1_attr, "C_USE_STACK_PROTECTION", 1);


 

    const char *microblazeModel    = icmGetVlnvString(NULL, "xilinx.ovpworld.org", "processor",  "microblaze",        "1.0", "model");
    const char *microblazeSemihost = icmGetVlnvString(NULL, "xilinx.ovpworld.org", "semihosting", "microblazeNewlib", "1.0", "model");

    // Create the processor instances
    icmProcessorP cpu1_c = icmNewProcessor(
        "cpu0",             // CPU name
        "microblaze",       // CPU type
        0,                  // CPU cpuId
        0,                  // CPU model flags
        32,                 // address bits
        microblazeModel,    // model file
        "modelAttrs",       // morpher attributes
        SIM_ATTRS,          // attributes
        cpu1_attr,          // user-defined attributes
	//0,		    // really bare metal
       microblazeSemihost, // semi-hosting file
        "modelAttrs"        // semi-hosting attributes
    );

    icmConnectProcessorBusses( cpu1_c, bus1_b, bus1_b );


////////////////////////////////////////////////////////////////////////////////
//                                Memory memory                               //
////////////////////////////////////////////////////////////////////////////////

    icmMemoryP memory_m = icmNewMemory("memory_m", 0x7, 0xffffffff);

    icmConnectMemoryToBus( bus1_b, "sp1", memory_m, 0x0);

    return cpu1_c;
}

////////////////////////////////////////////////////////////////////////////////
//                                   M A I N                                  //
////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {

    parseArgs(argc, argv);

    // the constructor
    icmProcessorP processor = createPlatform();
    //debug
    icmDebugThisProcessor(processor);

    // load the application executable file into processor memory space
    if(!icmLoadProcessorMemory(processor, application, ICM_LOAD_PHYSICAL, True, True)) {
        return -1;
    }

    // run till the end
    icmSimulatePlatform();

    // finish and clean up
    icmTerminate();

    return 0;
}

////////////////////////////////////////////////////////////////////////////////

//
// Parse the argument list and set variables
//
static void parseArgs(int argc, char **argv)
{
    // check for the application program name argument
    if((argc<2) || (argc>3)) {
        icmPrintf(
           "usage: %s %s\n\n",
            argv[0], usage
        );
        exit(1);
    }
    application = argv[1];

    // check for debugging requirement
    if(argc==3){
        enableDebug = False;
        if (!strcmp(argv[2], "autoGDB"))
            autoGDBConsole = True;
        else
            sscanf(argv[2], "%d", &portNum);
    }
}
