#ifndef deviceResource_h
#define deviceResource_h

#include <inttypes.h>
#include <EthernetClient.h>

int8_t portsRequest(uint8_t method, char * path, EthernetClient * client) ;
int8_t configRequest(uint8_t method, char * path, EthernetClient * client) ;

#endif
