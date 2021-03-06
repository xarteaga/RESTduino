#include "w5100.h"
#include "socket.h"

extern "C" {
  #include "string.h"
}

#include "Arduino.h"
#include "SD.h"
#include "Ethernet.h"
#include "EthernetClient.h"
#include "EthernetServer.h"
#include "Dns.h"

uint16_t EthernetClient::_srcport = 1024;

EthernetClient::EthernetClient() : _sock(MAX_SOCK_NUM) {
  data_offset = 0;
}

EthernetClient::EthernetClient(uint8_t sock) : _sock(sock) {
  data_offset = 0;
}

int EthernetClient::connect(const char* host, uint16_t port) {
  // Look up the host first
  int ret = 0;
  DNSClient dns;
  IPAddress remote_addr;

  dns.begin(Ethernet.dnsServerIP());
  ret = dns.getHostByName(host, remote_addr);
  if (ret == 1) {
    return connect(remote_addr, port);
  } else {
    return ret;
  }
}

int EthernetClient::connect(IPAddress ip, uint16_t port) {
  if (_sock != MAX_SOCK_NUM)
    return 0;

  for (int i = 0; i < MAX_SOCK_NUM; i++) {
    uint8_t s = W5100.readSnSR(i);
    if (s == SnSR::CLOSED || s == SnSR::FIN_WAIT) {
      _sock = i;
      break;
    }
  }

  if (_sock == MAX_SOCK_NUM)
    return 0;

  _srcport++;
  if (_srcport == 0) _srcport = 1024;
  socket(_sock, SnMR::TCP, _srcport, 0);

  if (!::connect(_sock, rawIPAddress(ip), port)) {
    _sock = MAX_SOCK_NUM;
    return 0;
  }

  while (status() != SnSR::ESTABLISHED) {
    delay(1);
    if (status() == SnSR::CLOSED) {
      _sock = MAX_SOCK_NUM;
      return 0;
    }
  }

  return 1;
}

size_t EthernetClient::write(uint8_t b) {
  return write(&b, 1);
}

size_t EthernetClient::write(const uint8_t *buf, size_t size) {
  if (_sock == MAX_SOCK_NUM) {
    setWriteError();
    return 0;
  }
  if (!send(_sock, buf, size)) {
    setWriteError();
    return 0;
  }
  return size;
}

size_t EthernetClient::flushTx() {  //********************************************
  uint16_t size = data_offset;
  if (!sFlushTx(_sock)) {
    setWriteError();
    size = 0;
  }
  data_offset = 0;
  return size;
}

size_t EthernetClient::pushTx(char c) {
  uint8_t bufferChar = c;
  return pushTx(&bufferChar, 1);
}

size_t EthernetClient::pushTx(char *str) {
  char *p = str;
  size_t err;
  uint8_t bufferChar;
  while (1) {
    bufferChar = *p++;
    if(bufferChar == 0) break;
    err = pushTx(&bufferChar, 1);
    if(err == 0) break;
  }
  return err;
}

size_t EthernetClient::pushTx(const __FlashStringHelper *str) {
  const char * __attribute__((progmem)) p = (const char * ) str;
  size_t err;
  uint8_t bufferChar;
  while (1) {
    bufferChar = pgm_read_byte(p++);
    if(bufferChar == 0) break;
    err = pushTx(&bufferChar, 1); //pushTx nomes retorna 0 si la connexio TCP s'ha tancat
    if(err == 0) break;
  }
  return err;
}

size_t EthernetClient::pushTx(const uint8_t *buf, size_t size) {  // *******************************
  if (!sPushTx(_sock, buf, size, data_offset)) {  //sPushTx nomes retorna 0 si la connexio TCP s'ha tancat
    setWriteError();   // encara que hi hagi error sempre s'ha de cridar flushTx per posar data_offset = 0
    return 0;
  }
  data_offset += size;
  return data_offset;
}

int EthernetClient::available() {
  if (_sock != MAX_SOCK_NUM)
    return W5100.getRXReceivedSize(_sock);
  return 0;
}

int EthernetClient::read() {
  uint8_t b;
  if ( recv(_sock, &b, 1) > 0 )
  {
    // recv worked
    return b;
  }
  else
  {
    // No data available
    return -1;
  }
}

int EthernetClient::read(uint8_t *buf, size_t size) {
  return recv(_sock, buf, size);
}

int EthernetClient::peek() {
  uint8_t b;
  // Unlike recv, peek doesn't check to see if there's any data available, so we must
  if (!available())
    return -1;
  ::peek(_sock, &b);
  return b;
}

void EthernetClient::flush() {
  while (available())
    read();
}

void EthernetClient::stop() {
  if (_sock == MAX_SOCK_NUM)
    return;

  // attempt to close the connection gracefully (send a FIN to other side)
  disconnect(_sock);
  unsigned long start = millis();

  // wait a second for the connection to close
  while (status() != SnSR::CLOSED && millis() - start < 1000)
    delay(1);

  // if it hasn't closed, close it forcefully
  if (status() != SnSR::CLOSED)
    close(_sock);

  EthernetClass::_server_port[_sock] = 0;
  _sock = MAX_SOCK_NUM;
}

uint8_t EthernetClient::connected() {
  if (_sock == MAX_SOCK_NUM) return 0;
  
  uint8_t s = status();
  return !(s == SnSR::LISTEN || s == SnSR::CLOSED || s == SnSR::FIN_WAIT ||
    (s == SnSR::CLOSE_WAIT && !available()));
}

uint8_t EthernetClient::status() {
  if (_sock == MAX_SOCK_NUM) return SnSR::CLOSED;
  return W5100.readSnSR(_sock);
}

// the next function allows us to use the client returned by
// EthernetServer::available() as the condition in an if-statement.

EthernetClient::operator bool() {
  return _sock != MAX_SOCK_NUM;
}
