import hug
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

@hug.get("/", version=1)
def base():
    return "EdiZon save file provider server"

@hug.get("/motd", version=1)
def getMOTD():
    data = { "motd" : MOTD }

    return data

@hug.get("/name", version=1)
def getServerName():
    data = { "name" : SERVER_NAME }

    return data

@hug.get("/list", version=1)
def getSaveFileList():
    data = { "save_files" : [] }

    for root, dirs, files in os.walk(SAVE_FILE_DIR):
        for file in files:
            data["save_files"].append( { "name" : file, "date" : datetime.datetime.fromtimestamp(os.path.getmtime(f"{root}/{file}")).strftime("%A, %m. %B %Y at %H:%M") })

    return data

@hug.get("/get", version=1, output=hug.output_format.file)
def getFile(fileName : str):
    if ".." in fileName or "/" in fileName or not os.path.exists(f"{SAVE_FILE_DIR}/{fileName}"):
        return "Please provide a valid file name"
    
    return os.path.join(SAVE_FILE_DIR, fileName)

@hug.get("/icon", version=1, output=hug.output_format.file)
def getIcon():
    return os.path.join(".", "icon.jpg")

hug.API(__name__).http.serve(port=PORT)