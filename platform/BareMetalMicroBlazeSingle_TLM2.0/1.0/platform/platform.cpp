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



#include "tlm.h"
#include "ovpworld.org/modelSupport/tlmPlatform/1.0/tlm2.0/tlmPlatform.hpp"
#include "ovpworld.org/modelSupport/tlmDecoder/1.0/tlm2.0/tlmDecoder.hpp"
#include "ovpworld.org/memory/ram/1.0/tlm2.0/tlmMemory.hpp"
#include "xilinx.ovpworld.org/processor/microblaze/1.0/tlm2.0/processor.igen.hpp"

////////////////////////////////////////////////////////////////////////////////
//                      BareMetalMicroBlazeSingle_TLM2_0 Class                      //
////////////////////////////////////////////////////////////////////////////////

class BareMetalMicroBlazeSingle_TLM2_0 : public sc_core::sc_module {

  public:
    BareMetalMicroBlazeSingle_TLM2_0 (sc_core::sc_module_name name);

    icmTLMPlatform        Platform;
    decoder             <2,2> bus1;
    ram                   ram1;
    ram                   ram2;
    microblaze            cpu1;

}; /* BareMetalMicroBlazeSingle_TLM2_0 */

////////////////////////////////////////////////////////////////////////////////
//                   BareMetalMicroBlazeSingle_TLM2_0 Constructor                   //
////////////////////////////////////////////////////////////////////////////////

BareMetalMicroBlazeSingle_TLM2_0::BareMetalMicroBlazeSingle_TLM2_0 ( sc_core::sc_module_name name)
    : sc_core::sc_module (name)
    , Platform ("icm", ICM_VERBOSE | ICM_STOP_ON_CTRLC| ICM_ENABLE_IMPERAS_INTERCEPTS )
    , bus1("bus1")
    , ram1 ("ram1", "sp1", 0x07FFFF)
    , ram2 ("ram2", "sp1", 0x0FFFFF)
    , cpu1 ( "cpu1", 0, ICM_ATTR_DEFAULT)
{

    // bus1 masters
    cpu1.INSTRUCTION.socket(bus1.target_socket[0]);
    cpu1.DATA.socket(bus1.target_socket[1]);

    // bus1 slaves
    bus1.initiator_socket[0](ram1.sp1); // Memory
    bus1.setDecode(0, 0x0, 0x7ffff);

    bus1.initiator_socket[1](ram2.sp1); // Memory
    bus1.setDecode(1, 0x00200000, 0x002fffff);
}

int sc_main (int argc, char *argv[] )
{
    const char *app = "noAppSupplied";

    sc_time stop(0,SC_MS);

    if (argc > 1 && argc <= 2) {
        app = argv[1];
    } else {
        cout << "Usage: " << argv[0] << " <application> [stop time]" << endl;
        cout << "     :   stop time is in milliseconds" << endl;
        return 0;
    }

    // Ignore some of the Warning messages
    icmIgnoreMessage ("ICM_NPF");

    BareMetalMicroBlazeSingle_TLM2_0 top("top");             // instantiate example top module

    // Use the processor to load memory with the program.
    top.cpu1.loadLocalMemory(app, (icmLoaderAttrs)(ICM_LOAD_VERBOSE));

    if (argc > 2) {
        stop = sc_time(atoi(argv[2]), SC_MS);

        sc_core::sc_start(stop);                  // start the simulation
    } else {
        sc_core::sc_start();                      // start the simulation
    }

    cout << "Finished sc_main." << endl;
    return 0;                             // return okay status
}
