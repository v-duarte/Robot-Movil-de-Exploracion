#include "esp_camera.h"
#include "WiFi.h"
#include "ESP32_FTPClient.h"
#include "time.h" // Para manejar el tiempo

const char* ssid_wifi = "Ingrese nombre de la red Wi-Fi";
const char* pass_wifi = "Ingrese contraseña de la red";

// Configuración del servidor FTP
char* ftp_server = "192.168.1.10";

char* ftp_user = "Ingrese usuario FTP";
char* ftp_pass = "Ingrese clave de usuario";
char* ftp_dir = ".";    //Directorio de trabajo del servidor 

// Definición de constantes para definir una zona horaria.
const int UTC_OFFSET_SECONDS = -10800; // UTC-3 (en segundos)
const int DAYLIGHT_OFFSET_SECONDS = 0; // Sin horario de verano

const int maxIntentos = 10; // Número máximo de intentos para realizar una conexion inalambrica

// FTP Client
ESP32_FTPClient ftp(ftp_server, ftp_user, ftp_pass);

// Función para generar un timestamp como nombre de archivo
String getTimestamp() {
  time_t now = time(nullptr);
  struct tm timeinfo;
  localtime_r(&now, &timeinfo);

  char buffer[30];
  strftime(buffer, sizeof(buffer), "%Y%m%d_%H%M%S.bmp", &timeinfo); // Formato: "20241217_154530.bmp"
  return String(buffer);
}

// Función para conectarse al WiFi
void conectarWiFi() {
  int intentos = 0;

  WiFi.begin(ssid_wifi, pass_wifi);

  while (WiFi.status() != WL_CONNECTED && intentos < maxIntentos) {
    delay(500);
    intentos++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    intentos = 0; //Reinicio el numero de intentos para usar la variable en la sincronizacion de hora
    // Configurar NTP para sincronizar la hora
    configTime(UTC_OFFSET_SECONDS, DAYLIGHT_OFFSET_SECONDS, "pool.ntp.org");
    while ((time(nullptr) < 1) && (intentos < maxIntentos)) {
      delay(500);
      intentos++;
    }
    digitalWrite(33, LOW); // LED indica éxito de conexion Wi-Fi
  } else {
    digitalWrite(33, HIGH); // Sin conexión WiFi
  }
}

// Función para enviar la foto al servidor FTP
void subirFoto(uint8_t * buf, size_t buf_len) {
  // Si tiene conexion a WiFi, sube la foto al servidor FTP
  if(WiFi.status() == WL_CONNECTED){
    // Obtener el nombre del archivo con timestamp
    String filename = getTimestamp();

    // Conectar al servidor FTP
    ftp.OpenConnection();

    // Subir la foto al FTP
    ftp.ChangeWorkDir(ftp_dir);  // Cambia al directorio de trabajo del servidor
    ftp.InitFile("Type I");  // Cambiar a modo binario
    ftp.NewFile(filename.c_str()); // Nombre del archivo en el servidor
    ftp.WriteData(buf, buf_len);
    ftp.CloseFile();
    ftp.CloseConnection();
  }
}

void setup_ftp() {
  pinMode(33, OUTPUT); // Configuro el pin como salida
  digitalWrite(33, HIGH); //Apago el LED integrado del ESP32 para indicar que esta no conectado a Internet.
  //No se conecta automaticamente a Internet, en su lugar se intentara conectar a Internet cuando se toma una foto si es necesario.
}