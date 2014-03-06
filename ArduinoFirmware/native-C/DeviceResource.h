#ifndef deviceResource_h
#define deviceResource_h

#include <inttypes.h>
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

void loadConfig();
void updateValues();

void portsRequest(uint8_t method, char * path, EthernetClient * client) ;
void configRequest(uint8_t method, char * path, EthernetClient * client) ;
void configSet(uint8_t method, char * path, EthernetClient * client) ;
void setOutput(uint8_t method, char * path, EthernetClient * client) ;

#endif
