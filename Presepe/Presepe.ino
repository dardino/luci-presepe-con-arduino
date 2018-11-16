#include <elapsedMillis.h>
#define DEBUG false

// 1. DEFINIZIONE DEI PIN ARDUINO
#define RED   3
#define GREEN 5
#define BLUE  6
#define STARS 10
#define CASE  12

// 2. DEFINIZIONE DELLA DURATA DEL CICLO
#define DayLength 5 // durata del ciclo giorno/notte (le 24H) in minuti reali

// 2. DEFINIZIONE DEGLI ORARI
//  2.1 CASE:
int CASE_ON [2]        { 18, 05 };
int CASE_OFF[2]        { 07, 45 };
//  2.2 STELLE           dalle       alle
int STELLE_IN [2][2] { { 17, 45 }, { 21, 45 } };
int STELLE_OUT[2][2] { { 05, 00 }, { 07, 15 } };
//  2.3 SOLE (led rgb notte->alba->giorno->tramonto->notte):
int ALBA    [2][2]   { { 05, 40 }, { 07, 00 } };
int TRAMONTO[2][2]   { { 17, 40 }, { 20, 00 } };

// 3. COLORI 
int rgbNotte[]    = {  0,   0,   0 }; // nero
int rgbAlba[]     = {255, 135, 150 }; // rosa
int rgbGiorno[]   = {255, 255, 255 }; // bianco
int rgbTramonto[] = {255,   0,   0 }; // rosso

elapsedMillis timeElapsed;
long fakeDayLength;
long minute;
// CALCOLO VALORI
#define GR 10 // granularità per migliorare i fade, consente un fade continuo (0-255) su (pseudo)25 minuti
long dayMinutes = (1440l * GR); // 24h * 60min * GR
// funzione che consente di ottenere i minuti con la granularità specificata
long toMin(int h, int m) { 
  return ((h * 60) + m) * GR; 
}

// calcolo 
long caseStart     = toMin(CASE_ON   [0]   , CASE_ON   [1]   );
long caseEnd       = toMin(CASE_OFF  [0]   , CASE_OFF  [1]   );
long starsStart0   = toMin(STELLE_IN [0][0], STELLE_IN [0][1]);
long starsStart1   = toMin(STELLE_IN [1][0], STELLE_IN [1][1]);
long starsEnd0     = toMin(STELLE_OUT[0][0], STELLE_OUT[0][1]);
long starsEnd1     = toMin(STELLE_OUT[1][0], STELLE_OUT[1][1]);
long albaStart     = toMin(ALBA      [0][0], ALBA      [0][1]);
long albaEnd       = toMin(ALBA      [1][0], ALBA      [1][1]);
long tramontoStart = toMin(TRAMONTO  [0][0], TRAMONTO  [0][1]);
long tramontoEnd   = toMin(TRAMONTO  [1][0], TRAMONTO  [1][1]);
long fadeStarsIn   = starsStart1 - starsStart0;
long fadeStarsOut  = starsEnd1   - starsEnd0  ;
long fadeAlba      = albaEnd     - albaStart  ;
long fadeTramonto  = tramontoEnd - tramontoStart;
void setup() {
  //Per il debug
  if (DEBUG) Serial.begin(19200);
  if (DEBUG) Serial.println(" initialization ... ");
  
  // Imposto le porte per la striscia led RGB:
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  pinMode(STARS, OUTPUT);
  pinMode(CASE, OUTPUT);

  writeColor(255,255,255);
  analogWrite(STARS, 255);
  digitalWrite(CASE, HIGH);

  delay(1000);

  writeColor(0,0,0);
  analogWrite(STARS, 0);
  digitalWrite(CASE, LOW);

  delay(1000);

  fakeDayLength = DayLength * 60l * 1000l; // es: 5 min * 60 secondi * 1000 millesimi

  if (DEBUG) {
    Serial.println("----------------------------");
    Serial.print("fakeDayLength: ");
    Serial.println(fakeDayLength);
    Serial.print("case    : ");
    Serial.print(caseStart);
    Serial.print(">");
    Serial.println(caseEnd);
    Serial.print("stelle  : ");
    Serial.print(starsStart0);
    Serial.print(">");
    Serial.print(starsStart1);
    Serial.print(" - ");
    Serial.print(starsEnd0);
    Serial.print(">");
    Serial.println(starsEnd1);
    Serial.print("notte   : 0>");
    Serial.println(albaStart);
    Serial.print("alba    : ");
    Serial.print(albaStart);
    Serial.print(">");
    Serial.println(albaEnd);
    Serial.print("giorno  : ");
    Serial.print(albaEnd);
    Serial.print(">");
    Serial.println(tramontoStart);
    Serial.print("tramonto: ");
    Serial.print(tramontoStart);
    Serial.print(">");
    Serial.println(tramontoEnd);
    Serial.print("notte   : ");
    Serial.print(tramontoEnd);
    Serial.print(">");
    Serial.println(1440*GR);
    Serial.println("----------------------------");
  }
  
  timeElapsed = 0;
  minute = 0;
  
  if (DEBUG) Serial.println(" done ");
}
int logger;
void loop() {
  if (timeElapsed >= fakeDayLength) { 
    Serial.println("");
    Serial.println("----------------------------");
    timeElapsed = 0; 
  }
  // minute = map(timeElapsed, 0l, fakeDayLength, 0l, 1440l * GR);
  minute = ((double)timeElapsed / (double)fakeDayLength) * dayMinutes;
  if (timeElapsed / 1000 != logger) { 
    logger = timeElapsed / 1000 ;
    Serial.print((double)timeElapsed / fakeDayLength);
    Serial.print("minuto: ");
    Serial.println(minute);
  }
  updateCase(minute);
  updateStelle(minute);
  updateSole(minute);
  
}

int r = 0;
int g = 0;
int b = 0;
void fadeColors(long _v, long _dur, int _rgbA[], int _rgbB[], String descr) {
  r = map(_v, 0, _dur, _rgbA[0], _rgbB[0]);
  g = map(_v, 0, _dur, _rgbA[1], _rgbB[1]);
  b = map(_v, 0, _dur, _rgbA[2], _rgbB[2]);
  logColor(descr, r, g, b);
  writeColor(r, g, b);
}
void writeColor(int _r, int _g, int _b) {
  analogWrite(RED, _r);
  analogWrite(GREEN, _g);
  analogWrite(BLUE, _b);
}
void logColor(String descr, int _r, int _g, int _b) {
  if (!DEBUG) return;
  Serial.print(descr);
  Serial.print(" R:"); Serial.print(_r);
  Serial.print(" G:"); Serial.print(_g);
  Serial.print(" B:"); Serial.print(_b);
  Serial.println("");
}


// update CASE
void updateCase(long m) {
  if (m <= caseStart && m > caseEnd) { // non è momento di illuminare le case
    digitalWrite(CASE, LOW);
    return;
  }
  digitalWrite(CASE, HIGH);
}

// update STELLE
void updateStelle(long m) {
  if (m < starsStart0 && m > starsEnd1) { // non è momento di illuminare le stelle
    analogWrite(STARS, 0);
    return;
  }
  if (m >= starsStart1 || m <= starsEnd0) { // stelle piena luminosità
    analogWrite(STARS, 255);
    return;
  }
  int starsV = 0; // value
  long starsO = 0; // offset dal punto di start del fade
  if (m >= starsStart0 && m < starsStart1) { // stelle in fade in
    starsO = m - starsStart0;
    starsV = map(starsO, 0, fadeStarsIn, 1, 255);
    analogWrite(STARS, starsV);
    return;
  }
  if (m >= starsEnd0 && m < starsEnd1) { // stelle in fade out
    starsO = fadeStarsOut - (m - starsEnd0);
    starsV = map(starsO, 0, fadeStarsOut, 1, 255);
    analogWrite(STARS, starsV);
    return;
  }
}

// update SOLE
void updateSole(long m) {
  if (m <= albaStart || m >= tramontoEnd) {
    writeColor(0, 0, 0);
    return;
  }
  if (m > albaStart && m < albaEnd) {
    if (m < albaStart + (fadeAlba / 2)) {
      fadeNotteAlba(m);
    } else {
      fadeAlbaGiorno(m);
    }
    return;
  }
  if (m >= albaEnd && m <= tramontoStart) {
    writeColor(255,255,255);
    return;
  }
  if (m > tramontoStart && m < tramontoEnd) {
    if (m < tramontoStart + (fadeTramonto / 2)) {
      fadeGiornoTramonto(m);
    } else {
      fadeTramontoNotte(m);
    }
    return;
  }
}
void fadeNotteAlba(long m) {
  long v = m - albaStart;
  fadeColors(v, fadeAlba / 2, rgbNotte, rgbAlba, "notte->alba");
}
void fadeAlbaGiorno(long m) {
  long v = m - (albaStart + (fadeAlba / 2));
  fadeColors(v, fadeAlba / 2, rgbAlba, rgbGiorno, "alba->giorno");
}
void fadeGiornoTramonto(long m) {
  long v = m - tramontoStart;
  fadeColors(v, fadeTramonto / 2, rgbGiorno, rgbTramonto, "giorno->tramonto");
}
void fadeTramontoNotte(long m) {
  long v = m - (tramontoStart + (fadeTramonto / 2));
  fadeColors(v, fadeTramonto / 2, rgbTramonto, rgbNotte, "tramonto->notte");
}

