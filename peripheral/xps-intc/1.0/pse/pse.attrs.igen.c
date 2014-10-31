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


#ifdef _PSE_
#    include "peripheral/impTypes.h"
#    include "peripheral/bhm.h"
#    include "peripheral/ppm.h"
#else
#    include "hostapi/impTypes.h"
#endif


static ppmBusPort busPorts[] = {
    {
        .name            = "plb",
        .type            = PPM_SLAVE_PORT,
        .addrHi          = 0x1fLL,
        .mustBeConnected = 1,
        .remappable      = 0,
        .description     = 0,
    },
    { 0 }
};

static PPM_BUS_PORT_FN(nextBusPort) {
    if(!busPort) {
        return busPorts;
    } else {
        busPort++;
    }
    return busPort->name ? busPort : 0;
}

static ppmNetPort netPorts[] = {
    {
        .name            = "Irq",
        .type            = PPM_OUTPUT_PORT,
        .mustBeConnected = 0,
        .description     = 0
    },
    {
        .name            = "Intr0",
        .type            = PPM_INPUT_PORT,
        .mustBeConnected = 0,
        .description     = 0
    },
    {
        .name            = "Intr1",
        .type            = PPM_INPUT_PORT,
        .mustBeConnected = 0,
        .description     = 0
    },
    {
        .name            = "Intr2",
        .type            = PPM_INPUT_PORT,
        .mustBeConnected = 0,
        .description     = 0
    },
    {
        .name            = "Intr3",
        .type            = PPM_INPUT_PORT,
        .mustBeConnected = 0,
        .description     = 0
    },
    {
        .name            = "Intr4",
        .type            = PPM_INPUT_PORT,
        .mustBeConnected = 0,
        .description     = 0
    },
    {
        .name            = "Intr5",
        .type            = PPM_INPUT_PORT,
        .mustBeConnected = 0,
        .description     = 0
    },
    {
        .name            = "Intr6",
        .type            = PPM_INPUT_PORT,
        .mustBeConnected = 0,
        .description     = 0
    },
    {
        .name            = "Intr7",
        .type            = PPM_INPUT_PORT,
        .mustBeConnected = 0,
        .description     = 0
    },
    {
        .name            = "Intr8",
        .type            = PPM_INPUT_PORT,
        .mustBeConnected = 0,
        .description     = 0
    },
    {
        .name            = "Intr9",
        .type            = PPM_INPUT_PORT,
        .mustBeConnected = 0,
        .description     = 0
    },
    {
        .name            = "Intr10",
        .type            = PPM_INPUT_PORT,
        .mustBeConnected = 0,
        .description     = 0
    },
    {
        .name            = "Intr11",
        .type            = PPM_INPUT_PORT,
        .mustBeConnected = 0,
        .description     = 0
    },
    {
        .name            = "Intr12",
        .type            = PPM_INPUT_PORT,
        .mustBeConnected = 0,
        .description     = 0
    },
    {
        .name            = "Intr13",
        .type            = PPM_INPUT_PORT,
        .mustBeConnected = 0,
        .description     = 0
    },
    {
        .name            = "Intr14",
        .type            = PPM_INPUT_PORT,
        .mustBeConnected = 0,
        .description     = 0
    },
    {
        .name            = "Intr15",
        .type            = PPM_INPUT_PORT,
        .mustBeConnected = 0,
        .description     = 0
    },
    {
        .name            = "Intr16",
        .type            = PPM_INPUT_PORT,
        .mustBeConnected = 0,
        .description     = 0
    },
    {
        .name            = "Intr17",
        .type            = PPM_INPUT_PORT,
        .mustBeConnected = 0,
        .description     = 0
    },
    {
        .name            = "Intr18",
        .type            = PPM_INPUT_PORT,
        .mustBeConnected = 0,
        .description     = 0
    },
    {
        .name            = "Intr19",
        .type            = PPM_INPUT_PORT,
        .mustBeConnected = 0,
        .description     = 0
    },
    {
        .name            = "Intr20",
        .type            = PPM_INPUT_PORT,
        .mustBeConnected = 0,
        .description     = 0
    },
    {
        .name            = "Intr21",
        .type            = PPM_INPUT_PORT,
        .mustBeConnected = 0,
        .description     = 0
    },
    {
        .name            = "Intr22",
        .type            = PPM_INPUT_PORT,
        .mustBeConnected = 0,
        .description     = 0
    },
    {
        .name            = "Intr23",
        .type            = PPM_INPUT_PORT,
        .mustBeConnected = 0,
        .description     = 0
    },
    {
        .name            = "Intr24",
        .type            = PPM_INPUT_PORT,
        .mustBeConnected = 0,
        .description     = 0
    },
    {
        .name            = "Intr25",
        .type            = PPM_INPUT_PORT,
        .mustBeConnected = 0,
        .description     = 0
    },
    {
        .name            = "Intr26",
        .type            = PPM_INPUT_PORT,
        .mustBeConnected = 0,
        .description     = 0
    },
    {
        .name            = "Intr27",
        .type            = PPM_INPUT_PORT,
        .mustBeConnected = 0,
        .description     = 0
    },
    {
        .name            = "Intr28",
        .type            = PPM_INPUT_PORT,
        .mustBeConnected = 0,
        .description     = 0
    },
    {
        .name            = "Intr29",
        .type            = PPM_INPUT_PORT,
        .mustBeConnected = 0,
        .description     = 0
    },
    {
        .name            = "Intr30",
        .type            = PPM_INPUT_PORT,
        .mustBeConnected = 0,
        .description     = 0
    },
    {
        .name            = "Intr31",
        .type            = PPM_INPUT_PORT,
        .mustBeConnected = 0,
        .description     = 0
    },
    { 0 }
};

static PPM_NET_PORT_FN(nextNetPort) {
    if(!netPort) {
        return netPorts;
    } else {
        netPort++;
    }
    return netPort->name ? netPort : 0;
}

static ppmParameter parameters[] = {
    { 0 }
};

static PPM_PARAMETER_FN(nextParameter) {
    if(!parameter) {
        return parameters;
    } else {
        parameter++;
    }
    return parameter->name ? parameter : 0;
}

ppmModelAttr modelAttrs = {

    .versionString = PPM_VERSION_STRING,
    .type          = PPM_MT_PERIPHERAL,

    .busPortsCB    = nextBusPort,  
    .netPortsCB    = nextNetPort,  
    .paramSpecCB   = nextParameter,

    .vlnv          = {
        .vendor  = "xilinx.ovpworld.org",
        .library = "peripheral",
        .name    = "xps-intc",
        .version = "1.0"
    },

    .family    = "xilinx.ovpworld.org",           

};
