#include <stdio.h>
#include <termios.h>
#include <string.h>
#include <wiringPiI2C.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

#include <fcntl.h>
#include <termios.h>
#include "EasyPIO.h"

#define FD_STDIN 0
#define MAX_INTENTOS 3
#define TAM_PASSWORD 5
#define numLED 8
#define ARDUINO_ADDRESS 0x08 //Direccion I2C del Arduino


int salidas[numLED] = {26,21, 20, 16, 12, 25, 24, 23};
int vectorVelocidades[8] = {5, 5, 5, 5, 5, 5, 5, 5};
char simuLeds[numLED];
int remoto;

void inicializarLeds();
void escribirLed();
void apagarLeds();
void imprimirLeds();

extern int chain2int(char*);



int fd;
int serial_port;
int remoto = 0;

char var = 0;
int vel_act = 0;

void autoFantastico(void);
void laApilada(void);
void contadorBinario(void);
void laPiramide(void);
void ejecutarSecuenciaPorTabla(char *, int, int);


void encenderLeds(void);
void apagarLeds();
int read_adc_values();

int menu(void);
int contrasena(void);
void habilitarModoNoBloqueante(struct termios *);
void restaurarModoBloqueante(const struct termios *);
int ajustarVelocidad(int);

void setearVelocidades();
void mostrarVelocidades();
int configurarSerial(void);

// Declaración de un mutex global
pthread_mutex_t lock;

void* thread_func(void* arg);
void espera(int max);

pthread_t thread1;



char elChoque[]={
            0b10000001,
            0b01000010,
            0b00100100,
            0b00011000,
            0b00011000,
            0b00100100,
            0b01000010,
            0b10000001,
        };

    
char laCarrera[]={
            0b10000000,
            0b01000000,
            0b00100000,
            0b00010000,
            0b10001000,
            0b00100100,
            0b00001010,
            0b00000011,
        };

 
char elRebote[]={
            0b00010001,
            0b00010010,
            0b00010100,
            0b00011000,
            0b00101000,
            0b01001000,
            0b10001000,
            0b01001000,
            0b00101000,
            0b00011000,
            0b00010100,
            0b00010010,
            0b00010001,
        };

    	
	
char elGusano[]={
            0b10000000,
            0b11000000,
            0b11100000,
            0b11110000,
            0b01111000,
            0b00111100,
            0b00011110,
            0b00001111,
            0b00000111,
            0b00000011,
            0b00000001,
            0b00000000,
        };

    
    











int main()
{
	
	/*pthread_t thread1;*/
	
	// Inicialización del mutex
	pthread_mutex_init(&lock, NULL);



	// Inicializacion de I2C
	fd = wiringPiI2CSetup(ARDUINO_ADDRESS);
	if (fd == -1){
		return -1;
	}

	inicializarLeds();


	printf("\033[1;36m");
	printf(
"  _____                           _  \n"                
" |  __ \\                         | |\n"                 
" | |__) | __ ___  _   _  ___  ___| |_ ___\n"            
" |  ___/ '__/ _ \\| | | |/ _ \\/ __| __/ _ \\\n"           
" | |   | | | (_) | |_| |  __/ (__| || (_) |\n"          
" |_|   |_|  \\___/ \\__, |\\___|\\___|\\__\\___/\n"           
"  ______ _         __/ |   _______ _____ _____ _____\n" 
" |  ____(_)       |___/ | |__   __|  __ \\_   _|_   _|\n"
" | |__   _ _ __   __ _| |    | |  | |  | || |   | |\n"  
" |  __| | | '_ \\ / _` | |    | |  | |  | || |   | |\n"  
" | |    | | | | | (_| | |    | |  | |__| || |_ _| |_\n" 
" |_|    |_|_| |_|\\__,_|_|    |_|  |_____/_____|_____|\n\n");

	printf("\033[1;33m");
	
	printf("Robertson, Avedano, Musso, Acarapi, Palacios\n\n");
	printf("\033[1;37m");
	
	int salir = 1;

	
	if (contrasena() == -1)
		return 0;
	// Limpia la terminal
	system("clear");
	
	do
	{
		salir = menu();
	} while (salir);
	
	pthread_mutex_destroy(&lock);
	
	return 0;
}





int menu()
{
	int opcion;
	do {
		printf("\033[1;37m");
		printf("\n\tMENU PRINCIPAL\n\n");
		printf("\033[1;31m");
		printf("*  0 _____ Salir\n");
		printf("\033[1;36m");
		printf("*  1 _____ El Auto Fantastico\n");
		printf("*  2 _____ El Choque\n");
		printf("*  3 _____ La Apilada\n");
		printf("*  4 _____ La Carrera\n");
		printf("*  5 _____ Contador Binario\n");
		printf("*  6 _____ El Rebote\n");
		printf("*  7 _____ El Gusano\n");
		printf("*  8 _____ La Piramide\n");
		printf("\033[1;32m");
		printf("*  9 _____ Setear velocidades\n");
		printf("* 10 _____ Mostrar velocidades\n");
		printf("\033[1;34m");
		if(remoto == 0){
			printf("* 11 _____ Activar modo Remoto\n\n");
		}else{
			printf("* 11 _____ Desactivar modo Remoto\n\n");
		}
		printf("\033[1;37m");
		
		printf("Escriba la opcion: ");
		scanf("%d", &opcion);
		
		if (opcion < 0 || opcion > 11)
		{	
			system("clear");
			printf("Opcion invalida, intentelo de nuevo.\n");
		}
	} while (opcion > 11 || opcion < 0);
	
	// Limpia la terminal
	system("clear");
	
	if(opcion > 0 && opcion < 9){
		if (pthread_create(&thread1, NULL, thread_func, NULL) != 0) {
			perror("Error al crear el hilo 1");
			exit(EXIT_FAILURE);
		}
		
		vel_act = (opcion-1);
	}
	
	switch (opcion){
		case 0:
			printf("Finalizando...\n\n");
			return 0;
		case 1:
			printf("Ejecutando Auto Fantastico, para salir presione 'Enter':\n\n");
			autoFantastico();
			break;
		case 2:
			printf("Ejecutando El Choque, para salir presione 'Enter':\n\n");
			ejecutarSecuenciaPorTabla(elChoque, sizeof(elChoque) / sizeof(char), (opcion-1));
			break;
		case 3:
			printf("Ejecutando La Apilada, para salir presione 'Enter':\n\n");
			laApilada();
			break;
		case 4:
			printf("Ejecutando La Carrera, para salir presione 'Enter':\n\n");
			ejecutarSecuenciaPorTabla(laCarrera, sizeof(laCarrera) / sizeof(char), (opcion-1));
			break;
		case 5:
			printf("Ejecutando Contador Binario, para salir presione 'Enter':\n\n");
			contadorBinario();
			break;
		case 6:
			printf("Ejecutando Rebote, para salir presione 'Enter':\n\n");
			ejecutarSecuenciaPorTabla(elRebote, sizeof(elRebote) / sizeof(char), (opcion-1));
			break;
		case 7:
			printf("Ejecutando Gusano, para salir presione 'Enter':\n\n");
			ejecutarSecuenciaPorTabla(elGusano, sizeof(elGusano) / sizeof(char), (opcion-1));
			break;
		case 8:
			printf("Ejecutando Piramide, para salir presione 'Enter':\n\n");
			laPiramide();
			break;
			
		case 9:
			setearVelocidades();
			break;
			
		case 10:
			mostrarVelocidades();
			break;
		case 11:
		if(remoto == 0){
		printf("Activando modo remoto...\n");
		espera(1);
		if (configurarSerial() == 0){
                remoto = 1;
		printf("\033[1;30m");
		printf("\033[103m");
                printf("Modo remoto activado\n");
		printf("\033[40m");
		printf("\033[1;37m");
		} else {
                printf("Error al abrir o configurar puerto serie\n");
		}
		} else {   
		printf("Desactivando modo remoto...\n");
		espera(1);
		close(serial_port);
		remoto = 0;
		printf("\033[1;30m");
		printf("\033[103m");
                printf("Modo remoto desactivado\n");
		printf("\033[40m");
		printf("\033[1;37m");
        }
	
	

        //printf("\nPresione la tecla Enter para volver al menu");
        // Esperar a que el presione una tecla
        //char cerrar = 0;
        //scanf("%c%c", &cerrar, &cerrar);
	

		break;
		default:
			printf("Secuencia no implementada.\n");
			break;
	}
	
	
	if(opcion > 0 && opcion < 11){
		
		if(var == 1){
			pthread_join(thread1, NULL);
			var = 0;
			apagarLeds();
		}
	system("clear");
	}
	
	
	
	return 1; // Permanece en el bucle principal
}

void setearVelocidades()
{
	int selec;
	
	printf("Desea elegir velocidad con potenciometro o por terminal?\n\n");
	printf("1. Potenciometro\n");
	printf("2. Teclado\n");
	
	while(selec != 1 && selec != 2)
	{
		scanf("%d", &selec);
	}
	
	if(selec == 1)
	{
		char cerrar = 0;
		int valor = 0;
		
		struct termios old_attr;
		habilitarModoNoBloqueante(&old_attr);
		
		
		for (int i = 0; i < 8; i++)
		{			
			while (cerrar != 10)							//10 equivale al Enter, por lo que para seleccionar la vel, se debe apretar esta tecla
			{
				valor = read_adc_values();
				if (valor <= 1000){
					valor /= 100;
				}else{
					valor = 10;
				}
				valor = (valor == 0 ? 1: valor);
				printf("Ajustar Potenciometro y presionar la tecla ENTER para seleccionar: \n");
				for (int j = 0; j < i; j++){
					printf("Velocidad %d: %d\n", j+1, vectorVelocidades[j]);
				}
				printf("Velocidad %d: %d\n", i+1, valor);
				
				//cerrar = getchar(); 
				read(FD_STDIN, &cerrar, 1);
				// printf("cerrar: %d\n", cerrar);
				usleep(1e5);
				system("clear");
			}
			cerrar = 0;
			vectorVelocidades[i] = valor;	
		}
		
		
		printf("\n\nVelocidades seteadas:\n\n");
		for (int i = 0; i < 8; i++)
		{
			printf("Velocidad %d: %d\n", i+1, vectorVelocidades[i]);
		}
		
		
		tcflush(FD_STDIN, TCIFLUSH);
		restaurarModoBloqueante(&old_attr);
		
		
	}else{
		
		printf("Setear velocidades (del 1 al 10):\n");
		for (int i = 0; i < 8; i++)
		{
			printf("Velocidad %d: ", i+1);
			do{
				scanf("%d", &vectorVelocidades[i]);
			} while(vectorVelocidades[i] < 1 || vectorVelocidades[i] > 10);
		}
		printf("\n\nVelocidades seteadas:\n\n");
		for (int i = 0; i < 8; i++)
		{
			printf("Velocidad %d: %d\n", i+1, vectorVelocidades[i]);
		}
		
	}
}





int contrasena()
{
	const char password_correcta[TAM_PASSWORD + 1] = "12345";
	char password_usuario[TAM_PASSWORD + 1];
	
	int intentos = 0;
	int i;
	
	struct termios t_old, t_new;
	tcgetattr(FD_STDIN, &t_old); // lee atributos del teclado
	t_new = t_old;
	t_new.c_lflag &= ~(ECHO | ICANON);    // anula entrada canÃ³nica y eco
	tcsetattr(FD_STDIN, TCSANOW, &t_new); // actualiza con los valores nuevos de la config. TCSANOW = activar la modificaciÃ³n inmediatamente
	
	printf("Bienvenido al programa\n");
	
	while (1)
	{
		printf("Ingrese su password de %d digitos (%d intentos): ", TAM_PASSWORD, 3 - intentos);
		
		printf("\033[0;35m");
		for (i = 0; i < TAM_PASSWORD; i++)
		{
			password_usuario[i] = getchar();
			printf("*");
		}
		
		printf("\033[0;0m");
		password_usuario[TAM_PASSWORD] = '\0';
		printf("\n");
		
		if (strcmp(password_usuario, password_correcta) == 0)
		{
			printf("Bienvenido al Sistema\n");
			// return 0;
			break;
		}
		else
		{	
			printf("\033[0;31m");
			printf("Password no valida\n");
			printf("\033[1;37m");
			intentos++;
		}
		
		if (intentos == MAX_INTENTOS)
		{	
			printf("\033[1;31m");
			printf("Numero maximo de intentos fallidos alcanzado.\n");
			printf("\033[1;37m");
			tcsetattr(FD_STDIN, TCSANOW, &t_old); // actualiza con los valores previos
			return -1;
		}
	}
	
	tcsetattr(FD_STDIN, TCSANOW, &t_old); // actualiza con los valores previos
	return 0;
}




// Habilita el modo no bloqueante y desactiva el eco
void habilitarModoNoBloqueante(struct termios *old_attr)
{
	struct termios new_attr;
	tcgetattr(FD_STDIN, old_attr); // Guarda la configuraciÃ³n actual
	new_attr = *old_attr;
	new_attr.c_cc[VMIN] = 0;                 // No espera caracteres
	new_attr.c_cc[VTIME] = 0;                // Sin temporizador
	new_attr.c_lflag &= ~(ECHO | ICANON);    // Desactiva eco y modo canÃ³nico
	tcsetattr(FD_STDIN, TCSANOW, &new_attr); // Aplica la nueva configuraciÃ³n
}




// Restaura la configuraciÃ³n original de la terminal
void restaurarModoBloqueante(const struct termios *old_attr)
{
	tcsetattr(FD_STDIN, TCSANOW, old_attr); // Restaura configuraciÃ³n previa
}


void mostrarVelocidades() {
	printf("\nVelocidades guardadas:\n\n");
	for (int i = 0; i < 8; i++)
	{
		printf("Velocidad %d: %d\n", i+1, vectorVelocidades[i]);
	}

	printf("\nPresione la tecla Enter para volver al menu: ");
	// Esperar a que el presione una tecla
	char cerrar = 0;
	scanf("%c%c", &cerrar, &cerrar);
}



int ajustarVelocidad(int indice)
{
	char tecla[3];
	if (read(FD_STDIN, tecla, 3) > 0)
	{
		if (tecla[0] == 10 || tecla[1] == 10 || tecla[3] == 10)
		{
			return 1;
		}
		if (tecla[0] == '\033' && tecla[1] == '[')
		{ // Codigo ANSI
			if (tecla[2] == 'A')
			{ // Flecha hacia arriba
				if (vectorVelocidades[indice] < 10)
				{
					(vectorVelocidades[indice])++;
					//vectorVelocidades[indice] = *velocidad; // Actualiza en el vector
					printf("Velocidad aumentada: %d\n", vectorVelocidades[indice]);
				}
			}
			else if (tecla[2] == 'B')
			{ // Flecha hacia abajo
				if (vectorVelocidades[indice] > 1)
				{
					(vectorVelocidades[indice])--;
					//vectorVelocidades[indice] = *velocidad; // Actualiza en el vector
					printf("Velocidad reducida: %d\n", vectorVelocidades[indice]);
				}
			}
		}
	}
	return 0;
	// Limpia el buffer de entrada para evitar residuos
	//tcflush(FD_STDIN, TCIFLUSH);							//comentado para mejorar la respuesta
}



void inicializarLeds() {

    pioInit();

    for (int i = 0; i < numLED; i++)
    {
        pinMode(salidas[i], OUTPUT);
        escribirLed(i, 0);
    }
}




void escribirLed(int indice, int estado) {
    digitalWrite(salidas[indice], estado);
    simuLeds[indice] = estado;
}





void apagarLeds()
{
    for (int i = 0; i < numLED; i++)
    {
        escribirLed(i, 0);
        
    }
	imprimirLeds();
}





void imprimirLeds() {
    unsigned char valorEntero = chain2int(simuLeds);
    
	/*	unsigned char valorEntero = 0;
	for (int i = 0; i < numLED; ++i) {
		valorEntero |= simuLeds[i] << i;
	}					*/

    for (int i = 0; i < numLED; i++){
        printf("%d", simuLeds[i]);
    }
    printf("      %d", valorEntero);
    printf("\n");

    // Si el modo remoto esta activo
    if (remoto == 1){
		write(serial_port, &valorEntero, 1);
    }
}



void autoFantastico()
{
    while (var != 1)
    {
        for (int i = 0; i < numLED; i++)
        {
            escribirLed(i, 1);
            imprimirLeds();
            espera(vectorVelocidades[vel_act]);
            escribirLed(i, 0);
	    if (var == 1) break;
        }

        for (int i = (numLED - 1); i >= 0; i--)
        {
           if (var == 1) break;
            escribirLed(i, 1);
            imprimirLeds();
            espera(vectorVelocidades[vel_act]);
            escribirLed(i, 0);
	    
        }
    }
}



void laApilada(){

	while (var != 1)
    {
        for (int i = 0; i < numLED; i++)
        {
            escribirLed(i, 0);
        }
	imprimirLeds();
	
        for(int k = numLED; k>0 ; k--){
            for(int j = 0; j<k; j++){
                escribirLed(j, 1);
                j>0? escribirLed(j-1, 0) : NULL;
		//if (j > 0) {escribirLed(j-1, 0);}
		imprimirLeds();
                espera(vectorVelocidades[vel_act]);
		        if (var == 1) return;

            }
            escribirLed(k-1, 0);
            imprimirLeds();
            espera(vectorVelocidades[vel_act]);
		    if (var == 1) break;
            escribirLed(k-1, 1);
            imprimirLeds();
        }
    }	
}





void contadorBinario()
{
    int maxContador = (1 << numLED) - 1;
    int contador = 0;


    while (var != 1)
    {

        for (int i = 0; i < numLED; i++)
        {
            int estado = (contador >> i) & 1;
            escribirLed(i, estado);
        }

        imprimirLeds();
        
        contador = (contador + 1) & maxContador;
        espera(vectorVelocidades[vel_act]);
		if (var == 1) break;
    }
}




void laPiramide()
{
	int escalon = 0;
	
	while (var != 1) {

		for (int i = 0; i < (numLED / 2) - escalon; ++i) {
			
			
			escribirLed(i, 1);
			escribirLed(numLED-i-1, 1);
			
			// Apagar salidas de iteracion anterior
			if (i) {
				escribirLed(i-1, 0);
				escribirLed(numLED-i, 0);
			}
			
			imprimirLeds();
			
			espera(vectorVelocidades[vel_act]);
			if (var == 1) break;
		}

		escalon++;

		if (escalon == numLED / 2) {
			escalon = 0;
			apagarLeds();
			
			
			imprimirLeds();
			
			espera(vectorVelocidades[vel_act]);
		}
	}


}

void ejecutarSecuenciaPorTabla(char *tabla, int nPasos, int indice)
{
    while (var != 1)
    {
        for (int paso = 0; paso < nPasos; paso++)
        {
            int estado = tabla[paso];
            for (int i = 0; i < numLED; i++)
            {
                int estado_led = (estado >> i) & 1;
                escribirLed(i, estado_led);
            }
            imprimirLeds();
            espera(vectorVelocidades[indice]);
		    if (var == 1) break;
        }
    }
}




	
	
//Funcion para leer los datos del adc desde el arduino
int read_adc_values(){
	int value = wiringPiI2CReadReg16(fd,0);
	return value;
}
	
	
	
	
int configurarSerial(){
	
	serial_port = open("/dev/serial0", O_RDWR);
	
	if (serial_port < 0) {
		perror("Error al abrir el puerto serie");
		return 1;
	}
	
	struct termios tty;
	
	if (tcgetattr(serial_port, &tty) != 0) {
		perror("Error al obtener atributos de terminal");
		return 1;
	}
	
	// Configuracion de parametros de comunicacion
	tty.c_cflag &= ~PARENB; // Sin paridad
	tty.c_cflag &= ~CSTOPB; // 1 bit de parada
	tty.c_cflag &= ~CSIZE; 
	tty.c_cflag |= CS8;     // 8 bits de datos
	tty.c_cflag |= CREAD | CLOCAL;  // Activar receptor, ignorar senales de control
	
	// Configuracion de velocidad en baudios (baud rate)
	cfsetispeed(&tty, B9600);
	cfsetospeed(&tty, B9600);
	
	// Aplicar configuracion de la terminal
	tcsetattr(serial_port, TCSANOW, &tty);
	
	return 0;
}
	
	
	
	
void* thread_func(void* arg) {
	
	struct termios old_attr;
	habilitarModoNoBloqueante(&old_attr);
	
	while(var!=1){
		var = ajustarVelocidad(vel_act);
	}
	
	tcflush(FD_STDIN, TCIFLUSH);
	restaurarModoBloqueante(&old_attr);
	
	pthread_exit(NULL);
}	
	
	
	
	
void espera(int max){
	int cont = 0;
	max = abs(max - 11);
	while(var!=1 && cont <= (max * 10)){
		usleep(10000);
		cont++;
	}
}
