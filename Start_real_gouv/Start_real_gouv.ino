//Test asservissement motor:
//Benjamin 01/2016

#include <stdio.h>
#include <stdlib.h>

int angleC=0;
int Rmax=10000;  //valeur pota
float alim=3.3;  //tension d'alimentation du pota

int motorpin1 = 7;       
int motorpin2 = 6;   


void setup() 
{
  Serial.begin(9600);
  Serial.println("Serial OK");
  pinMode(motorpin1,OUTPUT);        //set pin 3 as output
  pinMode(motorpin2,OUTPUT);        // set pin 4 as output
  
}

void loop()
{
//Get the consigne angle:
  if(Serial.available()>0)
  {
    angleC=Serial.parseInt();
    delay(200);
  Serial.print("Voici l'angle:");
  Serial.println(angleC);
  Serial.println("*************");
  }
  if(Serial.available()==0)
  {
    Serial.print("Pas d'angle de consigne");
  }
  
  //Measurement of the angle reel:
  int angleCbrut = analogRead(A0);
  //int angleR = map(angleCbrut,0,1023,-90,90); 
  Serial.print("pota : ");
  Serial.println(angleCbrut);
  Serial.print("angle converti par map: ");
  Serial.println(angleR);
  Serial.println("******************");
  
//Active le moteur DC tant que les 2 angles ne sont pas égaux
  if(angleC>10)
  {
      Serial.println("Sens 2");
      digitalWrite(motorpin1,LOW);
      digitalWrite(motorpin2,HIGH); 
   delay(200);   
  }
    else if(angleC<-10)
  {
      Serial.println("Sens 1");
      digitalWrite(motorpin1,HIGH);
      digitalWrite(motorpin2,LOW); 
   delay(200);   
  }
  else{
    Serial.println("Motor OFF");
      digitalWrite(motorpin1,LOW);
      digitalWrite(motorpin2,LOW); 
   delay(200);   
  }
  
  delay(800);

}
