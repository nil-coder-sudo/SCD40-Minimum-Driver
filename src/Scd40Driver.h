/**
 * @file Scd40Driver.h
 * @brief SCD40 CO2 感測器驅動程式庫的最小化公開介面。
 * * 專注於通訊和數據獲取。
 */

#ifndef SCD40_DRIVER_H
#define SCD40_DRIVER_H

#include <stdint.h>
#include <stddef.h> // 為了 size_t

// --- 1. 定義常數 ---

#define SCD40_ADDR                      0x62
#define START_PERIODIC_MEASUREMENT      0x21B1 // 啟動週期測量
#define READ_MEASUREMENT_RAW            0xEC05 // 讀取數據命令
#define SCD40_READ_BYTES                9      // 讀取位元組數 (CO2, Temp, Humi + 3 CRC)

// --- 2. 數據結構體 ---

/**
 * @brief SCD40 感測器讀取的數據結構體。
 */
typedef struct {
    uint16_t co2_ppm;       // CO2 濃度 (ppm)
    float temperature_C;    // 溫度 (攝氏度 °C)
    float humidity_perc;    // 濕度 (%)
} Scd40Data;


// --- 3. 公開函數宣告 ---

/**
 * @brief 初始化 SCD40 感測器，發送啟動週期測量的命令。
 * @return 0 成功；非 0 失敗。
 */
uint8_t SCD40_begin();


/**
 * @brief 讀取 SCD40 感測器數據並解析。
 * * 執行 I2C 讀取、CRC 校驗、以及數據轉換。
 * @param data_out 指向 Scd40Data 結構體的指標，用於儲存解析後的數據。
 * @return 0 成功且 CRC 通過；非 0 表示失敗或 CRC 錯誤。
 */
uint8_t SCD40_read_data(Scd40Data *data_out);


// --- 內部輔助函數宣告 (供 .c 檔案使用，但為了程式庫完整性仍保留) ---

/**
 * @brief 執行 CRC-8 校驗，確保 I2C 數據傳輸正確。
 * @return 計算出的 CRC-8 值。
 */
uint8_t SCD40_compute_crc(const uint8_t *data, size_t len);


#endif // SCD40_DRIVER_H