#ifndef _HELPERS_H
#define _HELPERS_H


/*
Completa un array de caracteres con una cadena al azar tomada del abcdario
*/
void generarCadenaRandom(char* str) {
	// memset(str, '\0', sizeof(str));
	char abcdario[37] = "abcdefghijklmnopqrstuvwxyz1234567890";
	for (uint8_t i = 0; i < 8; i++) { str[i] = abcdario[random(0, 36)]; }	
}

/*
Inicialización general de pines y breve notificación visual con led
*/
void inicializarPines() {
	pinMode(WDT_PIN, INPUT);
	
	pinMode(RELAY_PIN, OUTPUT);
	pinMode(LED1_PIN, OUTPUT);
	pinMode(BATERIA_PIN, INPUT);

	digitalWrite(RELAY_PIN, LOW);

	pinMode(PULSO_DIAC, INPUT_PULLDOWN);
	
	Serial.println("PINES INICIALIZADOS");
}

/*
Habilitación del watchdog interno
*/
void inicializarWatchdog() {
	esp_task_wdt_init(WDT_TIMEOUT, true); // Habilita el reinicio del ESP32 por pánico
	esp_task_wdt_add(NULL); // agrega el thread actual al control del WDT
}

/*
Ajusta físicamente el modo de trabajo (manual / auto) y estado del boyero (activo / inactivo)
*/
void ajustarBoyero() {
	// Ajuste de modo
	if (strcmp(config["modo"], "manual") == 0) {
		modo = MANUAL;
	} else if (strcmp(config["modo"], "auto") == 0) {
		modo = AUTOMATICO;
	}

	// Ajuste de estado
	if (strcmp(config["estado"], "activo") == 0 && modo == MANUAL) {
		digitalWrite(RELAY_PIN, HIGH);
	} else if (strcmp(config["estado"], "inactivo") == 0) {
		digitalWrite(RELAY_PIN, LOW);
	}
}

/*
Realiza las lecturas de voltaje de batería, voltaje de salida y pulsos de placa de control
*/
void verificarVoltajesYPulsos() {
	int lectura = analogRead(BATERIA_PIN);
	float voltajeMicro = lectura / BATERIA_K1;
	float voltajeFinal = voltajeMicro * BATERIA_K2;
	sprintf(voltajeBateria, "%.1f", voltajeFinal);
	sprintf(pulsosBoyero, "%.2f", T_pulsos/1000);
//inicializo las variables hasta las pueda medir con valor const
	// sprintf(pulsosBoyero, "1.234");
	sprintf(voltajeBoyero,"7.89");
}


#endif
