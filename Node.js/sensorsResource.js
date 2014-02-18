var genericResponses = require("./genericResponses");

var platforms = null;

exports.setPlatforms = function(_platforms){
    platforms =_platforms;
}

function getPlatforms(response){
    console.log(platforms.length);
    console.log('[SENSORS.JS] SERVING sensor platforms.');
    response.writeHead(200, {"Content-Type": "application/json"});
    response.write(JSON.stringify(platforms));
    response.end();
}

function getPlatform(index, response){
    index = parseInt(index);
    if (index >= platforms.length || isNaN(index)) {
        console.log('[SENSORS.JS] Exception serving platform, index out of bounds (' + index + ').');
        genericResponses.badRequest(response);
        return;
    }

}

function getPlatformConfig(index, response){
    index = parseInt(index);
    if (index >= platforms.length || isNaN(index)) {
        console.log('[SENSORS.JS] Exception serving platform, index out of bounds (' + index + ').');
        genericResponses.badRequest(response);
        return;
    }
    console.log('[SENSORS.JS] SERVING sensor platform CONFIG with index ' + index + '.');
    response.writeHead(200, {"Content-Type": "application/json"});
    response.write(JSON.stringify(platforms[index].ports));
    response.end();
}

function getPlatformPorts(index, response){
    index = parseInt(index);
    if (index >= platforms.length || isNaN(index)) {
        console.log('[SENSORS.JS] Exception serving platform, index out of bounds (' + index + ').');
        genericResponses.badRequest(response);
        return;
    }
    console.log('[SENSORS.JS] SERVING sensor platform PORTS with index ' + index + '.');
    response.writeHead(200, {"Content-Type": "application/json"});
    response.write(JSON.stringify(platforms[index].ports));
    response.end();
}

exports.resource = function(method, path, response){
    var parts = path.split('/');
    if (method == "GET" && path == "/") {
        console.log('[SENSORS.JS] SERVING sensor platforms.');
        getPlatforms(response);
        return;
    } else if (method == "GET" && parts[1] == 'platform' && parts[2] != 'undefined') {
        if (typeof parts[3] === 'undefined') {
            getPlatform(parts[2], response);
            return;
        } else if (parts[3] == 'config') {
            getPlatformConfig(parts[2], response);
            return;
        } else if (parts[3] == 'ports') {
            getPlatformPorts(parts[2], response);
            return;
        }
    }
    console.log('[SENSORS.JS] NOT found resource entry for ' + method + ' ' + path);
    genericResponses.notImplemented(response);
}