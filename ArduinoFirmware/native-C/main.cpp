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
#include <IPAddress.h>
#include <SD.h>
#include <HttpServer.h>

#include "FileResources.h"
//#include "DeviceResource.h"

void notFound(uint8_t method, char* path, EthernetClient *client) {
	client->println("HTTP/1.1 404 - File not Found\n");
	client->stop();
	return;
}

void setup() {
	// Start serial port
	Serial.begin(SERIAL_BAUDRATE);

	// Setup IP interfaces
	byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
	uint8_t ip[] = { 192, 168, 10, 2 };
	uint8_t dns[] = { 147, 83, 2, 3 };
	uint8_t gw[] = { 192, 168, 10, 1 };
	uint8_t subnet[] = { 255, 255, 255, 0 };
	uint16_t port = 80;

	// Push server entries
	pushEntry(GET, "/index.html", serveIndex);
	pushEntry(GET, "/bs.css", serveCss);
	pushEntry(GET, "/bs-res.css", serveCssRes);
	pushEntry(GET, "/jquery.js", serveJQuery);
	pushEntry(GET, "/control.js", serveControl);
	pushEntry(GET, "/bs.js", serveBs);
	pushEntry(GET, "/shield.jpg", serveImg);
	/*server.pushEntry(GET, F("/ports"), portsRequest);
	/*server.pushEntry(GET, F("/config"), configRequest);
	/*server.pushEntry(GET, F("/set"), configSet);
	server.pushEntry(GET, F("/out"), setOutput);
	server.pushEntry(GET, F("/favicon.ico"), notFound);*/
	pushEntry(GET, "/", serveIndex);

	// Start server
	start(mac, ip, dns, gw, subnet, port);

	// Start SD service
	digitalWrite(10, LOW);
	digitalWrite(10, HIGH);
	if (!SD.begin(4))
		Serial.println(F("SD Initialization failed!"));

	else {
		Serial.println(F("SD Initialization Ok!"));
	}

	// Load configuration
	//loadConfig();
}

int main(void) {
	char c;
	init();

#if defined(USBCON)
	USB.attach();
#endif

	setup();

	while (true) {

		if (Serial.available()) {
			c = Serial.read();
			Serial.print(c);
		}
		proccess();
		//updateValues();
		//delay(100);

		if (serialEventRun)
			serialEventRun();
	}

	return 0;
}
