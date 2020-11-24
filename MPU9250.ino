#include "MPU9250.h"
//#include <SD.h> //this is an arduiono library and it sucks
#include <SdFat.h>
SdFat SD;

#define USESD

#define FILE_BASE "T_"
#define FILE_EXT ".LOG"
#define BUF_SIZE 240
File dataFile;
char fileName[11];
char fileNum[3];
uint8_t buf1[BUF_SIZE];
int nBuf1 = 0;

const int SD_CS = 10;

#define SPIspeed 1000000
#define SPIport SPI
#define IMU_CS 9
#define MPU9250_ADDRESS 9001

long timeCount = 0;
long now;
uint8_t rawData[6];
MPU9250 myIMU(IMU_CS, SPIport, SPIspeed);

/**BEGIN SETUP*****************/
void setup()
{

  Serial.begin(38400);
  while (!Serial)
  {
  };

  SPIport.begin();

#ifdef USESD
  Serial.print("Initializing SD card...");

  pinMode(SD_CS, OUTPUT);

  if (!SD.begin(SD_CS))
  {
    Serial.println("initialization failed!");
    while (1) //wait for SD to initialize
      ;
    return;
  }
  Serial.println("initialization done.");
  for (int i = 0; i < 1000; i++) //max 1000 files on SD
  {

    strcpy(fileName, FILE_BASE);
    snprintf(fileNum, sizeof(fileNum), "%d", i);
    strcat(fileName, fileNum);
    strcat(fileName, FILE_EXT);
    Serial.println(fileName);
    if (!SD.exists(fileName))
    {
      dataFile = SD.open(fileName, O_WRITE | O_CREAT);
      i = 1000;
    }
  }

#endif
  delay(100);
  byte c = myIMU.readByte(MPU9250_ADDRESS, WHO_AM_I_MPU9250);
  Serial.print(F("MPU9250 I AM 0x"));
  Serial.print(c, HEX);
  Serial.print(F(" I should be 0x"));
  Serial.println(0x73, HEX);
  if (!(c == 0x71 || c == 0x73))
  {
    Serial.print("Could not connect to MPU9250: 0x");
    Serial.println(c, HEX);
    Serial.println(F("Communication failed, abort!"));
    Serial.flush();
    abort();
  }
  Serial.println(F("MPU9250 is online..."));
  myIMU.calibrateMPU9250(myIMU.accelBias);
  myIMU.initMPU9250();
  Serial.println("MPU9250 initialized for active data mode....");
  myIMU.getAres();
#ifdef PRINT_CONFIGS
  Serial.println("CONFIGURATION REGISTERS");
  c = myIMU.readByte(MPU9250_ADDRESS, PWR_MGMT_1);
  Serial.print(F("PWR_MGMT_1 "));
  Serial.println(c, HEX);

  c = myIMU.readByte(MPU9250_ADDRESS, PWR_MGMT_2);
  Serial.print(F("PWR_MGMT_2  "));
  Serial.println(c, HEX);

  c = myIMU.readByte(MPU9250_ADDRESS, CONFIG);
  Serial.print(F("CONFIG  "));
  Serial.println(c, HEX);

  c = myIMU.readByte(MPU9250_ADDRESS, SMPLRT_DIV);
  Serial.print(F("SMPLRT_DIV  "));
  Serial.println(c, HEX);

  c = myIMU.readByte(MPU9250_ADDRESS, GYRO_CONFIG);
  Serial.print(F("GYRO_CONFIG  "));
  Serial.println(c, HEX);

  c = myIMU.readByte(MPU9250_ADDRESS, ACCEL_CONFIG);
  Serial.print(F("ACCEL_CONFIG  "));
  Serial.println(c, HEX);

  c = myIMU.readByte(MPU9250_ADDRESS, ACCEL_CONFIG2);
  Serial.print(F("ACCEL_CONFIG2  "));
  Serial.println(c, HEX);

  c = myIMU.readByte(MPU9250_ADDRESS, INT_PIN_CFG);
  Serial.print(F("INT_PIN_CFG  "));
  Serial.println(c, HEX);

  c = myIMU.readByte(MPU9250_ADDRESS, INT_ENABLE);
  Serial.print(F("INT_ENABLE  "));
  Serial.println(c, HEX);

  Serial.println("ACCEL BIAS");
  Serial.print(myIMU.accelBias[0]);
  Serial.print('\t');
  Serial.print(myIMU.accelBias[1]);
  Serial.print('\t');
  Serial.println(myIMU.accelBias[2]);
  //dataFile.write(myIMU.accelBias,12);
#endif
}
/**END SETUP*****************/
/**BEGIN LOOP****************/
void loop()
{
  //ensure SD buffer is flushing at regular intervals (not sure if this *needs* to be done)
  if (millis() - timeCount >= 10000)
  {
    timeCount = millis();
    dataFile.flush();
  }

  //read IMU acceleration bytes
  if (myIMU.readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01)
  {
    myIMU.readBytes(myIMU._I2Caddr, ACCEL_XOUT_H, 6, &rawData[0]);

#ifdef USESD
    //put IMU bytes in buffer
    now = micros();
    buf1[nBuf1] = (byte)now & 0xFF;
    buf1[nBuf1 + 1] = (byte)(now >> 8) & 0xFF;
    buf1[nBuf1 + 2] = (byte)(now >> 16) & 0xFF;
    buf1[nBuf1 + 3] = (byte)(now >> 24) & 0xFF;
    nBuf1 += 4;
    buf1[nBuf1] = rawData[1];
    buf1[nBuf1 + 1] = rawData[0];
    buf1[nBuf1 + 2] = rawData[3];
    buf1[nBuf1 + 3] = rawData[2];
    buf1[nBuf1 + 4] = rawData[5];
    buf1[nBuf1 + 5] = rawData[4];
    nBuf1 += 6;

    //write buffer to SD card
    if (nBuf1 == BUF_SIZE)
    {
      if (BUF_SIZE != dataFile.write(buf1, BUF_SIZE))
      {
        Serial.println("WRITE ERROR");
      }
      nBuf1 = 0;
    }
#endif
  } // if (readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01)
} /**END LOOP*****************/
