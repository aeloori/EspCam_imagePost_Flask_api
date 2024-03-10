#include <Camera.h> // Replace with your camera library header (e.g., OV7670.h, ESP32Camera.h)
#include <WiFiClient.h>  // For HTTP client
#include <Base64.h>

// WiFi and Flask API server details (replace with your credentials)
const char* ssid = "Web3.0";
const char* password = "@Sweethome";
const char* serverAddress = "https://aeloori.pythonanywhere.com/upload_image";  // Replace with your API endpoint

Camera camera;
WiFiClient client;


void setup() {
  Serial.begin(115200);  // Initialize serial communication for debugging (optional)
  Serial.println("Connecting to WiFi..");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected!");

  // Initialize camera (refer to your camera library's documentation)
  camera.init(FRAMESIZE_VGA);  // Adjust resolution as needed
}

void loop() {
  // Trigger image capture based on your requirement (e.g., button press, sensor reading)
  if (/* condition to capture image */) {
    captureAndSendImage();
  }
  delay(1000);  // Adjust delay between captures as needed
}

void captureAndSendImage() {
  camera.capture();
  camera.waitCapture();

  // Get image data size (adjust data type based on your camera library)
  int imageSize = camera.getImageSize();
  Serial.print("Image size: ");
  Serial.println(imageSize);

  // Allocate memory for image data
  uint8_t* imageData = new uint8_t[imageSize];
  camera.readImage(imageData, imageSize);

  // Encode image data to Base64 string
  String encodedImageData=Base64.encode(imageData, imageSize, encodedImageData);
  Serial.println(encodedImageData);

  // Construct HTTP POST request with JSON payload
  String request = "POST " + String(serverAddress) + " HTTP/1.1\r\n";
  request += "Host: " + String(serverAddress) + "\r\n";
  request += "Content-Type: application/json\r\n";
  request += "Connection: close\r\n";
  request += "\r\n";
  request += "{\"image\": \"" + encodedImageData + "\"}\r\n\r\n";

  Serial.println("Sending image data...");

  // Connect to server and send request
  if (client.connect(serverAddress, 80)) {
    client.print(request);
    client.flush();

    // Check response code (handle errors as needed)
    int statusCode = client.read();
    if (statusCode == 200) {
      Serial.println("Image sent successfully!");
    } else {
      Serial.print("Error sending image. Status code: ");
      Serial.println(statusCode);
    }

    client.stop();  // Close connection
  } else {
    Serial.println("Connection to server failed.");
  }

  // Free memory allocated for image data
  delete[] imageData;
}