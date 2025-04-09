#include <WiFi.h>
#include "camara.h"
#include "conexion_udp.h"
#include "tf_lc02.h"
#include "motor_28byj48.h"
#include "ftp.h"

const char* ssid = "UNLP_RC_Car";  // Nombre de la red
const char* password = "cetadrc01";// Contraseña
IPAddress ipServidor;
char buffer[20];                   //Buffer para mensajes udp
bool ciaa = false;

unsigned long ultimoMensaje = 0;   //Tiempo del último mensaje recibido
const unsigned long timeout = 60000;  //Tiempo de espera (2 segundos), lo subi a 60 por delays en pruebas.

void setup() {
  Serial.begin(115200);         //Comunicacion con la ciaa de 115200 esta en Serial=Usb

  WiFi.softAP(ssid, password);  // Iniciar AP

  ipServidor = WiFi.softAPIP(); //Guardo mi ip

  setup_udp(ipServidor);        // Iniciar servidor UDP abre puerto de Control por defecto.

  setup_camara();
  
  setup_lidar();
  setup_motor(15); //Se inicia el motor paso a paso a 15 RPM

  setup_ftp();

}

bool conexion_ciaa(){
  bool ok= false;
  Serial.print("200");  //Mando un mensaje con 200 a la CIAA
  Serial.print('\n');   //Mando el caracter de fin de linea.
  unsigned long startMillis = millis(); // Captura el tiempo de inicio
  unsigned long timeout = 2000; // Timeout de 5 segundos

  while (!ok && (millis() - startMillis < timeout)){
    if (Serial.available()){
      String mensaje = Serial.readStringUntil('\n'); //Lee hasta el salto de linea.
      if (mensaje == "200") {
      ok = true;
      }
    }
  }
  if (ok){
   return true; // Conexion establecida... 
  }
  return false; // Fallo la conexion.
}

void procesar_mensaje(String mensaje) {

  mensaje.trim();  // Elimino posibles espacios en blanco, saltos y otros caracteres invisibles.
  
  int codigo = 0;  // Inicializamos el código para el switch.

  if (mensaje == "200") {
    codigo = 200;                 //Mensaje de inicio
  } else if (mensaje == "400") {
    codigo = 400;                 //Mensaje de fin
  } else if (mensaje == "foto") {
    codigo = 1000;                // Asigno un valor arbitrario para "foto".
  }

  switch (codigo) {
    case 200:
      conexion_ciaa();                                 //Conexion con ciaa
      conectarCliente();                               //Inicio comunicacion con el cliente
      //Serial.println("Cliente establecion comunicacion");
      break;
    
    case 400:
      Serial.print("400");                          //Envio mensaje de fin a la ciaa
      Serial.print('\n');
      desconectarCliente();                           //Desconecto al cliente de la comunicacion
      //Serial.println("Cliente cerro la comunicacion");
      break;

    case 1000:
      //Realizar el barrido.
      alternarModo();  // Paso a modo para tomar fotos
      pausarComunicacion();
      barrido();
      alternarModo();  // Paso a modo streaming
      renaudarComunicacion();
      break;

    default:
      Serial.println(mensaje);  //Envío a la ciaa si no coincide con "200", "400" o "foto".
      Serial.print('\n');
      break;
  }
}

void loop() {
  if (WiFi.softAPgetStationNum() > 0) {                             //Hay alguien conectado?
    while(getComunicacion()){                                       //mientras exista comunicacion
      if(hayPaqueteDatos(buffer, sizeof(buffer))){                  //Si hay un paquete "Solo debe enviar a los de la ip asociada, los demas los descarta"
        String mensaje = String(buffer);
        procesar_mensaje (mensaje);                                 //Proceso el mensaje.
        ultimoMensaje = millis();                                   //Actualizo tiempo del ultimo mensaje recivido.
      }
      else {                                                        //Si no hay mensaje del usuario que tiene la comunicacion establecida
        if (millis() - ultimoMensaje > timeout){                    //Se revisa el tiempo del ultimo mensaje recivido de dicho usuario, si supera el timeout              
          //Desconecto de la CIAA por timeout                       //Se desconecta la ciaa.
          //desconectarCliente();                                     //Se termina la comunicacion con el usuario, y queda libre para otra comunicacion.
        }
      }
    }
   if(hayPaqueteControl(buffer, sizeof(buffer))){                   //Si hay paquete de cualquier usuario.
      String mensaje = String(buffer);
      procesar_mensaje (mensaje);                                   //Proceso el mensaje para ver si es de inicio de comunicacion.
    }    
  }
  else {
    //Vaciar buffer del control "llamar una funcion de udp que haga = udp.flush();"
    //Dormirse hasta que se conecte alguien
  }
}

void barrido(){
  uint8_t *bmp_buf = NULL;    // Búfer que contendrá los datos BMP
  uint32_t medidas[16] = {};  //Aqui se guardaran las medidas en formato 0xRRGGBB.

  if(WiFi.status() != WL_CONNECTED){  // Revisa si tiene conexion a WiFi
    conectarWiFi();
  }

  moverMotor(128, 0);  //Giro a la izquierda para comenzar el barrrido
  for (int i=0; i < 15; i++){
    medidas[i] = mapearColor(getDistanciaCalibrada());  //Guardo el color que corresponde a la distancia que mide el sensor.
    moverMotor(16, 1);  //Giro a la derecha 16 pasos
  }
  medidas[15] = mapearColor(getDistanciaCalibrada());
  moverMotor(128, 0);  //Giro a la izquierda 128 pasos para centrar el sensor.
  size_t buf_len = tomarFoto(medidas, &bmp_buf); //Procesa la imagen para añadir las medidas.
  if(buf_len > 0){  //Si buf_len = 0, se traduce como false en el if. true si buf_len > 0.
    delay(200); //Un delay de aproximadamente el tiempo de procesamiento para prevenir que un pico de consumo reinicie el programa
    subirFoto(bmp_buf, buf_len);
  }
  free(bmp_buf);  //Libero la memoria ocupada por el buffer una vez que termine
}

uint32_t mapearColor(int distancia) { // Función para mapear distancia en milímetros a color RGB
  //No usa funcion de mapeo
  int colorDistancia = constrain(distancia/10, 0, 255);   // Me aseguro que el color equivalente a la distancia esté dentro del rango 0-255

  // Mapear distancia a valores RGB (rojo cercano, verde lejano)
  int red = 255 - colorDistancia;
  int blue = 0; // Mantener en 0 para un gradiente rojo-verde
  int green = colorDistancia;

  // Empaquetar el color en formato 0xRRGGBB
  return (red << 16) | (green << 8) | blue;
}