var px = new PxGamepad();

px.start();

px.on('a', function() {
    console.log('Turning LED on');
    jQuery.get('send', { msg: '1' });
});

px.on('b', function() {
    console.log('Turning LED off');
    jQuery.get('send', { msg: '0' });
});

setInterval(function() {
    px.update();
    document.getElementById('buttons').innerHTML = JSON.stringify(px.buttons);
    document.getElementById('sticks').innerHTML = JSON.stringify(px.leftStick) + JSON.stringify(px.rightStick);
}, 10);
