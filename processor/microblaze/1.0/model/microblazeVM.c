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

#include "vmi/vmiAttrs.h"
#include "vmi/vmiMessage.h"
#include "vmi/vmiRt.h"
#include "vmi/vmiTypes.h"

#include "microblazeStructure.h"
#include "microblazeVM.h"

VMI_VMINIT_FN(microblazeVMInit) {

    microblazeP microblaze = (microblazeP)processor;

    microblaze->pDomain = dataDomains[0];
    microblaze->vDomain[VM_MODE_VIRTUAL_PRIV] = vmirtNewDomain("VIRTUAL PRIV", 32);
    microblaze->vDomain[VM_MODE_VIRTUAL_USER] = vmirtNewDomain("VIRTUAL USER", 32);

    codeDomains[VM_MODE_VIRTUAL_PRIV] = microblaze->vDomain[VM_MODE_VIRTUAL_PRIV];
    dataDomains[VM_MODE_VIRTUAL_PRIV] = microblaze->vDomain[VM_MODE_VIRTUAL_PRIV];
    codeDomains[VM_MODE_VIRTUAL_USER] = microblaze->vDomain[VM_MODE_VIRTUAL_USER];
    dataDomains[VM_MODE_VIRTUAL_USER] = microblaze->vDomain[VM_MODE_VIRTUAL_USER];
}

