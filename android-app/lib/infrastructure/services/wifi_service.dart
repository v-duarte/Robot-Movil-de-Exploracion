import 'package:permission_handler/permission_handler.dart';
import 'package:wifi_iot/wifi_iot.dart';

final WifiService wifiService = WifiService();                 // Instancia global  

class WifiService {
  String ssid = "UNLP_RC_Car";        //Nombre de la red
  String password = "cetadrc01";      //Contraseña
 
  Future<bool> connectToWiFi() async {
    try {
      bool isConnected = await WiFiForIoTPlugin.connect(       //Intentamos conectar a la red WiFi
        ssid, 
        password: password, 
        security: NetworkSecurity.WPA,                         //Puede ser WPA, WPA2, etc., según tu red
      );
      if (await WiFiForIoTPlugin.isConnected()){               //Fuerza a que todo el tráfico use la red WiFi conectada
        WiFiForIoTPlugin.forceWifiUsage(true);
      }
      return isConnected;                                      //Retornamos si la conexión fue exitosa     
    } catch (e) {                                              //Si ocurre un error lo captura
      return false;                                            //Si hay un error, retornamos falso
    }
  }

  Future<void> requestPermissions() async {                    //Pide los siguientes permisos al usuario
    if (await Permission.location.isDenied) {
      await Permission.location.request();
    }
    if (await Permission.location.isPermanentlyDenied) {
      openAppSettings();
    }
  }

  Future<bool> disconnectFromWiFi() async {                     // Método para desconectar de la red WiFi
    try {
      WiFiForIoTPlugin.forceWifiUsage(false);                   // Intentamos desconectar la red WiFi
      bool disconnected = await WiFiForIoTPlugin.disconnect();
        return disconnected;                                    // Retornamos si la se desconecto exitosamente
    } catch (e) {                                               //Si ocurre un error lo captura
      return false;                                             //Si hay un error, retornamos falso
    }
  }
}