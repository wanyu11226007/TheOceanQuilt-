/*
   互動裝置：SG90伺服馬達 + HC-SR04超音波感測器
   功能流程：
   1. 觀眾靠近50公分內，感測器偵測到距離
   2. 觸發馬達從0度轉到180度，再轉回0度，波動水面
   3. 觀眾離開（超過50公分），馬達停止
   4. 每200毫秒感測一次距離
*/


// 引入伺服馬達函式庫
#include <Servo.h>


// 定義腳位
const int trigPin = 9;    // HC-SR04 Trig 腳位
const int echoPin = 10;   // HC-SR04 Echo 腳位
const int servoPin = 6;   // SG90 伺服馬達訊號腳位


// 距離閾值（公分）
const int thresholdDistance = 50;


// 建立伺服馬達物件
Servo myServo;


// 記錄上次感測時間
unsigned long lastMeasureTime = 0;
const unsigned long measureInterval = 200; // 200毫秒


void setup() {
   // 設定序列埠，方便除錯
   Serial.begin(9600);


   // 設定超音波感測器腳位
   pinMode(trigPin, OUTPUT);
   pinMode(echoPin, INPUT);


   // 連接伺服馬達
   myServo.attach(servoPin);


   // 馬達初始位置設為0度
   myServo.write(0);
}


void loop() {
   // 每200毫秒感測一次
   if (millis() - lastMeasureTime >= measureInterval) {
       lastMeasureTime = millis();


       // 取得距離
       long distance = measureDistance();


       // 印出距離（除錯用）
       Serial.print("[HC-SR04] 距離: ");
       Serial.print(distance);
       Serial.println(" cm");


       // 如果觀眾靠近50公分內
       if (distance > 0 && distance <= thresholdDistance) {
           // 觸發馬達波動水面
           waveWater();
       } else {
           // 觀眾離開，馬達停止於0度
           myServo.write(0);
       }
   }
}


// 量測距離（回傳公分）
long measureDistance() {
   // 發出10微秒的高電位脈衝到Trig腳位
   digitalWrite(trigPin, LOW);
   delayMicroseconds(2);
   digitalWrite(trigPin, HIGH);
   delayMicroseconds(10);
   digitalWrite(trigPin, LOW);


   // 讀取Echo腳位的高電位持續時間（微秒）
   long duration = pulseIn(echoPin, HIGH, 30000); // 最多等30ms


   // 計算距離（聲速約340m/s）
   long distance = duration * 0.034 / 2;


   // 若無回應，回傳-1
   if (duration == 0) return -1;
   return distance;
}


// 馬達波動水面（0度→180度→0度）
void waveWater() {
   // 由0度轉到180度
   for (int pos = 0; pos <= 180; pos += 5) {
       myServo.write(pos);
       delay(10); // 控制馬達速度
   }
   // 再由180度轉回0度
   for (int pos = 180; pos >= 0; pos -= 5) {
       myServo.write(pos);
       delay(10);
   }
}


/*
   線路接法：
   1. HC-SR04超音波感測器
        VCC → Arduino 5V
        GND → Arduino GND
        Trig → Arduino D9
        Echo → Arduino D10


   2. SG90伺服馬達
        紅線(VCC) → Arduino 5V（或外部5V電源，建議外接）
        棕線(GND) → Arduino GND
        橙線(Signal) → Arduino D6


   3. 工具與材料
        - Arduino UNO 或相容板
        - HC-SR04超音波感測器
        - SG90伺服馬達
        - 杜邦線
        - 水容器（用於波動水面）
        - 投影機/光源（投射動態影像）
*/
