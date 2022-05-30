#ifndef _GSM_H
#define _GSM_H


/*
Reinicia el modem GSM, y verifica conexión a red general y a servicio GRPS
*/
void setearGSM() {
	SerialGSM.begin(GSM_BAUDIOS, SERIAL_8N1, GSM_RX_PIN, GSM_TX_PIN, false);
	modemGSM.setBaud(GSM_BAUDIOS);

	delay(6000);
	
	if (!modemGSM.restart()) {
		ESP.restart();
	}
	Serial.println("Modem reiniciado");
	
	if (!modemGSM.waitForNetwork()) {
		ESP.restart();
	}
	Serial.println("Red conectada");
		
	if(!modemGSM.gprsConnect(GSM_APN, GSM_APN_USU, GSM_APN_PWD)) {
		ESP.restart();
	}
	Serial.println("Gprs conectado");
}

#endif
