MM=BUDDY
all:  bootloader kernel userland image
	@echo "Building with MEMORY MANAGER=$(MM)"

bootloader:
	cd Bootloader; make all

kernel:
	cd Kernel; make all MM=-D$(MM)

userland:
	cd Userland; make all

image: kernel bootloader userland
	cd Image; make all

clean:
	cd Bootloader; make clean
	cd Image; make clean
	cd Kernel; make clean
	cd Userland; make clean

pvs: clean
	pvs-studio-analyzer trace -- make all
	pvs-studio-analyzer analyze
	plog-converter -a '64:1,2,3;GA:1,2,3;OP:1,2,3' -t tasklist -o report.tasks PVS-Studio.log

.PHONY: bootloader image collections kernel userland all clean pvs
