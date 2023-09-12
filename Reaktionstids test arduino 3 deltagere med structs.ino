int reaktionstid[5];
struct deltagere{
  char navn;
  int reaktionstid[5]; //array er på fem da jeg vurderer 5 målinger er nok.
  float middelreaktid;
};

struct deltagere antal[3]; // antal deltagere i reaktionstesten

unsigned long startTid, stopTid; // tidsindikatorer for reaktionstiden
void setup() { 

  Serial.begin(115200);  // 9600 

  pinMode(13, OUTPUT); 

  pinMode(5, INPUT_PULLUP); 

} 
int maalReaktionsTid()
{
  int r;
  r=random(100,300);
  Serial.print("debug"); Serial.println(r);
  delay(r);// kun til test
  return (r);
}
float beregnmiddel(int databaseindex)
{
  
}

void loop() { 


for (int i = 0; i < 5; i++) { 

  //hvad er klokken ved start 

  delay(random(1000,5000)); 

  startTid = millis(); 

  digitalWrite(13, HIGH); 

  while ( digitalRead(5) == HIGH ) { 

  } 

  stopTid = millis(); 

  digitalWrite(13,LOW); 

  reaktionstid[i] = stopTid - startTid; 

  Serial.print("Din reaktionstid er "); 

  Serial.println(stopTid - startTid); 

} 

  //beregn gennemsnit 

  int middel = 0; 

  for(int i = 0; i < 5; i++){ 

    middel = middel + reaktionstid[i]; 

  } 

  Serial.print("Din gennemsnits reaktions tid er "); 

  Serial.println(middel/5); 

  Serial.println("Ny omgang"); 

  delay(10000);
} 