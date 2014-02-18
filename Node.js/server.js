var http = require("http");
var url = require("url");

function start(route) {
	function onRequest(request, response) {
        console.log('[SERVER.JS] New Request.');
		route(request.method, url.parse(request.url).pathname, response);
	}
  
	http.createServer(onRequest).listen(8888);
	console.log("Server has started.");
}

exports.start = start;
