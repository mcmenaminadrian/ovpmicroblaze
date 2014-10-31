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


#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "icm/icmCpuManager.h"

static    Uns32 uartPort          = 0;
static    Bool  connectUartPort   = False;
static    Bool  autoConsole       = False;
static    float stopafter         = 0.0;

void createPlatform(char *kernelFile) {

////////////////////////////////////////////////////////////////////////////////

    icmInitPlatform(ICM_VERSION, ICM_VERBOSE|ICM_STOP_ON_CTRLC, 0, 0, "XilinxML505");

////////////////////////////////////////////////////////////////////////////////
//                                 Bus bus1_b
////////////////////////////////////////////////////////////////////////////////

    icmBusP bus1_b = icmNewBus( "bus1_b", 32);


////////////////////////////////////////////////////////////////////////////////
//                           Processor microblaze_0
////////////////////////////////////////////////////////////////////////////////

    const char *microblaze_0_path = icmGetVlnvString(
        0,    // path (0 if from the product directory)
        0,    // vendor
        0,    // library
        "microblaze",    // name
        0,    // version
        "model"     // model
    );

    icmAttrListP microblaze_0_attr = icmNewAttrList();

    icmAddUns64Attr(microblaze_0_attr, "C_INTERCONNECT", 1);
    icmAddUns64Attr(microblaze_0_attr, "C_PVR", 2);
    icmAddUns64Attr(microblaze_0_attr, "C_D_LMB", 1);
    icmAddUns64Attr(microblaze_0_attr, "C_I_LMB", 1);
    icmAddUns64Attr(microblaze_0_attr, "C_USE_BARREL", 1);
    icmAddUns64Attr(microblaze_0_attr, "C_USE_DIV", 1);
    icmAddUns64Attr(microblaze_0_attr, "C_USE_HW_MUL", 2);
    icmAddUns64Attr(microblaze_0_attr, "C_USE_FPU", 2);
    icmAddUns64Attr(microblaze_0_attr, "C_USE_MSR_INSTR", 1);
    icmAddUns64Attr(microblaze_0_attr, "C_USE_PCMP_INSTR", 1);
    icmAddUns64Attr(microblaze_0_attr, "C_UNALIGNED_EXCEPTIONS", 1);
    icmAddUns64Attr(microblaze_0_attr, "C_ILL_OPCODE_EXCEPTION", 1);
    icmAddUns64Attr(microblaze_0_attr, "C_DIV_ZERO_EXCEPTION", 1);
    icmAddUns64Attr(microblaze_0_attr, "C_FPU_EXCEPTION", 1);
    icmAddUns64Attr(microblaze_0_attr, "C_OPCODE_0x0_ILLEGAL", 1);
    icmAddUns64Attr(microblaze_0_attr, "C_FSL_EXCEPTION", 1);
    icmAddUns64Attr(microblaze_0_attr, "C_DEBUG_ENABLED", 1);
    icmAddUns64Attr(microblaze_0_attr, "C_FSL_LINKS", 4);
    icmAddUns64Attr(microblaze_0_attr, "C_USE_EXTENDED_FSL_INSTR", 1);
    icmAddUns64Attr(microblaze_0_attr, "C_USE_MMU", 3);
    icmAddUns64Attr(microblaze_0_attr, "C_MMU_DTLB_SIZE", 4);
    icmAddUns64Attr(microblaze_0_attr, "C_MMU_ITLB_SIZE", 2);
    icmAddUns64Attr(microblaze_0_attr, "C_MMU_TLB_ACCESS", 3);
    icmAddUns64Attr(microblaze_0_attr, "C_MMU_ZONES", 16);
    icmAddDoubleAttr(microblaze_0_attr, "mips", 125.000000);
    icmAddStringAttr(microblaze_0_attr, "endian", "big");

    icmProcessorP microblaze_0_c = icmNewProcessor(
        "microblaze_0",   // name
        "microblaze",   // type
        0,   // cpuId
        0x0, // flags
         32,   // address bits
        microblaze_0_path,   // model
        "modelAttrs",   // symbol
        0x20,   // procAttrs
        microblaze_0_attr,   // attrlist
        0,   // semihost file
        0    // semihost symbol
    );

    icmConnectProcessorBusses( microblaze_0_c, bus1_b, bus1_b );


////////////////////////////////////////////////////////////////////////////////
//                                PSE LEDs_8Bit
////////////////////////////////////////////////////////////////////////////////

    const char *LEDs_8Bit_path = icmGetVlnvString(
        0,    // path (0 if from the product directory)
        0,    // vendor
        0,    // library
        "xps-gpio",    // name
        0,    // version
        "pse"     // model
    );

    icmAttrListP LEDs_8Bit_attr = icmNewAttrList();


    icmPseP LEDs_8Bit_p = icmNewPSE(
        "LEDs_8Bit",   // name
        LEDs_8Bit_path,   // model
        LEDs_8Bit_attr,   // attrlist
        0,   // semihost file
        0    // semihost symbol
    );

    icmConnectPSEBus( LEDs_8Bit_p, bus1_b, "plb", 0, 0x81400000, 0x8140ffff);


////////////////////////////////////////////////////////////////////////////////
//                               PSE IIC_EEPROM
////////////////////////////////////////////////////////////////////////////////

    const char *IIC_EEPROM_path = icmGetVlnvString(
        0,    // path (0 if from the product directory)
        0,    // vendor
        0,    // library
        "xps-iic",    // name
        0,    // version
        "pse"     // model
    );

    icmAttrListP IIC_EEPROM_attr = icmNewAttrList();


    icmPseP IIC_EEPROM_p = icmNewPSE(
        "IIC_EEPROM",   // name
        IIC_EEPROM_path,   // model
        IIC_EEPROM_attr,   // attrlist
        0,   // semihost file
        0    // semihost symbol
    );

    icmConnectPSEBus( IIC_EEPROM_p, bus1_b, "plb", 0, 0x81600000, 0x8160ffff);


////////////////////////////////////////////////////////////////////////////////
//                               PSE xps_intc_0
////////////////////////////////////////////////////////////////////////////////

    const char *xps_intc_0_path = icmGetVlnvString(
        0,    // path (0 if from the product directory)
        0,    // vendor
        0,    // library
        "xps-intc",    // name
        0,    // version
        "pse"     // model
    );

    icmAttrListP xps_intc_0_attr = icmNewAttrList();


    icmPseP xps_intc_0_p = icmNewPSE(
        "xps_intc_0",   // name
        xps_intc_0_path,   // model
        xps_intc_0_attr,   // attrlist
        0,   // semihost file
        0    // semihost symbol
    );

    icmConnectPSEBus( xps_intc_0_p, bus1_b, "plb", 0, 0x81800000, 0x8180001f);


////////////////////////////////////////////////////////////////////////////////
//                            PSE Hard_Ethernet_MAC
////////////////////////////////////////////////////////////////////////////////

    const char *Hard_Ethernet_MAC_path = icmGetVlnvString(
        0,    // path (0 if from the product directory)
        0,    // vendor
        0,    // library
        "xps-ll-temac",    // name
        0,    // version
        "pse"     // model
    );

    icmAttrListP Hard_Ethernet_MAC_attr = icmNewAttrList();


    icmPseP Hard_Ethernet_MAC_p = icmNewPSE(
        "Hard_Ethernet_MAC",   // name
        Hard_Ethernet_MAC_path,   // model
        Hard_Ethernet_MAC_attr,   // attrlist
        0,   // semihost file
        0    // semihost symbol
    );

    icmConnectPSEBus( Hard_Ethernet_MAC_p, bus1_b, "plb", 0, 0x81c00000, 0x81c0003f);


////////////////////////////////////////////////////////////////////////////////
//                           PSE SysACE_CompactFlash
////////////////////////////////////////////////////////////////////////////////

    const char *SysACE_CompactFlash_path = icmGetVlnvString(
        0,    // path (0 if from the product directory)
        0,    // vendor
        0,    // library
        "xps-sysace",    // name
        0,    // version
        "pse"     // model
    );

    icmAttrListP SysACE_CompactFlash_attr = icmNewAttrList();


    icmPseP SysACE_CompactFlash_p = icmNewPSE(
        "SysACE_CompactFlash",   // name
        SysACE_CompactFlash_path,   // model
        SysACE_CompactFlash_attr,   // attrlist
        0,   // semihost file
        0    // semihost symbol
    );

    icmConnectPSEBus( SysACE_CompactFlash_p, bus1_b, "plb", 0, 0x83600000, 0x8360ffff);


////////////////////////////////////////////////////////////////////////////////
//                               PSE xps_timer_1
////////////////////////////////////////////////////////////////////////////////

    const char *xps_timer_1_path = icmGetVlnvString(
        0,    // path (0 if from the product directory)
        0,    // vendor
        0,    // library
        "xps-timer",    // name
        0,    // version
        "pse"     // model
    );

    icmAttrListP xps_timer_1_attr = icmNewAttrList();


    icmPseP xps_timer_1_p = icmNewPSE(
        "xps_timer_1",   // name
        xps_timer_1_path,   // model
        xps_timer_1_attr,   // attrlist
        0,   // semihost file
        0    // semihost symbol
    );

    icmConnectPSEBus( xps_timer_1_p, bus1_b, "plb", 0, 0x83c00000, 0x83c0001f);


////////////////////////////////////////////////////////////////////////////////
//                              PSE RS232_Uart_1
////////////////////////////////////////////////////////////////////////////////

    const char *RS232_Uart_1_path = icmGetVlnvString(
        0,    // path (0 if from the product directory)
        0,    // vendor
        0,    // library
        "xps-uartlite",    // name
        0,    // version
        "pse"     // model
    );

    icmAttrListP RS232_Uart_1_attr = icmNewAttrList();

    if(connectUartPort) {
        if (autoConsole) {
            icmAddUns64Attr(RS232_Uart_1_attr, "console", 1);
        } else {
            icmAddDoubleAttr(RS232_Uart_1_attr, "portnum", uartPort);
        }
        icmAddUns64Attr(RS232_Uart_1_attr, "finishOnDisconnect", 1);
    }
    icmAddStringAttr(RS232_Uart_1_attr, "outfile", "RS232_Uart_1.log");

    icmPseP RS232_Uart_1_p = icmNewPSE(
        "RS232_Uart_1",   // name
        RS232_Uart_1_path,   // model
        RS232_Uart_1_attr,   // attrlist
        NULL,   // semihost file
        NULL    // semihost symbol
    );

    icmConnectPSEBus( RS232_Uart_1_p, bus1_b, "plb", 0, 0x84000000, 0x8400000f);

    if(connectUartPort) {
        icmSetPSEdiagnosticLevel(RS232_Uart_1_p, 1);
    }

////////////////////////////////////////////////////////////////////////////////
//                              PSE debug_module
////////////////////////////////////////////////////////////////////////////////

    const char *debug_module_path = icmGetVlnvString(
        0,    // path (0 if from the product directory)
        0,    // vendor
        0,    // library
        "mdm",    // name
        0,    // version
        "pse"     // model
    );

    icmAttrListP debug_module_attr = icmNewAttrList();


    icmPseP debug_module_p = icmNewPSE(
        "debug_module",   // name
        debug_module_path,   // model
        debug_module_attr,   // attrlist
        0,   // semihost file
        0    // semihost symbol
    );

    icmConnectPSEBus( debug_module_p, bus1_b, "plb", 0, 0x84400000, 0x8440ffff);


////////////////////////////////////////////////////////////////////////////////
//                                  PSE mpmc
////////////////////////////////////////////////////////////////////////////////

    const char *mpmc_path = icmGetVlnvString(
        0,    // path (0 if from the product directory)
        0,    // vendor
        0,    // library
        "mpmc",    // name
        0,    // version
        "pse"     // model
    );

    icmAttrListP mpmc_attr = icmNewAttrList();


    icmPseP mpmc_p = icmNewPSE(
        "mpmc",   // name
        mpmc_path,   // model
        mpmc_attr,   // attrlist
        0,   // semihost file
        0    // semihost symbol
    );

    icmConnectPSEBus( mpmc_p, bus1_b, "plb", 0, 0x84600180, 0x846001ff);


////////////////////////////////////////////////////////////////////////////////
//                                 PSE mb_plb
////////////////////////////////////////////////////////////////////////////////

    const char *mb_plb_path = icmGetVlnvString(
        0,    // path (0 if from the product directory)
        0,    // vendor
        0,    // library
        "xps-mch-emc",    // name
        0,    // version
        "pse"     // model
    );

    icmAttrListP mb_plb_attr = icmNewAttrList();


    icmPseP mb_plb_p = icmNewPSE(
        "mb_plb",   // name
        mb_plb_path,   // model
        mb_plb_attr,   // attrlist
        0,   // semihost file
        0    // semihost symbol
    );

    icmConnectPSEBus( mb_plb_p, bus1_b, "plb", 0, 0xa0000000, 0xa1ffffff);


////////////////////////////////////////////////////////////////////////////////
//                               Memory BOOTMEM
////////////////////////////////////////////////////////////////////////////////

    icmMemoryP BOOTMEM_m = icmNewMemory("BOOTMEM_m", 0x7, 0x1ffffff);

    icmConnectMemoryToBus( bus1_b, "sp1", BOOTMEM_m, 0x0);


    ////////////////////////////////////////////////////////////////////////////////
    //                              Memory DDR2_SDRAM
    ////////////////////////////////////////////////////////////////////////////////

    icmMemoryP DDR2_SDRAM_m = icmNewMemory("DDR2_SDRAM_m", 0x7, 0xfffffff);

    icmConnectMemoryToBus( bus1_b, "sp1", DDR2_SDRAM_m, 0x90000000);

    ////////////////////////////////////////////////////////////////////////////////
    //                              Memory UNKNOWN_PERIPH
    ////////////////////////////////////////////////////////////////////////////////

    icmMemoryP UNKNOWN_PERIPH_m = icmNewMemory("UNKNOWN_PERIPH_m", 0x7, 0x00000fff);

    icmConnectMemoryToBus( bus1_b, "sp1", UNKNOWN_PERIPH_m, 0x8FFFF000);


////////////////////////////////////////////////////////////////////////////////
//                                 CONNECTIONS
////////////////////////////////////////////////////////////////////////////////

    icmNetP Interrupt_net_n = icmNewNet("Interrupt_net_n" );

    icmConnectProcessorNet( microblaze_0_c, Interrupt_net_n, "Interrupt", ICM_INPUT);

    icmConnectPSENet( xps_intc_0_p, Interrupt_net_n, "Irq", ICM_OUTPUT);

////////////////////////////////////////////////////////////////////////////////
    icmNetP Interrupt_3_n = icmNewNet("Interrupt_3_n" );

    icmConnectPSENet( xps_intc_0_p, Interrupt_3_n, "Intr3", ICM_INPUT);

    icmConnectPSENet( xps_timer_1_p, Interrupt_3_n, "Interrupt", ICM_OUTPUT);

////////////////////////////////////////////////////////////////////////////////
    icmNetP Interrupt_8_n = icmNewNet("Interrupt_8_n" );

    icmConnectPSENet( xps_intc_0_p, Interrupt_8_n, "Intr8", ICM_INPUT);

    icmConnectPSENet( RS232_Uart_1_p, Interrupt_8_n, "Interrupt", ICM_OUTPUT);

    if(icmLoadProcessorMemory(microblaze_0_c, kernelFile, ICM_LOAD_PHYSICAL, True, True)) {
    } else {
        icmPrintf("Failed to load %s\n", kernelFile);
        exit(1);
    }
}

static const char* arguments = "[attach <port number>] "
                               "[stopafter <seconds to stop after>]";

static void parseArgs(int firstarg, int argc, char **argv) {

    int i;
    for(i=firstarg; i < argc; i++) {
        if (strcmp(argv[i], "attach") == 0) {
            if (strcmp(argv[++i], "auto") == 0) {
                autoConsole = True;
            } else {
                sscanf(argv[i], "%d", &uartPort);
            }
            connectUartPort = True;
        } else if (strcmp(argv[i], "stopafter") == 0) {
            sscanf(argv[++i], "%f", &stopafter);
       } else {
            fprintf(stderr, "unrecognised argument %s\n", argv[i]);
            exit(1);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
//                                   M A I N
////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {
    if((argc<2)) {
        icmPrintf(
           "usage: %s <kernelFile> %s\n",
            argv[0], arguments
        );
        return -1;
    }
    char *kernelFile = argv[1];

    parseArgs(2, argc, argv);

    // the constructor
    createPlatform(kernelFile);

    if (stopafter) {
        // test mode, run for limited number of instructions
        if(!icmSetSimulationStopTime(stopafter)){
            icmPrintf("Failed to Set Stop Time %f\n", stopafter);
            return -1;
        }
    }

    // run till the end
    icmSimulatePlatform();
    icmTerminate();
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
