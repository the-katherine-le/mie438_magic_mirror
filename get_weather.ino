#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h> 
#include <TFT_eSPI.h>
#include <SPI.h>
TFT_eSPI tft = TFT_eSPI();

//Comment here your wifi / hotspot credentials for personal testing
const char* ssid = "Katherine";
const char* password = "password123";

// Your Domain name with URL path or IP address with path
// Replace with unique API key
const String openWeatherMapApiKey = "477dd289768ed286827eeb432969043c";

// Replace with your country code and city
const String city = "Toronto";
const String countryCode = "CA";

// THE DEFAULT TIMER IS SET TO 10 SECONDS FOR TESTING PURPOSES
// For a final application, check the API call limits per hour/minute to avoid getting blocked/banned
unsigned long lastTime = 0;

//TO DO: set timer to an hourly interval instead to not exceed API Call limit, timerDelay = 3600*1000
// Set timer to 10 seconds (10000)
unsigned long timerDelay = 10000;
bool initWeatherQuery = false;
String temp, pressure, humidity, wind, description;
//float temp, wind;
//int humidity, pressure;
//String description;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Timer set to 10 seconds (timerDelay variable), it will take 10 seconds before publishing the first reading.");

  //Initialize touch screen
  //  tft.begin();
  //  tft.setRotation(1); //Horizontal
  //  tft.fillScreen(BLACK);
}

void loop() {
  if ((millis() - lastTime) > timerDelay) {
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Getting weather data...");
      getWeather(temp, pressure, humidity, wind, description);
      Serial.print("Temp: ");
      Serial.println(String(temp));
      Serial.print("Pressure: ");
      Serial.println(String(pressure));
      Serial.print("Humidity: ");
      Serial.println(String(humidity));
      Serial.println("Wind Speed: ");
      Serial.print(String(wind));
      Serial.print("Description");
      Serial.println(description);
    }
    else {
      Serial.println("Wifi disconnect...");
    }
    lastTime = millis(); //Just keep incrementing the timer
  }
  else {
    Serial.print(".");
  }
}

String getURLResponse(String url)
{
  HTTPClient http;
  String jsonstring = "";
  Serial.println("getting url: " + url);
  if (http.begin(url))
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

void getWeather(String &temp, String &pressure, String &humidity, String &wind, String &description)
{
  // The filter: it contains "true" for each value we want to keep
  StaticJsonDocument<500> filter;
  filter["list"][0]["main"]["temp"] = true;
  filter["list"][0]["main"]["humidity"] = true;
  filter["list"][0]["main"]["pressure"] = true;
  filter["list"][0]["wind"]["speed"] = true;
  filter["list"][0]["weather"][0]["description"] = true;
  
  StaticJsonDocument<1024> doc;
  String url = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&APPID=" + openWeatherMapApiKey + "&units=metric";
  String jsonWeather = getURLResponse(url);

  if (jsonWeather.length() > 0)
  {
    // remove start and end brackets, jsonBuffer is confused by them
    //jsonWeather = jsonWeather.substring(1, jsonWeather.length() - 1);

    Serial.println("using: " + jsonWeather);
    
    DeserializationError error = deserializeJson(doc, jsonWeather,DeserializationOption::Filter(filter));
    serializeJsonPretty(doc, Serial); //output results

    if (error)
    {
      Serial.println("json parseObject() failed");
      Serial.println("bad json: " + jsonWeather);
      Serial.println("json parseObject() failed");
    }
    else
    {
//      float t_temp = (float)(doc["main"]["temp"]);        // get temperature
//      int   t_humidity = doc["main"]["humidity"];                  // get humidity
//      float t_pressure = (float)(doc["main"]["pressure"]) / 1000;  // get pressure
//      float t_wind_speed = doc["wind"]["speed"];                   // get wind speed
//      String t_description = doc["weather"][0]["description"];
      
      //ISSUE: THIS IS STILL OUTPUTTING AS '0' IN THE SERIAL MONITOR

      String t_temp = doc["main"]["temp"];        // get temperature
      String t_humidity = doc["main"]["humidity"];                  // get humidity
      String t_pressure = doc["main"]["pressure"];  // get pressure
      String t_wind_speed = doc["wind"]["speed"];                   // get wind speed
      String t_description = doc["weather"][0]["description"];

      temp = t_temp;
      humidity = t_humidity;
      pressure = t_pressure;
      wind = t_wind_speed;
      description = t_description;
    }
  }
  else
  {
    Serial.println("Error retrieving URL");
  }
}
