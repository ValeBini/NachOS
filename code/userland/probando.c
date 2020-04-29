#include "syscall.h"

int main(){
	Write("Ingrese un nombre de maximo 10 char: ", 37, CONSOLE_OUTPUT);
	char name[10];
	int n = Read(name, 10, CONSOLE_INPUT);

	// Create("test.txt");
	// OpenFileId id = Open("test.txt");
	Write(name, n, CONSOLE_OUTPUT);
	Write("\n", 1, CONSOLE_OUTPUT);
	// Close(id);
	Halt();

}