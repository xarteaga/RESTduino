/* LIBRARIES */
var fs = require('fs');
var platform = require('./platform');

var platforms = [];

function getPlatforms() {
    return platforms;
}

function printPlatforms() {
    platforms.forEach(function (element) {
        platform.printPlatform(element);
    });
}

function load(fileName) {
    console.log('[SENSORS.JS] Loading platform list.');
    var data = fs.readFileSync(fileName, 'utf8', function (err) {
            return console.log(err);
    });

    try {
        platforms = platforms.concat(JSON.parse(data));
        console.log(platforms.length);
    } catch (e) {
        console.error('Error parsing list ' + e);
    }

    platforms.forEach(function (element) {
        platform.getConfig(element, function (config) {
            element.config = config;
            platform.getPorts(element, function (ports) {
                element.ports = ports;
                platform.printPlatform(element);
            });
        });
    });
    return platforms;
}

function save(fileName) {
    fs.writeFile(fileName, JSON.stringify(platforms), function (err) {
        if (err) {
            console.log('[SENSORS.JS] An error ocurred saving the platform list (' + err.message + ').');
        } else {
            console.log('[SENSORS.JS] Platforms saved.');
        }
    });
}

function putPlatform(element) {
    platforms.push(elememnt);
}

function deletePlatform(index) {
    if (index > platforms.length) {
        console.log('[SENSORS.JS] Platform index "' + index + '" is out of bounds.');
        return;
    }
    platforms.splice(index, index);
    console.log('[SENSORS.JS] Platform index "' + index + '" has been deleted.');
}

function printResponse(res) {
    console.log('STATUS: ' + res.statusCode);
    console.log('HEADERS: ' + JSON.stringify(res.headers));
    res.setEncoding('utf8');
    res.on('data', function (chunk) {
        console.log('BODY: ' + chunk);
    });
}

function updatePorts() {
    platforms.forEach(function (element) {
        platform.getPorts(element, function (ports) {
            element.ports = ports;
        });
    });
}

exports.load = load;
exports.save = save;
exports.putPlatform = putPlatform;
exports.deletePlatform = deletePlatform;
exports.getPlatforms = getPlatforms;
exports.updatePorts = updatePorts;