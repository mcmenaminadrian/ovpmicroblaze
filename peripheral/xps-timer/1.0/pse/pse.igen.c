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

// Microblaze LogiCORE IP XPS Timer/Counter

/////////////////////////////////// Licensing //////////////////////////////////

// Open Source Apache 2.0

////////////////////////////////// Limitations /////////////////////////////////

// Resolution of this timer is limited to the simulation time slice (aka quantum)
//  size

////////////////////////////////// Licensing_1 /////////////////////////////////

// Open Source Apache 2.0


#include "pse.igen.h"
/////////////////////////////// Port Declarations //////////////////////////////

plb_REG_dataT plb_REG_data;

handlesT handles;

/////////////////////////////// Diagnostic level ///////////////////////////////

// Test this variable to determine what diagnostics to output.
// eg. if (diagnosticLevel > 0) bhmMessage("I", "xps-timer", "Example");

Uns32 diagnosticLevel;

/////////////////////////// Diagnostic level callback //////////////////////////

static void setDiagLevel(Uns32 new) {
    diagnosticLevel = new;
}

///////////////////////////// MMR Generic callbacks ////////////////////////////

//////////////////////////////// View callbacks ////////////////////////////////

static PPM_VIEW_CB(view_plb_REG_TCSR0) {
    *(Uns32*)data = plb_REG_data.TCSR0.value;
}

static PPM_VIEW_CB(view_plb_REG_TLR0) {
    *(Uns32*)data = plb_REG_data.TLR0.value;
}

static PPM_VIEW_CB(view_plb_REG_TCR0) {
    *(Uns32*)data = plb_REG_data.TCR0.value;
}

static PPM_VIEW_CB(view_plb_REG_TCSR1) {
    *(Uns32*)data = plb_REG_data.TCSR1.value;
}

static PPM_VIEW_CB(view_plb_REG_TLR1) {
    *(Uns32*)data = plb_REG_data.TLR1.value;
}

static PPM_VIEW_CB(view_plb_REG_TCR1) {
    *(Uns32*)data = plb_REG_data.TCR1.value;
}
//////////////////////////////// Bus Slave Ports ///////////////////////////////

static void installSlavePorts(void) {
    handles.plb = ppmCreateSlaveBusPort("plb", 32);
    if (!handles.plb) {
        bhmMessage("E", "PPM_SPNC", "Could not connect port 'plb'");
    }

}

//////////////////////////// Memory mapped registers ///////////////////////////

static void installRegisters(void) {

    ppmCreateRegister("REG_TCSR0",
        0,
        handles.plb,
        0,
        4,
        ReadTCSR0,
        WriteTCSR0,
        view_plb_REG_TCSR0,
        (void*)0x0,
        True
    );
    ppmCreateRegister("REG_TLR0",
        0,
        handles.plb,
        4,
        4,
        ReadTLR0,
        WriteTLR0,
        view_plb_REG_TLR0,
        (void*)0x4,
        True
    );
    ppmCreateRegister("REG_TCR0",
        0,
        handles.plb,
        8,
        4,
        ReadTCR0,
        0,
        view_plb_REG_TCR0,
        (void*)0x8,
        True
    );
    ppmCreateRegister("REG_TCSR1",
        0,
        handles.plb,
        16,
        4,
        ReadTCSR1,
        WriteTCSR1,
        view_plb_REG_TCSR1,
        (void*)0x10,
        True
    );
    ppmCreateRegister("REG_TLR1",
        0,
        handles.plb,
        20,
        4,
        ReadTLR1,
        WriteTLR1,
        view_plb_REG_TLR1,
        (void*)0x14,
        True
    );
    ppmCreateRegister("REG_TCR1",
        0,
        handles.plb,
        24,
        4,
        ReadTCR1,
        0,
        view_plb_REG_TCR1,
        (void*)0x18,
        True
    );

}

/////////////////////////////////// Net Ports //////////////////////////////////

static void installNetPorts(void) {
// To write to this net, use ppmWriteNet(handles.Interrupt, value);

    handles.Interrupt = ppmOpenNetPort("Interrupt");

}

PPM_NET_CB(reset_resetNet) {
    if(value != 0 ) {
        plb_REG_data.TCR1.value = 0x0;
        plb_REG_data.TLR1.value = 0x0;
        plb_REG_data.TCSR1.value = 0x0;
        plb_REG_data.TCR0.value = 0x0;
        plb_REG_data.TLR0.value = 0x0;
        plb_REG_data.TCSR0.value = 0x0;
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

