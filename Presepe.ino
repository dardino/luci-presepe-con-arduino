int RED = 9;
int GREEN = 10;
int BLUE = 11;
#define RESOLUTION 255 //Numero di passi per il fde
#define FADE_SPEED 50 //ms per ogni passo del ciclo di fade

//In totale ogni fading dura 255 * 50ms = 12750 secondi

void setup() {
  //Per il debug
  //Serial.begin(19200);
  
  // Imposto le porte per la striscia led RGB:
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);

  analogWrite(RED, 0);
  analogWrite(GREEN, 0);
  analogWrite(BLUE, 0);
  delay(500);
}

unsigned long lastMills = 0;
int i = 0;
int fase = 0; //mi serve per tenere traccia della fase del giorno

void loop() {

  int r = 0, g = 0, b = 0;
  
  //Parte da buio ed arriva l'alba rosa
  if (fase == 0) {
    if (millis() - lastMills > FADE_SPEED) {
      i++;
      if (i <= RESOLUTION) {
        r = map(i, 0, RESOLUTION, 1, 255);
        g = map(i, 0, RESOLUTION, 1, 135);
        b = map(i, 0, RESOLUTION, 1, 150);
        /* Monitor seriale CTRL+SHIFT+L
        Serial.print(i);
        Serial.print(" ");
        Serial.print(r);
        Serial.print(" ");
        Serial.print(g);
        Serial.print(" ");
        Serial.println(b);
        */
        lastMills = millis();
        analogWrite(RED, r);
        analogWrite(GREEN, g);
        analogWrite(BLUE, b);
      }
      else {
        fase++;
        i = 0;
      }
    }
  }
  
  //Dall'alba arriva il giorno pieno
  if (millis() - lastMills > FADE_SPEED && fase == 1) {
    i++;
    if (i <= RESOLUTION) {
      r = map(i, 0, RESOLUTION, 255, 255);
      g = map(i, 0, RESOLUTION, 135, 255);
      b = map(i, 0, RESOLUTION, 150, 255);
      /* Monitor seriale CTRL+SHIFT+L
      Serial.print(i);
      Serial.print(" ");
      Serial.print(r);
      Serial.print(" ");
      Serial.print(g);
      Serial.print(" ");
      Serial.println(b);
      */
      lastMills = millis();
      analogWrite(RED, r);
      analogWrite(GREEN, g);
      analogWrite(BLUE, b);
    }
    else {
      delay(20000); //Per 20 sec il giorno pieno
      fase++;
      i = RESOLUTION;
    }
  }
  
   //Dal giorno al tramoto
  if (millis() - lastMills > FADE_SPEED && fase == 2) {
    i--;
    if (i >= 0) {
      r = map(i, 0, RESOLUTION, 255, 255);
      g = map(i, 0, RESOLUTION, 0, 255);
      b = map(i, 0, RESOLUTION, 0, 255);
      /* Monitor seriale CTRL+SHIFT+L
      Serial.print(i);
      Serial.print(" ");
      Serial.print(r);
      Serial.print(" ");
      Serial.print(g);
      Serial.print(" ");
      Serial.println(b);
      */
      lastMills = millis();
      analogWrite(RED, r);
      analogWrite(GREEN, g);
      analogWrite(BLUE, b);
    }
    else {
      fase++;
      i = RESOLUTION;
    }
  }

   //Dal tramoto al buio
  if (millis() - lastMills > FADE_SPEED && fase == 3) {
    i--;
    if (i >= 0) {
      r = map(i, 0, RESOLUTION, 0, 255);
      g = map(i, 0, RESOLUTION, 0, 0);
      b = map(i, 0, RESOLUTION, 0, 0);
      /* Monitor seriale CTRL+SHIFT+L
      Serial.print(i);
      Serial.print(" ");
      Serial.print(r);
      Serial.print(" ");
      Serial.print(g);
      Serial.print(" ");
      Serial.println(b);
      */
      lastMills = millis();
      analogWrite(RED, r);
      analogWrite(GREEN, g);
      analogWrite(BLUE, b);
    }
    else {
      delay(20000); //20 secondi di notte;
      fase = 0;
      i = 0;
    }
  }
 
}



