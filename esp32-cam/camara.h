#ifndef CAMARA_H
#define CAMARA_H

#include <Arduino.h>

void setup_camara();                                        //Inicia la camara
size_t tomarFoto(uint32_t colores[32], uint8_t **bmp_buf);  //Toma una imagen y la procesa para enriquecerla con datos
void alternarModo();                                        //Alterna la camara entre el modo de streaming y captura de fotos 
void startCameraServer();                                   //Inicia el servidor de la cámara para streaming

#endif