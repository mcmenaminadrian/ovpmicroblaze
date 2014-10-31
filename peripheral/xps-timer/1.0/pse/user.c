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
//                              Version 99999999
//                          Fri Jan  6 10:06:43 2012
//
////////////////////////////////////////////////////////////////////////////////

#include "pse.igen.h"
#include "bus.c.h"

#define PREFIX "xps-timer"

struct {
    Uns32 Interrupt;
    Uns32 Int[2];
} state;

//
// Shortcut for loading the TCSR[0:1] registers
//
union {
    Uns32 value;
    struct {
        unsigned MDT   : 1;
        unsigned UDT   : 1;
        unsigned GENT  : 1;
        unsigned CAPT  : 1;
        unsigned ARHT  : 1;
        unsigned LOAD  : 1;
        unsigned ENIT  : 1;
        unsigned ENT   : 1;
        unsigned TINT  : 1;
        unsigned PWMA  : 1;
        unsigned ENALL : 1;
    } bits;
} TCSR;

#define  STACKSIZE (1024 *32)

typedef struct threadInfoS {
    bhmThreadHandle handle;
    Uns64 timeout;
    double start;
    Uns8 stack[STACKSIZE];
} threadInfoT;
threadInfoT tinfo[2];

void interruptEvaluate();
void interruptSet(Uns32 value);

void timerEnable(Uns32 tmr, Bool changed);
void timerStart(Uns32 tmr, Uns64 timeout);
void timerStop(Uns32 tmr);

#define DEFAULT_FREQ_MHZ 125
float freq_mhz = DEFAULT_FREQ_MHZ;

//
// Write the Interrupt if the value changes
//
void interruptSet(Uns32 value) {
    if (state.Interrupt != value) {
        state.Interrupt = value;
        if (BHM_DIAG_HIGH) bhmMessage("I", PREFIX, "Setting Interrupt=%d %e\n", state.Interrupt, bhmGetCurrentTime());
        ppmWriteNet(handles.Interrupt, state.Interrupt);
    }
}

void getTmrStatus(Uns32 tmr) {
    if (tmr==0) {
        TCSR.value = plb_REG_data.TCSR0.value;
    } else {
        TCSR.value = plb_REG_data.TCSR1.value;
    }
}

void putTmrStatus(Uns32 tmr) {
    if (tmr==0) {
        plb_REG_data.TCSR0.value = TCSR.value;
    } else {
        plb_REG_data.TCSR1.value = TCSR.value;
    }
}

void interruptEvaluate()
{
    Uns32 irq = 0;

    irq |= plb_REG_data.TCSR0.bits.T0INT && plb_REG_data.TCSR0.bits.ENIT0;
    irq |= plb_REG_data.TCSR1.bits.T1INT && plb_REG_data.TCSR1.bits.ENIT1;

    if (BHM_DIAG_HIGH) bhmMessage("I", PREFIX, "interruptEvaluate irq=%d\n", irq);

    if (irq) {
        //
        // Generate timeout interrupt
        //
        interruptSet(1);
        interruptSet(0);
    }
}

void timerExpired(Uns32 tmr) {
    if (BHM_DIAG_HIGH) {
        double now = bhmGetCurrentTime();
        bhmMessage("I", PREFIX, "timerExpired tmr=%d time=%e\n", tmr, now);
    }

    getTmrStatus(tmr);

    //
    // Interrupt is active
    //
    TCSR.bits.TINT = 1;
    putTmrStatus(tmr);
    interruptEvaluate();
}

Uns32 getTimeout (Uns32 tmr) {
    Uns32 count;
    Uns32 TLR = (tmr == 0) ? plb_REG_data.TLR0.value : plb_REG_data.TLR1.value;

    if (TCSR.bits.UDT) {
        // Counting up
        count = TLR;
    } else {
        // Counting down
        count = ~0 - TLR;
    }
    return count;
}

void timerEnable(Uns32 tmr, Bool changed) {
    getTmrStatus(tmr);

    //
    // Some asserts
    //
    if (TCSR.bits.MDT) {
        bhmMessage("F", PREFIX, "Unsupported Mode Capture");
    }
    if (TCSR.bits.PWMA) {
        bhmMessage("F", PREFIX, "Unsupported Mode PWM");
    }

    //
    // Now setup the timer
    //
    if (changed || !TCSR.bits.ARHT) {
        timerStop(tmr);
        timerStart(tmr, getTimeout(tmr));
    }
}

void timerThread(void *user) {
    Uns32 tmr = (Uns32)user;
    getTmrStatus(tmr);

    double now = bhmGetCurrentTime();

    Uns64 timeout = tinfo[tmr].timeout;

    do {
        if (BHM_DIAG_HIGH) bhmMessage("I", PREFIX, "Start Timer_%d Begin at %e timeout=%lld US", tmr, now, timeout);

        tinfo[tmr].start = bhmGetCurrentTime();
        bhmWaitDelay(timeout);

        if (BHM_DIAG_HIGH) bhmMessage("I", PREFIX, "End Timer_%d Expired at %e", tmr, bhmGetCurrentTime());

        timerExpired(tmr);

    } while (TCSR.bits.ARHT);
}

void timerStop(Uns32 tmr) {
    static int cnt=0;
    if (tinfo[tmr].handle) {
        cnt++;
        if (BHM_DIAG_HIGH) bhmMessage("I", PREFIX, "Delete Timer_%d Thread cnt=%d", tmr, cnt);
        bhmDeleteThread(tinfo[tmr].handle);
        tinfo[tmr].handle = 0;
    }
}

void timerStart(Uns32 tmr, Uns64 timeout) {
    const char *tn = tmr==0 ? "timer0" : "timer1";

    tinfo[tmr].timeout = timeout / freq_mhz;
    if (BHM_DIAG_HIGH) {
        bhmMessage("I", PREFIX, "tinfo[%d].timeout = %lld", tmr, tinfo[tmr].timeout);
        bhmMessage("I", PREFIX, "Create Timer_%d Thread", tmr);
    }

    tinfo[tmr].handle = bhmCreateThread(timerThread, (void*)tmr, tn, &(tinfo[tmr].stack[STACKSIZE]));
}

Uns32 clocksRemaining (Uns32 tmr) {
    getTmrStatus(tmr);

    double now     = bhmGetCurrentTime();
    double start   = tinfo[tmr].start;
    double timeout = tinfo[tmr].timeout;
    double remain;
    //
    // Is this counting up or down
    //
    Uns32 value;
    remain = timeout - (now - start);

    value = remain * freq_mhz;

    return value;
}

//void dobrk () {
//    bhmPrintf("break\n");
//}
//////////////////////////////// Callback stubs ////////////////////////////////

PPM_REG_READ_CB(ReadTCR0) {

    Uns32 remain = clocksRemaining(0);

    if (!plb_REG_data.TCSR0.bits.UDT0) {
        remain = ~remain;
    }

    plb_REG_data.TCR0.value = remain;
    if (BHM_DIAG_HIGH) bhmMessage("I", PREFIX, "Read TCR0=0x%08x", plb_REG_data.TCR0.value);
    return GETBE(plb_REG_data.TCR0.value);
}

PPM_REG_READ_CB(ReadTCR1) {
    Uns32 remain = clocksRemaining(1);

    if (!plb_REG_data.TCSR1.bits.UDT1) {
        remain = ~remain;
    }

    plb_REG_data.TCR1.value = remain;
    if (BHM_DIAG_HIGH) bhmMessage("I", PREFIX, "Read TCR1=0x%08x", plb_REG_data.TCR1.value);
    return GETBE(plb_REG_data.TCR1.value);
}

PPM_REG_READ_CB(ReadTCSR0) {
    if (BHM_DIAG_HIGH) bhmMessage("I", PREFIX, "Read TCSR0=0x%08x", plb_REG_data.TCSR0.value);
    return GETBE(plb_REG_data.TCSR0.value);
}

PPM_REG_READ_CB(ReadTCSR1) {
    if (BHM_DIAG_HIGH) bhmMessage("I", PREFIX, "Read TCSR1=0x%08x", plb_REG_data.TCSR1.value);
    return GETBE(plb_REG_data.TCSR1.value);
}

PPM_REG_READ_CB(ReadTLR0) {
    if (BHM_DIAG_HIGH) bhmMessage("I", PREFIX, "Read TLR0=0x%08x", plb_REG_data.TLR0.value);
    return GETBE(plb_REG_data.TLR0.value);
}

PPM_REG_READ_CB(ReadTLR1) {
    if (BHM_DIAG_HIGH) bhmMessage("I", PREFIX, "Read TLR1=0x%08x", plb_REG_data.TLR1.value);
    return GETBE(plb_REG_data.TLR1.value);
}


void WriteTCSR (void *addr, Uns32 bytes, void *user, Uns32 data) {
    Uns32 tmr = ((Uns32)user) >> 4;

    Uns32 d = GETLE(data);

    Bool changed = True; // (TCSR.value != d);
//    if (unchanged) {
//        bhmMessage("I", PREFIX, "Mode for tmr%d unchanged\n", tmr);
//    }
    if (changed) {
        TCSR.value = d;
    }

    if (BHM_DIAG_HIGH) {
        bhmMessage("I", PREFIX, "Write TCSR%d=0x%08x", tmr, TCSR.value);
        bhmMessage("I", PREFIX, "    TCSR%d %s\n",                 tmr, TCSR.bits.MDT   ? "Capture" : "Generate");
        bhmMessage("I", PREFIX, "    TCSR%d %s\n",                 tmr, TCSR.bits.UDT   ? "Down"    : "Up");
        bhmMessage("I", PREFIX, "    TCSR%d %s\n",                 tmr, TCSR.bits.GENT  ? "External": "Internal");
        bhmMessage("I", PREFIX, "    TCSR%d %s Capture\n",         tmr, TCSR.bits.CAPT  ? "Enable"  : "Disable");
        bhmMessage("I", PREFIX, "    TCSR%d %s Counter/Capture\n", tmr, TCSR.bits.ARHT  ? "Reload"  : "Hold");
        bhmMessage("I", PREFIX, "    TCSR%d %s Timer\n",           tmr, TCSR.bits.LOAD  ? "Load"    : "No Load");
        bhmMessage("I", PREFIX, "    TCSR%d %s Interrupt\n",       tmr, TCSR.bits.ENIT  ? "Enable"  : "Disable");
        bhmMessage("I", PREFIX, "    TCSR%d %s Timer\n",           tmr, TCSR.bits.ENT   ? "Enable"  : "Disable");
        bhmMessage("I", PREFIX, "    TCSR%d %s T0INT\n",           tmr, TCSR.bits.TINT  ? "Clear"   : "No Change");
        bhmMessage("I", PREFIX, "    TCSR%d %s PWM\n",             tmr, TCSR.bits.PWMA  ? "Enable"  : "Disable");
        bhmMessage("I", PREFIX, "    TCSR%d %s All Timers\n",      tmr, TCSR.bits.ENALL ? "Enable"  : "No Effect");
    }

    //
    // Clear The interrupt if active, then place back into register
    //
    if (TCSR.bits.TINT) {
        TCSR.bits.TINT = 0;
    }

    //
    // Put to timer channel
    //
    putTmrStatus(tmr);

    //
    // Enable Timer ?
    //
    if (TCSR.bits.ENT) {
        timerEnable(tmr, changed);
    }
}

PPM_REG_WRITE_CB(WriteTCSR0) {
    WriteTCSR(addr, bytes, user, data);
    if (BHM_DIAG_HIGH) bhmMessage("I", PREFIX, "Write TCSR0=0x%08x", plb_REG_data.TCSR0.value);
}

PPM_REG_WRITE_CB(WriteTCSR1) {
    WriteTCSR(addr, bytes, user, data);
    if (BHM_DIAG_HIGH) bhmMessage("I", PREFIX, "Write TCSR1=0x%08x", plb_REG_data.TCSR1.value);
}

PPM_REG_WRITE_CB(WriteTLR0) {
    Uns32 d = GETLE(data);
    plb_REG_data.TLR0.value = d;
    if (BHM_DIAG_HIGH) bhmMessage("I", PREFIX, "Write TLR0=0x%08x", plb_REG_data.TLR0.value);
}

PPM_REG_WRITE_CB(WriteTLR1) {
    Uns32 d = GETLE(data);
    plb_REG_data.TLR1.value = d;
    if (BHM_DIAG_HIGH) bhmMessage("F", PREFIX, "Write TLR1=0x%08x", plb_REG_data.TLR0.value);
}

PPM_CONSTRUCTOR_CB(constructor) {
    periphConstructor();
    interruptSet(0);

    //
    // Calculate clock frequency, only updated if set
    //
    Uns64 frequency = DEFAULT_FREQ_MHZ * 1000000;
    bhmUns64Attribute("frequency", &frequency);
    freq_mhz = frequency / 1000000;
}

PPM_DESTRUCTOR_CB(destructor) {
    // YOUR CODE HERE (destructor)

    //
    // Clean up any threads left over
    //
    timerStop(0);
    timerStop(1);
}

