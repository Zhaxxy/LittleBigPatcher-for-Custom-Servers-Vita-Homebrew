---
title: LittleBigPatcher for Custom Servers (Vita Homebrew)
---

# LittleBigPatcher for Custom Servers (Vita Homebrew)
# livearea art and bubble by royalpaks
A simple way to patch your LittleBigPlanet games to connect to custom servers with only your Vita!

# source code
[https://github.com/LittleBigPatcherTeam/LittleBigPatcher-for-Custom-Servers-Vita-Homebrew](https://github.com/LittleBigPatcherTeam/LittleBigPatcher-for-Custom-Servers-Vita-Homebrew)
# Latest release (.vpk download) (can download directly on Vita with [Downloader Enabler plugin](http://github.com/TheOfficialFloW/VitaTweaks/releases/download/DownloadEnabler/download_enabler.suprx))
[http://bit.ly/patchvita](http://bit.ly/patchvita)

# How to use?

## Prerequisites
1. **FrenchAmericanGames Decrypter (FAGDec):** [Download here](https://github.com/CelesteBlue-dev/PSVita-RE-tools/raw/master/FAGDec/build/FAGDec.vpk).
2. **Server Address:** The custom server address you want to use.
3. **LittleBigPlanet Patcher:** [Download here](http://bit.ly/patchvita).

## Step 1: Decrypt the Game with FAGDec
Follow the detailed guide on decrypting your game using FAGDec: [FAGDec Guide](https://littlebigpatcherteam.github.io/2025/03/14/FAGDec-guide.html).

## Step 2: Install LittleBigPlanet Patcher
1. Download the LittleBigPlanet Patcher from [this link](http://bit.ly/patchvita).
2. Install the `.vpk` file on your PS Vita.

## Step 3: Set the Server Address
1. Open the LittleBigPatcher app on your PS Vita.
2. Navigate to **Edit URLs**.
    - If the server URL you want to use is not listed, add it manually.
3. Return to the main menu.
4. Navigate to **Select URL**.
    - Choose the server URL you want to use.
5. Return to the main menu.

## Step 4: Patch the Game
1. Open the LittleBigPatcher app.
2. Navigate to **Patch Games**.
3. Select the patching options you prefer.
4. Use the **Browse Games for Title ID** option to locate your LittleBigPlanet game.
    - **Note:** If your game is not listed, repeat Step 1. Ensure you follow these instructions carefully:
      - Do **not** press **START DECRYPT (ELF)**, as this option is currently broken and will not work.
      - Instead, press down once to select **[START] START DECRYPT (SELF)**.
5. Navigate to **Patch! (LittleBigPlanet Main Series)** and confirm by selecting **Yes**.
6. Wait for the patching process to complete.

You are now ready to play LittleBigPlanet with your custom server!

# patch.lua
`patch.lua` is licenced with the MIT Licence, so you can edit and change it all you like, refer to [https://github.com/LittleBigPatcherTeam/LittleBigElfPatcherLua](https://github.com/LittleBigPatcherTeam/LittleBigElfPatcherLua) for Licence file
# Instructions

# Credits (If i missed anyone please let me know)
## [VitaShell](https://github.com/TheOfficialFloW/VitaShell)
For help with stuff like text input and being a frontend to install the FAGDec.vpk
## [PSVita RE Tools](https://github.com/TeamFAPS/PSVita-RE-tools)
For a bunch of stuff, such as FAGDec, vita-unmake-self and elf_inject
## [taihen-parser](https://github.com/DaveeFTW/taihen-parser)
For providing information on how to parse the tai config.txt to install repatch and remove Allefresher
## [rePatch reLoaded:](https://github.com/SonicMastr/rePatch-reLoaded)
For the `repatch_ex.skprx` file which the app can install automatically
## [FAGDec](https://github.com/TeamFAPS/PSVita-RE-tools/tree/master/FAGDec/src)
Allowing this to be possible, and hopefully in future will be coded in this app directly during patching process

<div id='lua-credit'/>

## [Lua](https://www.lua.org/ftp/lua-5.4.7.tar.gz)

In no way am i saying i made lua lol, please refer to [lua](https://www.lua.org/license.html) but i did delete some unused files from the tar.gz to save space, and had to rename 2 functions named `main` to `luac_main` and `lua_main`
