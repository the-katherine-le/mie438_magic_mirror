#include <HTTPClient.h>
#include <ArduinoJson.h>   

#define WIFI_SSID "Katherine"
#define WIFI_PASSWORD "password123"
#define WIFI_TIMEOUT 30

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  //while(!Serial);

  Serial.print("Connecting to WiFi ");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  int counter = 0;
  while (WiFi.status() != WL_CONNECTED && counter < WIFI_TIMEOUT) {
    delay(1000);
    Serial.print(".");
    counter++;
  }
  
  String quote, author;
  if(WiFi.status() == WL_CONNECTED)
  {
    Serial.println("connected");
    getQuote(quote, author);  
  }
  else
  {
    quote = "WiFi connection timed out, try again later";
    Serial.println(quote);
  }
  
//  printQuote(String("\"") + quote + String("\""));
//  printAuthor(author);
//  printOther("adafruit.com/quotes");
  
//  epd.display();
//  Serial.println("done, going to sleep...");
//  // power down ePaper display
//  epd.powerDown();
//  // put microcontroller to sleep, wake up after specified time
//  ESP.deepSleep(SLEEP * 1e6);

}

void loop() {
  // put your main code here, to run repeatedly:
  //Serial.println(quote);

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
