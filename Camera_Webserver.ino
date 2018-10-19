#include "Camera_Exp.h"
#include <WiFi.h>

CAMERA cam;
char ssid[] = "DESS_MAKER_LAB";      //  your network SSID (name)
char pass[] = "1212312121";   // your network password
WiFiServer server(80);

IPAddress local_IP(192, 168, 0, 112); //111,112,113
IPAddress gateway(192, 168, 0, 107);
IPAddress subnet(255, 255, 255, 0);

String http_header = "HTTP/1.1 200 OK\r\n";
String http_stream = "Content-type: multipart/x-mixed-replace; boundary=123456789000000000000987654321\r\n\r\n";
String http_jpg = "Content-type: image/jpg\r\n\r\n";
String http_boundary = "--123456789000000000000987654321\r\n";
WiFiClient client;

void setup()
{
  Serial.begin(115200);

  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("STA Failed to configure");
  }

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.macAddress());
  Serial.print("Subnet Mask: ");
  Serial.println(WiFi.subnetMask());
  Serial.print("Gateway IP: ");
  Serial.println(WiFi.gatewayIP());
  server.begin();
  cam.setFrameSize(CAMERA_FS_VGA);
  esp_err_t err = cam.init();
  if (err != ESP_OK)
  {
    Serial.println("Camera init failed with error =" + String( err));
    return;
  }

Serial.println("Open Web Browser");
Serial.print("Get for single frame --> http://");//http://192.168.0.102/capture
Serial.print(WiFi.localIP());
Serial.println("/capture");

Serial.print("Stream  --> http://");//http://192.168.0.102/capture
Serial.print(WiFi.localIP());
Serial.println("/stream");


}

void loop()
{
  delay(5000);

  Serial.println("connecting to ");
  
  const int httpPort = 80;

  String httpreq;
  client = server.available();
  if (client)
  {
    Serial.println("New Client.");
    String httpreq = "";
    while (client.connected())
    {
      if (client.available())
      {
        String httpreq_line = client.readStringUntil('\n');
        httpreq += httpreq_line;
        if (httpreq_line == "\r")
        {
          if (httpreq.indexOf("GET /stream") != -1)
          {
                Serial.println("Stream");
                stream();
          }
          if (httpreq.indexOf("GET /capture") != -1)
          {
                Serial.println("Capture");
                capture();
          }
          
          httpreq = "";
          client.stop();
        }

      }
    }

  }
}

void stream()
{
  client.print(http_header);
  client.print(http_stream);  
  while (client.connected())
  {
    esp_err_t err;
    err = cam.capture();
    if (err != ESP_OK)
    {
      Serial.println("Camera capture failed with error =" + String(err));
      return;
    }
    client.print(http_jpg);
    client.write(cam.getfb(),cam.getSize());
    client.print(http_boundary);
  }
}
void capture()
{
   esp_err_t err;
    err = cam.capture();
    if (err != ESP_OK)
    {
      Serial.println("Camera capture failed with error =" + String(err));
      return;
    }
   client.print(http_header);
   client.print(http_jpg);
   client.write(cam.getfb(),cam.getSize());
   
}
