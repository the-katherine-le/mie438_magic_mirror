//https://forum.arduino.cc/t/press-a-button-display-a-random-quote/92459/3

#include <avr/pgmspace.h>

prog_char s1[] PROGMEM = "It is certain";
prog_char s2[] PROGMEM = "It is decidedly so";
prog_char s3[] PROGMEM = "Without a doubt";
prog_char s4[] PROGMEM = "Yes - definitely";
prog_char s5[] PROGMEM = "You may rely on it";
prog_char s6[] PROGMEM = "As I see it, yes";
prog_char s7[] PROGMEM = "Most likely";
prog_char s8[] PROGMEM = "Outlook good";
prog_char s9[] PROGMEM = "Signs point to yes";
prog_char s10[] PROGMEM = "Yes";
prog_char s11[] PROGMEM = "Reply hazy, try again";
prog_char s12[] PROGMEM = "Ask again later";
prog_char s13[] PROGMEM = "Better not tell you now";
prog_char s14[] PROGMEM = "Cannot predict now";
prog_char s15[] PROGMEM = "Concentrate and ask again";
prog_char s16[] PROGMEM = "Don't count on it";
prog_char s17[] PROGMEM = "My reply is no";
prog_char s18[] PROGMEM = "My sources say no";
prog_char s19[] PROGMEM = "Outlook not so good";
prog_char s20[] PROGMEM = "Very doubtful";

// this table is in ordinary memory but has pointers
// that reference the flash PROGMEM
const char *str_tab[] = {
s1, s2, s3, s4, s5, s6, s7, s8, s9, s10,
s11,s12,s13,s14,s15,s16,s17,s18,s19,s20 };
const int Number_of_fortunes=20;
#define Longest_fort 100 // buffer size for messages

long randNum;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  randNum = random(0,19);
  Serial.println(str_tab[randNum]);
  delay(500);
}

void loop() {
  // put your main code here, to run repeatedly:

}
