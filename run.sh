#!/bin/bash
if [[ "$OSTYPE" =~ "darwin" ]]; then
    if [[ "$1" = "-d" ]]; then
        qemu-system-x86_64 -s -S -d int -D log.txt -hda Image/x64BareBonesImage.qcow2 -m 512 -audiodev coreaudio,id=audio0 -machine pcspk-audiodev=audio0
    else
        qemu-system-x86_64 -d int -D log.txt -hda Image/x64BareBonesImage.qcow2 -m 512 -audiodev coreaudio,id=audio0 -machine pcspk-audiodev=audio0
    fi
else
    if [[ "$1" = "-d" ]]; then
        qemu-system-x86_64 -s -S -hda Image/x64BareBonesImage.qcow2 -m 512 
    else
        qemu-system-x86_64 -hda Image/x64BareBonesImage.qcow2 -m 512 -audiodev pa,id=audio0 -machine pcspk-audiodev=audio0
    fi
fi
