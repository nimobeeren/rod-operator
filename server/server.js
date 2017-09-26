var express = require('express');
var server = express();

server.use(express.static('client'));

server.get('/', function (req, res) {
    res.sendFile('/index.html');
});

server.listen(3000, function () {
    console.log('Server running');
});
