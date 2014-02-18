var http = require("http");

function getConfig(platform, callback) {
    console.log("[PLATFORM.JS] Getting CONFIG of platform " + platform.ip + ":" + platform.port);
    var options = {
        hostname: platform.ip,
        port: platform.port,
        path: '/config',
        method: 'GET'
    };
    var req = http.request(options, function (res) {
        res.on('data', function (data) {
            console.log("[PLATFORM.JS] Got CONFIG of platform " + platform.ip + ":" + platform.port);
            callback(JSON.parse(data));
        });
    });

    req.on('error', function (e) {
        console.log('[PLATFORM.JS] Problem with requesting CONFIG (platform ' + platform.ip + ':' + platform.port + '): ' + e.message);
    });

    req.end();
}

function getPorts(platform, callback) {
    console.log("[PLATFORM.JS] Getting PORTS of platform " + platform.ip + ":" + platform.port);
    var options = {
        hostname: platform.ip,
        port: platform.port,
        path: '/ports',
        method: 'GET'
    };
    var req = http.request(options, function (res) {
        res.on('data', function (data) {
            console.log("[PLATFORM.JS] Got PORTS of platform " + platform.ip + ":" + platform.port);
            callback(JSON.parse(data));
        });
    });

    req.on('error', function (e) {
        console.log('[PLATFORM.JS] Problem with requesting PORTS (platform ' + platform.ip + ':' + platform.port + '): ' + e.message);
    });
    req.end();
}

function printPlatform(platform) {
    console.log("IP: " + platform.ip);
    console.log("Port: " + platform.port);

    if (typeof platform.config !== 'undefined') {
        console.log("Name: " + platform.config.deviceName);
        console.log("Local IP: " + platform.config.ip);
        console.log("Ports: ");
        platform.config.ports.forEach(function (element, index) {
            console.log('\t[' + index + '] Type: ' + element.type);
        });
    }

    if (typeof platform.ports !== 'undefined') {
        console.log("Input ports: ");
        platform.ports.inputs.forEach(function (element, index) {
            console.log('\t[' + index + '] Value: ' + element.val);
        });
        console.log("Output ports: ");
        platform.ports.outputs.forEach(function (element, index) {
            console.log('\t[' + index + '] Value: ' + element.val);
        });
    }
}

exports.getConfig = getConfig;
exports.getPorts = getPorts;
exports.printPlatform = printPlatform;