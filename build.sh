set -e

mkdir -p build
cd build
cmake ..
make
ftp -u ftp://192.168.1.176:1337/ux0:/app/LBPC59548/eboot.bin 'LittleBigPatcher_for_Custom_Servers_Vita_Homebrew.self'

