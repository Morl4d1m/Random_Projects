#include <virtuabotixRTC.h>
// Creation of the Real Time Clock Object
virtuabotixRTC myRTC(6, 7, 8);
void setup()  {
  Serial.begin(115200);
  // Set the current date, and time in the following format:
  // seconds, minutes, hours, day of the week, day of the month, month, year
  myRTC.setDS1302Time(45, 34, 21, 3, 30, 11, 2022);
}
void loop()  {
  // This allows for the update of variables for time or accessing the individual elements.
  myRTC.updateTime();
  // Start printing elements as individuals
  Serial.print("Current Date / Time: ");
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
  Serial.println(myRTC.seconds);
  // Delay so the program doesn't print non-stop
  delay(1000);
}