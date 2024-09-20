#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include <videoDriver.h>
#include <idtLoader.h>
#include <sound.h>
#include <time.h>
#include <syscallHandler.h>
#include <betterCallcaOS.h>
#include <test_util.h>
#include <syscall.h>
#include <memoryManagement.h>
#include <test_mm.h>

extern void printtest();

extern uint64_t resetMain();
extern uint64_t show_registers[17];

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

// If there is a problem, change the address
static void * const sampleCodeModuleAddress = (void*)0x400000;
//static void * const sampleDataModuleAddress = (void*)0x500000; /* not used */
static void *const startHeapAddres = (void *) 0xF00000;
static void *const endHeapAddres = (void *) 0x2000000;

typedef int (*EntryPoint)();

void clearBSS(void * bssAddress, uint64_t bssSize) {
	memset(bssAddress, 0, bssSize);
}

void * getStackBase() {
	return (void*)(
		(uint64_t)&endOfKernel
		+ PageSize * 8				//The size of the stack itself, 32KiB
		- sizeof(uint64_t)			//Begin at the top of the stack
	);
}

void * initializeKernelBinary() {
	void * moduleAddresses[] = {sampleCodeModuleAddress };
	loadModules(&endOfKernelBinary, moduleAddresses);
	clearBSS(&bss, &endOfKernel - &bss);
	return getStackBase();
}

void welcomeSequence() {
	for (int i = 0; i < 4; i++) scale_down();
	scale_up();
	setCursorLine(8);
	print("                        Welcome to caOS!\n");
	putSaul(450,300,125,125);
	playBetterCallSaul();
	setCursorLine(23);
	print(" Starting...");
	wait(1000);
	scale_down();
	clear_screen();
}

int main() {
	load_IDT();
	memoryInit(startHeapAddres, (size_t) (endHeapAddres - startHeapAddres));
	wait(2000);
	char *juan[] = {"1000"}; 
	uint64_t result = test_mm(1, juan);
	if (result == -1) {
		print("Memory test failed\n");
	} else {
		print("Memory test passed\n");
	}
	// welcomeSequence();
	((EntryPoint)sampleCodeModuleAddress)();
	return 0;
}
