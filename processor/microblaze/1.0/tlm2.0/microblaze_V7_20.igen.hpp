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
//                          Mon Aug  4 13:39:19 2014
//
////////////////////////////////////////////////////////////////////////////////

#ifndef XILINX_OVPWORLD_ORG_PROCESSOR_MICROBLAZE_V7_20__1_0
#define XILINX_OVPWORLD_ORG_PROCESSOR_MICROBLAZE_V7_20__1_0

#include "ovpworld.org/modelSupport/tlmProcessor/1.0/tlm2.0/tlmProcessor.hpp"
using namespace sc_core;

class microblaze_V7_20 : public icmCpu
{
  private:
    const char *getModel() {
        return icmGetVlnvString (NULL, "xilinx.ovpworld.org", "processor", "microblaze", "1.0", "model");
    }

    const char *getSHL() {
        return icmGetVlnvString (NULL, "xilinx.ovpworld.org", "semihosting", "microblazeNewlib", "1.0", "model");
    }

  public:
    icmCpuMasterPort     INSTRUCTION;
    icmCpuMasterPort     DATA;
    icmCpuInterrupt      Interrupt;
    icmCpuInterrupt      Reset;
    icmCpuInterrupt      MB_Reset;
    icmCpuInterrupt      Ext_BRK;
    icmCpuInterrupt      Ext_NM_BRK;

    microblaze_V7_20(
        sc_module_name        name,
        const unsigned int    ID,
        icmNewProcAttrs       attrs        = ICM_ATTR_DEFAULT,
        icmAttrListObject    *attrList    = NULL,
        const char           *semiHost    = NULL,
        Uns32                 addressBits = 32,
        bool                  dmi         = true,
        Uns32                 cpuFlags    = 0
     )
    : icmCpu(name, ID, "microblaze", getModel(), 0, semiHost ? ((0 == strlen(semiHost)) ? 0 : semiHost) : getSHL(), attrs, attrList, addressBits, dmi, cpuFlags)
    , INSTRUCTION (this, "INSTRUCTION", 32)
    , DATA (this, "DATA", 32)
    , Interrupt("Interrupt", this)
    , Reset("Reset", this)
    , MB_Reset("MB_Reset", this)
    , Ext_BRK("Ext_BRK", this)
    , Ext_NM_BRK("Ext_NM_BRK", this)
    {
    }

    void before_end_of_elaboration() {
        DATA.bindIfNotBound();
    }
}; /* class microblaze_V7_20 */

#endif
