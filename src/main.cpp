#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <HX711.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>

const char *ssid = "HUAWEI";
const char *password = "bigtits69";

//const char *ssid = "wifiquintal";
//const char *password = "totoa1q9";

const int led = 2;
const int capteurLuminosite = 34;





//-----Variable pour les ingrédients dans la machine 

String PompeNo1 = "";
String PompeNo2 = "";
String PompeNo3 = "";
String PompeNo4 = "";
String PompeNo5 = "";
String PompeNo6 = "";
String PompeNo7 = "";
String PompeNo8 = "";

AsyncWebServer server(80);


void setup()
{
  //----------------------------------------------------Serial
  Serial.begin(115200);
  Serial.println("\n");


  // ----------------- ---------------------------------test json

  //-------------test no1 faire une recherche 
  const __FlashStringHelper* input_json = F(
      "{\"cod\":\"200\",\"message\":0,\"list\":[{\"dt\":1581498000,\"main\":{"
      "\"temp\":3.23,\"feels_like\":-3.63,\"temp_min\":3.23,\"temp_max\":4.62,"
      "\"pressure\":1014,\"sea_level\":1014,\"grnd_level\":1010,\"humidity\":"
      "58,\"temp_kf\":-1.39},\"weather\":[{\"id\":800,\"main\":\"Clear\","
      "\"description\":\"clear "
      "sky\",\"icon\":\"01d\"}],\"clouds\":{\"all\":0},\"wind\":{\"speed\":6."
      "19,\"deg\":266},\"sys\":{\"pod\":\"d\"},\"dt_txt\":\"2020-02-12 "
      "09:00:00\"},{\"dt\":1581508800,\"main\":{\"temp\":6.09,\"feels_like\":-"
      "1.07,\"temp_min\":6.09,\"temp_max\":7.13,\"pressure\":1015,\"sea_"
      "level\":1015,\"grnd_level\":1011,\"humidity\":48,\"temp_kf\":-1.04},"
      "\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"clear "
      "sky\",\"icon\":\"01d\"}],\"clouds\":{\"all\":9},\"wind\":{\"speed\":6."
      "64,\"deg\":268},\"sys\":{\"pod\":\"d\"},\"dt_txt\":\"2020-02-12 "
      "12:00:00\"}],\"city\":{\"id\":2643743,\"name\":\"London\",\"coord\":{"
      "\"lat\":51.5085,\"lon\":-0.1257},\"country\":\"GB\",\"population\":"
      "1000000,\"timezone\":0,\"sunrise\":1581492085,\"sunset\":1581527294}}");


      // The filter: it contains "true" for each value we want to keep
  StaticJsonDocument<200> filter;
  filter["list"][0]["dt"] = true;
  filter["list"][0]["main"]["temp"] = true;

  // Deserialize the document
  StaticJsonDocument<400> doc;
  deserializeJson(doc, input_json, DeserializationOption::Filter(filter));

  // Print the result
  serializeJsonPretty(doc, Serial);



//-------------------- afficher le contenue d'un fichier json enregistrer dans le spiffs
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  
  File fichier = SPIFFS.open("/test.json");
  if(!fichier){
    Serial.println("Failed to open file for reading");
    return;
  }
  int frite ; 
  Serial.println("File Content:");
  while(fichier.available()){
    frite = fichier.read();
    Serial.write(frite);
  }


// ------- trier le fichier json 
  StaticJsonDocument<2000> filtre;
  filtre["noDrink"] = true;

/*
  // Deserialize the document
  StaticJsonDocument<4000> doc2;
  deserializeJson(doc2, frite, DeserializationOption::Filter(filtre));

  // Print the result
  serializeJsonPretty(doc2, Serial);

*/
  fichier.close();




  //----------------------------------------------------GPIO
    pinMode(led, OUTPUT);
    digitalWrite(led, LOW);
    pinMode(capteurLuminosite, INPUT);

  // ---GIOP solenoide
    #define solenoide_1 6
    #define solenoide_2 7
    #define solenoide_3 10
    #define solenoide_4 11
    #define solenoide_5 12
    #define solenoide_6 13
    #define solenoide_7 14
    #define solenoide_8 15
    #define solenoide_9 16
    #define solenoide_10 17

    pinMode(solenoide_1, OUTPUT);
    pinMode(solenoide_2, OUTPUT);
    pinMode(solenoide_3, OUTPUT);
    pinMode(solenoide_4, OUTPUT);
    pinMode(solenoide_5, OUTPUT);
    pinMode(solenoide_6, OUTPUT);
    pinMode(solenoide_7, OUTPUT);
    pinMode(solenoide_8, OUTPUT);
    pinMode(solenoide_9, OUTPUT);
    pinMode(solenoide_10, OUTPUT);

  // ---GPIO Pompes
    #define pompe_1 1
    #define pompe_2 2

  // ---GPIO boutons
    #define btn_1 40
    #define btn_2 39
    #define btn_3 38

  //------------------------------------------------------HX711
  HX711 scale;
  #define HX711_SDA 42
  #define HX711_SCK 41
  scale.begin(HX711_SDA, HX711_SCK);       

  //------------------------------------------------------LCD 
  LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

  //------------------------------------------------------Capteur Alcool
  #define capteur_alcool 18
  pinMode(capteur_alcool, INPUT);
  
  //----------------------------------------------------SPIFFS
  if (!SPIFFS.begin())
  {
    Serial.println("Erreur SPIFFS...");
    return;
  }

  File root = SPIFFS.open("/");
  File file = root.openNextFile();

  //  fait juste afficher le nom de chacun des fichiers sur le moniteur série 
  while (file)
  {
    Serial.print("File: ");
    Serial.println(file.name());
    file.close();
    file = root.openNextFile();
  }



  

  //----------------------------------------------------WIFI
  WiFi.begin(ssid, password);
  Serial.print("Tentative de connexion...");

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(100);
  }

  Serial.println("\n");
  Serial.println("Connexion etablie!");
  Serial.print("Adresse IP: ");
  Serial.println(WiFi.localIP());

  //----------------------------------------------------SERVER
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", "text/html");
  });

  server.on("/w3.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/w3.css", "text/css");
  });

  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/script.js", "text/javascript");
  });
  

  server.on("/jquery-3.6.0.min.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/jquery-3.6.0.min.js", "text/javascript");
  });

// pas sur que c'est bon ce que je viens de rajouter **********
  server.on("/recipes.json", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/recipes.json", "text/plain");
  });


  server.on("/calibrationPompes", HTTP_POST, [](AsyncWebServerRequest *request) {     // recuille les boissons que nous avons mis dans chacunes des pompes 
    if(request->hasParam("PompeNo1", true))
    {
      PompeNo1 = request->getParam("PompeNo1", true)->value();
    }
    if(request->hasParam("PompeNo2", true))
    {
      PompeNo2 = request->getParam("PompeNo2", true)->value();
    }
    if(request->hasParam("PompeNo3", true))
    {
      PompeNo3 = request->getParam("PompeNo3", true)->value();
    }
    if(request->hasParam("PompeNo4", true))
    {
      PompeNo4 = request->getParam("PompeNo4", true)->value();
    }
    if(request->hasParam("PompeNo5", true))
    {
      PompeNo5 = request->getParam("PompeNo5", true)->value();
    }
    if(request->hasParam("PompeNo6", true))
    {
      PompeNo6 = request->getParam("PompeNo6", true)->value();
    }
    if(request->hasParam("PompeNo7", true))
    {
      PompeNo7 = request->getParam("PompeNo7", true)->value();
    }
    if(request->hasParam("PompeNo8", true))
    {
      PompeNo8 = request->getParam("PompeNo8", true)->value();
    }
    request->send(204);
  });

  server.begin();
  Serial.println("Serveur actif!");


}
//---------------------------------------------------------------------------


void loop()
{

  Serial.println(PompeNo1);
  Serial.println(PompeNo2);
  Serial.println(PompeNo3);
  Serial.println(PompeNo4);
  Serial.println(PompeNo5);
  Serial.println(PompeNo6);
  Serial.println(PompeNo7);
  Serial.println(PompeNo8);

  
  delay(1000);

}
