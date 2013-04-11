#!/bin/bash

# Make sure only root can run our script
if [[ $EUID -ne 0 ]]; then
   echo "This script must be run as root" 1>&2
   exit 1
fi

echo 27 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio27/direction

./framboisier-sensor --server_host localhost:5000