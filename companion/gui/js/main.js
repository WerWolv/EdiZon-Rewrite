const { app, BrowserWindow } = require("electron");

function createWindow() {
    let win = new BrowserWindow({
        width: 1080,
        height: 720,
        minWidth: 1080,
        minHeight: 720,
        autoHideMenuBar: true,
        frame: false,
        webPreferences: {
            nodeIntegration: true
        }
    });

    win.loadFile("html/index.html");
}

app.on("ready", createWindow);
