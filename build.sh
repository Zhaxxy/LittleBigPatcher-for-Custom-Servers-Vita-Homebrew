set -e

mkdir -p build
cd build
cmake ..
make
ftp -u ftp://192.168.1.176:1337/ux0:/vpk/build/out.vpk 'LittleBigPatcher_for_Custom_Servers_Vita_Homebrew.vpk'

