/*=====[Evitar inclusion multiple comienzo]==================================*/

#ifndef _CTRL_MARCHAS_H_
#define _CTRL_MARCHAS_H_

/*=====[Inclusiones de dependencias de funciones publicas]===================*/

//#include "main.h"
//#include <main.h>

/*=====[C++ comienzo]========================================================*/

//#ifdef __cplusplus
//extern "C" {
//#endif

/*=====[Macros de definicion de constantes publicas]=========================*/

//#define PI 3.14

/*=====[Macros estilo funcion publicas]======================================*/

//#define printInt(printer,number) printIntFormat((printer),(number),(DEC_FORMAT))

//#define printlnString(printer,string);   {printString((printer),(string));\
                                          printEnter((printer));}                                      

/*=====[Definiciones de tipos de datos publicos]=============================*/

// Tipo de datos que renombra un tipo basico
//typedef uint8_t bool_t;

// Tipo de datos de puntero a funcion
//typedef void (*callBackFuncPtr_t)(void *);

// Tipo de datos enumerado
/*typedef enum {
   GPIO_INPUT, 
   GPIO_OUTPUT
} gpioMode_t; */

/*typedef enum {
   HMC5883L_REG_X_MSB = 0x00, 
   HMC5883L_REG_X_LSB = 0x01,
   HMC5883L_REG_Y_MSB = 0x02,
   HMC5883L_REG_Y_LSB = 0x03,
   HMC5883L_REG_Z_MSB = 0x04,
   HMC5883L_REG_Z_LSB = 0x05
} hmc5883lRegisters_t; */

// Tipo de datos estructua, union o campo de bits
//typedef struct {
  // uint16_t year;	 /* 1 to 4095 */
   //uint8_t  month; /* 1 to 12   */
   //uint8_t  mday;	 /* 1 to 31   */
   //uint8_t  wday;	 /* 1 to 7    */
   //uint8_t  hour;	 /* 0 to 23   */
   //uint8_t  min;	 /* 0 to 59   */
   //uint8_t  sec;	 /* 0 to 59   */
//} rtc_t; 

/*=====[Prototipos de funciones publicas]====================================*/

void ctrl_Init( void );                                                                 //Inicializa el programa
void ctrl_ruedas (int pos_x, int pos_y);                          				//Actualiza el estado de las ruedas.

/*=====[Prototipos de funciones publicas de interrupcion]====================*/

//void UART0_IRQHandler(void);

/*=====[C++ fin]=============================================================*/

#ifdef __cplusplus
}
#endif

/*=====[Evitar inclusion multiple fin]=======================================*/

#endif /* _MODULE_NAME_H_ */