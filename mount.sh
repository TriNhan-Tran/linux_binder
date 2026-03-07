#!/bin/bash

set -euo pipefail

# This script mounts binderfs at /dev/binderfs and prepares binder device perms.
if [[ ! -d /dev/binderfs ]]; then
	sudo mkdir -p /dev/binderfs
fi

if ! mountpoint -q /dev/binderfs; then
	sudo mount -t binder binder /dev/binderfs -o stats=global
fi

if [[ -e /dev/binderfs/binder ]]; then
	sudo chmod 666 /dev/binderfs/binder
fi
