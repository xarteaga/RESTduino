<h1>RESTduino</h1>
This repository provide a base for an Arduino REST application engine.
It has a REST server skeleton where you can configure and read multiple
sensors connected in your Arduino.

<h2>Intructions for use it</h2>
<ol>
	<li>Download the files and uncompress</li>
	<li>Generate the SD files executing the batch script "generateSDdata.bat"
	inside the "UserGUI" folder.</li>
	<li>
		Copy the content of the directory "UserGUI/sdFiles" inside
		the SD card that you will use for the Arduino (The SD must been
		formated as FAT16 previously).
	</li>
	<li>
		Introduce the SD card inside the Arduino board SD slot.
	</li>
	<li>
		Open using the Arduino IDE the file "ArduinoFirmware.ino" which is
		located under the directory "ArduinoFirmware".
	</li>
	<li>
		Compile and load the code to the Arduino MCU.
	</li>
	<li>
		Connect the Arduino board to your Ethernet LAN.
	</li>
	<li>
		Acccess to Arduino using a web browser. By default: 'http://192.168.10.2'
	</li>
	<li>
		Hack and Enjoy ;)
	</li>
</ol>
