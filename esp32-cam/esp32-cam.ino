#include <WiFi.h>
#include "camara.h"
#include "conexion_udp.h"
#include "tf_lc02.h"
#include "motor_28byj48.h"
#include "ftp.h"

//Configuraciones para el barrido
const int pasosInicio = 120;
const int cantidadMedidas = 16;
const int pasosDesplazamiento = 16;
bool posicionSensor = 0;              //Indica la posicion del sensor. 1 = derecha, 0 = izquierda.

const char* ssid = "UNLP_RC_Car";     // Nombre de la red AP
const char* password = "cetadrc01";   // Contraseña
IPAddress ipServidor;
char buffer[20];                      //Buffer para mensajes udp

unsigned long ultimoMensaje = 0;      //Tiempo del último mensaje recibido
const unsigned long timeout = 20000;  //Tiempo de espera

void setup() {
  Serial.begin(115200);               //Comunicacion con la ciaa de 115200 esta en Serial=Usb

  WiFi.softAP(ssid, password);        // Iniciar AP

  ipServidor = WiFi.softAPIP();       //Guardo mi ip

  setup_udp(ipServidor);              // Iniciar servidor UDP abre puerto de Control por defecto.

  setup_camara();                     //Inicia la camara
  
  setup_lidar();
  setup_motor(15);                    //Se inicia el motor paso a paso a 15 RPM
  setup_ftp();

}

bool conexionCiaa(){
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

void cerrarConexion(){
  Serial.print("400");          //Envio mensaje de fin a la ciaa
  Serial.print('\n');
  //Centro el sensor
  desconectarCliente();         //Desconecto al cliente de la comunicacion
  if(!posicionSensor)
    moverMotor(pasosInicio, 1);
  else
    moverMotor(pasosInicio, 0); 
}
void procesarMensaje(String mensaje) {

  mensaje.trim();                     // Elimino posibles espacios en blanco, saltos y otros caracteres invisibles.
  
  int codigo = 0;                     // Inicializamos el código para el switch.

  if (mensaje == "200") {
    codigo = 200;                     //Mensaje de inicio
  } else if (mensaje == "400") {
    codigo = 400;                     //Mensaje de fin
  } else if (mensaje == "foto") {
    codigo = 1000;                    // Asigno un valor arbitrario para "foto".
  }

  switch (codigo) {
    case 200:
      conexionCiaa();              //Conexion con ciaa
      conectarCliente();            //Inicio comunicacion con el cliente
      //Coloco el sensor en posicion para realizar barridos
      if(!posicionSensor)
        moverMotor(pasosInicio, 0);
      else
        moverMotor(pasosInicio, 1);
      ultimoMensaje = millis();                                   //Inicializo tiempo del ultimo mensaje recibido.
      break;
    
    case 400:
      cerrarConexion();      
      break;

    case 1000:
      //Realizar el barrido.
      alternarModo();               // Paso a modo para tomar fotos
      pausarComunicacion();
      barrido();
      alternarModo();               // Paso a modo streaming
      renaudarComunicacion();
      break;

    default:
      Serial.println(mensaje);      //Envío a la ciaa si no coincide con "200", "400" o "foto".
      Serial.print('\n');
      break;
  }
}

void loop() {
  if (WiFi.softAPgetStationNum() > 0) {                             //Hay alguien conectado?
    while(getComunicacion()){                                       //mientras exista comunicacion
      if(hayPaqueteDatos(buffer, sizeof(buffer))){                  //Si hay un paquete "Solo debe enviar a los de la ip asociada, los demas los descarta"
        String mensaje = String(buffer);
        procesarMensaje(mensaje);                                   //Proceso el mensaje.
        ultimoMensaje = millis();                                   //Actualizo tiempo del ultimo mensaje recibido.
      }
      else {                                                        //Si no hay mensaje del usuario que tiene la comunicacion establecida
        if (millis() - ultimoMensaje > timeout){                    //Se revisa el tiempo del ultimo mensaje recivido de dicho usuario, si supera el timeout              
          //Desconecto de la CIAA por timeout                       //Se desconecta la ciaa.
          cerrarConexion();                                       //Se termina la comunicacion con el usuario, y queda libre para otra comunicacion.
        }
      }
    }
   if(hayPaqueteControl(buffer, sizeof(buffer))){                   //Si hay paquete de cualquier usuario.
      String mensaje = String(buffer);
      procesarMensaje(mensaje);                                   //Proceso el mensaje para ver si es de inicio de comunicacion.
    }    
  }
  else {
    //Podria vaciar buffer del control "llamar una funcion de udp que haga = udp.flush();"
    //O dormirse hasta que se conecte alguien
  }
}

void barrido(){
  uint8_t *bmp_buf = NULL;                   // Búfer que contendrá los datos BMP
  uint32_t medidas[cantidadMedidas] = {};    // Vector para almacenar las medidas en formato 0xRRGGBB.
  // Realizar el barrido
  for (int i = 0; i < cantidadMedidas; i++) {
    int indice = posicionSensor ? (cantidadMedidas - 1 - i) : i;  // Determinar el indice respecto a la dirección del barrido: false = izquierda (0), true = derecha (1)
    medidas[indice] = mapearColor(getDistanciaCalibrada());
    // Mover el motor en la dirección correspondiente (invirtiendo el valor de posicionSensor)
    if (i < cantidadMedidas - 1) {
      moverMotor(pasosDesplazamiento, !posicionSensor);
    }
  }

  // Alternar la posición del sensor para el siguiente barrido
  posicionSensor = !posicionSensor;

  // Procesar la imagen y añadir las medidas
  size_t buf_len = tomarFoto(medidas, &bmp_buf);
  if(buf_len > 0) {  
    delay(200);                                // Espera para prevenir reinicio por pico de consumo acorde al tiempo de procesamiento.
    if(WiFi.status() != WL_CONNECTED) {        // Verificar conexión WiFi antes de subir la imagen al servidor
      conectarWiFi();
    }
    subirFoto(bmp_buf, buf_len);
  }

  // Liberar memoria del buffer una vez finalizado
  free(bmp_buf);
}


uint32_t mapearColor(int distancia) {                     // Función para mapear distancia en milímetros a color RGB
  int colorDistancia = constrain(distancia/10, 0, 255);   // Me aseguro que el color equivalente a la distancia esté dentro del rango 0-255

  // Mapear distancia a valores RGB (rojo cercano, verde lejano)
  int red = 255 - colorDistancia;
  int blue = 0;                                           // Mantener en 0 para un gradiente rojo-verde
  int green = colorDistancia;

  // Empaquetar el color en formato 0xRRGGBB
  return (red << 16) | (green << 8) | blue;
}