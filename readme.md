# Proyecto ESP32 con Módulos de Sensores y Pantalla OLED

Este proyecto está diseñado para trabajar con un ESP32 y varios módulos conectados, incluyendo celdas de carga (HX711), un sensor ultrasónico (HC-SR04), un switch deslizante y una pantalla OLED (SSD1306). El sistema permite realizar mediciones de peso, distancia y mostrar información en pantalla, ideal para aplicaciones como balanzas electrónicas o sistemas de monitoreo.

## Componentes Utilizados

### Microcontrolador
- **ESP32 DevKitC v4**

### Sensores
- **2 x HX711** (Celdas de carga de 5kg)
- **HC-SR04** (Sensor ultrasónico)

### Otros
- **SSD1306 OLED** (Pantalla OLED con dirección I2C 0x3C)
- **Switch deslizante** (Interruptor para alternar estados)

## Conexiones

### Celdas de carga HX711
#### Cell1
- DT: GPIO 12
- SCK: GPIO 14
- VCC: 3.3V
- GND: GND

#### Cell2
- DT: GPIO 27
- SCK: GPIO 26
- VCC: 3.3V
- GND: GND

### Sensor ultrasónico HC-SR04
- VCC: 3.3V
- GND: GND
- TRIG: GPIO 5
- ECHO: GPIO 18

### Pantalla OLED SSD1306
- SDA: GPIO 21
- SCL: GPIO 22
- VCC: 3.3V
- GND: GND

### Switch deslizante
- Pin 1: GPIO 13
- Pin 2: GND

## Dependencias
Este proyecto puede necesitar las siguientes bibliotecas para un correcto funcionamiento, estos son:
- **Adafruit_GFX**: Para control gráfico de la pantalla OLED.
- **Adafruit_SSD1306**: Para control del módulo SSD1306.
- **HX711**: Para la comunicación con las celdas de carga HX711.
- **Ultrasonic**: Para manejar el sensor ultrasónico HC-SR04.

## Configuración y Uso
1. **Instalación de bibliotecas**: Asegúrate de instalar las bibliotecas necesarias desde el Administrador de Bibliotecas de Arduino IDE o tu entorno preferido.
2. **Conexiones**: Realiza las conexiones de acuerdo con las especificaciones mencionadas.
3. **Código**: Carga el código de control en el ESP32.
4. **Funcionamiento**:
   - Las celdas de carga medirán el peso y enviarán los datos al ESP32.
   - El sensor ultrasónico medirá la distancia y proporcionará resultados precisos.
   - La pantalla OLED mostrará los datos recopilados.
   - El switch deslizante permitirá alternar estados dentro del programa.

