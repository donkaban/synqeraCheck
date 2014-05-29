#!/bin/bash
VERSION="2.0 (gcc)" 
ERR_COLOR="\033[31m"
NFO_COLOR="\033[35m"

SCRIPT_NAME=`basename $0`
ARCH=$(uname -i)
NAME=$(lsb_release -si)

function err  {
    echo -e "\n" $ERR_COLOR $1 $2 $3 $4 $5 $6 "\n" ; tput sgr0;
}
function msg  {
    echo -e $NFO_COLOR "---------------------------------------------------------------" ; tput sgr0;
    echo -e $NFO_COLOR $1 $2 $3 $4 $5 $6 ; tput sgr0;
    echo -e $NFO_COLOR "---------------------------------------------------------------" ; tput sgr0;

}

if [ $NAME != "Ubuntu" -a $NAME != "Debian" -a $NAME != "LinuxMint" ]; then
    err "you must have Ubuntu or Mint or Debian distro"
    exit 1
fi
if [ "$(id -u)" != "0" ]; then
    err "need root permissions" "try sudo " $SCRIPT_NAME
    exit 1
fi
 
msg "Install " $ARCH  $NAME " synqera linux toolset version LITE version (for external use)" $VERSION

msg "Add repos :"
    add-apt-repository -y ppa:ubuntu-toolchain-r/test

msg "Update :"
    apt-get update

msg "Install technical prerequisites :"
    apt-get install -y linux-headers-`uname -r`
    apt-get install -y linux-source

msg "Install development stuff :"
    apt-get install -y coreutils 
    apt-get install -y make 

msg "Install GCC 4.8.X :"

    apt-get install -y gcc-4.8
    apt-get install -y g++-4.8
    update-alternatives --remove-all gcc
    update-alternatives --remove-all g++
    update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.8 20
    update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-4.8 20
    update-alternatives --config g++
    update-alternatives --config gcc

msg "Install  OpenGL stuff :"
    apt-get install -y libgles2-mesa-dev 
    apt-get install -y freeglut3-dev 

msg "Clean apt cache"
    apt-get -y autoremove

