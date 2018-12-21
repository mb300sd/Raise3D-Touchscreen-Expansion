#!/bin/bash


USAGE="
Usage: $0 [cmd] [cmdargs]\n
  cmds:\n
    export [gpio]\n
    read [gpio]\n
    write [gpio] [value]\n
    dir [gpio] [in|out]\n
"

if [[ "$1" == "export" && "$2" =~ ^[0-9]+$ ]]; then
   echo $2 > /sys/class/gpio/export
elif [[ "$1" == "read" && "$2" =~ ^[0-9]+$ ]]; then
   cat /sys/class/gpio/gpio$2/value
elif [[ "$1" == "write" && "$2" =~ ^[0-9]+$ && "$3" =~ ^[0-1]$ ]]; then
   echo $3 > /sys/class/gpio/gpio$2/value
elif [[ "$1" == "dir" && "$2" =~ ^[0-9]+$ ]] && [[ "$3" == "in" || "$3" == "out" ]]; then
   echo $3 > /sys/class/gpio/gpio$2/direction
else
   echo -e $USAGE
fi

