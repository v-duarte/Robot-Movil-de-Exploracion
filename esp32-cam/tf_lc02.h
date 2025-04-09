#ifndef TF_LC02_H
#define TF_LC02_H

#include <Arduino.h>

void setup_lidar();           //Inicia el sensor LiDAR
int getDistancia();           //Retorna la distancia medida por el sensor como tal
int getDistanciaCalibrada();  //Retorna la distancia medida por el sensor compensando el desfasaje entre la camara y el sensor

#endif
