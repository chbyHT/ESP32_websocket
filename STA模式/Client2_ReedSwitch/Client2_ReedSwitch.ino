//WebSocket client端 磁簧開關 STA模式

#include <WiFi.h>                     //引入函式庫
#include <WiFiClient.h>
#include <WebSocketsClient.h>

WebSocketsClient webSocket;

const char* ssid = "WiFi SSID";     //定義要連接的WiFi及密碼
const char* password = "WiFi password";

const int LED_R = 12;  //紅LED               //設置開發版腳位及變數
const int LED_G = 27;  //綠LED
const int BTN_O = 13;  //控制橘LED按鈕
const int BTN_W = 14;  //控制白LED按鈕
static int RledStatus; //紅LED狀態
static int GledStatus; //綠LED狀態

const int ReedSwitch = 36;  //磁簧開關
static int LastSwitchState = 0; //上次磁簧開關狀態

//蜂鳴器
int freq = 2000; //頻率   
int channel = 0; //頻道 
int resolution = 8; //分辨率
int speaker = 32;  //蜂鳴器腳位

String responds = "";

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {  //webSocket監聽函示
  responds = "";
  if(type == WStype_TEXT){
    for(int i=0; i<length; i++){
      Serial.print((char)payload[i]);
      responds += ((char) payload[i]);
    }
  }
  Serial.println();
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);     //定義腳位為輸出腳
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(BTN_O, INPUT);          //定義按鈕腳位為輸入腳
  pinMode(BTN_W, INPUT);
  pinMode(ReedSwitch, INPUT);
  digitalWrite(LED_BUILTIN, 0);
  digitalWrite(LED_R, 0);
  digitalWrite(LED_G, 0);
  ledcSetup(channel, freq, resolution);
  ledcAttachPin(speaker, channel);
  Serial.begin(115200);             //設定鮑率115200
  WiFi.mode(WIFI_STA);              //設定WiFi模式為STA
  WiFi.begin(ssid, password);       //連接至設定的WiFi
  Serial.println("");
  while (WiFi.status() != WL_CONNECTED) {   //等待WiFi連接
    delay(500);
    digitalWrite(LED_BUILTIN, 1);
    Serial.print(".");
    digitalWrite(LED_BUILTIN, 0);
  }
  Serial.println("");
  Serial.print("Connected to ");      //印出連接到的WiFi名稱及IP
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  webSocket.begin("192.168.4.1", 81, "/");  //webSocket連接至主機IP
  webSocket.onEvent(webSocketEvent);         //webSocket監聽webSocketEvevt函式
  webSocket.setReconnectInterval(5000);      //如果連接失敗，5秒後再次嘗試
}

void loop() {
  webSocket.loop();
  if(digitalRead(ReedSwitch) == HIGH ){   //如果磁簧開關打開
      ledcWrite(channel, 300);  //蜂鳴器發出300Hz
      delay(200);
      ledcWrite(channel, 6000); //蜂鳴器發出6000Hz
      delay(200);                                
  }
  if(digitalRead(ReedSwitch) == LOW ){    //如果磁簧開關關閉
      ledcWrite(channel, 0);    //蜂鳴器關閉
  }
  
  if(digitalRead(ReedSwitch) == HIGH && LastSwitchState == 0){   //如果磁簧開關打開 且 與上次狀態不同
      LastSwitchState = 1;                                       //改變上次狀態
      webSocket.sendTXT("open");                                 //socket傳送"open"
      delay(500);                                                //延遲500毫秒防彈跳
  }else if(digitalRead(ReedSwitch) == LOW && LastSwitchState == 1){
      LastSwitchState = 0;
      webSocket.sendTXT("close");
      delay(500);
  }else if(responds == "changeG"){    //如果socket收到字串為"changeO"
      GledStatus == HIGH ? webSocket.sendTXT("GLEDoff") : webSocket.sendTXT("GLEDon");
        //如果現在綠燈狀態為HIGH,socket傳送"GLEDoff",否則socket傳送"GLEDon"
      GledStatus = GledStatus == HIGH ? LOW : HIGH;
        //如果現在橘燈狀態(GledStatus)為HIGH,將GledStatus設為LOW,否則將GledStatus設為HIGH
      digitalWrite(LED_G, GledStatus); //接橘色LED輸出設為GledStatus
      responds = "";                   //清空變數
  }else if(responds == "changeR"){
      RledStatus == HIGH ? webSocket.sendTXT("RLEDoff") : webSocket.sendTXT("RLEDon");
      RledStatus = RledStatus == HIGH ? LOW : HIGH;
      digitalWrite(LED_R, RledStatus);
      responds = "";
  }else if(digitalRead(BTN_O) == HIGH){  //如果按鈕被按下(高電位)
      webSocket.sendTXT("changeO");      //socket傳送"changeR"
      delay(500);                        //延遲500毫秒防彈跳
  }else if(digitalRead(BTN_W) == HIGH){
      webSocket.sendTXT("changeW");
      delay(500);
  }

}
