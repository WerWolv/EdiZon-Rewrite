# Frequently asked Questions 

!!! Faq 1. How do I use the Memory Editor?
    The integrated Memory Editor works very similar to Cheat Engine, a popular cheat tool for the computer. To edit a value in a game, follow these steps:

    1. Open your game.
    2. Find a value in the game you want to change. Let's say you have 1000 coins in that game.
    3. Return back to the home menu with the game running in the background.
    4. Hold down the R button, open the Album, select EdiZon and open the Cheat Engine menu.
    5. Select a datatype that is big enough to fit your value, select the correct memory region (this is in most cases the HEAP region) and then enter the value you want to search for. In this case 1000.
    6. Let it search and once it's done go back into the game
    7. Change the coin count a little. Let's say you buy something and now you have 900 coins left.
    8. Go back into edizon and search for 900 this time. It will narrow down the amount of candidates.
    9. Repeat that until you only have a few addresses left
    10. Edit the few addresses that are left now to the value you want. Let's say 9999. 
    11. Go back in the game and if you got the correct address, you should now have 9999 coins.

!!! Faq 2. What is a cheat and how do I use them
    Cheats are tiny programs running in the background while you're playing a game that edit the game's memory according to the instructions inside the cheat. Cheats depend a lot on the layout of the game's memory. If the game code changes (via a update for example), the cheat codes break and usually have to be redone from scratch.

    Since SXOS had the first Cheat implementation, many sites call them SXOS Cheats. Atmosphere uses the exact same format though with some extensions. Therefor, all SXOS cheats work on Atmosphere but not all Atmosphere cheats work on SXOS.

    To activate cheats, do the following:

    1. Start your game of choice.
    2. Press the home button to go back to the main menu but don't close the game completely
    3. Hold down the R button and open the Album
    4. Open EdiZon
    5. Select the Cheats Tab
    6. Select the cheats you want to enable

!!! Faq 3. All Cheats are enabled by default. How can I turn this off?
    This is the default option so it doesn't have to depend purely on EdiZon to load it's cheats.

    It can be fixed by opening the `/atmosphere/system_settings.ini` file in a normal text editor and changing `dmnt_cheats_enabled_by_default = u8!0x1` to `dmnt_cheats_enabled_by_default = u8!0x0`.
    Next time you start a game now, all cheats will be disabled and you'll have to open EdiZon to activate the cheats you need.