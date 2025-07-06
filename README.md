Sistema de Monitoreo LoRa
Descripción
Sistema de monitoreo basado en PCB que mide temperatura, humedad, presión y gases (alcohol) en tiempo real, con transmisión de datos vía LoRa (915 MHz) a un Gateway que los sube a Firebase. Los datos se visualizan en una plataforma web: Sistema de Monitoreo.

Características
Sensores: BME680 (temperatura, humedad, presión, gases), MQ3 (alcohol).
Microcontrolador: ESP32-WROVER-IE.
Comunicación: LoRa (RFM95W), Wi-Fi, I2C.
Alimentación: 5V vía USB-C.
Dimensiones: 90x90 mm.

Archivos
Diseños: /Altium (esquemáticos, PCB, Gerber, BOM).
Documentación: /doc (instrucciones, datasheets).
Firmware: /firmware (código para ESP32).

Licencias
Hardware y diseños: CERN Open Hardware Licence v2 - Permissive (LICENSE)
Firmware: MIT License (LICENSE-SOFTWARE)
Documentación: Creative Commons Attribution 4.0 (LICENSE-DOCUMENTATION)

Certificación OSHWA
Este proyecto está en proceso de certificación con OSHWA como hardware de código abierto. [ID de certificación pendiente].

Instrucciones
Fabricación: Usa los archivos Gerber en /Altium/gerber para fabricar el PCB.
Ensamblaje: Sigue /doc/assembly.md.
Firmware: Carga el código desde /firmware usando Arduino IDE.