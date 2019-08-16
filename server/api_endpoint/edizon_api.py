from flask import Flask, escape, request, make_response, send_from_directory
from gevent.pywsgi import WSGIServer
import json
import time
import threading
import requests
import asyncio
import os

import discord
from discord.ext import commands
from discord.utils import get


try:
    import config
except ImportError:
    import config_tempalate as config

app = Flask("EdiZon API endpoint")
bot: commands.Bot = commands.Bot(command_prefix=".")

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

    if oldTime[0] == 0 or time.time() - oldTime[0] >= 60 * 5:
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

@app.route("/v4/notifications", subdomain="api.edizon", methods=["GET"])
def getNotifications():
    global oldTime

    if oldTime[2] == 0 or time.time() - oldTime[2] >= 60 * 5:
        fileUpdateMutex.acquire()
        oldTime[2] = time.time()

        try:
            with open("./data/notifications.json") as file:
                cachedData[2] = file.read()
        except:
            cachedData[2] = "{ \"error\": 3 }"
            oldTime[2] = 0
        finally:
            fileUpdateMutex.release()

    return cachedData[2]


def sendResponse(data):
    response = make_response(json.dumps(data))
    response.mimetype = 'application/json'

    return response

async def updateCheatCount():
    channel = bot.get_channel(611978928923607062)
    await welchannel.edit(name="Hello World")

async def startDiscordBot():
    await bot.start(config.DISCORD_BOT_CLIENT_SECRET)
    bot.loop.create_task(updateCheatCount)


def loopDiscordBot(loop):
    loop.run_forever()


def initDiscordBot():
    loop = asyncio.get_event_loop()
    loop.create_task(startDiscordBot())

    thread = threading.Thread(target=loopDiscordBot, args=(loop,))
    thread.start()

def hasElevatedRights(ctx):
    if not ctx.guild:
        return False
    return any(role.id in config.DISCORD_BOT_ELEVATED_COMMAND_ROLES for role in ctx.author.roles)

# Links to most used EdiZon related resources
@bot.command(aliases=['links'])
async def edizon(ctx):
    embed = discord.Embed(colour=discord.Colour(0xf5a623))

    embed.set_author(name="Links", url="http://werwolv.net", icon_url="https://raw.githubusercontent.com/WerWolv/EdiZon/master/icon.jpg")

    embed.add_field(name="__EdiZon Release__",          value="**NRO:** https://github.com/WerWolv/EdiZon/releases/latest\n**Cheats/Configs:** http://werwolv.net/api/edizon/v2/build.zip")
    embed.add_field(name="__EdiZon Nightly__",          value="**NRO:** http://bsnx.lavatech.top/edizon/edizon-latest.zip\n**Cheats/Configs:** http://werwolv.net/api/edizon/v3/build.zip")
    embed.add_field(name="__Uploaded save files__",     value="https://edizon.werwolv.net")
    embed.add_field(name="__GitHub repos__",            value="**EdiZon:** https://github.com/WerWolv/EdiZon\n**Database:** https://github.com/WerWolv/EdiZon_CheatsConfigsAndScripts")

    await ctx.send(embed=embed)

@commands.guild_only()
@commands.check(hasElevatedRights)
@bot.command()
async def notification(ctx, message):
    if message == "":
        return
    
    notifications = None
    
    fileUpdateMutex.acquire()
    
    if not os.path.exists(""):
        with open("./data/notifications.json", "w") as file: 
            file.write("{ \"notifications\": [ ] }")

    with open("./data/notifications.json", "r+") as file:
        notifications = json.loads(file.read())
        file.seek(0)
        file.truncate()

        notifications["notifications"].append({ "timestamp": int(time.time()), "message": message })
        file.write(json.dumps(notifications, indent=4))

    fileUpdateMutex.release()

    await ctx.message.add_reaction("👍")


if __name__ == "__main__":

    print(f"Starting EdiZon Discord Bot")

    initDiscordBot()

    print(f"Starting EdiZon API endpoint v4")

    http_server = WSGIServer(('0.0.0.0', 80), app)
    app.config["SERVER_NAME"] = "localhost"

    http_server.serve_forever()
