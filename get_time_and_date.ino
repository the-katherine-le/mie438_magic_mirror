#include <WiFi.h>
#include "time.h"

//Comment here your wifi / hotspot credentials for personal testing
const char* ssid = "Katherine";
const char* password = "password123";

const char* ntpServer = "pool.ntp.org";
// defines the offset in seconds between our time zone (EST) and GMT
// EST is 5 hours behind, so the offset is 5*3600= -18000 seconds
const long  gmtOffset_sec = -18000;
const int   daylightOffset_sec = 3600;
String hour, minute, weekDay, month, monthDay, year; 

void setup(){
  Serial.begin(115200);

  // Connect to Wi-Fi
  Serial.print("Connecting to: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  
  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  //getLocalTime(hour,minute,weekDay,month,monthDay,year);
  
}

void loop(){
  if(WiFi.status() == WL_CONNECTED){
    getLocalTime(hour,minute,weekDay,month,monthDay,year);
//    getLocalTime();
    delay(1000);
    Serial.println("Hour: " + hour);
    Serial.println("Minute: " + minute);
    Serial.println("Week Day: " + weekDay);
    Serial.println("Month: " + month);
    Serial.println("Month Day: " + monthDay);
    Serial.println("Year" + year);
  }
}

//Source to get desired formatting and abbreviations for time and date
// https://cplusplus.com/reference/ctime/strftime/

void getLocalTime(String &hour, String &minute, String &weekDay, String &month, String &monthDay, String &year){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
//    struct tm {
//        int8_t          tm_sec; /**< seconds after the minute - [ 0 to 59 ] */
//        int8_t          tm_min; /**< minutes after the hour - [ 0 to 59 ] */
//        int8_t          tm_hour; /**< hours since midnight - [ 0 to 23 ] */
//        int8_t          tm_mday; /**< day of the month - [ 1 to 31 ] */
//        int8_t          tm_wday; /**< days since Sunday - [ 0 to 6 ] */
//        int8_t          tm_mon; /**< months since January - [ 0 to 11 ] */
//        int16_t         tm_year; /**< years since 1900 */
//        int16_t         tm_yday; /**< days since January 1 - [ 0 to 365 ] */
//        int16_t         tm_isdst; /**< Daylight Saving Time flag */
//    };
//  char timeHour[10]; 
  //hour = strftime(timeHour,sizeof(timeHour), "%I", &timeinfo);
  hour = String(timeinfo.tm_hour);

//  char timeMinute[10];
//  minute = strftime(timeMinute,sizeof(timeMinute), "%M", &timeinfo);
  minute = String(timeinfo.tm_min);
  
//  char timeMonth[10];
//  month = strftime(timeMonth,sizeof(timeMonth), "%B", &timeinfo);
    month = String(timeinfo.tm_mon);
//
//  char timeMonthDay[10];
//  monthDay = strftime(timeMonthDay,sizeof(timeMonthDay), "%d", &timeinfo);
    monthDay = String(timeinfo.tm_mday);
//  
//  char timeWeekDay[10];
//  weekDay = strftime(timeWeekDay,sizeof(timeWeekDay), "%A", &timeinfo);
    weekDay = String(timeinfo.tm_wday);
//
//  char timeYear[10];
//  year = strftime(timeYear,sizeof(timeYear), "%Y", &timeinfo);
    year = String(1900 + int(timeinfo.tm_year));
}
//ORIGINAL FUNCTION WITH NO PASS BY REFERENCE
//void getLocalTime(void){
//  struct tm timeinfo;
//  if(!getLocalTime(&timeinfo)){
//    Serial.println("Failed to obtain time");
//    return;
//  }
////  
////  //TO DO: DECIDE WHAT / HOW WE WILL DISPLAY TIME AND DATE
////  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
////  Serial.print("Day of week: ");
////  Serial.println(&timeinfo, "%A");
////  Serial.print("Month: ");
////  Serial.println(&timeinfo, "%B");
////  Serial.print("Day of Month: ");
////  Serial.println(&timeinfo, "%d");
////  Serial.print("Year: ");
////  Serial.println(&timeinfo, "%Y");
////  Serial.print("Hour: ");
////  Serial.println(&timeinfo, "%H");
////  Serial.print("Hour (12 hour format): ");
////  Serial.println(&timeinfo, "%I");
////  Serial.print("Minute: ");
////  Serial.println(&timeinfo, "%M");
////  Serial.print("Second: ");
////  Serial.println(&timeinfo, "%S");
////  Serial.println("Time variables");
////  
//}
