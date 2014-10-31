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

#ifndef MICROBLAZE_PORTS_H
#define MICROBLAZE_PORTS_H

#include "microblazeTypeRefs.h"

void microblazeNewNetPorts(microblazeP microblaze);
void microblazeFreeNetPorts(microblazeP microblaze);
VMI_NET_PORT_SPECS_FN(microblazeNetPortSpecs);
#endif /* MICROBLAZE_PORTS_H */
