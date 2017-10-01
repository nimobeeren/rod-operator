var px = new PxGamepad();
px.start();

// Turn LED on when A is pressed
px.on('a', function () {
    console.log('Turning LED on');
    jQuery.get('send', {msg: '1'});
});

// Turn LED off when B is pressed
px.on('b', function () {
    console.log('Turning LED off');
    jQuery.get('send', {msg: '0'});
});

// Update controller input
setInterval(function () {
    px.update();
    document.getElementById('buttons').innerHTML = JSON.stringify(px.buttons, null, 4);
    document.getElementById('sticks').innerHTML = JSON.stringify(px.leftStick, null, 4) + JSON.stringify(px.rightStick, null, 4);
}, 10);
