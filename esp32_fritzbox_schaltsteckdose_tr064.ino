#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
WiFiMulti WiFiMulti;
#include <tr064.h>
 
//-------------------------------------------------------------------------------------
// Put your router settings here
//-------------------------------------------------------------------------------------
// Wifi network name (SSID)
const char* wifi_ssid = "......."; 
// Wifi network password
const char* wifi_password = ".....";
// The username if you created an account, "admin" otherwise
const char* fuser = "........";
// The password for the aforementioned account.
const char* fpass = "...........";
// IP address of your router. This should be "192.168.179.1" for most FRITZ!Boxes
const char* IP = "192.168.178.1";
// Port of the API of your router. This should be 49000 for all TR-064 devices.
const int PORT = 49000;
// -------------------------------------------------------------------------------------
 
// TR-064 connection
TR064 connection(PORT, IP, fuser, fpass);
 
// Die AIN der DECT!200 Steckdose findet sich im FritzBox Webinterface
const String Steckdose1 = "123456 123456";
 
void setup() {
    Serial.begin(115200);
    Serial.println("boot...");
 
    // Connect to wifi
    ensureWIFIConnection();
    Serial.println("WIFI connected...");
 
    connection.init();
    // Bei Problemen kann hier die Debug Ausgabe aktiviert werden
    //connection.debug_level = DEBUG_VERBOSE;
}
 
void loop() {
    if(Serial.available()) {
        serialEvent();
    }
    delay(20);
}
 
void serialEvent(){
  String inData;
  char inChar;
    while(Serial.available() > 0) {
        inChar = Serial.read();
        if(inChar != '\n' && inChar != '\r') {
            inData += inChar;
        }
    }
    if(inData == "on") {
        SetSwitch(Steckdose1, "ON");
    }else if(inData == "off") {
        SetSwitch(Steckdose1, "OFF");
    }else if(inData == "toggle") {
        SetSwitch(Steckdose1, "TOGGLE");
    }else if(inData == "info") {
      GetDeviceInfo(Steckdose1);
    }
    Serial.println(inData);
}
 
void SetSwitch(String AIN, String state) {
    ensureWIFIConnection();
    String paramsb[][2] = {{"NewAIN", AIN},{"NewSwitchState", state}};
    connection.action("urn:dslforum-org:service:X_AVM-DE_Homeauto:1", "SetSwitch", paramsb, 2);
}
 
void GetDeviceInfo(String AIN) {
    ensureWIFIConnection();
    String paramsb[][2] = {{"NewAIN", AIN}};
    String reqb[][2] = {{"NewMultimeterPower", ""}, {"NewTemperatureCelsius", ""}};
    connection.action("urn:dslforum-org:service:X_AVM-DE_Homeauto:1", "GetSpecificDeviceInfos", paramsb, 1, reqb, 2);
    float power = reqb[0][1].toInt() / 100.0;
    float temp = reqb[1][1].toInt() / 10.0;
    Serial.print("Stromverbrauch: ");
    Serial.print(power, 1);
    Serial.println("W");
    Serial.print("Temperatur: ");
    Serial.print(temp, 1);
    Serial.println("*C");
}
 
void ensureWIFIConnection() {
    if((WiFiMulti.run() != WL_CONNECTED)) {
        WiFiMulti.addAP(wifi_ssid, wifi_password);
        while ((WiFiMulti.run() != WL_CONNECTED)) {
            delay(100);
        }
    }
}