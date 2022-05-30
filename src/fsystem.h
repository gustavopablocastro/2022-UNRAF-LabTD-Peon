#ifndef _FSYSTEM_H
#define _FSYSTEM_H


#include <LITTLEFS.h>


/*
Abre el archivo de configuración en el sistema de archivos (LittleFS)
y devuelve su contenido parseado como objeto JSON
*/
JSONVar parsearConfig(fs::FS &fs, const char* ruta) {
	File archivo = fs.open(ruta, FILE_READ);
	if(!archivo || archivo.isDirectory()) { return false; }

	char contenidoJson[archivo.size()];
	archivo.read((uint8_t *)contenidoJson, sizeof(contenidoJson));
	JSONVar jsonParseado = JSON.parse(contenidoJson);
	
	archivo.close();

	return jsonParseado;
}

/*
Abre el archivo de configuración en el sistema de archivos (LittleFS)
y actualiza su contenido conforme el valor actual del objeto JSON general de config
*/
bool actualizarConfig(fs::FS &fs, const char* ruta) {
	File archivo = fs.open(ruta, FILE_WRITE);
	if(!archivo || archivo.isDirectory()){ return false; }

	bool resultado = false;
	char buffer[BUFFER_ARCHIVO_CFG] = "";
	String configSerializado = JSON.stringify(config);
	configSerializado.toCharArray(buffer, sizeof(buffer));
	
	if (archivo.print(buffer)) resultado = true;
	archivo.close();
	
	return resultado;
}

/*
Verifica simplemente que el sistema de archivos (LittleFS) se encuentre ok,
y de ser así, carga el objeto JSON general de config llamando a parsearConfig()
*/
bool inicializarFS() {
	if (!LITTLEFS.begin(true)) {
		return false;
	} else {
		config = parsearConfig(LITTLEFS, ARCHIVO_CONFIG);
		Serial.println("CONFIGURACION LITTLE_FS >> OK");
		return true;
	}
	return false;
}

#endif
