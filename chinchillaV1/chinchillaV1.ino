// Chinchilla V1 | Telegram @krujiente
#include <ArduinoOTA.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <TFT_eSPI.h>
#include "ifv.h"
#include "itorre.h"
#include "icasa.h"
#include "icoche.h"
#include "imargen.h"
#include "itotal.h"

TFT_eSPI tft = TFT_eSPI();

// CONFIGURACION
const char* ssid = "NombredetuWIFI"; // NOMBRE de la red WIFI, la misma subnet en la que está el Trydan
const char* password = "TUContraSEñadelWIFI12345"; // CLAVE de la red WIFI
const char* ip = "192.168.X.X"; // IP del Trydan. Se le debe haber asignado previamente una IP FIJA
int refresco = 5; // Refresco de la información mostrada en pantalla. Por defecto 5 segundos. Cuidado con reducirla y agobiar al Trydan
// FIN de la configuración de la chinchilla
const char* endpoint = "/RealTimeData";

// Inicializamos el WIFI y diversas cosas
void setup() {
  Serial.begin(115200);
  tft.init();
  refresco = refresco * 1000;
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(4);
  tft.setTextColor(TFT_WHITE);
  tft.setSwapBytes(true);
  WiFi.begin(ssid, password);
  
  Serial.print("Conectando a ");
  Serial.println(ssid);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(2000);
    Serial.print(".");
  }
  Serial.println("Conectado");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  delay(500);
  ArduinoOTA.begin();
}

void loop() {
  ArduinoOTA.handle();
  if (WiFi.status() != WL_CONNECTED) {
    tft.setTextSize(2);
    tft.setTextColor(TFT_YELLOW);
    tft.drawString("NO WIFI", 0,0, 2);
    delay(refresco);
    return; // Salir de loop si no hay conexión WiFi
  }
  
  HTTPClient http;
  String url = "http://" + String(ip) + String(endpoint);
  http.begin(url);
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    String payload = http.getString();
    DynamicJsonDocument doc(512);
    DeserializationError error = deserializeJson(doc, payload);

    // Vampirizamos el Trydan mediante JSON y empezamos a hacer cálculos
    if (!error) {
      int cargando = doc["ChargePower"];
      int enchufado = doc["ChargeState"];
      float casa = doc["HousePower"];
      casa = casa - cargando;
      int fv = doc["FVPower"];
      float energiacoche = doc["ChargeEnergy"];
      int tcarga = casa + cargando;
      int grid = tcarga - fv;
      int tiempo = doc["ChargeTime"];
      IPAddress ipl = WiFi.localIP();
      String firm = doc["FirmwareVersion"];
      //int tacumulado; // tiempo acumulado en la sesión de carga
      //float tenergia; // energia acumulada al coche en la sesión de carga
      
      //Empezamos a mostrar cositas
      tft.fillRect(0, 0, 135, 240, TFT_BLACK);
      tft.setTextSize(4);
    
      // Producción fotovoltaica
      tft.pushImage(0, 0, 30, 30, ifv);
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
      tft.setTextDatum(TR_DATUM);
      tft.drawString(String(fv), 135, 0, 1);
      
      // Consumo de la casa
      tft.pushImage(0, 34, 30, 30, icasa);
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
      tft.setTextDatum(TR_DATUM);
      tft.drawString(String(int(casa)), 135, 34, 1);
    
      // Potencia de carga suministrando al coche
      tft.pushImage(0, 68, 30, 30, icoche);
      if (enchufado == 0) {
        // Si no está enchufado, establece el color del texto a blanco
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        //tenergia = 0;
        //tacumulado = 0;
      }
      if (enchufado == 1) {
        // Si está enchufado pero no está cargando, establece el color del texto a verde
        tft.setTextColor(TFT_GREEN, TFT_BLACK);
      } 
      if (enchufado == 2) {
        // Si está enchufado y cargando, establece el color del texto a azul
        tft.setTextColor(TFT_BLUE, TFT_BLACK);
        //tenergia = tenergia + cargando;
        //tacumulado = tacumulado + tiempo;
      }
      tft.setTextDatum(TR_DATUM);
      tft.drawString(String(int(cargando)), 135, 67, 1);

      // Mostramos si estamos importando o tenemos exdecentes y cuánto
      tft.pushImage(0, 101, 30, 30, itorre);
      if (grid > 0) {  
        // Estamos importando de la red, rojo        
        tft.setTextColor(TFT_RED, TFT_BLACK);
      } else {
        // Tenemos excedentes, verde
        grid = grid * (-1);
        tft.setTextColor(TFT_GREEN, TFT_BLACK);
      }
      tft.setTextDatum(TR_DATUM);
      tft.drawString(String(int(grid)), 135, 99, 1);
      
      // Muestra la suma de las cargas de la casa y el Trydan
      tft.pushImage(0, 135, 30, 30, itotal);
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
      tft.setTextDatum(TR_DATUM);
      tft.drawString(String(int(tcarga)), 135, 135, 1);

      // Muestra la energía total en la sesión suministrada al coche
      tft.setTextSize(3);
      tft.pushImage(0, 167, 30, 30, imargen);
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
      tft.setTextDatum(TR_DATUM);
      tft.drawString(String(energiacoche), 135, 171, 1);
     
      // Muestra la duración de la sesión de carga
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
      tft.setTextDatum(TL_DATUM);
      tft.setTextSize(2);
      tft.drawString("T", 1,196, 2);
      tft.setTextSize(1);
      // Calculamos las horas, minutos y segundos
      int horas = tiempo / 3600;
      int minutos = (tiempo % 3600) / 60;
      int segundos = tiempo % 60;
      // Mostramos en pantalla en formato HH:MM:SS
      tft.setTextDatum(TR_DATUM);
      String tiempoFormateado = String(horas) + ":" + String(minutos) + ":" + String(segundos);
      tft.drawString(tiempoFormateado, 135, 201, 4);

      tft.setTextSize(1);
      tft.setTextColor(TFT_GREEN, TFT_BLACK);
      tft.setCursor(1,225);
//      tft.textFont(1);
      //tft.setTextDatum(TL_DATUM);
      tft.print("IP:");
      tft.println(ipl);
      //tft.setCursor(1,234);
      tft.print(" F:");
      tft.print(firm);
    } else {
      Serial.print("Error al deserializar JSON: ");
      Serial.println(error.c_str());
      tft.fillScreen(TFT_BLACK);
      tft.setTextSize(2);
      tft.setTextColor(TFT_MAGENTA);
      tft.drawString("NO JSON", 0,70, 2);
    
    }
  } else {
    Serial.printf("Error: %s\n", http.errorToString(httpCode).c_str());
    tft.setTextSize(2);
    tft.setTextColor(TFT_MAGENTA);
    tft.drawString("NO JSON", 0,70, 2);
    
      
  }

  http.end();
  delay(refresco);
}
