//SailDrone maquette code - 03/02/2016
// Benjamin and Remi
// The objectif of the following code is to run the drone simulateur and all of the electronic part.

#include <Wire.h>
#include <ADXL345.h>
#include <math.h>
#include <Servo.h>

int angleC=0; //Angle de cinsigne
int Rmax=10000; //valeur pota
float alim=3.3; //tension d'alimentation du pota
int precision =10; //précision de "l'asservissement"
int anglevent=0;
int anglevoile=0;
int val =0;
float coefvoile=0.0; //Correcteur en fonction de la force du vent
int motorpin1 = 7;
int motorpin2 = 6;
int pinVoile = 3;

ADXL345 adxl; //variable adxl is an instance of the ADXL345 library

int alarmg=8; //led de signalisation trop de gite !
int ventNormal = 11; //Leds de force du vent
int ventFort = 12;
int ventExtreme = 13;
int boutonVent = 10;
int gammin=46; //angles limites du vent par rapport au bateau pour assurer une avancee convenable
int gammax=168;
int alphamin = 14; //Angles limites de position de la voile
int alphamax = 90;

Servo servoVoile;
void setup()
{
  Serial.begin(9600);
  Serial.println("Serial OK");

  pinMode(motorpin1,OUTPUT);
  pinMode(motorpin2,OUTPUT);
  pinMode(alarmg, OUTPUT);
  pinMode(ventNormal, OUTPUT);
  pinMode(ventFort, OUTPUT);
  pinMode(ventExtreme, OUTPUT);
  pinMode(boutonVent, INPUT);
  digitalWrite(11,HIGH);

  adxl.powerOn();
  adxl.setActivityThreshold(75); //62.5mg per increment
  adxl.setInactivityThreshold(75); 
  adxl.setTimeInactivity(10); 
  //look of activity movement on this axes - 1 == on; 0 == off
  adxl.setActivityX(1);
  adxl.setActivityY(1);
  adxl.setActivityZ(1);

  servoVoile.attach(pinVoile); //sortie pwm pour la voile
}

void loop()
{
  //On recupere l’angle de consigne:
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
  int angleCbrut = analogRead(A0); //On récupère la valeur du potentiomètre du gouvernail
  int angleR = map(angleCbrut,0,1023,-180,180);
  Serial.print("angle converti par map: ");
  Serial.println(angleR);
  Serial.println("******************");
  Serial.println(" ");

  //"Asservissement":
  int e= abs(angleC)-abs(angleR);
  e=abs(e);
  Serial.print("Voici e= ");
  Serial.println(e);
  Serial.println("**********************");

  int M=angleC*angleR; //vérifier que les angles sont de meme signe.
  if(e>=precision || M<0)
    {
      Serial.println("Motor ON");
      digitalWrite(motorpin1,LOW);
      digitalWrite(motorpin2,HIGH);
    }
  if(e<=precision && M>0)
    {
      Serial.println("Motor OFF");
      digitalWrite(motorpin1,LOW);
      digitalWrite(motorpin2,LOW);
    }

   //Accelerometer :
  int x,y,z;
  double pitch,roll,cap;
  adxl.readXYZ(&x, &y, &z); //read the accelerometer values and store them in variables x,y,z
  double xyz[3];
  double ax,ay,az;
  adxl.getAcceleration(xyz);
  ax = xyz[0];
  ay = xyz[1];
  az = xyz[2];
  pitch = (atan2(ax,sqrt(ay*ay+az*az)) * 180.0) / PI;
  roll = (atan2(ay,sqrt(ax*ax+az*az)) * 180.0) / PI;
  Serial.print("Pitch :"); //suivant axe X;
  Serial.println(pitch);
  Serial.print("Roll :"); //suivant axe Y;11
  Serial.println(roll);
  Serial.println("**********************");
  if(roll>35 ||roll<-35)
    {
      digitalWrite(alarmg, HIGH);
    }
  else
    {
      digitalWrite(alarmg, LOW);
    }

  //MAT:
  //On lit l'angle du vent sur le deuxième potentiomètre
  anglevent=map(analogRead(A1),0,1023,0,179);
  if(digitalRead(10)==HIGH) //Reglage de la force du vent
    {
      if(digitalRead(11)==HIGH)
        {
          digitalWrite(11,LOW);
          digitalWrite(12,HIGH);
          coefvoile=0.6; //Vent fort, on réduit la prise au vent de l'aile
        }
     else if(digitalRead(12)==HIGH)
        {
          digitalWrite(12,LOW);
          digitalWrite(13,HIGH);
          coefvoile=0; //Vent extrême, on garde l'aile face au vent
        }
    else if(digitalRead(13)==HIGH)
        {
          digitalWrite(13,LOW);
          digitalWrite(11,HIGH);
          coefvoile=1; //Vent normal, on positionne l'aile afin de maximiser la vitesse
        }
    delay(500); //Anti-rebond pour le bouton poussoir
  } 
  anglevoile=actvoile(anglevent,coefvoile);
  servoVoile.write(anglevoile);
}

int actvoile(int vent, float coef) //Fonction donnant l'angle de la voile en fonction de la direction du vent et du mode de fonctionnement
{
  if (0<=vent<gammin)
    {
      val=alphamin*coef;
    }
  else if (gammin<=vent<gammax)
    {
      val=(vent*0.623-14.656)*coef;
    }
  else if (gammax<=vent<180)
    {
      val=alphamax*coef;
    }
    
  if (val==0) return vent; //On pilote la voile tant que le vent n'est pas extrême, sinon la voile se met face au vent
  else return val;
}
