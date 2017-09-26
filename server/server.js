var net = require('net');
var express = require('express');
var server = express();
var tcp = new net.Socket();

server.use(express.static('client'));

server.get('/', function (req, res) {
    res.sendFile('/index.html');
});

server.get('/send', function (req, res) {
    var msg = req.query['msg'];
    console.log('Sending to Arduino: ' + msg);
    tcp.write(msg);
    res.send('Sent to Arduino: ' + msg);
});

server.listen(3000, function () {
    console.log('Server running');
});

tcp.connect(2001, '192.168.1.1', function () {
    console.log('Connected to Arduino');
});

tcp.on('close', function () {
    console.log('Connection to Arduino closed');
});
