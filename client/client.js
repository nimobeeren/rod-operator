var px = new PxGamepad();
px.start();

px.on('leftTrigger', function () {
    jQuery.get('send', {msg: 'lt'});
});

px.on('rightTrigger', function () {
    jQuery.get('send', {msg: 'rt'});
});

px.on('b', function () {
    jQuery.get('send', {msg: 'b'});
});

px.on('leftTop', function () {
    jQuery.get('send', {msg: 'lb'});
});

px.on('rightTop', function () {
    jQuery.get('send', {msg: 'rb'});
});

px.on('y', function () {
    jQuery.get('send', {msg: 'y'});
});

px.on('x', function () {
    jQuery.get('send', {msg: 'x'});
});

setInterval(function () {
    px.update();
    document.getElementById('buttons').innerHTML = JSON.stringify(px.buttons, null, 4);
    document.getElementById('sticks').innerHTML = JSON.stringify(px.leftStick, null, 4) + JSON.stringify(px.rightStick, null, 4);
}, 10);
