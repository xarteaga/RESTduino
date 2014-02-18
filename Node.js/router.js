
var resources = [];

function pushResource(method, path, callback){
    for (var i = 0; i<resources.length; i++){
        if (method == resources[i].method && path == resources[i].path){
            console.log('[ROUTER.JS] Modifying resource entry for ' + method + ' ' + path);
            array[index].callback = callback;
            return;
        }
    }
    console.log('[ROUTER.JS] Pushing resource entry for ' + method + ' ' + path);
    resources.push({
        method: method,
        path: path,
        callback: callback
    });
}

function deleteResource(method, path){
    for (var i = 0; i<resources.length; i++){
        if (method == resources[i].method && path == resources[i].path){
            console.log('[ROUTER.JS] Deleting resource entry for ' + method + ' ' + path);
            array.splice(i, i);
            return;
        }
    }
    console.log('[ROUTER.JS] NOT found resource entry for ' + method + ' ' + path);
}

function route(method, path, response) {
	console.log('[ROUTER.JS] Processing request (' + method + ' ' + path + ')');
    var found = false;

    for (var i = 0; i<resources.length; i++){
        if (method == resources[i].method && path.substring(0,resources[i].path.length) == resources[i].path){
            resources[i].callback(method, path.substring(resources[i].path.length), response);
            return;
        }
    }
    console.log('[ROUTER.JS] NOT found resource entry for ' + method + ' ' + path);
    response.writeHead(501, {"Content-Type": "text/plain"});
    response.write("Not Implemented");
    response.end();
}

exports.route = route;
exports.pushResource = pushResource;
exports.deleteResource = deleteResource;