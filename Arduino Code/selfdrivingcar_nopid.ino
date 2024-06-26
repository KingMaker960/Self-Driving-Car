                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                #define LEN 10
#define REN 9
#define RF 2
#define RB 3
#define LB 4
#define LF 5

#define TRIGGER 6
#define ECHO 7

long duration;  
int distance;

void setup() {
  Serial.begin(9600);
  pinMode(LEN, OUTPUT);
  pinMode(REN, OUTPUT);
  pinMode(RF, OUTPUT);
  pinMode(RB, OUTPUT);
  pinMode(LB, OUTPUT);
  pinMode(LF, OUTPUT);
  pinMode(TRIGGER,OUTPUT);  
  pinMode(ECHO,INPUT);  
}

void loop() {
 calculateDistance();
 delay(10);
}
void msgCheck(){
  if (Serial.available() > 0) {
    //Data from master device
    String masterData = Serial.readStringUntil('\n');
    int Result = masterData.toInt();
    Serial.println(Result);  
    if (Result >0 && Result <5){
      Forward();
    }else if (Result >=5 && Result <20){
      Right2();
    }else if (Result >20 && Result<63){
      Right3();
    }else if (Result <0 && Result >-5){
      Forward();
    } else if (Result <=-5 && Result >-20){
      Left2();
    } else if (Result <-20 && Result >-63){
      Left3();
    }else if(Result == 1000){                      //Stop Sign and Traffic Light Detetcted Detetcted 
      Stop();
    } 
  }
}

void calculateDistance(){
  digitalWrite(TRIGGER,HIGH);  
  delayMicroseconds(10);  
  digitalWrite(TRIGGER,LOW);  
  duration=pulseIn(ECHO,HIGH);  
  distance = duration*0.034/2;
  Serial.println(distance);
  if(distance>25){
      msgCheck();
  }else{
    Stop();
  }
}

void Forward(){
  analogWrite(LEN, 75);
  analogWrite(REN, 75);
  digitalWrite(RF, HIGH);
  digitalWrite(RB, LOW);
  digitalWrite(LF, HIGH);
  digitalWrite(LB, LOW);
}
void backward(){
  analogWrite(LEN, 80);
  analogWrite(REN, 80);
  digitalWrite(RF, LOW);
  digitalWrite(RB, HIGH);
  digitalWrite(LF, LOW);
  digitalWrite(LB, HIGH);
}
void Right1(){
  analogWrite(LEN, 85);
  analogWrite(REN, 0);
  digitalWrite(RF, HIGH);
  digitalWrite(RB, LOW);
  digitalWrite(LF, HIGH);
  digitalWrite(LB, LOW);
}
void Right2(){
  analogWrite(LEN, 110);
  analogWrite(REN, 0);
  digitalWrite(RF, HIGH);
  digitalWrite(RB, LOW);
  digitalWrite(LF, HIGH);
  digitalWrite(LB, LOW);
}
void Right3(){
  analogWrite(LEN, 140);
  analogWrite(REN, 0);
  digitalWrite(RF, HIGH);
  digitalWrite(RB, LOW);
  digitalWrite(LF, HIGH);
  digitalWrite(LB, LOW);
}
void Left1(){
  analogWrite(LEN, 0);
  analogWrite(REN, 85);
  digitalWrite(RF, HIGH);
  digitalWrite(RB, LOW);
  digitalWrite(LF, HIGH);
  digitalWrite(LB, LOW);
}
void Left2(){
  analogWrite(LEN, 0);
  analogWrite(REN, 110);
  digitalWrite(RF, HIGH);
  digitalWrite(RB, LOW);
  digitalWrite(LF, HIGH);
  digitalWrite(LB, LOW);
}
void Left3(){
  analogWrite(LEN, 0);
  analogWrite(REN, 130);
  digitalWrite(RF, HIGH);
  digitalWrite(RB, LOW);
  digitalWrite(LF, HIGH);
  digitalWrite(LB, LOW);
}
void Stop(){
  analogWrite(LEN, 0);
  analogWrite(REN, 0);
  digitalWrite(RF, LOW);
  digitalWrite(RB, LOW);
  digitalWrite(LF, LOW);
  digitalWrite(LB, LOW);
}
void END(){
  analogWrite(LEN, 0);
  analogWrite(REN, 0);
  digitalWrite(RF, LOW);
  digitalWrite(RB, LOW);
  digitalWrite(LF, LOW);
  digitalWrite(LB, LOW);
}
