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


////////////////////////////////////////////////////////////////////////////////
//
//                W R I T T E N   B Y   I M P E R A S   I G E N
//
//                             Version 20140731.0
//                          Mon Aug  4 13:35:18 2014
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////// Description /////////////////////////////////

// Microblaze LogiCORE IP XPS Interrupt Controller

/////////////////////////////////// Licensing //////////////////////////////////

// Open Source Apache 2.0

////////////////////////////////// Limitations /////////////////////////////////

// This model implements all of the required behavior sufficient to boot Linux


#include "pse.igen.h"
/////////////////////////////// Port Declarations //////////////////////////////

plb_REG_dataT plb_REG_data;

handlesT handles;

/////////////////////////////// Diagnostic level ///////////////////////////////

// Test this variable to determine what diagnostics to output.
// eg. if (diagnosticLevel > 0) bhmMessage("I", "xps-intc", "Example");

Uns32 diagnosticLevel;

/////////////////////////// Diagnostic level callback //////////////////////////

static void setDiagLevel(Uns32 new) {
    diagnosticLevel = new;
}

///////////////////////////// MMR Generic callbacks ////////////////////////////

static PPM_VIEW_CB(view32) {  *(Uns32*)data = *(Uns32*)user; }

//////////////////////////////// Bus Slave Ports ///////////////////////////////

static void installSlavePorts(void) {
    handles.plb = ppmCreateSlaveBusPort("plb", 32);
    if (!handles.plb) {
        bhmMessage("E", "PPM_SPNC", "Could not connect port 'plb'");
    }

}

//////////////////////////// Memory mapped registers ///////////////////////////

static void installRegisters(void) {

    ppmCreateRegister("REG_ISR",
        0,
        handles.plb,
        0,
        4,
        ReadISR,
        WriteISR,
        view32,
        &(plb_REG_data.ISR.value),
        True
    );
    ppmCreateRegister("REG_IPR",
        0,
        handles.plb,
        4,
        4,
        ReadIPR,
        0,
        view32,
        &(plb_REG_data.IPR.value),
        True
    );
    ppmCreateRegister("REG_IER",
        0,
        handles.plb,
        8,
        4,
        ReadIER,
        WriteIER,
        view32,
        &(plb_REG_data.IER.value),
        True
    );
    ppmCreateRegister("REG_IAR",
        0,
        handles.plb,
        12,
        4,
        0,
        WriteIAR,
        view32,
        &(plb_REG_data.IAR.value),
        True
    );
    ppmCreateRegister("REG_SIE",
        0,
        handles.plb,
        16,
        4,
        0,
        WriteSIE,
        view32,
        &(plb_REG_data.SIE.value),
        True
    );
    ppmCreateRegister("REG_CIE",
        0,
        handles.plb,
        20,
        4,
        0,
        WriteCIE,
        view32,
        &(plb_REG_data.CIE.value),
        True
    );
    ppmCreateRegister("REG_IVR",
        0,
        handles.plb,
        24,
        4,
        ReadIVR,
        0,
        view32,
        &(plb_REG_data.IVR.value),
        True
    );
    ppmCreateRegister("REG_MER",
        0,
        handles.plb,
        28,
        4,
        ReadMER,
        WriteMER,
        view32,
        &(plb_REG_data.MER.value),
        True
    );

}

/////////////////////////////////// Net Ports //////////////////////////////////

static void installNetPorts(void) {
// To write to this net, use ppmWriteNet(handles.Irq, value);

    handles.Irq = ppmOpenNetPort("Irq");

    handles.Intr0 = ppmOpenNetPort("Intr0");
    if (handles.Intr0) {
        ppmInstallNetCallback(handles.Intr0, WriteIntr, (void*)0);
    }

    handles.Intr1 = ppmOpenNetPort("Intr1");
    if (handles.Intr1) {
        ppmInstallNetCallback(handles.Intr1, WriteIntr, (void*)1);
    }

    handles.Intr2 = ppmOpenNetPort("Intr2");
    if (handles.Intr2) {
        ppmInstallNetCallback(handles.Intr2, WriteIntr, (void*)2);
    }

    handles.Intr3 = ppmOpenNetPort("Intr3");
    if (handles.Intr3) {
        ppmInstallNetCallback(handles.Intr3, WriteIntr, (void*)3);
    }

    handles.Intr4 = ppmOpenNetPort("Intr4");
    if (handles.Intr4) {
        ppmInstallNetCallback(handles.Intr4, WriteIntr, (void*)4);
    }

    handles.Intr5 = ppmOpenNetPort("Intr5");
    if (handles.Intr5) {
        ppmInstallNetCallback(handles.Intr5, WriteIntr, (void*)5);
    }

    handles.Intr6 = ppmOpenNetPort("Intr6");
    if (handles.Intr6) {
        ppmInstallNetCallback(handles.Intr6, WriteIntr, (void*)6);
    }

    handles.Intr7 = ppmOpenNetPort("Intr7");
    if (handles.Intr7) {
        ppmInstallNetCallback(handles.Intr7, WriteIntr, (void*)7);
    }

    handles.Intr8 = ppmOpenNetPort("Intr8");
    if (handles.Intr8) {
        ppmInstallNetCallback(handles.Intr8, WriteIntr, (void*)8);
    }

    handles.Intr9 = ppmOpenNetPort("Intr9");
    if (handles.Intr9) {
        ppmInstallNetCallback(handles.Intr9, WriteIntr, (void*)9);
    }

    handles.Intr10 = ppmOpenNetPort("Intr10");
    if (handles.Intr10) {
        ppmInstallNetCallback(handles.Intr10, WriteIntr, (void*)10);
    }

    handles.Intr11 = ppmOpenNetPort("Intr11");
    if (handles.Intr11) {
        ppmInstallNetCallback(handles.Intr11, WriteIntr, (void*)11);
    }

    handles.Intr12 = ppmOpenNetPort("Intr12");
    if (handles.Intr12) {
        ppmInstallNetCallback(handles.Intr12, WriteIntr, (void*)12);
    }

    handles.Intr13 = ppmOpenNetPort("Intr13");
    if (handles.Intr13) {
        ppmInstallNetCallback(handles.Intr13, WriteIntr, (void*)13);
    }

    handles.Intr14 = ppmOpenNetPort("Intr14");
    if (handles.Intr14) {
        ppmInstallNetCallback(handles.Intr14, WriteIntr, (void*)14);
    }

    handles.Intr15 = ppmOpenNetPort("Intr15");
    if (handles.Intr15) {
        ppmInstallNetCallback(handles.Intr15, WriteIntr, (void*)15);
    }

    handles.Intr16 = ppmOpenNetPort("Intr16");
    if (handles.Intr16) {
        ppmInstallNetCallback(handles.Intr16, WriteIntr, (void*)16);
    }

    handles.Intr17 = ppmOpenNetPort("Intr17");
    if (handles.Intr17) {
        ppmInstallNetCallback(handles.Intr17, WriteIntr, (void*)17);
    }

    handles.Intr18 = ppmOpenNetPort("Intr18");
    if (handles.Intr18) {
        ppmInstallNetCallback(handles.Intr18, WriteIntr, (void*)18);
    }

    handles.Intr19 = ppmOpenNetPort("Intr19");
    if (handles.Intr19) {
        ppmInstallNetCallback(handles.Intr19, WriteIntr, (void*)19);
    }

    handles.Intr20 = ppmOpenNetPort("Intr20");
    if (handles.Intr20) {
        ppmInstallNetCallback(handles.Intr20, WriteIntr, (void*)20);
    }

    handles.Intr21 = ppmOpenNetPort("Intr21");
    if (handles.Intr21) {
        ppmInstallNetCallback(handles.Intr21, WriteIntr, (void*)21);
    }

    handles.Intr22 = ppmOpenNetPort("Intr22");
    if (handles.Intr22) {
        ppmInstallNetCallback(handles.Intr22, WriteIntr, (void*)22);
    }

    handles.Intr23 = ppmOpenNetPort("Intr23");
    if (handles.Intr23) {
        ppmInstallNetCallback(handles.Intr23, WriteIntr, (void*)23);
    }

    handles.Intr24 = ppmOpenNetPort("Intr24");
    if (handles.Intr24) {
        ppmInstallNetCallback(handles.Intr24, WriteIntr, (void*)24);
    }

    handles.Intr25 = ppmOpenNetPort("Intr25");
    if (handles.Intr25) {
        ppmInstallNetCallback(handles.Intr25, WriteIntr, (void*)25);
    }

    handles.Intr26 = ppmOpenNetPort("Intr26");
    if (handles.Intr26) {
        ppmInstallNetCallback(handles.Intr26, WriteIntr, (void*)26);
    }

    handles.Intr27 = ppmOpenNetPort("Intr27");
    if (handles.Intr27) {
        ppmInstallNetCallback(handles.Intr27, WriteIntr, (void*)27);
    }

    handles.Intr28 = ppmOpenNetPort("Intr28");
    if (handles.Intr28) {
        ppmInstallNetCallback(handles.Intr28, WriteIntr, (void*)28);
    }

    handles.Intr29 = ppmOpenNetPort("Intr29");
    if (handles.Intr29) {
        ppmInstallNetCallback(handles.Intr29, WriteIntr, (void*)29);
    }

    handles.Intr30 = ppmOpenNetPort("Intr30");
    if (handles.Intr30) {
        ppmInstallNetCallback(handles.Intr30, WriteIntr, (void*)30);
    }

    handles.Intr31 = ppmOpenNetPort("Intr31");
    if (handles.Intr31) {
        ppmInstallNetCallback(handles.Intr31, WriteIntr, (void*)31);
    }

}

PPM_NET_CB(reset_resetNet) {
    if(value != 0 ) {
        plb_REG_data.MER.value = 0x0;
        plb_REG_data.IVR.value = 0xffffffff;
        plb_REG_data.CIE.value = 0x0;
        plb_REG_data.SIE.value = 0x0;
        plb_REG_data.IAR.value = 0x0;
        plb_REG_data.IER.value = 0x0;
        plb_REG_data.IPR.value = 0x0;
        plb_REG_data.ISR.value = 0x0;
    }
}

////////////////////////////////// Constructor /////////////////////////////////

PPM_CONSTRUCTOR_CB(periphConstructor) {
    installSlavePorts();
    installRegisters();
    installNetPorts();
    reset_resetNet(1,0);
}

///////////////////////////////////// Main /////////////////////////////////////

int main(int argc, char *argv[]) {
    diagnosticLevel = 0;
    bhmInstallDiagCB(setDiagLevel);
    constructor();

    bhmWaitEvent(bhmGetSystemEvent(BHM_SE_END_OF_SIMULATION));
    destructor();
    return 0;
}

