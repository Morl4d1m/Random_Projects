/*
Ét styks rigtig lang kode:
I koden har jeg forsoegt at vise mange af de facetter vi har laert igennem semestret
Det er bygget op med en setup funktion, som goer alt klar, derefter et loop med en switch, der indeholder 6 cases
Derudover er der indsat et ur, da de to foerste cases benytter et saadan
Ved alle "Serial.println();" er der indsat et "F()" indenfor parenteserne, og det er for at spare plads i rammen, og i stedet benytte flash memory
Det skal ogsaa bemaerkes at med en standard stroemforsyning fra Arduinoen kan man ikke drive både KY-015 og hoejtaleren paa samme tid, saa faar KY-015 nemlig ikke nok stroem

I case 1 bruges et ur, højtaler og SD-modul, for at kunne afspille Last Christmas med Wham mellem kl 8 og 16 hver dag, og så spille den samme antal gange, som datoen. Altsaa at man hoerer Wham 5 gange 5. december 
Case 2 er næsten magen til, her er det bare Angel of Death med Slayer der skal hoeres i stedet, og saa i et andet tidsinterval
I case 3 og 4 benyttes en KY-015 temperatur/fugtigheds-sensor til at maale om der eventuelt skal luftes ud, eller justeres på fyret
Case 5 er lidt anderledes da denne benytter analogporten og ADC til at hente info omkring lydtryk via et KY-038 modul, og hvis larmen bliver for stor, taendes en LED
Case 6 viser i store traek bare, at der kan bruges pointere til at skifte vaerdier af variable.
Yderligere ville jeg ogsaa have indsat en funktion, som kunne fortaelle hvilken adresse variablen "LoopValg" havde, men dette understoetter Arduino IDE aabenbart ikke.

Håber dette giver lidt klarificering af koden, ellers maa de oevrige kommentare kunne hjaelpe på vej

- Christian Lykke Joergensen

*/

//Biblioteksudvalg
#include <dht.h>
#include "SD.h"
#include <time.h>
#include "TMRpcm.h"
#include "SPI.h"
#include <virtuabotixRTC.h>
#include <LiquidCrystal_I2C.h>

//Diverse definitioner
#define SD_ChipSelectPin 4
#define DHT11_PIN 2
#define randomMinWham 1 //Minimal randomvaerdi på 1 millisekund, så man risikerer at hører wham med det samme
#define randomMaxWham 28800000 //Maksimalt randomvaerdi på 8 timer, så wham senest starter efter 8 timer
#define randomMinSlayer 1 //Minimal randomvaerdi på 1 millisekund, så man risikerer at hører slayer med det samme
#define randomMaxSlayer 28800000 //Maksimalt randomvaerdi på 8 timer, så slayer senest starter efter 8 timer

//Aktivering/definition af moduler
virtuabotixRTC myRTC(6, 7, 8);
LiquidCrystal_I2C lcd(0x27,20,4);
TMRpcm tmrpcm;
dht DHT;

//Globale variable
int i;
int LoopValg=3;
int mic = A3;
const int LED = 10;
const int knap = 2;
const int WhamStart=8; //Vi vil først høre wham fra kl 8
const int WhamSlut=16; //Efter kl 16 vil vi ikke længere høre wham
const int SlayerStart=21; //Slayer er bedst efter 21
const int SlayerSlut=24; //Slayer er lidt tungt at sove til


void setup(){
Serial.begin(115200);
pinMode(knap, INPUT_PULLUP); //Knappens pin
pinMode(3, OUTPUT); //KY-038 lydsensor
pinMode(5, OUTPUT); //KY-015/fugt/temp sensor
tmrpcm.speakerPin = 9; //Definerer at højtaleren får input fra pin 9
pinMode(LED, OUTPUT); //LED output

{
lcd.init(); //Starter lcd skærmen
lcd.backlight(); //Tænder belysningen i skærmen
}

if (!SD.begin(SD_ChipSelectPin))  //Tjekker om der er isat SDkort
{
  Serial.println(F("SD fail"));
    return;
}

//Saet nuvaerende tid og dato i foelgende format:
//Sekunder, minutter, timer, ugedag, dag på maaneden, maaned, aar
myRTC.setDS1302Time(55, 58, 20, 3, 30, 11, 2022);
}

void loop()  {
//Hvis man skulle ha brug for at opdatere uret kan man gøre det i nedenstaaende funktion
myRTC.updateTime();
int timetal = myRTC.hours; //Giver en variabel programmet kan reagere ud fra, og som kan tilskrives nye værdier, uden at uret går forkert
int dato = myRTC.dayofmonth; //Giver en variabel programmet kan reagere ud fra, og som kan tilskrives nye værdier, uden at uret går forkert
//Start printing elements as individuals
char visJegKanBrugeArrays[52];
sprintf(visJegKanBrugeArrays, "Nuvaerende dato og klokkeslaet: %i/%i/%i  %i:%i:%i",myRTC.dayofmonth,myRTC.month,myRTC.year,myRTC.hours,myRTC.minutes,myRTC.seconds);
Serial.println(visJegKanBrugeArrays);
/*Serial.print("Nuvaerende dato og klokkeslaet: "); // linje 90 til 101 er erstattet med linje 88 og 89, for at vise man kan lave en laengere string lidt alternativt
Serial.print(myRTC.dayofmonth);
Serial.print("/");
Serial.print(myRTC.month);
Serial.print("/");
Serial.print(myRTC.year);
Serial.print("  ");
Serial.print(myRTC.hours);
Serial.print(":");
Serial.print(myRTC.minutes);
Serial.print(":");
Serial.println(myRTC.seconds);*/
Serial.println(LoopValg); //Printer hvilket casenr der er igang
if (LoopValg != 5){
  delay(1000);  //Forsinkelse så uret kun bliver opdateret 1 gang i sekundet
}
else{ //Da case 5 afhaenger af lydtryk, er det vigtigt der ikke er nogen delay, saa programmet kan sample hurtigere
}
switch(LoopValg){
case 1:
if (timetal>=WhamStart && timetal<WhamSlut){
  while (timetal>=WhamStart && timetal<=WhamSlut) {
  delay(random(randomMinWham, randomMaxWham));
  Serial.println(F("Nu skal der hoeres WHAM"));
  for (i=1; i<=dato; i++){ //for loop der afspiller sangen ligeså mange gange som der er gaaet dage i december (5. december = at man hører wham 5 gange)
    tmrpcm.play("Wham.wav"); //funktionen der afspiller last christmas
    digitalWrite(LED, HIGH); //Taender en LED, for at vise sangen spiller
    if (digitalRead(knap)==HIGH){ //Tjekker om knappen bliver aktiveret
      break; //Giver mulighed for at gå til naeste case
    }
    else {
      return; //Returnerer til while loop
    }
  delay(277000);//Delay i hele sangens længde
  digitalWrite(LED, LOW); //Slukker LED'en igen
  Serial.println(i);} //Taeller antal gange der er hoert wham
  break; //afslutter whileloop, saa if loopet kan tjekke om klokken stadig er mellem 8 og 16
  }
}
else {
  Serial.print(F("Det er ikke tid til WHAM endnu\n")); //Fortaeller at det ikke er tid til wham endnu
    if (digitalRead(knap)==HIGH){ //Tjekker om knappen bliver aktiveret
      break; //Giver mulighed for at gå til naeste case
  return;
}
}
while (digitalRead(knap) == HIGH) {}
  LoopValg++; //Taeller loopvalg op, saa en ny case vaelges
  break; //Giver mulighed for at gå til naeste case
case 2:
if (timetal>=SlayerStart && timetal<SlayerSlut){
  while (timetal>=SlayerStart && timetal<=SlayerSlut) {
  delay(random(randomMinSlayer, randomMaxSlayer));
  Serial.println(F("Nu skal der hoeres Slayer"));
  for (i=1; i<=dato; i++){ //for loop der afspiller sangen ligeså mange gange som der er gaaet dage i december (5. december = at man hører wham 5 gange)
    tmrpcm.play("Slayer.wav"); //funktionen der afspiller last christmas
    digitalWrite(LED, HIGH); //Taender en LED, for at vise sangen spiller
    if (digitalRead(knap)==HIGH){ //Tjekker om knappen bliver aktiveret
      break; //Giver mulighed for at gå til naeste case
    }
    else {
      return; //Returnerer til while loop
    }
  delay(291000);//Delay i hele sangens længde
  digitalWrite(LED, LOW); //Slukker LED'en igen
  Serial.println(i);} //Taeller antal gange der er hoert Slayer
  break; //afslutter whileloop, saa if loopet kan tjekke om klokken stadig er mellem 8 og 16
  }
}
else {
  Serial.print(F("Det er ikke tid til Slayer endnu\n")); //Fortaeller at det ikke er tid til wham endnu
    if (digitalRead(knap)==HIGH){ //Tjekker om knappen bliver aktiveret
      break; //Giver mulighed for at gå til naeste case
  return;
}
}
while (digitalRead(knap) == HIGH) {} //Tjekker om knappen bliver aktiveret
  LoopValg++; //Taeller loopvalg op, saa en ny case vaelges
  break; //Giver mulighed for at gå til naeste case
case 3:
{
int chk1 = DHT.read11(DHT11_PIN);
delay(2000);
int fugt=DHT.humidity; //Giver int fugt vaerdien af den aktuelle maengde fugt i luften fra DHT structen
if (fugt >= 40){
  Serial.println(F("Du boer aabne dit vindue og faa luftet ud"));
  lcd.clear(); //Sletter alt på LCD-skaermen
  lcd.setCursor(0,0); //Saetter "cursoren" paa LCD'en, saa teksten i naeste linje bliver vist det rigtige sted
  lcd.print("Du boer aabne dit vindue og faa luftet ud");
  lcd.setCursor(0,1);  //Saetter "cursoren" paa LCD'en, saa teksten i naeste linje bliver vist det rigtige sted
  lcd.print("Fugtighed: ");
  lcd.print(DHT.humidity); //tilgaar data i DHT structen fra DHT.h til aflaesning på LCD'en
  lcd.print("%");
  if (digitalRead(knap)==HIGH){ //Tjekker om knappen bliver aktiveret
    break; //Giver mulighed for at gå til naeste case
}
}
else if (fugt >= 60){
  Serial.println(F("Nu skal du virkelig ha luftet ud"));
  lcd.clear(); //Sletter alt på LCD-skaermen
  lcd.setCursor(0,0); //Saetter "cursoren" paa LCD'en, saa teksten i naeste linje bliver vist det rigtige sted
  lcd.print("Nu skal du virkelig ha luftet ud");
  lcd.setCursor(0,1);  //Saetter "cursoren" paa LCD'en, saa teksten i naeste linje bliver vist det rigtige sted
  lcd.print("Fugtighed: ");
  lcd.print(DHT.humidity); //tilgaar data i DHT structen fra DHT.h til aflaesning på LCD'en
  lcd.print("%");
  if (digitalRead(knap)==HIGH){ //Tjekker om knappen bliver aktiveret
    break; //Giver mulighed for at gå til naeste case
}
}
else if (fugt >= 90){
  Serial.println(F("Det er utroligt, hvis der ikke gror skimmel i hjoernerne"));
  lcd.clear(); //Sletter alt på LCD-skaermen
  lcd.setCursor(0,0); //Saetter "cursoren" paa LCD'en, saa teksten i naeste linje bliver vist det rigtige sted
  lcd.print("Det er utroligt, hvis der ikke gror skimmel i hjoernerne");
  lcd.setCursor(0,1);  //Saetter "cursoren" paa LCD'en, saa teksten i naeste linje bliver vist det rigtige sted
  lcd.print("Fugtighed: ");
  lcd.print(DHT.humidity); //tilgaar data i DHT structen fra DHT.h til aflaesning på LCD'en
  lcd.print("%");
  if (digitalRead(knap)==HIGH){ //Tjekker om knappen bliver aktiveret
    break; //Giver mulighed for at gå til naeste case
}
}
else{
  Serial.println(F("Luftfugtigheden er fin :)"));
  lcd.clear(); //Sletter alt på LCD-skaermen
  lcd.setCursor(0,0); //Saetter "cursoren" paa LCD'en, saa teksten i naeste linje bliver vist det rigtige sted
  lcd.print("Luftfugtigheden er fin :)");
  lcd.setCursor(0,1);  //Saetter "cursoren" paa LCD'en, saa teksten i naeste linje bliver vist det rigtige sted
  lcd.print("Fugtighed: ");
  lcd.print(DHT.humidity); //tilgaar data i DHT structen fra DHT.h til aflaesning på LCD'en
  lcd.print("%");
  if (digitalRead(knap)==HIGH){ //Tjekker om knappen bliver aktiveret
    break; //Giver mulighed for at gå til naeste case
}
}
}
while (digitalRead(knap) == HIGH) {} //Tjekker om knappen bliver aktiveret
  LoopValg++; //Taeller loopvalg op, saa en ny case vaelges
    break; //Giver mulighed for at gå til naeste case
case 4:
{ 
int chk2 = DHT.read11(DHT11_PIN);
delay(2000);
int temp=DHT.temperature;  //Giver int fugt vaerdien af den aktuelle temperatur fra DHT structen
Serial.println(temp);
if (temp >= 40){
  Serial.println(F("Sluk fyret!"));
  lcd.clear(); //Sletter alt på LCD-skaermen
  lcd.setCursor(0,0); //Saetter "cursoren" paa LCD'en, saa teksten i naeste linje bliver vist det rigtige sted
  lcd.print("Sluk fyret!");
  lcd.setCursor(0,1);  //Saetter "cursoren" paa LCD'en, saa teksten i naeste linje bliver vist det rigtige sted
  lcd.print("Temp: ");
  lcd.print(DHT.temperature); //tilgaar data i DHT structen fra DHT.h til aflaesning på LCD'en
  lcd.print((char)223); //Printer "grader" symbolet
  lcd.print("C"); //C fordi det er i celsius
  if (digitalRead(knap)==HIGH){ //Tjekker om knappen bliver aktiveret
    break; //Giver mulighed for at gå til naeste case
}
}
else if (temp >= 30){
  Serial.println(F("Skru ned"));
  lcd.clear(); //Sletter alt på LCD-skaermen
  lcd.setCursor(0,0); //Saetter "cursoren" paa LCD'en, saa teksten i naeste linje bliver vist det rigtige sted
  lcd.print("Skru ned");
  lcd.setCursor(0,1);  //Saetter "cursoren" paa LCD'en, saa teksten i naeste linje bliver vist det rigtige sted
  lcd.print("Temp: ");
  lcd.print(DHT.temperature); //tilgaar data i DHT structen fra DHT.h til aflaesning på LCD'en
  lcd.print((char)223); //Printer "grader" symbolet
  lcd.print("C"); //C fordi det er i celsius
  if (digitalRead(knap)==HIGH){ //Tjekker om knappen bliver aktiveret
    break; //Giver mulighed for at gå til naeste case
}
}
else if (temp >= 20){
  Serial.println(F("Helt perfekt"));
  lcd.clear(); //Sletter alt på LCD-skaermen
  lcd.setCursor(0,0); //Saetter "cursoren" paa LCD'en, saa teksten i naeste linje bliver vist det rigtige sted
  lcd.print("Helt perfekt");
  lcd.setCursor(0,1);  //Saetter "cursoren" paa LCD'en, saa teksten i naeste linje bliver vist det rigtige sted
  lcd.print("Temp: ");
  lcd.print(DHT.temperature); //tilgaar data i DHT structen fra DHT.h til aflaesning på LCD'en
  lcd.print((char)223); //Printer "grader" symbolet
  lcd.print("C"); //C fordi det er i celsius
  if (digitalRead(knap)==HIGH){ //Tjekker om knappen bliver aktiveret
    break; //Giver mulighed for at gå til naeste case
}
}
else if (temp >= 15){
  Serial.println(F("Skru op for varmen"));
  lcd.clear(); //Sletter alt på LCD-skaermen
  lcd.setCursor(0,0); //Saetter "cursoren" paa LCD'en, saa teksten i naeste linje bliver vist det rigtige sted
  lcd.print("Skru op for varmen");
  lcd.setCursor(0,1);  //Saetter "cursoren" paa LCD'en, saa teksten i naeste linje bliver vist det rigtige sted
  lcd.print("Temp: ");
  lcd.print(DHT.temperature); //tilgaar data i DHT structen fra DHT.h til aflaesning på LCD'en
  lcd.print((char)223); //Printer "grader" symbolet
  lcd.print("C"); //C fordi det er i celsius
  if (digitalRead(knap)==HIGH){ //Tjekker om knappen bliver aktiveret
    break; //Giver mulighed for at gå til naeste case
}
}
else{
  Serial.println(F("Taend for varmen"));
  lcd.clear(); //Sletter alt på LCD-skaermen
  lcd.setCursor(0,0); //Saetter "cursoren" paa LCD'en, saa teksten i naeste linje bliver vist det rigtige sted
  lcd.print("Taend for varmen");
  lcd.setCursor(0,1);  //Saetter "cursoren" paa LCD'en, saa teksten i naeste linje bliver vist det rigtige sted
  lcd.print("Temp: ");
  lcd.print(DHT.temperature); //tilgaar data i DHT structen fra DHT.h til aflaesning på LCD'en
  lcd.print((char)223); //Printer "grader" symbolet
  lcd.print("C"); //C fordi det er i celsius
  if (digitalRead(knap)==HIGH){ //Tjekker om knappen bliver aktiveret
    break; //Giver mulighed for at gå til naeste case
}
}
}
while (digitalRead(knap) == HIGH) {} //Tjekker om knappen bliver aktiveret
  LoopValg++; //Taeller loopvalg op, saa en ny case vaelges
    break; //Giver mulighed for at gå til naeste case
case 5:
{
float sensorValue = analogRead(mic);
Serial.println(sensorValue);
if (sensorValue>=530){
  digitalWrite(LED, HIGH); //Taender LED'en
  delay(100); //Venter 1/10 sekund
  digitalWrite(LED, LOW); //Slukker LED'en
  lcd.clear(); //Sletter alt på LCD-skaermen
  lcd.setCursor(0,0); //Saetter "cursoren" paa LCD'en, saa teksten i naeste linje bliver vist det rigtige sted
  lcd.print(sensorValue);
  lcd.setCursor(0,1); //Saetter "cursoren" paa LCD'en, saa teksten i naeste linje bliver vist det rigtige sted
  lcd.print("Find hoerevaern");
  if (digitalRead(knap)==HIGH){ //Tjekker om knappen bliver aktiveret
    LoopValg++; //Taeller loopvalg op, saa en ny case vaelges -- DU MAA IKKE SLIPPE KNAPPEN MENS DEN LAESER LYDTRYK, SAA GAAR DEN TIL NAESTE CASE
      break; //Giver mulighed for at gå til naeste case
}
else {
  return; //Returnerer til forrige if loop
}
return;
}
else {
  lcd.clear(); //Sletter alt på LCD-skaermen
  lcd.setCursor(0,0); //Saetter "cursoren" paa LCD'en, saa teksten i naeste linje bliver vist det rigtige sted
  lcd.print(sensorValue);
  lcd.setCursor(0,1); //Saetter "cursoren" paa LCD'en, saa teksten i naeste linje bliver vist det rigtige sted
  lcd.print("Ingen hoerevaern");
  if (digitalRead(knap)==HIGH){ //Tjekker om knappen bliver aktiveret
    LoopValg++; //Taeller loopvalg op, saa en ny case vaelges -- DU MAA IKKE SLIPPE KNAPPEN MENS DEN LAESER LYDTRYK, SAA GAAR DEN TIL NAESTE CASE
      break; //Giver mulighed for at gå til naeste case
}
else {
  return; //Returnerer til forrige if loop
}
return;
}

}
while (digitalRead(knap) == HIGH) {} //Tjekker om knappen bliver aktiveret
  LoopValg++; //Taeller loopvalg op, saa en ny case vaelges
    break; //Giver mulighed for at gå til naeste case
case 6:
callByReferenceOgPointers();
while (digitalRead(knap) == HIGH) {} //Tjekker om knappen bliver aktiveret
  LoopValg=1;//nulstiller caseswitchen, så man ikke ender i en cases, som ikke eksisterer
    break;
}
}


void callByReferenceOgPointers() {
//lokale variabler
int a = 100;
int b = 200;
Serial.println(F("Foer ombytning, vaerdi af a : "));
Serial.println(a);
Serial.println(F("Foer ombytning, vaerdi af b : "));
Serial.println(b);
bytteBytteKoebmand(&a, &b); // kalder bytteBytteKoebmand
Serial.println(F("Efter ombytning, vaerdi af a : "));
Serial.println(a);
Serial.println(F("Efter ombytning, vaerdi af b : "));
Serial.println(b);
return 0;
}
void bytteBytteKoebmand(int *x, int *y) {

int midlertidig;

midlertidig = *x; //gemmer værdien af x i midlertidig
*x = *y;  //gemmer y i x og overskriver x's vaerdi
*y = midlertidig; //gemmer den originale x vaerdi (fra midlertidig) i y

return;
}
/* Den funktion herunder kan jeg ikke faa til at du i arduino IDE, da denne ikke understoetter pointere. Derfor er dette stykke skrevet til at kunne blive sat ind i onlineGDB, og saa deri vise hvad pointere også kan bruges til
#include <stdio.h>
int LoopValg = 1;
int main ()
{
printf ("%p\n", &LoopValg);	//Printer adressen for LoopValg
int *ptr = &LoopValg;		//Pointer definition
printf ("%d\n", ptr);		//printer adressen for LoopValg vha pointeren
printf ("%d\n", *ptr);	//printer vaerdien gemt i ptr (samme som LoopValg)
}
}*/