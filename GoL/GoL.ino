#include "LedControl.h"   

const int dispositivos = 4; 
LedControl
lc=LedControl(2,4,3
,dispositivos);//DATA | CLK | CS/LOAD | numbero de matrices

LedControl
lc2=LedControl(5,7,6
,dispositivos);//DATA | CLK | CS/LOAD | numbero de matrices

LedControl
lc3=LedControl(8,10,9
,dispositivos);//DATA | CLK | CS/LOAD | numbero de matrices

LedControl
lc4=LedControl(11,13,12
,dispositivos);//DATA | CLK | CS/LOAD | numbero de matrices

const unsigned char texto[] PROGMEM ={"       CONWAY'S GAME OF LIFE           "};

long scrollDelay = 20;
unsigned long bufferLong [14] = {0}; 
unsigned long universe[32];
unsigned long universeNextFrame[32];
int frames=0;
int timeLimitCounter=0;
int contadorVivas[2]={0,0};
byte turnoContador=0;
byte turnoMarquesina=0;

void setup()
{

  delay(1000);
  
  for (int x=0; x<dispositivos; x++) {
    lc.shutdown(x,false);
    lc2.shutdown(x,false);
    lc3.shutdown(x,false);
    lc4.shutdown(x,false);
    lc.setIntensity(x,1);
    lc2.setIntensity(x,1);
    lc3.setIntensity(x,1);
    lc4.setIntensity(x,1);
    lc.clearDisplay(x);
    lc2.clearDisplay(x);
    lc3.clearDisplay(x);
    lc4.clearDisplay(x);
  }

  scrollMessage(texto);
  randomSeed(analogRead(0));

  buildGlider(10,10,0);
  buildGlider(10,20,1);
  buildGlider(20,20,2);
  buildGlider(20,10,3);
  buildLightSpaceship(16,16);

  renderUniverse();
  delay(3000);

}
 
void loop(){

  updateNextFrame();
  for (int y=0;y<32;y++) {
    for (int x=0;x<32;x++) {

      int vecinosAlive = checkEntorno(x,y);
      
      if (!bitRead(universe[y],x) && vecinosAlive==3) {
        bitWrite(universeNextFrame[y],x,true);
      } else if (bitRead(universe[y],x) && (vecinosAlive<2 || vecinosAlive>3)) {
        bitWrite(universeNextFrame[y],x,false);
      }
       
    }
  }
  
  updateActualFrame();
  renderUniverse();
  checkForRestart();
  //delay(25);
}

int checkEntorno(int x,int y) {
   int cont=0; 
   int xMinusOne = x-1;
   int xPlusOne = x+1;
   int yMinusOne = y-1;
   int yPlusOne = y+1;

   if (xMinusOne<0) { xMinusOne=31; }
   if (xPlusOne>31) { xPlusOne=0; }
   if (yMinusOne<0) { yMinusOne=31; }
   if (yPlusOne>31) { yPlusOne=0; }
   
   if (bitRead(universe[yMinusOne],xMinusOne)) { cont++; }
   if (bitRead(universe[yMinusOne],x)) { cont++; }     
   if (bitRead(universe[yMinusOne],xPlusOne)) { cont++; }  

   if (bitRead(universe[y],xMinusOne)) { cont++; }   
   if (bitRead(universe[y],xPlusOne)) { cont++; } 

   if (bitRead(universe[yPlusOne],xMinusOne)) { cont++; }
   if (bitRead(universe[yPlusOne],x)) { cont++; }     
   if (bitRead(universe[yPlusOne],xPlusOne)) { cont++; }  

   return cont;
}

void renderUniverse() {

  for (int y=0; y<8; y++) {
    for (int i=0; i<4; i++) {
      lc.setRow(i,y,((universe[y]>>(i*8)) & 0xff));
    }
  }
  for (int y=8; y<16; y++) {
    for (int i=0; i<4; i++) {
      lc2.setRow(i,y-8,((universe[y]>>(i*8)) & 0xff));
    }
  }
  for (int y=16; y<24; y++) {
    for (int i=0; i<4; i++) {
      lc3.setRow(i,y-16,((universe[y]>>(i*8)) & 0xff));
    }
  }
  for (int y=24; y<32; y++) {
    for (int i=0; i<4; i++) {
      lc4.setRow(i,y-24,((universe[y]>>(i*8)) & 0xff));
    }
  }
}

void updateNextFrame() {

  for (int i=0;i<32;i++) {
    universeNextFrame[i]=universe[i];
  }
}

void updateActualFrame() {

  for (int i=0;i<32;i++) {
    universe[i]=universeNextFrame[i];
  }
}

void checkForRestart() {
  if (++frames>64) {
    timeLimitCounter++;
    
    if (turnoContador == 1) { turnoContador=0; } else { turnoContador=1; }
    contadorVivas[turnoContador]=0; frames=0;
    
    for (int y=0;y<32;y++) {
      for (int x=0;x<32;x++) {
        if (bitRead(universe[y],x)) { contadorVivas[turnoContador]++; } 
      }
    }

    if (contadorVivas[0] == contadorVivas[1] || timeLimitCounter>64) {
      timeLimitCounter=0; startGame();
    }
  }
}

void startGame() {
  for (int x=0; x<dispositivos; x++) { lc2.clearDisplay(x); }
  if (++turnoMarquesina>3) { turnoMarquesina=0; }
  scrollMessage(texto);
  randomSeed(analogRead(0));

  if (random(2)>0) {
    for (int y=0;y<32;y++) {
      randomSeed(analogRead(0));
      for (int x=0;x<32;x++) {
        bitWrite(universe[y],x,random(4)==0);
      }
    }
  } else {
    for (int y=0;y<8;y++) {
      randomSeed(analogRead(0));
      buildGlider(random(32),random(32),random(4));
    }
    randomSeed(analogRead(0));
    buildLightSpaceship(random(32),random(32));
  }

  renderUniverse();
  delay(3000);
}

void buildGlider(int x, int y, byte state) {
  int xMinusOne = x-1;
  int xPlusOne = x+1;
  int yMinusOne = y-1;
  int yPlusOne = y+1;

  if (xMinusOne<0) { xMinusOne=31; }
  if (xPlusOne>31) { xPlusOne=0; }
  if (yMinusOne<0) { yMinusOne=15; }
  if (yPlusOne>15) { yPlusOne=0; }

  switch(state) {
    case 1:
      bitWrite(universe[yPlusOne],x,true);
      bitWrite(universe[y],xMinusOne,true);
      bitWrite(universe[yMinusOne],xMinusOne,true);
      bitWrite(universe[yMinusOne],x,true);
      bitWrite(universe[yMinusOne],xPlusOne,true);
      break;
    case 2:
      bitWrite(universe[yMinusOne],x,true);
      bitWrite(universe[y],xMinusOne,true);
      bitWrite(universe[yPlusOne],xMinusOne,true);
      bitWrite(universe[yPlusOne],x,true);
      bitWrite(universe[yPlusOne],xPlusOne,true);
      break;
    case 3:
      bitWrite(universe[yMinusOne],x,true);
      bitWrite(universe[y],xPlusOne,true);
      bitWrite(universe[yPlusOne],xPlusOne,true);
      bitWrite(universe[yPlusOne],x,true);
      bitWrite(universe[yPlusOne],xMinusOne,true);
      break;
    default:
      bitWrite(universe[yPlusOne],x,true);
      bitWrite(universe[y],xPlusOne,true);
      bitWrite(universe[yMinusOne],xPlusOne,true);
      bitWrite(universe[yMinusOne],x,true);
      bitWrite(universe[yMinusOne],xMinusOne,true);
  }
}

void buildLightSpaceship(int x, int y) {
  int xMinusOne = x-1;
  int xPlusOne = x+1;
  int xPlusTwo = x+2;
  int xPlusThree = x+3;
  int yMinusOne = y-1;
  int yPlusOne = y+1;
  int yPlusTwo = y+2;

  if (xMinusOne<0) { xMinusOne=31; }
  if (xPlusOne>31) { xPlusOne=0; }
  if (xPlusTwo>31) { xPlusTwo=0; }
  if (xPlusThree>31) { xPlusThree=0; }
  if (yMinusOne<0) { yMinusOne=15; }
  if (yPlusOne>15) { yPlusOne=0; }
  if (yPlusTwo>15) { yPlusTwo=0; }
   
  bitWrite(universe[yMinusOne],x,true);
  bitWrite(universe[yMinusOne],xPlusThree,true);
  bitWrite(universe[y],xMinusOne,true);
  bitWrite(universe[yPlusOne],xMinusOne,true);
  bitWrite(universe[yPlusOne],xPlusThree,true);
  bitWrite(universe[yPlusTwo],xMinusOne,true);
  bitWrite(universe[yPlusTwo],x,true);
  bitWrite(universe[yPlusTwo],xPlusOne,true);
  bitWrite(universe[yPlusTwo],xPlusTwo,true);
}
 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 
const unsigned char font5x7 [] PROGMEM = {      //Numeric Font Matrix (Arranged as 7x font data + 1x kerning data)
    B00000000,  //Space (Char 0x20)
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    3,//cambias el tamaño del espacio entre letras
 
    B01000000,  //!
    B01000000,
    B01000000,
    B01000000,
    B01000000,
    B01000000,
    B00000000,
    B01000000,
    2,
 
    B10100000,  //"
    B10100000,
    B10100000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    4,
 
    B01010000,  //#
    B01010000,
    B11111000,
    B01010000,
    B01010000,
    B11111000,
    B01010000,
    B01010000,
    6,
 
    B00100000,  //$
    B01111000,
    B10100000,
    B01110000,
    B00101000,
    B11110000,
    B00100000,
    B00100000,
    6,
 
    B11000000,  //%
    B11001000,
    B00010000,
    B00100000,
    B01000000,
    B10011000,
    B00011000,
    B00000000,
    6,
 
    B01100000,  //&
    B10010000,
    B10100000,
    B10100000,
    B01000000,
    B10101000,
    B10010000,
    B01101000,
    6,
 
    B11000000,  //'
    B01000000,
    B10000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    3,
 
    B01001000,  //( => Ñ
    B10110000,
    B00000000,
    B10001000,
    B11001000,
    B10101000,
    B10011000,
    B10001000,
    6,
 
    B00010000,  //) => Ú
    B00100000,
    B10001000,
    B10001000,
    B10001000,
    B10001000,
    B10001000,
    B01110000,
    6,
 
    B00000000,  //*
    B00100000,
    B10101000,
    B01110000,
    B10101000,
    B00100000,
    B00000000,
    B00000000,
    6,
 
    B00000000,  //+
    B00100000,
    B00100000,
    B11111000,
    B00100000,
    B00100000,
    B00000000,
    B00000000,
    6,
 
    B00000000,  //,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B01000000,
    B01000000,
    B10000000,
    3,
 
    B00000000,  //-
    B00000000,
    B00000000,
    B11111000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    6,
 
    B00000000,  //.
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B11000000,
    B11000000,
    3,
 
    B00000000,  ///
    B00001000,
    B00010000,
    B00100000,
    B01000000,
    B10000000,
    B00000000,
    B00000000,
    6,
 
    B01110000,  //0
    B10001000,
    B10011000,
    B10101000,
    B10101000,
    B11001000,
    B10001000,
    B01110000,
    6,
 
    B01000000,  //1
    B11000000,
    B01000000,
    B01000000,
    B01000000,
    B01000000,
    B01000000,
    B11100000,
    4,
 
    B01110000,  //2
    B10001000,
    B00001000,
    B00001000,
    B00010000,
    B00100000,
    B01000000,
    B11111000,
    6,
 
    B11111000,  //3
    B00010000,
    B00100000,
    B01100000,
    B00010000,
    B00001000,
    B10001000,
    B01110000,
    6,
 
    B00010000,  //4
    B00110000,
    B01010000,
    B10010000,
    B11111000,
    B00010000,
    B00010000,
    B00010000,
    6,
 
    B11111000,  //5
    B10000000,
    B10000000,
    B11110000,
    B00001000,
    B00001000,
    B10001000,
    B01110000,
    6,
 
    B00110000,  //6
    B01000000,
    B10000000,
    B11110000,
    B10001000,
    B10001000,
    B10001000,
    B01110000,
    6,
 
    B11111000,  //7
    B10001000,
    B00001000,
    B00010000,
    B00010000,
    B00100000,
    B00100000,
    B00100000,
    6,
 
    B01110000,  //8
    B10001000,
    B10001000,
    B01110000,
    B10001000,
    B10001000,
    B10001000,
    B01110000,
    6,
 
    B01110000,  //9
    B10001000,
    B10001000,
    B01111000,
    B00001000,
    B00001000,
    B00010000,
    B01100000,
    6,
 
    B00000000,  //:
    B11000000,
    B11000000,
    B00000000,
    B11000000,
    B11000000,
    B00000000,
    B00000000,
    3,
 
    B00000000,  //;
    B11000000,
    B11000000,
    B00000000,
    B00000000,
    B11000000,
    B01000000,
    B10000000,
    3,
 
    B00010000,  //<
    B00100000,
    B01000000,
    B10000000,
    B01000000,
    B00100000,
    B00010000,
    B00000000,
    5,
 
    B00000000,  //=
    B00000000,
    B11111000,
    B00000000,
    B11111000,
    B00000000,
    B00000000,
    B00000000,
    6,
 
    B10000000,  //>
    B01000000,
    B00100000,
    B00010000,
    B00100000,
    B01000000,
    B10000000,
    B00000000,
    5,
 
    B01110000,  //?
    B10001000,
    B00001000,
    B00010000,
    B00100000,
    B00100000,
    B00000000,
    B00100000,
    6,
 
    B01110000,  //@
    B10001000,
    B00001000,
    B01101000,
    B10101000,
    B10101000,
    B01110000,
    B00000000,
    6,
 
    B01100000,  //A
    B10010000,
    B10010000,
    B10010000,
    B11110000,
    B10010000,
    B10010000,
    B10010000,
    5,
 
    B11100000,  //B
    B10010000,
    B10010000,
    B11100000,
    B10010000,
    B10010000,
    B10010000,
    B11100000,
    5,
 
    B01110000,  //C
    B10000000,
    B10000000,
    B10000000,
    B10000000,
    B10000000,
    B10000000,
    B01110000,
    5,
 
    B11100000,  //D
    B10010000,
    B10001000,
    B10001000,
    B10001000,
    B10001000,
    B10010000,
    B11100000,
    6,
 
    B11110000,  //E
    B10000000,
    B10000000,
    B11110000,
    B10000000,
    B10000000,
    B10000000,
    B11110000,
    5,
 
    B11110000,  //F
    B10000000,
    B10000000,
    B11110000,
    B10000000,
    B10000000,
    B10000000,
    B10000000,
    5,
 
    B01110000,  //G
    B10001000,
    B10000000,
    B10111000,
    B10001000,
    B10001000,
    B10001000,
    B01111000,
    6,
 
    B10010000,  //H
    B10010000,
    B10010000,
    B11110000,
    B10010000,
    B10010000,
    B10010000,
    B10010000,
    5,
 
    B11100000,  //I
    B01000000,
    B01000000,
    B01000000,
    B01000000,
    B01000000,
    B01000000,
    B11100000,
    4,
 
    B01110000,  //J
    B00100000,
    B00100000,
    B00100000,
    B00100000,
    B00100000,
    B00100000,
    B11000000,
    5,
 
    B10001000,  //K
    B10010000,
    B10100000,
    B11000000,
    B11000000,
    B10100000,
    B10010000,
    B10001000,
    6,
 
    B10000000,  //L
    B10000000,
    B10000000,
    B10000000,
    B10000000,
    B10000000,
    B10000000,
    B11110000,
    5,
 
    B10001000,  //M
    B11011000,
    B10101000,
    B10101000,
    B10001000,
    B10001000,
    B10001000,
    B10001000,
    6,
 
    B10001000,  //N
    B10001000,
    B11001000,
    B10101000,
    B10101000,
    B10011000,
    B10001000,
    B10001000,
    6,
 
    B01100000,  //O
    B10010000,
    B10010000,
    B10010000,
    B10010000,
    B10010000,
    B10010000,
    B01100000,
    5,
 
    B11100000,  //P
    B10010000,
    B10010000,
    B11100000,
    B10000000,
    B10000000,
    B10000000,
    B10000000,
    5,
 
    B01110000,  //Q
    B10001000,
    B10001000,
    B10001000,
    B10001000,
    B10101000,
    B10010000,
    B01101000,
    6,
 
    B11110000,  //R
    B10001000,
    B10001000,
    B11110000,
    B11000000,
    B10100000,
    B10010000,
    B10001000,
    6,
 
    B01110000,  //S
    B10000000,
    B10000000,
    B01100000,
    B00010000,
    B00010000,
    B00010000,
    B11100000,
    5,
 
    B11111000,  //T
    B00100000,
    B00100000,
    B00100000,
    B00100000,
    B00100000,
    B00100000,
    B00100000,
    6,
 
    B10010000,  //U
    B10010000,
    B10010000,
    B10010000,
    B10010000,
    B10010000,
    B10010000,
    B01100000,
    5,
 
    B10001000,  //V
    B10001000,
    B10001000,
    B10001000,
    B10001000,
    B01010000,
    B01010000,
    B00100000,
    6,
 
    B10001000,  //W
    B10001000,
    B10001000,
    B10101000,
    B10101000,
    B10101000,
    B01010000,
    B01010000,
    6,
 
    B10001000,  //X
    B10001000,
    B01010000,
    B00100000,
    B01010000,
    B01010000,
    B10001000,
    B10001000,
    6,
 
    B10001000,  //Y
    B10001000,
    B10001000,
    B01010000,
    B00100000,
    B00100000,
    B00100000,
    B00100000,
    6,
 
    B11111000,  //Z
    B00001000,
    B00010000,
    B00100000,
    B00100000,
    B01000000,
    B10000000,
    B11111000,
    6,
 
    B11100000,  //[
    B10000000,
    B10000000,
    B10000000,
    B10000000,
    B10000000,
    B10000000,
    B11100000,
    4,
 
    B00000000,  //(Backward Slash)
    B10000000,
    B01000000,
    B00100000,
    B00010000,
    B00001000,
    B00000000,
    B00000000,
    6,
 
    B11100000,  //]
    B00100000,
    B00100000,
    B00100000,
    B00100000,
    B00100000,
    B00100000,
    B11100000,
    4,
 
    B00100000,  //^
    B01010000,
    B10001000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    6,
 
    B00000000,  //_
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B11111000,
    6,
 
    B10000000,  //`
    B01000000,
    B00100000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    4,
 
    B00000000,  //a
    B00000000,
    B00000000,
    B01110000,
    B00001000,
    B01111000,
    B10001000,
    B01111000,
    6,
 
    B00000000,  //b
    B10000000,
    B10000000,
    B10110000,
    B11001000,
    B10001000,
    B10001000,
    B11110000,
    6,
 
    B00000000,  //c
    B00000000,
    B00000000,
    B01110000,
    B10001000,
    B10000000,
    B10001000,
    B01110000,
    6,
 
    B00000000,  //d
    B00001000, 
    B00001000,
    B01101000,
    B10011000,
    B10001000,
    B10001000,
    B01111000,
    6,
 
    B00000000,  //e
    B00000000,
    B00000000,
    B01110000,
    B10001000,
    B11111000,
    B10000000,
    B01110000,
    6,
 
    B00000000,
    B00110000,  //f
    B01001000,
    B01000000,
    B11100000,
    B01000000,
    B01000000,
    B01000000,
    6,
 
    B00000000,
    B00000000,  //g
    B01111000,
    B10001000,
    B10001000,
    B01111000,
    B00001000,
    B01110000,
    6,
 
    B00000000,
    B10000000,  //h
    B10000000,
    B10110000,
    B11001000,
    B10001000,
    B10001000,
    B10001000,
    6,
 
    B00000000,
    B01000000,  //i
    B00000000,
    B11000000,
    B01000000,
    B01000000,
    B01000000,
    B11100000,
    4,
 
    B00000000,
    B00010000,  //j
    B00000000,
    B00110000,
    B00010000,
    B00010000,
    B10010000,
    B01100000,
    5,
 
    B00000000,
    B10000000,  //k
    B10000000,
    B10010000,
    B10100000,
    B11000000,
    B10100000,
    B10010000,
    5,
 
    B00000000,
    B11000000,  //l
    B01000000,
    B01000000,
    B01000000,
    B01000000,
    B01000000,
    B11100000,
    4,
 
    B00000000,
    B00000000,  //m
    B00000000,
    B11010000,
    B10101000,
    B10101000,
    B10001000,
    B10001000,
    6,
 
    B00000000,
    B00000000,  //n
    B00000000,
    B10110000,
    B11001000,
    B10001000,
    B10001000,
    B10001000,
    6,
 
    B00000000,
    B00000000,  //o
    B00000000,
    B01110000,
    B10001000,
    B10001000,
    B10001000,
    B01110000,
    6,
 
    B00000000,
    B00000000,  //p
    B00000000,
    B11110000,
    B10001000,
    B11110000,
    B10000000,
    B10000000,
    6,
 
    B00000000,
    B00000000,  //q
    B00000000,
    B01101000,
    B10011000,
    B01111000,
    B00001000,
    B00001000,
    6,
 
    B00000000,
    B00000000,  //r
    B00000000,
    B10110000,
    B11001000,
    B10000000,
    B10000000,
    B10000000,
    6,
 
    B00000000,
    B00000000,  //s
    B00000000,
    B01110000,
    B10000000,
    B01110000,
    B00001000,
    B11110000,
    6,
 
    B00000000,
    B01000000,  //t
    B01000000,
    B11100000,
    B01000000,
    B01000000,
    B01001000,
    B00110000,
    6,
 
    B00000000,
    B00000000,  //u
    B00000000,
    B10001000,
    B10001000,
    B10001000,
    B10011000,
    B01101000,
    6,
 
    B00000000,
    B00000000,  //v
    B00000000,
    B10001000,
    B10001000,
    B10001000,
    B01010000,
    B00100000,
    6,
 
    B00000000,
    B00000000,  //w
    B00000000,
    B10001000,
    B10101000,
    B10101000,
    B10101000,
    B01010000,
    6,
 
    B00000000,
    B00000000,  //x
    B00000000,
    B10001000,
    B01010000,
    B00100000,
    B01010000,
    B10001000,
    6,
 
    B00000000,
    B00000000,  //y
    B00000000,
    B10001000,
    B10001000,
    B01111000,
    B00001000,
    B01110000,
    6,
 
    B00000000,
    B00000000,  //z
    B00000000,
    B11111000,
    B00010000,
    B00100000,
    B01000000,
    B11111000,
    6,
 
    B00001000,  //{ -> é
    B00010000,
    B00000000,
    B01110000,
    B10001000,
    B11111000,
    B10000000,
    B01110000,
    6,
 
    B10000000,  //|
    B10000000,
    B10000000,
    B10000000,
    B10000000,
    B10000000,
    B10000000,
    B10000000,
    2,
 
    B00001000,  //} -> à
    B00010000,
    B00000000,
    B01110000,
    B00001000,
    B01111000,
    B10001000,
    B01111000,
    6,
 
    B01100000,  //~ > º
    B10010000,
    B10010000,
    B01100000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    5,
 
    B01100000,  // (Char 0x7F)
    B10010000,
    B10010000,
    B01100000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    5,
    
    B00000000,  // smiley
    B01100000,
    B01100110,
    B00000000,
    B10000001,
    B01100110,
    B00011000,
    B00000000,
    5
};
 
// Scroll Message
void scrollMessage(const unsigned char * messageString) {
    int counter = 0;
    int myChar=0;
    do {
        // read back a char 
        myChar =  pgm_read_byte_near(messageString + counter); 
        if (myChar != 0){
            loadBufferLong(myChar);
        }
        counter++;
    } 
    while (myChar != 0);
}
// Load character into scroll buffer
void loadBufferLong(int ascii){
    if (ascii >= 0x20 && ascii <=0x7f){
        for (int a=0;a<8;a++){                      // Loop 7 times for a 5x7 font
            unsigned long c = pgm_read_byte_near(font5x7 + ((ascii - 0x20) * 9) + a);     // Index into character table to get row data
            unsigned long x = bufferLong [a*2];     // Load current scroll buffer
            x = x | c;                              // OR the new character onto end of current
            bufferLong [a*2] = x;                   // Store in buffer
        }
        byte count = pgm_read_byte_near(font5x7 +((ascii - 0x20) * 9) + 8);     // Index into character table for kerning data
        for (byte x=0; x<count;x++){
            rotateBufferLong();
            printBufferLong();
            delay(scrollDelay);
        }
    }
}
// Rotate the buffer
void rotateBufferLong(){
    for (int a=0;a<8;a++){                      // Loop 7 times for a 5x7 font
        unsigned long x = bufferLong [a*2];     // Get low buffer entry
        byte b = bitRead(x,31);                 // Copy high order bit that gets lost in rotation
        x = x<<1;                               // Rotate left one bit
        bufferLong [a*2] = x;                   // Store new low buffer
        x = bufferLong [a*2+1];                 // Get high buffer entry
        x = x<<1;                               // Rotate left one bit
        bitWrite(x,0,b);                        // Store saved bit
        bufferLong [a*2+1] = x;                 // Store new high buffer
    }
}  
// Display Buffer on LED matrix
void printBufferLong(){
  switch (turnoMarquesina) {
    case 0:
      for (int a=0;a<8;a++){                    // Loop 7 times for a 5x7 font
        unsigned long x = bufferLong [a*2+1];   // Get high buffer entry
        byte y = x;                             // Mask off first character
        lc.setRow(3,a,y);                       // Send row to relevent MAX7219 chip
        x = bufferLong [a*2];                   // Get low buffer entry
        y = (x>>24);                            // Mask off second character
        lc.setRow(2,a,y);                       // Send row to relevent MAX7219 chip
        y = (x>>16);                            // Mask off third character
        lc.setRow(1,a,y);                       // Send row to relevent MAX7219 chip
        y = (x>>8);                             // Mask off forth character
        lc.setRow(0,a,y);                       // Send row to relevent MAX7219 chip
      }
    break;
    case 1:
      for (int a=0;a<8;a++){                    // Loop 7 times for a 5x7 font
        unsigned long x = bufferLong [a*2+1];   // Get high buffer entry
        byte y = x;                             // Mask off first character
        lc2.setRow(3,a,y);                       // Send row to relevent MAX7219 chip
        x = bufferLong [a*2];                   // Get low buffer entry
        y = (x>>24);                            // Mask off second character
        lc2.setRow(2,a,y);                       // Send row to relevent MAX7219 chip
        y = (x>>16);                            // Mask off third character
        lc2.setRow(1,a,y);                       // Send row to relevent MAX7219 chip
        y = (x>>8);                             // Mask off forth character
        lc2.setRow(0,a,y);                       // Send row to relevent MAX7219 chip
      }
    break;
    case 2:
      for (int a=0;a<8;a++){                    // Loop 7 times for a 5x7 font
        unsigned long x = bufferLong [a*2+1];   // Get high buffer entry
        byte y = x;                             // Mask off first character
        lc3.setRow(3,a,y);                       // Send row to relevent MAX7219 chip
        x = bufferLong [a*2];                   // Get low buffer entry
        y = (x>>24);                            // Mask off second character
        lc3.setRow(2,a,y);                       // Send row to relevent MAX7219 chip
        y = (x>>16);                            // Mask off third character
        lc3.setRow(1,a,y);                       // Send row to relevent MAX7219 chip
        y = (x>>8);                             // Mask off forth character
        lc3.setRow(0,a,y);                       // Send row to relevent MAX7219 chip
      }
    break;
    case 3:
      for (int a=0;a<8;a++){                    // Loop 7 times for a 5x7 font
        unsigned long x = bufferLong [a*2+1];   // Get high buffer entry
        byte y = x;                             // Mask off first character
        lc4.setRow(3,a,y);                       // Send row to relevent MAX7219 chip
        x = bufferLong [a*2];                   // Get low buffer entry
        y = (x>>24);                            // Mask off second character
        lc4.setRow(2,a,y);                       // Send row to relevent MAX7219 chip
        y = (x>>16);                            // Mask off third character
        lc4.setRow(1,a,y);                       // Send row to relevent MAX7219 chip
        y = (x>>8);                             // Mask off forth character
        lc4.setRow(0,a,y);                       // Send row to relevent MAX7219 chip
      }
    break;
  } 
}
