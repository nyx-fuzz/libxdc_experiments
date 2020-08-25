/*
 *
 * honggfuzz - Intel PT decoder
 * -----------------------------------------
 *
 * Author: Robert Swiecki <swiecki@google.com>
 *
 * Copyright 2010-2018 by Google Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * permissions and limitations under the License.
 *
 */

#include "pt.h"

#include <inttypes.h>
#include <linux/perf_event.h>
#include <stdio.h>

//#include "libhfcommon/common.h"
//#include "libhfcommon/log.h"
//#include "libhfcommon/util.h"


#include <intel-pt.h>

struct pt_cpu ptCpu = {
    .vendor   = pcv_unknown,
    .family   = 0,
    .model    = 0,
    .stepping = 0,
};

void perf_ptInit(void) {
    FILE* f = fopen("/proc/cpuinfo", "rb");
    if (!f) {
        PLOG_E("Couldn't open '/proc/cpuinfo'");
        return;
    }
    for (;;) {
        char k[1024], t[1024], v[1024];
        int  ret = fscanf(f, "%1023[^\t]%1023[\t]: %1023[^\n]\n", k, t, v);
        if (ret == EOF) {
            break;
        }
        if (ret != 3) {
            break;
        }
        if (strcmp(k, "vendor_id") == 0) {
            if (strcmp(v, "GenuineIntel") == 0) {
                ptCpu.vendor = pcv_intel;
                LOG_D("IntelPT vendor: Intel");
            } else {
                ptCpu.vendor = pcv_unknown;
                LOG_D("Current processor is not Intel, IntelPT will not work");
            }
        }
        if (strcmp(k, "cpu family") == 0) {
            ptCpu.family = atoi(v);
            LOG_D("IntelPT family: %" PRIu16, ptCpu.family);
        }
        if (strcmp(k, "model") == 0) {
            ptCpu.model = atoi(v);
            LOG_D("IntelPT model: %" PRIu8, ptCpu.model);
        }
        if (strcmp(k, "stepping") == 0) {
            ptCpu.stepping = atoi(v);
            LOG_D("IntelPT stepping: %" PRIu8, ptCpu.stepping);
        }
    }
    fclose(f);
}

/* Sign-extend a uint64_t value. */
inline static uint64_t sext(uint64_t val, uint8_t sign) {
    uint64_t signbit, mask;

    signbit = 1ull << (sign - 1);
    mask    = ~0ull << sign;

    return val & signbit ? val | mask : val & ~mask;
}

#define ATOMIC_GET(x)     __atomic_load_n(&(x), __ATOMIC_RELAXED)

__attribute__((always_inline)) static inline bool ATOMIC_BITMAP_SET(uint8_t* addr, size_t offset) {
    addr += (offset / 8);
    uint8_t mask = (1U << (offset % 8));

    if (ATOMIC_GET(*addr) & mask) {
        return true;
    }

#if defined(__x86_64__) || defined(__i386__)
    bool old;
    __asm__ __volatile__("lock bts %2, %0\n\t"
                         "sbb %1, %1\n\t"
                         : "+m"(*addr), "=r"(old)
                         : "Ir"(offset % 8));
    return old;
#else  /* defined(__x86_64__) || defined(__i386__) */
    return (ATOMIC_POST_OR(*addr, mask) & mask);
#endif /* defined(__x86_64__) || defined(__i386__) */
}



__attribute__((hot)) inline static void perf_ptAnalyzePkt(struct pt_packet* packet, uint64_t limit, uint8_t* bitmap) {
    if (packet->type != ppt_tip) {
        return;
    }

    uint64_t ip;

    switch (packet->payload.ip.ipc) {
        case pt_ipc_update_16:
            ip = packet->payload.ip.ip & 0xFFFF;
            break;
        case pt_ipc_update_32:
            ip = packet->payload.ip.ip & 0xFFFFFFFF;
            break;
        case pt_ipc_update_48:
            ip = packet->payload.ip.ip & 0xFFFFFFFFFFFF;
            break;
        case pt_ipc_sext_48:
            ip = sext(packet->payload.ip.ip, 48);
            break;
        case pt_ipc_full:
            ip = packet->payload.ip.ip;
            break;
        default:
            return;
    }

    if (ip >= limit) {
        return;
    }

    ip &= _HF_PERF_BITMAP_BITSZ_MASK;
    register bool prev = ATOMIC_BITMAP_SET(bitmap, ip);
}

void arch_ptAnalyze(uint8_t* trace_start, uint8_t* trace_end, uint64_t range_limit, uint8_t* bitmap) {

    struct pt_config ptc;
    pt_config_init(&ptc);
    ptc.begin = trace_start;
    ptc.end   = trace_end;
    ptc.cpu   = ptCpu;

    int errcode = pt_cpu_errata(&ptc.errata, &ptc.cpu);
    if (errcode < 0) {
        LOG_F("pt_errata() failed: %s\n", pt_errstr(-errcode));
    }

    struct pt_packet_decoder* ptd = pt_pkt_alloc_decoder(&ptc);
    if (ptd == NULL) {
        LOG_F("pt_pkt_alloc_decoder() failed");
    }

    errcode = pt_pkt_sync_forward(ptd);
    if (errcode < 0) {
        LOG_W("pt_pkt_sync_forward() failed: %s", pt_errstr(-errcode));
        pt_pkt_free_decoder(ptd);
        return;
    }
    printf("running stuff\n");
    for (;;) {
        struct pt_packet packet;
        errcode = pt_pkt_next(ptd, &packet, sizeof(packet));
        if (errcode == -pte_eos) {
            break;
        }
        if (errcode < 0) {
            LOG_W("pt_pkt_next() failed: %s", pt_errstr(-errcode));
            break;
        }
        perf_ptAnalyzePkt(&packet, range_limit, bitmap);
    }

    pt_pkt_free_decoder(ptd);
}