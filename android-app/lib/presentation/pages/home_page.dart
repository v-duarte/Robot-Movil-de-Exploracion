import 'package:control/infrastructure/services/wifi_service.dart';
import 'package:control/infrastructure/services/udp_service.dart';
import 'package:control/presentation/pages/control_page.dart';
import 'package:flutter/material.dart';

class HomePage extends StatefulWidget{
  static const name = 'home_page';
  const HomePage({super.key});
  @override
  State<HomePage> createState() => _HomePageState();
}
class _HomePageState extends State<HomePage> with WidgetsBindingObserver {
  bool conectado = false;
  bool comunicacion = false;
  late WifiService wifi;

  @override
  void initState() {
    super.initState();
    wifi = WifiService();
    wifi.requestPermissions();
    WidgetsBinding.instance.addObserver(this);
    
  }
   @override
  void dispose() {
    WidgetsBinding.instance.removeObserver(this);
    super.dispose();
  }

  void onButtonPressed1() async {
    conectado = await wifi.connectToWiFi();
    setState(() {

    });
  } 

  void onButtonPressed2() async {             
  showDialog(                                                                         //Muestra un cuadro de diálogo con el ícono de carga
    context: context,
    barrierDismissible: false,                                                        //Impide que el usuario cierre el diálogo tocando fuera de él
    builder: (BuildContext context) {
      return const AlertDialog(
        backgroundColor: Color.fromARGB(255, 41, 41, 41),
        content: Row(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            CircularProgressIndicator(color: Colors.white,),                        //El ícono de carga
            SizedBox(width: 10),                                                      //Espacio entre el ícono y el texto
            Text('Iniciando comunicacion...',style: TextStyle(color: Colors.white),),//Texto opcional
          ],
        ),
      );
    },
  );
    bool comunicacion = await udpService.connectToServer();
    if (!mounted) return;
    Navigator.pop(context);
    if (comunicacion){
      Navigator.push(
      context,
      MaterialPageRoute(builder: (context) => const ControlPage()),
      );
    }
    else {
      mostrarConexionFallida(context);
      setState(() {

    });
    }
  }

  void onButtonPressed3() async {
    conectado = !(await wifi.disconnectFromWiFi());
    setState(() {

    });
  }

  void mostrarConexionFallida(BuildContext context) {
    const snackBar = SnackBar(
      content: Text('Vehiculo en uso'),
      backgroundColor: Colors.red,                                                  // Puedes cambiar el color de fondo
    );
    ScaffoldMessenger.of(context).showSnackBar(snackBar);                             // Mostrar el Snackbar
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: Stack(
        children: <Widget>[
          Container(
            decoration: const BoxDecoration(
              image: DecorationImage(
                image: AssetImage("lib/assets/images/FONDO.png"),                     // Ruta de tu imagen
                fit: BoxFit.cover,                                                    // Ajusta la imagen al tamaño del contenedor
              ),
            ),
          ),
          /*Align( 
            alignment: Alignment.topRight, 
            child: IconButton(
                    icon: const Icon(Icons.refresh_rounded),
                    onPressed: (){
                      setState(() {
                      });
                    },
                  ), 
          ),*/
          if (!conectado)
          Positioned(
            bottom: 110,
            left: 65,
            right: 65,  
            child: SizedBox(
              height: 90,
              child: TextButton(
                      onPressed: onButtonPressed1,
                      style: ElevatedButton.styleFrom(padding: const EdgeInsets.all(0),
                                                      minimumSize: Size.zero,
                                                      tapTargetSize: MaterialTapTargetSize.shrinkWrap,),
                      child: Image.asset('lib/assets/images/button_conectar.png',fit: BoxFit.cover,),
              ),
            ),
          ),
          if (conectado)
          Positioned(
            bottom: 125,
            left: 0,
            right: 0,
            height: 60,
            child: Row(
                      mainAxisAlignment: MainAxisAlignment.center,
                      children: <Widget>[
                        Align(  
                          child:  TextButton(
                                    onPressed: onButtonPressed2,
                                    style: ElevatedButton.styleFrom(padding:  const EdgeInsets.all(0),
                                                                    minimumSize: Size.zero,
                                                                    tapTargetSize: MaterialTapTargetSize.shrinkWrap,),
                                    child: Image.asset('lib/assets/images/button_controles.png',fit: BoxFit.cover,),
                            ),
                        ),
                      Align(  
                        child: TextButton(
                                  onPressed: onButtonPressed3,
                                  style: ElevatedButton.styleFrom(padding:  const EdgeInsets.all(0),
                                                                  minimumSize: Size.zero,
                                                                  tapTargetSize: MaterialTapTargetSize.shrinkWrap,),
                                  child: Image.asset('lib/assets/images/button_desconectar.png',fit: BoxFit.cover,),
                          ),
                      ),
                      ], 
                    ),
          ), 
        ],
      ),
    );
  }
}
