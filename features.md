# EdiZon 4.0.0 Rewrite features

## General ideas
- [ ] Better updater
  - [ ] Choose between release and nightly version
  - [ ] Update put cheats in stray cheats folder or download only cheats for games the user owns
  - [ ] Download only save editors for games the player owns
  - [X] Open source server part so people can host their own
    - [ ] Multiple update servers support
- [X] Via extra sysmodule, possibly map buttons to button sequences, multi buttons, etc? 

## Main menu
- [ ] Batch backup/restore button
- [ ] Open cheat menu automatically if game is running
  - [ ] Don't allow backup/restore/edit while a game is running
  - [ ] ~~Allow overclocking config~~
- [ ] Display info icons above game icon
  - [ ] Running game
  - [ ] Running with cheats
  - [ ] Has save editor
  - [ ] Beta save editor
  - [ ] ???
- [ ] Status bar
  - [ ] Time
  - [ ] Battery percentage
  - [ ] EdiZon version
  - [ ] Connected to PC client
  - [ ] Connected to the internet
  - [ ] Notifications!
    - [ ] Viewable in a separate menu
- [X] All game list
  - [ ] Different sorting options
  - [X] Maybe different display styles?
- [ ] Buttons
  - [ ] Manual button
  - [ ] Notifications button
  - [ ] Credits button
  - [ ] Updater button
- [ ] Built-in Guide
  - [ ] How does save backup/restore work?
  - [ ] How does save editing work?
  - [ ] How do cheats work?
  - [ ] How to use the cheat engine?
  - [ ] ~~How to use the overclocking service?~~
  - [ ] **More in-depth guide in the Electron app**

## Save file management
- [ ] Backup/Restore of save files as usual
- [ ] Upload/Download of save files to open source self hostable servers
   - [ ] Upload of current save file and previously backed up save files
   - [ ] Download and direct injecting of uploaded save files or saving for later
- [ ] Swap current save file with backed up one
- [ ] Batch backups + Restoring of batch backups
- [ ] Optional description for backups 
- [ ] Current save file displayed next to all backed up save files in list
- [ ] Access to save files over USB/TCP via electron client
- [ ] Wipe save file feature
- [ ] Copy save file from one profile to another directly

## Save file editing
- [X] Editing using config and script files as before (Lua + Python)
- [ ] Automatic save states before editing for easy revert back
- [ ] Custom UIs using script files
   - [ ] Custom button, text, images and widget layouts to fit every use case
   - [ ] Allow complete access to all save files in the save file system
   - [ ] Allow access to a folder on the SD card as well to load resources
   - [ ] Allow access to the internet
- [ ] Hex/Text view of save files on console

## Cheat management
- [X] Display TID, PID and BID
- [X] Memory layout view with better explainations
- [X] Interface with dmnt:cht to select running cheats
- [ ] `system_settings.ini` configuration option
- [ ] Stray cheats folder
- [ ] Cheat renaming to current buildID
- [X] Proper error messages why cheats didn't get loaded
- [ ] Limited on-console cheat engine
  - [ ] Search types `==`, `!=`, `<=`, `>=`, `<` and `>`
  - [ ] Data types `u8`, `u16`, `u32`, `u64`, `s8`, `s16`, `s32` and `s64`
  - [ ] Memory Regions `HEAP`, `MAIN` and `HEAP+MAIN`
- [ ] Full cheat finder through additional sysmodule + electron client over USB/TCP
  - [X] Extract and launch sysmodule when needed
  - [ ] Additional features to the above ones
    - [ ] `SAME`, `DIFF`, `INC`, `DEC`, `A:B`
    - [ ] `Find pointers`
    - [ ] `Follow pointer`
    - [ ] Direct memory read and writing in hex-editor style
    - [ ] Watch window
    - [ ] Memory breakpoints with conditionals
