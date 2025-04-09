#include "sapi.h"       // <= sAPI header
#include <stdbool.h>
#include <stdlib.h>
/* Puertos del driver:(Pines puente H output)
   1 y 4 positivos
   2 y 3 negativos
   -- La configuracion anterior es para que los motores giren hacia adelante. --
  Puertos del driver:
   GPIO1(+) GPIO2(-) ENA: PWM0    T_FIL1   Motor Izquierdo
   GPIO4(+) GPIO3(-) ENB: PWM4    T_FIL3   Motor Derecho
*/

/* ------------- INICIALIZACIONES ------------- */
void ctrl_Init( )
{
   /* Inicializar los GPIO como salida */
   gpioConfig( GPIO1, GPIO_OUTPUT );
   gpioConfig( GPIO2, GPIO_OUTPUT );
   gpioConfig( GPIO3, GPIO_OUTPUT );
   gpioConfig( GPIO4, GPIO_OUTPUT );
   
   /* Inicializar los PWM */
   // Configurar el (o los) Timer(s) para generar las senales PWM
   pwmInit( 0, PWM_ENABLE );    
   pwmInit( PWM0 , PWM_ENABLE_OUTPUT ); //Enable motor A, pin TFIL1 Actua sobre el motor en los pines 1, 2 Output del puente H
   pwmInit( PWM4 , PWM_ENABLE_OUTPUT );  //Enable motor B, pin T_FIL3 Actua sobre el motor en los pines 3, 4 Output del puente H   
}

void ctrl_motor (int sentido, int  velocidad, pwmMap_t  PWM, gpioMap_t  positivo, gpioMap_t  negativo)  //Este modulo energiza el motor referenciado por el PWM.
{
   if (sentido > 0){   
      gpioWrite(negativo, OFF);       
      gpioWrite(positivo, ON);
   }else if (sentido < 0) {
      gpioWrite(positivo, OFF);
      gpioWrite(negativo, ON);
   }else if (sentido = 0) {
      gpioWrite(positivo, OFF);
      gpioWrite(negativo, OFF);
   }
   pwmWrite( PWM, velocidad );
}

int obtener_vel (int pos_x, int pos_y, pwmMap_t pwm) // Este modulo calcula la velocidad de giro para una rueda especifica
{  
   int aux_x = 0;   
   int aux_y = 0;
   int abs_x = abs (pos_x);
   int abs_y = abs (pos_y);
   int velocidad;
      if ((pwm == PWM0) && (pos_y < 0) || (pwm == PWM4) && (pos_y > 0) ) { //Chequeo si el motor es el que quiere doblar.
         if (abs_y > 0 && abs_y <38){                                    //Determinamos cuanto reducir la velocidad para doblar.
               aux_y = 0;
            } else {
               aux_y = 2.55 * abs_y;
            }
         if (abs_x > 0 && abs_x <38){                                    //Determinamos velocidad inicial.
               aux_x = 0;
            } else {
               aux_x = 2.55 * abs_x;
            }
         velocidad = (aux_x) - (aux_y);                                    //Determinamos Velocidad final del motor.
      }
      else{                                                                //Este motor no esta afectado para doblar
            if (abs_x > 0 && abs_x <38){
               velocidad = 0;
            } else {
               velocidad = 2.55 * abs_x;
            }
         }
   return velocidad;
}

void ctrl_ruedas (int pos_x, int pos_y)  //Este modulo recorre todas las ruedas, y en base al las posiciones de los controles, calcula para cada una la velocidad de giro y las energiza.
{
   int positivo;
   int negativo;
   int i;
   int velocidad;
   for (i=0; i<2; i++){
      switch(i) {
        case 0:
            velocidad = obtener_vel (pos_x, pos_y,PWM0);
            ctrl_motor (pos_x,velocidad,PWM0, GPIO1, GPIO2);
            break;
        case 1:
            velocidad = obtener_vel (pos_x, pos_y,PWM4);
            ctrl_motor (pos_x,velocidad,PWM4, GPIO4, GPIO3);
            break;
        default:
           break;
      }
   }
}
