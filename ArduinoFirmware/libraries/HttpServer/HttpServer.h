#ifndef httpserver_h
#define httpserver_h

#include <inttypes.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <EthernetServer.h>

#define MAX_PATH		24
#define MAX_PATH_ENTRY		12
#define MAX_ENTRIES		10
#define MAX_HEAD		1024
#define MAX_LINE_LEN		30

// Methods
#define NULL	0x0
#define GET	0x1
#define POST	0x2
#define PUT	0x3
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

typedef struct {
	uint8_t key;
	char name[8];
} MethodMapper;

const MethodMapper methodMap [] = {
			{GET,"GET"},
			{POST,"POST"},
			{PUT,"PUT"},
			{PATCH,"PATCH"},
			{DELETE,"DELETE"},
			{COPY,"COPY"},
			{HEAD,"HEAD"},
			{OPTIONS,"OPTIONS"},
			{LINK,"LINK"},
			{UNLINK,"UNLINK"},
			{PURGE,"PURGE"},
			{NULL,"NONE"}
		};


typedef int8_t (*callback_t) (uint8_t method, char* path, EthernetClient *client);

typedef struct {
	uint8_t method;
	char path [MAX_PATH_ENTRY]; // Path pattern
	callback_t callback;
} ServerEntry;

class HttpServer {
	public:
		HttpServer():server(EthernetServer(80)){};
		void start(byte * mac, IPAddress ip, IPAddress dns, IPAddress gwAddr, IPAddress subnet, uint16_t port);
		int8_t proccess(void);
		int8_t pushEntry(uint8_t m, char *p, callback_t callback);

	private:
		char path[MAX_PATH];
		EthernetServer server;
		EthernetClient client;
		uint8_t numEntries;
		ServerEntry serverEntries[MAX_ENTRIES];
		virtual int8_t getMethod(void);
		virtual int8_t getPath();
		virtual uint8_t getHeaders();
};

#endif
