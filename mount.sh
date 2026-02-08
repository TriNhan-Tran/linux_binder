#!/bin/bash

# This script mounts the binder filesystem at /dev/binderfs
sudo mkdir -p /dev/binderfs
sudo mount -t binder binder /dev/binderfs