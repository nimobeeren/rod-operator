var pollingRate = 20; // Hz

var px = new PxGamepad();
px.start();

function send(msg) {
    jQuery.get('send', {msg: msg});
}

function floatToString(f) {
    var d = Math.round(f * 100) / 100; // round to 2 decimals
    Math.max(0, Math.min(d, 2)); // clamp to [0, 2]
    return d.toFixed(2);
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

    // Get a 4-character string equal to 1 + the stick values
    var leftStickX = floatToString(gamepad.leftStick.x + 1);
    var leftStickY = floatToString(gamepad.leftStick.y + 1);
    var rightStickX = floatToString(gamepad.rightStick.x + 1);
    var rightStickY = floatToString(gamepad.rightStick.y + 1);

    // Get a 4-character string equal to the trigger values
    var leftTrigger = floatToString(gamepad.leftTrigger);
    var rightTrigger = floatToString(gamepad.rightTrigger);

    // Return encoded gamepad state
    return buttons + ',' + leftStickX + ',' + leftStickY + ',' + rightStickX + ',' + rightStickY + ',' +
        leftTrigger + ',' + rightTrigger + ';';
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
}, 1000 / pollingRate);
