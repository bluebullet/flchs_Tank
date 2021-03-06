/*
 Example sketch for the Xbox Wireless Reciver library - developed by Kristian Lauszus
 It supports up to four controllers wirelessly
 For more information see the blog post: http://blog.tkjelectronics.dk/2012/12/xbox-360-receiver-added-to-the-usb-host-library/ or
 send me an e-mail:  kristianl@tkjelectronics.com
 */

#include <XBOXRECV.h>

// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#include <SPI.h>
#endif

USB Usb;
XBOXRECV Xbox(&Usb);

/** ^^^^^^ DO NOT CHANGE ABOVE THIS ^^^^^^^ **/

/** Concept of Operation

L1 - 
L2 - 
R1 - Actuator Fine Adjust (In)
R2 - Actuator Fine Adjust (Out)
Left Hat - Drive Motor
Right Hat - Lift Motor (Up/Down)
L3 - 
R3 - 
Up - 
Down - 
Left - 
Right - 
A - Actuator Pre-set 1
B - Actuator Pre-set 2
X - Actuator Pre-set 3
Y - Actuator Pre-set 4

**/


/** pin definition **/
const int PWM_A   = 3,
          DIR_A   = 12,
          BRAKE_A = 9,
          PWM_B   = 11,
          DIR_B   = 13,
          BRAKE_B = 8,
          SNS_A   = A0,
          SNS_B   = A1;

/** variable definiton **/

bool L1_State = 0,
     L2_State = 0,
     R1_State = 0,
     R2_State = 0,
     L3_State = 0,
     R3_State = 0, 
     Up_State = 0,
     Down_State = 0,
     Left_State = 0,
     Right_State = 0,
     A_State = 0,
     B_State = 0,
     X_State = 0,
     Y_State = 0;

int  LeftHatX_Raw = 0,
     LeftHatY_Raw = 0,
     RightHatX_Raw = 0,
     RightHatY_Raw = 0;

int i = 0; // variable to xbox code counter

int left_x = 0,
    left_y = 0,
    right_x = 0,
    right_y = 0;

void setup() {
  Serial.begin(115200); // set up Serial library at 115200 bps
  
#if !defined(__MIPSEL__)
  while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif

  /** USB start error **/
  /** !!!!!!!!!!!!!!!!!!!!!! NEED ANOTHER ACTION OR INDUCTION !!!!!!!!!!!!!!!!!!!!!!!! **/
  if (Usb.Init() == -1) {
    Serial.print(F("\r\nOSC did not start"));
    while (1); //halt
  }
  
  Serial.print(F("\r\nXbox Wireless Receiver Library Started"));

  // Configure the A output
  pinMode(BRAKE_A, OUTPUT);  // Brake pin on channel A
  pinMode(DIR_A, OUTPUT);    // Direction pin on channel A
  pinMode(BRAKE_B, OUTPUT);  // Brake pin on channel B
  pinMode(DIR_B, OUTPUT);    // Direction pin on channel B
  
}

void loop() {
  
  /** Main loop logics:
  
  - Read USB Joystick
  - Update Motor Speed
  - Individual buttons action
  
  **/
  
  if (!readJoystickInput()) {
    Serial.println("USB Joystick Failed/Not Connected!");
  }
  
  updateMotorSpeed();
    
  
}

bool readJoystickInput() {
  /** Reading USB Value **/
  Usb.Task();
  if (Xbox.XboxReceiverConnected) {
    
    /** can connect up to 4 XBox controller **/
    for (uint8_t i = 0; i < 4; i++) {
      if (Xbox.Xbox360Connected[i]) {
        
        // use global variables to save reading to reduce process load
        // and allow all function to access the readings
        L1_State = Xbox.getButtonPress(L1, i);
        L2_State = Xbox.getButtonPress(L2, i);
        L3_State = Xbox.getButtonPress(L3, i);
        R1_State = Xbox.getButtonPress(R1, i);
        R2_State = Xbox.getButtonPress(R2, i);
        R3_State = Xbox.getButtonPress(R3, i);
        Up_State = Xbox.getButtonPress(UP, i);
        Down_State = Xbox.getButtonPress(DOWN, i);
        Left_State = Xbox.getButtonPress(LEFT, i);
        Right_State = Xbox.getButtonPress(RIGHT, i);
        A_State = Xbox.getButtonPress(A, i);
        B_State = Xbox.getButtonPress(B, i);
        X_State = Xbox.getButtonPress(X, i);
        Y_State = Xbox.getButtonPress(Y, i);
        LeftHatX_Raw = Xbox.getAnalogHat(LeftHatX, i);
        LeftHatY_Raw = Xbox.getAnalogHat(LeftHatY, i);
        RightHatX_Raw = Xbox.getAnalogHat(RightHatX, i);
        RightHatY_Raw = Xbox.getAnalogHat(RightHatY, i);
        
        // Adjust for null zone
        if (LeftHatX_Raw < 7500 && LeftHatX_Raw > -7500) LeftHatX_Raw = 0;
        if (LeftHatY_Raw < 7500 && LeftHatY_Raw > -7500) LeftHatY_Raw = 0;
        if (RightHatX_Raw < 7500 && RightHatX_Raw > -7500) RightHatX_Raw = 0;
        if (RightHatY_Raw < 7500 && RightHatY_Raw > -7500) RightHatY_Raw = 0;
        
        // debug printout
        /*
        Serial.print("LX:");
        Serial.print(LeftHatX_Raw);
        Serial.print(" LY:");
        Serial.print(LeftHatY_Raw);
        Serial.print(" RX:");
        Serial.print(RightHatX_Raw);
        Serial.print(" RY:");
        Serial.print(RightHatY_Raw);
        
        Serial.print(" L1:");
        Serial.print((L1_State) ? 1 : 0);
        Serial.print(" L2:");
        Serial.print((L2_State) ? 1 : 0);
        Serial.print(" L3:");
        Serial.print((L3_State) ? 1 : 0);
        Serial.print(" R1:");
        Serial.print((R1_State) ? 1 : 0);
        Serial.print(" R2:");
        Serial.print((R2_State) ? 1 : 0);
        Serial.print(" R3:");
        Serial.print((R3_State) ? 1 : 0);
        
        Serial.print(" U:");
        Serial.print((Up_State) ? 1 : 0);
        Serial.print(" D:");
        Serial.print((Down_State) ? 1 : 0);
        Serial.print(" L:");
        Serial.print((Left_State) ? 1 : 0);
        Serial.print(" R:");
        Serial.print((Right_State) ? 1 : 0);
        
        Serial.print(" A:");
        Serial.print((A_State) ? 1 : 0);
        Serial.print(" B:");
        Serial.print((B_State) ? 1 : 0);
        Serial.print(" X:");
        Serial.print((X_State) ? 1 : 0);
        Serial.print(" Y:");
        Serial.println((Y_State) ? 1 : 0);
        */
        // debug printout end
        
        /** Analog reading is from -32768 (left/down) to 32767 (right/up) **/
        /** convert raw value into useable value (-255 - +255) **/
        left_x = map(LeftHatX_Raw, -32768, 32767, -255, 255);
        left_y = map(LeftHatY_Raw, -32768, 32767, -255, 255);
        
        right_x = map(RightHatX_Raw, -32768, 32767, -255, 255);
        right_y = map(RightHatY_Raw, -32768, 32767, -255, 255);
      }
    }
    
    return true;
    
  } else {
    
    return false;
    
  }
}

void updateMotorSpeed() {
  
  static int old_speed_left,
             old_speed_right;

  /*
  Serial.print(" L X: ");
  Serial.print(left_x);
  Serial.print(" L Y: ");
  Serial.print(left_y);
  */

  /** mix x and y to get differential drive **/
  int speed_left_raw = left_y - left_x;
  int speed_right_raw = left_y + left_x;

  /** determine max value from calculation above **/
  int speed_scale = max(abs(speed_left_raw), abs(speed_right_raw));
  speed_scale = max(255, speed_scale);

  /*
  Serial.print(" L X: ");
  Serial.print(speed_left_raw);
  Serial.print(" L Y: ");
  Serial.print(speed_right_raw);
  
  Serial.print(" Scale: ");
  Serial.print(speed_scale);
  */

  int speed_left = (int) (float) speed_left_raw / (float) speed_scale * 255.0;
  int speed_right = (int) (float) speed_right_raw / (float) speed_scale * 255.0;

  speed_left = constrain(speed_left, -255, 255);
  speed_right = constrain(speed_right, -255, 255);

  /*
  Serial.print(" L Speed: ");
  Serial.print(speed_left);
  Serial.print(" R Speed: ");
  Serial.println(speed_right);
  */
  
  // update motor speed only if there are change
  if (speed_left != old_speed_left || speed_right != old_speed_right) {
    
    if (speed_left > 0) {
      digitalWrite(DIR_A, HIGH);
    } else {
      digitalWrite(DIR_A, LOW);
    }
    
    if (speed_right > 0) {
      digitalWrite(DIR_B, LOW);
    } else {
      digitalWrite(DIR_B, HIGH);
    }
    
    analogWrite(PWM_A, abs(speed_left));
    analogWrite(PWM_B, abs(speed_right));
  }
  
  old_speed_left = speed_left;
  old_speed_right = speed_right;
  
}
