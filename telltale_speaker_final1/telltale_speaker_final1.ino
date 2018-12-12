#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(7, 8); // CE, CSN
//left telltale, top speaker board
const byte address[6] = "00001"; 
const int pitch = 2000;
const int pitch_duration = 200;
const int speaker = 3; 
const int delay_gain = 100;

int status = 0;
int cur_delay = 0;
char read[2];

void setup() {
  pinMode(speaker, OUTPUT);
  Serial.begin(115200);
  Serial.println("Starting wireless communication...");
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}

void loop() {
  if (radio.available()) {
    radio.read(&read, sizeof(read));
    Serial.println(read[0]);
    status = (int)(read[0])-'0';
    Serial.print("Received: ");
    Serial.println(status);
    cur_delay = delay_gain*status;
  } 
  
  if (cur_delay){
    tone(speaker, pitch, pitch_duration);
    delay(cur_delay+pitch_duration);
    Serial.println("Beep!");
  }
}
