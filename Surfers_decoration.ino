/*
    Manifesting and Embodying Data From the Internet:

    Decoration for surfers

    We've created a Decoration that indicates live wave height and wind speed
    
    This sketch connects to the internet to fetch real-time weather data (wind speed and wave height) from OpenWeatherMap and Stormglass APIs. 
    The ESP32 controls a NeoPixel LED strip and a servo motor based on the fetched data.
    The LED strip displays wave heights using moving blue LEDs (resembles the waves),
    while the servo motor adjusts its position according to the wind speed.
    The program continuously updates the data every hour.

    In order to use the script please fill in your Wifi credentials
    If your wifi has a password, also change line 148

    The circuit:
    * Input:
        - WiFi connection to fetch data from the internet.
    * Output:
        - NeoPixel LED strip connected to pin 26.
        - Servo motor connected to pin 23.

    Video link: https://www.youtube.com/watch?v=dNDxYnmZQeg
    Created By:
    Oren Savich 
    Rotem Peled 
*/

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>
#include <string.h>
#include <ESP32Servo.h>

// WiFi Credentials
const char* ssid = "Your_SSID";
const char* pass = "Your_Password";

#define CONNECTION_TIMEOUT_SECS 20 //timeout to restart ESP32 when Wifi not found

#define NUM_LEDS 12 //number of led lights in our strip
#define LED_PIN 26 //pin connected to led strip
#define SERVO_PIN 23 //pin connected to servo

String openWeatherMapApiKey = ""; //Add Open Weather Map Api Key
String lat = "32.178698389979665"; //Herzelia 
String lon = "34.83778824811556";

String stormglassApiKey = ""; //Add Stormglass API key 
String stormglassLat = "58.7984"; // Herzelia
String stormglassLon = "17.8081"; 

unsigned long lastTime = 0;
unsigned long timerDelay = 10000;
String jsonBuffer;

unsigned long previousMillis = 0; // Store the last time the info was updated
unsigned long info_interval = 3600000; // refresh info every hour

unsigned long previousLedMillis = 0;
unsigned long led_interval = 300;

float wind = 0; //measures wind speed   range 0 - 20
float wave = 0; //measures wave height    range 0 - 4    every led is 0.5    0.5-1:begginers 1-2:intermediate 2-3:advanced

// Create NeoPixel object
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// creates servo object
Servo myservo;

void setup() 
{
  Serial.begin(115200);
  strip.begin();// Initialize the LED strip
  strip.show(); // Initialize all pixels to 'off'
  strip.setBrightness(50);//originally was 150 

  //setup servo
  ESP32PWM::allocateTimer(0); // Allocate timers for the Servo library
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  myservo.setPeriodHertz(50); // Standard 50 Hz servo
  myservo.attach(SERVO_PIN, 500, 2500); // Attach the servo to the specified pin

  connect_to_Wifi();
  wind = fetch_wind_speed(); //range 0 - 20 
  wave = 3;
  //wave = fetch_wave_height(); // range 0 - 4    every led is 0.5    0.5-1:begginers 1-2:intermediate 2-3:advanced
  activate_servo();
  light_led_strip();
}

void loop() 
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= info_interval) //refresh wave and wind data every hour
  {
    previousMillis = currentMillis;
    wind = fetch_wind_speed(); //range 0 - 20 
    wave = fetch_wave_height(); // range 0 - 4    every led is 0.5    0.5-1:begginers 1-2:intermediate 2-3:advanced

    activate_servo();
  }

  if (currentMillis - previousLedMillis >= led_interval) // refresh lighting to give a wave resemblance
  {
    previousLedMillis = currentMillis;
    light_led_strip(); 
  }
  
}

//from json sketchbook
String getJsonContent(const char* url) 
{
  String payload = "{}";
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient https;
  https.begin(client, url);
  int rc = https.GET();
  if (rc > 0) 
  {
    Serial.printf("HTTP Response code: %d\n", rc);
    payload = https.getString();
  }
  else 
  {
    Serial.printf("Error code: %d\n", rc);
  }
  https.end();
  return payload;
}

//connects to wifi, from wifi sketchbook
void connect_to_Wifi()
{
//  WiFi.begin(ssid, password);//use when there's a password
  WiFi.begin(ssid);//use when no password needed
  Serial.println("Connecting");
  
  long started_looking_for_wifi = millis();
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
    
    if ((millis() - started_looking_for_wifi) >= CONNECTION_TIMEOUT_SECS * 1000) 
    {
      Serial.print("Restarting ESP32");
      ESP.restart();
    }
  }
  
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}

float fetch_wind_speed()
{
  //Serial.println("Fetching wind speed:");
  float wind_speed = -1; //default value for issues
  String url = "https://api.openweathermap.org/data/2.5/weather?lat=" + lat + "&lon=" + lon + "&appid=" + openWeatherMapApiKey;
  jsonBuffer = getJsonContent(url.c_str());
  const size_t capacity = JSON_OBJECT_SIZE(3) + 1000;
  DynamicJsonDocument myObject(capacity);
  DeserializationError error = deserializeJson(myObject, jsonBuffer);
  if (error) {
    Serial.print("Failed to parse JSON: ");
    Serial.println(error.c_str());
    return wind_speed;
  }
//  Serial.print("Temperature: ");
//  Serial.println(myObject["main"]["temp"].as<float>());
//  Serial.print("Pressure: ");
//  Serial.println(myObject["main"]["pressure"].as<float>());
//  Serial.print("Humidity: ");
//  Serial.println(myObject["main"]["humidity"].as<float>());
  Serial.print("Wind Speed: ");
  wind_speed = myObject["wind"]["speed"].as<float>();
  Serial.println(wind_speed);
  
  return wind_speed;
}


//Using stormglass API to fetch wave height
float fetch_wave_height() {
  Serial.println("Fetching wave height:");
  float waveHeight = -1;
  String current_time = fetch_time();
  String url = "https://api.stormglass.io/v2/weather/point?lat=" + stormglassLat + "&lng=" + stormglassLon + "&params=waveHeight&source=sg&start=" + current_time + "&end=" + current_time;
  String payload = "{}";

  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient https;

  https.begin(client, url);
  https.addHeader("Authorization", stormglassApiKey);
  int rc = https.GET();

  if (rc > 0) {
    Serial.printf("HTTP Response code: %d\n", rc);
    payload = https.getString();
    //Serial.println("Received payload:");
    //Serial.println(payload); // Print the entire payload

    // Parse JSON response
    StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, payload);

    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.f_str());
      return waveHeight;
    }

    // Extract the wave height
    waveHeight = doc["hours"][0]["waveHeight"]["sg"];
    String time = doc["hours"][0]["time"];
    //Serial.print("Latest Time: ");
    //Serial.println(time);
    Serial.print("Wave Height (SG): ");
    Serial.println(waveHeight);
  } else {
    Serial.printf("Error code: %d\n", rc);
  }

  https.end();
  return waveHeight;
}

//Fetches current time from worldtimeapi api
String fetch_time()
{
  String url = "https://worldtimeapi.org/api/timezone/Europe/Athens";
  jsonBuffer = getJsonContent(url.c_str());
  const size_t capacity = JSON_OBJECT_SIZE(3) + 1000;
  DynamicJsonDocument myObject(capacity);
  DeserializationError error = deserializeJson(myObject, jsonBuffer);
  if (error) {
    Serial.print("Failed to parse JSON: ");
    Serial.println(error.c_str());
    return "";
  }

  //Serial.print("time: ");
  //Serial.println(myObject["datetime"].as<String>());

  String original = myObject["datetime"].as<String>();
  String extracted = original.substring(0, 13);

  //Serial.println("extracted:");
  //Serial.println(extracted + ":00");
  return extracted + ":00";
}

//lights the led strip acording to the wave height
//Makes the leds looking like waves
void light_led_strip()
{
  // Turn off all LEDs first
  strip.clear();
  
  // Calculate the number of LEDs to light up based on the wave height
  int numLedsToLight = wave * (NUM_LEDS / 4.0); // Each 0.5 in wave lights up one additional LED

  // Light up the calculated number of LEDs
  for (int i = 0; i < numLedsToLight; i++) 
  {
    strip.setPixelColor(NUM_LEDS - 1 - i, strip.Color(0, 0, rand() % 255)); // Blue color
    strip.show();
  }
}

//activates the servo in sync with the wind speed
void activate_servo()
{
  // Map the wind speed to the servo position (0-180 degrees)
  int servoPosition = map(wind, 0, 20, 0, 180); //up to 20 kmh

  // Set the servo position based on the wind speed
  myservo.write(180 - servoPosition);
}
