// branch pour détecter plusieurs wifi 

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <HX711.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include <WiFi.h>
#include <preferences.h>

// const char *ssid = "HUAWEI";
// const char *password = "bigtits69";

// const char *ssid = "RT-AC1200_E0_2G";
// const char *password = "eagle_4742";

//const char *ssid = "omega";
//const char *password = "Rougepomme";

// const char *ssid = "wifiquintal";
// const char *password = "totoa1q9";

String ssid = "";
String password = "";

const char *soft_ap_ssid          = "DRINK_MAKER";
const char *soft_ap_password      = NULL;



// preference (flash)
Preferences preferences;

const int led = 2;
//const int capteurLuminosite = 34;

String drink_voulue;


int enregistrement_bouteille = 0;

//----- Fonctions 

void trouverDrinksPossibles(void);
void ingredientsDuDrink(String drink_voulue);
String tableauDrinkPossible[50];      // augmenter cette valeur pour augmenter la valeur max de drink possible 
String ingerdinetDrinkChoisi[10];      // augmenter cette valeur pour augmenter la valeur max d'ingrédient par drink
String listeBreuvageHTML = "";      
String quantite[10];
uint32_t Wheel(byte WheelPos);
void menuLCD (void);
float readAlcool(void);
void calibrationLoadCell(void);
void trouverCentilitre(void);
void melange(void);
// void saveBouteillesEEPROM(void);
// void readBouteilleEEPROM(void);

// enregistrer bouteilles dans la flash
void saveBouteillesFlash(void);
void readBouteilleFlash(void);



DynamicJsonDocument doc (65535);      // document dans le quel nous allons parse le fichier reccipes.json

//-----Variables---------

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

int etat_menu = 0 ;
int menu_pompe_manuelle = 1;
int serving_On = 0;

String AP_IP = "";
int ServeurAP_ON = 0;


//------INITIALLISATION DES LIBRAIRES
LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display
HX711 scale;

float calibration = 1086.66;



void setup()
{
  //----------------------------------------------------Serial
  Serial.begin(115200);
  EEPROM.begin(1500);
  Serial.println("\n");
  

  //----------------------------------------------------GPIO
    pinMode(led, OUTPUT);
    digitalWrite(led, LOW);
    //pinMode(capteurLuminosite, INPUT);

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
    #define buzzer 2

    pinMode(pompe_1, OUTPUT);
    pinMode(buzzer, OUTPUT);


  // ---GPIO boutons
    #define btn_1 38
    #define btn_2 39
    #define btn_3 40

    pinMode(btn_1, INPUT);
    pinMode(btn_2, INPUT);
    pinMode(btn_3, INPUT);

  // -- GPIO LEDS
  #define led_rouge 19  // allume si le wifi est déconnecter 
  #define led_bleu 20   //allume si le verre n'est pas présent 
  
  pinMode(led_rouge, OUTPUT);
  pinMode(led_bleu, OUTPUT);

  //------------------------------------------------------HX711

  #define HX711_SDA 42
  #define HX711_SCK 41
  scale.begin(HX711_SDA, HX711_SCK);  


  scale.set_scale(10000);   // valeur random pour la valeur de valibration de la balance


  //---------------------------------------------------------------------------------Capteur Alcool
  #define capteur_alcool 18   
  pinMode(capteur_alcool, INPUT);
  
  //-----------------------------------------------------------------------------------------SPIFFS
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

  //------------------------------------------------------------------------------------------data wifi
  preferences.begin("data_wifi", false);
  ssid = preferences.getString("ssid", "default");
  password = preferences.getString("password", "default");
  readBouteilleFlash();


  //------------------------------------------------------------------------------------------WIFI
  WiFi.begin(ssid.c_str(), password.c_str());
  Serial.print("Tentative de connexion...");
  int comteur = 0;
  int errreurConnection = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(100);
    comteur++;
    if (comteur == 30)                          // donne une erreur de connection et sort de la boucle 
    {
      Serial.println("Erreur de connexion");
      errreurConnection = 1;
      break;
    }

  }

  if (errreurConnection == 0)       // si la connection a été établie
  {
    Serial.println("\n");
    Serial.println("Connexion etablie!");
    Serial.print("Adresse IP: ");
    Serial.println(WiFi.localIP());
  }
  else if (errreurConnection == 1)  // si la connection n'a pas été établie
  {
    if(WiFi.SSID() == "")          // si le ssid est vide
    {
      WiFi.mode(WIFI_AP);           // met le mode en point d'accès
      WiFi.softAP(soft_ap_ssid,soft_ap_password);    // ssid , mot de passe
      Serial.println("Mode point d'acces");
      Serial.print("Adresse IP: ");
      AP_IP = WiFi.softAPIP().toString();
      Serial.println(WiFi.softAPIP());
      ServeurAP_ON = 1;
    }

  }




  //-------SERVER PAGE ACCUEILLE
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {

    if (ServeurAP_ON == 1)
    {
      request -> send(SPIFFS, "/AP.html", "text/html");
    }
    else
    {
      request->send(SPIFFS, "/index.html", "text/html");
    }
    // request->send(SPIFFS, "/index.html", "text/html");
  });
  //-------SERVER PAGE 2
  server.on("/page2", HTTP_GET, [](AsyncWebServerRequest *request) {
    trouverDrinksPossibles();
    File html = SPIFFS.open("/index1.html");
    String htmlPage2 = html.readString();
    html.close();
    
    listeBreuvageHTML = "";
    for (int i = 0; i < sizeof(tableauDrinkPossible) / sizeof(tableauDrinkPossible[0]); i++)
    {
      listeBreuvageHTML += "<option value=\"" + String(i) + "\">" + tableauDrinkPossible[i] + "</option>";
    }

    Serial.print(listeBreuvageHTML);

    
    htmlPage2.replace("<select id=\"drinkPossibles\" style=\"width:80%; height: 50px; border-color: orange;\"></select>", "<select id=\"drinkPossibles\" style=\"width:80%; height: 50px; border-color: orange;\">" + listeBreuvageHTML + "</select>");
    Serial.print(htmlPage2);
    request->send(200, "text/html", htmlPage2);
  });

  //-------FICHIER CSS
  server.on("/w3.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/w3.css", "text/css");
  });

  //-------FICHIER JS
  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/script.js", "text/javascript");
  });
 
  //-------FICHIER JQUERY
  server.on("/jquery-3.6.0.min.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/jquery-3.6.0.min.js", "text/javascript");
  });

  //-------RECUILLE LES BOUTEILLES ENTREES DANS LA MACHINE 
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
    enregistrement_bouteille =1;
    // trouverDrinksPossibles();       // une fois que le bouton APPLIQUER est appuyer, affiche la liste des drink possible d'etre fait  
    request->send(204);
  });

  // FAIRE LE DRINK CHOISI PAR L'UTILISATEUR 
  server.on("/faireDrink", HTTP_POST, [](AsyncWebServerRequest *request) {     // recuille les boissons que nous avons mis dans chacunes des pompes 
    if(request->hasParam("drink_voulue", true))
    {
      drink_voulue = request->getParam("drink_voulue", true)->value();
    }
    Serial.println(drink_voulue);
    ingredientsDuDrink(drink_voulue) ;
    serving_On = 1;           //flag pour le mélange 
    //melange();
    request->send(204);
  });


  // enregistre le ssid et le mot de passe du wifi
  server.on("/AP_enregister", HTTP_POST, [](AsyncWebServerRequest *request) {     
    if(request->hasParam("ssid", true))
    {
      ssid = request->getParam("ssid", true)->value();
      Serial.println("ssid avant enregistrement: " + ssid);
      preferences.putString("ssid", ssid);
    }
    if(request->hasParam("password", true))
    {
      password = request->getParam("password", true)->value();
      Serial.println("password avant enregistrement: " + password);
      preferences.putString("password", password);
    }
    String s = preferences.getString("ssid", "default");
    String p = preferences.getString("password", "default");
    Serial.print("ssid: ");
    Serial.println(s);
    Serial.print("password: ");
    Serial.println(p);

    delay(500);
    ESP.restart();

    request->send(204);
  });


  // enregistre les bouteilles precedentes 
  server.on("/afficheBouteille_1_Precedentes", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    String b1 = BouteilleNo1;
    request->send(200, "text/plain", b1);
  });

  server.on("/afficheBouteille_2_Precedentes", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    String b2 = BouteilleNo2;
    request->send(200, "text/plain", b2);
  }); 

  server.on("/afficheBouteille_3_Precedentes", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    String b3 = BouteilleNo3;
    request->send(200, "text/plain", b3);
  });

  server.on("/afficheBouteille_4_Precedentes", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    String b4 = BouteilleNo4;
    request->send(200, "text/plain", b4);
  });

  server.on("/afficheBouteille_5_Precedentes", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    String b5 = BouteilleNo5;
    request->send(200, "text/plain", b5);
  });

  server.on("/afficheBouteille_6_Precedentes", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    String b6 = BouteilleNo6;
    request->send(200, "text/plain", b6);
  });

  server.on("/afficheBouteille_7_Precedentes", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    String b7 = BouteilleNo7;
    request->send(200, "text/plain", b7);
  });

  server.on("/afficheBouteille_8_Precedentes", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    String b8 = BouteilleNo8;
    request->send(200, "text/plain", b8);
  });

  server.on("/afficheBouteille_9_Precedentes", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    String b9 = BouteilleNo9;
    request->send(200, "text/plain", b9);
  });

  server.on("/afficheBouteille_10_Precedentes", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    String b10 = BouteilleNo10;
    request->send(200, "text/plain", b10);
  });


  server.begin();
  Serial.println("Serveur actif!");

  lcd.init();                      // initialize the lcd 
  lcd.backlight();

  scale.set_scale(calibration);        // valeur de calibration de la balance 
  scale.tare();                       // reset the scale to 0



}
//---------------------------------------------------------------------------


void loop()
{
  // regarde si le wifi est connecter, si non, il se reconnecte
  // if (WiFi.status() != WL_CONNECTED)
  // {
  //   digitalWrite(led_rouge,HIGH);
  //   lcd.clear();
  //   while (WiFi.status() != WL_CONNECTED)
  //   {
  //     WiFi.begin(ssid, password);
  //     lcd.setCursor(0,0);
  //     lcd.print("Connexion en cours");
  //     Serial.print("Tentative de reconnection...");
  //     delay(100);
  //   }
  //   lcd.clear();
  // } 

  if (ServeurAP_ON == 1 )
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(" CONNECTION ROUTEUR");
    lcd.setCursor(0,1);
    lcd.print("1. Wifi = DRINKMAKER");
    lcd.setCursor(0,2);
    lcd.print("2. IP ="+ AP_IP);
    Serial.print("IP");
    Serial.println(AP_IP);
    digitalWrite(led_rouge, HIGH);
    while(1){}

  }
  else
  {
    digitalWrite(led_rouge, LOW);

  if (scale.get_units() > 10)   // allume la led si aucun verre n'est présent 
  {
    digitalWrite(led_bleu, LOW);
  }
  else
  {
    digitalWrite(led_bleu, HIGH);
  }



  if (enregistrement_bouteille == 1)
  {
    enregistrement_bouteille = 0;
    saveBouteillesFlash();
    readBouteilleFlash();
  }
  //------
  //Serial.println(WiFi.RSSI());
  menuLCD();      // affichage sur l'écran LCD
  if (serving_On == 1)
  {
    melange();
  }
  }
  

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
  for(int i = 0; i < 50; i++)
  {
    tableauDrinkPossible[i] = " ";
  }

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
   // delay(10);

    JsonArray ingredients = drink["ingredients"];     // affiche le nom de l'ingédients 
    bool canMakeDrink = true;
    
    for (JsonObject ingredient : ingredients) {
      const char *ingredientName = ingredient["ingredient"];
      //delay(10);
      bool hasIngredient = false;
      for (int i = 0; i < ingredientsCount; i++) {            // passe en balayage un ingédient à la fois dans le breuvage X pour voir si nous l'avons ou non 
        if (ingredientsAvailable[i] == ingredientName) {
          hasIngredient = true;
         // delay(10);
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
   
   for(int i = 0 ; i < 10 ; i++)   // efface les ingédients du drink choisi précédement 
   {
      ingerdinetDrinkChoisi[i] = "";
   }

   int drink = drink_voulue.toInt();     // converti le string en int
   
   String breuvage = tableauDrinkPossible[drink];     // décommanter lors des vrai test ************************************************
  //String breuvage = "Planter's Punch";
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




//*********************************************************************************************************************
//          FONCTION POUR LE CAPTEUR D'ALCOOL
//*********************************************************************************************************************

float readAlcool(void)
{
  float sensorValue = analogRead(capteur_alcool);
  return sensorValue;
}


//*********************************************************************************************************************
//          FONCTION POUR LE MENU DE L'ÉCRAN LCD 
//*********************************************************************************************************************

void menuLCD (void)
{
  
  switch (etat_menu)
  {
    case 0:
      lcd.setCursor(2,0);
      lcd.print(WiFi.localIP());        // affiche l'adresse IP su serveur
      lcd.setCursor(0,3);
      lcd.print("PAGE SUIVANTE = MENU");
      if (digitalRead(btn_1) == 1 && digitalRead(btn_3) == 1)
      {
        calibrationLoadCell();
        scale.set_scale(calibration); // ajuste la calibration           
      }
      if (digitalRead(btn_2) == 1)
      {
        etat_menu = 1;
        delay(1000);
      }

      if (scale.get_units() > 10)
      {
        digitalWrite(led_bleu, LOW);
      }
      else
      {
        digitalWrite(led_bleu, HIGH);
      }

      break;
    case 1:


      lcd.clear();
      lcd.setCursor(2,0);
      lcd.print("POMPES MANUELLES");
      lcd.setCursor(0,2);
      lcd.print("1 2 3 4 5 6 7 8 9 10");
      while (digitalRead(btn_2) != 1)   // si  le btn_2 n'est pas appuyer 
      {

        if (scale.get_units() > 10)
        {
          digitalWrite(led_bleu, LOW);
        }
        else
        {
          digitalWrite(led_bleu, HIGH);
        }

        if (digitalRead(btn_3) == 1)
        {
          menu_pompe_manuelle += 1;
          delay(250);
        }

        Serial.println(menu_pompe_manuelle);

        switch (menu_pompe_manuelle)
        {
          case 2:           // test 1 cl quand on appuie sur le btn
            if(digitalRead(btn_1) == 1 && scale.get_units() > 10)
            {
              digitalWrite(solenoide_1,HIGH);
              delay(500);
              digitalWrite(pompe_1,HIGH);
              delay(3000);
              digitalWrite(solenoide_5,HIGH);
              digitalWrite(solenoide_1,LOW);
              delay(5000);
            }
            else
            {
              digitalWrite(pompe_1,LOW);
              digitalWrite(solenoide_1,LOW);
              digitalWrite(solenoide_5,LOW);
            }
            break;
          case 4:
            if(digitalRead(btn_1) == 1 && scale.get_units() > 10)
            {
              digitalWrite(solenoide_2,HIGH);
              delay(500);
              digitalWrite(pompe_1,HIGH);
              delay(3000);
              digitalWrite(solenoide_5,HIGH);
              digitalWrite(solenoide_2,LOW);
              delay(5000);
            }
            else
            {
              digitalWrite(pompe_1,LOW);
              digitalWrite(solenoide_2,LOW);
              digitalWrite(solenoide_5,LOW);
            }
            break;
          case 6:
            if(digitalRead(btn_1) == 1 && scale.get_units() > 10)
            {
              digitalWrite(solenoide_3,HIGH);
              delay(500);
              digitalWrite(pompe_1,HIGH);
              delay(3000);
              digitalWrite(solenoide_5,HIGH);
              digitalWrite(solenoide_3,LOW);
              delay(5000);
            }
            else
            {
              digitalWrite(pompe_1,LOW);
              digitalWrite(solenoide_3,LOW);
              digitalWrite(solenoide_5,LOW);
            }
            break;
          case 8:
            if(digitalRead(btn_1) == 1 && scale.get_units() > 10)
            {
              digitalWrite(solenoide_4,HIGH);
              delay(500);
              digitalWrite(pompe_1,HIGH);
              delay(3000);
              digitalWrite(solenoide_5,HIGH);
              digitalWrite(solenoide_4,LOW);
              delay(5000);
            }
            else
            {
              digitalWrite(pompe_1,LOW);
              digitalWrite(solenoide_4,LOW);
              digitalWrite(solenoide_5,LOW);
            }
            break;
          case 10:
            if(digitalRead(btn_1) == 1 && scale.get_units() > 10)
            {
              digitalWrite(solenoide_5,HIGH);
              delay(500);
              digitalWrite(pompe_1,HIGH);
            }
            else
            {
              digitalWrite(pompe_1,LOW);
              digitalWrite(solenoide_5,LOW);
              digitalWrite(solenoide_5,LOW);
            }
            break;
          case 12:
            if(digitalRead(btn_1) == 1 && scale.get_units() > 10)
            {
              digitalWrite(solenoide_6,HIGH);
              delay(500);
              digitalWrite(pompe_1,HIGH);
              delay(3000);
              digitalWrite(solenoide_5,HIGH);
              digitalWrite(solenoide_6,LOW);
              delay(5000);
            }
            else
            {
              digitalWrite(pompe_1,LOW);
              digitalWrite(solenoide_6,LOW);
              digitalWrite(solenoide_5,LOW);
            }
            break;
          case 14:
            if(digitalRead(btn_1) == 1 && scale.get_units() > 10)
            {
              digitalWrite(solenoide_7,HIGH);
              delay(500);
              digitalWrite(pompe_1,HIGH);
              delay(3000);
              digitalWrite(solenoide_5,HIGH);
              digitalWrite(solenoide_7,LOW);
              delay(5000);
            }
            else
            {
              digitalWrite(pompe_1,LOW);
              digitalWrite(solenoide_7,LOW);
              digitalWrite(solenoide_5,LOW);
            }
            break;
          case 16:
            if(digitalRead(btn_1) == 1 && scale.get_units() > 10)
            {
              digitalWrite(solenoide_8,HIGH);
              delay(500);
              digitalWrite(pompe_1,HIGH);
              delay(3000);
              digitalWrite(solenoide_5,HIGH);
              digitalWrite(solenoide_8,LOW);
              delay(5000);
            }
            else
            {
              digitalWrite(pompe_1,LOW);
              digitalWrite(solenoide_8,LOW);
              digitalWrite(solenoide_5,LOW);
            }
            break;
          case 18:
            if(digitalRead(btn_1) == 1 && scale.get_units() > 10)
            {
              digitalWrite(solenoide_9,HIGH);
              delay(500);
              digitalWrite(pompe_1,HIGH);
              delay(3000);
              digitalWrite(solenoide_5,HIGH);
              digitalWrite(solenoide_9,LOW);
              delay(5000);
            }
            else
            {
              digitalWrite(pompe_1,LOW);
              digitalWrite(solenoide_9,LOW);
              digitalWrite(solenoide_5,LOW);
            }
            break;
          case 20:
            if(digitalRead(btn_1) == 1 && scale.get_units() > 10)
            {
              digitalWrite(solenoide_10,HIGH);
              delay(500);
              digitalWrite(pompe_1,HIGH);
              delay(3000);
              digitalWrite(solenoide_5,HIGH);
              digitalWrite(solenoide_10,LOW);
              delay(5000);
            }
            else
            {
              digitalWrite(pompe_1,LOW);
              digitalWrite(solenoide_10,LOW);
              digitalWrite(solenoide_5,LOW);
            }
            break;
          case 1:
            lcd.setCursor(18,3);
            lcd.print(" ");
            lcd.setCursor(0,3);
            lcd.print("^");
            menu_pompe_manuelle += 1;
            break;
          case 3:
            lcd.setCursor(0,3);
            lcd.print(" ");
            lcd.setCursor(2,3);
            lcd.print("^");
            menu_pompe_manuelle += 1;
            break;
          case 5:
            lcd.setCursor(2,3);
            lcd.print(" ");
            lcd.setCursor(4,3);
            lcd.print("^");
            menu_pompe_manuelle += 1;
            break;
          case 7:
            lcd.setCursor(4,3);
            lcd.print(" ");
            lcd.setCursor(6,3);
            lcd.print("^");
            menu_pompe_manuelle += 1;
            break;
          case 9:
            lcd.setCursor(6,3);
            lcd.print(" ");
            lcd.setCursor(8,3);
            lcd.print("^");
            menu_pompe_manuelle += 1;
            break;
          case 11:
            lcd.setCursor(8,3);
            lcd.print(" ");
            lcd.setCursor(10,3);
            lcd.print("^");
            menu_pompe_manuelle += 1;
            break;
          case 13:
            lcd.setCursor(10,3);
            lcd.print(" ");
            lcd.setCursor(12,3);
            lcd.print("^");
            menu_pompe_manuelle += 1;
            break;
          case 15:
            lcd.setCursor(12,3);
            lcd.print(" ");
            lcd.setCursor(14,3);
            lcd.print("^");
            menu_pompe_manuelle += 1;
            break;
          case 17:
            lcd.setCursor(14,3);
            lcd.print(" ");
            lcd.setCursor(16,3);
            lcd.print("^");
            menu_pompe_manuelle += 1;
            break;
          case 19:
            lcd.setCursor(16,3);
            lcd.print(" ");
            lcd.setCursor(18,3);
            lcd.print("^");
            menu_pompe_manuelle += 1;
            break;
          default:
            menu_pompe_manuelle = 1;
            break;
        }
      }
      etat_menu = 2;
      lcd.clear();
      delay(1000);
      break;
    case 2:
      lcd.setCursor(4,0);
      lcd.print("TESTER ALCOOL");
      lcd.setCursor(0,2);
      lcd.print("COMMNECER = SELECT");
      lcd.setCursor(0,3);
      lcd.print("SORTIR = MENU");
      if (digitalRead(btn_3) == 1)
      {
        lcd.clear();
        int i = 5;
        while(i != 0)
        {
          
          lcd.setCursor(0,0);
          lcd.print("SOUFFLER 5 SECONDES");
          lcd.setCursor(9,1);
          lcd.print(i);
          i--;
          delay(1000);
        }
        lcd.clear();
        lcd.setCursor(2,0);
        if (readAlcool() < 3000)
        {
          lcd.print("PAS ALCOOLISE");
        }
        else if (readAlcool()>300 && readAlcool() < 4500)
        {
          lcd.print("SOUS LE 0.08 BAC");
        }
        else if (readAlcool() > 4500)
        {
          lcd.print("DEPASSE 0.08 BAC");
        }
        delay(5000);
        lcd.clear();
        etat_menu = 3;
      }
      else if (digitalRead(btn_2) == 1)
      {
        etat_menu = 3;
        lcd.clear();
        delay(1000);
      }
      break;
    // case 3:
    //   trouverCentilitre();
    //   etat_menu = 0;
    //   break;
    default:
      etat_menu = 0; 
      break;
  }
}



  //*************************************************
  // calibration de la load celle si l'utilisatieur appuie 
  // sur le bnt 1 et le btn 2 en même temps 
  //*************************************************
 void calibrationLoadCell(void)
 {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("CALIBRATION");
    lcd.setCursor(0,1);
    lcd.print("Vider plateau");
    lcd.setCursor(0,4);
    lcd.print("Btn3 = continuer");
    delay (2000);

    while(digitalRead(btn_3) == 0) {}  //attendre que le bouton 3 soit appuyé
    lcd.clear();
    scale.tare(10); //reset la valeur de la balance
    lcd.setCursor(0,0);
    lcd.print("Mettre poid calib ");
    lcd.setCursor(0,1);
    lcd.print("(397 g)");
    lcd.setCursor(0,4);
    lcd.print("Btn3 = continuer");

    while(digitalRead(btn_3) == 0){}  //attendre que le bouton 3 soit appuyé

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Calibration en cours");
    calibration = scale.get_value(20) / 397; //calculer la valeur de calibration  (397 est le poid du verre que je prend comme poids de référence) 

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Calibration termine");
    lcd.setCursor(0,1);
    lcd.print("Calibration = ");
    lcd.setCursor(0,2);
    lcd.print(calibration);


    delay(5000);
    lcd.clear();
 }



 void trouverCentilitre(void)
 {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("test");
    while (digitalRead(btn_2) == 0)
    {
      if (digitalRead(btn_3) == 1)
      {
        digitalWrite(solenoide_2,HIGH);
        delay(500);
        digitalWrite(pompe_1,HIGH);
        delay(500);
        digitalWrite(solenoide_2,LOW);
        digitalWrite(pompe_1,LOW);

        delay(500);
        digitalWrite(solenoide_1,HIGH);
        delay(500);
        digitalWrite(pompe_1,HIGH);
        delay(2000);
        digitalWrite(solenoide_1,LOW);
        digitalWrite(pompe_1,LOW);
      }
      lcd.setCursor(0,1);
      lcd.print(scale.get_units());
      delay(500);
    }
    
 }


void melange (void)
 {

    int unCentilitre = 3000;
    int verrePresent = 0;

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("MELANGE EN COURS");  
    
    for(int i=0 ; i<10 ; i++)
    {

      if (scale.get_units() > 10)
      {
        digitalWrite(led_bleu, LOW);
      }
      else
      {
        digitalWrite(led_bleu, HIGH);
      }

      if(ingerdinetDrinkChoisi[i] == "")
      {
        break;
      }
      else
      {
        if (scale.get_units() < 10)
        {
          verrePresent = 0;
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("PLATEAU VIDE");
          lcd.setCursor(0,1);
          lcd.print("METTRE VERRE");
          delay(1000);
          lcd.clear();
        }
        else 
        {
          verrePresent = 1;
        }
        if (ingerdinetDrinkChoisi[i] == BouteilleNo1 && verrePresent == 1)
        {
          Serial.print("Bouteille 1 ON");
          digitalWrite (solenoide_1,HIGH);
          delay(500);
          digitalWrite (pompe_1,HIGH);
          delay(unCentilitre * quantite[i].toInt());   
          digitalWrite (solenoide_1,LOW);
          digitalWrite (pompe_1,LOW);

          digitalWrite(solenoide_5,HIGH);
          delay(500);
          digitalWrite(pompe_1,HIGH); 
          delay(4000);
          digitalWrite(pompe_1,LOW);
          digitalWrite(solenoide_5,LOW);
        }
        else if (ingerdinetDrinkChoisi[i] == BouteilleNo2 && verrePresent == 1)
        {
          Serial.print("Bouteille 2 ON");
          digitalWrite (solenoide_2,HIGH);
          delay(500);
          digitalWrite (pompe_1,HIGH);
          delay(unCentilitre * quantite[i].toInt());   
          digitalWrite (solenoide_2,LOW);
          digitalWrite (pompe_1,LOW); 

          digitalWrite(solenoide_5,HIGH);
          delay(500);
          digitalWrite(pompe_1,HIGH); 
          delay(4000);
          digitalWrite(pompe_1,LOW);
          digitalWrite(solenoide_5,LOW);
        }
        else if (ingerdinetDrinkChoisi[i] == BouteilleNo3 && verrePresent == 1)
        {
          Serial.print("Bouteille 3 ON");
          digitalWrite (solenoide_3,HIGH);
          delay(500);
          digitalWrite (pompe_1,HIGH);
          delay(unCentilitre * quantite[i].toInt());   
          digitalWrite (solenoide_3,LOW);
          digitalWrite (pompe_1,LOW); 

          digitalWrite(solenoide_5,HIGH);
          delay(500);
          digitalWrite(pompe_1,HIGH); 
          delay(4000);
          digitalWrite(pompe_1,LOW);
          digitalWrite(solenoide_5,LOW);
        }
        else if (ingerdinetDrinkChoisi[i] == BouteilleNo4 && verrePresent == 1)
        {
          digitalWrite (solenoide_4,HIGH);
          delay(500);
          digitalWrite (pompe_1,HIGH);
          delay(unCentilitre * quantite[i].toInt());   
          digitalWrite (solenoide_4,LOW);
          digitalWrite (pompe_1,LOW); 

          digitalWrite(solenoide_5,HIGH);
          delay(500);
          digitalWrite(pompe_1,HIGH); 
          delay(4000);
          digitalWrite(pompe_1,LOW);
          digitalWrite(solenoide_5,LOW);
        }
        else if (ingerdinetDrinkChoisi[i] == BouteilleNo5 && verrePresent == 1)
        {
          digitalWrite (solenoide_5,HIGH);
          delay(500);
          digitalWrite (pompe_1,HIGH);
          delay(unCentilitre * quantite[i].toInt());   
          digitalWrite (solenoide_5,LOW);
          digitalWrite (pompe_1,LOW); 

          digitalWrite(solenoide_5,HIGH);
          delay(500);
          digitalWrite(pompe_1,HIGH); 
          delay(4000);
          digitalWrite(pompe_1,LOW);
          digitalWrite(solenoide_5,LOW);
        }
        else if (ingerdinetDrinkChoisi[i] == BouteilleNo6 && verrePresent == 1)
        {
          digitalWrite (solenoide_6,HIGH);
          delay(500);
          digitalWrite (pompe_1,HIGH);
          delay(unCentilitre * quantite[i].toInt());   
          digitalWrite (solenoide_6,LOW);
          digitalWrite (pompe_1,LOW); 

          digitalWrite(solenoide_5,HIGH);
          delay(500);
          digitalWrite(pompe_1,HIGH); 
          delay(4000);
          digitalWrite(pompe_1,LOW);
          digitalWrite(solenoide_5,LOW);
        }
        else if (ingerdinetDrinkChoisi[i] == BouteilleNo7 && verrePresent == 1)
        {
          digitalWrite (solenoide_7,HIGH);
          delay(500);
          digitalWrite (pompe_1,HIGH);
          delay(unCentilitre * quantite[i].toInt());   
          digitalWrite (solenoide_7,LOW);
          digitalWrite (pompe_1,LOW); 

          digitalWrite(solenoide_5,HIGH);
          delay(500);
          digitalWrite(pompe_1,HIGH); 
          delay(4000);
          digitalWrite(pompe_1,LOW);
          digitalWrite(solenoide_5,LOW);
        }
        else if (ingerdinetDrinkChoisi[i] == BouteilleNo8 && verrePresent == 1)
        {
          digitalWrite (solenoide_8,HIGH);
          delay(500);
          digitalWrite (pompe_1,HIGH);
          delay(unCentilitre * quantite[i].toInt());   
          digitalWrite (solenoide_8,LOW);
          digitalWrite (pompe_1,LOW); 

          digitalWrite(solenoide_5,HIGH);
          delay(500);
          digitalWrite(pompe_1,HIGH); 
          delay(4000);
          digitalWrite(pompe_1,LOW);
          digitalWrite(solenoide_5,LOW);
        }
        else if (ingerdinetDrinkChoisi[i] == BouteilleNo9 && verrePresent == 1)
        {
          digitalWrite (solenoide_9,HIGH);
          delay(500);
          digitalWrite (pompe_1,HIGH);
          delay(unCentilitre * quantite[i].toInt());   
          digitalWrite (solenoide_9,LOW);
          digitalWrite (pompe_1,LOW); 

          digitalWrite(solenoide_5,HIGH);
          delay(500);
          digitalWrite(pompe_1,HIGH); 
          delay(4000);
          digitalWrite(pompe_1,LOW);
          digitalWrite(solenoide_5,LOW);
        }
        else if (ingerdinetDrinkChoisi[i] == BouteilleNo10 && verrePresent == 1)
        {
          digitalWrite (solenoide_10,HIGH);
          delay(500);
          digitalWrite (pompe_1,HIGH);
          delay(unCentilitre * quantite[i].toInt());   
          digitalWrite (solenoide_10,LOW);
          digitalWrite (pompe_1,LOW); 

          digitalWrite(solenoide_5,HIGH);
          delay(500);
          digitalWrite(pompe_1,HIGH); 
          delay(4000);
          digitalWrite(pompe_1,LOW);
          digitalWrite(solenoide_5,LOW);
        }
        
      }
      
    }
    lcd.clear();
    lcd.setCursor(2,0);
    lcd.print("MELANGE TERMINE");
    Serial.print("MELANGE TERMINE");

    digitalWrite(buzzer,HIGH);    // active le buzzer quand le drink est fini 
    delay(50);
    digitalWrite(buzzer,LOW);
    delay(50);
    digitalWrite(buzzer,HIGH);
    delay(50);
    digitalWrite(buzzer,LOW);
    delay(50);
    digitalWrite(buzzer,HIGH);
    delay(50);
    digitalWrite(buzzer,LOW);
   
    delay(1000);
    lcd.clear();
    serving_On = 0;

 }


// save les bouteilles enregistrées dans la mémoire flash
void saveBouteillesFlash(void)
{
  preferences.putString("BouteilleNo1", "");
  preferences.putString("BouteilleNo2", "");
  preferences.putString("BouteilleNo3", "");
  preferences.putString("BouteilleNo4", "");
  preferences.putString("BouteilleNo5", "");
  preferences.putString("BouteilleNo6", "");
  preferences.putString("BouteilleNo7", "");
  preferences.putString("BouteilleNo8", "");
  preferences.putString("BouteilleNo9", "");
  preferences.putString("BouteilleNo10", "");


  preferences.putString("BouteilleNo1", BouteilleNo1);
  preferences.putString("BouteilleNo2", BouteilleNo2);
  preferences.putString("BouteilleNo3", BouteilleNo3);
  preferences.putString("BouteilleNo4", BouteilleNo4);
  preferences.putString("BouteilleNo5", BouteilleNo5);
  preferences.putString("BouteilleNo6", BouteilleNo6);
  preferences.putString("BouteilleNo7", BouteilleNo7);
  preferences.putString("BouteilleNo8", BouteilleNo8);
  preferences.putString("BouteilleNo9", BouteilleNo9);
  preferences.putString("BouteilleNo10", BouteilleNo10);

}


// read les bouteilles enregistrées dans la mémoire flash
void readBouteilleFlash(void)
{
  BouteilleNo1 = preferences.getString("BouteilleNo1", "default");
  BouteilleNo2 = preferences.getString("BouteilleNo2", "default");
  BouteilleNo3 = preferences.getString("BouteilleNo3", "default");
  BouteilleNo4 = preferences.getString("BouteilleNo4", "default");
  BouteilleNo5 = preferences.getString("BouteilleNo5", "default");
  BouteilleNo6 = preferences.getString("BouteilleNo6", "default");
  BouteilleNo7 = preferences.getString("BouteilleNo7", "default");
  BouteilleNo8 = preferences.getString("BouteilleNo8", "default");
  BouteilleNo9 = preferences.getString("BouteilleNo9", "default");
  BouteilleNo10 = preferences.getString("BouteilleNo10", "default");

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

}



/*
------------------choses à faire------------------------------------

- faire afficher les bouteilles prédente sur l'ecran lcd au lieu de sur le site web 
- mettre les bouteilles dans la flash et non le eeeprom

- enlever les qui se rentre automatiquement ou mettre un option vide dans le html

- https://tommydesrochers.com/la-meilleure-methode-pour-heberger-une-interface-web-sur-esp32-version-avancee-esp32-ep4/
- aller vois ce lien pour afficher les bouteilles sur le site web


- faire la partie affichage des drink possible sur le site web et leurs ingrédients 
- mettre une btn back sur le site  

*/