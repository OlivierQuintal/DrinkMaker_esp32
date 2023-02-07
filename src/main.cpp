#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <HX711.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>

//const char *ssid = "HUAWEI";
//const char *password = "bigtits69";

const char *ssid = "wifiquintal";
const char *password = "totoa1q9";

const int led = 2;
const int capteurLuminosite = 34;


//----- Fonctions 

void trouverDrinksPossibles(void);


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

//--------------------------Scan des drinks 
  trouverDrinksPossibles();
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



void trouverDrinksPossibles(void)
{
  //-------------------- afficher le contenue d'un fichier json enregistrer dans le spiffs
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  
  String list_drink_possible;                 // met en string tout les drinks possible 


 File recette_json = SPIFFS.open("/recipes.json","r");     // ouvre le fichier json avec les recettes 

  if (!recette_json) {
    Serial.println("Failed to open file for reading");    // affiche un message d'erruer si le fichier est introuvable 
    return;
  }

  DynamicJsonDocument doc (65535);                        // grosseur du fichier dans le quel nous voulons parser le fichier json
  DeserializationError error = deserializeJson(doc, recette_json);     //  parse du fichier json 
  if (error) {
    Serial.println("Failed to parse file");
    Serial.println("Error: " + String(error.c_str()));
    return;
  }

 String ingredientsAvailable[] = {"Gin", "Vodka", "Vermouth", "White rum", "Lime juice", "Syrup", "Tequila," , "Orange juice" , "Campari"};     // ingédients que nous allons mettre dans la machine 
 int ingredientsCount = sizeof(ingredientsAvailable) / sizeof(ingredientsAvailable[0]);                     // enregistre le nombre d'ingrédients
 Serial.print("nb ingredients :");
 Serial.println(ingredientsCount);

JsonArray drinks = doc.as<JsonArray>();           // créé un array avec le json des recettes 

for (JsonObject drink : drinks) {                 // sépare chacun des drinks pour les analysés
  Serial.println("Drink name: " + drink["name"].as<String>());
  Serial.println("Ingredients:");
  delay(10);

  JsonArray ingredients = drink["ingredients"];     // affiche le nom de l'ingédients 
  bool canMakeDrink = true;
  for (JsonObject ingredient : ingredients) {
    const char *ingredientName = ingredient["ingredient"];
    delay(10);
    bool hasIngredient = false;
    for (int i = 0; i < ingredientsCount; i++) {            // passe en balayage un ingédient à la fois pour voir si nous l'avons ou non 
      if (ingredientsAvailable[i] == ingredientName) {
        hasIngredient = true;
        delay(10);
        break;
      }
    }

    if (!hasIngredient) {
      canMakeDrink = false;
      Serial.println("Missing ingredient: " + String(ingredientName));    // si nous n'avons pas l'ingrédient, il affiche l'ingédient manquant 
    } else {
      Serial.println(ingredientName);             // affiche simplement l'ingrédient si nous l'avons 
    }
  }

  if (canMakeDrink) {                                             // affiche si nous pouvons faire le drinks ou non 
    Serial.println("Nous pouvons faire ce drink ");                        
    list_drink_possible += " , " + drink["name"].as<String>();    // enregistre tout les drinks possible dans un string 
  } else {
    Serial.println("Impossible de faire ce drink ");                     
  }
  Serial.println();
}
Serial.print("Liste des drinks possible : ");
Serial.println(list_drink_possible);            // affiche la liste des drinks qui est possible d'etre fait 

 recette_json.close();
}