//*****************************************************************************
// @file    SoftDevice GATTS wrapper functions
// @brief
// @details
//
// ----------------------------------------------------------------------------
//        Copyright (c) 2017-2020  WearLinq, Inc. All Rights Reserved.
//
// NOTICE: All information contained herein is, and remains the property of
// WearLinq, Inc.  and its suppliers, if any.  The intellectual and technical
// concepts contained herein are proprietary to WearLinq, Inc. and its suppliers
// and may be covered by U.S. and Foreign Patents, patents in process, and are
// protected by trade secret or copyright law. Dissemination of this information
// or reproduction of this material is strictly forbidden unless prior written
// permission is obtained from WearLinq, Inc.
//
//*****************************************************************************

// Nordic includes
#include "nrf_log.h"

// Application includes
#include "sd_wrapper.h"

#ifdef TRACE_BLE_GATTS_HVX

static int m_queue_count = 0;

void init_gatts_queue_count(int queue_size)
{
    m_queue_count = queue_size;
}

uint32_t gatts_hvx_wrapper(uint16_t conn_handle, ble_gatts_hvx_params_t const *p_hvx_params)
{
    ret_code_t ret_code = sd_ble_gatts_hvx(conn_handle, p_hvx_params);
    if (ret_code == NRF_SUCCESS)
    {
        if (p_hvx_params->type == BLE_GATT_HVX_NOTIFICATION)
            --m_queue_count;
    }
    NRF_LOG_INFO("HVX: GATTS Q %d", m_queue_count);
}

void gatts_tx_complete(ble_evt_t const * p_ble_evt)
{
    ble_gatts_evt_hvn_tx_complete_t * p_tx_complete = (ble_gatts_evt_hvn_tx_complete_t *)p_ble_evt;
    ++m_queue_count;
    NRF_LOG_INFO("TXC: GATTS Q %d", m_queue_count);
}

uint32_t get_gatts_queue_count(void)
{
    NRF_LOG_INFO("GATTS Q %d", m_queue_count);
    return m_queue_count;
}

void reset_gatts_queue_count(void)
{
    m_queue_count = NRF_BLE_GQ_DATAPOOL_ELEMENT_COUNT;
    NRF_LOG_INFO("GATTS Q %d", m_queue_count);
}
#endif // TRACE_BLE_GATTS_HVX