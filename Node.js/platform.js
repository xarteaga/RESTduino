var http = require("http");

function getConfig(platform, callback){
	console.log("[PLATFORM.JS] Getting config of platform " + platform.ip + ":" + platform.port);
	var options = {
		hostname: platform.ip,
		port: platform.port,
		path: '/config',
		method: 'GET'
	};
	var req = http.request(options, function(res){
		res.on('data', function (data) {
			callback(JSON.parse(data));
		});
	});
	
	req.on('error', function(e) {
		console.log('Problem with request: ' + e.message);
	});

	req.end();
}

function getPorts(platform, callback) {
	var options = {
		hostname: platform.ip,
		port: platform.port,
		path: '/config',
		method: 'GET'
	};
	var req = http.request(options);

	req.on('error', function(e) {
		console.log('Problem with request: ' + e.message);
	});
}

exports.getConfig = getConfig;
exports.getPorts = getPorts;
