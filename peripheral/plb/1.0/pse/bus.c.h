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

#ifndef PLB_BUS_H
#define PLB_BUS_H
#include "peripheral/impTypes.h"
#include "peripheral/bhm.h"
#include "peripheral/ppm.h"

//
// Reorganize word for Big/Little Endian and correct size
//
Uns32 toLittleEndian (Uns32 val, Uns8 bytes) {
    Uns32 rval = 0;
    //
    // Swap Endian to Little
    //
    rval |= (val>>24) & 0x000000ff;
    rval |= (val>>8)  & 0x0000ff00;
    rval |= (val<<8)  & 0x00ff0000;
    rval |= (val<<24) & 0xff000000;
    //
    // Mask upper bytes
    //
    Uns32 mask = -1 >> (bytes * 8);
    rval &= mask;
    return rval;
}

Uns32 toBigEndian (Uns32 val, Uns8 bytes) {
    Uns32 rval = 0;
    //
    // Swap Endian to Big
    //
    rval |= (val<<24) & 0xff000000;
    rval |= (val<<8)  & 0x00ff0000;
    rval |= (val>>8)  & 0x0000ff00;
    rval |= (val>>24) & 0x000000ff;
    //
    // Mask upper bytes
    //
    Uns32 mask = -1 << (bytes * 8);
    rval &= mask;
    return rval;
}

#define GETLE(_v) toLittleEndian(_v, bytes)
#define GETBE(_v) toBigEndian(_v, bytes)

#endif
