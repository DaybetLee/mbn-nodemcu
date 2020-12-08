extern "C"
{
#include "user_interface.h" // this is for the RTC memory read/write functions
}
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <AESLib.h>
#include <base64.h>

AESLib aesLib;
HTTPClient http;
bool msgSent;
String authToken;
int httpResponseCode;
String apiEndpoint;

int d2 = 4;
int d1 = 5;
int retry = 0;
int retryLimit = 30;
int ldrLimit = 500;
int deepSleepTimer = 5e6;
int arr[8];
String apiUrl = "http://mailboxnotify.herokuapp.com/api";
String ssid = "SSID";
String pass = "SSID Password";
String secret = "12345678";
String mac = WiFi.macAddress().c_str();
String byteArrStr = "";
char cleartext[255];
byte aes_key[] = {0xE8, 0xA1, 0xF1, 0x5D, 0x15, 0xE7, 0x6A, 0xA6, 0x7C, 0x57, 0x1B, 0xD4, 0x97, 0x83, 0x61, 0x4F};
byte enc_iv[N_BLOCK] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

void setup()
{
    WiFi.disconnect();
    Serial.begin(74880);

    pinMode(d1, OUTPUT);
    pinMode(d2, OUTPUT);
    Serial.print("INFO::Reset Reason: ");
    Serial.println(ESP.getResetReason());
    system_rtc_mem_read(65, &msgSent, 4);
    Serial.print("INFO::Read Var msgSent: ");
    Serial.println(msgSent);
}

void loop()
{
    delay(10000);
    if (digitalRead(d2) == HIGH)
    {
        Serial.println("INFO::Box closed");
        digitalWrite(d1, HIGH);
        delay(50);
        checkLDR(analogRead(A0));
    }
    else
    {
        digitalWrite(d1, LOW);
        Serial.println("INFO::Box opened");
    }
}

/////////////////////////////

void checkLDR(int value)
{
    Serial.print("INFO::IDR result: ");
    Serial.println(value);

    if (value < ldrLimit)
    {
        Serial.println("INFO::MailBox not empty");
        digitalWrite(d1, LOW);
        if (msgSent == false)
        {
            notifyOwner();
        }
        Serial.println("INFO::Notification has been sent prior. Entering Deep-Sleep");
        ESP.deepSleep(deepSleepTimer);
    }
    else
    {
        Serial.println("INFO::MailBox empty");
        digitalWrite(d1, LOW);
        msgSent = false;
        system_rtc_mem_write(65, &msgSent, 4);
        Serial.println("INFO::Updating Var msgSent to 0");
        Serial.println("INFO::Entering Deep-Sleep");
        ESP.deepSleep(deepSleepTimer);
    }
}

void notifyOwner()
{
    Serial.println("INFO::Notifying owner");
    connectWiFi();
    if (retry < retryLimit)
    {
        //      getAuthToken();
        sendNotification();
    };
    WiFi.disconnect();
    system_rtc_mem_write(65, &msgSent, 4);
    Serial.println("INFO::Entering Deep-Sleep");
    ESP.deepSleep(deepSleepTimer);
}

void connectWiFi()
{
    delay(3000);
    Serial.print("INFO::Connecting to WiFi");
    WiFi.begin(ssid, pass);
    while ((!(WiFi.status() == WL_CONNECTED) && retry < retryLimit))
    {
        delay(300);
        Serial.print("..");
        retry++;
    }
    if (retry == retryLimit)
    {
        Serial.println("failed");
    }
    else
    {
        Serial.println("connected");
    };
}

//void getAuthToken(){
//  apiEndpoint = apiUrl + "/auth";
//  http.begin(apiEndpoint);
//  http.addHeader("Content-Type", "application/json");
//  httpResponseCode = http.POST("{\"mac\":\"" + mac +"\",\"psk\":\"" + secret + "\"}");
//  if(httpResponseCode==200){
//    authToken = http.getString();
//    sendNotification();
//    http.end();
//  }
//  else{
//    Serial.print("WARN::HTTP request failed - ");
//    Serial.println(httpResponseCode);
//    Serial.print("WARN::Fail Reason: ");
//    Serial.println(http.getString());
//  };
//}

void sendNotification()
{
    apiEndpoint = apiUrl + "/mailer/alert";
    String iv = gen_iv();
    String encrypted = encrypt("{\"mac\":\"" + mac + "\",\"psk\":\"" + secret + "\"}");
    String body = "{\"message\":\"" + encrypted + "\",\"iv\":\"" + iv + "\"}";

    http.begin(apiEndpoint);
    http.addHeader("Content-Type", "application/json");
    httpResponseCode = http.POST(body);
    if (httpResponseCode == 200)
    {
        Serial.println("INFO::Updating Var msgSent to true");
        msgSent = http.getString().toInt();
        Serial.println("INFO::HTTP request sent");
    }
    else if (httpResponseCode == 405)
    {
        Serial.println("INFO::Updating Var msgSent to true");
        msgSent = http.getString().toInt();
        Serial.println("INFO::User alert has been disabled...update history ");
    }
    else
    {
        Serial.print("WARN::HTTP request failed - ");
        Serial.println(httpResponseCode);
        Serial.print("WARN::Fail Reason: ");
        Serial.println(http.getString());
    };
}

void update_iv(byte *iv, int arr[])
{
    for (int i = 0; i < N_BLOCK / 2; i++)
    {
        iv[i] = (byte)arr[i];
    }
}

void randomArr(int arr[])
{
    for (int i = 0; i < 8; i++)
    {
        arr[i] = random(0, 256);
    }
}

String encrypt(String message)
{
    aesLib.set_paddingmode(paddingMode::ZeroLength);
    sprintf(cleartext, message.c_str());
    update_iv(enc_iv, arr);
    char *msg = cleartext;
    int msgLen = strlen(msg);
    char encrypted[2 * msgLen];
    int enclen = aesLib.encrypt((const unsigned char *)msg, msgLen, encrypted, aes_key, sizeof(aes_key), enc_iv);
    encrypted[enclen] = 0;
    return String(encrypted);
}

String gen_iv()
{
    randomArr(arr);
    for (int i = 0; i < 8; i++)
    {
        if (i != 7)
        {
            byteArrStr += String(arr[i]);
            byteArrStr += ",";
        }
        else
        {
            byteArrStr += String(arr[i]);
        }
    }
    return base64::encode(byteArrStr);
}