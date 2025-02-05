#ifndef EASY_PIO_H
#define EASY_PIO_H

//include statements
#include <sys/mman.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

//Constantes

//GPIO FSEL Types
#define INPUT 0
#define OUTPUT 1
#define ALT0 4
#define ALT1 5
#define ALT2 6
#define ALT3 7
#define ALT4 3
#define ALT5 2

//Memory map

#define BCM2835_PERI_BASE 0x3F000000
#define GPIO_BASE (BCM2835_PERI_BASE + 0x200000)
#define BLOCK_SIZE (4*1024)

//Punteros a memoria mapeada cuando pioInit() es llamado
volatile unsigned int *gpio; //puntero a la base de gpio

//GPIO REGISTROS

#define GPFSEL ((volatile unsigned int *) (gpio + 0))
#define GPSET ((volatile unsigned int *) (gpio + 7))
#define GPCLR ((volatile unsigned int *) (gpio + 10))
#define GPLEV ((volatile unsigned int *) (gpio + 13))

//Funciones Generales

void pioInit(){
	int mem_fd;
	void *reg_map;
	
	mem_fd = open("/dev/mem", O_RDWR|O_SYNC); //abre ek dispositivo
	reg_map = mmap(
			NULL,
			BLOCK_SIZE,
			PROT_READ|PROT_WRITE,
			MAP_SHARED,
			mem_fd,
			GPIO_BASE);
	
	gpio = (volatile unsigned *)reg_map;
	close(mem_fd);
}

//Funciones GPIO
	
void pinMode(int pin, int function){
	int reg = pin/10;
	int offset = (pin%10)*3;
	
	GPFSEL[reg] &= ~((0b111 & ~function) << offset);
	GPFSEL[reg] |= ((0b111 & function) << offset);
	
}
	
void digitalWrite(int pin, int val){
	int reg = pin/32;
	int offset = pin%32;
	
	if (val)
		GPSET[reg] = 1<<offset;
	else
		GPCLR[reg] = 1<<offset;
}

int digitalRead(int pin){
	int reg = pin/32;
	int offset = pin%32;
	
	return (GPLEV[reg] >> offset) & 0x00000001;
}
	
#endif
