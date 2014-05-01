<h1>RESTduino</h1>
This repository provide a base for an Arduino REST application.

It has a REST server skeleton where you can configure and read multiple sensors connected in your Arduino.

The platform is based in [Arduino Ethernet board]{http://arduino.cc/en/Main/ArduinoBoardEthernet} and the TinkerKit shield.

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

<h2>Requests Paths</h2>
The device is configured using the path of the HTTP requests. These paths set 
the device configuration which is stored in the EEPROM internal memory. In fact,
the paths begining with "/set/" are used for configure the board. This path is
followed by parameter to being configured. The allowed parameters are the following:

|  Char  |         Description       |
|--------|---------------------------|
|   ixy  |        Input port         | 
|   oxy  |        Output Port        |
|   ay   |     Network address       |
|   ty   | Data logger sampling time |
|   py   |  TCP Maximum Packet Size  |
|   ny   |        Device name        |

Where x is the number of ports and y the parameter. The response of these requests
is the current configuration after apply the changes in JSON format.

<h3>Port types</h3>
Each connector of the TinkerKit shield is considered as one port. In exception of the
output connectors O0 and O1 which are reserved for SPI bus. These ports can be 
configured by setting a letter as parameter in order to identify the type of port.

| Option |  Description  |
|--------|---------------|
|   t    |  Temperature  | 
|   p    | Potentiometer |
|   l    |  Light (LDR)  |
|   b    | Logic (Binary)|
|   r    |      Raw      |
|   e    | Not connected |

There is an example of an URL which sets the input number 2 to temperature.
'''url
	http://192.168.10.2/set/i2p
'''

<h3>Predefinied network addresses</h3>
Due the fact that Arduino's microcontroller has limited computing resources, it has a list
of predefined IP addresses and they are selected using the correct request.

| Option |        Description     |
|--------|------------------------|
|    1   | 192.168.10.130/26:8080 |
|    2   | 10.0.1.2/24:80         |
|    3   | 10.0.1.129/26:8080     |
|    4   | 192.168.1.2/24:80      |
|  Other | 192.168.10.2/24:80     |

There is an example of an URL which sets the network address to ''10.0.1.129'' and the
netmask to ''255.255.255.192'' (26 bit).
'''url
	http://192.168.10.2/set/a3
'''
The new IP address will be applied after reset the board.

<h3>Data logger sampling time</h3>
The sata logger sampling time has been also predefinied using some options in order to avoid
use extra computing resources due parsing number in the URL.

| Option | Description |
|--------|-------------|
|    1   | 5 seconds   |
|    2   | 10 seconds  |
|    3   | 1 minute    |
|    4   | 5 minutes   |
|    5   | 10 minutes  |
|  Other | 1 second    |

There is an example of an URL which sets the sampling time to 10 minutes:
'''url
	http://192.168.10.2/set/t5
'''
The new sampling time will be applied after the next sample.

<h3>Device name</h3>
As said previously each device can be named with a name with lesss than 16 characters. It 
is only admitted letters, numbers and '_' which acts as an space in the Arduino's memory.

There is an example of an URL which sets the device name as ''Hello World'':
'''url
	http://192.168.10.2/set/nHello_World
'''
The new device name will be applied instantly.

<h2>Current status</h2>
The sensor platform is already running. The performance has been improved as much as possible.




