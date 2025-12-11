#include <TinyWireM.h> 
#include "Scd40Driver.h" // 引用程式庫

Scd40Data data; // 程式庫定義的數據結構體

void setup() {
  // 初始化 I2C
  TinyWireM.begin();
  
  // 初始化 SCD40
  SCD40_begin(); 
  
  // (可選) 添加 Serial.begin() 或 OLED.begin() 用於輸出/顯示
}

void loop() {
  // 嘗試讀取數據
  if (SCD40_read_data(&data) == 0) { // 成功讀取且 CRC 通過
    // 輸出數據
    // OLED 顯示代碼可以在這裡使用 data.co2_ppm, data.temperature_C 等
    
  } else {
    // 錯誤處理 (例如在 OLED 上顯示 "CRC ERR")
  }

  delay(5000); // SCD40 週期測量需等待約 5 秒
}