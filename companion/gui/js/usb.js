var usb = require('usb');

let devices = usb.getDeviceList();
let device;
let interface;
let inEndpoint;
let outEndpoint;
let testString = "Yaniv";
let testData;

usb.on('attach', function(connected_device) {
    if (connected_device.deviceDescriptor.idVendor == 0x057e && connected_device.deviceDescriptor.idProduct == 0x3000) {
        console.log("Switch Detected!");
        interactToDevice(connected_device);
    }
});

usb.on('detach', function(connected_device) {
    if (connected_device.deviceDescriptor.idVendor == 0x057e && connected_device.deviceDescriptor.idProduct == 0x3000) {
        console.log("Switch Disconnected!");
    }
});

for (var i=0;i<devices.length;i++) {
    if (devices[i].deviceDescriptor.idVendor == 0x057e && devices[i].deviceDescriptor.idProduct == 0x3000) {
        interactToDevice(devices[i]);
    }
}

function interactToDevice(dev) {
    device = dev;
    device.open();

    interface = device.interfaces[0];
    interface.claim();

    inEndpoint = interface.endpoints[0];
    outEndpoint = interface.endpoints[1];
    
    testData = new Array(255).fill(0);
    testString.split("").forEach(function(item, index) {
        item = item.charCodeAt(0);
        testData[index] = item;
    });

    inEndpoint.on("data", function(data) {
        console.log("Received: ");
        console.log(String.fromCharCode.apply(null, data));
    });
    inEndpoint.on("error", function(err) {
        console.log(err);
    });
    inEndpoint.startPoll(1, 255);

    /*inEndpoint.transfer(255, function (error, data) {
        if (!error) {
            console.log("Received: ");
            console.log(String.fromCharCode.apply(null, data));
        } else {
            console.log(error);
        }
    });*/

    setInterval(function() {
        outEndpoint.transfer(testData, function(err) {
            console.log(err);
        });    
    }, 1000);
    
}