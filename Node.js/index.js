var server = require("./server");
var router = require("./router");
var sensors= require("./sensors");
var sensorsResource = require("./sensorsResource");

server.start(router.route);

var platforms = sensors.load('sensorlist.txt');

setInterval(sensors.updatePorts, 5000);

sensorsResource.setPlatforms(platforms);
router.pushResource("GET", "/wm/sensors", sensorsResource.resource);