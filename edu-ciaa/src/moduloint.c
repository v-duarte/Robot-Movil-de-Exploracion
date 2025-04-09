/*==================[inlcusiones]============================================*/
#include "sapi.h"
#include "moduloint.h"


/*==================[definiciones y macros]==================================*/
bool_t dataToSendToUart232Pending = false;

/*==================[definiciones de datos internos]=========================*/

/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/

/*==================[declaraciones de funciones externas]====================*/

/*==================[funcion principal]======================================*/

bool_t getFlagUart232(void){
   return dataToSendToUart232Pending;
}
void setFlagUart232(void){
   dataToSendToUart232Pending = false;
}

// Implementacion de la funcion de callback
void dataInBuffer( void *param ){
   uartInterrupt(UART_232, false);
   uartWriteString(UART_USB, ">>>> Atendiendo interrupcion. \r\n" );
   dataToSendToUart232Pending = true;
   uartClearPendingInterrupt(UART_232);
}

void mod_int_init ( void){
      uartWriteString(UART_USB, ">>>> Mon Init. \r\n" );
      uartInterrupt(UART_232, true);      // Habilitar interrupcion por recepcion de UART 232
      uartCallbackSet(UART_232, UART_RECEIVE, dataInBuffer, NULL);     // Configurar callback para recepcion
      uartClearPendingInterrupt(UART_232);
      uartWriteString(UART_USB, ">>>> Fin \r\n" );
}