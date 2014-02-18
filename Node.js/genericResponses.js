exports.notImplemented = function (response){
    response.writeHead(501, {"Content-Type": "text/plain"});
    response.write("Not Implemented");
    response.end();
}

exports.badRequest = function (response){
    response.writeHead(400, {"Content-Type": "text/plain"});
    response.write("Bad request");
    response.end();
}