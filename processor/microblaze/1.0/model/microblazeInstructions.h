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

#ifndef MICROBLAZE_INSTRUCTIONS_H
#define MICROBLAZE_INSTRUCTIONS_H

//
// Macros to perform shift, mask, sign/zero extend and bit extraction
//

// mask out _VEC from _HI and above
#define MASK(_VEC, _HI) (((Uns64)_VEC) & ((1<<(_HI))-1))

// strip _VEC, _HI downto _LO, and position at _ORIGIN
#define VECINDEX(_VEC,_HI,_LO,_ORIGIN) (((Uns64) -1 >> (63 - (_HI) + (_LO))) & (((Uns64)_VEC) >> (_LO))) << _ORIGIN

// Sign extend _VEC, from _POS all the way to left
#define SEXTEND(_VEC,_POS) ((Int64)(0x8000000000000000ULL & (((Uns64)_VEC) << (63 - (_POS)))) >> (63 - (_POS))) | ((Uns64)_VEC)

// ZERO extend _VEC, after _POS all the way left
#define ZERO(_VEC,_POS) ((Uns64)(((Uns64)_VEC) << (63 - (_POS))) >> (31 - (_POS)))

// SHIFTL _VEC _SHIFT
#define SHIFTL(_VEC,_SHIFT) ((Uns64)(((Uns64)_VEC) << (_SHIFT)))

// SHIFTR _VEC _SHIFT
#define SHIFTR(_VEC,_SHIFT) ((Uns64)(((Uns64)_VEC) >> (_SHIFT)))

#endif // MICROBLAZE_INSTRUCTIONS_H
