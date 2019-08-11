var net = require('net');
var remote = require('electron').remote;

function init() {
    if (remote.getCurrentWindow().isMaximized())
        document.getElementById("maximize-button").innerText = "🗗";
    else
        document.getElementById("maximize-button").innerText = "🗖";
}

function closeWindow() {
    remote.getCurrentWindow().close();
}

function maximizeWindow() {
    if (remote.getCurrentWindow().isMaximized()) {
        remote.getCurrentWindow().restore();
        document.getElementById("maximize-button").innerText = "🗖";
    }
    else {
        remote.getCurrentWindow().maximize();
        document.getElementById("maximize-button").innerText = "🗗";
    }
}

function minimizeWindow() {
    remote.getCurrentWindow().minimize();
}

var server = net.createServer(function(socket) {
    socket.write('Echo server\r\n');
    socket.pipe(socket);
});

server.listen(1337, '127.0.0.1');

function tcp() {
    var client = new net.Socket();
    
    client.connect(1337, '127.0.0.1', function() {
        console.log('Connected');
        client.write('Hello, server! Love, Client.');
    });
    
    client.on('data', function(data) {
        console.log('Received: ' + data);
        client.destroy(); // kill client after server's response
    });
    
    client.on('close', function() {
        console.log('Connection closed');
    });
}
