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
//    Net output  Irq;
//    Net input  Intr0;
//    Net input  Intr1;
//    Net input  Intr2;
//    Net input  Intr3;
//    Net input  Intr4;
//    Net input  Intr5;
//    Net input  Intr6;
//    Net input  Intr7;
//    Net input  Intr8;
//    Net input  Intr9;
//    Net input  Intr10;
//    Net input  Intr11;
//    Net input  Intr12;
//    Net input  Intr13;
//    Net input  Intr14;
//    Net input  Intr15;
//    Net input  Intr16;
//    Net input  Intr17;
//    Net input  Intr18;
//    Net input  Intr19;
//    Net input  Intr20;
//    Net input  Intr21;
//    Net input  Intr22;
//    Net input  Intr23;
//    Net input  Intr24;
//    Net input  Intr25;
//    Net input  Intr26;
//    Net input  Intr27;
//    Net input  Intr28;
//    Net input  Intr29;
//    Net input  Intr30;
//    Net input  Intr31;

#ifndef XILINX_OVPWORLD_ORG_PERIPHERAL_XPS_INTC__1_0
#define XILINX_OVPWORLD_ORG_PERIPHERAL_XPS_INTC__1_0
#include "ovpworld.org/modelSupport/tlmPeripheral/1.0/tlm2.0/tlmPeripheral.hpp"
using namespace sc_core;

class xps_intc : public icmPeripheral
{
  private:
    const char *getModel() {
        return icmGetVlnvString (NULL, "xilinx.ovpworld.org", "peripheral", "xps-intc", "1.0", "pse");
    }

  public:
    icmSlavePort        plb;
    icmOutputNetPort Irq;
    icmInputNetPort Intr0;
    icmInputNetPort Intr1;
    icmInputNetPort Intr2;
    icmInputNetPort Intr3;
    icmInputNetPort Intr4;
    icmInputNetPort Intr5;
    icmInputNetPort Intr6;
    icmInputNetPort Intr7;
    icmInputNetPort Intr8;
    icmInputNetPort Intr9;
    icmInputNetPort Intr10;
    icmInputNetPort Intr11;
    icmInputNetPort Intr12;
    icmInputNetPort Intr13;
    icmInputNetPort Intr14;
    icmInputNetPort Intr15;
    icmInputNetPort Intr16;
    icmInputNetPort Intr17;
    icmInputNetPort Intr18;
    icmInputNetPort Intr19;
    icmInputNetPort Intr20;
    icmInputNetPort Intr21;
    icmInputNetPort Intr22;
    icmInputNetPort Intr23;
    icmInputNetPort Intr24;
    icmInputNetPort Intr25;
    icmInputNetPort Intr26;
    icmInputNetPort Intr27;
    icmInputNetPort Intr28;
    icmInputNetPort Intr29;
    icmInputNetPort Intr30;
    icmInputNetPort Intr31;

    xps_intc(sc_module_name name, icmAttrListObject *initialAttrs = 0 )
        : icmPeripheral(name, getModel(), 0, initialAttrs)
        , plb(this, "plb", 0x20) // static
        , Irq(this, "Irq")
        , Intr0(this, "Intr0")
        , Intr1(this, "Intr1")
        , Intr2(this, "Intr2")
        , Intr3(this, "Intr3")
        , Intr4(this, "Intr4")
        , Intr5(this, "Intr5")
        , Intr6(this, "Intr6")
        , Intr7(this, "Intr7")
        , Intr8(this, "Intr8")
        , Intr9(this, "Intr9")
        , Intr10(this, "Intr10")
        , Intr11(this, "Intr11")
        , Intr12(this, "Intr12")
        , Intr13(this, "Intr13")
        , Intr14(this, "Intr14")
        , Intr15(this, "Intr15")
        , Intr16(this, "Intr16")
        , Intr17(this, "Intr17")
        , Intr18(this, "Intr18")
        , Intr19(this, "Intr19")
        , Intr20(this, "Intr20")
        , Intr21(this, "Intr21")
        , Intr22(this, "Intr22")
        , Intr23(this, "Intr23")
        , Intr24(this, "Intr24")
        , Intr25(this, "Intr25")
        , Intr26(this, "Intr26")
        , Intr27(this, "Intr27")
        , Intr28(this, "Intr28")
        , Intr29(this, "Intr29")
        , Intr30(this, "Intr30")
        , Intr31(this, "Intr31")
    {
    }

}; /* class xps_intc */

#endif
