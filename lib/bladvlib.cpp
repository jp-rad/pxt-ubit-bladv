/*
MIT License

Copyright (c) 2022 jp-rad

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

// https://github.com/lancaster-university/codal-microbit-v2/blob/master/source/bluetooth/MicroBitBLEManager.cpp

/*
The MIT License (MIT)

Copyright (c) 2016 British Broadcasting Corporation.
This software is provided by Lancaster University by arrangement with the BBC.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

#include "pxt.h"
#include "bladvlib.h"

namespace bladvlib {

//================================================================
#if MICROBIT_CODAL
//================================================================

#include "ble_advdata.h"
#include "peer_manager.h"

/**
 * For configure advertising
 * 
 * https://github.com/lancaster-university/codal-microbit-v2/blob/master/source/bluetooth/MicroBitBLEManager.cpp#L133
 * https://github.com/lancaster-university/codal-microbit-v2/blob/master/source/bluetooth/MicroBitBLEManager.cpp#L134
 */
// static uint8_t              m_adv_handle    = BLE_GAP_ADV_SET_HANDLE_NOT_SET;
static uint8_t              m_adv_handle    = 0; // WARNING: magic handle number! 
static uint8_t              m_enc_advdata[ BLE_GAP_ADV_SET_DATA_SIZE_MAX];

// static void microbit_ble_configureAdvertising( bool, bool, bool, uint16_t, int, ble_advdata_t *);
// https://github.com/lancaster-university/codal-microbit-v2/blob/master/source/bluetooth/MicroBitBLEManager.cpp#L1187

/**
 * Function to configure advertising
 *
 * @param connectable Choose connectable advertising events.
 * @param discoverable Choose LE General Discoverable Mode.
 * @param whitelist Filter scan and connect requests with whitelist.
 * @param interval_ms Advertising interval in milliseconds.
 * @param timeout_seconds Advertising timeout in seconds
 */
static void microbit_ble_configureAdvertising( bool connectable, bool discoverable, bool whitelist,
                                               uint16_t interval_ms, int timeout_seconds,
                                               ble_advdata_t *p_advdata)
{
    MICROBIT_DEBUG_DMESG( "configureAdvertising connectable %d, discoverable %d", (int) connectable, (int) discoverable);
    MICROBIT_DEBUG_DMESG( "whitelist %d, interval_ms %d, timeout_seconds %d", (int) whitelist, (int) interval_ms, (int) timeout_seconds);

    ble_gap_adv_params_t    gap_adv_params;
    memset( &gap_adv_params, 0, sizeof( gap_adv_params));
    gap_adv_params.properties.type  = connectable
                                    ? BLE_GAP_ADV_TYPE_CONNECTABLE_SCANNABLE_UNDIRECTED
                                    : BLE_GAP_ADV_TYPE_NONCONNECTABLE_SCANNABLE_UNDIRECTED;
    gap_adv_params.interval         = ( 1000 * interval_ms) / 625;  // 625 us units
    if ( gap_adv_params.interval < BLE_GAP_ADV_INTERVAL_MIN) gap_adv_params.interval = BLE_GAP_ADV_INTERVAL_MIN;
    if ( gap_adv_params.interval > BLE_GAP_ADV_INTERVAL_MAX) gap_adv_params.interval = BLE_GAP_ADV_INTERVAL_MAX;
    gap_adv_params.duration         = timeout_seconds * 100;              //10 ms units
    gap_adv_params.filter_policy    = whitelist
                                    ? BLE_GAP_ADV_FP_FILTER_BOTH
                                    : BLE_GAP_ADV_FP_ANY;
    gap_adv_params.primary_phy      = BLE_GAP_PHY_1MBPS;
                
    ble_gap_adv_data_t  gap_adv_data;
    memset( &gap_adv_data, 0, sizeof( gap_adv_data));
    gap_adv_data.adv_data.p_data    = m_enc_advdata;
    gap_adv_data.adv_data.len       = BLE_GAP_ADV_SET_DATA_SIZE_MAX;
    MICROBIT_BLE_ECHK( ble_advdata_encode( p_advdata, gap_adv_data.adv_data.p_data, &gap_adv_data.adv_data.len));
    //NRF_LOG_HEXDUMP_INFO( gap_adv_data.adv_data.p_data, gap_adv_data.adv_data.len);
    MICROBIT_BLE_ECHK( sd_ble_gap_adv_set_configure( &m_adv_handle, &gap_adv_data, &gap_adv_params));
}

// static void microbit_ble_configureAdvertising( bool, bool, bool, uint16_t, int, ble_uuid_t *)
// https://github.com/lancaster-university/codal-microbit-v2/blob/master/source/bluetooth/MicroBitBLEManager.cpp#L1218

static void microbit_ble_configureAdvertising( bool connectable, bool discoverable, bool whitelist,
                                               uint16_t interval_ms, int timeout_seconds, ble_uuid_t *p_uuid)
{
    ble_advdata_t advdata;
    memset( &advdata, 0, sizeof( advdata));
    advdata.name_type = BLE_ADVDATA_FULL_NAME;
    advdata.flags     = !whitelist && discoverable
                      ? BLE_GAP_ADV_FLAG_BR_EDR_NOT_SUPPORTED | BLE_GAP_ADV_FLAG_LE_GENERAL_DISC_MODE
                      : BLE_GAP_ADV_FLAG_BR_EDR_NOT_SUPPORTED;
    advdata.uuids_complete.p_uuids = p_uuid;
    advdata.uuids_complete.uuid_cnt = 1;

    microbit_ble_configureAdvertising( connectable, discoverable, whitelist, interval_ms, timeout_seconds, &advdata);
}

// Setup advertising.
// https://github.com/lancaster-university/codal-microbit-v2/blob/master/source/bluetooth/MicroBitBLEManager.cpp#L478

void accumulateCompleteList16BitServiceID( const uint16_t serviceUUID)
{
    // Stop
    uBit.bleManager.stopAdvertising();

    // Configure
    static ble_uuid_t uuid;
    uuid.type = BLE_UUID_TYPE_BLE;
    uuid.uuid = serviceUUID;

    bool connectable = true;
    bool discoverable = true;
    bool whitelist = false;
    
#if CONFIG_ENABLED(MICROBIT_BLE_WHITELIST)
    // Configure a whitelist to filter all connection requetss from unbonded devices.
    // Most BLE stacks only permit one connection at a time, so this prevents denial of service attacks.
//    ble->gap().setScanningPolicyMode(Gap::SCAN_POLICY_IGNORE_WHITELIST);
//    ble->gap().setAdvertisingPolicyMode(Gap::ADV_POLICY_FILTER_CONN_REQS);
    
    pm_peer_id_t peer_list[ MICROBIT_BLE_MAXIMUM_BONDS];
    uint32_t list_size = MICROBIT_BLE_MAXIMUM_BONDS;
    MICROBIT_BLE_ECHK( pm_peer_id_list( peer_list, &list_size, PM_PEER_ID_INVALID, PM_PEER_ID_LIST_ALL_ID ));
    //MICROBIT_BLE_ECHK( pm_whitelist_set( list_size ? peer_list : NULL, list_size));
    //MICROBIT_BLE_ECHK( pm_device_identities_list_set( list_size ? peer_list : NULL, list_size));
    connectable = discoverable = whitelist = list_size > 0;
    MICROBIT_DEBUG_DMESG( "whitelist size = %d", list_size);
#endif
  
    // Setup advertising.
    microbit_ble_configureAdvertising( connectable, discoverable, whitelist,
                                       MICROBIT_BLE_ADVERTISING_INTERVAL, MICROBIT_BLE_ADVERTISING_TIMEOUT, &uuid);

    // Restart
    uBit.bleManager.advertise();
} 

//================================================================
#else // MICROBIT_CODAL
//================================================================

void accumulateCompleteList16BitServiceID( const uint16_t serviceUUID)
{
    // Complete list of 16-bit Service IDs.
    uBit.ble->accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_16BIT_SERVICE_IDS, (uint8_t *)&serviceUUID, 2);
}

//================================================================
#endif // MICROBIT_CODAL
//================================================================

}