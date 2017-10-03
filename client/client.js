var px = new PxGamepad();
px.start();

px.on('rightTrigger', function () {
    console.log('Moving forward');
    jQuery.get('send', {msg: 'rt'});
});

px.on('leftTrigger', function () {
    console.log('Moving backward');
    jQuery.get('send', {msg: 'lt'});
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
