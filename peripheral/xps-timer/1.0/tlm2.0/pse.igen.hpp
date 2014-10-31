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
//                          Mon Aug  4 13:35:21 2014
//
////////////////////////////////////////////////////////////////////////////////

// MODEL IO:
//    Slave Port plb
//    Net output  Interrupt;

#ifndef XILINX_OVPWORLD_ORG_PERIPHERAL_XPS_TIMER__1_0
#define XILINX_OVPWORLD_ORG_PERIPHERAL_XPS_TIMER__1_0
#include "ovpworld.org/modelSupport/tlmPeripheral/1.0/tlm2.0/tlmPeripheral.hpp"
using namespace sc_core;

class xps_timer : public icmPeripheral
{
  private:
    const char *getModel() {
        return icmGetVlnvString (NULL, "xilinx.ovpworld.org", "peripheral", "xps-timer", "1.0", "pse");
    }

  public:
    icmSlavePort        plb;
    icmOutputNetPort Interrupt;

    xps_timer(sc_module_name name, icmAttrListObject *initialAttrs = 0 )
        : icmPeripheral(name, getModel(), 0, initialAttrs)
        , plb(this, "plb", 0x20) // static
        , Interrupt(this, "Interrupt")
    {
    }

}; /* class xps_timer */

#endif
