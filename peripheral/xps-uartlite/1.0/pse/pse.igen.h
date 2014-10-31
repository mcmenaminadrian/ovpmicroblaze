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
    } RxFifo;
    union { 
        Uns32 value;
    } TxFifo;
    union { 
        Uns32 value;
        struct {
            unsigned RxFifoValidData : 1;
            unsigned RxFifoFull : 1;
            unsigned TxFifoEmpty : 1;
            unsigned TxFifoFull : 1;
            unsigned IntrEnabled : 1;
            unsigned OverrunError : 1;
            unsigned FrameError : 1;
            unsigned ParityError : 1;
        } bits;
    } Stat;
    union { 
        Uns32 value;
        struct {
            unsigned RstTxFifo : 1;
            unsigned RstRxFifo : 1;
            unsigned __pad2 : 2;
            unsigned EnableIntr : 1;
        } bits;
    } Ctrl;
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

PPM_REG_READ_CB(ReadRxFifo);
PPM_REG_READ_CB(ReadStat);
PPM_REG_WRITE_CB(WriteCtrl);
PPM_REG_WRITE_CB(WriteTxFifo);
PPM_NET_CB(reset_resetNet);
PPM_CONSTRUCTOR_CB(periphConstructor);
PPM_DESTRUCTOR_CB(periphDestructor);
PPM_CONSTRUCTOR_CB(constructor);
PPM_DESTRUCTOR_CB(destructor);

#endif

#endif
