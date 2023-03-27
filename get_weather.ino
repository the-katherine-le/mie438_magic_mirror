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

//Variables
float temp;
int pressure; 
int humidity; 
float wind; 
char description[30];

//Define colors for touchscreen display
#define BLACK 0x0000;
#define WHITE 0xFFFF;

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
    tft.begin();
    tft.setRotation(1); //Horizontal
    tft.fillScreen(BLACK);
}

void loop() {
  if ((millis() - lastTime) > timerDelay) {
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Getting weather data...");
      getWeather(temp, pressure, humidity, wind, description);

      //float &temp, int &pressure, int &humidity, float &wind, const *char &description

      String s_temp = String(temp);
      String s_pressure = String(pressure);
      String s_humidity = String(humidity);
      String s_wind = String(wind);
      //I think description is already a string?
      
      Serial.print("Temp: ");
      Serial.println(s_temp);
      Serial.print("Pressure: ");
      Serial.println(s_pressure);
      Serial.print("Humidity: ");
      Serial.println(s_humidity);
      Serial.println("Wind Speed: ");
      Serial.print(s_wind);
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
  //drawWeather(s_temp,s_pressure,s_humidity,s_wind,description);
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

void getWeather(float &temp, int &pressure, int &humidity, float &wind, char &description[30])
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

    DeserializationError error = deserializeJson(doc, jsonWeather, DeserializationOption::Filter(filter));
    serializeJsonPretty(doc, Serial); //output results

    if (error)
    {
      Serial.println("json parseObject() failed");
      Serial.println("bad json: " + jsonWeather);
      Serial.println("json parseObject() failed");
    }
    else
    {
      JsonObject weather_0 = doc["weather"][0];
      const char* weather_0_description = weather_0["description"]; // "overcast clouds"

      JsonObject main = doc["main"];
      float main_temp = main["temp"];
      int main_pressure = main["pressure"]; // 1017
      int main_humidity = main["humidity"]; // 88

      JsonObject wind = doc["wind"];
      float wind_speed = wind["speed"]; // 4.63

      Serial.println("JSON OUTPUT: ");

      temp = main_temp;
      humidity = main_humidity;
      pressure = main_pressure;
      wind = wind_speed;
      //description = weather_0_description;
      strcpy(description,weather_0_description);
    }
  }
  else
  {
    Serial.println("Error retrieving URL");
  }
}

void drawWeather(String &temp, String &pressure, String &humidity, String &wind, char &description[30]){
  tft.fillScreen(BLACK);
 
  tft.setCursor(100, 10, 2);
  tft.setTextColor(WHITE,BLACK);
  tft.setTextSize(2);
  tft.println("Temperature: "+ temp + "Celsius");

  tft.setCursor(100, 20, 2);
  tft.setTextColor(WHITE,BLACK);
  tft.setTextSize(2);
  tft.println("Pressure: "+ pressure + "hpa");

  tft.setCursor(100, 30, 2);
  tft.setTextColor(WHITE,BLACK);
  tft.setTextSize(2);
  tft.println("Humidity: "+ humidity);
  
  tft.setCursor(100, 40, 2);
  tft.setTextColor(WHITE,BLACK);
  tft.setTextSize(2);
  tft.println("Wind: "+ wind + "m/s");

  tft.setCursor(1000, 50, 2);
  tft.setTextColor(WHITE,BLACK);
  tft.setTextSize(2);
  tft.println("Looks like : "+ description);
}

//SAMPLE FROM ARDUINOJSON ASSISTANT
//ASSISTANT: https://arduinojson.org/v6/assistant/#/step1
//URL: http://api.openweathermap.org/data/2.5/weather?q=Toronto,CA&APPID=477dd289768ed286827eeb432969043c&units=metric
// Stream& input;
//StaticJsonDocument<768> doc;
//
//DeserializationError error = deserializeJson(doc, input);
//
//if (error) {
//  Serial.print(F("deserializeJson() failed: "));
//  Serial.println(error.f_str());
//  return;
//}
//
//float coord_lon = doc["coord"]["lon"]; // -79.4163
//float coord_lat = doc["coord"]["lat"]; // 43.7001
//
//JsonObject weather_0 = doc["weather"][0];
//int weather_0_id = weather_0["id"]; // 804
//const char* weather_0_main = weather_0["main"]; // "Clouds"
//const char* weather_0_description = weather_0["description"]; // "overcast clouds"
//const char* weather_0_icon = weather_0["icon"]; // "04d"
//
//const char* base = doc["base"]; // "stations"
//
//JsonObject main = doc["main"];
//float main_temp = main["temp"]; // 2.44
//float main_feels_like = main["feels_like"]; // -1.66
//float main_temp_min = main["temp_min"]; // 1.31
//float main_temp_max = main["temp_max"]; // 3.4
//int main_pressure = main["pressure"]; // 1017
//int main_humidity = main["humidity"]; // 88
//
//int visibility = doc["visibility"]; // 10000
//
//JsonObject wind = doc["wind"];
//float wind_speed = wind["speed"]; // 4.63
//int wind_deg = wind["deg"]; // 30
//float wind_gust = wind["gust"]; // 8.75
//
//int clouds_all = doc["clouds"]["all"]; // 100
//
//long dt = doc["dt"]; // 1679932268
//
//JsonObject sys = doc["sys"];
//int sys_type = sys["type"]; // 2
//long sys_id = sys["id"]; // 2043365
//const char* sys_country = sys["country"]; // "CA"
//long sys_sunrise = sys["sunrise"]; // 1679915315
//long sys_sunset = sys["sunset"]; // 1679960244
//
//int timezone = doc["timezone"]; // -14400
//long id = doc["id"]; // 6167865
//const char* name = doc["name"]; // "Toronto"
//int cod = doc["cod"]; // 200
