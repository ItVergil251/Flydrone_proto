#include <SPI.h>      // бібліотека SPI потрібна обов'язково
#include "nRF24L01.h" // обидві бібліотеки необхідні для nRF24L01
#include "RF24.h"

RF24 radio(2,4); // піни для ESP NodeMCU; для ESP-12F можливо потрібно замінити

byte address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"}; //возможные номера труб

void setup(){
  Serial.begin(9600);        //порт для відслідкування даних в Serial
  radio.begin();             //запуск модуля
  radio.setAutoAck(1);       //режим підтвердження прийому, 1 вкл 0 вмкн
  radio.setRetries(0,3);     //(час між спробою "достукатись", кількість спроб)
  radio.enableAckPayload();  //дозволити передачу даних у відповідь
  radio.setPayloadSize(4);   //розмір пакету в БАЙТАХ

    radio.openReadingPipe(1, address[0]);   // хотим слушать трубу 0
  radio.setChannel(0x60);           //обираємо канал (немає бути шумів!)

  radio.setPALevel (RF24_PA_MAX); //рівень потужності передатчика. На вибір RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setDataRate (RF24_2MBPS); //швидкість обміну. На вибір RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
  /*
  при нижчій швидкості маємо вищу чутливість та дальність!!
  мінімальна швидкість для Duplex обміну 1 МБ/с
  ПАРАМЕТРИ швидкості та передачі мають бути однакові на обох модулях
  */
  
  radio.powerUp(); //почати роботу модуля
  radio.startListening();  //прослуховуємо ефір оскільки це приймач а не передатчик
}

void loop(void) {
    byte pipeNo, gotByte[4], courd[3]; //змінні для "труби" та буферів прийому та Сallback
    for(uint8_t i = 0;i<3;i++){courd[i]=random(0,255);} //рандомайзер щоб буфер Callback не був порожній                          
    
    /*
    Прийом даних працює наступним чином:
    1) Якщо дані не надходять, то нічого не відбувається і так в циклі;
    2) Дані, які приходять вони записуються в буфер, який виводиться в Serial Monitor
    3) Після запису отриманих даних надсилається готовий масив "Callback"
    */
    while(radio.available(&pipeNo)) // перевірка ефіру та регістр трубки
    {    
      radio.read( &gotByte, sizeof(gotByte) ); // читання даних та запис у буфер
      radio.writeAckPayload(pipeNo,&courd,3);  // надсилання "CallBack"
      Serial.println("Recieved: " +            // Виведення даних в Serial
      String(gotByte[0]) + " " +               
      String(gotByte[1]) + " " +               
      String(gotByte[2]) + " " +               
      String(gotByte[3]));                     
    }
}


