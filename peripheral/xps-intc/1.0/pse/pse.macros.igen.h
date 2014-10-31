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
////////////////////////////////// Description /////////////////////////////////

// Microblaze LogiCORE IP XPS Interrupt Controller

/////////////////////////////////// Licensing //////////////////////////////////

// Open Source Apache 2.0

////////////////////////////////// Limitations /////////////////////////////////

// This model implements all of the required behavior sufficient to boot Linux

// Before including this file in the application, define the indicated macros
// to fix the base address of each slave port.
// Set the macro 'PLB' to the base of port 'plb'
#ifndef PLB
    #error PLB is undefined.It needs to be set to the port base address
#endif
#define PLB_REG_ISR    (PLB + 0x0)

#define PLB_REG_ISR_INT0   0x1
#define PLB_REG_ISR_INT1   (0x1 << 1)
#define PLB_REG_ISR_INT2   (0x1 << 2)
#define PLB_REG_ISR_INT3   (0x1 << 3)
#define PLB_REG_ISR_INT4   (0x1 << 4)
#define PLB_REG_ISR_INT5   (0x1 << 5)
#define PLB_REG_ISR_INT6   (0x1 << 6)
#define PLB_REG_ISR_INT7   (0x1 << 7)
#define PLB_REG_ISR_INT8   (0x1 << 8)
#define PLB_REG_ISR_INT9   (0x1 << 9)
#define PLB_REG_ISR_INT10   (0x1 << 10)
#define PLB_REG_ISR_INT11   (0x1 << 11)
#define PLB_REG_ISR_INT12   (0x1 << 12)
#define PLB_REG_ISR_INT13   (0x1 << 13)
#define PLB_REG_ISR_INT14   (0x1 << 14)
#define PLB_REG_ISR_INT15   (0x1 << 15)
#define PLB_REG_ISR_INT16   (0x1 << 16)
#define PLB_REG_ISR_INT17   (0x1 << 17)
#define PLB_REG_ISR_INT18   (0x1 << 18)
#define PLB_REG_ISR_INT19   (0x1 << 19)
#define PLB_REG_ISR_INT20   (0x1 << 20)
#define PLB_REG_ISR_INT21   (0x1 << 21)
#define PLB_REG_ISR_INT22   (0x1 << 22)
#define PLB_REG_ISR_INT23   (0x1 << 23)
#define PLB_REG_ISR_INT24   (0x1 << 24)
#define PLB_REG_ISR_INT25   (0x1 << 25)
#define PLB_REG_ISR_INT26   (0x1 << 26)
#define PLB_REG_ISR_INT27   (0x1 << 27)
#define PLB_REG_ISR_INT28   (0x1 << 28)
#define PLB_REG_ISR_INT29   (0x1 << 29)
#define PLB_REG_ISR_INT30   (0x1 << 30)
#define PLB_REG_ISR_INT31   (0x1 << 31)
#define PLB_REG_IPR    (PLB + 0x4)

#define PLB_REG_IPR_INT0   0x1
#define PLB_REG_IPR_INT1   (0x1 << 1)
#define PLB_REG_IPR_INT2   (0x1 << 2)
#define PLB_REG_IPR_INT3   (0x1 << 3)
#define PLB_REG_IPR_INT4   (0x1 << 4)
#define PLB_REG_IPR_INT5   (0x1 << 5)
#define PLB_REG_IPR_INT6   (0x1 << 6)
#define PLB_REG_IPR_INT7   (0x1 << 7)
#define PLB_REG_IPR_INT8   (0x1 << 8)
#define PLB_REG_IPR_INT9   (0x1 << 9)
#define PLB_REG_IPR_INT10   (0x1 << 10)
#define PLB_REG_IPR_INT11   (0x1 << 11)
#define PLB_REG_IPR_INT12   (0x1 << 12)
#define PLB_REG_IPR_INT13   (0x1 << 13)
#define PLB_REG_IPR_INT14   (0x1 << 14)
#define PLB_REG_IPR_INT15   (0x1 << 15)
#define PLB_REG_IPR_INT16   (0x1 << 16)
#define PLB_REG_IPR_INT17   (0x1 << 17)
#define PLB_REG_IPR_INT18   (0x1 << 18)
#define PLB_REG_IPR_INT19   (0x1 << 19)
#define PLB_REG_IPR_INT20   (0x1 << 20)
#define PLB_REG_IPR_INT21   (0x1 << 21)
#define PLB_REG_IPR_INT22   (0x1 << 22)
#define PLB_REG_IPR_INT23   (0x1 << 23)
#define PLB_REG_IPR_INT24   (0x1 << 24)
#define PLB_REG_IPR_INT25   (0x1 << 25)
#define PLB_REG_IPR_INT26   (0x1 << 26)
#define PLB_REG_IPR_INT27   (0x1 << 27)
#define PLB_REG_IPR_INT28   (0x1 << 28)
#define PLB_REG_IPR_INT29   (0x1 << 29)
#define PLB_REG_IPR_INT30   (0x1 << 30)
#define PLB_REG_IPR_INT31   (0x1 << 31)
#define PLB_REG_IER    (PLB + 0x8)

#define PLB_REG_IER_INT0   0x1
#define PLB_REG_IER_INT1   (0x1 << 1)
#define PLB_REG_IER_INT2   (0x1 << 2)
#define PLB_REG_IER_INT3   (0x1 << 3)
#define PLB_REG_IER_INT4   (0x1 << 4)
#define PLB_REG_IER_INT5   (0x1 << 5)
#define PLB_REG_IER_INT6   (0x1 << 6)
#define PLB_REG_IER_INT7   (0x1 << 7)
#define PLB_REG_IER_INT8   (0x1 << 8)
#define PLB_REG_IER_INT9   (0x1 << 9)
#define PLB_REG_IER_INT10   (0x1 << 10)
#define PLB_REG_IER_INT11   (0x1 << 11)
#define PLB_REG_IER_INT12   (0x1 << 12)
#define PLB_REG_IER_INT13   (0x1 << 13)
#define PLB_REG_IER_INT14   (0x1 << 14)
#define PLB_REG_IER_INT15   (0x1 << 15)
#define PLB_REG_IER_INT16   (0x1 << 16)
#define PLB_REG_IER_INT17   (0x1 << 17)
#define PLB_REG_IER_INT18   (0x1 << 18)
#define PLB_REG_IER_INT19   (0x1 << 19)
#define PLB_REG_IER_INT20   (0x1 << 20)
#define PLB_REG_IER_INT21   (0x1 << 21)
#define PLB_REG_IER_INT22   (0x1 << 22)
#define PLB_REG_IER_INT23   (0x1 << 23)
#define PLB_REG_IER_INT24   (0x1 << 24)
#define PLB_REG_IER_INT25   (0x1 << 25)
#define PLB_REG_IER_INT26   (0x1 << 26)
#define PLB_REG_IER_INT27   (0x1 << 27)
#define PLB_REG_IER_INT28   (0x1 << 28)
#define PLB_REG_IER_INT29   (0x1 << 29)
#define PLB_REG_IER_INT30   (0x1 << 30)
#define PLB_REG_IER_INT31   (0x1 << 31)
#define PLB_REG_IAR    (PLB + 0xc)

#define PLB_REG_IAR_INT0   0x1
#define PLB_REG_IAR_INT1   (0x1 << 1)
#define PLB_REG_IAR_INT2   (0x1 << 2)
#define PLB_REG_IAR_INT3   (0x1 << 3)
#define PLB_REG_IAR_INT4   (0x1 << 4)
#define PLB_REG_IAR_INT5   (0x1 << 5)
#define PLB_REG_IAR_INT6   (0x1 << 6)
#define PLB_REG_IAR_INT7   (0x1 << 7)
#define PLB_REG_IAR_INT8   (0x1 << 8)
#define PLB_REG_IAR_INT9   (0x1 << 9)
#define PLB_REG_IAR_INT10   (0x1 << 10)
#define PLB_REG_IAR_INT11   (0x1 << 11)
#define PLB_REG_IAR_INT12   (0x1 << 12)
#define PLB_REG_IAR_INT13   (0x1 << 13)
#define PLB_REG_IAR_INT14   (0x1 << 14)
#define PLB_REG_IAR_INT15   (0x1 << 15)
#define PLB_REG_IAR_INT16   (0x1 << 16)
#define PLB_REG_IAR_INT17   (0x1 << 17)
#define PLB_REG_IAR_INT18   (0x1 << 18)
#define PLB_REG_IAR_INT19   (0x1 << 19)
#define PLB_REG_IAR_INT20   (0x1 << 20)
#define PLB_REG_IAR_INT21   (0x1 << 21)
#define PLB_REG_IAR_INT22   (0x1 << 22)
#define PLB_REG_IAR_INT23   (0x1 << 23)
#define PLB_REG_IAR_INT24   (0x1 << 24)
#define PLB_REG_IAR_INT25   (0x1 << 25)
#define PLB_REG_IAR_INT26   (0x1 << 26)
#define PLB_REG_IAR_INT27   (0x1 << 27)
#define PLB_REG_IAR_INT28   (0x1 << 28)
#define PLB_REG_IAR_INT29   (0x1 << 29)
#define PLB_REG_IAR_INT30   (0x1 << 30)
#define PLB_REG_IAR_INT31   (0x1 << 31)
#define PLB_REG_SIE    (PLB + 0x10)

#define PLB_REG_SIE_INT0   0x1
#define PLB_REG_SIE_INT1   (0x1 << 1)
#define PLB_REG_SIE_INT2   (0x1 << 2)
#define PLB_REG_SIE_INT3   (0x1 << 3)
#define PLB_REG_SIE_INT4   (0x1 << 4)
#define PLB_REG_SIE_INT5   (0x1 << 5)
#define PLB_REG_SIE_INT6   (0x1 << 6)
#define PLB_REG_SIE_INT7   (0x1 << 7)
#define PLB_REG_SIE_INT8   (0x1 << 8)
#define PLB_REG_SIE_INT9   (0x1 << 9)
#define PLB_REG_SIE_INT10   (0x1 << 10)
#define PLB_REG_SIE_INT11   (0x1 << 11)
#define PLB_REG_SIE_INT12   (0x1 << 12)
#define PLB_REG_SIE_INT13   (0x1 << 13)
#define PLB_REG_SIE_INT14   (0x1 << 14)
#define PLB_REG_SIE_INT15   (0x1 << 15)
#define PLB_REG_SIE_INT16   (0x1 << 16)
#define PLB_REG_SIE_INT17   (0x1 << 17)
#define PLB_REG_SIE_INT18   (0x1 << 18)
#define PLB_REG_SIE_INT19   (0x1 << 19)
#define PLB_REG_SIE_INT20   (0x1 << 20)
#define PLB_REG_SIE_INT21   (0x1 << 21)
#define PLB_REG_SIE_INT22   (0x1 << 22)
#define PLB_REG_SIE_INT23   (0x1 << 23)
#define PLB_REG_SIE_INT24   (0x1 << 24)
#define PLB_REG_SIE_INT25   (0x1 << 25)
#define PLB_REG_SIE_INT26   (0x1 << 26)
#define PLB_REG_SIE_INT27   (0x1 << 27)
#define PLB_REG_SIE_INT28   (0x1 << 28)
#define PLB_REG_SIE_INT29   (0x1 << 29)
#define PLB_REG_SIE_INT30   (0x1 << 30)
#define PLB_REG_SIE_INT31   (0x1 << 31)
#define PLB_REG_CIE    (PLB + 0x14)

#define PLB_REG_CIE_INT0   0x1
#define PLB_REG_CIE_INT1   (0x1 << 1)
#define PLB_REG_CIE_INT2   (0x1 << 2)
#define PLB_REG_CIE_INT3   (0x1 << 3)
#define PLB_REG_CIE_INT4   (0x1 << 4)
#define PLB_REG_CIE_INT5   (0x1 << 5)
#define PLB_REG_CIE_INT6   (0x1 << 6)
#define PLB_REG_CIE_INT7   (0x1 << 7)
#define PLB_REG_CIE_INT8   (0x1 << 8)
#define PLB_REG_CIE_INT9   (0x1 << 9)
#define PLB_REG_CIE_INT10   (0x1 << 10)
#define PLB_REG_CIE_INT11   (0x1 << 11)
#define PLB_REG_CIE_INT12   (0x1 << 12)
#define PLB_REG_CIE_INT13   (0x1 << 13)
#define PLB_REG_CIE_INT14   (0x1 << 14)
#define PLB_REG_CIE_INT15   (0x1 << 15)
#define PLB_REG_CIE_INT16   (0x1 << 16)
#define PLB_REG_CIE_INT17   (0x1 << 17)
#define PLB_REG_CIE_INT18   (0x1 << 18)
#define PLB_REG_CIE_INT19   (0x1 << 19)
#define PLB_REG_CIE_INT20   (0x1 << 20)
#define PLB_REG_CIE_INT21   (0x1 << 21)
#define PLB_REG_CIE_INT22   (0x1 << 22)
#define PLB_REG_CIE_INT23   (0x1 << 23)
#define PLB_REG_CIE_INT24   (0x1 << 24)
#define PLB_REG_CIE_INT25   (0x1 << 25)
#define PLB_REG_CIE_INT26   (0x1 << 26)
#define PLB_REG_CIE_INT27   (0x1 << 27)
#define PLB_REG_CIE_INT28   (0x1 << 28)
#define PLB_REG_CIE_INT29   (0x1 << 29)
#define PLB_REG_CIE_INT30   (0x1 << 30)
#define PLB_REG_CIE_INT31   (0x1 << 31)
#define PLB_REG_IVR    (PLB + 0x18)

#define PLB_REG_MER    (PLB + 0x1c)

#define PLB_REG_MER_ME   0x1
#define PLB_REG_MER_HIE   (0x1 << 1)


#endif
