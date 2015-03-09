
Pebble.addEventListener('ready',
    function(e) {
    console.log('JavaScript app ready and running!');
    }
);

function AppMessage(dict) {
    Pebble.sendAppMessage(dict,
                          function(e) {console.log('AppMessage successful.');},
                          function(e) {console.error('AppMessage failed!');} );
}

function SendPOST(message) {
    var request = new XMLHttpRequest();
    request.onload = function() {
        if (request.readyState == 4 && request.status == 200) {
            var response = JSON.parse(request.responseText);
            console.log("Got response:", JSON.stringify(response));
            AppMessage({ 'KEY_JS_STATUS':200 });
        } else {
            console.error('Error bad response');
            AppMessage({ 'KEY_JS_STATUS':0 });
        }
    };

    request.open("POST", "http://10.0.0.3:8000", true);
    request.setRequestHeader("Content-type", "application/json");
//     var message = {"Hello!":12};
//     console.error("Sending data:", JSON.stringify(data));
    request.send(JSON.stringify(message));
//     console.error("Data sent");
}

function SendStart() {
    // TODO more meaningful data sent with the start command such as time or duration
    var start = {"start":35};
    SendPOST(start);
}

function SendData(data) {
    SendPOST(data);
}

function SendStop() {
    var stop = {"stop":34};
    SendPOST(stop);
}

// Convert from little endian
function convert(bytes) {
    var data = 0;
    for (var i=0; i<bytes.length/2; i++) {
//         data[i] = bytes[i] + (bytes[i+1] << 1); // assuming little endian right now
        data += bytes[i] << i;
    }
    return data;
}


// Add event listener to Pebble sending an app message. Send the data to the web from that callback.
Pebble.addEventListener('appmessage',
    function(e) {
//         console.error('js got appmessage');
        
//         SendData("Hello");
//         SendStart2();
//         SendPOST({"Hello!":35}, Hello);
        
        // use e.payload to get the AppMessage Dictionary

        var command = e.payload.KEY_PB_COMMAND;
        if (command == 1) { // start
            SendStart();
        } else if (command == 3) { // stop
            SendStop();
        } else {
            var bytes = e.payload.KEY_PB_DATA;
//             console.log.error(bytes);
            
            // TODO I'm sending 5 samples not just 1
            
            var data = {};
            data.did_vibrate = bytes[0];
            data.timestamp = convert(bytes.slice(1,8));
            data.x = convert(bytes.slice(9,10));
            data.y = convert(bytes.slice(11,12));
            data.z = convert(bytes.slice(13,14));
            
    //         console.log('js sending data')
            SendData(data);
            
            SendData(bytes);
        }
    }
);
