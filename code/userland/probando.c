#include "syscall.h"

int main(){
	Write("Ingrese un nombre de maximo 10 char: ", 37, 1);
	char name[10];
	int n = Read(name, 10, 0);

	// Create("test.txt");
	// OpenFileId id = Open("test.txt");
	Write(name, n, 1);
	// Close(id);
	Halt();
}