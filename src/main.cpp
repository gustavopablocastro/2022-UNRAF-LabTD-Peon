/*
Proyecto boyero automatizado UNRAF PEON
Código placa de comunicación base ESP32, SIM800L y RTC DS3231
Archivo principal versión 2.2 13abr2022
*/

//#include <WiFi.h>
#include <Arduino.h>
#include "config.h"

//*** Declaro la función de interrupción x pulso boyero
void ISR();

/*
Inicialización en secuencia de los diferentes servicios en el setup,
si hay un problema con el sistema de archivos o el módulo RTC, se detiene la ejecución,
caso contrario se intenta inicializar el módulo GSM y la conexión MQTT, reiniciando
si es necesario hasta lograrlo.
*/
void setup() {
	Serial.begin(115200);
	Serial.println("SISTEMA INICIADO");
	
	inicializarPines();
	if (!inicializarFS()) { while(1); }
	if (!inicializarRTC()) { while(1); }
	ajustarBoyero();
	inicializarMqtt();


//---AGREGADO POR GC PARA TRABAJAR POR WIFI EN LUGAR DE GPRS
/*	WiFi.begin(ssid,password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.println("WiFi connected");
*/
delay(5000);
}

/*
El loop principal atiende esencialmente 3 ramas:
1)
2) Monitoreo y parseo del tráfico MQTT
3) Envío de actualización MQTT a intervalos regulares para evitar cortes
*/
void loop() {
	static unsigned long timerTtl = millis();
	static unsigned long timerAuto = millis();

	// 
	if ((ts_mediciones+FREC_LECTURA_PULSOS) < millis()){
  		attachInterrupt (PULSO_DIAC, ISR, RISING);
    	ts_mediciones = millis();
  	}

  	if (flag_pulso==0){
  	}else if (flag_pulso==1){
      	detachInterrupt(PULSO_DIAC);
      	Serial.printf(" Periodo entre pulsos: %.3f seg \n",T_pulsos/1000);
      	flag_pulso=2;
    }

	// Constantemente se monitorea el estado de la conexión MQTT
	// y se recibe cualquier payload que ingrese
	if (!mqtt.connected()) {
		digitalWrite(LED1_PIN,LOW);		//apago el ESP32 LED_BUIDIN si no hay conexión
		setearGSM();
		verificarMqtt();
	} else {
		digitalWrite(LED1_PIN,HIGH);	//prendo el ESP32 LED_BUIDIN si hay conexión
		mqtt.processPackets(10000);
	}

	// Se publica un mensaje ttl periódico para evitar desconexiones
	if (millis() - timerTtl >= FREC_PING) {
		// if(!mqtt.ping()) { mqtt.disconnect(); }
		if (mqtt.connected()) {
			char ts[20];
			verTimestampRTC().toCharArray(ts, 20);
			publicacionTtl.publish(ts);
		} 
		timerTtl = millis();
	}

	if (modo == AUTOMATICO) {
		if (millis() - timerAuto >= FREC_CTRL_AUTO) {
			verificar_alarmas();
			timerAuto = millis();
		}
	}
}

//*** Función interrupción desde pulso DIAC boyero

void ISR(){
  if(ts_pulso0!=millis() && (flag_pulso==2)){
    ts_pulso0 = millis();
    ts_pulso1 = ts_pulso0;
    flag_pulso=0;
  }else if (ts_pulso0!=millis() && ts_pulso0 == ts_pulso1){
    ts_pulso1 = millis();
    flag_pulso=1;
    T_pulsos = ts_pulso1 - ts_pulso0;
    }else{}
};