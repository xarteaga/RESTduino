// Definitions
#define SERIAL_BAUDRATE 115200
#define REQUEST_MAXBUFFER 30
#define HEADER_MAXBUFFER 30
#define DEV_NAME_MAX_LEN 16
// Compilation options
//#define DEBUG_EEPROM
//#define DEBUG
#define PRINT_HEADERS
#define VERBOSE
#define ALLOW_CACHE

// Includes
#include <SPI.h>
#include <SD.h>
#include <Ethernet.h>
#include <EEPROM.h>

EthernetClient client;

// Request declarions
char request [REQUEST_MAXBUFFER];

// Initialize the Ethernet server library
// with the IP address and port you want to use 
// (port 80 is default for HTTP):
EthernetServer server = EthernetServer(80);

/*****************************************************************************************************
 *                                                  EEPROM                                           *
 *****************************************************************************************************/
// EEPROM BASE ADDR
#define _EEPROM_BASE   0x10

// Define input/output/name/address/samplingtime config sufixes
#define _INPUT         'i'
#define _OUTPUT        'o'
#define _NAME          'n'
#define _ADDR          'a'
#define _SAMPT         't'
#define _PACK          'p'

// Analog inputs
#define _EMPTY         'e'
#define _RAW           'r'
#define _TEMPERATURE   't'
#define _POTENCIOMETER 'p'
#define _LIGHT         'l'

// Outputs
#define _LOGICAL       'b'  // 'b' of binary
#define _PWM           'w'  // 'w' of width

// Header options
#define _REQ_OK        0x01
#define _NOT_MODIFIED  0x02

// Sampling time definitions
#define TS_0 1000 /* 1 Second */
#define TS_1 5000 /* 5 Seconds */
#define TS_2 10000 /* 10 Seconds */
#define TS_3 60000 /* 1 minute */
#define TS_4 300000 /* 5 minute */
#define TS_5 600000 /* 10 minute */

// Packet length definitions
#define LEN_0 4 /* 4 x 128 = 512 */
#define LEN_1 6 /* 6 x 128 = 768 */
#define LEN_2 8 /* 8 x 128 = 1024 */
#define LEN_3 10 /* 10 x 128 = 1280 */

// Configuration structure
struct Configuration{
  char inputs [6];
  char outputs [6];
  byte ip;
  char devName[DEV_NAME_MAX_LEN];
  byte samplingTime;
  byte packetSize;
};
Configuration conf;

struct SInputRawValues{
  unsigned int nextTimeStamp;
  unsigned short int inputs [6];
};
SInputRawValues inputRawValues;

int freeRam () 
{
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

void updateValues (){
  char timeString[29]="nan";
  File file;
  char fileName [] = "ANX.DAT";
  unsigned int timeStamp = millis();
  
  if (timeStamp < inputRawValues.nextTimeStamp)
    return;
    
  if (!getTime(timeString))
    return;
    
  for (byte i = 0; i<29; i++){
    EEPROM.write(400+i, timeString[i]);
  }
  
  switch(conf.samplingTime){
    case '0':
      inputRawValues.nextTimeStamp = timeStamp + TS_0;   
      break;
    case '1':
      inputRawValues.nextTimeStamp = timeStamp + TS_1;   
      break;
    case '2':
      inputRawValues.nextTimeStamp = timeStamp + TS_2;   
      break;
    case '3':
      inputRawValues.nextTimeStamp = timeStamp + TS_3;   
      break;
    case '4':
      inputRawValues.nextTimeStamp = timeStamp + TS_4;   
      break;
    case '5':
      inputRawValues.nextTimeStamp = timeStamp + TS_5;   
      break;
    default:
      inputRawValues.nextTimeStamp = timeStamp + TS_1;   
      break;
  }
  
  unsigned short int * rawValues = inputRawValues.inputs;
  for (byte i = 0; i<6; i++){
    rawValues[i] = analogRead(i);
    fileName[2] = 0x30 + i;
  
    file = SD.open(fileName, FILE_WRITE);
    if (file){
      file.print(F(",{\"date\":\""));
      file.write((uint8_t*)timeString, 29);
      file.print(F("\",\"value\":\""));
      file.print(rawValues[i]);
      file.print("\"}");
      file.close();
    } else {
      Serial.print(F("Error opening file: '"));
      Serial.print(fileName);
      Serial.println(F("'"));
    }
  }
  Serial.println(freeRam());
}

/*****************************************************************************************************
 *                                            Request Read                                           *
 *****************************************************************************************************/
byte requestRead () {
  char header [HEADER_MAXBUFFER];
  boolean currentLineIsBlank = true;
  boolean firstLine = true;
  byte reqLen = 0;
  byte headLen = 0;
  byte val = 0x00;
  
  while (client.connected() && client.available()) {
    // Read header
    char c = client.read();
    
    // Store in request only the first line with the httpmethod and the path
    if (firstLine && reqLen < REQUEST_MAXBUFFER-1){
      request[reqLen] = c;
      reqLen ++;
    } else if (headLen < HEADER_MAXBUFFER-1) {
      header[headLen] = c;
      headLen ++;
    }
    
    // Detect the end of the header
    if (c == '\n' && firstLine) {
      request[reqLen-1] = '\n';
      request[reqLen] = '\0';
      Serial.print(request);
      //Serial.println(F("### Ok Request ###"));
      val += _REQ_OK;
    } else if (c == '\n') {
      header[headLen-1] = '\n';
      header [headLen] = '\0';
      #ifdef PRINT_HEADERS
      Serial.print(header);
      #endif // PRINT_HEADERS
      #ifdef ALLOW_CACHE
      if ((strncmp(header, "If-None-Match: contentNotModified", 23)==0 /*||
            strncmp(header, "Cache-Control: max-age=0",23)==0*/) &&
            (val & _NOT_MODIFIED) == 0){
        val += _NOT_MODIFIED;
        Serial.println(F("NO MODIFIED DETECTED!"));
      }
      #endif // ALLOW_CACHE
    }
    
    if (c == '\n' && currentLineIsBlank){
      return val;
    }
    
    // Detect line jumps
    if (c == '\n') {
      firstLine = false;
      currentLineIsBlank = true;
      reqLen = 0;
      headLen = 0;
    } else if (c != '\r') {
      currentLineIsBlank = false;
    }
  }
  return val;
}

boolean getTime(char * timeString) {
        char buffer[4];
        if (!client.connect({173,194,34,56}, 80)) {
                Serial.println(F("Error: Can not get Google Time!"));
                return false;
        }
        client.println("GET /gen_204 HTTP/1.1\nConnection: close\n");
 
        // Wait response
        delay(100);
 
        // Read headers until it finds the Date
        while (client.connected() && client.available()) {
                // read buffer
                client.read((uint8_t*)buffer, 4);
                // Detect if the response header is the date one
                if (strncmp(buffer, "Date", 4) == 0) {
                        // Get date string
                        client.read();
                        client.read();
                        client.read((uint8_t*)timeString, 29);
                        
                        // Close connection
                        client.stop();
                        return true;
                } else {
                        // Advance the reading pointer
                        while(client.read()!='\n');
                }
        }
 
  // Force close if not
  if (client.connected())
    client.stop();
  return false; 
}

/*****************************************************************************************************
 *                                          Configure Arduino                                        *
 *****************************************************************************************************/
void configure (char* code){
  byte nport, type;
  char inout;
  
  // Mode the pointer to the begining of the configuration code
  code ++;
  while(*code != '/')
    code ++;
  code ++;

  inout = code[0];        // Get the type of port: input, output or device name
  nport = code[1] - 0x30; // Get the port number (if any)
  type = code[2];         // Get port configuration
  
  switch(inout){
    case _INPUT:
      if (nport > 5) return;  // If not valid value, return
      conf.inputs[nport] = type;
      EEPROM.write(_EEPROM_BASE + nport, type);
      break;
    case _OUTPUT:
      if (nport > 5) return;  // If not valid value, return
      conf.outputs[nport] = type;
      EEPROM.write(_EEPROM_BASE + nport + 6, type);
      break;
    case _NAME:
      byte devNameLen; // Declare the device name length
      code ++; // Adjust config code pointer
      /* Look for the end of the name, in case of being longer then DEV_NAME_MAX_LEN-1
       * it will be cut to this number. */
      for (devNameLen = 0; devNameLen<DEV_NAME_MAX_LEN-1 && code[devNameLen]!=' '; devNameLen++);
      // Copy the Device name in the config structure
      strncpy(conf.devName, code, devNameLen);
      conf.devName[devNameLen] = '\0';  // End wil the end string (null) char
      checkDeviceName(conf.devName);
      // Copy the device name in the EEPROM
            Serial.println(conf.devName);

      for (byte i = 0; i<devNameLen+1; i++)
        EEPROM.write(_EEPROM_BASE + (size_t)&conf.devName-(size_t)&conf + i, conf.devName[i]);
      break;
    case _ADDR:
      code ++;
      conf.ip = *code - 0x30;
      EEPROM.write(_EEPROM_BASE + 12, conf.ip);
      break;
    case _SAMPT:
      code ++;
      conf.samplingTime = *code;
      EEPROM.write(_EEPROM_BASE + ((size_t)&conf.samplingTime - (size_t)&conf), *code);
      break;
    case _PACK:
      code++;
      conf.packetSize = *code;
      EEPROM.write(_EEPROM_BASE + ((size_t)&conf.packetSize - (size_t)&conf), *code);
      break;
  }
}

/*****************************************************************************************************
 *                                          SET Output                                               *
 *****************************************************************************************************/
void setOutput (char * cmd) {
  const byte outTable [6] = { 11, 10, 9, 6, 5, 3};
  cmd += 4;
  byte nport = *cmd - 0x30;
  if (nport > 5)
    return;
  cmd ++;
  switch(*cmd){
  case '0':
    digitalWrite(outTable[nport], LOW);
    break;
  case '1':
    digitalWrite(outTable[nport], HIGH);
    break;
  }
}

/*****************************************************************************************************
 *                                 ALL Sensors values Request (JSON)                                 *
 *****************************************************************************************************/
void portsRequested (){
  char base []= "_{\"type\":\"X\",\"val\":\"XXXX\"}_";
  char baseEmpty []= "_{\"val\":\"Void\"}_";
  char actBaseOn []=    "_{\"val\":\"On\"}_";
  char actBaseOff []=   "_{\"val\":\"Off\"}_";
  const byte outTable [6] = { 11, 10, 9, 6, 5, 3 };
  client.pushTx("{\"deviceName\":\"");
  client.pushTx(conf.devName);
  client.pushTx("\",\"timestamp\":\"");
  for (uint8_t i = 0; i<29; i++){
    client.pushTx(EEPROM.read(400+i));
  }
  client.pushTx("\",\"inputs\":");
  for (char i = 0; i < 6; i++) {
    short raw = inputRawValues.inputs[i]; // Read analog port value
    base[0] = (i==0)?'[':' ';
    base[10] = conf.inputs[i]; // type
    base[23] = 0x30 + raw%10; // Units
    raw /= 10;
    base[22] = 0x30 + raw%10; // Decs
    raw /= 10;
    base[21] = 0x30 + raw%10; // Hundreds
    base[20] = 0x30 + raw/10; // Thousands
    base[sizeof(base)-2] = (i!=5) ? ',':']';
    client.pushTx(base);
  }
  client.pushTx(",\"outputs\":");
  for (char i = 0; i < 6; i++) {
    char type = conf.outputs[i];
    if (type == _LOGICAL){
      if (digitalRead(outTable[i])==1){
        actBaseOn[0] = (i==0)?'[':' ';
        actBaseOn[13] = (i!=5)? ',':']';
        client.pushTx(actBaseOn);
      } else {
        actBaseOff[0] = (i==0)?'[':' ';
        actBaseOff[14] = (i!=5)? ',':']';
        client.pushTx(actBaseOff);
      }
    } else {
      baseEmpty[0] = (i==0)?'[':' ';
      baseEmpty[15] = (i!=5)? ',':']';
      client.pushTx(baseEmpty);
    }
  }
  client.pushTx("}\n");
  client.flushTx();
}

/*****************************************************************************************************
 *                                   Ports types request (JSON)                                      *
 *****************************************************************************************************/
void confRequest (){
  char confBase [] = "_{\"type\":\"X\"}_}";
  client.pushTx("{\"deviceName\":\"");
  client.pushTx(conf.devName);
  client.pushTx("\",\"ip\":\"");
  client.pushTx(0x30 + conf.ip);
  client.pushTx("\",\"samplingTime\":\"");
  client.pushTx((char)conf.samplingTime);
  client.pushTx("\",\"packetSize\":\"");
  client.pushTx((char)conf.packetSize);
  client.pushTx("\",\"ports\":");
  for (char i=0; i<12; i++){
    confBase [0] = (i==0)?'[':' ';
    confBase [13] = (i!=11)?',':']';
    confBase [14] = (i!=11)?'\0':'}';
    confBase[10] = conf.inputs[i];
    client.pushTx(confBase);
  }
  client.flushTx();
}

/*****************************************************************************************************
 *                                            File request                                           *
 *****************************************************************************************************/
void fileRequest (const __FlashStringHelper* filePathP) {
  uint8_t packLen;
  #define BUFFLEN 128 // 8*128 = 1024 (Size of packets in bytes)
  char  filePath[16];
  strcpy_P(filePath, (const prog_char*)filePathP); // Convert File path in RAM path from Flash
  size_t i;                // Counter
  uint8_t buffer[BUFFLEN]; // Buffer, the data from the SD and Ethernet are comming and going throught
                           // SPI, therefore, it is needed a buffer
  File file = SD.open(filePath);  // Open SD file
  
  // Get packet length
  switch(conf.packetSize){
     case '0':
       packLen = 4;
       break;
     case '1':
       packLen = 6;
       break;
     case '2':
       packLen = 8;
       break;
     default:
       packLen = 10;
       break;
  }
  
  if (file) {
    // read from the file until there's nothing else in it:
    while (file.available()) {
      for (i = 0; i < packLen && file.available(); i++){
        // Read BUFFLEN characters and push it in the Socket buffer
        client.pushTx(buffer, file.read(buffer, BUFFLEN));
      }
      
      if (!client.connected())
        break;
        
      // Empty the tx buffer (send buffered data)
      client.flushTx();
    }

    // Close the file:
    file.close();
  } else {
    Serial.print(F("File '"));
    Serial.print(filePath);
    Serial.println(F("' not found!"));
  }
}

boolean isValidChar4Name (char c){
  if (c>0x2F && c<0x3A)
    return true;
  else if (c>0x40 && c<0x5B)
    return true;
  else if (c>0x60 && c<0x7B)
    return true;
  else if (c=='_')
    return true;
    
  return false;
}

boolean checkDeviceName (char * devName){
  byte i = 0;
  for (i=0; i<DEV_NAME_MAX_LEN; i++){
    if (!isValidChar4Name(devName[i])){
      if (devName[i]=='\0' && i!=0)
        return true;
      // Invalid character
      Serial.println(devName);
      devName[0] = 'W';
      devName[1] = 'e';
      devName[2] = 'b';
      devName[3] = 'd';
      devName[4] = 'u';
      devName[5] = 'i';
      devName[6] = 'n';
      devName[7] = 'o';
      devName[8] = '\0';
      return false;
    }
  }
  devName[i] = 0; // Force End 
  return false;
}

inline void resetHistory(){
    File file;
  char fileName []= "ANX.DAT";
  for (byte i = 0; i<6; i++){
    fileName[2] = 0x30 + i;
    // Remove file
    SD.remove(fileName);
    
    if (SD.exists(fileName)){
      Serial.print(F("Error removing file "));
      Serial.println(fileName);
    }
    
    file = SD.open(fileName, FILE_WRITE);
    file.close();
    
    if (!SD.exists(fileName)){
      Serial.print(F("Error creating file "));
      Serial.println(fileName);
    }
  }
}

/*****************************************************************************************************
 *                                          SETUP Subroutine                                         *
 *****************************************************************************************************/
void setup() {
  // Digital output vector
  const char outTable [6] = { 11, 10, 9, 6, 5, 3 };
  
  // Open serial communications and wait for port to open:
  Serial.begin(SERIAL_BAUDRATE);
  Serial.println(F("--- Arduino Ethernet Debug interface ---"));

  // Setup ports configuration
  byte* confptr = (byte*)&conf;
  for (byte i=0; i<sizeof(Configuration); i++){
    confptr[i] = EEPROM.read(_EEPROM_BASE + i);
  }
  
  // Check the device name and configuration (first time name setting)
  char *devName = conf.devName;
  if (!checkDeviceName(devName)){
    for (byte i = 0; i<DEV_NAME_MAX_LEN; i++){
        EEPROM.write(_EEPROM_BASE + (size_t)&conf.devName-(size_t)&conf + i, conf.devName[i]);
    }
  }

  // Check the sampling time is bounded
  if (conf.samplingTime>'9' || conf.samplingTime<'0')
    conf.samplingTime = '0';
    
    // Check the packet size is bounded
  if (conf.packetSize>'9' || conf.packetSize<'0')
    conf.packetSize = '0';

  /* Uncomment next line to reset network Addr */
  //configure("//a0");

  // start the Ethernet connection and the server:
  byte ipAddr [4];
  byte gwAddr [4];
  byte subnet [4];
  byte dns[4] = {147, 83, 2, 3};
  byte i;
  uint16_t port = 0;
  
  if (conf.ip==1){ /* --- NETWORK CONFIGURATION 1 --- */
    // Ip Address 192.168.10.130
    ipAddr[0] = 192;
    ipAddr[1] = 168;
    ipAddr[2] = 10;
    ipAddr[3] = 130;
    // Gateway Address 192.168.10.1
    gwAddr[0] = 192;
    gwAddr[1] = 168;
    gwAddr[2] = 10;
    gwAddr[3] = 129;
    // Subnet Mask 255.255.255.192 (/26)
    subnet[0] = 255;
    subnet[1] = 255;
    subnet[2] = 255;
    subnet[3] = 192;
    // Port 8080
    port = 8080;
  } else if (conf.ip==2){
    // Ip Address 10.0.1.2
    ipAddr[0] = 10;
    ipAddr[1] = 0;
    ipAddr[2] = 1;
    ipAddr[3] = 2;
    // Gateway Address 10.0.1.1
    gwAddr[0] = 10;
    gwAddr[1] = 0;
    gwAddr[2] = 1;
    gwAddr[3] = 1;
    // Subnet Mask 255.255.255.192 (/24)
    subnet[0] = 255;
    subnet[1] = 255;
    subnet[2] = 255;
    subnet[3] = 0;
    // Port 80
    port = 80;
  } else if (conf.ip==3){
    // Ip Address 10.0.1.130
    ipAddr[0] = 10;
    ipAddr[1] = 0;
    ipAddr[2] = 1;
    ipAddr[3] = 130;
    // Gateway Address 10.0.1.129
    gwAddr[0] = 10;
    gwAddr[1] = 0;
    gwAddr[2] = 1;
    gwAddr[3] = 129;
    // Subnet Mask 255.255.255.192 (/26)
    subnet[0] = 255;
    subnet[1] = 255;
    subnet[2] = 255;
    subnet[3] = 192;
    // Port 8080
    port = 8080;
  } else { /* --- NETWORK CONFIGURATION 0 --- */
    // Ip Address 192.168.10.2
    ipAddr[0] = 192;
    ipAddr[1] = 168;
    ipAddr[2] = 10;
    ipAddr[3] = 2;
    // Gateway Address 192.168.10.1
    gwAddr[0] = 192;
    gwAddr[1] = 168;
    gwAddr[2] = 10;
    gwAddr[3] = 1;
    // Subnet Mask 255.255.255.0 (/24)
    subnet[0] = 255;
    subnet[1] = 255;
    subnet[2] = 255;
    subnet[3] = 0;
    // Port 80
    port = 80;
  } 
  
  // Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
  byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
  IPAddress ip (ipAddr[0],ipAddr[1],ipAddr[2],ipAddr[3]);
  Ethernet.begin(mac, ip, dns, gwAddr, subnet); 
  
  server.setPort(port);
  server.begin();
  
  Serial.print(F("Server is at "));
  Serial.print(Ethernet.localIP());
  Serial.print(F(":"));
  Serial.println(port);
  Serial.print(F("Gateway: "));
  for (i = 0; i<4; i++){
    Serial.print(gwAddr[i]);
    if (i!=3)
      Serial.print(F("."));
  }
  Serial.print(F("\nNetmask: "));
  for (i = 0; i<4; i++){
    Serial.print(subnet[i]);
    if (i!=3)
      Serial.print(F("."));
  }
  Serial.print(F("\n"));
  
  //  Print Device name
  Serial.print(F("Device name: "));
  Serial.print(conf.devName);
  Serial.print('\n');
  
  // Setup outputs
  for (byte i = 0; i < 6; i++){
    pinMode(outTable[i], OUTPUT);
  }
 
  // Create SD
  digitalWrite(10, LOW);
  digitalWrite(10, HIGH);
  if (!SD.begin(4)) {
    Serial.println(F("SD Initialization failed!"));
  }

  resetHistory();
  
  inputRawValues.nextTimeStamp = millis();
}

void loop() {
  updateValues();
  
  // Pointer to the flash memory
  const __FlashStringHelper* jsonHeadPath = F("/JSONHEAD.HDR");
  const __FlashStringHelper* htmlHeadPath = F("/HTMLHEAD.HDR");
  const __FlashStringHelper* jsHeadPath = F("/JSHEAD.HDR");
  const __FlashStringHelper* cssHeadPath = F("/CSSHEAD.HDR");

  // Listen for incoming clients
  client = server.available();
  char* path = request;

  if (!client)
    return;
    
  while (client && client.available())
  {  
    #ifdef DEBUG_HTTP
      Serial.println(F("--- Begin of the Request ---"));
    #endif // DEBUG_HTTP
    
    byte headers = requestRead();
    if ((headers & _REQ_OK) == 0)
      return;
    
    while(*path != '/')
      path++;
    path++;
        
    if (strncmp(request, "GET", 3)!=0)
      return;
  
    if ((headers & _NOT_MODIFIED) != 0){
        Serial.println(F("Sending not modified"));
        client.print("HTTP/1.1 304 - Not Modified\n");
    } else if (strncmp(path, "ports", 5) == 0) {
      fileRequest(jsonHeadPath);
      portsRequested();
    } else if (strncmp(path, "config", 6) == 0){
      fileRequest(jsonHeadPath);
      confRequest();
    } else if (strncmp(path, "set", 3)==0){
      configure(path);
      fileRequest(jsonHeadPath);
      confRequest();
    } else if (strncmp(path, "out",3)==0){
      setOutput(path);
      fileRequest(jsonHeadPath);
      portsRequested();
    } else if (strncmp(path, "index", 5) == 0){
      fileRequest(htmlHeadPath);
      fileRequest(F("/INDEX.TXT"));
    } else if (strncmp(path, "bs.css", 6) == 0){
      fileRequest(cssHeadPath);
      fileRequest(F("/BS.CSS"));
    } else if (strncmp(path, "bs-res", 6) == 0){
      fileRequest(cssHeadPath);
      fileRequest(F("/BSR.CSS"));
    } else if (strncmp(path, "jquery", 6) == 0){
      fileRequest(jsHeadPath);
      fileRequest(F("/JQUERY.TXT"));
    } else if (strncmp(path, "control", 7) == 0){
      fileRequest(jsHeadPath);
      fileRequest(F("/CTRL.TXT"));
    } else if (strncmp(path, "bs.js", 5) == 0){
      fileRequest(jsHeadPath);
      fileRequest(F("/BSJS.TXT"));
    } else if (strncmp(path, "shield", 6) == 0){
      fileRequest(F("/SHIELD.TXT"));
    } else if (*path == ' '){
      fileRequest(htmlHeadPath);
      fileRequest(F("/INDEX.TXT"));
    /*}  else if (strncmp_P(path, "graph", 5) == 0){
      fileRequest(htmlHeadPath);
      fileRequest(F("/GRAPH.TXT"));*/
    } else if (strncmp(path, "histX", 4) == 0){
      switch(path[4]){
        case 'r':
          resetHistory();
          fileRequest(jsonHeadPath);
          portsRequested();
          break;  
        case '0':
          fileRequest(jsonHeadPath);
          client.pushTx("[0");
          fileRequest(F("/AN0.DAT"));
          break;    
        case '1':
          fileRequest(jsonHeadPath);
          client.pushTx("[0");
          fileRequest(F("/AN1.DAT"));
          break;
        case '2':
          fileRequest(jsonHeadPath);
          client.pushTx("[0");
          fileRequest(F("/AN2.DAT"));
          break;
        case '3':
          fileRequest(jsonHeadPath);
          client.pushTx("[0");
          fileRequest(F("/AN3.DAT"));
          break;
        case '4':
          fileRequest(jsonHeadPath);
          client.pushTx("[0");
          fileRequest(F("/AN4.DAT"));
          break;
        case '5':
          fileRequest(jsonHeadPath);
          client.pushTx("[0");
          fileRequest(F("/AN5.DAT"));
          break;
        case '6':
          fileRequest(jsonHeadPath);
          client.pushTx("[0");
          fileRequest(F("/AN6.DAT"));
          break;
        default:
          client.print("HTTP/1.1 404 - File not Found\n");
          break;          
      }
    
      client.pushTx("]");
      client.flushTx();
    } else {
      Serial.println(F("### Bad Path request ###"));
      client.print("HTTP/1.1 404 - File not Found\n");
    }
    #ifdef DEBUG_HTTP
    Serial.println(F("---  End of the Request  ---"));
    #endif // DEBUG_HTTP
  }
  // close the connection:
  if (client){
    client.stop();
  }
}

