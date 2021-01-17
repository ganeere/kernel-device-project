#!/bin/bash



make
insmod scull.ko
mknod /dev/vault c 247 0
mknod /dev/vault1 c 247 1
gcc app_change_key.c -o app_change_key
gcc app_clear_content.c -o app_clear_content
#./app_change_key
#./app_clear_content