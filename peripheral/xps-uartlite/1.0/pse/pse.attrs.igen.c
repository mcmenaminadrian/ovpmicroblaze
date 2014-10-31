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
        .addrHi          = 0xfLL,
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
        .name            = "Interrupt",
        .type            = PPM_OUTPUT_PORT,
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
    {
        .name        = "console",
        .type        = ppm_PT_BOOL,
        .description = "Standard Serial Socket Parameter: See OVP BHM and PPM API Function Reference:     Automatic console",
    },
    {
        .name        = "portnum",
        .type        = ppm_PT_UNS64,
        .description = "Standard Serial Socket Parameter: See OVP BHM and PPM API Function Reference:       Specify port to open for a connection.      A value of zero causes the OS to select the next available port.",
    },
    {
        .name        = "infile",
        .type        = ppm_PT_STRING,
        .description = "Standard Serial Socket Parameter: See OVP BHM and PPM API Function Reference:      UART takes input from this serial input source file",
    },
    {
        .name        = "outfile",
        .type        = ppm_PT_STRING,
        .description = "Standard Serial Socket Parameter: See OVP BHM and PPM API Function Reference:      Serial output file",
    },
    {
        .name        = "portFile",
        .type        = ppm_PT_STRING,
        .description = "Standard Serial Socket Parameter: See OVP BHM and PPM API Function Reference:      When portnum is set to zero, write the assigned port number to this file",
    },
    {
        .name        = "log",
        .type        = ppm_PT_BOOL,
        .description = "Standard Serial Socket Parameter: See OVP BHM and PPM API Function Reference:      Report serial output in the simulator log",
    },
    {
        .name        = "finishOnDisconnect",
        .type        = ppm_PT_BOOL,
        .description = "Standard Serial Socket Parameter: See OVP BHM and PPM API Function Reference:      When defined the simulation will be terminated if the port is disconnected",
    },
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
        .name    = "xps-uartlite",
        .version = "1.0"
    },

    .family    = "xilinx.ovpworld.org",           

};
