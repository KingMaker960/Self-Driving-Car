#define LEN 10
#define REN 9
#define RF 2
#define RB 3
#define LB 4
#define LF 5

#define TRIGGER 6
#define ECHO 7

long duration;  
int distance;
double kp = 2.368;   // Proportional gain
double ki = 0.01;   // Integral gain
double kd = 0;  // Derivative gain

int targetOffset = 0;  // Target offset from the center line
int currentOffset = 0; // Current offset from the center line

double integral = 0;
double lastError = 0;

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
    int currentOffset = masterData.toInt();
    if(currentOffset == 1000){
      Stop();                       //Traffic Light And Traffic Sign Detected
    }else{
      PIDController(currentOffset);
    }
      
  }
}
void PIDController(int currentOffset){
      // Calculate PID terms
    double error = targetOffset - currentOffset;
    integral += error;
    double derivative = error - lastError;
    
    // Calculate motor speeds using PID formula
    double output = kp * error + ki * integral + kd * derivative;

    // Adjust motor speeds based on the PID output
    int motorSpeedRight = constrain(150 - output, 0, 150);
    int motorSpeedLeft = constrain(150 + output, 0, 150);

    // Set motor speed and control logic
    analogWrite(LEN, motorSpeedLeft);
    analogWrite(REN, motorSpeedRight);
    MoveCar();
    
    // Update last error for the next iteration
    lastError = abs(error);

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
   
    Stop();     //Obstacle Detected
  }
}

void MoveCar(){
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
