#ifndef _RTC_H
#define _RTC_H


// Prototipo p/ evitar warnings al compilar
void reportarEstado();


/*
Inicializa el módulo reloj de tiempo real (RTC) para confirmar que puede usarse sin problemas
*/
bool inicializarRTC() {
	if (! rtc.begin()) return false;
	
	rtc.disableAlarm(1);
	rtc.disableAlarm(2);
	rtc.clearAlarm(1);
	rtc.clearAlarm(2);

	Serial.println("CONFIGURACION RTC >> OK");
	return true;
}

/*
Convierte un timestamp en formato de fecha y hora español, a inglés
*/
DateTime timestampEs2En(char* ts) {
	char tsConv[20];
	
	tsConv[0] = ts[6];
	tsConv[1] = ts[7];
	tsConv[2] = ts[8];
	tsConv[3] = ts[9];
	tsConv[4] = '/';
	tsConv[5] = ts[3];
	tsConv[6] = ts[4];
	tsConv[7] = '/';
	tsConv[8] = ts[0];
	tsConv[9] = ts[1];
	tsConv[10] = ts[10];
	tsConv[11] = ts[11];
	tsConv[12] = ts[12];
	tsConv[13] = ts[13];
	tsConv[14] = ts[14];
	tsConv[15] = ts[15];
	tsConv[16] = ts[16];
	tsConv[17] = ts[17];
	tsConv[18] = ts[18];
	
	return DateTime(tsConv);
}

/*
Devuelve el timestamp actual del módulo RTC
*/
String verTimestampRTC() {
	char timestamp[20];
	DateTime ahora = rtc.now();
	sprintf(timestamp, "%02u/%02u/%d %02u:%02u:%02u", ahora.day(), ahora.month(), ahora.year(), ahora.hour(), ahora.minute(), ahora.second());
	return timestamp;
}

void verificar_alarmas() {
	if (rtc.alarmFired(1) && config["estado"] != "activo") {
		rtc.disableAlarm(1);
		rtc.clearAlarm(1);

		digitalWrite(RELAY_PIN, HIGH);
			
		config["estado"] = "activo";
		actualizarConfig(LITTLEFS, ARCHIVO_CONFIG);
		reportarEstado();
	}

	if (rtc.alarmFired(2) && config["estado"] != "inactivo") {
		rtc.disableAlarm(2);
		rtc.clearAlarm(2);

		digitalWrite(RELAY_PIN, LOW);
		
		config["estado"] = "inactivo";
		actualizarConfig(LITTLEFS, ARCHIVO_CONFIG);
		reportarEstado();
	}
}

#endif
