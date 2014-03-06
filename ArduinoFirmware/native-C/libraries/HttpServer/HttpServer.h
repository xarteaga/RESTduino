#ifndef httpserver_h
#define httpserver_h

#include <inttypes.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <EthernetServer.h>

#define MAX_PATH		32
#define MAX_PATH_ENTRY	12
#define MAX_ENTRIES		13
#define MAX_HEAD		1024
#define MAX_LINE_LEN	30

// Methods
#define NONE	0x0
#define GET		0x1
#define POST	0x2
#define PUT		0x3
#define PATCH	0x4
#define DELETE	0x5
#define COPY	0x6
#define HEAD	0x7
#define OPTIONS	0x8
#define LINK	0x9
#define UNLINK	0xA
#define PURGE	0xB

// Header masks
#define _REQ_OK        0x01
#define _NOT_MODIFIED  0x02

typedef void (*callback_t)(uint8_t method, char* path, EthernetClient *client);

typedef struct {
	uint8_t method;
	const char* path; // Path pattern
	callback_t callback;
}ServerEntry;

typedef struct {
	uint8_t key;
	const char* name;
}MethodMapper;


void start(uint8_t* mac, uint8_t* ip, uint8_t* dns, uint8_t* gwAddr,
		uint8_t* subnet, uint16_t port);
void proccess(void);
void pushEntry(uint8_t m, const char* p, callback_t callback);

#endif
