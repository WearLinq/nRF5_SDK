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
#ifndef SD_GATTS_WRAPPER_H
#define SD_GATTS_WRAPPER_H

// Nordic includes
#include "ble.h"
#include "ble_gatts.h"

#define TRACE_BLE_GATTS_HVX

#ifdef TRACE_BLE_GATTS_HVX
    #define BLE_GATTS_HVX(conn_handle, p_hvx_params) gatts_hvx_wrapper(conn_handle, p_hvx_params)
#else
    #define BLE_GATTS_HVX(conn_handle, p_hvx_params) sd_ble_gatts_hvx(conn_handle, p_hvx_params)
#endif

#ifdef TRACE_BLE_GATTS_HVX
void init_gatts_queue_count(int queue_size);
uint32_t gatts_hvx_wrapper(uint16_t conn_handle, ble_gatts_hvx_params_t const *p_hvx_params);
void gatts_tx_complete(ble_evt_t const * p_ble_evt);
uint32_t get_gatts_queue_count(void);
void reset_gatts_queue_count(void);
#endif

#endif // SD_GATTS_WRAPPER_H