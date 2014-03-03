#include <inttypes.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <EthernetServer.h>
#include "HttpServer.h"

int8_t HttpServer::getMethod() {

	uint8_t i;
	char method[8];
	char c;
	for (i = 0; i < 8 && client.connected() && client.available(); i++) {
		c = client.read();
		if (c == ' ') {
			break;
		} else {
			method[i] = c;
		}
	}

	// If client disconnected or error
	if (!(client.connected() || client.available() || i == 8))
		return -1;

	// Limit the method string
	method[i] = '\0';

	// Compare method
	for (i = 0; methodMap[i].key != 0; i++) {
		if (strcmp_P(method, (PGM_P) methodMap[i].name) == 0) {
			Serial.print(F("Method: "));
			Serial.println(method);
			return methodMap[i].key;
		}
	}
	return -2;
}

int8_t HttpServer::getPath() {
	uint8_t i;
	char c;
	for (i = 0; i < MAX_PATH && client.connected() && client.available(); i++) {
		c = client.read();
		if (c == ' ')
			break;
		else
			path[i] = c;
	}

	// If client disconnected or error reading
	if (!(client.connected() || client.available() || i == MAX_PATH))
		return -1;

	// Limit the method string
	path[i] = '\0';

	// Print Path
	Serial.print(F("Path: "));
	Serial.println(path);

	return i;
}

uint8_t HttpServer::getHeaders() {
	uint16_t counter = 0;
	uint8_t i = 0, headers = 0;
	char c, line[MAX_LINE_LEN];
	for (counter = 0;
			counter < MAX_HEAD && client.connected() && client.available();
			counter++) {
		c = client.read();
		if (c == '\n') {
			line[i] = '\0';
			if ((strncmp_P(line,
					(const prog_char*) F("If-None-Match: contentNotModified"),
					23) == 0
					|| strncmp_P(line,
							(const prog_char*) F("Cache-Control: max-age=0"),
							23) == 0) && ((headers & _NOT_MODIFIED) == 0)) {
				headers += _NOT_MODIFIED;
			} else if (i == 0) { // Blank line -> End of header
				headers += _REQ_OK;
				return headers;
			}
			i = 0;
		} else {
			line[i] = c;
			if (i < MAX_LINE_LEN)
				i++;
		}
	}

	return headers;
}

void HttpServer::start(byte * mac, IPAddress ip, IPAddress dns,
		IPAddress gwAddr, IPAddress subnet, uint16_t port) {
	uint8_t i;
	Ethernet.begin(mac, ip, dns, gwAddr, subnet);
	server.setPort(port);
	Serial.println(F("Starting Server!"));
	Serial.print(F("    IP: "));
	Serial.println(ip);
	Serial.print(F("  Port: "));
	Serial.println(port);
	Serial.print(F("    GW: "));
	Serial.println(gwAddr);
	Serial.print(F("SubNet: "));
	Serial.println(subnet);
}

void HttpServer::pushEntry(uint8_t m, const __FlashStringHelper* p, callback_t callback) {
	// Check space
	if (numEntries >= MAX_ENTRIES)
	return;

	// Push entry
	ServerEntry entry;
	entry.method = m;
	entry.path = p;
	entry.callback = callback;
	serverEntries[numEntries] = entry;
	numEntries++;
}

void HttpServer::proccess() {
	boolean hit = false;
	uint8_t method, pathLen, i, n, headers;

	// Get client, if not return
	client = server.available();
	if (!client)
		return;

	// Get method	
	method = getMethod();

	// load path and get pathlen
	pathLen = getPath();

	// Get headers
	headers = getHeaders();

	if ((headers & _NOT_MODIFIED) != 0) {
		client.println("HTTP/1.1 304 - Not Modified\n");
		delay(10);
		client.stop();
		return;
	}

	// Get backcall
	for (i = 0; i < numEntries; i++) {
		ServerEntry entry = serverEntries[i];
		n = strlen_P((const prog_char*) entry.path);

		if (n <= pathLen
				&& strncmp_P(path, (const prog_char*) entry.path, n) == 0
				&& method == entry.method) {
			(*(entry.callback))(method, path, &client);
			hit = true;
			break;
		}
	}
	if (!hit)
		client.println("HTTP/1.1 404 - File not Found\n");
	Serial.println(F("THNKX!"));
	client.stop();
}
