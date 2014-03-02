#ifndef file_resources_h
#define file_resources_h

inline void fileRequest (const __FlashStringHelper* filePathP, EthernetClient * client);

int8_t serveIndex (uint8_t method, char* path, EthernetClient *client);

int8_t serveCss (uint8_t method, char* path, EthernetClient *client);

int8_t serveCssRes (uint8_t method, char* path, EthernetClient *client);

int8_t serveJQuery (uint8_t method, char* path, EthernetClient *client);

int8_t serveControl (uint8_t method, char* path, EthernetClient *client);

int8_t serveBs (uint8_t method, char* path, EthernetClient *client);

int8_t serveImg (uint8_t method, char* path, EthernetClient *client);

#endif
