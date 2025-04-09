/*==================[inlcusiones]============================================*/
#include <ctrl_marchas.h>
#include "sapi.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/*==================[definiciones y macros]==================================*/

/*==================[definiciones de datos internos]=========================*/

/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/

/*==================[declaraciones de funciones externas]====================*/

/*==================[funcion principal]======================================*/

void procesarMenssage ( char *cadena){   //  "ets Jun 8 2016" "AP IP address: 160..."
    int pos_X = 101; 
    int pos_Y = 101;
    int index = 0;
    char *endptr;
    char *token = strtok(cadena, ",");
    
   if (token != NULL) {
        double valor = strtod(token, &endptr);  // Intentamos convertir el token a número
        if (*endptr == '\0') {                  // Verificamos si toda la cadena fue convertida
            pos_X = valor * 100;
        }
    }
    token = strtok(NULL, ",");  // Continuamos con el siguiente token
    if (token != NULL) {
        double valor = strtod(token, &endptr);
        if (*endptr == '\0') {
            pos_Y = valor * 100;
        }
    }
    switch (pos_X){
        case 200:
            //Enviar mensaje al ESP32 de conexion establecida.
            //vaciar el buffer de UART_232
            //uartWriteString(UART_232, "200");                   //envio la cadena 200 para que el ESP32 sepa que puede empezar a transmitir.
            //Enciendo led de conexion activa.
            //gpioWrite( GPIO6, ON );
            uartWriteString(UART_USB, "Llego un 200."); 
            break;
        case 400: 
            //Seria que el ESP32 envio mensaje que el usuario se desconecto.
            //vaciar el buffer de UART_232
            //Apago el led de conexion activa.
            //gpioWrite( GPIO6, OFF );
            uartWriteString(UART_USB, "Llego un 400."); 
        
        default:
            if (pos_X > -101 && pos_X < 101 && pos_Y > -101 && pos_Y < 101){
               uartWriteString( UART_USB, "Enviando Cordenadas al Auto: ");
               uartWriteString(UART_USB, intToStringGlobal(pos_X));
               uartWriteString(UART_USB, ",");
               uartWriteString(UART_USB, intToStringGlobal(pos_Y));
               uartWriteString( UART_USB, "\r\n");
               ctrl_ruedas (pos_X, pos_Y);
            }
            break;
    } 
}