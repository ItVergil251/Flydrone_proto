#include <NanitLib.h>
#include "nRF24L01.h"
#include "RF24.h"

RF24 radio(P7_4, P6_1);

#define AngleX P6_2  // Підключення піна для вимірювання кута X (Connection of the pin for measuring angle X)
#define AngleY P6_3  // Підключення піна для вимірювання кута Y (Connection of the pin for measuring angle Y)

#define DriveZ P7_1  // Підключення піна для управління осьовою швидкістю Z (Connection of the pin for controlling axial speed Z)
#define DriveR P7_2  // Підключення піна для управління обертовою швидкістю R (Connection of the pin for controlling rotational speed R)

#define BACKCOLOR 0x18E3
#define BARCOLOR 0x0620
#define SCALECOLOR 0xFFFF

byte address[6] = {"tyuio"};  //заданий номер труби(можна підправити на інший, головне щоб в обох було однаково)

uint8_t
  LastDriveZPercent = 0,
  LastDriveR = 0,
  LastAngleX = 0,
  LastAngleY = 0,
  dataTrans[4] = { 0 };


void setup(void) {
  Serial.begin(9600);
  Nanit_Base_Start();

  tft.fillScreen(BACKCOLOR);
  drawScale();
  tft.setTextSize(1.5);
  tft.setTextColor(ST7735_WHITE);
  tft.setCursor(0, 0);
  tft.print("Engine:");
  radio.begin();             //запуск модуля
  radio.setAutoAck(1);       //режим підтвердження прийому, 1 вкл 0 вмкн
  radio.setRetries(0,3);     //(час між спробою "достукатись", кількість спроб)
  radio.enableAckPayload();  //дозволити передачу даних у відповідь
  radio.setPayloadSize(4);   //розмір пакету в БАЙТАХ

  radio.openWritingPipe(address); //труба 0, відкриваємо канал для передачі даних
  radio.setChannel(0x60);         //обираємо канал (в якомму нема шумів!)

  radio.setPALevel (RF24_PA_MAX); //рівень потужності передатчика. На вибір RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setDataRate (RF24_1MBPS); //швидкість обміну. На вибір RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
  /*
  при нижчій швидкості маємо вищу чутливість та дальність!!
  мінімальна швидкість для Duplex обміну 1 МБ/с
  ПАРАМЕТРИ швидкості та передачі мають бути однакові на обох модулях
  */

  radio.powerUp(); //запуск модуля
  radio.stopListening();  //вимикаємо прослуховування ефіру, оскільки це передатчик

}

void loop() {
  uint8_t DriveZ_Percent, Angle_X, Angle_Y, Drive_R;
  Angle_X = map(analogRead(AngleX), 0, 1023, 0, 255);  // Вимірювання кута X та мапування значень (Measuring angle X and mapping values)
  Angle_Y = map(analogRead(AngleY), 0, 1023, 0, 255);  // Вимірювання кута Y та мапування значень (Measuring angle Y and mapping values)
  Drive_R = map(analogRead(DriveR), 0, 1023, 0, 255);
  DriveZ_Percent = int((analogRead(DriveZ) / 1024.0) * 100.0);

  if (DriveZ_Percent != LastDriveZPercent) {
    drawBar(DriveZ_Percent);
    LastDriveZPercent = DriveZ_Percent;
  }
  if (Angle_X != LastAngleX) {
    LastAngleX = Angle_X;
  }
  if (Angle_Y != LastAngleY) {
    LastAngleY = Angle_Y;
  }
  if (Drive_R != LastDriveR) {
    LastDriveR = Drive_R;
  }
  dataTrans[0] = map(analogRead(DriveZ),0,1023,0,255);
  dataTrans[1] = LastDriveR;
  dataTrans[2] = Angle_X;
  dataTrans[3] = Angle_Y;
  if(radio.write(&dataTrans,4)){
    Serial.println("Sent: " +  // виведення буферу для передачі
    String(dataTrans[0]) + " " + 
    String(dataTrans[1]) + " " + 
    String(dataTrans[2]) + " " + 
    String(dataTrans[3]));
    if (!radio.available()){Serial.print("NO Callback");}
    else
    {
      // while(radio.available()){
      //   // radio.read();
      // }
    }
    
  }
  else{Serial.println("Fail Transmit!!!");}
}

void drawScale() {
  tft.drawFastVLine(10, 20, 100, SCALECOLOR);  // Vertical Scale Line
  tft.drawFastHLine(7, 20, 8, SCALECOLOR);     // Major Division
  tft.drawFastHLine(10, 44, 5, SCALECOLOR);    // Minor Division
  tft.drawFastHLine(7, 69, 8, SCALECOLOR);     // Major Division
  tft.drawFastHLine(10, 94, 5, SCALECOLOR);    // Minor Division
  tft.drawFastHLine(7, 119, 8, SCALECOLOR);    // Major Division
}

void drawBar(int nPer) {
  if (nPer < LastDriveZPercent) {
    tft.fillRect(21, 20 + (100 - LastDriveZPercent), 20, LastDriveZPercent - nPer, BACKCOLOR);
  } else {
    tft.fillRect(21, 20 + (100 - nPer), 20, nPer - LastDriveZPercent, BARCOLOR);
  }
}
