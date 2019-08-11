from flask import Flask, escape, request, make_response, send_from_directory
from gevent.pywsgi import WSGIServer
import os
import io
import json
import datetime

###################### SERVER CONFIG ######################

# The port this server should run on
PORT = 1337

# Server name
SERVER_NAME = "EdiZon save file provider"

# Message of the Day. A short description about what kind of save files this server does serve
MOTD = "Save files by WerWolv"

# Path to folder containing save files
SAVE_FILE_DIR = "saves"

###########################################################


app = Flask(MOTD)

@app.route("/", methods=["GET"])
def base():
    return "EdiZon save file provider server"

@app.route("/motd", methods=["GET"])
def getMOTD():
    data = { "motd" : MOTD }

    return sendResponse(data)

@app.route("/name", methods=["GET"])
def getServerName():
    data = { "name" : SERVER_NAME }

    return sendResponse(data)

@app.route("/list", methods=["GET"])
def getSaveFileList():
    data = { "save_files" : [] }

    for root, dirs, files in os.walk(SAVE_FILE_DIR):
        for file in files:
            data["save_files"].append( { "name" : file, "date" : datetime.datetime.fromtimestamp(os.path.getmtime(f"{root}/{file}")).strftime("%A, %m. %B %Y at %H:%M") })

    return sendResponse(data)

@app.route("/get", methods=["GET"])
def getFile():
    fileName = request.args.get("file_name", "..")
    if ".." in fileName or "/" in fileName or not os.path.exists(f"{SAVE_FILE_DIR}/{fileName}"):
        return "Please provide a valid file name"
    
    return send_from_directory(SAVE_FILE_DIR, fileName, as_attachment=True)

@app.route("/icon", methods=["GET"])
def getIcon():
    return send_from_directory(".", "icon.jpg", as_attachment=True)


def sendResponse(data):
    response = make_response(json.dumps(data))
    response.mimetype = 'application/json'

    return response


if __name__ == "__main__":
    print(f"Starting EdiZon save file provider server on Port {PORT}...\n")
    print(f"{SERVER_NAME} : {MOTD}")

    http_server = WSGIServer(('0.0.0.0', PORT), app)

    if not os.path.exists(SAVE_FILE_DIR):
        os.mkdir(SAVE_FILE_DIR)

    print("Running!")
    http_server.serve_forever()
