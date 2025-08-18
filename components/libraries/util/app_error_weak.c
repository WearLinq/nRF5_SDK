/**
 * Copyright (c) 2016 - 2020, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include "app_error.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "app_util_platform.h"
#include "nrf_strerror.h"
#include "nrf_delay.h"

#if defined(SOFTDEVICE_PRESENT) && SOFTDEVICE_PRESENT
#include "nrf_sdm.h"
#endif

#define LOG_BLANK()        NRF_LOG_ERROR(" ")

#define LOG_BLOCK_BEGIN()  do {                          \
    LOG_BLANK();                                         \
    NRF_LOG_ERROR("============ APP FAULT ============");\
} while (0)

#define LOG_BLOCK_END()    do {                          \
    NRF_LOG_ERROR("===================================");\
    LOG_BLANK();                                         \
} while (0)

// Helper: key–value (aligned)
#define LOG_KV(key, fmt, ...) NRF_LOG_ERROR("%-10s: " fmt, key, ##__VA_ARGS__)

#define LOG_KV(key, fmt, ...) NRF_LOG_ERROR("%-10s: " fmt, key, ##__VA_ARGS__)

void my_app_error_fault_printer(uint32_t id, uint32_t pc, uint32_t info)
{
    LOG_BLOCK_BEGIN();

    switch (id)
    {
#if defined(SOFTDEVICE_PRESENT) && SOFTDEVICE_PRESENT
        case NRF_FAULT_ID_SD_ASSERT:
            LOG_KV("Type", "SOFTDEVICE ASSERT");
            LOG_KV("PC",   "0x%08X", (unsigned)pc);
            break;

        case NRF_FAULT_ID_APP_MEMACC:
            LOG_KV("Type", "SOFTDEVICE MEMACC");
            LOG_KV("PC",   "0x%08X", (unsigned)pc);
            break;
#endif

        case NRF_FAULT_ID_SDK_ASSERT:
        {
            const assert_info_t *p = (const assert_info_t *)info;
            LOG_KV("Type", "SDK ASSERT");
            LOG_KV("File", "%s", p ? (const char*)p->p_file_name : NULL);
            LOG_KV("Line", "%u", p ? (unsigned)p->line_num : 0u);
            LOG_KV("PC",   "0x%08X", (unsigned)pc);
        } break;

        case NRF_FAULT_ID_SDK_ERROR:
        {
            const error_info_t *p = (const error_info_t *)info;
            LOG_KV("Type", "SDK ERROR");
            LOG_KV("Code", "%d [%s]",
                   p ? (int)p->err_code : 0,
                   p ? nrf_strerror_get(p->err_code) : "unknown");
            LOG_KV("File", "%s", p ? (const char*)p->p_file_name : NULL);
            LOG_KV("Line", "%u", p ? (unsigned)p->line_num : 0u);
            LOG_KV("PC",   "0x%08X", (unsigned)pc);
        } break;

        default:
            LOG_KV("Type", "UNKNOWN");
            LOG_KV("PC",   "0x%08X", (unsigned)pc);
            LOG_KV("Info", "0x%08X", (unsigned)info);
            break;
    }

    LOG_BLOCK_END();
}

/*lint -save -e14 */
/**
 * Function is implemented as weak so that it can be overwritten by custom application error handler
 * when needed.
 */
__WEAK void app_error_fault_handler(uint32_t id, uint32_t pc, uint32_t info)
{
    __disable_irq();
    NRF_LOG_FINAL_FLUSH();

    my_app_error_fault_printer(id, pc, info);
    NRF_LOG_WARNING("System reset is coming...\n");

    NVIC_SystemReset();
}
/*lint -restore */
