#include <inttypes.h>
#include <EthernetClient.h>

#include "DeviceResource.h"
#include "FileResources.h"
#include "SensorPlatform.h"

int8_t portsRequest(uint8_t method, char * path, EthernetClient * client) {
	fileRequest(F("/JSONHEAD.HDR"), client);
	sendPorts(client);
	client->stop();
	return 0;
}

int8_t configRequest(uint8_t method, char * path, EthernetClient * client) {
	fileRequest(F("/JSONHEAD.HDR"), client);
	sendConfig(client);
	Serial.println(F("PROBE!"));
	client->stop();
	return 0;
}
