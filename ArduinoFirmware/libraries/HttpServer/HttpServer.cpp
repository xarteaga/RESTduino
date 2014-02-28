
#include <inttypes.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <EthernetServer.h>
#include "HttpServer.h"

/*HttpServer::HttpServer() {
	Serial.println("Hola Mundo!");
}*/

int8_t HttpServer::getMethod(){
	uint8_t i;
	char method[8];
	char c;
	for (i = 0; i<8 && client.connected() && client.available(); i++){
		c = client.read();
		if (c==' '){
			break;
		}else{
			method[i] = c;
		}
	}

	// If client disconnected or error
	if (!(client.connected() || client.available() || i==8))
		return -1;

	// Limit the method string
	method[i] = '\0';

	// Compare method
	for (i=0; methodMap[i].key!=0; i++){
		if (strcmp(method, methodMap[i].name)==0)
			return methodMap[i].key;
	}
	return -2;
};

int8_t HttpServer::getPath(){
	uint8_t i;
	char c;
	for (i = 0; i < MAX_PATH && client.connected() && client.available(); i++){
		c = client.read();
		if (c==' ')
			break;
		else
			path[i] = c;
	}

	// If client disconnected or error reading
	if (!(client.connected() || client.available() || i==MAX_PATH))
		return -1;

	// Limit the method string
	path[i] = '\0';

	return i;
}

uint8_t HttpServer::getHeaders(){
	uint16_t counter = 0;
	uint8_t i = 0, headers = 0;
	char c, line[MAX_LINE_LEN];
	for (counter = 0; counter < MAX_HEAD && client.connected() && client.available(); counter++){
		c = client.read();
		if (c=='\n'||c=='\r'){
			line[i] = '\0';
			if (   (strncmp(line, "If-None-Match: contentNotModified", 23) == 0 ||
				strncmp(line, "Cache-Control: max-age=0",23) == 0) &&
			      ((headers & _NOT_MODIFIED) == 0)){
				headers += _NOT_MODIFIED;
			} else if (i == 0) { // Blank line -> End of header
				headers += _REQ_OK;
				return headers;
			}
			i = 0;
		} else {
			line [i] = c;
			if (i<MAX_LINE_LEN)
				i ++;
		}
	}

	return headers;
}

void HttpServer::start(byte * mac, IPAddress ip, IPAddress dns, IPAddress gwAddr, IPAddress subnet, uint16_t port) {
	Ethernet.begin(mac, ip, dns, gwAddr, subnet);
	server.setPort(port);
	Serial.println("Server started!");
}

int8_t HttpServer::pushEntry(uint8_t m, char *p, callback_t callback){
	// Check space
	if (numEntries >= MAX_ENTRIES)
		return -1;

	// Push entry
	ServerEntry entry;
	entry.method = m;
	strcpy(entry.path, p);
	entry.callback = callback;
	serverEntries[numEntries] = entry;
	
	// Return number of pushed entries
	return numEntries++;
}

int8_t HttpServer::proccess() {
	uint8_t method, pathLen, i, n, headers;

	// Get client, if not return
	client = server.available();
	if (!client)
		return 0;

	// Get method	
	method = getMethod();

	// load path and get pathlen
	pathLen = getPath();

	// Get headers
	headers = getHeaders();
	Serial.println(headers);

	// Get backcall
	for (i=0; i<numEntries; i++){
		ServerEntry entry = serverEntries[i];
		n = strlen(entry.path);
		Serial.print("Comparing: '");
		Serial.print(path);
		Serial.print("' with '");
		Serial.print(entry.path);
		Serial.println("'");

		if (n<=pathLen && strncmp(entry.path, path, n)==0){	
			Serial.println("HIT!");
			return (*(entry.callback))(method, path, &client);
		}
	}

	while(client.available()){
		char c = client.read();
	}
	client.stop();
	return 0;
}
