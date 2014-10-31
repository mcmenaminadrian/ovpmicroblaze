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

#ifndef MICROBLAZE_FUNCTIONS_H
#define MICROBLAZE_FUNCTIONS_H
#include "vmi/vmiAttrs.h"

// save/restore functions
VMI_SAVE_STATE_FN(microblazeSaveStateCB);
VMI_RESTORE_STATE_FN(microblazeRestoreStateCB);

// constructor & destructor
VMI_CONSTRUCTOR_FN(microblazeConstructor);
VMI_VMINIT_FN(microblazeVMInit);
VMI_DESTRUCTOR_FN(microblazeDestructor);

// morph function
VMI_MORPH_FN(microblazeMorph);
VMI_FETCH_SNAP_FN(microblazeFetchSnap);

// simulation support functions
VMI_ENDIAN_FN(microblazeGetEndian);
VMI_NEXT_PC_FN(microblazenextPC);
VMI_DISASSEMBLE_FN(microblazeDis);
VMI_IASSWITCH_FN(microblazeSwitch);

// debugger integration support routines
VMI_REG_GROUP_FN(microblazeRegGroup);
VMI_REG_INFO_FN(microblazeRegInfo);
VMI_EXCEPTION_INFO_FN(microblazeExceptionInfo);
VMI_MODE_INFO_FN(microblazeModeInfo);
VMI_GET_EXCEPTION_FN(microblazeGetException);
VMI_GET_MODE_FN(microblazeGetMode);
VMI_DEBUG_FN(microblazeDebug);

// exception functions
VMI_RD_PRIV_EXCEPT_FN(microblazeRdPrivExcept);
VMI_WR_PRIV_EXCEPT_FN(microblazeWrPrivExcept);
VMI_RD_ALIGN_EXCEPT_FN(microblazeRdAlignExcept);
VMI_WR_ALIGN_EXCEPT_FN(microblazeWrAlignExcept);
VMI_RD_ABORT_EXCEPT_FN(microblazeRdAbortExcept);
VMI_WR_ABORT_EXCEPT_FN(microblazeWrAbortExcept);
VMI_IFETCH_FN(microblazeIfetchExcept);
VMI_ARITH_EXCEPT_FN(microblazeArithExcept);
VMI_FP_ARITH_EXCEPT_FN(microblazeFPArithExcept);
VMI_ICOUNT_FN(microblazeIcountExcept);

// parameter support functions
VMI_PROC_PARAM_SPECS_FN(microblazeParamSpecs);
VMI_PROC_PARAM_TABLE_SIZE_FN(microblazeParamValueSize);

// port functions
VMI_BUS_PORT_SPECS_FN(microblazeBusPortSpecs);
VMI_NET_PORT_SPECS_FN(microblazeNetPortSpecs);
VMI_FIFO_PORT_SPECS_FN(microblazeFifoPortSpecs);

// Imperas intercepted function support
VMI_INT_RETURN_FN(microblazeIntReturn);
VMI_INT_RESULT_FN(microblazeIntResult);
VMI_INT_PAR_FN(microblazeIntPar);

// Processor information support
VMI_PROC_INFO_FN(microblazeProcInfo);
#endif // MICROBLAZE_FUNCTIONS_H
