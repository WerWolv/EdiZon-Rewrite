# EdiZon
  <p align="center"><img src="https://raw.githubusercontent.com/WerWolv/EdiZon/master/icon.jpg"><br />
      <a href="https://github.com/WerWolv/EdiZon/releases/latest"><img src="https://img.shields.io/github/downloads/WerWolv/EdiZon/total.svg" alt="Latest Release" /></a>
    <a href="https://discord.gg/qyA38T8"><img src="https://discordapp.com/api/guilds/465980502206054400/embed.png" alt="Discord Server" /></a>
    <a href="https://travis-ci.com/WerWolv/EdiZon"><img src="https://travis-ci.com/WerWolv/EdiZon.svg?branch=master" alt="Build Status" /></a>
  </p>

A Homebrew save file manager, editor, cheat engine, cheat creator and button sequence mapper for Horizon, the OS of the Nintendo Switch.

# Overview
  EdiZon consists of 6 different main functionalities.
  - **Save file management**
    - Extraction of game saves.
    - Injection of extracted game saves (Your own and your friends save files).
    - Extraction and injection of all save files found on your device at once.
    - Creations of save file systems when injecting a save file (No need to open every game first!)
    - Automatic backups of all your save files and optionally uploading them to a remote server
    - Uploading of savefiles directly to https://anonfile.com.
    - Batch extraction of all save files of all games on the system.
  - **Save file repositories**
    - Open source server to host save file repositories online for everybody to access them
    - Official repos for Mario Maker 2, 100% files and more
  - **Save file editing**
    - Easy to use, scriptable and easily expandable on-console save editing.
      - Lua and Python script support.
    - Built-in save editor updater.
  - **On-the-fly memory editing**
    - Cheat Engine like RAM editing.
    - Freezing of values in RAM via Atmosphère's cheat module.
    - Interface for loading, managing and updating Atmosphère cheats.
    - Screen overlay to manage your cheats and modify your RAM without leaving the game you're playing
  - **Virtual controllers**
    - Map button combos and button press sequences to any joycon button
    - Redirect controller inputs from your computer to the switch or vice versa
  - **Computer companion app**
    - Extensive interface tool to inspect your console's RAM and create cheats from it
    - Save file management from your computer
    - Connection over USB or WiFi

  All packed into one easy to use and easy to install Homebrew.

# Images
  <p align="center"><img src="https://raw.githubusercontent.com/WerWolv/EdiZon/master/assets/main_menu.jpg"></p>
  <p align="center"><img src="https://raw.githubusercontent.com/WerWolv/EdiZon/master/assets/save_editor_1.jpg"></p>
  <p align="center"><img src="https://raw.githubusercontent.com/WerWolv/EdiZon/master/assets/save_editor_2.jpg"></p>
  <p align="center"><img src="https://raw.githubusercontent.com/WerWolv/EdiZon/master/assets/ram_editor.jpg"></p>

# Save editor Config and Script files

  To download working Editor Config and Editor Script files, visit [this repository](https://github.com/WerWolv/EdiZon_ConfigsAndScripts/tree/master)

  Check out our [Wiki page](https://github.com/WerWolv/EdiZon/wiki) for more information on how to build your own Editor Config and Editor Script files.

# How to install

  1. Download the latest release from the [GitHub release page](https://github.com/WerWolv/EdiZon/releases/latest).
  2. Unpack the downloaded zip file, put the files on your Nintendo Switch's SD card and let the folders merge.
  3. Use a free open source CFW like [Atmosphère](https://github.com/Atmosphere-NX/Atmosphere) to launch the hbmenu and start EdiZon from there.
     1. If you want to use the cheat manager you absolutely have to use [Atmosphère](https://github.com/Atmosphere-NX/Atmosphere) as only their cheats are supported.
     2. For the best experience, open the `/atmosphere/system_settings.ini` file and change `dmnt_cheats_enabled_by_default = u8!0x1` to `dmnt_cheats_enabled_by_default = u8!0x0`.


# How to compile

  1. Clone the EdiZon repo to your computer using `git clone https://github.com/WerWolv/EdiZon --recurse-submodules`.
  2. Download and install devkitA64. It comes bundled with the [devkitPro](https://devkitpro.org) toolchain.
  3. Use the pacman package manager that comes with devkitPro to download and install libnx, the switch portlibs (`switch-portlibs`), the switch OpenGL libs (`switch-glfw`, `switch-mesa` and `switch-glm`).
  4. The rest of the compilation works using the `make` command. Type `make application` to build only EdiZon, `make sysmodule` to build only the companion sysmodule or just `make` to build everything.
  5. If you want to use the debug utilities built into the makefile, you also need `python3` and `lftp` installed on your computer and [sys-ftpd](https://github.com/jakibaki/sys-ftpd) installed on your Switch.

# Discord

  For support with the usage of EdiZon or the creation of save editor configs and scripts, feel free to join the EdiZon server on Discord: https://discord.gg/qyA38T8

# Credits

  Thanks to...

  - [devkitPro](https://devkitpro.org) for their amazing toolchain!
  - [SwitchBrew](https://switchbrew.org/) for the [Homebrew Launcher](https://github.com/switchbrew/nx-hbmenu) GUI and shared font code.
  - [jakibaki](https://github.com/jakibaki) for his massive help with the implementation of RAM editing and sys-netcheat which was used as inspiration.
  - [averne](https://github.com/averne) for his amazing research around overlay drawing and the implementation of the overlay drawing code
  - [SciresM](https://github.com/SciresM) for the aarch64 hardware accelerated SHA256 code, libstratosphere, his implementation of the Atmosphère cheat engine and his support during development.
  - [onepiecefreak](https://github.com/onepiecefreak3) for generally being helpful during development
  - [Ac_K](https://github.com/AcK77) for help with the server side update scripts, the EdiZon save website and reverse engineering of IPC calls.
  - **kardch** for the beautiful current icon.
  - **bernv3** for the beautiful old icon.
  - **All save editor and cheat creators** for bringing this project to life!

  <br>

  - [nlohmann](https://github.com/nlohmann) for his great json library.
  - [Lua](https://www.lua.org/) for their scripting language.
  - [Python](https://www.python.org/) and [nx-python](https://github.com/nx-python) for their scripting language respectively their python port to the switch.


  <br>
  <p align="center"><img src="https://www.lua.org/images/logo.gif">
  <img src="https://upload.wikimedia.org/wikipedia/commons/c/c3/Python-logo-notext.svg"><p>
