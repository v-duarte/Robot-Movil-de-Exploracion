#ifndef CONEXION_UDP_H
#define CONEXION_UDP_H

#include <Arduino.h>

void setup_udp(IPAddress ip);                                 //Iniciar servidor UDP de control por defecto
bool getComunicacion();                                       //Retorna true = comunicacion establecida o false = no hay comunicacion
bool hayPaqueteDatos(char *buffer, size_t bufferSize);        //Retorna true = si hay paquete udpDatos del usuario conectado 
bool hayPaqueteControl(char *buffer, size_t bufferSize);      //Retorna true = si hay paquete udpControl de cualquier usuario
void conectarCliente();                                       //Establece comunicacion con el cliente.
void desconectarCliente();                                    //Finaliza comunicacion con el cliente
void limpiarPaquetes (char *buffer, size_t bufferSize);       //Limpieza para descartar paquetes.
void pausarComunicacion();                                    //Cierra el puerto de datos para que el sistema no reciba mensajes durante el barrido 
void renaudarComunicacion();                                  //Abre el puerto para renaudar la comunicacion con la aplicacion al finalizar el barrido
#endif