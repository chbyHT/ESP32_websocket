// WebSocket server端 SAT模式

//#include <WiFiClient.h>

#include <WiFi.h>                       //引入函式庫
#include <WiFiClient.h>
#include <WebServer.h>
#include <WebSocketsServer.h>

const char* ssid = "WiFi SSID";    //定義要連接的WiFi及密碼
const char* password = "WiFi password";

const int LED_B = 12;  //藍LED               //設置開發版腳位及變數
const int LED_Y = 27;  //黃LED

String responds = "";

WebServer server(80);                               //WebServer port開在80
WebSocketsServer webSocket = WebSocketsServer(81);  //WebSocketsServer port開在81

char webpage[] PROGMEM = R"=====(
<html>
  <head>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8">
  </head>
  <script>
    var Socket;
    let responds = "";
    function init(){
      Socket = new WebSocket('ws://' + window.location.hostname + ':81/');
      Socket.onopen = function(){
        console.log("socket以連線");
      }
      Socket.onmessage = function(event){
        //document.getElementById("rxConsole").value += event.data;
        console.log(event.data)
        if(event.data == "open"){
           document.getElementById("ReedSwitch").innerHTML = "開起"
        }else if(event.data == "close"){
           document.getElementById("ReedSwitch").innerHTML = "關閉"
        }else if(event.data == "RLEDoff"){
           document.getElementById("R_LED").innerHTML = "關"
           document.getElementById("R_LED").style.backgroundColor="#DDD"
        }else if(event.data == "RLEDon"){
           document.getElementById("R_LED").innerHTML = "開"
           document.getElementById("R_LED").style.backgroundColor="#f08080"
        }else if(event.data == "GLEDoff"){
           document.getElementById("G_LED").innerHTML = "關"
           document.getElementById("G_LED").style.backgroundColor="#DDD"
        }else if(event.data == "GLEDon"){
           document.getElementById("G_LED").innerHTML = "開"
           document.getElementById("G_LED").style.backgroundColor="#90ee90"
        }else if(event.data == "OLEDoff"){
           document.getElementById("O_LED").innerHTML = "關"
           document.getElementById("O_LED").style.backgroundColor="#DDD"
        }else if(event.data == "OLEDon"){
           document.getElementById("O_LED").innerHTML = "開"
           document.getElementById("O_LED").style.backgroundColor="#ffa500"
        }else if(event.data == "WLEDoff"){
           document.getElementById("W_LED").innerHTML = "關"
           document.getElementById("W_LED").style.backgroundColor="#DDD"
        }else if(event.data == "WLEDon"){
           document.getElementById("W_LED").innerHTML = "開"
           document.getElementById("W_LED").style.backgroundColor="#FFF"
        }else if(event.data == "BLEDoff"){
           document.getElementById("B_LED").innerHTML = "關"
           document.getElementById("B_LED").style.backgroundColor="#DDD"
        }else if(event.data == "BLEDon"){
           document.getElementById("B_LED").innerHTML = "開"
           document.getElementById("B_LED").style.backgroundColor="#87cefa"
        }else if(event.data == "YLEDoff"){
           document.getElementById("Y_LED").innerHTML = "關"
           document.getElementById("Y_LED").style.backgroundColor="#DDD"
        }else if(event.data == "YLEDon"){
           document.getElementById("Y_LED").innerHTML = "開"
           document.getElementById("Y_LED").style.backgroundColor="#fefe22"
        }
      }
      Socket.onclose = function (event) { //socket端線時會執行
        console.log("socket以斷線!!");
        init();
        console.log("重新連線...");
      }
    }

    function sendText(){
      Socket.send(document.getElementById("txBar").value);
      document.getElementById("txBar").value = "";
    }


    function changeR(){
      Socket.send("changeR");    
    }
    function changeG(){
      Socket.send("changeG");    
    }
    function changeO(){
      Socket.send("changeO");    
    }
    function changeW(){
      Socket.send("changeW");   
    }
    function changeB(){
      Socket.send("changeB");   
    }
    function changeY(){
      Socket.send("changeY");    
    }

  </script>
  <body onload="javascript:init()">
    <div>
      <h2>ESP32 控制網頁</h2>
      <table style="border:3px #cccccc solid;" cellpadding="10" border='1'>
       <tr>
         <td>磁簧開關狀態：</td>
         <td><span id="ReedSwitch">關</span></td>
       </tr>
       <tr>
         <td>紅色LED狀態：</td>
         <td><button id="R_LED" onclick="changeR()">關</button></td>
       </tr>
       <tr>
         <td>綠色LED狀態：</td>
         <td><button id="G_LED" onclick="changeG()">關</button></td>
       </tr>
       <tr>
         <td>橘色LED狀態：</td>
         <td><button id="O_LED" onclick="changeO()">關</button></td>
       </tr>
       <tr>
         <td>白色LED狀態：</td>
         <td><button id="W_LED" onclick="changeW()">關</button></td>
       </tr>
       <tr>
         <td>藍色LED狀態：</td>
         <td><button id="B_LED" onclick="changeB()">關</button></td>
       </tr>
       <tr>
         <td>黃色LED狀態：</td>
         <td><button id="Y_LED" onclick="changeY()">關</button></td>
       </tr>
      </table>
    </div>
  </body>
</html>
)=====";


void setup(void) {
  pinMode(LED_BUILTIN, OUTPUT);  //設定LED腳位為輸出
  pinMode(LED_B, OUTPUT);
  pinMode(LED_Y, OUTPUT);
  digitalWrite(LED_BUILTIN, 0);
  digitalWrite(LED_B, 0);
  digitalWrite(LED_Y, 0);
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
  Serial.print("Connected to ");         //印出連接到的WiFi名稱及IP
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  server.on("/", []() {       //網頁路由為"/"時,顯示webpage網頁
    server.send(200, "text/html", webpage);
  });
  server.begin();
  webSocket.begin();                      //啟動WebSocketsServer
  webSocket.onEvent(webSocketEvevt);      //webSocket監聽webSocketEvevt函式
}

void loop(void) {
  webSocket.loop();
  server.handleClient();
  if(responds == "open"){       //如果socket收到字串為"open"
      digitalWrite(LED_B, 1);   //將藍色LED點亮
      digitalWrite(LED_Y, 0);   //將黃色LED熄滅
      responds = "";            //清空變數
  }else if(responds == "close"){
      digitalWrite(LED_B, 0);
      digitalWrite(LED_Y, 1);
      responds = "";
  }
}


void webSocketEvevt(uint8_t num, WStype_t type, uint8_t * payload, size_t length){  //webSocket監聽函示
  responds = "";              //清空webSocket監聽變數
  if(type == WStype_TEXT){    //如果收到的型態為純文字
    if(payload[0] == '#'){
      uint16_t brightness = (uint16_t) strtol((const char *) &payload[1], NULL, 10);
      brightness = 1024-brightness;
      //analogWrite(LED_G,brightness);
      Serial.print("Range = ");
      Serial.println(brightness);
    }
    else{
      for(int i=0; i<length; i++){    //for迴圈把收到的回應存到responds變數中
        Serial.print((char)payload[i]);
        responds += ((char) payload[i]);
      }
    }
    Serial.println();
    webSocket.broadcastTXT(responds);
  }else if(type == WStype_CONNECTED){
    Serial.println("Websocket 用戶已連接");
    IPAddress ip = webSocket.remoteIP(num);
    Serial.println(ip.toString());
  }else if(type == WStype_DISCONNECTED){
    Serial.println("Websocket 用戶已斷線");
    Serial.printf("[%u] Disconnected!\n", num);
  }
}
