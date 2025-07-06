# Diseño del Sistema de Monitoreo

## Descripción General
Este sistema de monitoreo está diseñado para medir temperatura, humedad, presión atmosférica y concentración de gases (alcohol) en tiempo real, con transmisión de datos mediante LoRa en áreas con señal de telefonía débil o inexistente. El PCB integra sensores, un microcontrolador y un módulo de comunicación LoRa para enviar datos desde un nodo a un Gateway, que los sube a Firebase y los muestra en una plataforma web desarrollada con Next.js y Firebase.

## Especificaciones Técnicas
- **Sensores**:
  - BME680: Temperatura, humedad, presión y gases.
  - MQ3: Detección de alcohol en el ambiente.
- **Microcontrolador**: ESP32-WROVER-IE (dual-core, 240 MHz, con soporte Wi-Fi y Bluetooth).
- **Comunicación**:
  - LoRa (RFM95W, 915 MHz) para transmisión de datos de largo alcance.
  - I2C para comunicación con sensores.
  - Wi-Fi (ESP32) para conexión del Gateway a Internet.
- **Alimentación**: 5V mediante conector USB-C (con regulador LM1117).
- **Dimensiones del PCB**: 90x90 mm.

## Diagrama de Bloques
![Diagrama de Bloques](/doc/schematic.png)

## Componentes Principales
- **Sensor BME680**:
  - Mide temperatura (±1°C), humedad (±3% RH), presión (±1 hPa) y gases (índice de calidad del aire).
  - Conexión: I2C.
- **Sensor MQ3**:
  - Detecta concentración de alcohol en el ambiente (rango: 0.04-4 mg/L).
  - Conexión: Analógica.
- **Microcontrolador ESP32-WROVER-IE**:
  - Procesa datos de los sensores y gestiona la comunicación Wi-Fi (en el Gateway) y LoRa (en el nodo).
- **Módulo LoRa RFM95W**:
  - Transmite datos a 915 MHz con protocolo LoRa, alcanzando hasta [especifica rango, por ejemplo, 1-2 km en línea de visión].
- **Otros**:
  - Indicadores NeoPixel (WS2812B) para retroalimentación visual.
  - Regulador de voltaje LM1117 para alimentación estable.

## Funcionamiento
1. Los sensores BME680 y MQ3 recopilan datos de temperatura, humedad, presión atmosférica y concentración de alcohol cada [especifica intervalo, por ejemplo, 5 segundos].
2. El microcontrolador ESP32 procesa los datos y los envía al Gateway mediante LoRa (RFM95W).
3. El Gateway, también basado en ESP32, recibe los datos y los sube a Firebase a través de Wi-Fi.
4. Los datos se visualizan en una plataforma web: [Sistema de Monitoreo](https://sistema-monitoreo-lora-alexander-sactics-projects.vercel.app/).

## Notas de Diseño
- El PCB de 2 capas reduce costos de fabricación y mantiene un diseño compacto.
- Se utilizó un stencil para facilitar el ensamblaje de componentes SMD.
- La frecuencia de 915 MHz para LoRa se seleccionó para cumplir con regulaciones locales y maximizar el alcance.

## Referencias
- [BME680 Datasheet](/doc/bst-bme680-ds001.pdf)
- [LoRa RFM95W Datasheet](/doc/RFM96W-V2.0_1695351477.pdf)
- [MQ3 Datasheet](/doc/TGS822TechnicalInfo.pdf)
- [NeoPixel (WS2812B) Datasheet](/doc/WS2812B.pdf)
- [Regulador LM1117 Datasheet](/doc/lm1117.pdf)