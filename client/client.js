var px = new PxGamepad();
px.start();

px.on('a', function () {
    console.log('Moving forward');
    jQuery.get('send', {msg: '1'});
});

px.on('b', function () {
    console.log('Moving backward');
    jQuery.get('send', {msg: '2'});
});

px.on('x', function () {
    console.log('Stopping');
    jQuery.get('send', {msg: '0'});
});

setInterval(function () {
    px.update();
    document.getElementById('buttons').innerHTML = JSON.stringify(px.buttons, null, 4);
    document.getElementById('sticks').innerHTML = JSON.stringify(px.leftStick, null, 4) + JSON.stringify(px.rightStick, null, 4);
}, 10);
