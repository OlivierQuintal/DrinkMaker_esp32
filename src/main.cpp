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

const char *ssid = "RT-AC1200_E0_2G";
const char *password = "eagle_4742";


//const char *ssid = "wifiquintal";
//const char *password = "totoa1q9";

const int led = 2;
const int capteurLuminosite = 34;

String drink_voulue;

//----- Fonctions 

void trouverDrinksPossibles(void);
void ingredientsDuDrink(String drink_voulue);
String tableauDrinkPossible[50];      // augmenter cette valeur pour augmenter la valeur max de drink possible 
String ingerdinetDrinkChoisi[10];      // augmenter cette valeur pour augmenter la valeur max d'ingrédient par drink
String quantite[10];

DynamicJsonDocument doc (65535);      // document dans le quel nous allons parse le fichier reccipes.json

//-----Variable pour les ingrédients dans la machine 

String BouteilleNo1 = "";
String BouteilleNo2 = "";
String BouteilleNo3 = "";
String BouteilleNo4 = "";
String BouteilleNo5 = "";
String BouteilleNo6 = "";
String BouteilleNo7 = "";
String BouteilleNo8 = "";
String BouteilleNo9 = "";
String BouteilleNo10 = "";
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

  server.on("/page2", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index1.html", "text/html");
    Serial.println("sdfjsgdfuysdgfosudfgsdifyg");
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

  server.on("/page2/AfficheDrink", HTTP_GET, [](AsyncWebServerRequest *request) {
    String valeur = "patate";
    Serial.println("paatteee");
    request->send(200, "text/plain", valeur);
  });

  server.on("/calibrationPompes", HTTP_POST, [](AsyncWebServerRequest *request) {     // recuille les boissons que nous avons mis dans chacunes des pompes 
    if(request->hasParam("BouteilleNo1", true))
    {
      BouteilleNo1 = request->getParam("BouteilleNo1", true)->value();
    }
    if(request->hasParam("BouteilleNo2", true))
    {
      BouteilleNo2 = request->getParam("BouteilleNo2", true)->value();
    }
    if(request->hasParam("BouteilleNo3", true))
    {
      BouteilleNo3 = request->getParam("BouteilleNo3", true)->value();
    }
    if(request->hasParam("BouteilleNo4", true))
    {
      BouteilleNo4 = request->getParam("BouteilleNo4", true)->value();
    }
    if(request->hasParam("BouteilleNo5", true))
    {
      BouteilleNo5 = request->getParam("BouteilleNo5", true)->value();
    }
    if(request->hasParam("BouteilleNo6", true))
    {
      BouteilleNo6 = request->getParam("BouteilleNo6", true)->value();
    }
    if(request->hasParam("BouteilleNo7", true))
    {
      BouteilleNo7 = request->getParam("BouteilleNo7", true)->value();
    }
    if(request->hasParam("BouteilleNo8", true))
    {
      BouteilleNo8 = request->getParam("BouteilleNo8", true)->value();
    }
    if(request->hasParam("BouteilleNo9", true))
    {
      BouteilleNo9 = request->getParam("BouteilleNo9", true)->value();
    }
    if(request->hasParam("BouteilleNo10", true))
    {
      BouteilleNo10 = request->getParam("BouteilleNo10", true)->value();
    }
    trouverDrinksPossibles();       // une fois que le bouton APPLIQUER est appuyer, affiche la liste des drink possible d'etre fait  
    ingredientsDuDrink(drink_voulue);     // test
    request->send(204);
  });

  server.begin();
  Serial.println("Serveur actif!");

 
}
//---------------------------------------------------------------------------


void loop()
{

  Serial.println(BouteilleNo1);
  Serial.println(BouteilleNo2);
  Serial.println(BouteilleNo3);
  Serial.println(BouteilleNo4);
  Serial.println(BouteilleNo5);
  Serial.println(BouteilleNo6);
  Serial.println(BouteilleNo7);
  Serial.println(BouteilleNo8);
  Serial.println(BouteilleNo9);
  Serial.println(BouteilleNo10);

  Serial.println("ok");
  
  delay(1000);

}


//----------------------------------------------------------
//          FONCTION TROUVER_DRINK_POSSIBLE
//  Cette focntion passe en scan la database de tous les 
//  drinks et met en mémoire dans un tableau les drins 
//  qui sont possible d'être fait avec les ingédients que 
//  nous avons mis dans la machine 
//----------------------------------------------------------
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

  //DynamicJsonDocument doc (65535);                        // grosseur du fichier dans le quel nous voulons parser le fichier json
  DeserializationError error = deserializeJson(doc, recette_json);     //  parse du fichier json 
  doc.shrinkToFit();                                                  // réduit la grosseur du fichier json pour qu'il soit plus petit
  if (error) {
    Serial.println("Failed to parse file");
    Serial.println("Error: " + String(error.c_str()));
    return;
  }

  //String ingredientsAvailable[] = {"Gin", "Vodka", "Vermouth", "White rum", "Lime juice", "Syrup", "Tequila," , "Orange juice" , "Campari"};     // ingédients que nous allons mettre dans la machine 
  String ingredientsAvailable[] = {BouteilleNo1, BouteilleNo2, BouteilleNo3, BouteilleNo4, BouteilleNo5, BouteilleNo6, BouteilleNo7 , BouteilleNo8 , BouteilleNo9, BouteilleNo10};
  int ingredientsCount = sizeof(ingredientsAvailable) / sizeof(ingredientsAvailable[0]);                     // enregistre le nombre d'ingrédients
  Serial.print("nb ingredients :");
  Serial.println(ingredientsCount);

  JsonArray drinks = doc.as<JsonArray>();           // créé un array avec le json des recettes 
  int j = 0;                                  // compteur pour placer les drinks dans un tableau 
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
      for (int i = 0; i < ingredientsCount; i++) {            // passe en balayage un ingédient à la fois dans le breuvage X pour voir si nous l'avons ou non 
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

      tableauDrinkPossible[j] = drink["name"].as<String>();         // place les drink possible dans un tableau pour pouvoir les utiliser plus tard 
      j++;                                                      

    } else {
      Serial.println("Impossible de faire ce drink ");                     
    }
    Serial.println();
  }
  Serial.print("Liste des drinks possible : ");
  Serial.println(list_drink_possible);            // affiche la liste des drinks qui est possible d'etre fait 

  Serial.print("Liste dans le tableau 0 : ");
  Serial.println(tableauDrinkPossible[0]);            // affiche la liste des drinks qui est possible d'etre fait
  Serial.print("Liste dans le tableau 1: "); 
  Serial.println(tableauDrinkPossible[1]);
  Serial.print("Liste dans le tableau 2: ");
  Serial.println(tableauDrinkPossible[2]);
  Serial.print("Liste dans le tableau 3: ");
  Serial.println(tableauDrinkPossible[3]);

  recette_json.close();
}

//----------------------------------------------------------
//          FONCTION INGREDIENT_DU_DRINK
//  Cette focntion met dans un tableau les ingrédients 
//  du drink que l'utilisateur à choisi
//----------------------------------------------------------

void ingredientsDuDrink(String drink_voulue)      // mettre dans la fonction le drink voulue par l'utilisateur
{
  
  //String drink = drink_voulue;     // décommanter lors des vrai test ************************************************
  String breuvage = "Planter's Punch";
  bool breauvageTrouver = false;

  JsonArray drinks2 = doc.as<JsonArray>();           // créé un array avec le json des recettes
  for (JsonObject drink2 : drinks2)
  {
    if (drink2["name"] == breuvage)
    {
      breauvageTrouver = true;
      //break;
      
    }
    else 
    {
      breauvageTrouver = false;
    }  



    if (breauvageTrouver == true)
    {
      Serial.println("Le drink a été trouver");

      JsonArray ingredients2 = drink2["ingredients"];     // affiche le nom de l'ingédients 
    int i = 0;
    for (JsonObject ingredient2 : ingredients2) 
      {
        String rechercheDuDrink = ingredient2["ingredient"];
        String rechercheQuantite = ingredient2["amount"];
        ingerdinetDrinkChoisi[i] = rechercheDuDrink;
        quantite[i] = rechercheQuantite;
        i++; 
        
        // mettre le code pour mettre les ingrédients dans un tableau ****************************************************
      }
      break;
    }
    
  }

  Serial.println("Ingredient dans le drink choisi : ");
  Serial.println(ingerdinetDrinkChoisi[0] + " " + quantite[0]);
  Serial.println(ingerdinetDrinkChoisi[1] + " " + quantite[1]);
  Serial.println(ingerdinetDrinkChoisi[2] + " " + quantite[2]);
  Serial.println(ingerdinetDrinkChoisi[3] + " " + quantite[3]);
  Serial.println(ingerdinetDrinkChoisi[4] + " " + quantite[4]);




 

}

/*
------------------choses à faire------------------------------------

- Quand on entre une bouteille dans le site, le met directement dans le tableau ingredientsAvailable[] --------------------- fait 
- Attendre que le btn appliquer sois appuyer avant de donner les drinks possible  ------------------------------------------ fait 
- Savoir la quantier de chacun des ingédients et la mettre dans une variables     ------------------------------------------ fait
- Mettre les ingrédients dans des variables pour les comparer a ceux des pompes   ------------------------------------------ fait
- Mettre le nom des drinks possible dans des variables (pt le mettre dans un tableau)  ------------------------------------- fait 

- faire la partie affichage des drink possible sur le site web et leurs ingrédients 
- mettre une btn back sur le site 

*/