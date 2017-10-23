#include "mbed.h"

#define DELTA 200
#define DELTAMOTOR 0.2
#define ACCELERATION 0.2
#define BASE_ACCELERATION 0.2
#define DELTATEMPS 0.1
#define DELTAMOTORCORRECTION 0.04
#define POWER_LIMIT 0.8

class Counter {
  public:
    Counter(PinName pin) : _interrupt(pin) {        // create the InterruptIn on the pin specified to Counter
      _interrupt.rise(this, &Counter::increment); // attach increment function of this counter instance
    }
    void increment() {
      _count++;
    }
    int read() {
      return _count;
    }
    void reset() {
      _count=0;
    }
  private:
    InterruptIn _interrupt;
    volatile int _count;
};

int setTimePush(float timelapsed, int timePush) {
  if (timelapsed == 1.0) {
    return 1;
  } else if (timelapsed == 2.0) {
    return 2;
  } else if (timelapsed == 3.0) {
    return 3;
  } else if (timelapsed == 4.0) {
    return 4;
  } else {
    return timePush;
  }
}

Serial pc(USBTX, USBRX);
PwmOut myled(LED1);

Counter counterGauche1(PB_13);
Counter counterGauche2(PB_1);
Counter counterDroite1(PB_14);
Counter counterDroite2(PB_2);

PwmOut pwmDroite(D10);
DigitalOut dirDroite(D8);
PwmOut pwmGauche(D9);
DigitalOut dirGauche(D7);

int cG1=0;
int cD1=0;
int cG2=0;
int cD2=0;

float power_Droite = 0.0;
float power_Gauche = 0.0;
float power_Droite_Correctif = 0.0;
float power_Gauche_Correctif = 0.0; 
int main() {
  myled=1;wait(.5);
  myled=0;wait(.5);
  myled=1;wait(.5);
  myled=0;wait(.5);
  myled=1;wait(.5);

  pc.printf("Hello!\n");

  pwmDroite.period_us(1000);
  pwmGauche.period_us(1000);
  dirDroite=0;
  dirGauche=1;

  pwmDroite.pulsewidth_us(0);
  pwmGauche.pulsewidth_us(0);
  char c='$';
  char oldCharacter = '$';
  float timelapsed = 0.0;
  int timePush = 0;

  while(1) {
    c='$';
    //pc.printf("===DEBUT====\n");
    cG1 = counterGauche1.read();
    cD1 = counterDroite1.read();
    cG2 = counterGauche2.read();
    cD2 = counterDroite2.read();
    
    if (pc.readable()) {
      c=pc.getc();
      if (c == oldCharacter) {
        timelapsed += DELTATEMPS;
        timePush = setTimePush(timelapsed, timePush);
      } else {
        oldCharacter = c;
        timePush = 0;
        timelapsed = 0;
      }
      pc.printf("  >>>counterGauche1, counterGauche2, counterDroite1, counterDroite2, timelapsed: %d, %d, %d, %d, %d\n",cG1,cG2,cD1,cD2, timelapsed);
    } else {
      oldCharacter = '$';
      timePush = 0;
      timelapsed = 0;
    }

//DEBUT ANCIEN CODE CORRECTIF
//    //droite a plus tourne; on peut augmenter gauche
//    bool gaucheUp = cD>cG+DELTA && power_Gauche<POWER_LIMIT;
//    //droite a plus tourne; on ne peut plus augmenter gauche
//    bool droiteDown = cD>cG+DELTA && power_Droite >= DELTAMOTOR ;//&& (!gaucheUp);
//    //gauche a plus tourne; on peut augmenter droite
//    bool droiteUp = cG>cD+DELTA && power_Droite<POWER_LIMIT;
//    //gauche a plus tourne; on ne peut plus augmenter droite
//    bool gaucheDown = cG>cD+DELTA && power_Droite>= DELTAMOTOR ;//&& (!droiteUp);
//
//    power_Droite_Correctif = 0;
//    power_Gauche_Correctif = 0;
//    //if (power_Gauche !=0.0 && power_Droite != 0.0){
//    if (droiteUp) power_Droite_Correctif += DELTAMOTORCORRECTION;
//    if (droiteDown) power_Droite_Correctif -= DELTAMOTORCORRECTION;
//    if (gaucheUp) power_Gauche_Correctif += DELTAMOTORCORRECTION;
//    if (gaucheDown) power_Gauche_Correctif -= DELTAMOTORCORRECTION;
//    //}
//
//    // pc.printf("correction droite: %f\n",(float)(1+((cG-cD)/DELTA)));
//    //        pc.printf("powerDroite,powerGauche: %f,%f\n",power_Droite+power_Droite_Correctif,power_Gauche+power_Gauche_Correctif);
//    //       pc.printf("gaucheUp, gaucheDown, droiteUp, droiteDown :%d, %d, %d, %d\n",gaucheUp,gaucheDown,droiteUp,droiteDown);
//FIN ANCIEN CODE CORRECTIF

    if((c == 'q') && (power_Gauche < POWER_LIMIT)) {
      power_Gauche += DELTAMOTOR;
      power_Gauche_Correctif = BASE_ACCELERATION;
      pc.printf("Gauche Up!\n");
    }
    if((c == 'a') && (power_Gauche > -POWER_LIMIT)) {
      power_Gauche -= DELTAMOTOR;
      power_Droite_Correctif = BASE_ACCELERATION;
      pc.printf("Gauche Down!\n");
    }
    if((c == 'd') && (power_Droite < POWER_LIMIT)) {
      power_Droite += DELTAMOTOR;
      power_Droite_Correctif = BASE_ACCELERATION;
      pc.printf("Droite Up!\n");
    }
    if((c == 'e') && (power_Droite > -POWER_LIMIT)) {
      power_Droite -= DELTAMOTOR;
      power_Droite_Correctif = BASE_ACCELERATION;
      pc.printf("Droite Down!\n");
    }
    if((c == 'z') && (power_Droite < POWER_LIMIT) && (power_Gauche < POWER_LIMIT)) {
      power_Droite += DELTAMOTOR;
      power_Gauche += DELTAMOTOR;
      power_Droite_Correctif = ACCELERATION * timePush;
      power_Gauche_Correctif = ACCELERATION * timePush;
      pc.printf("Gauche Droite Up!\n");
    }
    if((c == 's') && (power_Droite > -POWER_LIMIT)&& (power_Gauche > -POWER_LIMIT)) {
      power_Droite -= DELTAMOTOR;
      power_Gauche -= DELTAMOTOR;
      power_Droite_Correctif = BASE_ACCELERATION;
      power_Gauche_Correctif = BASE_ACCELERATION;
      pc.printf("Gauche Droite Down!\n");
    }
    if((c == 'b') || (c == ' ')) {
      power_Droite = 0;
      power_Gauche = 0;
      counterGauche1.reset();
      counterGauche2.reset();
      counterDroite1.reset();
      counterDroite2.reset();
      pc.printf("Stop!\n");
    } 

    if(power_Droite>0){
      dirDroite=0;
      pwmDroite.pulsewidth_us(1000*(power_Droite+power_Droite_Correctif));
    } else {
      dirDroite=1;
      pwmDroite.pulsewidth_us(-1000*(power_Droite+power_Droite_Correctif));
    }
    if(power_Gauche>0){
      dirGauche=1;
      pwmGauche.pulsewidth_us(1000*(power_Gauche+power_Gauche_Correctif));
    } else {
      dirGauche=0;
      pwmGauche.pulsewidth_us(-1000*(power_Gauche+power_Gauche_Correctif));
    }

    //pc.printf("===FIN====\n");
    wait(DELTATEMPS);
  }
}

