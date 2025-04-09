#include "conexion_udp.h"
#include <WiFiUdp.h>

IPAddress ipServidorUDP;
IPAddress ipCliente;

const int portControl = 1234;
const int portDatos = 1235;

WiFiUDP udpControl;       //Socket para control
WiFiUDP udpDatos;         //Socket para Datos

uint32_t lastSequence = 0;        //Número de secuencia del último paquete procesado
uint32_t maxSeqNum = UINT32_MAX;  //El máximo número de secuencia posible

bool clienteConectado = false;    //Verdadero = Comunicacion establecida
bool controlActivo = false;       //Verdadero = Puerto de control abierto
bool datosActivo = false;         //Verdadero = Puerto de datos abierto

void abrirPort(int port){
  switch (port) {
    case portControl:
      udpControl.begin(port);    //Iniciar servidor UDP de control
      controlActivo = true;
      break;

    case portDatos:
      udpDatos.begin(port);      //Iniciar servidor UDP de datos
      datosActivo = true;
      break;
    default:
      //Puerto incorrecto.
      break;
  }
}

void setup_udp(IPAddress ip){
  ipServidorUDP = ip;            //Guardo la ip para enviar al cliente
  abrirPort(portControl);        //Iniciar servidor UDP de control por defecto
}


void cerrarPort(int port){
  switch (port) {
    case portControl:
      udpControl.stop();         //Cierra servidor UDP de control
      controlActivo = false;
      break;

    case portDatos:
      udpDatos.stop();           //Cierra servidor UDP de datos
      datosActivo = false;
      break;
    default:
      //Puerto incorrecto.
      break;
  }
}

void pausarComunicacion(){
  cerrarPort(portDatos);
}

void renaudarComunicacion(){
  abrirPort(portDatos);
}

void enviarMensaje(int port, IPAddress ip, const char* mensaje) { 
  udpDatos.beginPacket(ip, port);              //Creo paquete para enviar a ip, por el puerto
  udpDatos.write((const uint8_t*)mensaje, strlen(mensaje));    //Escribe el mensaje
  udpDatos.endPacket();                        //Finalizar y enviar el paquete
}

bool getComunicacion(){
  return clienteConectado;
}

bool getPortControlActivo(){
  return controlActivo;
}

bool getPortDatosActivo(){
  return datosActivo;
}

void descartarPacket(int packetSize,char *buffer, size_t bufferSize) {  //Solo esta implementado para descartar paquetes de datos
  while (packetSize > 0) {
    int bytes = udpDatos.read(buffer, min(packetSize,(int)bufferSize)); // Lee lo que queda del paquete o lo que entre en el buffer
    if (bytes > 0) {                                                    // Si la lectura fue exitosa
      packetSize -= bytes;                                              // Decrementa el tamaño del paquete restante
    }
    else 
      break;
  }
}

void limpiarPaquetes (char *buffer, size_t bufferSize){
  int packetSize = udpDatos.parsePacket();                     // Verifica si hay un paquete recibido
  while (packetSize > 0) {
        descartarPacket (packetSize, buffer, bufferSize);
        packetSize = udpDatos.parsePacket();                   // Verifica si hay más paquetes pendientes
    }
}

bool packeteEnOrden(char *buffer, size_t bufferSize ){
  String packet = String(buffer);                                 
  int numComa = packet.indexOf(',');                           // Parsear el mensaje recibido (por ejemplo, "1,0,1")
  int sequenceNumber = packet.substring(0, numComa).toInt();    // Número de secuencia
  String datosPaquete = packet.substring(numComa + 1);         // String con el contenido del paquete sin el numero de secuencia.

  if ((sequenceNumber > lastSequence) || (lastSequence - sequenceNumber > (maxSeqNum / 2))) {  // Verificar si es un paquete más nuevo o si se reinicio el conteo de paquetes
    lastSequence = sequenceNumber;                                // Actualizar el último número de secuencia
    datosPaquete.toCharArray(buffer, bufferSize);             // Guardo el dato en el buffer.
    return true;
  }
  return false;  
}

bool hayPaqueteDatos(char *buffer, size_t bufferSize) {
  if (getPortDatosActivo() && clienteConectado){            //Chequeo socket de datos y clienteConectado
    int packetSize = udpDatos.parsePacket();
    if (packetSize > 0 && packetSize < bufferSize) {        //Chequeo que hay mensaje y que entra en el buffer
      //Serial.println("Hay paquete udp de Datos");
      if (udpDatos.remoteIP() == ipCliente){                //Chequeo que la ip del packete sea la del clienteConectado
        memset(buffer, 0, bufferSize);                      // Limpia el buffer    
        int len = udpDatos.read(buffer, bufferSize - 1);    //Guardo el mensaje en el buffer
          if (len > 0){
            buffer[len] = '\0';                             //Agrego caracter de fin del mensaje
            if (packeteEnOrden(buffer, bufferSize)){        //Se fija que el packete este en tiempo, y elimina el numero de secuencia.
              //Serial.println("Hay paquete de datos en buffer");
              return true;                                  //Se retorna true, dejando en buffer el mensaje listo para procesar.
            }
              return false;                                 //Se retorna dado que el mensaje llego tarde.
          }
          else{
            return false;                                   //Fallo la lectura del paquete.
          }
      }
      descartarPacket (packetSize, buffer, bufferSize);     //Descarto el paquete dado que no es del usuario en comunicacion
      //Serial.println("Paquete de datos descartado, no es del cliente");
      return false;
    }
    if (packetSize > bufferSize){
        descartarPacket(packetSize, buffer, bufferSize);    //Descarta el paquete porque supera el tamaño del buffer
        //Serial.println("Paquete de datos descartado, es grande");
      }
    return false;
  }
  return false;
}
bool hayPaqueteControl(char *buffer, size_t bufferSize) {
  if (getPortControlActivo()){                              //Chequeo el socket de control
    int packetSize = udpControl.parsePacket();
    if (packetSize > 0) {                                   //Chequeo que hay mensaje y que entra en el buffer
    //Serial.println("Hay paquete udp de Control");
      memset(buffer, 0, bufferSize);                        //Limpia el buffer
      ipCliente = udpControl.remoteIP();                    //Guardo la ip del ultimo mensaje
      int len = udpControl.read(buffer, bufferSize - 1);    //Guardo el mensaje en el buffer
      if (len > 0){
        buffer[len] = '\0';                                 //Agrego caracter de fin del mensaje
        //Serial.println("Hay paquete de datos en buffer");
        return true;
      }
      return false;                                         //Fallo la lectura del paquete
    }
    return false;
  }
  return false;
}

void conectarCliente(){
  clienteConectado = true;                                  //Actualizo la variable avisando que hay una comunicacion establecida
  lastSequence = 0;                                         //Reinicio el numero de secuencia.
  abrirPort (portDatos);                                    //Enviar la IP del ESP32 al cliente       
  String mensaje = "AP IP:" + ipServidorUDP.toString();     //Construir el mensaje
  const char* mensajeFinal = mensaje.c_str();               //Convierte el mensaje a const char
  enviarMensaje (portDatos, ipCliente, mensajeFinal);       //Envio mensaje de confirmacion
  //Serial.println("Se envio la ip al cliente");
  cerrarPort (portControl);
}

void desconectarCliente(){
  ipCliente = IPAddress(0, 0, 0, 0);                        // Asignar la IP "vacía" 0.0.0.0;
  //Serial.println("Cliente desconectado");
  abrirPort(portControl);
  cerrarPort(portDatos);
  clienteConectado = false;
}