#include <NewPing.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_MotorShield.h>


// -----------------------------------------
// ULTRASONIC SENSOR (HC-SR04) CONFIGURATION
// -----------------------------------------

// CENTER SONAR PINS
#define TRIGGER_CENTER 4
#define ECHO_CENTER 5

// RIGHT SONAR PINS
#define TRIGGER_RIGHT 2
#define ECHO_RIGHT 3

// LEFT SONAR PINS
#define TRIGGER_LEFT 6
#define ECHO_LEFT 7

// SENSOR SETUP
#define MAX_DISTANCE 200
NewPing sonar_center(TRIGGER_CENTER, ECHO_CENTER, MAX_DISTANCE);
NewPing sonar_left(TRIGGER_LEFT, ECHO_LEFT, MAX_DISTANCE);
NewPing sonar_right(TRIGGER_RIGHT, ECHO_RIGHT, MAX_DISTANCE);

int distance_center = 100;
int distance_right = 100;
int distance_left = 100;


// -----------------------------------------
// LCD CONFIGURATION
// -----------------------------------------
LiquidCrystal_I2C lcd(0x3F, 16, 2);

// CUSTOM CHARACTERS
byte custom_char_robot[8] = {
  0b00000,
  0b10101,
  0b00100,
  0b00100,
  0b01110,
  0b11111,
  0b11111,
  0b10001
};

byte custom_char_heart[8] = {
  0b00000,
  0b01010,
  0b11111,
  0b11111,
  0b01110,
  0b00100,
  0b00000,
  0b00000
};

byte custom_char_arrow_up[8] = {
  0b00100,
  0b01110,
  0b10101,
  0b00100,
  0b00100,
  0b00100,
  0b00100,
  0b00100
};

byte custom_char_arrow_down[8] = {
  0b00100,
  0b00100,
  0b00100,
  0b00100,
  0b00100,
  0b10101,
  0b01110,
  0b00100
};

byte custom_char_arrow_right[8] = {
  0b00000,
  0b00100,
  0b00010,
  0b11111,
  0b00010,
  0b00100,
  0b00000,
  0b00000
};

byte custom_char_arrow_left[8] = {
  0b00000,
  0b00100,
  0b01000,
  0b11111,
  0b01000,
  0b00100,
  0b00000,
  0b00000
};


// -----------------------------------------
// BUZZER CONFIGURATION
// -----------------------------------------
#define BUZZER 8


// -----------------------------------------
// LIGHTS CONFIGURATION
// -----------------------------------------
#define LIGHTS_STRIP 9
#define LIGHTS_FR 10
#define LIGHTS_FL 11
#define LIGHTS_BR 12
#define LIGHTS_BL 13


// -----------------------------------------
// MOTOR CONFIGURATION
// -----------------------------------------
Adafruit_MotorShield AFMS = Adafruit_MotorShield();

Adafruit_DCMotor *motor_backleft = AFMS.getMotor(1);
Adafruit_DCMotor *motor_backright = AFMS.getMotor(2);
Adafruit_DCMotor *motor_frontleft = AFMS.getMotor(3);
Adafruit_DCMotor *motor_frontright = AFMS.getMotor(4);


// -----------------------------------------
// GENRAL CONFIGURATION
// -----------------------------------------
enum robot_states {
  CONTROLLED,
  AUTONOMOUS,
  STANDBY
};

enum robot_states state = STANDBY;

char command = 0;
char drive_control = 0;

boolean input_received = false;
boolean lcd_backlight = true;
boolean lights_strip = false;
boolean lights_front = false;
boolean lights_back = false;

String primary_display_output = "";


// -----------------------------------------
// SETUP FUNCTION
// -----------------------------------------
void setup() {
  Serial.begin(9600);
  
  // MOTOR SETUP
  AFMS.begin();
  setSpeed(200);
  motor_backleft->run(RELEASE);
  motor_backright->run(RELEASE);
  motor_frontleft->run(RELEASE);
  motor_frontright->run(RELEASE);

  // BUZZER SETUP
  pinMode(BUZZER, OUTPUT);

  // LIGHTS SETUP
  pinMode(LIGHTS_STRIP, OUTPUT);
  pinMode(LIGHTS_FR, OUTPUT);
  pinMode(LIGHTS_FL, OUTPUT);
  pinMode(LIGHTS_BR, OUTPUT);
  pinMode(LIGHTS_BL, OUTPUT);

  // LCD SETUP - with custom greeting
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, custom_char_robot);
  lcd.createChar(1, custom_char_heart);
  lcd.createChar(2, custom_char_arrow_up);
  lcd.createChar(3, custom_char_arrow_down);
  lcd.createChar(4, custom_char_arrow_right);
  lcd.createChar(5, custom_char_arrow_left);
  lcd.setCursor(0, 0);
  lcd.print("Hello!");
  lcd.setCursor(7, 0);
  lcd.write((byte)1);
  lcd.setCursor(8, 0);
  lcd.write((byte)1);
  lcd.setCursor(9, 0);
  lcd.write((byte)1);
  lcd.setCursor(0, 1);
  lcd.print("I am 'CA-21'");
  lcd.setCursor(13, 1);
  lcd.write((byte)0);
  delay(5000);
  setPrimaryDisplayOutput("Standing By...");
}


// -----------------------------------------
// MAIN FUNCTION
// -----------------------------------------
void loop() {
  if(Serial.available() > 0) {
    command = Serial.read();
    
    switch(command) {
      case '1': // STATE: CONTROLLED
        stopMoving();
        drive_control = 0;
        setPrimaryDisplayOutput("CONTROLLED");
        state = CONTROLLED;
        break;
      case '2': // STATE: AUTONMOUS - 5 seconds countdown to position robot or cancel autonomous driving if necessary
        stopMoving();
        lcd.clear();
        lcd.home();
        lcd.print("Autonomous in...");
        lcd.setCursor(0, 1); 
        lcd.print("5 seconds");  
        delay(1000);
        lcd.setCursor(0, 1); 
        lcd.print("4 seconds");
        delay(1000);
        lcd.setCursor(0, 1); 
        lcd.print("3 seconds");
        delay(1000);
        lcd.setCursor(0, 1); 
        lcd.print("2 seconds");
        delay(1000);
        lcd.setCursor(0, 1); 
        lcd.print("1 second");
        delay(1000);
        setPrimaryDisplayOutput("AUTONOMOUS");
        state = AUTONOMOUS;
        break;
      case '3': // STATE: STANDBY
        setPrimaryDisplayOutput("Standing By...");
        state = STANDBY;
        break;
      case '6': // LCD BACKLIGHT CONTROL
        if(lcd_backlight) {
          lcd_backlight = false;
          lcd.noBacklight();
        } else {
          lcd_backlight = true;
          lcd.backlight();
        }
        break;
      case '7': // LIGHTS STRIP CONTROL
        if(lights_strip) {
          lights_strip = false;
          digitalWrite(LIGHTS_STRIP, LOW);
        } else {
          lights_strip = true;
          digitalWrite(LIGHTS_STRIP, HIGH);
        }
        break;
      case '8': // LIGHTS FRONT CONTROL
        if(lights_front) {
          lights_front = false;
          digitalWrite(LIGHTS_FR, LOW);
          digitalWrite(LIGHTS_FL, LOW);
        } else {
          lights_front = true;
          digitalWrite(LIGHTS_FR, HIGH);
          digitalWrite(LIGHTS_FL, HIGH);
        }
        break;
      case '9': // LIGHTS BACK CONTROL
        if(lights_back) {
          lights_back = false;
          digitalWrite(LIGHTS_BR, LOW);
          digitalWrite(LIGHTS_BL, LOW);
        } else {
          lights_back = true;
          digitalWrite(LIGHTS_BR, HIGH);
          digitalWrite(LIGHTS_BL, HIGH);
        }
        break;
      case '0': // BUZZER CONTROL
        for(int i = 1; i <= 150; i++) {
          digitalWrite(BUZZER, HIGH);
          delay(1);
          digitalWrite(BUZZER, LOW);
          delay(1);
        }
        break;
      default:
        input_received = true;
        drive_control = command;
    }
  }

  switch(state) {
    case CONTROLLED:
      if(input_received) {
        switch(drive_control) {
          case '1':
            setSpeed(40);
            break;
          case '2':
            setSpeed(80);
            break;
          case '3':
            setSpeed(100);
            break;
          case '4':
            setSpeed(120);
            break;
          case '5':
            setSpeed(140);
            break;
          case '6':
            setSpeed(160);
            break;
          case '7':
            setSpeed(180);
            break;
          case '8':
            setSpeed(200);
            break;
          case '9':
            setSpeed(220);
            break;
          case '0':
            setSpeed(255);
            break;
          case 'w':
            driveForward();
            break;
          case 's':
            driveBackward();
            break;
          case 'd':
            turnRight(0);
            break;
          case 'a':
            turnLeft(0);
            break;
          case 'q':
            stopMoving();
            break;
          default:
            stopMoving();
        }
      }
      input_received = false;
      break;
    case AUTONOMOUS:
      delay(200);
      distance_center = sonar_center.ping_cm();
      distance_right = sonar_right.ping_cm();
      distance_left = sonar_left.ping_cm();
      if((distance_center < 30) && (distance_center != NO_ECHO)) {
        if((distance_right < 30) && (distance_right != NO_ECHO)) {
          if((distance_left < 30) && (distance_left != NO_ECHO)) {
            stopMoving();
          } else {
            turnLeft(1000);
          }
        } else {
          turnRight(1000);
        }
      } else {
        if((distance_right < 30) && (distance_right != NO_ECHO)) {
          if((distance_left < 30) && (distance_left != NO_ECHO)) {
            stopMoving();
          } else {
            turnLeft(500);
          } 
        } else {
          if((distance_left < 30) && (distance_left != NO_ECHO)) {
            turnRight(500);   
          } else {
            driveForward();
          }
        }
      }
      break;
    case STANDBY:
      stopMoving();
      break;
    default:
      setPrimaryDisplayOutput("Standing By...");
      stopMoving();
  } 
}


// -----------------------------------------
// DISPLAY CONTROL FUNCTION
// -----------------------------------------
void setPrimaryDisplayOutput(String text) {
  if(!primary_display_output.equals(text)) {
    primary_display_output = text;
    lcd.clear();
    lcd.home();
    lcd.print(primary_display_output);
  }
}


// -----------------------------------------
// MOTOR CONTROL FUNCTIONS
// -----------------------------------------
void setSpeed(int new_speed) {
  motor_backleft->setSpeed(new_speed);
  motor_backright->setSpeed(new_speed);
  motor_frontleft->setSpeed(new_speed);
  motor_frontright->setSpeed(new_speed);
}

void driveForward() {
  lcd.setCursor(0, 1); 
  lcd.write((byte)2);
  lcd.setCursor(2, 1); 
  lcd.print("Go Forward    ");
  motor_backleft->run(FORWARD);
  motor_backright->run(FORWARD);
  motor_frontleft->run(FORWARD);
  motor_frontright->run(FORWARD);
}

void driveBackward() {
  lcd.setCursor(0, 1); 
  lcd.write((byte)3);
  lcd.setCursor(2, 1); 
  lcd.print("Go Backward   ");
  motor_backleft->run(BACKWARD);
  motor_backright->run(BACKWARD);
  motor_frontleft->run(BACKWARD);
  motor_frontright->run(BACKWARD); 
}

void turnRight(int duration) {
  lcd.setCursor(0, 1); 
  lcd.write((byte)4);
  lcd.setCursor(2, 1); 
  lcd.print("Go Right      ");
  motor_backleft->run(FORWARD);
  motor_backright->run(BACKWARD);
  motor_frontleft->run(FORWARD);
  motor_frontright->run(BACKWARD);
  delay(duration);
}

void turnLeft(int duration) {
  lcd.setCursor(0, 1); 
  lcd.write((byte)5);
  lcd.setCursor(2, 1); 
  lcd.print("Go Left       ");
  motor_backleft->run(BACKWARD);
  motor_backright->run(FORWARD);
  motor_frontleft->run(BACKWARD);
  motor_frontright->run(FORWARD);
  delay(duration);
}

void stopMoving() {
  lcd.setCursor(0, 1); 
  lcd.print("                ");
  motor_backleft->run(RELEASE);
  motor_backright->run(RELEASE);
  motor_frontleft->run(RELEASE);
  motor_frontright->run(RELEASE);
}
