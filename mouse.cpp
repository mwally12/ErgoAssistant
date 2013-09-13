#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <pthread.h>

#include <linux/input.h>
#include <fcntl.h>

using namespace std;

//Se definen los archivos en los que se encuentran los eventos del teclado y del mouse
#define KEYFILE "/dev/input/by-path/platform-i8042-serio-0-event-kbd"
#define MOUSEFILE "/dev/input/mice" //alternativa: /dev/input/mouse0

//Funcion del primer thread que detecta los eventos del mouse
void* detectarMouse(void*) {
    //Se definen las variables que guardara informacion importante del archivo del mouse
    int fd_mouse;
    struct input_event ie_mouse;
    //Se abre el archivo que contiene todos los eventos del mouse
    if((fd_mouse = open(MOUSEFILE, O_RDONLY)) == -1) {
        perror("opening mouse");
        exit(EXIT_FAILURE);
    }

    while (read(fd_mouse, &ie_mouse, sizeof(struct input_event))) { 
        printf("time %ld.%06ld\ttype %d\tcode %d\tvalue %d\n", ie_mouse.time.tv_sec, ie_mouse.time.tv_usec, ie_mouse.type, ie_mouse.code, ie_mouse.value);
    }

}

//Funcion del segundo thread que detecta los eventos del teclado
void* detectarTeclado(void*) {
    //Se definen las variables que guardara informacion importante del archivo del teclado
    int fd_teclado;
    struct input_event ie_teclado;
    //Se abre el archivo que contiene todos los eventos del teclado
    if((fd_teclado = open(KEYFILE, O_RDONLY)) == -1) {
        perror("opening keyboard");
        exit(EXIT_FAILURE);
    }
    while (read(fd_teclado, &ie_teclado, sizeof(struct input_event))) { 
        printf("time %ld.%06ld\ttype %d\tcode %d\tvalue %d\n", ie_teclado.time.tv_sec, ie_teclado.time.tv_usec, ie_teclado.type, ie_teclado.code, ie_teclado.value);
    }
}

int main() {
    //Se crean los hilos
    pthread_t t1, t2;
    //Se llaman a los hilos
    pthread_create(&t1, NULL, &detectarMouse, NULL);
    pthread_create(&t2, NULL, &detectarTeclado, NULL);
    //Se espera a que los hilos terminen
    pthread_join( t1, NULL);
    pthread_join( t2, NULL);
    return 0;
}