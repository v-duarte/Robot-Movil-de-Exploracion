#include <Stepper.h>
#include "motor_28byj48.h"

// ULN2003 Motor Driver Pins
#define IN1 15
#define IN2 14
#define IN3 2
#define IN4 4

const int stepsPerRevolution = 2048;  // Numero de pasos por revolucion acorde al motor paso a paso

// Inicializa la libreria stepper
Stepper myStepper(stepsPerRevolution, IN1, IN3, IN2, IN4);

void setup_motor(int rpm) {
  // Configura la velocidad del motor
  myStepper.setSpeed(rpm);
}

void moverMotor(int pasos, int direccion){
  if (direccion){
    myStepper.step(pasos);
  }
  else{
    myStepper.step(-pasos);
  }
}