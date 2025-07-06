# Sistema de Monitoreo LoRa

![Diagrama de Bloques](doc/schematic.png)

Este proyecto es un **sistema de monitoreo** basado en un PCB que mide temperatura, humedad, presión atmosférica y concentración de alcohol en tiempo real. Utiliza comunicación LoRa (915 MHz) para transmitir datos desde un nodo a un Gateway, que los sube a Firebase para visualización en una plataforma web desarrollada con Next.js: [Sistema de Monitoreo](https://sistema-monitoreo-lora-alexander-sactics-projects.vercel.app/).

El proyecto está diseñado para operar en áreas con señal de telefonía débil o inexistente y se encuentra en proceso de certificación como hardware de código abierto con la [Open Source Hardware Association (OSHWA)](https://certification.oshwa.org/).

## Características

- **Sensores**:
  - **BME680**: Mide temperatura (±1°C), humedad (±3% RH), presión (±1 hPa) y gases (índice de calidad del aire).
  - **MQ3**: Detecta concentración de alcohol (rango: 0.04-4 mg/L).
- **Microcontrolador**: ESP32-WROVER-IE (dual-core, 240 MHz, Wi-Fi y Bluetooth).
- **Comunicación**:
  - LoRa (RFM95W, 915 MHz) para transmisión de largo alcance.
  - Wi-Fi (ESP32) para conexión del Gateway a Internet.
  - I2C para sensores.
- **Alimentación**: 5V mediante USB-C (regulador LM1117).
- **Dimensiones del PCB**: 90x90 mm.
- **Indicadores**: NeoPixel (WS2812B) para retroalimentación visual.

## Estructura del Repositorio

| Carpeta       | Contenido                                    |
|---------------|----------------------------------------------|
| `/Altium`     | Esquemáticos, PCB, archivos Gerber y BOM.    |
| `/doc`        | Documentación técnica y datasheets.          |
| `/firmware`   | Código fuente para el ESP32 (Arduino IDE).   |

## Requisitos

- **Hardware**:
  - Componentes listados en `/Altium/bom/BOM.csv`.
  - Estación de soldadura para ensamblaje SMD.
  - Programador USB-C para cargar el firmware.
- **Software**:
  - [Altium Designer](https://www.altium.com/) para abrir esquemáticos y PCB.
  - [Arduino IDE](https://www.arduino.cc/en/software) para compilar y cargar el firmware.
- **Fabricación**:
  - Usa los archivos Gerber en `/Altium/gerber` para fabricar el PCB (recomendado: JLCPCB o PCBWay).

## Instalación y Uso

1. **Fabricación del PCB**:
   - Exporta los archivos Gerber desde `/Altium/gerber` y envíalos a un fabricante de PCBs.
   - Consulta la lista de materiales (BOM) en `/Altium/bom/BOM.csv` para adquirir componentes.

2. **Ensamblaje**:
   - Sigue las instrucciones en `/doc/assembly.md` para soldar los componentes (BME680, MQ3, RFM95W, ESP32, NeoPixel).
   - Verifica conexiones con un multímetro para evitar cortocircuitos.

3. **Carga del Firmware**:
   - Abre el código en `/firmware/main.ino` con Arduino IDE.
   - Configura la librería LoRa para 915 MHz y los parámetros de conexión Wi-Fi (SSID, contraseña).
   - Carga el firmware al ESP32 mediante USB-C.

4. **Uso**:
   - Alimenta el nodo con 5V vía USB-C.
   - Los datos se transmitirán al Gateway vía LoRa y se visualizarán en la plataforma web: [Sistema de Monitoreo](https://sistema-monitoreo-lora-alexander-sactics-projects.vercel.app/).

## Licencias

- **Hardware y Diseños**: [CERN Open Hardware Licence v2 - Permissive](LICENSE)
- **Firmware**: [MIT License](LICENSE-SOFTWARE)
- **Documentación**: [Creative Commons Attribution 4.0](LICENSE-DOCUMENTATION)

## Certificación OSHWA

Este proyecto está en proceso de certificación con OSHWA como hardware de código abierto. [ID de certificación pendiente]. Todos los diseños, firmware y documentación son accesibles en este repositorio para cumplir con la definición de hardware abierto.

## Contribuir

¡Las contribuciones son bienvenidas! Por favor:
1. Crea un *fork* del repositorio.
2. Realiza tus cambios en una rama nueva.
3. Envía un *pull request* con una descripción clara de las mejoras.

Consulta `/doc/system_design.md` para detalles técnicos y `/doc/assembly.md` para instrucciones de ensamblaje.

## Referencias

- [BME680 Datasheet](/doc/bst-bme680-ds001.pdf)
- [LoRa RFM95W Datasheet](/doc/RFM96W-V2.0_1695351477.pdf)
- [MQ3 Datasheet](/doc/TGS822TechnicalInfo.pdf)
- [NeoPixel (WS2812B) Datasheet](/doc/WS2812B.pdf)
- [Regulador LM1117 Datasheet](/doc/lm1117.pdf)

## Contacto

Para preguntas o sugerencias, dejo mi correo: alexander1sactic1@gmail.com