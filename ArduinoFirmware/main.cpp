// Definitions
#define SERIAL_BAUDRATE 115200

// Compilation options
//#define DEBUG_EEPROM
//#define DEBUG
#define PRINT_HEADERS
#define VERBOSE
#define ALLOW_CACHE

// Includes
#include <inttypes.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <EthernetServer.h>

#include "HttpServer.h"
#include "FileResources.h"
#include "DeviceResource.h"
#include "SensorPlatform.h"

HttpServer server;

void setup() {
	// Start serial port
	Serial.begin(SERIAL_BAUDRATE);

	// Setup IP interfaces
	byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
	IPAddress ip = IPAddress(192, 168, 10, (uint8_t) 2);
	IPAddress dns = IPAddress(147, 83, 2, 3);
	IPAddress gw = IPAddress(192, 168, 10, 1);
	IPAddress subnet = IPAddress(255, 255, 255, 0);
	uint16_t port = 80;

	// Push server entries
	server.pushEntry(GET, F("/index.html"), serveIndex);
	server.pushEntry(GET, F("/bs.css"), serveCss);
	server.pushEntry(GET, F("/bs-res.css"), serveCssRes);
	server.pushEntry(GET, F("/jquery.js"), serveJQuery);
	server.pushEntry(GET, F("/control.js"), serveControl);
	server.pushEntry(GET, F("/bs.js"), serveBs);
	server.pushEntry(GET, F("/shield.jpg"), serveBs);
	server.pushEntry(GET, F("/ports"), portsRequest);
	server.pushEntry(GET, F("/config"), configRequest);
	server.pushEntry(GET, F("/"), serveIndex);

	// Start server
	server.start(mac, ip, dns, gw, subnet, port);

	// Start SD service
	digitalWrite(10, LOW);
	digitalWrite(10, HIGH);
	if (!SD.begin(4)) {
		while (true) {
			Serial.println(F("SD Initialization failed!"));
			delay(1000);
		}
	}

	// Load configuration
	loadConfig();
}

void loop() {
	//updateValues();
	server.proccess();
}

