var px = new PxGamepad();
px.start();

px.on('leftTrigger', function () {
    send('lt');
});

px.on('rightTrigger', function () {
    send('lt');
});

px.on('b', function () {
    send('b');
});

px.on('leftTop', function () {
    send('lb');
});

px.on('rightTop', function () {
    send('rb');
});

px.on('y', function () {
    send('y');
});

px.on('x', function () {
    send('x');
});

setInterval(function () {
    px.update();
    document.getElementById('buttons').innerHTML = JSON.stringify(px.buttons, null, 4);
    document.getElementById('sticks').innerHTML = JSON.stringify(px.leftStick, null, 4) + JSON.stringify(px.rightStick, null, 4);
}, 10);

function send(msg) {
    jQuery.get('send', {msg: msg});
}
