//CHINCHILLA V1.0 | Telegram @krujiente

//   C...C
//  ( . . )
//   \   /
//    `o` 

// Accesorio para el cargador Trydan de V2C | Grupo Telegram t.me/v2c_trydan
// Visualiza información relativa a diversas cargas y producción fotovoltaica
// No es necesario ningún sistema intermedio de terceros, como por ejemplo Home Assitant, app, etc..
// Toda la información se obtiene directamente via JSON del Trydan 
// El Trydan a su vez toma los valores del Shelly, pinzas V2C, inversor u otro medidor configurado en el Trydan. Si no están bien configurados los datos se mostrarán mal
// Basado en Arduino, placa esp32 Lilygo/TTGO T-display de 1,14 pulgadas y sus librerías necesarias
// Alimentación: 5V por USB | batería de 3,3V en el socket habilitado | 3,3V ó 5V a través de los pines de la placa
// Activada de serie la actualización de firmware vía OTA (Over The Air, WIFI) como alternativa a USB

// Por orden muestra:
// Producción fotovoltaica | No es necesaria una instalación FV
// Consumo de la casa
// Consumo del Trydan | blanco: coche sin manguera conectada; verde: manguera conectada; azul: suministrando al coche
// Balance en ese instante; rojo: importando de la red; verde: exportando excedentes
// Suma del consumo casa + Trydan
// Total de energía suministrada al coche en esa sesión de carga
// Duración de la sesión de carga | Falta por encontrar la lógica que emplea
// * La sesión se reinicia bien desconectando la manguera o comenzando a cargar después de una pausa

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

// CONFIGURACION DE LA CHINCHILLA
  // El Trydan y la chinchilla DEBEN estar en la misma subred, aunque estén en diferentes WIFIS debido a repetidores o PLCs.
  // Ejemplo BIEN: Trydan 192.168.1.40 y chinchilla 192.168.1.156 | Ejemplo MAL: Trydan 192.168.1.40 y chinchilla 192.168.0.156. Ojo con el 0 y el 1
  // SE RECOMIENDA ENCARECIDAMENTE conectar tanto el Trydan como la chinchilla a una red WIFI 2,4 Ghz.
  // NO SE RECOMIENDA redes mixtas 2,4 y 5 Ghz con el mismo nombre, puede ser el origen de posibles problemas de conexión y volvernos locos porque ni la chinchilla ni/o el Trydan conectan o pierden la señal. 
  // NO CONECTAR a redes de 5 Ghz, ni el Trydan ni la chinchilla.
  // ¿Aún así habrá gente que lo haga? o_O XD

  // Modificar los valores entre comillas para adecuarlos a nuestra WIFI/configuración:
  const char* ssid = "Kaitain"; // NOMBRE de la red WIFI, la misma en la que está el Trydan
  const char* password = "Tengounavacalechera69"; // CLAVE de la red WIFI
  const char* ip = "192.168.1.30"; // IP del Trydan. Se le debe haber asignado previamente una IP FIJA
  int refresco = 5; // Refresco de la información mostrada en pantalla. Por defecto 5 segundos. Cuidado con reducirla y agobiar al Trydan
// FIN de la configuración de la chinchilla




//***********************************************************
int tacumulado; // tiempo acumulado en la sesión de carga
int tenergia; // energia acumulada al coche en la sesión de carga

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
      //int tacumulado; // tiempo acumulado en la sesión de carga
      //float tenergia; // energia acumulada al coche en la sesión de carga
      
      //Empezamos a mostrar cositas
      tft.setTextSize(4);
      tft.fillRect(0, 0, 135, 240, TFT_BLACK);
      
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
          tenergia = 0;
          tacumulado = 0;
          }
      if (enchufado == 1) {
            // Si está enchufado pero no está cargando, establece el color del texto a verde
            tft.setTextColor(TFT_GREEN, TFT_BLACK);
            } 
      if (enchufado == 2) {
              // Si está enchufado y cargando, establece el color del texto a azul
              tft.setTextColor(TFT_BLUE, TFT_BLACK);
              //tenergia = tenergia + cargando;
              tacumulado = tacumulado + tiempo;
             }
        tft.setTextDatum(TR_DATUM);
        tft.drawString(String(int(cargando)), 135, 67, 1);

      // Mostramos si estamos importando o tenemos exdecentes y cuánto
      tft.pushImage(0, 99, 30, 30, itorre);
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
      tft.drawString("T", 1,196, 1);
      tft.setTextSize(1);
      // Calculamos las horas, minutos y segundos
          int horas = tiempo / 3600;
          int minutos = (tiempo % 3600) / 60;
          int segundos = tiempo % 60;
          // Mostramos en pantalla en formato HH:MM:SS
          tft.setTextDatum(TR_DATUM);
          String tiempoFormateado = String(horas) + ":" + String(minutos) + ":" + String(segundos);
          tft.drawString(tiempoFormateado, 135, 201, 4);
    } else {
      Serial.print("Error al deserializar JSON: ");
      Serial.println(error.c_str());
    }
  } else {
    Serial.printf("Error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
  delay(refresco);
}
