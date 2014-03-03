#include <Arduino.h>
#include <EEPROM.h>
#include <HttpServer.h>

#include "SensorPlatform.h"

#define EEPROM_DEVNAME_PTR(conf) ((size_t)&conf.devName - (size_t)&conf)

Configuration conf;
SInputRawValues inputRawValues;

boolean isValidChar4Name(char c) {
	if (c > 0x2F && c < 0x3A)
		return true;
	else if (c > 0x40 && c < 0x5B)
		return true;
	else if (c > 0x60 && c < 0x7B)
		return true;
	else if (c == '_')
		return true;

	return false;
}

boolean checkDeviceName(char * devName) {
	byte i = 0;
	for (i = 0; i < DEV_NAME_MAX_LEN; i++) {
		if (!isValidChar4Name(devName[i])) {
			if (devName[i] == '\0' && i != 0)
				return true;
			// Invalid character
			Serial.println(devName);
			devName[0] = 'W';
			devName[1] = 'e';
			devName[2] = 'b';
			devName[3] = 'd';
			devName[4] = 'u';
			devName[5] = 'i';
			devName[6] = 'n';
			devName[7] = 'o';
			devName[8] = '\0';
			return false;
		}
	}
	devName[i] = 0; // Force End
	return false;
}

void loadConfig() {
	// Setup ports configuration
	uint8_t* confptr = (uint8_t*) &conf;
	for (uint8_t i = 0; i < sizeof(Configuration); i++) {
		confptr[i] = EEPROM.read(_EEPROM_BASE + i);
	}

	// Check the device name and configuration (first time name setting)
	char *devName = conf.devName;
	if (!checkDeviceName(devName)) {
		Serial.println(F("Warning: Bad status for device name."));
		for (byte i = 0; i < DEV_NAME_MAX_LEN; i++) {
			EEPROM.write(_EEPROM_BASE + EEPROM_DEVNAME_PTR(conf) + i,
					conf.devName[i]);
		}
	}
}

void sendPorts(EthernetClient * client) {
	char baseRaw[] = "_{\"val\":\"XXXX\"}_";
	char baseTemp[] = "_{\"val\":\"+XXX.X ºC\"}_"; // || {"val":"+068.0 ºC"},||
	char basePot[] = "_{\"val\":\"XXX.X %\"}_";
	char baseLogical1[] = "_{\"val\":\"True\"}_";
	char baseLogical0[] = "_{\"val\":\"False\"}_";
	char baseEmpty[] = "_{\"val\":\"Empty\"}_";
	char actBaseOn[] = "_{\"val\":\"On\"}_";
	char actBaseOff[] = "_{\"val\":\"Off\"}_";
	char actBaseEmpty[] = "_{\"val\":\"Empty\"}_";
	const uint8_t outTable[6] = { 11, 10, 9, 6, 5, 3 };
	short raw;
	char type;
	short val;
	//char * date = "Sun, 02 Mar 2014 18:26:34 GMT";

	client->pushTx("{\"deviceName\":\"");
	client->pushTx(conf.devName);
	/*client->pushTx("\,\"date\":\"");
	client->pushTx(date);*/
	client->pushTx("\",\"inputs\":");
	for (char i = 0; i < 6; i++) {
		raw = inputRawValues.inputs[i]; // Read analog port value
		type = conf.inputs[i];
		switch (type) {
		case _RAW:
			baseRaw[0] = (i == 0) ? '[' : ' ';
			baseRaw[12] = 0x30 + raw % 10;
			raw /= 10;
			baseRaw[11] = 0x30 + raw % 10;
			raw /= 10;
			baseRaw[10] = 0x30 + raw % 10;
			baseRaw[9] = 0x30 + raw / 10;
			baseRaw[15] = (i != 5) ? ',' : ']';
			client->pushTx(baseRaw);
			break;
		case _TEMPERATURE:
			val = (raw * 10) / 4 - 205;
			baseTemp[0] = (i == 0) ? '[' : ' ';
			baseTemp[9] = (val < 0) ? '-' : '+';
			baseTemp[14] = 0x30 + val % 10;
			val /= 10;
			baseTemp[12] = 0x30 + val % 10;
			val /= 10;
			baseTemp[11] = 0x30 + val % 10;
			baseTemp[10] = 0x30 + val / 10;
			baseTemp[21] = (i != 5) ? ',' : ']';
			client->pushTx(baseTemp);
			break;
		case _POTENCIOMETER:
			val = (((((raw * 10) >> 3) * 10) >> 3) * 10) >> 4;
			basePot[0] = (i == 0) ? '[' : ' ';
			basePot[13] = 0x30 + val % 10;
			val /= 10;
			basePot[11] = 0x30 + val % 10;
			val /= 10;
			basePot[10] = 0x30 + val % 10;
			basePot[9] = 0x30 + val / 10;
			basePot[18] = (i != 5) ? ',' : ']';
			client->pushTx(basePot);
			break;
		case _LIGHT:
			raw = raw >> 1;
			baseRaw[0] = (i == 0) ? '[' : ' ';
			baseRaw[12] = 0x30 + raw % 10;
			raw /= 10;
			baseRaw[11] = 0x30 + raw % 10;
			raw /= 10;
			baseRaw[10] = 0x30 + raw % 10;
			baseRaw[9] = 0x30 + raw / 10;
			baseRaw[15] = (i != 5) ? ',' : ']';
			client->pushTx(baseRaw);
			break;
		case _LOGICAL:
			if (raw > 512) {
				baseLogical1[0] = (i == 0) ? '[' : ' ';
				baseLogical1[15] = (i != 5) ? ',' : ']';
				client->pushTx(baseLogical1);
			} else {
				baseLogical0[0] = (i == 0) ? '[' : ' ';
				baseLogical0[16] = (i != 5) ? ',' : ']';
				client->pushTx(baseLogical0);
			}
			break;
		default:
			baseEmpty[0] = (i == 0) ? '[' : ' ';
			baseEmpty[16] = (i != 5) ? ',' : ']';
			client->pushTx(baseEmpty);
			break;
		}

	}
	client->pushTx(",\"outputs\":");
	for (char i = 0; i < 6; i++) {
		char type = (char)conf.outputs[i];
		switch (type) {
		case _LOGICAL:
			if (digitalRead(outTable[i]) == HIGH) {
				actBaseOn[0] = (i == 0) ? '[' : ' ';
				actBaseOn[13] = (i != 5) ? ',' : ']';
				client->pushTx(actBaseOn);
			} else {
				actBaseOff[0] = (i == 0) ? '[' : ' ';
				actBaseOff[14] = (i != 5) ? ',' : ']';
				client->pushTx(actBaseOff);
			}
			break;
		default:
			actBaseEmpty[0] = (i == 0) ? '[' : ' ';
			actBaseEmpty[16] = (i != 5) ? ',' : ']';
			client->pushTx(actBaseEmpty);
			break;
		}
	}
	client->pushTx("}\n");
	client->flushTx();
}

void configure(uint8_t method, char * path, EthernetClient * client) {
	byte nport, type;
	char inout;

	// Mode the pointer to the beginning of the configuration code
	path++;
	while (*path != '/')
		path++;
	path++;

	inout = path[0];       // Get the type of port: input, output or device name
	nport = path[1] - 0x30; // Get the port number (if any)
	type = path[2];         // Get port configuration

	switch (inout) {
	case _INPUT:
		if (nport > 5)
			return;  // If not valid value, return
		conf.inputs[nport] = type;
		EEPROM.write(_EEPROM_BASE + nport, type);
		break;
	case _OUTPUT:
		if (nport > 5)
			return;  // If not valid value, return
		conf.outputs[nport] = type;
		EEPROM.write(_EEPROM_BASE + nport + 6, type);
		break;
	case _NAME:
		uint8_t devNameLen; // Declare the device name length
		path++; // Adjust config code pointer
		/* Look for the end of the name, in case of being longer then DEV_NAME_MAX_LEN-1
		 * it will be cut to this number. */
		for (devNameLen = 0;
				devNameLen < DEV_NAME_MAX_LEN - 1 && path[devNameLen] != ' ';
				devNameLen++)
			;
// Copy the Device name in the config structure
		strncpy(conf.devName, path, devNameLen);
		conf.devName[devNameLen] = '\0';  // End wil the end string (null) char
		checkDeviceName(conf.devName);
// Copy the device name in the EEPROM
		Serial.println(conf.devName);

		for (uint8_t i = 0; i < devNameLen + 1; i++)
			EEPROM.write(
			_EEPROM_BASE + (size_t) & conf.devName - (size_t) & conf + i,
					conf.devName[i]);
		break;
	case _ADDR:
		path++;
		conf.ip = *path - 0x30;
		EEPROM.write(_EEPROM_BASE + 12, conf.ip);
		break;
	}
}

void setOutputValue(char * path) {
	const uint8_t outTable[6] = { 11, 10, 9, 6, 5, 3 };

	// Mode the pointer to the beginning of the configuration code
	path++;
	while (*path != '/')
		path++;
	path++;
	byte nport = *path - 0x30;
	if (nport > 5)
		return;
	path++;
	switch (*path) {
	case '0':
		digitalWrite(outTable[nport], LOW);
		break;
	case '1':
		digitalWrite(outTable[nport], HIGH);
		break;
	}
}

void updateValues() {
	uint8_t i;
	//File file;
	unsigned int timeStamp = millis();

	if (timeStamp < inputRawValues.nextTimeStamp)
		return;

	inputRawValues.nextTimeStamp = timeStamp + 1000;
	unsigned short int * rawValues = inputRawValues.inputs;
	for (i = 0; i < 6; i++) {
		rawValues[i] = analogRead(i);
	}
	return;
}

void sendConfig(EthernetClient * client) {
	char confBase[] = "_{\"type\":\"X\"}_}";
	client->pushTx("{\"deviceName\":\"");
	client->pushTx(conf.devName);
	client->pushTx("\",\"ip\":\"");
	client->pushTx(0x30 + conf.ip);
	client->pushTx("\",\"ports\":");
	for (char i = 0; i < 12; i++) {
		confBase[0] = (i == 0) ? '[' : ' ';
		confBase[13] = (i != 11) ? ',' : ']';
		confBase[14] = (i != 11) ? '\0' : '}';
		confBase[10] = conf.inputs[i];
		client->pushTx(confBase);
	}
	client->flushTx();
}

void configure(char* path) {
	byte nport, type;
	char inout;

	// Mode the pointer to the begining of the configuration code
	path++;
	while (*path != '/')
		path++;
	path++;

	inout = path[0];       // Get the type of port: input, output or device name
	nport = path[1] - 0x30; // Get the port number (if any)
	type = path[2];         // Get port configuration

	switch (inout) {
	case _INPUT:
		if (nport > 5)
			return;  // If not valid value, return
		conf.inputs[nport] = type;
		EEPROM.write(_EEPROM_BASE + nport, type);
		break;
	case _OUTPUT:
		if (nport > 5)
			return;  // If not valid value, return
		conf.outputs[nport] = type;
		EEPROM.write(_EEPROM_BASE + nport + 6, type);
		break;
	case _NAME:
		byte devNameLen; // Declare the device name length
		path++; // Adjust config code pointer
		/* Look for the end of the name, in case of being longer then DEV_NAME_MAX_LEN-1
		 * it will be cut to this number. */
		for (devNameLen = 0;
				devNameLen < DEV_NAME_MAX_LEN - 1 && path[devNameLen] != ' ';
				devNameLen++)
			;

		// Copy the Device name in the config structure
		strncpy(conf.devName, path, devNameLen);
		conf.devName[devNameLen] = '\0';  // End wil the end string (null) char

		checkDeviceName(conf.devName);
		Serial.println(F("Warning: Bad status for device name."));
		for (byte i = 0; i < DEV_NAME_MAX_LEN; i++) {
			EEPROM.write(_EEPROM_BASE + EEPROM_DEVNAME_PTR(conf) + i,
					conf.devName[i]);
		}
		break;
	case _ADDR:
		path++;
		conf.ip = *path - 0x30;
		EEPROM.write(_EEPROM_BASE + 12, conf.ip);
		break;
	}
}

void setOutput(char * path) {
	const byte outTable[6] = { 11, 10, 9, 6, 5, 3 };

	path++;
	while (*path != '/')
		path++;
	path++;
	path += 4;
	byte nport = *path - 0x30; // get number of port
	if (nport > 5)
		return;
	path++;
	switch (*path) {
	case '0':
		digitalWrite(outTable[nport], LOW);
		break;
	case '1':
		digitalWrite(outTable[nport], HIGH);
		break;
	}
}
