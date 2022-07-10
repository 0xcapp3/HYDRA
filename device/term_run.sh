#!/bin/sh

echo "[=] Remember always to run with sudo!\r\n"

../../RIOT/dist/tools/tapsetup/tapsetup -d
../../RIOT/dist/tools/tapsetup/tapsetup

ip a a fec0:affe::1/64 dev tapbr0

make -j16 term