#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,20,4);



void setup() {
  {
  lcd.init(); 
  lcd.init();
  lcd.backlight();
}
initHW();
initSW();
Serial.println("Startes op, vent venligst");
 lcd.setCursor(0,0);
  lcd.print("Startes op,");
  lcd.setCursor(0,1);
  lcd.print("vent venligst");
 meaAll();
 lcd.clear();
  dumpAll();
  blink(13, 10);
}


#define randomMin 500
#define randomMax 5000
#define loopDelay 2000

const int LED = 13;
const int knap = 5;
const int antalPers = 2;
const int antalMaal = 3;
void loop(){}
void initHW()
{
  pinMode(13,OUTPUT);
  pinMode(5, INPUT_PULLUP);
  Serial.begin(115200);
}

struct deltagerMaal{
  unsigned long maaling[antalMaal];
  int hurtigst, langsomst;
  int middel;
};

struct deltagerMaal reakData [antalPers];

int aktuelPers = -1;
int initPers(int index = 0)
{
  for (int i = 0; i < antalMaal; i++){
    reakData[index].middel =-1;
    reakData[index].hurtigst = -1;
    reakData[index].langsomst = -1;}
}
void initSW(void)
{
  for (int i=0 ; i < antalPers ; i++)
  {
    initPers(i);
  }
}

unsigned long meaReacTime()
{
unsigned long startTid, stopTid, reakTid;
 while (digitalRead(knap) == LOW){}
  delay(random(randomMin, randomMax));
 startTid = millis();
 digitalWrite(LED, HIGH);
 while (digitalRead(knap) == HIGH) {}
 stopTid = millis();
 digitalWrite(LED, LOW);
 reakTid = stopTid - startTid;
 return reakTid;
}

void meaPers(int persNr)
{
  nextPers();
  for (int m=0; m<antalMaal; m++)
  {
    reakData[persNr].maaling[m] = meaReacTime();
  }
  calcMeanMinMax(persNr);
}

void nextPers()
{
  blink(13,10);
  digitalWrite(13,HIGH);
  Serial.println("Tryk paa knappen for start");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Tryk paa knappen");
  lcd.setCursor(0,1);
  lcd.print("for start!");
    while (digitalRead(knap)== HIGH){}
  digitalWrite(13,LOW);
  Serial.println("Saa koerer vi!");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Saa koerer vi!");
}

int calcMeanMinMax(int persNr)
{
  int hlp;
  int index=0;
  hlp = 0;
  for (int n = 0; n < antalMaal; n++)
  {
    hlp+= reakData[persNr].maaling[n];
  }
reakData[persNr].middel =hlp/antalMaal;

hlp = reakData[persNr].maaling[0];
index=0;
for (int n =0; n< antalMaal; n++)
{
  if (hlp < reakData[persNr].maaling[n])
  {
    index=n;
    hlp=reakData[persNr].maaling[n];
  }
}
reakData[persNr].langsomst=hlp;

hlp = reakData[persNr].maaling[0];
index=0;
for (int n =0; n< antalMaal; n++)
{
  if (reakData[persNr].maaling[n] < hlp)
  {
    index=n;
    hlp=reakData[persNr].maaling[n];
  }
}
reakData[persNr].hurtigst=hlp;
}
void dumpPers(int persNr)
{
char buffer[38];
sprintf(buffer, "Spiller %i Middel %i Maks %i Min %i ",persNr+1,reakData[persNr].middel,reakData[persNr].langsomst,reakData[persNr].hurtigst);
Serial.print(buffer);
Serial.print("\nDeltager nr "); Serial.println(persNr+1);
//lcd.clear();
//lcd.print(buffer);
//lcd.setCursor(8,persNr);
//lcd.print(persNr+1);
Serial.print("Middel "); Serial.println(reakData[persNr].middel);
//lcd.setCursor(10,persNr);
//lcd.print("Middel "); 
//lcd.setCursor(21,persNr);
//lcd.print(reakData[persNr].middel);
Serial.print("Maks   "); Serial.println(reakData[persNr].langsomst);
//lcd.setCursor(0,persNr);
//lcd.print("Maks "); 
//lcd.setCursor(30,persNr);
//lcd.print(reakData[persNr].langsomst);
Serial.print("Min    "); Serial.println(reakData[persNr].hurtigst);
//lcd.setCursor(0,persNr);
//lcd.print("Min "); 
//lcd.setCursor(38,persNr);
//lcd.print(reakData[persNr].hurtigst);
{
lcd.setCursor(16,persNr);
for(int pos=0;pos<39;pos++)
{
lcd.scrollDisplayLeft();
lcd.print(buffer[pos]);
delay(500);;
}
for (int i=0; i<antalMaal; i++)
{ Serial.print(reakData[persNr].maaling[i]); Serial.print(" ");
}
Serial.println("\n------------");
}
}
void dumpAll()
{
  for(int i=0; i<antalPers;i++)
  {
    dumpPers(i);
  }
}

void meaAll()
{
 for (int i = 0 ; i < antalPers; i++) {
    meaPers(i);
  }
}

void blink(int ledNr, int antalBlink)
{
  digitalWrite(ledNr, LOW);
  delay(500);
  while (antalBlink--) {
    digitalWrite(ledNr, HIGH);
    delay(100);
    digitalWrite(ledNr, LOW);
    delay(100);
  }
}

