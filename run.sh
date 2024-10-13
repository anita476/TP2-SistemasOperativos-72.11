#!/bin/bash
if [[ "$OSTYPE" =~ "darwin" ]]; then
    qemu-system-x86_64 -D ./log.txt -d cpu_reset -hda Image/x64BareBonesImage.qcow2 -m 512 -audiodev coreaudio,id=audio0 -machine pcspk-audiodev=audio0
else
    if [[ "$1" = "-d" ]]; then
        qemu-system-x86_64 -s -S -hda Image/x64BareBonesImage.qcow2 -m 512 


    else
        qemu-system-x86_64 -hda Image/x64BareBonesImage.qcow2 -m 512 -audiodev pa,id=audio0 -machine pcspk-audiodev=audio0
    fi
fi


#-audiodev pa,id=audio0 -machine pcspk-audiodev=audio0