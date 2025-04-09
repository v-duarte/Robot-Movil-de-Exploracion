import 'dart:io';
import 'dart:async';
import 'dart:typed_data';
import 'package:udp/udp.dart';
import 'package:connectivity_plus/connectivity_plus.dart';
final UdpService udpService = UdpService();                                                   // Instancia global

class UdpService {
  bool ok = false;
  int portControl = 1234;
  int portDato = 1235;
  int numPacket = 0;
  String esp32Ip = '0.0.0.0';
  late UDP sender;
  late UDP receiver;
  late Endpoint esp32Endpoint;

Future<bool> connectToServer() async {
  receiver = await UDP.bind(Endpoint.any(port: Port(portDato)));                              //Configurar el receptor UDP para escuchar la respuesta en el puerto 'portDato'.
  Completer<bool> completer = Completer();                                                    // Crear un Completer para controlar la respuesta
  receiver.asStream(timeout: const Duration(seconds: 15)).listen(                              // Configurar el stream de escucha con un timeout de 5 segundos. (En paralelo puedo ejecutar otras acciones)
    (datagram) {
      if (datagram != null) {                                                                 // Convertir el paquete de datos en un string
        var respuesta = String.fromCharCodes(datagram.data);
        if (respuesta.contains('AP IP:')) {                                                   // Comprobar si la respuesta contiene la IP
          esp32Ip = respuesta.replaceAll('AP IP:', '').trim();                                // Guardar la IP recibida
          esp32Endpoint = Endpoint.unicast(InternetAddress(esp32Ip), port: Port(portDato));
          completer.complete(true);                                                           // Comunicación establecida
        }
      }
    },
    onDone: () {                                                                              // Si no se completó la comunicación en el tiempo asignado, completamos con 'false'
      if (!completer.isCompleted) {
        completer.complete(false);
      }
    },
  );
  //Ejecucion en paralelo al Stram
  sender = await UDP.bind(Endpoint.any(port: Port(portControl)));                             //Enviar el mensaje "200" al ESP32 para que responda con su IP.
  var dataLength = await sender.send("200".codeUnits, Endpoint.broadcast(port: Port(portControl)));

  if (dataLength <= 0) {                                                                      //Si no se pudo enviar el mensaje, retornar 'false'
    return false;
  }

  return completer.future;                                                                    //Retornar si la comunicación se estableció correctamente (espera la respuesta del ESP32)
}
  void cerrarComunicacion() {                                                                 //Cerrar la comunicación cuando ya no sea necesario
    numPacket = 0;
    sender.close();
    receiver.close();
  }

  void enviarDato(String msj) async {
    try {
      var connectivityResult = await Connectivity().checkConnectivity();                        // Verificar la conectividad de la red
      if (connectivityResult.contains(ConnectivityResult.none)) {
        return;                                                                                 // Si no hay conexión, no intentamos enviar los datos
      }
      String mensaje = "$numPacket,$msj";                                                       // Le agrego al mensaje el número de packet
      List<int> datos = Uint8List.fromList(mensaje.codeUnits);                                  // Convertir el mensaje a bytes
      sender = await UDP.bind(Endpoint.any(port: Port(portDato)));                              // Abrir puerto UDP
      esp32Endpoint = Endpoint.unicast(InternetAddress(esp32Ip), port: Port(portDato));
      await sender.send(datos, esp32Endpoint);                                                  // Enviar el mensaje, usando un try-catch para manejar posibles errores de red
      numPacket++;                                                                              // Incrementar el número del paquete
    } on SocketException catch (e) {                                                         //Aquí podrías implementar un mecanismo de reintento o alertar al usuario
    } 
  }


}