/*
MIT License

Copyright (c) 2022-2023 jp-rad

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

#define TAKO_TX_POWER_LEVEL 7

static const int8_t TAKO_TX_POWER_LEVEL_VALUE[] = {-30, -20, -16, -12, -8, -4, 0, 4};

static const ManagedString takoCircle(   "00000000");
static const ManagedString takoSquare(   "00000001");
static const ManagedString takoStar(     "00000002");
static const ManagedString takoHeart(    "00000003");
static const ManagedString takoTriangle( "00000004");

static ManagedString createSymbolDeviceName( const int8_t symbol)
{
    // gap device name
    ManagedString gapName;  // length: 16
    
    if ( 0 > symbol)
    {
        // reset
        gapName = ManagedString("0000000000000000");
    }
    else
    {
        ManagedString seq;
        seq = (ManagedString("8B") + ManagedString(uBit.random(99998) + 1) + ManagedString("FFFFFF")).substring(0, 8);
        switch (symbol)
        {
        case 0:
            gapName = seq + takoCircle;
            break;
        case 1:
            gapName = seq + takoSquare;
            break;
        case 2:
            gapName = seq + takoStar;
            break;
        case 3:
            gapName = seq + takoHeart;
            break;
        case 4:
            gapName = seq + takoTriangle;
            break;
        default:
            gapName = seq + takoCircle;
            break;
        }
    }

    return gapName;
}

//================================================================
#if MICROBIT_CODAL
//================================================================

#include "ble_advdata.h"
#include "peer_manager.h"

#define LBS_UUID_TAKO_BASE { 0xc1, 0x97, 0x12, 0xb5, 0xcf, 0x7e, 0xd7, 0xbc, 0x38, 0x45, 0x02, 0x00, 0x00, 0x00, 0x40, 0x25}
#define LBS_UUID_TAKO_SERVICE 0xb6b0

/**
 * For configure advertising
 * 
 * https://github.com/lancaster-university/codal-microbit-v2/blob/master/source/bluetooth/MicroBitBLEManager.cpp#L133
 * https://github.com/lancaster-university/codal-microbit-v2/blob/master/source/bluetooth/MicroBitBLEManager.cpp#L134
 */
// static uint8_t              m_adv_handle    = BLE_GAP_ADV_SET_HANDLE_NOT_SET;
static uint8_t              m_adv_handle    = 0; // WARNING: magic handle number! 
static uint8_t              m_enc_advdata[ BLE_GAP_ADV_SET_DATA_SIZE_MAX];
static uint8_t              m_enc_scanrsp[ BLE_GAP_ADV_SET_DATA_SIZE_MAX];

/**
 * Tako 
 */
static bool m_tako_initialized = false;
static ble_uuid_t m_tako_uuid;
static const uint16_t TAKO_MANUF_ID = 0x8888;
static uint8_t TAKO_MANUF_STR[] = "TAKO de-s";

// static void microbit_ble_configureAdvertising( bool, bool, bool, uint16_t, int, ble_advdata_t *);
// https://github.com/lancaster-university/codal-microbit-v2/blob/master/source/bluetooth/MicroBitBLEManager.cpp#L1187

/**
 * Function to configure advertising
 *
 * @param connectable Choose connectable advertising events.
 * @param discoverable Choose LE General Discoverable Mode.
 * @param whitelist Filter scan and connect requests with whitelist.
 * @param interval_ms Advertising interval in milliseconds.
 * @param timeout_seconds Advertising timeout in seconds.
 * @param p_advdata advertising data.
 * @param p_scanrsp scan responce data.
 */
static void custom_microbit_ble_configureAdvertising( bool connectable, bool discoverable, bool whitelist,
                                               uint16_t interval_ms, int timeout_seconds,
                                               ble_advdata_t *p_advdata, ble_advdata_t *p_scanrsp)
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
    gap_adv_data.scan_rsp_data.p_data = m_enc_scanrsp;
    gap_adv_data.scan_rsp_data.len = BLE_GAP_ADV_SET_DATA_SIZE_MAX;
    MICROBIT_BLE_ECHK(ble_advdata_encode(p_scanrsp, gap_adv_data.scan_rsp_data.p_data, &gap_adv_data.scan_rsp_data.len));
    // NRF_LOG_HEXDUMP_INFO( gap_adv_data.scan_rsp_data.p_data, gap_adv_data.scan_rsp_data.len);
    MICROBIT_BLE_ECHK( sd_ble_gap_adv_set_configure( &m_adv_handle, &gap_adv_data, &gap_adv_params));
}

// Setup advertising.
// https://github.com/lancaster-university/codal-microbit-v2/blob/master/source/bluetooth/MicroBitBLEManager.cpp#L478

// static void microbit_ble_configureAdvertising( bool, bool, bool, uint16_t, int, ble_uuid_t *)
// https://github.com/lancaster-university/codal-microbit-v2/blob/master/source/bluetooth/MicroBitBLEManager.cpp#L1218

void accumulateCompleteList16BitServiceID( const uint16_t serviceUUID)
{
    // Stop
    uBit.bleManager.stopAdvertising();

    // 16bit uuid
    static ble_uuid_t uuid;
    uuid.type = BLE_UUID_TYPE_BLE;
    uuid.uuid = serviceUUID;

    // Configure
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
  
    // adv_data
    ble_advdata_t advdata;
    memset( &advdata, 0, sizeof( advdata));
    advdata.name_type = BLE_ADVDATA_FULL_NAME;
    advdata.flags     = !whitelist && discoverable
                      ? BLE_GAP_ADV_FLAG_BR_EDR_NOT_SUPPORTED | BLE_GAP_ADV_FLAG_LE_GENERAL_DISC_MODE
                      : BLE_GAP_ADV_FLAG_BR_EDR_NOT_SUPPORTED;
    // scan_rsp_data
    ble_advdata_t scanrsp;
    memset(&scanrsp, 0, sizeof(scanrsp));
    // advdata.name_type = BLE_ADVDATA_NO_NAME;
    scanrsp.uuids_complete.p_uuids = &uuid;
    scanrsp.uuids_complete.uuid_cnt = 1;
    // configure
    custom_microbit_ble_configureAdvertising( connectable, discoverable, whitelist, 
                                       MICROBIT_BLE_ADVERTISING_INTERVAL, MICROBIT_BLE_ADVERTISING_TIMEOUT,
                                       &advdata, &scanrsp);

    // Restart
    uBit.bleManager.advertise();
}

static void initTako()
{
    if (m_tako_initialized)
    {
        return;
    }
    m_tako_initialized = true;
    // 128bit uuid for TAKO
    static ble_uuid128_t base_uuid = {LBS_UUID_TAKO_BASE};
    MICROBIT_BLE_ECHK(sd_ble_uuid_vs_add(&base_uuid, &m_tako_uuid.type));
    m_tako_uuid.uuid = LBS_UUID_TAKO_SERVICE;
    // Bluetooth Appearance values
    // https://infocenter.nordicsemi.com/topic/com.nordic.infocenter.s113.api.v7.0.1/group___b_l_e___a_p_p_e_a_r_a_n_c_e_s.html
    MICROBIT_BLE_ECHK(sd_ble_gap_appearance_set(BLE_APPEARANCE_GENERIC_COMPUTER));
}

static void setSymbolDeviceName( const int8_t symbol)
{
    ManagedString gapName;
    gapName = createSymbolDeviceName(symbol);
    // set gap device name
    ble_gap_conn_sec_mode_t sec_mode;
    memset( &sec_mode, 0, sizeof( sec_mode));
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);
    MICROBIT_BLE_ECHK(sd_ble_gap_device_name_set(&sec_mode, (const uint8_t *)gapName.toCharArray(), gapName.length()));
}

void advertiseTako( const int8_t symbol)
{
    // Stop
    uBit.bleManager.stopAdvertising();

    // Initialize (once)
    initTako();

    // gap device name
    setSymbolDeviceName( symbol);

    // set tx power
    uBit.bleManager.setTransmitPower(TAKO_TX_POWER_LEVEL);

    // Setup advertising.
    bool connectable = false;
    bool discoverable = true;
    bool whitelist = false;
    // adv_data
    ble_advdata_t advdata;
    memset( &advdata, 0, sizeof( advdata));
    advdata.name_type = BLE_ADVDATA_FULL_NAME;
    advdata.flags     = !whitelist && discoverable
                      ? BLE_GAP_ADV_FLAG_BR_EDR_NOT_SUPPORTED | BLE_GAP_ADV_FLAG_LE_GENERAL_DISC_MODE
                      : BLE_GAP_ADV_FLAG_BR_EDR_NOT_SUPPORTED;
    advdata.include_appearance = true;
    static int8_t tx_power_level = TAKO_TX_POWER_LEVEL_VALUE[TAKO_TX_POWER_LEVEL];
    advdata.p_tx_power_level = &tx_power_level;
    // scan_rsp_data
    ble_advdata_t scanrsp;
    memset(&scanrsp, 0, sizeof(scanrsp));
    // advdata.name_type = BLE_ADVDATA_NO_NAME;
    scanrsp.uuids_complete.p_uuids = &m_tako_uuid;
    scanrsp.uuids_complete.uuid_cnt = 1;
    ble_advdata_manuf_data_t manuf;
    memset(&manuf, 0, sizeof(manuf));
    manuf.company_identifier = TAKO_MANUF_ID;
    manuf.data.p_data = TAKO_MANUF_STR;
    manuf.data.size = sizeof(TAKO_MANUF_STR) - 1;
    scanrsp.p_manuf_specific_data = &manuf;
    // configure
    custom_microbit_ble_configureAdvertising( connectable, discoverable, whitelist, 
                                       MICROBIT_BLE_ADVERTISING_INTERVAL, MICROBIT_BLE_ADVERTISING_TIMEOUT,
                                       &advdata, &scanrsp);

    // Restart
    uBit.bleManager.advertise();
}

//================================================================
#else // MICROBIT_CODAL
//================================================================

/**
 * Tako 
 */
static const uint8_t TAKO_SERVICE_UUID[] = {0xc1, 0x97, 0x12, 0xb5, 0xcf, 0x7e, 0xd7, 0xbc, 0x38, 0x45, 0x02, 0x00, 0xb0, 0xb6, 0x40, 0x25};
static const uint8_t TAKO_MANUF_DATA[] = {0x88, 0x88, 'T', 'A', 'K', 'O', ' ', 'd', 'e', '-', 's'};
static bool m_tako_initialized = false;

void accumulateCompleteList16BitServiceID( const uint16_t serviceUUID)
{
    uBit.ble->stopAdvertising();
    // [Scan Response] Complete list of 16-bit Service IDs.
    uBit.ble->accumulateScanResponse(GapAdvertisingData::COMPLETE_LIST_16BIT_SERVICE_IDS, (uint8_t *)&serviceUUID, 2);

    uBit.ble->startAdvertising();
}

static void initTako()
{
    if (m_tako_initialized)
    {
        return;
    }
    m_tako_initialized = true;
    
    // TODO: not connectable, tx power, appearance
    // // Device is scannable but not connectable.
    // uBit.ble->setAdvertisingType(GapAdvertisingParams::ADV_SCANNABLE_UNDIRECTED);
    // // set tx power
    // uBit.bleManager.setTransmitPower(TAKO_TX_POWER_LEVEL);
    // // Tx power
    // uBit.ble->setTxPower(TAKO_TX_POWER_LEVEL_VALUE[TAKO_TX_POWER_LEVEL]);
    // // Bluetooth Appearance values
    // // https://infocenter.nordicsemi.com/topic/com.nordic.infocenter.s113.api.v7.0.1/group___b_l_e___a_p_p_e_a_r_a_n_c_e_s.html
    // uBit.ble->setAppearance(GapAdvertisingData::GENERIC_COMPUTER);

    // [Scan Response] Complete list of 16-bit Service IDs.
    uBit.ble->accumulateScanResponse(GapAdvertisingData::COMPLETE_LIST_128BIT_SERVICE_IDS, (uint8_t *)TAKO_SERVICE_UUID, sizeof(TAKO_SERVICE_UUID));
    // [Scan Response] Manufacturer Specific Data.
    uBit.ble->accumulateScanResponse(GapAdvertisingData::MANUFACTURER_SPECIFIC_DATA, (uint8_t *)TAKO_MANUF_DATA, sizeof(TAKO_MANUF_DATA));
}

void advertiseTako( const int8_t symbol)
{
    uBit.ble->stopAdvertising();
    initTako();

    // Complete Local Name.
    ManagedString gapName;
    gapName = createSymbolDeviceName(symbol);
    uBit.ble->accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LOCAL_NAME, (uint8_t *)gapName.toCharArray(), gapName.length());
 
    uBit.ble->startAdvertising();
}

//================================================================
#endif // MICROBIT_CODAL
//================================================================

}