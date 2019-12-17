/******************************************************************************
* SPI Communication between PGA411-Q1EVM and Arduino Uno board to aquire Angle 
  and Velocity readings via a Motor resolver 
* Author: Ranitha Mataraarachchi [ranitha@ieee.org]
* Date:   17/12/2019
*****************************************************************************/



#include <SPI.h>
#include <Stdio.h>

//DECLARE THE FUNCTIONS
void diag();
void edev();
void erom();
void ddiag();
void getAngle();
void getVelocity();
void setDefault();
unsigned int crcCal(unsigned long datin);

/////////////////////////////////////////////////////////////////////////////////////////////


const int ncs = 10;         //Chip select pin
const int FAULTRES = 3;     //FAULTRES pin
const int NRESET = 4;       //NRESET pin
unsigned int ang = 0;       //Absolute value of Angle
int vel = 0;                //Velocity calculation variable 
double angle = 0.0;         //Float value of Angle
int velocity = 0;           //Velocity
unsigned long frame32 = 0;  //32b dataframe for CRC calculation 
unsigned int scrc = 0;      //Slave's crc code 
unsigned int getcrc = 0;    //Calculated crc code from received data 


/////////////////////////////////////////////////////////////////////////////////////////////



void setup() {

  
  //START THE SERIAL DATA TRANSFER
  Serial.begin(115200);

  Serial.println("*****************************************");

  Serial.println("PROGRAM START");

  //INITIALIZE PINS
  pinMode(ncs, OUTPUT);     //Chip select pin is an output
  digitalWrite(ncs, HIGH);  //Set it high to ensure no data is transferred at the beginning
  pinMode(FAULTRES, OUTPUT);//FAULTRES pin is an output
  pinMode(NRESET, OUTPUT);  //NRESET pin is an output
  Serial.println("PINS SET");



  //START THE SPI LIBRARY
  SPI.begin();


  //NRESET MUST GO FROM LOW TO HIGH 
  digitalWrite(NRESET, LOW);
  delay(5);                //Wait some time
  digitalWrite(NRESET, HIGH);
  Serial.println("RESET STATE");
  
  //WAIT AT LEAST 5.2ms TO COMPLETE DIAGNOSTICS TEST
  delay(10);

 


  //INITIALIZE THE DATA TRANSFER
  diag();                   //Enter the Diagnostics mode
  edev();                  //Unlock the device
  erom();                  //Unlock the EEPROM
  setDefault();            //Set to Default settings
  ddiag();                 //Return from the Diagnostics mode

  //FAULT RESET PROCEDURE
  digitalWrite(FAULTRES, LOW);
  delay(1);
  digitalWrite(FAULTRES, HIGH);
  Serial.println("FAULTS RESET");

  //GIVE THE SENSOR SOME TIME TO SETUP
  delay(10);

  Serial.println("DEVICE IS READY TO COMMUNICATE ANGLE/VELOCITY VALUES...");
  Serial.println("*****************************************");
  Serial.println("\n");
  Serial.println("ANGLE \t VELOCITY");

}


void loop() {

  getAngle();                //Get Angle Value and Print it
  Serial.print("\t");
  getVelocity();            //Get Velocity value and Print it
  
}

/////////////////////////////////////////////////////////////////////////////////////////////


//ENTER THE DIAGNOSTICS MODE
void diag() {

  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE1));    //Begin the SPI transaction according to pga411q1 datasheet (8MHz clock, MSBFIRST, SPI_MODE1)
  digitalWrite(ncs, LOW);    //Begin SPI communication

  //ENABLE DIAGNOSTICS
  //0xAE000426
  SPI.transfer(0xAE);
  SPI.transfer(0x00);
  SPI.transfer(0x04);
  SPI.transfer(0x26);

  delay(1);                  //Wait at least 1ms between two NCS transitions
  digitalWrite(ncs, HIGH);   //End SPI communication
  Serial.println("DIAGNOSTICS STATE");


}


//UNLOCK THE DEVICE
void edev() {

  digitalWrite(ncs, LOW);    //Begin SPI communication

  //UNLOCK DEVICE
  //0x56000F13 ;
  SPI.transfer(0x56);
  SPI.transfer(0x00);
  SPI.transfer(0x0F);
  SPI.transfer(0x13);

  //0x56005508 ;
  SPI.transfer(0x56);
  SPI.transfer(0x00);
  SPI.transfer(0x55);
  SPI.transfer(0x08);

  //0x5600AA0F ;
  SPI.transfer(0x56);
  SPI.transfer(0x00);
  SPI.transfer(0xAA);
  SPI.transfer(0x0F);

  //0x5600F014 ;
  SPI.transfer(0x56);
  SPI.transfer(0x00);
  SPI.transfer(0xF0);
  SPI.transfer(0x14);

  delay(1);                  //Wait at least 1ms between two NCS transitions

  digitalWrite(ncs, HIGH);    //End SPI communication

  Serial.println("DEVICE UNLOCKED");
  
}


//UNLOCK THE EEPROM
void erom() {

  digitalWrite(ncs, LOW);     //Begin SPI communication

  //UNLOCK EEPROM 
  //0x95000F39 ;
  SPI.transfer(0x95);
  SPI.transfer(0x00);
  SPI.transfer(0x0F);
  SPI.transfer(0x39);

  //0x95005522 ;
  SPI.transfer(0x95);
  SPI.transfer(0x00);
  SPI.transfer(0x55);
  SPI.transfer(0x22);

  //0x9500AA25 ;
  SPI.transfer(0x95);
  SPI.transfer(0x00);
  SPI.transfer(0xAA);
  SPI.transfer(0x25);

  //0x9500F03E ;
  SPI.transfer(0x95);
  SPI.transfer(0x00);
  SPI.transfer(0xF0);
  SPI.transfer(0x3E);

  delay(1);                  //Wait at least 1ms between two NCS transitions

  digitalWrite(ncs, HIGH);    //End SPI communication

  Serial.println("EEPROM UNLOCKED");

}


//RETURN FROM THE DIAGNOSTICS MODE TO NORMAL MODE
void ddiag() {

  digitalWrite(ncs, LOW);     //Begin SPI communication

  //DISABLE DIAG
  //0x0D000127  
  SPI.transfer(0x0D);
  SPI.transfer(0x00);
  SPI.transfer(0x01);
  SPI.transfer(0x27);

  delay(1);                  //Wait at least 1ms between two NCS transitions

  digitalWrite(ncs, HIGH);    //End SPI communication
  
  Serial.println("NORMAL STATE");

}


//GET THE ANGLE VALUE
void getAngle() {
  
  digitalWrite(ncs, LOW);     //Begin SPI communication

  //DATA TRANSFER PHASE 1 - FIRST READ IS IGNORED
  SPI.transfer(0x41);
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  SPI.transfer(0x29);
  delay(1);                  //Wait at least 1ms between two NCS transitions

  
  digitalWrite(ncs, HIGH);    //Transition of ncs from HIGH to LOW as pga411q1 does not support back to back SPI com.
  delay(1);                  //Wait at least 1ms between two NCS transitions
  digitalWrite(ncs, LOW);

  //DATA TRANSFER PHASE 2 - SECOND READ GETS VALUES
  frame32 = SPI.transfer(0x41);
  frame32 = frame32 << 16;

  ang = SPI.transfer(0x00);   //Only the middle 2bytes contain info. Get the fist byte into int ang 
  ang = ang << 8;             //Shift the byte left by 8bits
  ang |= SPI.transfer(0x00);  //Bitwise XOR the next byte into int ang
  frame32 |= ang;
  frame32 = frame32 << 8;
  scrc = SPI.transfer(0x29);
  frame32 |= scrc;
  
  delay(1);                  //Wait at least 1ms between two NCS transitions
  digitalWrite(ncs, HIGH);    //End SPI communication


  //12b ANGLE CALCULATION
  ang &= 0x1FFF;              //Mask to obtain the last 13 bits 

  angle = (ang * 360.0) / 4096;   //Store in a float variable to retain the decimal values. 12bit calculation in datasheet.
  ang=0;

  getcrc = crcCal(frame32);
  
  if(getcrc==scrc){
  Serial.print(angle);
  }
}



//GET THE VELOCITY VALUE
void getVelocity() {
  
  
  digitalWrite(ncs, LOW);     //Begin SPI communication

  //DATA TRANSFER PHASE 1 - FIRST READ IS IGNORED
  SPI.transfer(0x6F);
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  SPI.transfer(0x03);
  delay(1);                  //Wait at least 1ms between two NCS transitions

  digitalWrite(ncs, HIGH);    //Transition of ncs from HIGH to LOW as pga411q1 does not support back to back SPI com.
  delay(1);                  //Wait at least 1ms between two NCS transitions
  digitalWrite(ncs, LOW);

  //DATA TRANSFER PHASE 2 - SECOND READ GETS VALUES
  frame32 = SPI.transfer(0x6F);
  frame32 = frame32 << 16;       

  vel = SPI.transfer(0x00);   //Only the middle 2bytes contain info. Get the fist byte into int vel 
  vel = vel << 8;             //Shift the byte left by 8bits
  vel |= SPI.transfer(0x00);  //Bitwise XOR the next byte into int vel
  frame32 |= vel;
  frame32 = frame32 << 8;
  scrc = SPI.transfer(0x03);
  frame32 |= scrc;
   
  delay(1);                  //Wait at least 1ms between two NCS transitions
  digitalWrite(ncs, HIGH);    //End SPI communication


  //12b VELOCITY CALCULATION. THIS FUNCTION IS GIVEN IN THE DATASHEET
  if (vel & 0x0800) /* negative number ? */ {
    /* convert to positive number first */
    vel = (((~vel) + 1) & 0x07FF);
    vel *= -1; /* and make it negative */
  } else {
    /* positive number, preserve only needed bits */
    vel = (vel & 0x07FF);
  }

  vel = vel + 1;

  velocity = vel * (60 * 20000000 / 33554432);  //20MHz is the device clock. 
  vel = 0;

  getcrc = crcCal(frame32);
  
  if (getcrc==scrc){
  Serial.println(velocity);
  }
}


/////////////////////////////////////////////////////////////////////////////////////////////


void setDefault(){

  digitalWrite(ncs,LOW);

  //DEV_OVUV1
  SPI.transfer(0x87);
  SPI.transfer(0x8B);
  SPI.transfer(0x40);
  SPI.transfer(0x03);

  //DEV_OVUV2
  SPI.transfer(0x26);
  SPI.transfer(0x00);
  SPI.transfer(0xED);
  SPI.transfer(0x21);

  //DEV_OVUV3
  SPI.transfer(0x17);
  SPI.transfer(0xFC);
  SPI.transfer(0xFF);
  SPI.transfer(0x11);

  //DEV_OVUV4
  SPI.transfer(0x39);
  SPI.transfer(0x07);
  SPI.transfer(0xF2);
  SPI.transfer(0x02);

  //DEV_OVUV5
  SPI.transfer(0x75);
  SPI.transfer(0x1C);
  SPI.transfer(0x00);
  SPI.transfer(0x0E);

  //DEV_OVUV6
  SPI.transfer(0x83);
  SPI.transfer(0x03);
  SPI.transfer(0x8F);
  SPI.transfer(0x0F);

  //DEV_TLOOP_CFG
  SPI.transfer(0x42);
  SPI.transfer(0x05);
  SPI.transfer(0x14);
  SPI.transfer(0x0A);

  //DEV_AFE_CFG
  SPI.transfer(0x91);
  SPI.transfer(0x00);
  SPI.transfer(0x05);
  SPI.transfer(0x11);

  //DEV_PHASE_CFG
  SPI.transfer(0x85);
  SPI.transfer(0x14);
  SPI.transfer(0x00);
  SPI.transfer(0x08);

  //DEV_CONFIG1
  SPI.transfer(0xEB);
  SPI.transfer(0x00);
  SPI.transfer(0x02);
  SPI.transfer(0x23);

  //DEV_CONTROL1
  SPI.transfer(0x0D);
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  SPI.transfer(0x3C);

  //DEV_CONTROL2
  SPI.transfer(0x38);
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  SPI.transfer(0x25);

  digitalWrite(ncs,HIGH);

  Serial.println("DEFAULT SETTINGS SET!");
  delay(10);
  Serial.println("DEGLITCH TIME PASSED!");
  }


//CRC CALCULATION DIRECTLY FROM THE DATASHEET WITH A LITTLE BIT OF TWEAKING FOR ARDUINO
 unsigned int crcCal(unsigned long datin) {
  unsigned int byte_idx, bit_idx, crc = (0x3F << 2);
  /* byte by byte starting from most significant (3-2-1) */
  for (byte_idx = 3; byte_idx >= 1; byte_idx--) {
    /* XOR-in new byte from left to right */
    crc ^= ((datin >> (byte_idx << 3)) & 0x000000FF);
    /* bit by bit for each byte */
    for (bit_idx = 0; bit_idx < 8; bit_idx++) {
      crc = crc << 1 ^ (crc & 0x80 ? (0x5B << 2) : 0);
    }
  }
  return (crc >> 2 & 0x3F); /*restore two bit offset */
}
