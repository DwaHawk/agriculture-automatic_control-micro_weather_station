# 微型氣象站資料收集系統

本專案示範如何以 ESP32 整合多種感測器，並將資料上傳至 Google Sheets，建立基礎的物聯網氣象資料平台。

## 專案緣起
在農業領域實時環境資料是作為農業決策的判斷依據，但是感測器與中央控制器通常成本高使得務農第一線人員不易採用。
近年開源軟體Arduino及周邊配件的流行，使得相關技術成本的降低。本專案使用ESP32作為控制核心來連接常用感測器，並將資料回傳到google sheet以利後續分析。

**感謝銘傳大學應資系在開發上的協助，讓這套系統能有實際使用經驗。**

## ✨ 專案特色
- ESP32 整合多個環境感測器
- 資料即時上傳至 Google Sheets
- Arduino + Google Apps Script 結合實現遠端資料收集


## 🧩 使用感測器
這邊使用低成本的工業或科研級感測器，感測器都可以更換，只要遵循ESP32有的通訊協定及硬體負載。
👉 [點我查看詳細感測器列表](hardware/sensors_list.md)

## 🔧 使用步驟

### Step 1 - Google Sheets & Apps Script
1. 建立 Google Sheet 並命名為 `tableA`或你想取的table name。
2. 開啟 Apps Script，貼上 `google_scripts/weatherRecordApp.js` 內容。
3. 部署為 Web App，取得 URL 與 `deploy ID`。
4. 修改 `esp32/weather_station.ino` 中 `GOOGLE_SCRIPT_ID` 為你自己的。

### Step 2 - Arduino 開發環境
1. 安裝 Arduino IDE 與 ESP32 Board（[教學連結](https://hackmd.io/@DwaHawk/ByxO0qQ5T)）
2. 匯入 `weather_station.ino` 程式。
3. 確保所需的 Library 安裝完成（如 DHT、BH1750、Adafruit BMP180 等）
4. 修改 WiFi SSID 與密碼。
5. 燒錄至 ESP32 開發板，常有板子版本與驅動程式版本不符問題，這時試試較舊的驅動。

## 🧱 注意事項
- 如果遇到 ESP32 WiFi 不穩建議加入自動重連機制。
- RG-15 有時會回傳 NA，可考慮改用其他雨量計。
- 建議之後整合 PCB 設計減少接線錯誤與增強耐候性。

## 📜 授權
- 本專案中所有程式碼（如 ESP32、Google Apps Script）採用 [MIT License](LICENSE) 授權。
- 所有說明文件與圖文內容（如本說明、感測器列表）則採用 [CC BY-SA 4.0](https://creativecommons.org/licenses/by-sa/4.0/deed.zh_TW) 授權。

## 🙋 聯絡我
由 David Huang 開發，歡迎貢獻與討論。
huang7766@gmail.com
