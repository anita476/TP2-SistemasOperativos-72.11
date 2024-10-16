// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <stdint.h>
#include <lib.h>
#include <videoDriver.h>
#include <time.h>
#include <exceptions.h>
#include <syscallHandler.h>

#define ZERO_EXCEPTION_CODE 0
#define WRONG_OPCODE_EXC_CODE 6

static void zero_division();
static void wrong_opcode();

extern uint64_t resetMain();

static char * registerOrder[18] = {"RIP", "RAX", "RBX", "RCX", "RDX", "RSI", "RDI", "RBP", "RSP", "R8", "R9", "R10", "R11", "R12", "R13", "R14", "R15", "RFLAGS"};
static char * errorMessage = "Program failed due to unhandled exception:\n";
static char * waiting = "Restarting...";

void exceptionDispatcher(int exception, uint64_t registers[17]) {
	wait(18*3);
	clearScreen();
	print(errorMessage);
	char buffer[10];
	intToStr(exception, buffer, 10);
	print("Code: ");
	print(buffer);
	print("\n");
	if (exception == ZERO_EXCEPTION_CODE) {
		zero_division();
	}
	else if (exception == WRONG_OPCODE_EXC_CODE) {
		wrong_opcode();
	}
	else {
		print("Unknown");
		print("\n");
	}
	for (int i = 0; i < 18; i++) {
		print(registerOrder[i]);
		print(": ");
		intToStr(registers[i], buffer, 16);
		print(buffer);
		if (i != 17) print(", ");
	}
	timer_wait(18 * 5); // Time to see the registers
	print("\n");
	print(waiting);
	timer_wait(18 * 2);
	clearScreen();
	setCursorLine(0);
	resetMain();
}

static void zero_division() {
	print("Zero division");
	print("\n");
}

static void wrong_opcode() {
	print("Wrong opcode");
	print("\n");
}