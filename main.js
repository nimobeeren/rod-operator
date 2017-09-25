var px = new PxGamepad();

px.start();

px.on('start', function() {
    alert('Start button pressed');
});

setInterval(function() {
    px.update();
    document.getElementById('buttons').innerHTML = JSON.stringify(px.buttons);
    document.getElementById('sticks').innerHTML = JSON.stringify(px.leftStick) + JSON.stringify(px.rightStick);
}, 10);
