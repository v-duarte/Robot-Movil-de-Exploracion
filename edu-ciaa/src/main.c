#include "sapi.h"       // <= sAPI header
#include "ctrl_marchas.h" //Modulo del motor
#include "esp32_cam.h" //Modulo de conexion con la ESP32-CAM
#include "mensajes.h"  //Libreria que se encarga de procesar los mensajes.
#include "string.h"
#include "moduloint.h"

#define CADENA_BUFF_SIZE         512


int main(void){
   char cadenaBuffer[ CADENA_BUFF_SIZE ];
   uint32_t cadenaBufferSize = CADENA_BUFF_SIZE;
   /* ------------- INICIALIZACIONES ------------- */

   /* Inicializar la placa */
   boardConfig();
      
   /* Inicializar el motor */
   ctrl_Init();
   
   /* Inicializar la conexion UART con la ESP32-CAM */
   esp32_init();
   
   /*Inicializar interrupcion */
   mod_int_init();
   
   delay (100);
   while (1){

         if(!getFlagUart232()) {          // Pone el procesador en espera hasta que ocurra una interrupcion (No funciona. Hay alguna interrupcion que lo activa y no es UART.)
            //gpioWrite( GPIO5, OFF );
            __WFI();
            delay(1);
            //gpioWrite( GPIO5, ON );
         }
         // Si ocurrio una interrupcion, procesar los datos aqui
         if (getFlagUart232()) {
            uartWriteString(UART_USB, ">>>> Llego una interrupcion \r\n" ); 
            while(uartRxReady( UART_232 )){
               reciveMessage(cadenaBuffer);
               uartWriteString( UART_USB, "Recibi el siguiente mensaje:");
               uartWriteString( UART_USB, cadenaBuffer );
               uartWriteString( UART_USB, "\r\n Procese el siguiente mensaje: \r\n");
               procesarMenssage(cadenaBuffer);
               memset(cadenaBuffer, 0, CADENA_BUFF_SIZE);                       //limpiamos el buffer antes de usarlo nuevamente.
            }
            setFlagUart232();    // Resetear la bandera despues de procesar
            uartInterrupt(UART_232, true); 
         }
   }
   /* NO DEBE LLEGAR NUNCA AQUI, debido a que a este programa no es llamado
      por ningun S.O. */
   return 0 ;
   /*==================[end of file]============================================*/
}