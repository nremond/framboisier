#!/bin/sh

echo 27 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio27/direction

./framboisier-sensor --server_host localhost:5000