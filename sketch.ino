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

// Variables del endpoint y token
String endpointId = "3";
const char* bearerToken = "eyJhbGciOiJIUzI1NiJ9.eyJzdWIiOiIzNDVSQzEyIiwiaWF0IjoxNzMwNTAzMjE1LCJleHAiOjE3MzA1ODk2MTV9.1j1Vle2aqvQ4TDTmjhW9NF-RFgTrEvxHXgrNEIzGZyU";

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

// Enviar datos al servidor (solicitud PUT)
void sendData(int tableCapacity, int tableGuests, String tableStatus, String needAssistanceStatus) {
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
