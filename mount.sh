#!/bin/bash

# This script mounts the binder filesystem at /dev/binderfs
sudo mkdir -p /dev/binderfs
sudo mount -t binder binder /dev/binderfs

sudo chown root:binderfs /dev/binderfs
sudo chown root:binderfs /dev/binderfs/binder
sudo chmod 750 /dev/binderfs
sudo chmod 660 /dev/binderfs/binder