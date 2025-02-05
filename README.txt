
Pasos para preparar y ejecutar el programa:

as -g chain2int.s -o chain.o
gcc -Wall -g -c finalchar.c -o finalchar.o
gcc -g chain.o finalchar.o -o final -lwiringPi -lpthread

sudo ./final






Configurar comuunicaCION

sudo raspi-config
reboot


Configurar puerto serie

sudo stty -F /dev/serial0 -icrnl -onlcr
sudo stty -F /dev/serial0 raw -icanon -isig -iexten -echo
