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

/////////////////////////////////// Licensing //////////////////////////////////

// Open Source Apache 2.0


#include "pse.igen.h"
/////////////////////////////// Port Declarations //////////////////////////////

plb_REG_dataT plb_REG_data;

handlesT handles;

/////////////////////////////// Diagnostic level ///////////////////////////////

// Test this variable to determine what diagnostics to output.
// eg. if (diagnosticLevel > 0) bhmMessage("I", "xps-uartlite", "Example");

Uns32 diagnosticLevel;

/////////////////////////// Diagnostic level callback //////////////////////////

static void setDiagLevel(Uns32 new) {
    diagnosticLevel = new;
}

///////////////////////////// MMR Generic callbacks ////////////////////////////

static PPM_VIEW_CB(view32) {  *(Uns32*)data = *(Uns32*)user; }

//////////////////////////////// Bus Slave Ports ///////////////////////////////

static void installSlavePorts(void) {
    handles.plb = ppmCreateSlaveBusPort("plb", 16);
    if (!handles.plb) {
        bhmMessage("E", "PPM_SPNC", "Could not connect port 'plb'");
    }

}

//////////////////////////// Memory mapped registers ///////////////////////////

static void installRegisters(void) {

    ppmCreateRegister("REG_RxFifo",
        0,
        handles.plb,
        0,
        4,
        ReadRxFifo,
        0,
        view32,
        &(plb_REG_data.RxFifo.value),
        True
    );
    ppmCreateRegister("REG_TxFifo",
        0,
        handles.plb,
        4,
        4,
        0,
        WriteTxFifo,
        view32,
        &(plb_REG_data.TxFifo.value),
        True
    );
    ppmCreateRegister("REG_Stat",
        0,
        handles.plb,
        8,
        4,
        ReadStat,
        0,
        view32,
        &(plb_REG_data.Stat.value),
        True
    );
    ppmCreateRegister("REG_Ctrl",
        0,
        handles.plb,
        12,
        4,
        0,
        WriteCtrl,
        view32,
        &(plb_REG_data.Ctrl.value),
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
        plb_REG_data.Ctrl.value = 0x0;
        plb_REG_data.Stat.value = 0x2;
        plb_REG_data.TxFifo.value = 0x0;
        plb_REG_data.RxFifo.value = 0x0;
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

