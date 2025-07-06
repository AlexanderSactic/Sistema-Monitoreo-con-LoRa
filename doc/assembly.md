# Instrucciones de Ensamblaje

## Requisitos
- **Herramientas**:
  - Estación de soldadura (recomendado: 350°C para soldadura sin plomo).
  - Pinzas de punta fina.
  - Multímetro para pruebas.
  - Cable USB para ESP32.
- **Componentes**: Ver la lista de materiales en `/Altium/bom/BOM.xlsx`.
- **Software**: 
  - Altium Designer para verificar esquemáticos
  - Arduino IDE para cargar firmware al ESP32, tanto del nodo como del Gateway.

## Pasos de Ensamblaje
1. **Preparación del PCB**:
   - Verifica que el PCB esté limpio y sin defectos.
   - Revisa los archivos Gerber (`/Altium/gerber`) para confirmar la disposición de las capas.
2. **Soldadura de Componentes**:
   - Comienza con componentes de bajo perfil (resistencias, condensadores).
   - Continúa con sensores y el microcontrolador.
   - Finaliza con conectores y componentes de mayor tamaño.
   - Usa el esquemático (`/Altium/Main.schdoc`) como referencia.
3. **Verificación**:
   - Usa un multímetro para verificar continuidad y detectar cortocircuitos.
   - Asegúrate de que no haya puentes de soldadura.
4. **Carga del Firmware** (si aplica):
   - Conecta el PCB a la PC mediante el cable.
   - Usa Arduino IDE para cargar el firmware desde `/firmware/Gateway` si es el módulo para el Gateway, o `/firmware/Nodo/nodo` para los nodos.
   - Configura parámetros del SSID y contraseña para Wi-Fi en el ESP32.
   - Agrega tus configuraciones de Firebase como el apiKey para poder conectarte.
5. **Pruebas**:
   - Alimenta el PCB con USB 5V.
   - Verifica que los sensores envíen datos desde el monitor serial.
   - Confirma que los datos se transmitan al Gateway en el monitor serial y en el Dashboard el indicador que el Gateway está listo para recibir los datos.

## Notas
- El sensor MQ3 requiere un tiempo de inicialización de 5 segundos para que esté listo.
- Consulta los datasheets en `/doc` para detalles de los componentes.

## Solución de Problemas
- **El PCB no enciende**: Verifica la fuente de alimentación y conexiones.
- **Los datos no se transmiten**: Asegúrate de que el firmware esté correctamente cargado y configurado.