/******************************************************
 *              CONFIGURE Device Port                 *
 ******************************************************/
function configDevice(id, type) {
    var e = document.getElementById(id);
    switch(type){
        case 'e':
            c = 'btn-default';
            t = 'Void';
            break;
        case 'r':
            c = 'btn-info';
            t = 'Raw';
            break;
        case 'w':
            c = 'btn-success';
            t = 'PWM';
            break;
        case 'p':
            c = 'btn-success';
            t = 'Potentiometer';
            break;
        case 'l':
            c = 'btn-warning';
            t = 'Light';
            break;
        case 't':
            c = 'btn-danger';
            t = 'Temperature';
            break;
        case 'b':
            c = 'btn-primary';
            t = 'Logical';
            break;
        default:
            c = 'btn-default';
            t = 'Error';
            break;
    }
    e.setAttribute('class', 'btn ' + c + ' dropdown-toggle');
    e.setAttribute('Value', t);
    e.innerHTML = t + ' <span class="caret"></span>';
}

/* Count the difference between IPv4 addresses */
function countAddr(ip1Int, ip2Int) {
    return ip1Int[3] - ip2Int[3] + 1 +
        (ip1Int[2] - ip2Int[2]) * 256 +
        (ip1Int[1] - ip2Int[1]) * 256 * 256 +
        (ip1Int[0] - ip2Int[0]) * 256 * 256 * 256;
}

/* Convert an IPv4 String to an 4 integer array */
function convertIPv4FromString(ipIn) {
    res = ipIn.split('.');
    if (res.length != 4) {
        window.alert("Bad format in one IP Address");
        return null;
    }
    ipOut = [];
    res.forEach(function (num) {
        intNum = parseInt(num);
        if (intNum > 255 || intNum < 0) {
            window.alert("The IP address number must be below 255! (Address: $ipIn)");
            return null;
        }
        ipOut.push(intNum);
    });
    return ipOut;
}

/******************************************************
 *        CONVERT INT ARRAY TO IPv4 STRING            *
 ******************************************************/
function convertIPv4FromIntArray(ipIn) {
    return ipIn[0] + "." + ipIn[1] + "." + ipIn[2] + "." + ipIn[3];
}

/******************************************************
 *             ADD ONE TO THE IPv4 ADDR               *
 ******************************************************/
function addOneIPv4(ipIn) {
    var intIp = convertIPv4FromString(ipIn);
    intIp[3]++;
    if (intIp[3] == 256) {
        intIp[3] = 0;
        intIp[2]++;
        if (intIp[2] == 256) {
            intIp[2] = 0;
            intIp[2]++;
            if (intIp[1] == 256) {
                intIp[1] = 0;
                intIp[0]++;
            }
        }
    }
    return convertIPv4FromIntArray(intIp);
}

/******************************************************
 *                VALIDATE IPv4 ADDRESS               *
 ******************************************************/
function validateIPv4(addr) {
    var res = addr.split(".");
    if (res.length != 4) {
        return false;
    }
    res.forEach( function(val){
        val = parseInt(val);
        if (val >256 || val < 0)
            return false;
    });
    return true;
}

/******************************************************
 *                     REST Requests                  *
 ******************************************************/
/* Create Cross domain petition */
function createCORSRequest(method, url) {
	var xhr = new XMLHttpRequest();
	if ("withCredentials" in xhr) {
		// Check if the XMLHttpRequest object has a "withCredentials" property.
		// "withCredentials" only exists on XMLHTTPRequest2 objects.
		xhr.open(method, url, true);
	} else if (typeof XDomainRequest != "undefined") {
		// Otherwise, check if XDomainRequest.
		// XDomainRequest only exists in IE, and is IE's way of making CORS requests.
		xhr = new XDomainRequest();
		xhr.open(method, url);
	} else {
		// Otherwise, CORS is not supported by the browser.
		xhr = null;
	}
	xhr.timeout = 1000;
	xhr.onerror = function(e) {
		stopAutoRead();
		window.alert("The IP address does not point to a valid Server\nURL:'"+url+"'\n" + e);
	};

	xhr.ontimeout = function() {
		stopAutoRead();
		window.alert("The IP address is unreachable.\nURL:'"+url+"'");
	};

  return xhr;
}

/******************************************************
 *          Configure device in same domain           *
 ******************************************************/
function configDevProtected(path) {
    if (document.domain != document.getElementById("setNewIp").value) {
        window.alert("This functionality is only available for computers inside server network! (Address: '" + document.getElementById("setNewIp").value + "')");
        return;
    }

    // Create request and check if the navigation browser is valid
    $.ajax({
        url:path
    }).done(function(data){
        showConfig(data);
    });
}

/******************************************************
 *         UPDATE Ports (for cross Domain)            *
 ******************************************************/
function configDev(sufix) {
    res = document.getElementById("ipAddress").value.split(":");
    port = 80;

    // Check if IP address is valid
    if (!validateIPv4(res[0])) {
        window.alert('IP Address not valid!');
        return;
    }

    if (res.length == 2) {
        port = res[1];
    }

    if (document.domain != res[0]) {
        url = "http://" + res[0] + ":" + port + "/" + sufix;
    } else {
        url = "/" + sufix;
    }

    // Create request and check if the navigation browser is valid
    var xhr = createCORSRequest('GET', url);
    if (!xhr) {
        window.alert('CORS not supported');
        return;
    }

    // Response handler
    xhr.onload = function () {
        showConfig(xhr.responseText);
    };

    xhr.send();
}

/******************************************************
 *                     READ INPUTS                    *
 ******************************************************/
var timer = null;
var failedReads = 0;
function startAutoRead() {
    var refreshTime = document.getElementById("refreshTime").value;
    if (!parseInt(refreshTime) > 0) {
        window.alert("The refresh time must be numeric!");
        return;
    }

    // Check if the timer is enabled
    if (timer != null)
        stopAutoRead();
    failedReads = 0;

    // Change the buttons colors
    document.getElementById("startStopButtons").innerHTML = "<button type='button' class='btn btn-default'>Start</button><button type='button' class='btn btn-danger' onclick='stopAutoRead()'>Stop</button>";

    timer = setInterval(function () {
        readValues();
    }, refreshTime * 1000);
    readValues();
}

function stopAutoRead() {
    clearInterval(timer);
    timer = null;
    document.getElementById("startStopButtons").innerHTML = "<button type='button' class='btn btn-success' onclick='startAutoRead()'>Start</button><button type='button' class='btn btn-default'>Stop</button>";
}

function errorReading (){
    failedReads += 1;
    if (failedReads == 3){
        failedReads = 0;
        stopAutoRead();
        window.alert('Three failed trials to read sensors. Did you write properly the ip address?');
    }
}

function readValues() {
    res = document.getElementById("ipAddress2").value.split(":");

    if (res.length != 2)
        res[1] = 80;

    if (document.domain != res[0])
        url = "http://" + res[0] + ":" + res[1] + "/" + "ports";
    else
        url = "/" + "ports";

    // Check if IP address is valid
    if (!validateIPv4(res[0])) {
        stopAutoRead();
        window.alert('IP Address not valid!');
        return;
    }

	// Create request and check if the navigation browser is valid
	var xhr = createCORSRequest('GET', url);
	if (!xhr) {
		stopAutoRead();
		window.alert('CORS not supported');
		return;
	}
	// Response handlers
	xhr.onload = function() {
		showPorts (xhr.responseText);
	};
	xhr.timeout = 2000;
	xhr.onError = function(e){errorReading();};
	xhr.onTimeout = function(){errorReading();};

	xhr.send();
}

function setOutput(code) {
    res = document.getElementById("ipAddress2").value.split(":");

    if (res.length != 2)
        res[1] = 80;

    if (document.domain != res[0])
        url = "http://" + res[0] + ":" + res[1] + "/" + "out/" + code;
    else
        url = "/" + "out/" + code;

    var xhr = createCORSRequest('GET', url);
    xhr.onload = function () {
        showPorts(xhr.responseText);
    };
    xhr.send();
}

/******************************************************
 *              DEVICE DISCOVERY FUNCTION             *
 ******************************************************/
discoveryContinue = 0;
discoveryCount = 0;
discoveryIndex = 0;
discoveryIpCurrent = 0;

function nextDiscover() {
    // Generate Ping
    new ping(discoveryIpCurrent, function (ip, status, e) {
        var element = document.getElementById("progressbar");
        percent =  parseFloat(element.getAttribute("percent")) + 1 / (discoveryCount) * 100.0;
        element.setAttribute("percent",  percent);
        element.setAttribute("style", "width: " + percent + "%");
        document.getElementById("progresslbl").innerHTML = Math.round(percent) + "% Complete";

        label = 'undefined';
        if (status == "success") {
            label = '<span class="label label-success">Online Arduino</span>';
        } else if (status == "error") {
            label = '<span class="label label-warning">Other host</span>';
        } else if (status == "aborted" && document.getElementById("showOffline").checked == true) {
            label = '<span class="label label-default">Offline</span>';
        } else if (document.getElementById("showOffline").checked == true) {
            label = '<span class="label label-default">Offline</span>';
        }

        if (label != 'undefined') {
            document.getElementById("devicesTable").innerHTML += "<tr><td>" + ip + "</td><td><center>" + label + "</center></td></tr>";
        }
    });

    // Increment the IP address by one
    discoveryIpCurrent = addOneIPv4(discoveryIpCurrent);
    discoveryIndex = discoveryIndex + 1;
    if (discoveryIndex < discoveryCount && discoveryContinue == true)
        discoveryTimer = setTimeout(function () {
            nextDiscover();
        }, 200);
    else
        stopDiscovery();
}

/******************************************************
 *              FIND DEVICE FUNCTION                  *
 ******************************************************/
function startDiscovery() {
    discoveryIpCurrent = document.getElementById("startIP").value;
    startAddrInt = convertIPv4FromString(discoveryIpCurrent);
    stopAddrInt = convertIPv4FromString(document.getElementById("endIP").value);
    if (startAddrInt == null || stopAddrInt == null) {
        return;
    }

    // Check if the IP addresses order
    discoveryCount = countAddr(stopAddrInt, startAddrInt);
    if (discoveryCount < 0){
        window.alert("The start address must be lower than end address (count: " + discoveryCount + ")");
        return;
    }
    console.log('Starting scan... Number of ip addresses: ' + discoveryCount);

    var element = document.getElementById("progressbar");
    element.setAttribute("percent", 0);
    element.setAttribute("style", "width: " + 0 + "%");

    document.getElementById("progresslbl").innerHTML = (0) + "% Complete";
    document.getElementById("devicesTable").innerHTML = "";
    discoveryIndex = 0;

    document.getElementById("scanCtrl").innerHTML =
                    "<button type='button' class='btn btn-default' onclick='startDiscovery()'> Start </button>" +
                    "<button type='button' class='btn btn-danger' onclick='stopDiscovery()'> Stop </button>";
    discoveryContinue = true;
    nextDiscover(); // First discovery
}

function stopDiscovery() {
    discoveryContinue = false;
    var discCtrl = document.getElementById("scanCtrl");
    discCtrl.innerHTML = "<button type='button' class='btn btn-success' onclick='startDiscovery()'> Start </button>";
    discCtrl.innerHTML += "<button type='button' class='btn btn-default' onclick='stopDiscovery()'> Stop </button>";
}

/******************************************************
 *                   PING FUNCTION                    *
 ******************************************************/
function ping(ip, callback) {
// Example: http://jsfiddle.net/Maslow/GSSCD/
    if (!this.inUse) {
        this.status = 'unchecked';
        this.inUse = true;
        this.callback = callback;
        this.ip = ip;
        var _that = this;
        this.xhr = createCORSRequest("GET", "http://" + ip + "/config");
        this.xhr.timeout = 400;

        this.xhr.onload = function () {
            _that.inUse = false;
            _that.callback(_that.ip, 'success');
        };

        this.xhr.onerror = function (e) {
            if (_that.inUse) {
                _that.inUse = false;
                _that.callback(_that.ip, 'error', e);
            }
        };

        this.xhr.ontimeout = function () {
            if (_that.inUse) {
                _that.inUse = false;
                _that.callback(_that.ip, 'timeout', 'timeout');
            }
        };
        this.xhr.onabort = function () {
            if (_that.inUse) {
                _that.inUse = false;
                _that.callback(_that.ip, 'abort', 'computer');
            }
        };
        this.xhr.send();
    }
}

/******************************************************
 *                  Update PORT states                *
 ******************************************************/
function showPorts(txtPort) {
    values = JSON.parse(txtPort);
    table = document.getElementById("analogReads");
    document.getElementById("devName2").innerHTML = values.deviceName.replace("_", " ").replace("%20", " ");
    table.innerHTML = "";
    for (i = 0; i < 6; i++) {
        port = "A" + i;
        value = values.inputs[i].val;
        table.innerHTML += "<tr><td><center>" + port + "</center></td><td><center>" + value + "</center></td></tr>";
    }
    table = document.getElementById("actuators");
    table.innerHTML = "";
    for (i = 0; i < 6; i++) {
        port = "O" + i;
        value = values.outputs[i].val;
        if (value == "On") {
            table.innerHTML += "<tr><td><center>" + port + "</center></td><td><center>" + value + "</center></td><td><div class='btn-group btn-group-sm'><button class='lbl btn-success' onclick='setOutput(\"" + i + "1\")'>On</button><button class='lbl btn-danger btn-xs' onclick='setOutput(\"" + i + "0\")'>Off</button></div></td></tr>";
        } else if (value == "Off") {
            table.innerHTML += "<tr><td><center>" + port + "</center></td><td><center>" + value + "</center></td><td><div class='btn-group btn-group-sm'><button class='lbl btn-success' onclick='setOutput(\"" + i + "1\")'>On</button><button class='lbl btn-danger btn-xs' onclick='setOutput(\"" + i + "0\")'>Off</button></div></td></tr>";
        } else {
            table.innerHTML += "<tr><td><center>" + port + "</center></td><td><center>" + value + "</center></td><td></td></tr>";
        }
    }
}

/******************************************************
 *                     Update CONFIG                  *
 ******************************************************/
function showConfig(txtConfig) {
    var config = JSON.parse(txtConfig);
    document.getElementById("devName").innerHTML = config.deviceName.replace("_", " ").replace("%20", " ");
    switch (config.ip) {
        case "1":
            document.getElementById("setNewIp").innerHTML = "192.168.10.130:8080/26 <span class='caret'></span>";
            if (document.getElementById("setNewIp").value == "")
                document.getElementById("setNewIp").value = "192.168.10.130";
            break;
        case "2":
            document.getElementById("setNewIp").innerHTML = "10.0.1.2:80/24 <span class='caret'></span>";
            if (document.getElementById("setNewIp").value == "")
                document.getElementById("setNewIp").value = "10.0.1.2";
            break;
        case "3":
            document.getElementById("setNewIp").innerHTML = "10.0.1.130:8080/26 <span class='caret'></span>";
            if (document.getElementById("setNewIp").value == "")
                document.getElementById("setNewIp").value = "10.0.1.130";
            break;
        default:
            document.getElementById("setNewIp").innerHTML = "192.168.10.2:80/24 <span class='caret'></span>";
            if (document.getElementById("setNewIp").value == "")
                document.getElementById("setNewIp").value = "192.168.10.2";
            break;
    }

    for (i = 0; i < 6; i++) {
        type = config.ports[i].type;
        configDevice("an" + i, type);
    }
    for (i = 6; i < 12; i++) {
        type = config.ports[i].type;
        configDevice("out" + (i - 6), type);
    }
}

/******************************************************
 *                   INITIALIZE FORM                  *
 ******************************************************/
arduinoIp = document.domain;
if (arduinoIp != "") {
    document.getElementById("ipAddress").value = arduinoIp;
    document.getElementById("ipAddress2").value = arduinoIp;
    intIP = convertIPv4FromString(arduinoIp);
    intIP[3] = 1;
    document.getElementById("startIP").value = convertIPv4FromIntArray(intIP);
    intIP[3] = 255;
    document.getElementById("endIP").value = convertIPv4FromIntArray(intIP);
    configDev("config");
} else {
    document.getElementById("ipAddress").value = "192.168.10.2";
    document.getElementById("ipAddress2").value = "192.168.10.2";
    document.getElementById("startIP").value = "192.168.10.1";
    document.getElementById("endIP").value = "192.168.10.10";
    configDev("config");
}