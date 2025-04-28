#ifndef CAMARA_H
#define CAMARA_H

#include <Arduino.h>
const int cantMedidas = 16;        //Fija la cantidad de medidas que se realizaran durante el barrido.

void setup_camara();                                                  //Inicia la camara
size_t tomarFoto(uint32_t colores[cantMedidas], uint8_t **bmp_buf);   //Toma una imagen y la procesa para enriquecerla con datos
void alternarModo();                                                  //Alterna la camara entre el modo de streaming y captura de fotos 
void startCameraServer();                                             //Inicia un servidor para streaming de la camara

#endif