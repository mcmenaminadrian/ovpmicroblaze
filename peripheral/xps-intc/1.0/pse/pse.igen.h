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
            unsigned INT0 : 1;
            unsigned INT1 : 1;
            unsigned INT2 : 1;
            unsigned INT3 : 1;
            unsigned INT4 : 1;
            unsigned INT5 : 1;
            unsigned INT6 : 1;
            unsigned INT7 : 1;
            unsigned INT8 : 1;
            unsigned INT9 : 1;
            unsigned INT10 : 1;
            unsigned INT11 : 1;
            unsigned INT12 : 1;
            unsigned INT13 : 1;
            unsigned INT14 : 1;
            unsigned INT15 : 1;
            unsigned INT16 : 1;
            unsigned INT17 : 1;
            unsigned INT18 : 1;
            unsigned INT19 : 1;
            unsigned INT20 : 1;
            unsigned INT21 : 1;
            unsigned INT22 : 1;
            unsigned INT23 : 1;
            unsigned INT24 : 1;
            unsigned INT25 : 1;
            unsigned INT26 : 1;
            unsigned INT27 : 1;
            unsigned INT28 : 1;
            unsigned INT29 : 1;
            unsigned INT30 : 1;
            unsigned INT31 : 1;
        } bits;
    } ISR;
    union { 
        Uns32 value;
        struct {
            unsigned INT0 : 1;
            unsigned INT1 : 1;
            unsigned INT2 : 1;
            unsigned INT3 : 1;
            unsigned INT4 : 1;
            unsigned INT5 : 1;
            unsigned INT6 : 1;
            unsigned INT7 : 1;
            unsigned INT8 : 1;
            unsigned INT9 : 1;
            unsigned INT10 : 1;
            unsigned INT11 : 1;
            unsigned INT12 : 1;
            unsigned INT13 : 1;
            unsigned INT14 : 1;
            unsigned INT15 : 1;
            unsigned INT16 : 1;
            unsigned INT17 : 1;
            unsigned INT18 : 1;
            unsigned INT19 : 1;
            unsigned INT20 : 1;
            unsigned INT21 : 1;
            unsigned INT22 : 1;
            unsigned INT23 : 1;
            unsigned INT24 : 1;
            unsigned INT25 : 1;
            unsigned INT26 : 1;
            unsigned INT27 : 1;
            unsigned INT28 : 1;
            unsigned INT29 : 1;
            unsigned INT30 : 1;
            unsigned INT31 : 1;
        } bits;
    } IPR;
    union { 
        Uns32 value;
        struct {
            unsigned INT0 : 1;
            unsigned INT1 : 1;
            unsigned INT2 : 1;
            unsigned INT3 : 1;
            unsigned INT4 : 1;
            unsigned INT5 : 1;
            unsigned INT6 : 1;
            unsigned INT7 : 1;
            unsigned INT8 : 1;
            unsigned INT9 : 1;
            unsigned INT10 : 1;
            unsigned INT11 : 1;
            unsigned INT12 : 1;
            unsigned INT13 : 1;
            unsigned INT14 : 1;
            unsigned INT15 : 1;
            unsigned INT16 : 1;
            unsigned INT17 : 1;
            unsigned INT18 : 1;
            unsigned INT19 : 1;
            unsigned INT20 : 1;
            unsigned INT21 : 1;
            unsigned INT22 : 1;
            unsigned INT23 : 1;
            unsigned INT24 : 1;
            unsigned INT25 : 1;
            unsigned INT26 : 1;
            unsigned INT27 : 1;
            unsigned INT28 : 1;
            unsigned INT29 : 1;
            unsigned INT30 : 1;
            unsigned INT31 : 1;
        } bits;
    } IER;
    union { 
        Uns32 value;
        struct {
            unsigned INT0 : 1;
            unsigned INT1 : 1;
            unsigned INT2 : 1;
            unsigned INT3 : 1;
            unsigned INT4 : 1;
            unsigned INT5 : 1;
            unsigned INT6 : 1;
            unsigned INT7 : 1;
            unsigned INT8 : 1;
            unsigned INT9 : 1;
            unsigned INT10 : 1;
            unsigned INT11 : 1;
            unsigned INT12 : 1;
            unsigned INT13 : 1;
            unsigned INT14 : 1;
            unsigned INT15 : 1;
            unsigned INT16 : 1;
            unsigned INT17 : 1;
            unsigned INT18 : 1;
            unsigned INT19 : 1;
            unsigned INT20 : 1;
            unsigned INT21 : 1;
            unsigned INT22 : 1;
            unsigned INT23 : 1;
            unsigned INT24 : 1;
            unsigned INT25 : 1;
            unsigned INT26 : 1;
            unsigned INT27 : 1;
            unsigned INT28 : 1;
            unsigned INT29 : 1;
            unsigned INT30 : 1;
            unsigned INT31 : 1;
        } bits;
    } IAR;
    union { 
        Uns32 value;
        struct {
            unsigned INT0 : 1;
            unsigned INT1 : 1;
            unsigned INT2 : 1;
            unsigned INT3 : 1;
            unsigned INT4 : 1;
            unsigned INT5 : 1;
            unsigned INT6 : 1;
            unsigned INT7 : 1;
            unsigned INT8 : 1;
            unsigned INT9 : 1;
            unsigned INT10 : 1;
            unsigned INT11 : 1;
            unsigned INT12 : 1;
            unsigned INT13 : 1;
            unsigned INT14 : 1;
            unsigned INT15 : 1;
            unsigned INT16 : 1;
            unsigned INT17 : 1;
            unsigned INT18 : 1;
            unsigned INT19 : 1;
            unsigned INT20 : 1;
            unsigned INT21 : 1;
            unsigned INT22 : 1;
            unsigned INT23 : 1;
            unsigned INT24 : 1;
            unsigned INT25 : 1;
            unsigned INT26 : 1;
            unsigned INT27 : 1;
            unsigned INT28 : 1;
            unsigned INT29 : 1;
            unsigned INT30 : 1;
            unsigned INT31 : 1;
        } bits;
    } SIE;
    union { 
        Uns32 value;
        struct {
            unsigned INT0 : 1;
            unsigned INT1 : 1;
            unsigned INT2 : 1;
            unsigned INT3 : 1;
            unsigned INT4 : 1;
            unsigned INT5 : 1;
            unsigned INT6 : 1;
            unsigned INT7 : 1;
            unsigned INT8 : 1;
            unsigned INT9 : 1;
            unsigned INT10 : 1;
            unsigned INT11 : 1;
            unsigned INT12 : 1;
            unsigned INT13 : 1;
            unsigned INT14 : 1;
            unsigned INT15 : 1;
            unsigned INT16 : 1;
            unsigned INT17 : 1;
            unsigned INT18 : 1;
            unsigned INT19 : 1;
            unsigned INT20 : 1;
            unsigned INT21 : 1;
            unsigned INT22 : 1;
            unsigned INT23 : 1;
            unsigned INT24 : 1;
            unsigned INT25 : 1;
            unsigned INT26 : 1;
            unsigned INT27 : 1;
            unsigned INT28 : 1;
            unsigned INT29 : 1;
            unsigned INT30 : 1;
            unsigned INT31 : 1;
        } bits;
    } CIE;
    union { 
        Uns32 value;
    } IVR;
    union { 
        Uns32 value;
        struct {
            unsigned ME : 1;
            unsigned HIE : 1;
        } bits;
    } MER;
} plb_REG_dataT, *plb_REG_dataTP;

/////////////////////////////// Port Declarations //////////////////////////////

extern plb_REG_dataT plb_REG_data;

#ifdef _PSE_
///////////////////////////////// Port handles /////////////////////////////////

typedef struct handlesS {
    void                 *plb;
    ppmNetHandle          Irq;
    ppmNetHandle          Intr0;
    ppmNetHandle          Intr1;
    ppmNetHandle          Intr2;
    ppmNetHandle          Intr3;
    ppmNetHandle          Intr4;
    ppmNetHandle          Intr5;
    ppmNetHandle          Intr6;
    ppmNetHandle          Intr7;
    ppmNetHandle          Intr8;
    ppmNetHandle          Intr9;
    ppmNetHandle          Intr10;
    ppmNetHandle          Intr11;
    ppmNetHandle          Intr12;
    ppmNetHandle          Intr13;
    ppmNetHandle          Intr14;
    ppmNetHandle          Intr15;
    ppmNetHandle          Intr16;
    ppmNetHandle          Intr17;
    ppmNetHandle          Intr18;
    ppmNetHandle          Intr19;
    ppmNetHandle          Intr20;
    ppmNetHandle          Intr21;
    ppmNetHandle          Intr22;
    ppmNetHandle          Intr23;
    ppmNetHandle          Intr24;
    ppmNetHandle          Intr25;
    ppmNetHandle          Intr26;
    ppmNetHandle          Intr27;
    ppmNetHandle          Intr28;
    ppmNetHandle          Intr29;
    ppmNetHandle          Intr30;
    ppmNetHandle          Intr31;
} handlesT, *handlesTP;

extern handlesT handles;

////////////////////////////// Callback prototypes /////////////////////////////

PPM_REG_READ_CB(ReadIER);
PPM_REG_READ_CB(ReadIPR);
PPM_REG_READ_CB(ReadISR);
PPM_REG_READ_CB(ReadIVR);
PPM_REG_READ_CB(ReadMER);
PPM_REG_WRITE_CB(WriteCIE);
PPM_REG_WRITE_CB(WriteIAR);
PPM_REG_WRITE_CB(WriteIER);
PPM_REG_WRITE_CB(WriteISR);
PPM_NET_CB(WriteIntr);
PPM_REG_WRITE_CB(WriteMER);
PPM_REG_WRITE_CB(WriteSIE);
PPM_NET_CB(reset_resetNet);
PPM_CONSTRUCTOR_CB(periphConstructor);
PPM_DESTRUCTOR_CB(periphDestructor);
PPM_CONSTRUCTOR_CB(constructor);
PPM_DESTRUCTOR_CB(destructor);

#endif

#endif
