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

# FAGDec usage for DECRYPT(SELF)
[Follow this guide](https://littlebigpatcherteam.github.io/2025/03/14/FAGDec-guide.html)

# patch.lua
`patch.lua` is licenced with the MIT Licence, so you can edit and change it all you like, refer to [https://github.com/LittleBigPatcherTeam/LittleBigElfPatcherLua](https://github.com/LittleBigPatcherTeam/LittleBigElfPatcherLua) for Licence file

# README TODO
I need to do this
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
## [lbpv_free_dlc.py](https://gist.github.com/Zhaxxy/8bc5994881678ef15739fd6d51465b3b)
For making the orginal dlc remover and suggesting being add to this patcher (it is not piracy based)

<div id='lua-credit'/>

## [Lua](https://www.lua.org/ftp/lua-5.4.7.tar.gz)

In no way am i saying i made lua lol, please refer to [lua](https://www.lua.org/license.html) but i did delete some unused files from the tar.gz to save space, and had to rename 2 functions named `main` to `luac_main` and `lua_main`
