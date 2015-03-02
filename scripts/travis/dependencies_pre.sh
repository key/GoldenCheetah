#!/bin/sh

# install depended packages
sudo apt-get update
sudo apt-get install -y libboost-dev libvlc-dev libqt4-dev qt4-dev-tools git build-essential

# copy build configurations
# FIXME anyone change paths in gcconfig.pri.in
# cp src/gcconfig.pri.in src/gcconfig.pri
cp scripts/travis/gcconfig.pri src/gcconfig.pri
cp qwt/qwtconfig.pri.in qwt/qwtconfig.pri
