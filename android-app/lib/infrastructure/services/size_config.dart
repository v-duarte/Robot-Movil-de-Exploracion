import 'package:flutter/material.dart';

class SizeConfig {
  static late double screenWidth;
  static late double screenHeight;
  static late double blockWidth;
  static late double blockHeight;

  static void init(BuildContext context) {                  //Inicializa las dimensiones basadas en el contexto
    final size = MediaQuery.of(context).size;
    screenWidth = size.width;
    screenHeight = size.height;
    blockWidth = screenWidth / 100;                         //Calculamos bloques como un porcentaje (1% del total)
    blockHeight = screenHeight / 100;
  }
}

// ejemplo de uso:
// child: Container(
//           width: SizeConfig.blockWidth * 80,  // 80% del ancho         // Ajusta el tamaño en base al porcentaje de la pantalla
//           height: SizeConfig.blockHeight * 20, // 20% del alto
//           color: Colors.blue,
//           child: Center(
//             child: Text(
//               "Soy Responsive",
//               style: TextStyle(
//                 fontSize: SizeConfig.blockWidth * 5, // Texto ajustado (5% del ancho)
//               ),
//             ),
//           ),
//         ),