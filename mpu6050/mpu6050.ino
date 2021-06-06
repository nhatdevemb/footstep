
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2); // Set I2C address to 0x27


long accelX, accelY, accelZ;
float gForceX, gForceY, gForceZ;

long gyroX, gyroY, gyroZ;
float rotX, rotY, rotZ;

unsigned long previousMillis=0;
const long interval = 1000;
long count=0;
unsigned long currentMillis ;
int flag=0;

byte customChar[] = {
  B00000,
  B00000,
  B00011,
  B00111,
  B11111,
  B11111,
  B00000,
  B00000
};
void setup() {
  lcd.begin();
  lcd.backlight();
  lcd.createChar(0, customChar);
  lcd.home();
  Serial.begin(9600);
  Wire.begin();
  setupMPU();
}


void loop() {
  recordAccelRegisters();
  recordGyroRegisters();
//  printData();
  stepCount();//dem buoc chan
  delay(100);
}

void setupMPU(){
  Wire.beginTransmission(0b1101000); //địa chỉ i2c của MPU (b1101000/b1101001 for AC0 low/high datasheet sec. 9.2)
  Wire.write(0x6B); //Accessing the register 6B - Power Management (Sec. 4.28)
  Wire.write(0b00000000); //chế độ ngủ (Required; see Note on p. 9)
  Wire.endTransmission();  
  Wire.beginTransmission(0b1101000); //I2C address of the MPU
  Wire.write(0x1B); //Accessing the register 1B - Gyroscope Configuration (Sec. 4.4) 
  Wire.write(0x00000000); //Đặt con quay hồi chuyển thành tỷ lệ đầy đủ +/- 250deg./s 
  Wire.endTransmission(); 
  Wire.beginTransmission(0b1101000); //I2C address of the MPU
  Wire.write(0x1C); //Truy cập thanh ghi 1C - Cấu hình gia tốc kế (Sec. 4.5) 
  Wire.write(0b00000000); //Đặt accel thành +/- 2g
  Wire.endTransmission(); 
}

void recordAccelRegisters() {
  Wire.beginTransmission(0b1101000); //I2C address of the MPU
  Wire.write(0x3B); //Bắt đầu đăng ký Accel Readings
  Wire.endTransmission();
  Wire.requestFrom(0b1101000,6); //Request Accel Registers (3B - 40)
  while(Wire.available() < 6);
  accelX = Wire.read()<<8|Wire.read(); //Store first two bytes into accelX
  accelY = Wire.read()<<8|Wire.read(); //Store middle two bytes into accelY
  accelZ = Wire.read()<<8|Wire.read(); //Store last two bytes into accelZ
  processAccelData();
}

void processAccelData(){
  gForceX = accelX / 16384.0;
  gForceY = accelY / 16384.0; 
  gForceZ = accelZ / 16384.0;
}

void recordGyroRegisters() {
  Wire.beginTransmission(0b1101000); //I2C address of the MPU
  Wire.write(0x43); //Starting register for Gyro Readings
  Wire.endTransmission();
  Wire.requestFrom(0b1101000,6); //Request Gyro Registers (43 - 48)
  while(Wire.available() < 6);
  gyroX = Wire.read()<<8|Wire.read(); //gia tốc trục X
  gyroY = Wire.read()<<8|Wire.read(); //gia tốc trục Y
  gyroZ = Wire.read()<<8|Wire.read(); //gia tốc trục Z
  processGyroData();
}

void processGyroData() {
  rotX = gyroX / 131.0;
  rotY = gyroY / 131.0; 
  rotZ = gyroZ / 131.0;
}

//void printData() {
//  Serial.print("Gyro (deg)");
//  Serial.print(" X=");
//  Serial.print(rotX);
//  Serial.print(" Y=");
//  Serial.print(rotY);
//  Serial.print(" Z=");
//  Serial.print(rotZ);
//  Serial.print(" Accel (g)");
//  Serial.print(" X=");
//  Serial.print(gForceX);
//  Serial.print(" Y=");
//  Serial.print(gForceY);
//  Serial.print(" Z=");
//  Serial.println(gForceZ);
//}
void stepCount(){
  if(gForceY>0.5)
  {
    flag = 1;
    Serial.print("inside greater, flag= ");
    Serial.println(flag);
    previousMillis=millis();
    currentMillis=millis();
  }

  
  if((currentMillis - previousMillis <=interval) &&(flag))
  {
    Serial.println("inside time loop");
    
    if(gForceY < -0.5)
    {
      count++;
      flag=0;
      Serial.print("inside lesser,flag= ");
      Serial.println(flag);
    }
  }
  currentMillis=millis();
  if(currentMillis - previousMillis >interval){
    flag=0;
    Serial.print("inside clear,flag= ");
    Serial.println(flag);
  }
  if(count>60)
  {
    count=0;
  }
  Serial.print("step= ");
   lcd.setCursor(0,0);
    lcd.print("CHAY NGAY DI!!! ");
  lcd.setCursor(0,1);
  lcd.print("So buoc chan: ");
  lcd.setCursor(0,2);
  lcd.write(0);
  lcd.print(" ");
  lcd.print(count);
  Serial.println(count);
  Serial.print( "Y= ");
  Serial.println(gForceY);
}
