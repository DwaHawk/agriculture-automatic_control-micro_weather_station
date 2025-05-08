//weatherRecordApp v.1
//Copyright (C) 2025, David Huang

var sheet_id = "your_sheet_id";
var ss = SpreadsheetApp.openById(sheet_id);
var sheetName = "tableA";// make sure the sheet name is correct

function doGet(e){
    var information = e.parameter.information;
    var si1t = e.parameter.si1t;//soil_in_1_temp, we initially set up four soil sensors
    var si1h = e.parameter.si1h;
    var si1c = e.parameter.si1c;
    var si2t = e.parameter.si2t;
    var si2h = e.parameter.si2h;
    var si2c = e.parameter.si2c;
    var si3t = e.parameter.si3t;
    var si3h = e.parameter.si3h;
    var si3c = e.parameter.si3c;
    var si4t = e.parameter.si4t;
    var si4h = e.parameter.si4h;
    var si4c = e.parameter.si4c;
    var wDi = e.parameter.wDi;
    var li = e.parameter.li;
    var ti = e.parameter.ti;
    var hi = e.parameter.hi;
    var pi = e.parameter.pi;
    var wVi = e.parameter.wVi;
    var rain = e.parameter.rain;
    var rain1 = e.parameter.rain1;
    var rain2 = e.parameter.rain2;
    var rain3 = e.parameter.rain3;
    let today = new Date();
    Logger.log(today);
    if (information !== undefined & information == 4){
        ss.getSheetByName(sheetName).appendRow(
        [`${today.getFullYear()}-${today.getMonth()+1}-${today.getDate()} ${today.getHours()}:${today.getMinutes()}:${today.getSeconds()}`,
        String(pi),
        String(ti),
        String(hi),
        String(li),
        String(wDi),
        String(wVi),
        String(si1t),
        String(si1h),
        String(si1c),
        String(si2t),
        String(si2h),
        String(si2c),
        String(si3t),
        String(si3h),
        String(si3c),
        String(rain),
        String(rain1),
        String(rain2),
        String(rain3),
        String(si4t),
        String(si4h),
        String(si4c)
        ]);//You can sort the Google Sheets in the way you want.
    }
    return ContentService.createTextOutput("Data recorded successfully.");
}