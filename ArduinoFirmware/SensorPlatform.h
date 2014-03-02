#ifndef sensorplatform_h
#define sensorplatform_h

#include <EthernetClient.h>

// EEPROM BASE ADDR
#define _EEPROM_BASE   0x10

// Define input/output/devName/devAddr
#define _INPUT         'i'
#define _OUTPUT        'o'
#define _NAME          'n'
#define _ADDR          'a'

// Analog inputs
#define _EMPTY         'e'	// Empty/Void port
#define _RAW           'r'	// Raw Port
#define _TEMPERATURE   't'	// Temperature
#define _POTENCIOMETER 'p'	// Potentiometer
#define _LIGHT         'l'	// Light

// Outputs
#define _LOGICAL       'b'  // 'b' of binary
#define _PWM           'w'  // 'w' of width

// Configuration structure
#define DEV_NAME_MAX_LEN 16
struct Configuration {
	char inputs[6];
	char outputs[6];
	byte ip;
	char devName[DEV_NAME_MAX_LEN];
};

// Sensor values
struct SInputRawValues {
	unsigned int nextTimeStamp;
	unsigned short int inputs[6];
};

void loadConfig();
void sendPorts(EthernetClient * client);
void sendConfig(EthernetClient * client);

#endif
