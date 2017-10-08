
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

//from the book
//<NUL><NUL><NUL><NUL><NUL><SOH>”Z00”<STX>”AAHELLO”<EOT>
const char nul=0x00;
const char soh=0x01;
const char* z00="\x7A\x30\x30";
const char stx=0x02;
const char* cmd="\x41\x41";
//const char* msg="\x4D\x41\x4b\x45\x52\x46\x58";
const char* msg = "MakerFX";
const char eot=0x04;


const char* ssid = "[replaced]";
const char* password = "[replaced]";
const int   port = 2112;
IPAddress ip(192,168,1,201);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

String oldtxt;

ESP8266WiFiMulti WiFiMulti;


#define USE_SERIAL Serial

void writeToSign(String txt) {
    if (txt != oldtxt) {
      Serial1.write(nul);
      Serial1.write(soh);
      Serial1.write(z00);
      Serial1.write(stx);
      Serial1.write(cmd);

      Serial1.print(txt);
      Serial1.write(eot);
      oldtxt = txt;
    }
    
}

void setup() {
 
  Serial1.begin(9600, SERIAL_8N1);

   USE_SERIAL.begin(115200);
   // USE_SERIAL.setDebugOutput(true);

    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

    for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }

    WiFiMulti.addAP("makerfx-member", "m@k3stuff");
  
}

void loop() {
delay(1000);

 // wait for WiFi connection
    if((WiFiMulti.run() == WL_CONNECTED)) {

        HTTPClient http;

        USE_SERIAL.print("[HTTP] begin...\n");
        // configure traged server and url
        //http.begin("https://192.168.1.12/makerfx", "7a 9c f4 db 40 d3 62 5a 6e 21 bc 5c cc 66 c8 3e a1 45 59 38"); //HTTPS
        http.begin("http://192.168.1.68:8080/makerfx"); //HTTP

        USE_SERIAL.print("[HTTP] GET...\n");
        // start connection and send HTTP header
        int httpCode = http.GET();

        // httpCode will be negative on error
        if(httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);

            // file found at server
            if(httpCode == HTTP_CODE_OK) {
                String payload = http.getString();
                USE_SERIAL.println(payload);
                writeToSign(payload);
            }
        } else {
            USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();
    }
  
}

