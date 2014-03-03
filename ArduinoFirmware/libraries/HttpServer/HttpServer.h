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

typedef void (*callback_t) (uint8_t method, char* path, EthernetClient *client);

typedef struct {
	uint8_t method;
	const __FlashStringHelper* path; // Path pattern
	callback_t callback;
} ServerEntry;

typedef struct {
	uint8_t key;
	const __FlashStringHelper* name;
} MethodMapper;

class HttpServer {
	public:
		HttpServer():server(EthernetServer(80)){
			numEntries = 0;

			const __FlashStringHelper* meth_get = F("GET");
			/*const __FlashStringHelper* meth_post = F("POST");
			const __FlashStringHelper* meth_put = F("PUT");
			const __FlashStringHelper* meth_patch = F("PATCH");
			const __FlashStringHelper* meth_delete = F("DELETE");
			const __FlashStringHelper* meth_copy = F("COPY");
			const __FlashStringHelper* meth_head = F("HEAD");
			const __FlashStringHelper* meth_options = F("OPTIONS");
			const __FlashStringHelper* meth_link = F("LINK");
			const __FlashStringHelper* meth_unlink = F("UNLINK");
			const __FlashStringHelper* meth_purge = F("PURGE");*/
			const __FlashStringHelper* meth_none = F("NONE");

			methodMap = {
								{GET, meth_get},
								/*{POST, meth_post},
								{PUT, meth_put},
								{PATCH, meth_patch},
								{DELETE, meth_delete},
								{COPY, meth_copy},
								{HEAD, meth_head},
								{OPTIONS, meth_options},
								{LINK, meth_link},
								{UNLINK, meth_unlink},
								{PURGE, meth_purge},*/
								{NONE, meth_none}
							};
		};
		void start(byte * mac, IPAddress ip, IPAddress dns, IPAddress gwAddr, IPAddress subnet, uint16_t port);
		void proccess(void);
		void pushEntry(uint8_t m, const __FlashStringHelper* p, callback_t callback);
		void getTime();

	private:
		char path[MAX_PATH];
		EthernetServer server;
		EthernetClient client;
		uint8_t numEntries;
		ServerEntry serverEntries[MAX_ENTRIES];
		int8_t getMethod(void);
		int8_t getPath();
		uint8_t getHeaders();
		MethodMapper methodMap [2];
};

#endif
