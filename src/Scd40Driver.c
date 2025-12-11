/**
 * @file Scd40Driver.c
 * @brief SCD40 CO2 感測器驅動程式庫的實作。
 */

#include "Scd40Driver.h" 
#include <TinyWireM.h>   // 必須引用 I2C 庫
#include <util/delay.h>  // 使用延遲函數 _delay_ms

// --- 內部函數實作 (發送命令 - I2C 輔助) ---

/**
 * @brief 發送 16-bit 命令到 SCD40。
 */
static void send_command(uint16_t command) {
    TinyWireM.beginTransmission(SCD40_ADDR);
    TinyWireM.write((uint8_t)(command >> 8));
    TinyWireM.write((uint8_t)(command & 0xFF));
    TinyWireM.endTransmission();
}


// --- 1. CRC-8 校驗函數 ---

uint8_t SCD40_compute_crc(const uint8_t *data, size_t len) {
    uint8_t crc = 0xFF; 
    const uint8_t polynomial = 0x31; 

    for (size_t i = 0; i < len; i++) {
        crc ^= data[i]; 
        for (int j = 0; j < 8; j++) {
            if (crc & 0x80) { 
                crc = (crc << 1) ^ polynomial;
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}


// --- 2. 公開介面實作：初始化 ---

uint8_t SCD40_begin() {
    // 啟動 I2C 週期測量
    send_command(START_PERIODIC_MEASUREMENT);
    
    // 等待 SCD40 進入測量模式 (必須有延遲才能開始讀取)
    _delay_ms(10); 
    
    // 成功發送命令返回 0
    return 0; 
}


// --- 3. 公開介面實作：讀取數據 ---

uint8_t SCD40_read_data(Scd40Data *data_out) {
    uint8_t data[SCD40_READ_BYTES]; 
    
    // 1. 發送讀取命令
    send_command(READ_MEASUREMENT_RAW);
    
    // 2. 請求 9 個位元組數據
    TinyWireM.requestFrom(SCD40_ADDR, SCD40_READ_BYTES);
    
    if (TinyWireM.available() != SCD40_READ_BYTES) {
        // 返回 1 代表數據長度錯誤
        return 1; 
    }
    
    // 3. 接收數據
    for (int i = 0; i < SCD40_READ_BYTES; i++) {
        data[i] = TinyWireM.receive();
    }
    
    // --- 4. CRC 校驗 (檢查 CO2, Temp, Humi 的 CRC) ---
    if (SCD40_compute_crc(&data[0], 2) != data[2] || // CO2 CRC
        SCD40_compute_crc(&data[3], 2) != data[5] || // Temp CRC
        SCD40_compute_crc(&data[6], 2) != data[8])   // Humi CRC
    {
        // 返回 2 代表 CRC 校驗失敗
        return 2; 
    }
    
    // --- 5. 解析並轉換數據 (使用你的邏輯，但改為浮點數運算) ---
    uint16_t raw_co2  = (data[0] << 8) | data[1];
    uint16_t raw_temp = (data[3] << 8) | data[4];
    uint16_t raw_humi = (data[6] << 8) | data[7];
    
    // CO2
    data_out->co2_ppm = raw_co2;
    
    // 溫度: T [°C] = -45 + 175 * raw_temp / 2^16
    data_out->temperature_C = -45.0f + 175.0f * (float)raw_temp / 65536.0f;
    
    // 濕度: RH [%] = 100 * raw_humi / 2^16
    data_out->humidity_perc = 100.0f * (float)raw_humi / 65536.0f;
    
    // 返回 0 代表成功
    return 0;
}