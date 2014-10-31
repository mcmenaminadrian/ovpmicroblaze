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

#ifndef PSE_MACROS_IGEN_H
#define PSE_MACROS_IGEN_H
/////////////////////////////////// Licensing //////////////////////////////////

// Open Source Apache 2.0

// Before including this file in the application, define the indicated macros
// to fix the base address of each slave port.
// Set the macro 'PLB' to the base of port 'plb'
#ifndef PLB
    #error PLB is undefined.It needs to be set to the port base address
#endif
#define PLB_REG_RXFIFO    (PLB + 0x0)

#define PLB_REG_TXFIFO    (PLB + 0x4)

#define PLB_REG_STAT    (PLB + 0x8)

#define PLB_REG_STAT_RXFIFOVALIDDATA   0x1
#define PLB_REG_STAT_RXFIFOFULL   (0x1 << 1)
#define PLB_REG_STAT_TXFIFOEMPTY   (0x1 << 2)
#define PLB_REG_STAT_TXFIFOFULL   (0x1 << 3)
#define PLB_REG_STAT_INTRENABLED   (0x1 << 4)
#define PLB_REG_STAT_OVERRUNERROR   (0x1 << 5)
#define PLB_REG_STAT_FRAMEERROR   (0x1 << 6)
#define PLB_REG_STAT_PARITYERROR   (0x1 << 7)
#define PLB_REG_CTRL    (PLB + 0xc)

#define PLB_REG_CTRL_RSTTXFIFO   0x1
#define PLB_REG_CTRL_RSTRXFIFO   (0x1 << 1)
#define PLB_REG_CTRL_ENABLEINTR   (0x1 << 4)


#endif
