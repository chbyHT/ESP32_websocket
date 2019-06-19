//WebSocket client端 LED STA模式

#include <WiFi.h>               //引入函式庫
#include <WiFiClient.h>
#include <WebSocketsClient.h>

WebSocketsClient webSocket;

const char* ssid = "WiFi SSID";     //定義要連接的WiFi及密碼
const char* password = "WiFi password";

const int LED_O = 12;  //橘LED               //設置開發版腳位及變數
const int LED_W = 27;  //白LED
const int BTN_R = 13;  //控制紅LED按鈕
const int BTN_G = 14;  //控制綠LED按鈕
static int OledStatus; //橘LED狀態
static int WledStatus; //白LED狀態

String responds = "";

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {   //webSocket監聽函示
  responds = "";
  if(type == WStype_TEXT){
    for(int i=0; i<length; i++){
      Serial.print((char)payload[i]);
      responds += ((char) payload[i]);      //將socket收到的字存到responds
    }
  }
  else if(type ==WStype_CONNECTED){
     Serial.printf("[WSc] Connected to url: %s\n", payload);
      webSocket.sendTXT("Client Connected~");
  }
  Serial.println();
}

void setup(){
  pinMode(LED_BUILTIN, OUTPUT);   //定義LED腳位為輸出腳
  pinMode(LED_O, OUTPUT);
  pinMode(LED_W, OUTPUT);
  pinMode(BTN_R, INPUT);          //定義按鈕腳位為輸入腳
  pinMode(BTN_G, INPUT);
  digitalWrite(LED_BUILTIN, 0);
  digitalWrite(LED_O, 0);
  digitalWrite(LED_W, 0);
  Serial.begin(115200);           //設定鮑率115200
  WiFi.mode(WIFI_STA);            //設定WiFi模式為STA
  WiFi.begin(ssid, password);     //連接至設定的WiFi
  Serial.println("");
  while (WiFi.status() != WL_CONNECTED) {   //等待WiFi連接
    digitalWrite(LED_BUILTIN, 1);
    delay(500);
    Serial.print(".");
    digitalWrite(LED_BUILTIN, 0);
  }
  Serial.println("");
  Serial.print("Connected to ");      //印出連接到的WiFi名稱及IP
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  webSocket.begin("192.168.4.1", 81);    //webSocket連接至server IP
  webSocket.onEvent(webSocketEvent);     //webSocket監聽webSocketEvevt函式
  webSocket.setReconnectInterval(5000);  //如0果連接失敗，5秒後再次嘗試
}

void loop() {
  webSocket.loop();

  if(responds == "changeO"){           //如果socket收到字串為"changeO"
      OledStatus == HIGH ? webSocket.sendTXT("OLEDoff") : webSocket.sendTXT("OLEDon");  
        //如果現在橘燈狀態為HIGH,socket傳送"OLEDoff",否則socket傳送"OLEDon"
      OledStatus = OledStatus == HIGH ? LOW : HIGH;
        //如果現在橘燈狀態(OledStatus)為HIGH,將OledStatus設為LOW,否則將OledStatus設為HIGH
      digitalWrite(LED_O, OledStatus); //接橘色LED輸出設為OledStatus
      responds = "";                   //清空變數
  }else if(responds == "changeW"){
      WledStatus == HIGH ? webSocket.sendTXT("WLEDoff") : webSocket.sendTXT("WLEDon");
      WledStatus = WledStatus == HIGH ? LOW : HIGH;
      digitalWrite(LED_W, WledStatus);
      responds = "";
  }else if(digitalRead(BTN_R) == HIGH){   //如果按鈕被按下(高電位)
      webSocket.sendTXT("changeR");       //socket傳送"changeR"
      delay(500);                         //延遲500毫秒防彈跳
  }else if(digitalRead(BTN_G) == HIGH){
      webSocket.sendTXT("changeG");
      delay(500);
  }
}
