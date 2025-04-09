import 'package:control/presentation/pages/home_page.dart';
import 'package:flutter/material.dart';
import 'package:control/presentation/routes/app_route.dart';
import 'package:control/presentation/themes/app_theme.dart';
import 'package:control/infrastructure/services/udp_service.dart';


void main() => runApp(const MyApp());

class MyApp extends StatefulWidget {
  const MyApp({super.key});
  @override
  State<MyApp> createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> with WidgetsBindingObserver {
  @override
  void initState() {
    super.initState();
    WidgetsBinding.instance.addObserver(this); // Observa el ciclo de vida
  }

  @override
  void didChangeAppLifecycleState(AppLifecycleState state) {
    if (state == AppLifecycleState.paused || state == AppLifecycleState.detached) {
      udpService.cerrarComunicacion();                                                //Llama a tu método para cerrar la comunicación
      final navigator = Navigator.of(context);
      if (navigator.canPop()) {
        navigator.popUntil((route) => route.isFirst); // Regresa al inicio
      } else {
        navigator.pushReplacement(
          MaterialPageRoute(builder: (context) => const HomePage()),
        );
      }
    }
  }

  @override
  Widget build(BuildContext context) {
    return MaterialApp.router(
      routerConfig: appRouter,
      debugShowCheckedModeBanner: false,
      title: 'ControlRemoto',
      theme: AppTheme().getThem(),
    );
  }
}