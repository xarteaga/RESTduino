#ifndef file_resources_h
#define file_resources_h

inline void fileRequest (const __FlashStringHelper* filePathP, EthernetClient * client);

void serveIndex (uint8_t method, char* path, EthernetClient *client);

void serveCss (uint8_t method, char* path, EthernetClient *client);

void serveCssRes (uint8_t method, char* path, EthernetClient *client);

void serveJQuery (uint8_t method, char* path, EthernetClient *client);

void serveControl (uint8_t method, char* path, EthernetClient *client);

void serveBs (uint8_t method, char* path, EthernetClient *client);

void serveImg (uint8_t method, char* path, EthernetClient *client);

#endif
