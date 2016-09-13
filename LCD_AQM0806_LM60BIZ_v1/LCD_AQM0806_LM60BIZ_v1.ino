/*
 AQM0806 LDC+ LM60BIZ with ATmega328P
*/
#include <Wire.h>

#define ADDR 0x3e

uint8_t CONTROLL[] = {
  0x00, // Command
  0x40, // Data
};

const int C_COMM = 0;
const int C_DATA = 1;

const int LINE = 2;
const int mVoutPin = 0;
String mBuff[LINE];

static uint32_t mTimerTmp;
int mTempNum=0;

//
uint8_t settings[] = {

  0b00111001, // 0x39 // [Function set] DL(4):1(8-bit) N(3):1(2-line) DH(2):0(5x8 dot) IS(0):1(extension)

  // Internal OSC frequency
  (0b0001 << 4) + 0b0100, // BS(3):0(1/5blas) F(210):(internal Freq:100)

  // Contrast set
  (0b0111 << 4) + 0b0100, // Contrast(3210):4

  // Power/ICON/Contrast control
  (0b0101 << 4) + 0b0110, // Ion(3):0(ICON:off) Bon(2):1(booster:on) C5C4(10):10(contrast set)

  // Follower control
  (0b0110 << 4) + 0b1100, // Fon(3):1(on) Rab(210):100

  // Display ON/OFF control
  (0b00001 << 3) + 0b111, // D(2):1(Display:ON) C(1):1(Cursor:ON) B(0):1(Cursor Blink:ON)
};

//
long convert_Map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
// reading LM60BIZ
int getTempNum(){
  int iRet=0;
  float fSen  = 0;
  unsigned long reading  = 0;   
  for (int i=0; i<10; i++) {
    int  iTmp = analogRead(mVoutPin);
    reading  += iTmp; 
    delay(100);
  }
  int SValue= reading / 10;
  int voltage=convert_Map(SValue, 0, 1000, 0,3300);  // V
  //int voltage=convert_Map(SValue, 0, 1000, 0, 5000);  // V
  int iTemp = (voltage - 424) / 6.25; //電圧値を温度に変換, offset=425
  iRet= iTemp;
  
  return iRet;  
}

//
void setup() {  
  Serial.begin(9600);
  Serial.println("# Start-setup");
  //Sensor
  pinMode(mVoutPin, INPUT);
//Wire.begin(4, 5); // (SDA,SCL) Default
  Wire.begin(); // (SDA,SCL)

  // LCD初期化
  write(C_COMM, settings, sizeof(settings));
}

//
void loop() {
    delay(100);
    if (millis() > mTimerTmp) {
        mTimerTmp = millis()+ 3000;    
        mTempNum= getTempNum();
        String line_1 ="";
        String line_2 ="";
        //Sensor
        String sTmp=String(mTempNum);
          char sResT[2+1];
          sprintf(sResT, "%02d", mTempNum);
Serial.print("tmp=");
Serial.println(sResT);
        //init
        for (int i = 0; i < LINE ; i++) {
          mBuff[i]="";
        }
        //disp
        line_1 = "Temp:";
        line_2 = sTmp+"C";
          mBuff[0]=line_1;
          mBuff[1]=line_2;
          for (int i = 0; i < LINE ; i++) {
              print2line();
//Serial.print("buf[");
//Serial.print(i);
//Serial.print("]=");
//Serial.println(buff[i]);
          }
    }  // if_timeOver
}

// print
void print2line() {
  uint8_t cmd[] = {0x01};
  write(C_COMM, cmd, sizeof(cmd));
  delay(1);

  for (int i = 0; i < LINE; i++) {
    uint8_t pos = 0x80 | i * 0x40;
    uint8_t cmd[] = {pos};
    write(C_COMM, cmd, sizeof(cmd));
    write(C_DATA, (uint8_t *)mBuff[i].c_str(), mBuff[i].length());
  }
}

//
void write(int type, uint8_t *data, size_t len) {
  for (int i=0; i < len; i++) {
    Wire.beginTransmission(ADDR);
    Wire.write(CONTROLL[type]);
    Wire.write(data[i]);
    Wire.endTransmission();
    delayMicroseconds(27);
  }
}




