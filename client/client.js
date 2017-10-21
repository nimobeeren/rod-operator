var px = new PxGamepad();
px.start();

function send(msg) {
    jQuery.get('send', {msg: msg});
}

function binaryToHex(bin) {
    // Convert binary to hex string
    var hex = parseInt(bin, 2).toString(16);

    // Add 0-padding to fill 4 characters
    while (hex.length < 4) {
        hex = '0' + hex;
    }

    return hex;
}

function encode(gamepad) {
    // Encode button states into a 16-digit binary string
    var binary = '';
    Object.keys(gamepad.buttons).forEach(function (button) {
        binary += gamepad.buttons[button] ? '1' : '0';
    });

    // Convert binary to a 4-digit hex string
    var buttons = binaryToHex(binary);

    // Get a 4-character string representing 1 + the left stick X value
    var x = 1 + Math.round(gamepad.leftStick.x * 100) / 100;
    Math.max(0, Math.min(x, 2));
    var leftStickX = x.toFixed(2);

    return buttons + ',' + leftStickX;
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
