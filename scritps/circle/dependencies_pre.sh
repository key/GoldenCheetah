#!/bin/sh

# install depended packages
sudo aptitude update
sudo aptitude install -y libqt4-dev qt4-dev-tools git

# copy build configurations
cp src/gcconfig.pri.in src/gcconfig.pri
cp qwt/qwtconfig.pri.in qwt/qwtconfig.pri
