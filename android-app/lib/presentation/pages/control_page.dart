import 'package:flutter/material.dart';
import 'package:flutter_joystick/flutter_joystick.dart';
import 'package:flutter/services.dart';
import 'package:control/infrastructure/services/udp_service.dart';
import 'package:flutter_mjpeg/flutter_mjpeg.dart';

class ControlPage extends StatefulWidget {
  const ControlPage({super.key});

  @override
  State<ControlPage> createState() => _ControlPageState();
}

class _ControlPageState extends State<ControlPage> {
  double valorVertical = 0.0;
  double valorHorizontal = 0.0;
  String coordenadas = "0.0,0.0";

  @override
  void initState() {
    super.initState();
    SystemChrome.setPreferredOrientations([
      DeviceOrientation.landscapeRight,
      DeviceOrientation.landscapeLeft,
    ]);
  }

  @override
  void dispose() {
    udpService.enviarDato("400");
    udpService.cerrarComunicacion();
    SystemChrome.setPreferredOrientations([
      DeviceOrientation.portraitUp,
      DeviceOrientation.portraitDown,
    ]);
    super.dispose();
  }

  void crearDatos() {
    coordenadas = '$valorVertical,$valorHorizontal';
    udpService.enviarDato(coordenadas);
  }

  void tomarFoto() {
    // Función para tomar una foto
    udpService.enviarDato('foto');
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text("ControlRemoto"),
        backgroundColor: const Color.fromARGB(255, 2, 83, 94),
      ),
      body: Stack(
        children: <Widget>[
          const Center(
            child: Padding(
              padding: EdgeInsets.all(16.0), // Espaciado alrededor del stream
              child: AspectRatio(
                aspectRatio: 4 / 3,
                child: Mjpeg(
                  stream: 'http://192.168.4.1/stream',
                  isLive: true,
                ),
              ),
            ),
          ),
          Align(
            alignment: Alignment.bottomLeft,
            child: Padding(
              padding: const EdgeInsets.all(20.0),
              child: Joystick(
                includeInitialAnimation: false,
                mode: JoystickMode.vertical,
                base: JoystickBase(
                  mode: JoystickMode.vertical,
                  decoration: JoystickBaseDecoration(
                    color: const Color.fromARGB(150, 0, 0, 0),
                    drawOuterCircle: false,
                  ),
                  arrowsDecoration: JoystickArrowsDecoration(
                    color: const Color.fromARGB(255, 3, 238, 218),
                  ),
                ),
                listener: (details) {
                  setState(() {
                    valorVertical = double.parse((-details.y).toStringAsFixed(2));
                  });
                  crearDatos();
                },
              ),
            ),
          ),
          Positioned(
            top: 20,
            left: 20,
            child: Text(
              coordenadas,
              style: const TextStyle(
                fontSize: 24,
                color: Colors.white,
                fontWeight: FontWeight.bold,
              ),
            ),
          ),
          Align(
            alignment: Alignment.bottomRight,
            child: Padding(
              padding: const EdgeInsets.all(20.0),
              child: Joystick(
                includeInitialAnimation: false,
                mode: JoystickMode.horizontal,
                base: JoystickBase(
                  mode: JoystickMode.horizontal,
                  decoration: JoystickBaseDecoration(
                    color: const Color.fromARGB(150, 0, 0, 0),
                    drawOuterCircle: false,
                  ),
                  arrowsDecoration: JoystickArrowsDecoration(
                    color: const Color.fromARGB(255, 3, 238, 218),
                  ),
                ),
                listener: (details) {
                  setState(() {
                    valorHorizontal = double.parse((details.x).toStringAsFixed(2));
                  });
                  crearDatos();
                },
              ),
            ),
          ),
        ],
      ),
      floatingActionButton: FloatingActionButton(
        onPressed: tomarFoto,
        backgroundColor: Colors.teal,
        child: const Icon(Icons.camera_alt),
      ),
      backgroundColor: const Color.fromARGB(255, 2, 83, 94),
    );
  }
}
