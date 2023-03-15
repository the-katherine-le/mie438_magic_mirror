#include <WiFi.h>
#include "time.h"
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include <ArduinoJson.h>//why are there TWO different JSON libraries
  
TFT_eSPI tft = TFT_eSPI(); //GUI

#define WIFI_SSID "Katherine"
#define WIFI_PASSWORD "WIFI_PASSWORD123"
#define WIFI_TIMEOUT 30

////DATE AND TIME SETUP//////////////////////////////////////////////////////////////////////////////
const char* ntpServer = "pool.ntp.org";
// defines the offset in seconds between our time zone (EST) and GMT
// EST is 5 hours behind, so the offset is 5*3600= -18000 seconds
const long  gmtOffset_sec = -18000;
const int   daylightOffset_sec = 3600;

////WEATHER/////////////////////////////////////////////////////////////////////////////////////////
// Your Domain name with URL path or IP address with path
// Replace with unique API key
String openWeatherMapApiKey = "477dd289768ed286827eeb432969043c";

// Replace with your country code and city
String city = "Toronto";
String countryCode = "CA";

// THE DEFAULT TIMER IS SET TO 10 SECONDS FOR TESTING PURPOSES
// For a final application, check the API call limits per hour/minute to avoid getting blocked/banned
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
//TO DO: set timer to an hourly interval instead to not exceed API Call limit, timerDelay = 3600*1000
// Set timer to 10 seconds for testing purposes (10000)
unsigned long timerDelay = 10000;
//Parse JSON string
String jsonBuffer;

////QUOTE//////////////////////////////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  Serial.print("Connecting to: ");
  Serial.println(WIFI_SSID);
  int counter = 0; //For timeout
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  while (WiFi.status() != WL_CONNECTED && counter < WIFI_TIMEOUT) {
    delay(500);
    Serial.print(".");
    counter++;
  }
  
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println(WiFi.localIP());

  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  

  //Getting the inspirational quote
  String quote, author;
  
  if(WiFi.status() == WL_CONNECTED)
  {
    Serial.println("connected");
    getQuote(quote, author); //get quote
    
  }
  else
  {
    quote = "WiFi connection timed out, try again later";
    Serial.println(quote);
  }

  delay(1000);
  printLocalTime();
  delay(1000);
  getWeather(); 

}

void loop() {
  // put your main code here, to run repeatedly:

}

//GET TIME AND DATE///////////////////////////////////////////////////////////////////////////////////////
void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }

  //TO DO: DECIDE WHAT / HOW WE WILL DISPLAY TIME AND DATE
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  Serial.print("Day of week: ");
  Serial.println(&timeinfo, "%A");
  Serial.print("Month: ");
  Serial.println(&timeinfo, "%B");
  Serial.print("Day of Month: ");
  Serial.println(&timeinfo, "%d");
  Serial.print("Year: ");
  Serial.println(&timeinfo, "%Y");
  Serial.print("Hour: ");
  Serial.println(&timeinfo, "%H");
  Serial.print("Hour (12 hour format): ");
  Serial.println(&timeinfo, "%I");
  Serial.print("Minute: ");
  Serial.println(&timeinfo, "%M");
  Serial.print("Second: ");
  Serial.println(&timeinfo, "%S");

  Serial.println("Time variables");
  char timeHour[3];
  strftime(timeHour, 3, "%H", &timeinfo);
  Serial.println(timeHour);
  char timeWeekDay[10];
  strftime(timeWeekDay, 10, "%A", &timeinfo);
  Serial.println(timeWeekDay);
  Serial.println();

}

void getWeather() {
  // Send an HTTP GET request
  if ((millis() - lastTime) > timerDelay) {
    // Check WiFi connection status
    if (WiFi.status() == WL_CONNECTED) {
      String serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&APPID=" + openWeatherMapApiKey + "&units=metric";

      jsonBuffer = httpGETRequest(serverPath.c_str());
      Serial.println(jsonBuffer);
      JSONVar myObject = JSON.parse(jsonBuffer);

      // JSON.typeof(jsonVar) can be used to get the type of the var
      if (JSON.typeof(myObject) == "undefined") {
        Serial.println("Parsing input failed!");
        return;
      }

      Serial.print("JSON object = ");
      Serial.println(myObject);
      Serial.print("Temperature: ");
      Serial.println(myObject["main"]["temp"]);
      Serial.print("Pressure: ");
      Serial.println(myObject["main"]["pressure"]);
      Serial.print("Humidity: ");
      Serial.println(myObject["main"]["humidity"]);
      Serial.print("Wind Speed: ");
      Serial.println(myObject["wind"]["speed"]);
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}

////HTTP REQUEST AND JSON PARSER///////////////////////////////////////////////////////////////////////////////////
String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;

  // Your Domain name with URL path or IP address with path
  http.begin(client, serverName);

  // Send HTTP POST request
  int httpResponseCode = http.GET();

  String payload = "{}";

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}

String getURLResponse(String url)
{
  HTTPClient http;
  String jsonstring = "";
  Serial.println("getting url: " + url);
  if(http.begin(url))
  {
    Serial.print("[HTTP] GET...\n");
    // start connection and send HTTP header
    int httpCode = http.GET();

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been sent and Server response header has been handled
      Serial.println("[HTTP] GET... code: " + String(httpCode));

      // file found at server
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
      jsonstring = http.getString();
      // use this string for testing very long quotes
      //jsonstring = "[{\"text\":\"Don't worry about what anybody else is going to do… The best way to predict the future is to invent it. Really smart people with reasonable funding can do just about anything that doesn't violate too many of Newton's Laws!\",\"author\":\"Alan Kay\"}]";
      Serial.println(jsonstring);
    }
    } else {
      Serial.println("[HTTP] GET... failed, error: " + http.errorToString(httpCode));
    }
    http.end();
  }
  else {
    Serial.println("[HTTP] Unable to connect");
  }
  return jsonstring;
}

void getQuote(String &quote, String &author)
{
  StaticJsonDocument<1024> doc;
  String url = "https://www.adafruit.com/api/quotes.php";
  String jsonquote = getURLResponse(url);
  if(jsonquote.length() > 0)
  {
    // remove start and end brackets, jsonBuffer is confused by them
    jsonquote = jsonquote.substring(1,jsonquote.length()-1);
    Serial.println("using: " + jsonquote);
    DeserializationError error = deserializeJson(doc, jsonquote);
    if (error) 
    {
      Serial.println("json parseObject() failed");
      Serial.println("bad json: " + jsonquote);
      quote = "json parseObject() failed";
    }
    else
    {
      String tquote = doc["text"];
      String tauthor = doc["author"];
      quote = tquote;
      author = tauthor;

      Serial.println(quote);
      Serial.println(author);
    }
  }
  else
  {
    quote = "Error retrieving URL";
  }
}
