#include <WiFi.h>
#include <HTTPClient.h>
#include <HX711.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Configuración WiFi
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// Pines para el sensor HX711
#define DOUT_PIN_1 12
#define SCK_PIN_1 14
#define DOUT_PIN_2 27
#define SCK_PIN_2 26

// Configuración del slide switch
#define SLIDE_SWITCH_PIN 13

// Configuración del sensor de proximidad HC-SR04
#define TRIG_PIN 5
#define ECHO_PIN 18

// Variables del endpoint y token
String endpointId = "4";
const char* bearerToken = "eyJhbGciOiJIUzI1NiJ9.eyJzdWIiOiIzNDVSQzEyIiwiaWF0IjoxNzMxNzg0ODA3LCJleHAiOjE3MzE4NzEyMDd9.rnhrVlBKJOVTo_00uyhIVp9QrwXFJnbgy5L4zKQ5ZzQ";

// Instancias del sensor HX711
HX711 scale1;
HX711 scale2;

// Configuración inicial
void setup() {
    Serial.begin(115200);

    // Configuración de pantalla OLED
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("Fallo al inicializar OLED"));
        for (;;);
    }
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.display();

    // Configuración de pines y calibración de las celdas de carga HX711
    scale1.begin(DOUT_PIN_1, SCK_PIN_1);
    scale1.set_scale(60.f);
    scale1.tare();

    scale2.begin(DOUT_PIN_2, SCK_PIN_2);
    scale2.set_scale(60.f);
    scale2.tare();

    // Configuración del slide switch
    pinMode(SLIDE_SWITCH_PIN, INPUT_PULLUP);

    // Configuración del sensor HC-SR04
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);

    // Conexión WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Conectando a WiFi...");
    }
    Serial.println("Conectado a WiFi.");
}

// Función para medir el peso en gramos
float getWeight(HX711 &scale) {
    if (scale.is_ready()) {
        return scale.get_units(10);
    } else {
        Serial.println("Error: Sensor no está listo.");
        return -1;
    }
}

// Función para medir la distancia con el HC-SR04
float getDistance() {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    long duration = pulseIn(ECHO_PIN, HIGH);
    float distance = (duration * 0.0343) / 2; // Convertir a centímetros
    return distance;
}

// Enviar datos del estado de la mesa (solicitud PUT)
void sendTableData(int tableCapacity, int tableGuests, String tableStatus, String needAssistanceStatus) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String serverName = "https://kitchen-tech-fqghavb0fychfkhm.brazilsouth-01.azurewebsites.net/api/kitchentech/v1/table/" + endpointId;

        http.begin(serverName);
        http.addHeader("Content-Type", "application/json");
        http.addHeader("Authorization", "Bearer " + String(bearerToken));

        StaticJsonDocument<200> jsonDoc;
        jsonDoc["id"] = endpointId.toInt();
        jsonDoc["tableNumber"] = 1;
        jsonDoc["tableCapacity"] = tableCapacity;
        jsonDoc["tableGuests"] = tableGuests;
        jsonDoc["tableStatus"] = tableStatus;
        jsonDoc["NeedAssistance"] = needAssistanceStatus;
        jsonDoc["restaurantId"] = 1;

        String jsonPayload;
        serializeJson(jsonDoc, jsonPayload);

        int httpResponseCode = http.PUT(jsonPayload);
        if (httpResponseCode > 0) {
            Serial.printf("Código de respuesta: %d\n", httpResponseCode);
        } else {
            Serial.printf("Error en la solicitud HTTP: %s\n", http.errorToString(httpResponseCode).c_str());
        }
        http.end();
    } else {
        Serial.println("WiFi desconectado");
    }
}

// Enviar datos del suministro al servidor
void sendSupplyData(float currentlyOnStock) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String serverName = "https://kitchen-tech-fqghavb0fychfkhm.brazilsouth-01.azurewebsites.net/api/kitchentech/v1/supply/1";

        http.begin(serverName);
        http.addHeader("Content-Type", "application/json");
        http.addHeader("Authorization", "Bearer " + String(bearerToken));

        StaticJsonDocument<200> jsonDoc;
        jsonDoc["id"] = 1;
        jsonDoc["supplyName"] = "Cacao";
        jsonDoc["supplyCategory"] = "Desserts";
        jsonDoc["metricUnit"] = "g";
        jsonDoc["currentlyOnStock"] = currentlyOnStock;
        jsonDoc["costPerUnit"] = 0.02;
        jsonDoc["stateOfSupply"] = "OnStock";
        jsonDoc["restaurantId"] = 6;

        String jsonPayload;
        serializeJson(jsonDoc, jsonPayload);

        int httpResponseCode = http.PUT(jsonPayload);
        if (httpResponseCode > 0) {
            Serial.printf("Código de respuesta: %d\n", httpResponseCode);
        } else {
            Serial.printf("Error en la solicitud HTTP: %s\n", http.errorToString(httpResponseCode).c_str());
        }
        http.end();
    } else {
        Serial.println("WiFi desconectado");
    }
}

void updateDisplay(int tableCapacity, int tableGuests, String tableStatus, String needAssistanceStatus, float distance) {
    display.clearDisplay();  // Limpia la pantalla para evitar superposición

    // Línea 1: Estado de la mesa
    display.setCursor(0, 0);
    display.print("Table Capacity: ");
    display.print(tableCapacity);
    
    display.setCursor(0, 10);
    display.print("Quantity Guest: ");
    display.print(tableGuests);

    // Línea 2: Estado general
    display.setCursor(0, 20);
    display.print("Table Status: ");
    display.print(tableStatus);

    // Línea 3: Necesita ayuda
    display.setCursor(0, 30);
    display.print("Need Assistance: ");
    display.print(needAssistanceStatus);

    // Línea 5: Distancia
    display.setCursor(0, 50);
    display.print("Distance: ");
    display.print(distance, 1);
    display.print(" cm");

    // Actualiza la pantalla con los nuevos datos
    display.display();
}

void loop() {
    // Lógica de la mesa
    float weight1 = getWeight(scale1);
    float weight2 = getWeight(scale2);

    int tableGuests = 0;
    if (weight1 >= 3) tableGuests++;
    if (weight2 >= 3) tableGuests++;

    int tableCapacity = 4;
    String tableStatus = (tableGuests > 0) ? "Occupied" : "Free";
    String needAssistanceStatus = digitalRead(SLIDE_SWITCH_PIN) == LOW ? "Yes" : "No";

    sendTableData(tableCapacity, tableGuests, tableStatus, needAssistanceStatus);

    // Lógica del sensor de proximidad
    float distance = getDistance();
    if (distance > 0 && distance <= 100) { // Solo enviar si la distancia es válida y en rango
        float currentlyOnStock = 120.0 - (distance * 0.5); // Ejemplo de cálculo basado en distancia
        sendSupplyData(currentlyOnStock);
    }

    updateDisplay(tableCapacity, tableGuests, tableStatus, needAssistanceStatus, distance);

    delay(1000); // Espera 1 segundo para la próxima iteración
}
