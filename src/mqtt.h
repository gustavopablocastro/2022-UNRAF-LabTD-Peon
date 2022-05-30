#ifndef _MQTT_H
#define _MQTT_H

/*
Forma y envía por MQTT un mensaje de estado,
indicando fecha y hora, estado, nivel de batería, pulsos y voltaje de salida
*/
void reportarEstado() {
	char ts[20];
	char modo[7];
	char estado[9];
	char contenido[256];
	
	verTimestampRTC().toCharArray(ts, 20);
	verificarVoltajesYPulsos();
	strcmp(config["modo"], "manual") == 0 ? strcpy(modo, "manual") : strcpy(modo, "auto");
	strcmp(config["estado"], "activo") == 0 ? strcpy(estado, "activo") : strcpy(estado, "inactivo");
	sprintf(contenido, " {\"time\": \"%s\", \"modo\": \"%s\", \"estado\": \"%s\", \"bateria\": \"%s\", \"pulsos\": \"%s\", \"voltaje\": \"%s\" }", ts, modo, estado, voltajeBateria, pulsosBoyero, voltajeBoyero);

	publicacionEstado.publish(contenido);
}

/*
Forma y envía por MQTT un mensaje de resultado,
por ejemplo ante la solicitud de actualización de fecha y hora del RTC
*/

void reportarResultado(char* msj) {
	char ts[20];
	char estado[9];
	char contenido[256];

	verTimestampRTC().toCharArray(ts, 20);
	strcmp(config["estado"], "activo") == 0 ? strcpy(estado, "activo") : strcpy(estado, "inactivo");
	sprintf(contenido, " {\"time\": \"%s\", \"estado\": \"%s\", \"resultado\": \"%s\" }", ts, estado, msj);
	
	publicacionEstado.publish(contenido);
}

/*
Callback general de la suscripción al tópico de comandos,
que parsea el payload recibido y ejecuta las acciones necesarias según el comando indicado
*/
void cbComandos(char *datos, uint16_t len) {

	DynamicJsonDocument mensajeParseado(256);
	DeserializationError error = deserializeJson(mensajeParseado, datos,len);	
  	if (error) {
    	Serial.print(F("deserializeJson() failed: "));
    	Serial.println(error.f_str());
    	return;
  	}
	const char* comando = mensajeParseado["comando"];
	Serial.println(comando);


	// Activa el boyero manualmente
	if (strcmp(mensajeParseado["comando"], "activar") == 0) {
		digitalWrite(RELAY_PIN, HIGH);
		
		config["modo"] = "manual";
		config["estado"] = "activo";
		actualizarConfig(LITTLEFS, ARCHIVO_CONFIG);
		
		reportarEstado();
	
	// Desactiva el boyero manualmente
	} else if (strcmp(mensajeParseado["comando"], "desactivar") == 0) {
		digitalWrite(RELAY_PIN, LOW);
		
		rtc.disableAlarm(1);
		rtc.clearAlarm(1);

		config["modo"] = "manual";
		config["estado"] = "inactivo";
		actualizarConfig(LITTLEFS, ARCHIVO_CONFIG);
		
		reportarEstado();
	
	// Ajusta el valor de referencia del RTC
	} else if (strcmp(mensajeParseado["comando"], "ajustarrtc") == 0) {
		if (mensajeParseado["timestamp"] != "") {
			char ts[20];
			char resultado[13] = "RTC ajustado";
			strncpy(ts, mensajeParseado["timestamp"], sizeof(ts));
			
			DateTime fechaEn = timestampEs2En(ts);
			rtc.adjust(fechaEn);
			
			reportarResultado(resultado);
		}
	// Cambia a modo AUTOMATICO, activando las alarmas para horario de encendido y apagado del boyero
	} else if (strcmp(mensajeParseado["comando"], "programar") == 0) {
		
		Serial.println("entro a programar");		
		if (mensajeParseado["timeAuto"][0] != "" && mensajeParseado["timeAuto"][1] != "") {
			
			char timeON[20];
			char timeOFF[20];
			strncpy(timeON, mensajeParseado["timeAuto"][0], sizeof(timeON));
			strncpy(timeOFF, mensajeParseado["timeAuto"][1], sizeof(timeOFF));

			Serial.println(timeON);
			Serial.println(timeOFF);

			bool set_Auto_activacion =    rtc.setAlarm1(timestampEs2En(timeON), DS3231_A1_Hour);
			bool set_Auto_desactivacion = rtc.setAlarm2(timestampEs2En(timeOFF), DS3231_A2_Hour);
		
			if (set_Auto_activacion && set_Auto_desactivacion) {
				modo = AUTOMATICO;
				config["modo"] = "auto";
				actualizarConfig(LITTLEFS, ARCHIVO_CONFIG);	
				reportarEstado();
			}	
		}
	// Solo envía el reporte de estado actual del boyero
	} else if (strcmp(mensajeParseado["comando"], "estado") == 0) {
		reportarEstado();
	}
	
}

/*
Verificador de la conexión MQTT
*/
void verificarMqtt() {
	int8_t ret;
	Serial.print("Connecting to MQTT... ");
	while ((ret = mqtt.connect()) != 0) {
		Serial.println(mqtt.connectErrorString(ret));
		Serial.println("Retrying MQTT connection in 5 seconds...");
		mqtt.disconnect();
		delay(5000);
	}

	Serial.println("MQTT conectado");
}

/*
Inicializador de la conexión MQTT, seteo de parámetros y suscripciones
*/
void inicializarMqtt() {
	suscripcionCmd.setCallback(cbComandos);
	mqtt.subscribe(&suscripcionCmd);
//	Serial.println("MQTT subscripción OK");
}

#endif
