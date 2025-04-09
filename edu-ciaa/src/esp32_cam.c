/*==================[inlcusiones]============================================*/

#include <string.h>   // Biblioteca de manejo de Strings
#include <stdio.h>
#include <stdlib.h>
#include "esp32_cam.h"

/*==================[definiciones y macros]==================================*/

// UART list:
//  - UART_USB  (Debug con ciaa-pc)
//  - UART_232  (ESP32 - Ciaa)

#define UART_DEBUG                 UART_USB
#define UART_ESP32                 UART_232
#define UARTS_BAUD_RATE            115200
#define ESP32_RX_BUFF_SIZE         1024  // 1mb


/*==================[definiciones de datos internos]=========================*/

CONSOLE_PRINT_ENABLE
DEBUG_PRINT_ENABLE

/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/

/*==================[declaraciones de funciones externas]====================*/

// ESP32 Rx Buffer
char espResponseBuffer[ ESP32_RX_BUFF_SIZE ];
uint32_t espResponseBufferSize = ESP32_RX_BUFF_SIZE;

/*==================[funcion principal]======================================*/

// FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE ENCENDIDO O RESET.
// Establece todos los puertos necesarios para comunicacion con ESP32-CAM.

uint32_t BAUD = UARTS_BAUD_RATE;

void vaciarBuffer( void){   //Vacia el buffer descartando mensajes..
   char c;
   while (uartRxReady( UART_232 ) ) {
       if (uartReadByte(UART_232, &c)){
         if (c == '\n' || c == '\r'){
            break;
         }
       }
   }
}
/*
void stopProgramError( void ){
   // Si hay un error grave me quedo en un bucle infinito en modo bajo consumo
   while( TRUE ){
      sleepUntilNextInterrupt();
   }
}
*/

void esp32CleanRxBuffer( void ){
   espResponseBufferSize = ESP32_RX_BUFF_SIZE;
   memset( espResponseBuffer, 0, espResponseBufferSize );
} 

void reciveMessage( char *cadena){   //Recive un puntero a una cadena, para guardar la lectura completa del buffer. 
   int index = 0;
   char c;
   esp32CleanRxBuffer();                     //limpiamos el buffer antes de usarlo nuevamente.
   
   while (index < (ESP32_RX_BUFF_SIZE - 1) && uartRxReady( UART_232 ) ) {
       if (uartReadByte(UART_232, &c)){
         if (c == '\n' || c == '\r'){
            espResponseBuffer[index] = '\0';
            break;
         }
         espResponseBuffer[index] = c;
         index ++;
       }
   }
   strcpy(cadena, espResponseBuffer);   //Copiames el Buffer en cadena.
   //uartWriteString(UART_USB, espResponseBuffer);    Sirve para imprimir por completo lo que tiene el buffer. "DEBUG"
}

void esp32_init( void ){
   
   // ---------- CONFIGURACIONES ------------------------------

   //gpioInit( GPIO5, GPIO_OUTPUT );// Gpio5 como Salida senial del led para saber si hay conexion con el esp32.
   //gpioInit( GPIO6, GPIO_OUTPUT );// Gpio6 como Salida senial del led para saber si hay alguien conectado al esp32.
   
   // Inicializar otra UART_USB para debug y mostrar mensaje en consola. 
   uartConfig( UART_DEBUG, UARTS_BAUD_RATE );
   uartWriteString(UART_USB, "UART_USB configurada correctamente.\r\n");

   // Inicializar otra UART donde se conecta el ESP32 y mostrar mensaje en consola.
   uartInit( UART_ESP32, UARTS_BAUD_RATE );
   uartWriteString(UART_USB, ">>>> UART_ESP (donde se conecta el ESP32), \r\n>>>> configurada como salida de consola.\r\n" );
}

/*==================[fin del archivo]========================================*/