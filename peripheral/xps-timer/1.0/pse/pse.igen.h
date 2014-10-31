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

#ifndef PSE_IGEN_H
#define PSE_IGEN_H

#ifdef _PSE_
#    include "peripheral/impTypes.h"
#    include "peripheral/bhm.h"
#    include "peripheral/ppm.h"
#else
#    include "hostapi/impTypes.h"
#endif

//////////////////////////////////// Externs ///////////////////////////////////

    extern Uns32 diagnosticLevel;

/////////////////////////// Register data declaration //////////////////////////

typedef struct plb_REG_dataS { 
    union { 
        Uns32 value;
        struct {
            unsigned MDT0 : 1;
            unsigned UDT0 : 1;
            unsigned GENT0 : 1;
            unsigned CAPT0 : 1;
            unsigned ARHT0 : 1;
            unsigned LOAD0 : 1;
            unsigned ENIT0 : 1;
            unsigned ENT0 : 1;
            unsigned T0INT : 1;
            unsigned PWMA0 : 1;
            unsigned ENALL : 1;
        } bits;
    } TCSR0;
    union { 
        Uns32 value;
    } TLR0;
    union { 
        Uns32 value;
    } TCR0;
    union { 
        Uns32 value;
        struct {
            unsigned MDT1 : 1;
            unsigned UDT1 : 1;
            unsigned GENT1 : 1;
            unsigned CAPT1 : 1;
            unsigned ARHT1 : 1;
            unsigned LOAD1 : 1;
            unsigned ENIT1 : 1;
            unsigned ENT1 : 1;
            unsigned T1INT : 1;
            unsigned PWMA1 : 1;
            unsigned ENALL : 1;
        } bits;
    } TCSR1;
    union { 
        Uns32 value;
    } TLR1;
    union { 
        Uns32 value;
    } TCR1;
} plb_REG_dataT, *plb_REG_dataTP;

/////////////////////////////// Port Declarations //////////////////////////////

extern plb_REG_dataT plb_REG_data;

#ifdef _PSE_
///////////////////////////////// Port handles /////////////////////////////////

typedef struct handlesS {
    void                 *plb;
    ppmNetHandle          Interrupt;
} handlesT, *handlesTP;

extern handlesT handles;

////////////////////////////// Callback prototypes /////////////////////////////

PPM_REG_READ_CB(ReadTCR0);
PPM_REG_READ_CB(ReadTCR1);
PPM_REG_READ_CB(ReadTCSR0);
PPM_REG_READ_CB(ReadTCSR1);
PPM_REG_READ_CB(ReadTLR0);
PPM_REG_READ_CB(ReadTLR1);
PPM_REG_WRITE_CB(WriteTCSR0);
PPM_REG_WRITE_CB(WriteTCSR1);
PPM_REG_WRITE_CB(WriteTLR0);
PPM_REG_WRITE_CB(WriteTLR1);
PPM_NET_CB(reset_resetNet);
PPM_CONSTRUCTOR_CB(periphConstructor);
PPM_DESTRUCTOR_CB(periphDestructor);
PPM_CONSTRUCTOR_CB(constructor);
PPM_DESTRUCTOR_CB(destructor);

#endif

#endif
