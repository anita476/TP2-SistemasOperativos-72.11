// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

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
#include <processes.h>
#include <scheduler.h>
#include <test_util.h>
#include <test_processes.h>
#include <test_prio.h>

extern void haltcpu();
extern void printtest();
extern void _cli();
extern void _sti();

extern uint64_t resetMain();
extern uint64_t show_registers[17];

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

static void * const sampleCodeModuleAddress = (void*)0x400000;
//static void * const sampleDataModuleAddress = (void*)0x500000; /* goodbye */
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
void endlessLoop(int argc, char *argv[]) {
    while (1)
        ;
}

int main() {
	_cli();
	load_IDT();
	init_memory_manager(startHeapAddres,(size_t) (endHeapAddres - startHeapAddres));
	_sti();
	//welcomeSequence();
	const char * const argv[1] = {"Hello"};
	createProcessInfo pci = {.name = "process",
                             .start = (ProcessStart) endlessLoop,
                             .fg_flag = 1,
                             .priority = FOREGROUND,
                             .argc = 1,
                             .argv = argv};
	pid pid = createProcess(&pci);

	if(pid >= 0){
		print("Succesful process creation\n");
		int i = setPriority(pid,7);
		if(i !=0){
			print("Set priority\n");
		}



	}
	else{
		print("Unsuccesful process creation\n");
	}
	if(sendToBackground(pid)){
		print("Process sent to background\n");
		if(isForeground(pid)){
			print("Process didnt remain in background\n");
		}
	}
	if(bringToForeground(pid)){
		print("Process brought to foregorund\n");
		if(!isForeground(pid)){
			print("Process is not in foreground\n");
		}
	} 
	//char * argv2[1] = {{"5"}};
	//int n = test_processes(1, argv2);
	int n = 0;
	test_prio();

	if(n != 0 ){
		print("Problem with test\n");
	}
	 int k = kill(pid);

	if(k!=0){
		print("Unsuccesful process kill\n");
	}
	else {
		print("Succesfull process kill\n");
	}
 


	((EntryPoint)sampleCodeModuleAddress)();
	return 0;
}
