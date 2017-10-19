var px = new PxGamepad();
px.start();

function send(msg) {
    jQuery.get('send', {msg: msg});
}

function encode(gamepad) {
    // Encode button states into a 4 charachter hex string
    var buttonsBinary = '';
    for (var button in gamepad.buttons) {
        if (!gamepad.buttons.hasOwnProperty(button)) continue;
        buttonsBinary += gamepad.buttons[button] ? '1' : '0';
    }
    var buttonsHex = parseInt(buttonsBinary, 2).toString(16);
    while (buttonsHex.length < 4) {
        buttonsHex = '0' + buttonsHex;
    }

    return buttonsHex;
}

setInterval(function () {
    // Update gamepad state
    px.update();

    if (px.getGamepad()) {
        // Reflect gamepad state on UI, for debugging
        document.getElementById('buttons').innerHTML = JSON.stringify(px.buttons, null, 4);
        document.getElementById('sticks').innerHTML = JSON.stringify(px.leftStick, null, 4) + JSON.stringify(px.rightStick, null, 4);

        // Send encoded gamepad state to Arduino
        send(encode(px));
    } else {
        document.getElementById('buttons').innerHTML = 'No gamepad connected';
        document.getElementById('sticks').innerHTML = '';
    }
}, 10);
