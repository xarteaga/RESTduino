var server = require("./server");
var router = require("./router");
var sensors= require("./sensors");

server.start(router.route);

sensors.load("sensorlist.txt");


