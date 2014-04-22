<h1>RESTduino</h1>
This repository provide a base for an Arduino REST application.

It has a REST server skeleton where you can configure and read multiple sensors connected in your Arduino.

The platform is based for [Arduino Ethernet board]{http://arduino.cc/en/Main/ArduinoBoardEthernet} and the TinkerKit shield.

<h2>Intructions for use</h2>
<ol>
	<li>Download the files and uncompress</li>
	<li>
		Copy the content of the directory "UserGUI/sdFiles" inside
		the SD card that you will use for the Arduino (The SD must been
		formated as FAT16 previously, the factory format usually works).
	</li>
	<li>
		Introduce the SD card inside the Arduino board SD slot.
	</li>
	<li>
		Open using the Arduino IDE the file "ArduinoFirmware.ino" which is
		located under the directory "ArduinoFirmware".
	</li>
	<li>
		Compile and upload the code to the Arduino MCU.
	</li>
	<li>
		Connect the Arduino board to your Ethernet LAN (192.168.10.0 network
		by default). This network must provide internet connection to the board.
		If not it will not be able to get the Google's server time for the
		datalogger.
	</li>
	<li>
		Acccess to Arduino using a web browser. By default: 'http://192.168.10.2'
		(port 80).
	</li>
	<li>
		Hack and Enjoy ;)
	</li>
</ol>

<h2>REST API Paths</h2>
<h3>Port types</h3>
Each connector of the TinkerKit shield is considered as one port. In exception of the
 connectors O0 and O1 which are reserved for SPI bus. These ports can be configured
 by setting a letter as parameter in order to identify the type of port.

| Letter |  Description  |
|--------|---------------|
|   t    |  Temperature  | 
|   p    | Potentiometer |
|   l    |  Light (LDR)  |
|   b    | Logic (Binary)|
|   r    |      Raw      |
|   e    | Not connected |

<h3>Configuration paths</h3>

The paths begining with "/set/" is used for configure the board.

|     Path       |                             Description                            |
|----------------|--------------------------------------------------------------------|
| /set/i0t       | Set the input 0 as temperature type. The 0 can be changed to any   
                   other input port (0-5) and the 't' to any type (t, p, l, b, r, r). 
                                                                                      |
| /set/o2b       | Set the output 2 as logic output. The 2 can be changed to any other 
                   available output (2-5). The available types are logic ('b') and    
                   Void/Empty/NoConnected ('e').                                       
                                                                                      |
| /set/a1        | The letter 'a' indicates that it is setting the network             
                   configuration. The numbers indicates:                               
                   1 -> 192.168.10.130/26:8080                                         
                   2 -> 10.0.1.2/24:80                                                 
                   3 -> 10.0.1.129/26:8080                                             
                   4 -> 192.168.1.2/24:80                                              
                   Other -> 192.168.10.2/24:80                                         
                                                                                      |
| /set/nDev_Name | Set the device name. The maximum length is 15 characters. The space 
                   character is substituted by '_'.                                    
                                                                                      |
| /set/t1        | Set the sampling time of the datalogger. The available options are 
                   the following:                                                     
                   1 -> 5 seconds                                                     
                   2 -> 10 seconds                                                    
                   3 -> 1 minute                                                      
                   4 -> 5 minutes                                                     
                   5 -> 10 minutes                                                    
                   Other -> 1 second                                                  
                                                                                      |



<h2>Current status</h2>
The sensor platform is already running. It has been improved inperformance as much as possible
(without programm hardware).

By now the code is being improved (robust way). Also, It being developed a remote server in Node.JS for
manage the sensor values (logic tier).



