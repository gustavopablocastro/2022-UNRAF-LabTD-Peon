#ifndef _CONFIG_H
#define _CONFIG_H

//***  Seteo de pines

#define GSM_RX_PIN 16
#define GSM_TX_PIN 17
#define RELAY_PIN 32
#define WDT_PIN 5
#define LED1_PIN 2
#define BATERIA_PIN 34
#define PULSO_DIAC 4

//***  Seteo constantes generales

#define GSM_BAUDIOS 9600
#define TINY_GSM_MODEM_SIM800
#define TINY_GSM_USE_GPRS true
#define TINY_GSM_USE_WIFI false
#define TINY_GSM_RX_BUFFER 1024
#define BATERIA_MAX 12.4
#define BATERIA_NOR 11.1
#define BATERIA_MIN 9.8
#define BATERIA_K1 1214.0       // lectura a voltaje
#define BATERIA_K2 5.73786      // voltaje a voltaje final
#define KEEPALIVE_MQTT 60       // segs keepalive
#define WDT_TIMEOUT 30          // timeout para el watchdog interno
#define MQTT_SOCKET_TIMEOUT 30
#define BUFFER_ARCHIVO_CFG 128  // Buffer para actualización archivo config
#define FREC_PING 300000        // Frecuencia de envío de señal de vida para mantener la conexión MQTT activa (ms)
#define FREC_CTRL_AUTO 10000    // Frecuencia de control de reloj en modo AUTO
#define FREC_LECTURA_PULSOS 60000    // Frecuencia de lectura de pulsos/habilitacion de interrupción


//*** Parámetros WiFi

/*
WiFiClient clienteWiFi;
const char* ssid = "Red Wi-Fi HOME";
const char* password = "PauliyGus2020";
*/



//*** Parámetros GPRS y MQTT

const char* ARCHIVO_CONFIG = "/config.json";
const int MQTT_PUERTO PROGMEM = 1883;
const char* MQTT_BROKER PROGMEM = "broker.hivemq.com"; // pad19.com, o el broker MQTT que se decida utilizar
const char* MQTT_USU PROGMEM = ""; // 18473D5FF87B*, usuario MQTT
const char* MQTT_PWD PROGMEM = ""; // 18473D5FF87B*, clave MQTT
const char* MQTT_CLIENTE PROGMEM = "PEON/18473D5FF87B*"; // Id del cliente, utilizar la mac del dispositivo
const char* MQTT_TOPICO_PUB_TTL PROGMEM = "PEON/18473D5FF87B*/ttl";
const char* MQTT_TOPICO_PUB_ESTADO PROGMEM = "PEON/18473D5FF87B*/estado";
const char* MQTT_TOPICO_SUB_COMANDO PROGMEM = "PEON/18473D5FF87B*/comando";

//*** Datos del servidor GSM del prestador telefónico

const char* GSM_APN PROGMEM = "igprs.claro.com.ar";
const char* GSM_APN_USU PROGMEM = "clarogprs";
const char* GSM_APN_PWD PROGMEM = "clarogprs999";
// CLARO: igprs.claro.com.ar
// MOVISTAR: wap.gprs.unifon.com.ar usuario: wap  contraseña: wap
// PERSONAL: datos.personal.com  usuario: datos  contraseña: datos
// TUENTI: internet.movil usuario:internet contraseña: internet
const char* TAG = "BOYERO"; // Solo para logueo interno


//***  Variables globales

char pulsosBoyero[5]; // Placeholder para Periodo pulsos final boyero formateado MQTT
char voltajeBoyero[5]; // Placeholder para el voltaje final boyero formateado MQTT
char voltajeBateria[5]; // Placeholder para el voltaje final batería formateado MQTT

int flag_pulso = 2;     //flag que controla el inicio y final de la medición de periodo del pulsos
unsigned long ts_pulso0 = 0, ts_pulso1 = 0; //timestamp de tiemps entre pulsos
unsigned long ts_mediciones = 0;            //tiempo entre mediciones, para abilitar interrupciones
double T_pulsos = 0;                        //periodo entre pulsos en milisegundos

//***  Librerías externas

#include <ArduinoJson.h>
#include <TinyGsmClient.h>
#include <Arduino_JSON.h>
#include <Wire.h>
#include <RTClib.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>




enum Modos { MANUAL, AUTOMATICO };
Modos modo = MANUAL;


//***  Objetos globales

hw_timer_t *timerWatchdog = NULL;
HardwareSerial SerialGSM(1);
TinyGsm modemGSM(SerialGSM);
TinyGsmClient cliente(modemGSM);
RTC_DS3231 rtc;
JSONVar config;

//***  Se genera un id de cliente MQTT al azar, para evitar coincidencias.
String idCliente = "boyero-" + String(random(0xffff), HEX);
Adafruit_MQTT_Client mqtt(&cliente, MQTT_BROKER, MQTT_PUERTO, idCliente.c_str(), MQTT_USU, MQTT_PWD);           //cliente GSM
//Adafruit_MQTT_Client mqtt(&clienteWiFi, MQTT_BROKER, MQTT_PUERTO, idCliente.c_str(), MQTT_USU, MQTT_PWD);     //cliente WIFI
Adafruit_MQTT_Publish publicacionTtl = Adafruit_MQTT_Publish(&mqtt, MQTT_TOPICO_PUB_TTL);
Adafruit_MQTT_Publish publicacionEstado = Adafruit_MQTT_Publish(&mqtt, MQTT_TOPICO_PUB_ESTADO);
Adafruit_MQTT_Subscribe suscripcionCmd = Adafruit_MQTT_Subscribe(&mqtt, MQTT_TOPICO_SUB_COMANDO, MQTT_QOS_1);


//***  Componentes internos

// #include <esp_log.h>
#include <esp_task_wdt.h>
#include "helpers.h"
#include "fsystem.h"
#include "rtc.h"
#include "gsm.h"
#include "mqtt.h"

#endif
