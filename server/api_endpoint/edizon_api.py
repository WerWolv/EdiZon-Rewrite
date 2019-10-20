import hug
import json
import time
import threading
import requests
import asyncio
import os
import inotify.adapters
import inotify.constants


###################### SERVER CONFIG ######################

# The port this server should run on
PORT = 1337

###########################################################


cachedData = [ 0, 0, 0, 0 ]
fileUpdateMutex = threading.Lock()

@hug.get('/', version=1)
def base():
    return "EdiZon API endpoint"

# Returns a list of official save file provider servers
@hug.get('/official_providers', version=1)
def getOfficialProviders():    
    return json.loads(cachedData[0])

# Returns information about the latest EdiZon release like title, changelog, version number, release date and download count
@hug.get('/release', version=1)
def getLatestRelease():
    return cachedData[1]

@hug.get('/notifications', version=1)
def getNotifications():
    return cachedData[2]


def fileListener():
    i = inotify.adapters.Inotify()

    i.add_watch('./data')

    for event in i.event_gen(yield_nones=False):
        (_, type_names, path, filename) = event

        if type_names[0] == 'IN_MODIFY':
            fileUpdateMutex.acquire()

            with open("./data/official_providers.json") as file:
                cachedData[0] = file.read()
            with open("./data/notifications.json") as file:
                cachedData[2] = json.loads(file.read())

            fileUpdateMutex.release()

def timer():
    response = json.loads(requests.get("https://api.github.com/repos/WerWolv/EdiZon/releases/latest").text)
    
    try:
        cachedData[1] = { "name": response["name"], "changelog": response["body"].replace("\r\n", "\n"), "tag": response["tag_name"], "date": response["published_at"], "download_count": response["assets"][0]["download_count"] }
    except:
        cachedData[1] = { "error": 2 }

    time.sleep(60 * 5)


with open("./data/official_providers.json") as file:
    cachedData[0] = file.read()
with open("./data/notifications.json") as file:
    cachedData[2] = json.loads(file.read())

response = json.loads(requests.get("https://api.github.com/repos/WerWolv/EdiZon/releases/latest").text)

try:
    cachedData[1] = { "name": response["name"], "changelog": response["body"].replace("\r\n", "\n"), "tag": response["tag_name"], "date": response["published_at"], "download_count": response["assets"][0]["download_count"] }
except:
    cachedData[1] = { "error": 2 }

fileThread = threading.Thread(target=fileListener)
timerThread = threading.Thread(target=timer)

fileThread.start()
timerThread.start()

hug.API(__name__).http.serve(port=PORT)
