#include <WiFi.h>
#include <HTTPClient.h>

// Sensor da vaga 1
#define TRIG1_PIN 13
#define ECHO1_PIN 14

// Sensor da vaga 2
#define TRIG2_PIN 26
#define ECHO2_PIN 27


// nome da rede e senha
const char* ssid = "testewifi";
const char* password = "ArEnDeDe2706";
const char* apiURL = "https://fast-api-teste-five.vercel.app/dados";

float duration_us1, distance_cm1;
float duration_us2, distance_cm2;

bool ultimo_estado_vaga1 = false;
bool ultimo_estado_vaga2 = false;
// bloco para conexão de wifi
void setup() {
  Serial.begin(9600);

  WiFi.begin(ssid, password);
  Serial.print("Conectando ao WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado!");

  pinMode(TRIG1_PIN, OUTPUT);
  pinMode(ECHO1_PIN, INPUT);
  pinMode(TRIG2_PIN, OUTPUT);
  pinMode(ECHO2_PIN, INPUT);
}

float medirDistancia(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  float duration = pulseIn(echoPin, HIGH);
  return 0.017 * duration;
}

void loop() {
  distance_cm1 = medirDistancia(TRIG1_PIN, ECHO1_PIN);
  distance_cm2 = medirDistancia(TRIG2_PIN, ECHO2_PIN);

  bool vaga1_ocupada = (distance_cm1 > 0 && distance_cm1 < 25);
  bool vaga2_ocupada = (distance_cm2 > 0 && distance_cm2 < 25);

Serial.println("----- Leitura -----");

Serial.print("Vaga 1:\n  Distância: ");
Serial.print(distance_cm1);
Serial.print(" cm\n  Ocupada: ");
Serial.println(vaga1_ocupada ? "Sim" : "Não");

Serial.print("Vaga 2:\n  Distância: ");
Serial.print(distance_cm2);
Serial.print(" cm\n  Ocupada: ");
Serial.println(vaga2_ocupada ? "Sim" : "Não");

Serial.println("-------------------\n");


  // verifica se houve mudança no estado de qualquer vaga
  if (vaga1_ocupada != ultimo_estado_vaga1 || vaga2_ocupada != ultimo_estado_vaga2) {
    ultimo_estado_vaga1 = vaga1_ocupada;
    ultimo_estado_vaga2 = vaga2_ocupada;

    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      http.begin(apiURL);
      http.addHeader("Content-Type", "application/json");

      String jsonPayload = "{\"vagas\": {";
      jsonPayload += "\"1\": " + String(vaga1_ocupada ? "true" : "false") + ",";
      jsonPayload += "\"2\": " + String(vaga2_ocupada ? "true" : "false");
      jsonPayload += "}}";

      Serial.print("Enviando JSON: ");
      Serial.println(jsonPayload);

      int httpResponseCode = http.PUT(jsonPayload);

      if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.print("Resposta API: ");
        Serial.println(response);
      } else {
        Serial.print("Erro no PUT: ");
        Serial.println(httpResponseCode);
      }

      http.end();
    } else {
      Serial.println("WiFi desconectado!");
    }
  }

  delay(10000);
}
