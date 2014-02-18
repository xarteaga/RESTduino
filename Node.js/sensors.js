/* LIBRARIES */
var fs = require('fs');
var platform = require('./platform');

/* PLATFORM LIST */
var platforms = null;

function getPlatforms (){
	return platforms;
}

function printPlatforms () {
	platforms.forEach(function(element){
		console.log('\tIP: ' + element.ip + '\tPORT: ' + element.port);
		if (element.config != 'undefined'){
			console.log(element);
		}
	});
}

function load (fileName){
	fs.readFile(fileName, 'utf8', function (err, data) {
		if (err) {
			return console.log(err);
		}
		try {
			platforms = JSON.parse(data);
		} catch (e) {
			console.error('Error parsing list ' + e);
		}
		platforms.forEach(function(element){
			platform.getConfig(element, function(config){
				element.config = config;
				console.log(element);
			});
		});
		console.log('THE FOLLOWING PLATFORMS HAVE BEEN LOADED:');
		printPlatforms();
		return;
	});
}

function printResponse (res){
	console.log('STATUS: ' + res.statusCode);
	console.log('HEADERS: ' + JSON.stringify(res.headers));
	res.setEncoding('utf8');
	res.on('data', function(chunk){
		console.log('BODY: ' + chunk);
	});
}

exports.load = load;
exports.getPlatforms = getPlatforms;
