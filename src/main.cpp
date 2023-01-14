#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>



//const char *ssid = "HUAWEI";
//const char *password = "bigtits69";

const char *ssid = "wifiquintal";
const char *password = "totoa1q9";

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

  //----------------------------------------------------GPIO
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
  pinMode(capteurLuminosite, INPUT);

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
