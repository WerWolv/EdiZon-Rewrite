from flask import Flask, escape, request, make_response, send_from_directory
from gevent.pywsgi import WSGIServer
import json
import time
import threading
import requests

app = Flask("EdiZon API endpoint")

oldTime = [ 0, 0, 0, 0 ]
cachedData = [ 0, 0, 0, 0 ]
fileUpdateMutex = threading.Lock()


@app.route("/", subdomain="api.edizon", methods=["GET"])
def base():
    return "EdiZon save file provider server"

# Returns a list of official save file provider servers
@app.route("/v4/official_providers", subdomain="api.edizon", methods=["GET"])
def getOfficialProviders():
    global oldTime
    global cachedData

    if oldTime[0] == 0 or time.time() - oldTime[0] >= 3600:
        fileUpdateMutex.acquire()
        oldTime[0] = time.time()

        try:
            with open("./data/official_providers.json") as file:
                cachedData[0] = file.read()
        except:
            cachedData[0] = "{ \"error\": 1 }"
            oldTime[0] = 0
        finally:
            fileUpdateMutex.release()
    
    response = make_response(cachedData[0])
    response.mimetype = 'application/json'

    return response


# Returns information about the latest EdiZon release like title, changelog, version number, release date and download count
@app.route("/v4/release", subdomain="api.edizon", methods=["GET"])
def getLatestRelease():
    global oldTime

    if oldTime[1] == 0 or time.time() - oldTime[1] >= 60:
        oldTime[1] = time.time()

        response = json.loads(requests.get("https://api.github.com/repos/WerWolv/EdiZon/releases/latest").text)
        
        try:
            cachedData[1] = { "name": response["name"], "changelog": response["body"].replace("\r\n", "\n"), "tag": response["tag_name"], "date": response["published_at"], "download_count": response["assets"][0]["download_count"] }
        except:
            cachedData[1] = { "error": 2 }

    return sendResponse(cachedData[1])

@app.route("/v4/release", subdomain="api.edizon", methods=["GET"])


def sendResponse(data):
    response = make_response(json.dumps(data))
    response.mimetype = 'application/json'

    return response



if __name__ == "__main__":
    print(f"Starting EdiZon API endpoint v4\n")

    http_server = WSGIServer(('0.0.0.0', 80), app)
    app.config["SERVER_NAME"] = "localhost"

    http_server.serve_forever()
