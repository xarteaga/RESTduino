#include <inttypes.h>
#include <EthernetClient.h>

#include "DeviceResource.h"
#include "FileResources.h"
#include "SensorPlatform.h"

void portsRequest (uint8_t method, char * path, EthernetClient * client) {
	fileRequest(F("/JSONHEAD.HDR"), client);
	sendPorts(client);
}

void configRequest (uint8_t method, char * path, EthernetClient * client) {
	fileRequest(F("/JSONHEAD.HDR"), client);
	sendConfig(client);
}

void configSet (uint8_t method, char * path, EthernetClient * client) {
	fileRequest(F("/JSONHEAD.HDR"), client);
	configure(path);
	sendConfig(client);
}

void setOutput (uint8_t method, char * path, EthernetClient * client) {
	fileRequest(F("/JSONHEAD.HDR"), client);
	setOutputValue(path);
	sendPorts(client);
}
