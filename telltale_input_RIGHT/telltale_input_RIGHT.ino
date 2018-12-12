/*
 * Program that uses photogate to examine tell-tale
 */

#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(9, 10); // CE, CSN
const byte address[6] = "00010";


//---program consts---

//time
const int string_check_time = 1;
const int flow_check_time = 30;
const int base_delay = 5;
const int flow_check_delay = 0;

const int GATE_PIN = 6;
const int GATE_PIN_2 = 7;

const int max_when_testing = flow_check_time * 0.6; //TODO - set this !!
const int max_in_flow = min(max_when_testing, int(flow_check_time/string_check_time));
const int msg_max_val = 9;

//const int string_thresh = 20;
#define STRING_THRESH 0.2


//---program vars---
int num_string_seen = 0;
int num_loops = 0;


void setup() {
  //while(!Serial); // for flora
  //delay(500);
  
  
  num_string_seen = 0;
  num_loops = 0;

  pinMode(GATE_PIN, INPUT);
  pinMode(GATE_PIN_2, INPUT);
  Serial.begin(115200);       // for debugging

  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();

  
  
}

void loop() {
  // put your main code here, to run repeatedly:



  if(num_loops % string_check_time == 0){
    //check string state
    check_string();
  }

  if(num_loops == flow_check_time){
    //examine flow
    //Serial.println(num_string_seen);
    
    int flow_num = examine_flow();
    
    //send values
    send_out(flow_num);
    
    //reset vars
    num_string_seen = 0;
    num_loops = 0;
    delay(flow_check_delay);
    
  }
  num_loops++;
  delay(base_delay);

}

/*
 *Method to check if string crosses gate 
 */
void check_string(){
  int string_state = digitalRead(GATE_PIN);
  //Serial.println(string_state);
  
  if (string_state == 0){
    num_string_seen++;
    //Serial.println("Saw string!");
    
  }

  int bot_state = digitalRead(GATE_PIN_2);
  if (bot_state == 0){
    num_string_seen--;
    //Serial.println("string on bottom!");
    
  }
  
  //Serial.print("Counting string passes: ");
  //Serial.println(num_string_seen);
  
  return;
}

/*
 * Method to analyze what fraction of time string covered the gate
 */
int examine_flow(){
  double percent_seen = double(num_string_seen)/max_in_flow;
  Serial.print("Percent covered: ");
  printDouble(percent_seen, 100);

   //scale the value to communication scale
  int scaled_flow = int(percent_seen * msg_max_val);

  if(scaled_flow > msg_max_val){
    scaled_flow = msg_max_val;
  }
  if(scaled_flow < 0){
    scaled_flow = 0;
  }
  return scaled_flow;
  
}

/*
 * Method to send results to reciving device
 * #TODO - switch to bluetooth output
 */
void send_out(int msg){
  Serial.println(msg); //for debugging
 
  //send over radio
  char the_num = msg + '0';
  char text[] = {the_num};
  
    
  Serial.print("text is : ");
  Serial.println(text[0]);
  
  //Serial.println("trying to send");
  
  radio.write(&text, sizeof(text));
  
  //Serial.println("sent");
  

    
  return;
  
}



void printDouble( double val, unsigned int precision){
// prints val with number of decimal places determine by precision
// NOTE: precision is 1 followed by the number of zeros for the desired number of decimial places
// example: printDouble( 3.1415, 100); // prints 3.14 (two decimal places)

   Serial.print (int(val));  //prints the int part
   Serial.print("."); // print the decimal point
   unsigned int frac;
   if(val >= 0)
       frac = (val - int(val)) * precision;
   else
       frac = (int(val)- val ) * precision;
   Serial.println(frac,DEC) ;
} 
