#ifndef deviceResource_h
#define deviceResource_h

#include <inttypes.h>
#include <EthernetClient.h>

void portsRequest(uint8_t method, char * path, EthernetClient * client) ;
void configRequest(uint8_t method, char * path, EthernetClient * client) ;
void configSet(uint8_t method, char * path, EthernetClient * client) ;
void setOutput(uint8_t method, char * path, EthernetClient * client) ;

#endif
