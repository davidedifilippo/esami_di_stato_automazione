// Definizione dei Pin di ingresso
const int pinP1 = 2;
const int pinP2 = 3;
const int pinP3 = 4;
const int pinSP = 5;
const int pinFC = 6;
const int pinPirani = A0; // Ingresso analogico

// Definizione dei Pin di uscita
const int pin_CilindroPneumatico = 8;
const int pin_MAT_Nastro = 9;
const int pin_MAT_Tavola = 10;
const int pin_Lampada = 11;
const int pin_MAT_Pompa_Vuoto = 12;
const int pin_Riscaldatore = 13;

// Variabili di stato
int CP = 0; // Contatore pezzi
int ContaS = 0; // Contatore satelliti
bool stato_precedente_SP = LOW; // Per rilevare il fronte di discesa
unsigned long startMetallizzazione = 0;
bool timerActive = false;
const unsigned long timeout = 300000; // 5 minuti in millisecondi

void setup() {
  // Setup input...
  pinMode(pinP1, INPUT);
  pinMode(pinP2, INPUT);
  pinMode(pinP3, INPUT);
  pinMode(pinSP, INPUT);
  pinMode(pinFC, INPUT);
  // Setup output...
  pinMode(pin_MAT_Nastro , OUTPUT);
  pinMode(pin_MAT_Tavola, OUTPUT);
  pinMode(pin_MAT_Pompa_Vuoto, OUTPUT);
  pinMode(pin_Lampada, OUTPUT);
  pinMode(pin_Riscaldatore, OUTPUT); 
  pinMode(pin_CilindroPneumatico, OUTPUT);
}

void loop() {
  // Lettura ingresso
  bool P1 = digitalRead(pinP1);
 
  // --- Fase di caricamento ---
  if (P1) {
    digitalWrite(pin_MAT_Nastro, HIGH); 
    digitalWrite(pin_CilindroPneumatico, HIGH);
  }

  // --- Controllo lo stato della fotocellula ---
  bool SP = digitalRead(pinSP);
  
  if (!SP) {
    digitalWrite(pin_MAT_Nastro, HIGH); // Pezzo assente
  } else {
    digitalWrite(pin_MAT_Nastro, LOW);  // Pezzo presente
  }

  // --- Rilevamento fronte di discesa della fotocellula SP  ---
  if (stato_precedente_SP == HIGH && SP == LOW) {
    CP++;  //Icremento il numero di pezzi caricati
  }
  stato_precedente_SP = SP; // Aggiorno lo stato per il prossimo ciclo

   // --- Controllo lo stato del pulsante P2 ---
  bool P2 = digitalRead(pinP2);

  // --- Controllo se il satellite è pieno ---
  if (P2 && CP == 8) {
    CP = 0;
    digitalWrite(pin_MAT_Tavola, HIGH);
  }

// --- Controllo lo stato del fine corsa ---
  bool FC = digitalRead(pinFC);

  // --- Controllo l'allineamento del satellite---
  if (FC) {
    digitalWrite(pin_MAT_Tavola, LOW);
    ContaS++; //incremento il conta satelliti
  }

// --- Controllo lo stato del pulsante P3 ---
  bool P3 = digitalRead(pinP3);

  // --- Se i satelliti sono pieni ---
  if (P3 && ContaS == 4 && CP == 8) {
    digitalWrite(pin_Lampada, HIGH); //Accendo la lampada
    digitalWrite(pin_CilindroPneumatico, LOW); //Apro la porta della camera
    digitalWrite(pin_MAT_Pompa_Vuoto, HIGH);
    ContaS = 0; //resetto i contatori per il caricamento successivo
    CP = 0;
  }

  int Pirani = analogRead(0);
  // --- Controllo condizione di vuoto ---
  if (Pirani < 9830) {
    digitalWrite(pin_Riscaldatore, HIGH);
    digitalWrite(pin_MAT_Tavola, HIGH);
    digitalWrite(pin_MAT_Pompa_Vuoto, LOW); //Vuoto raggiunto
  }

  // --- Avvio del timer a pressione raggiunta ---
  if (Pirani > 9968 && !timerActive) {
    startMetallizzazione = millis(); // Mi segno il tempo 
  }

  // Controllo se il timeout è stato raggiunto
  if ( (millis() - startMetallizzazione >= timeout)) {
    digitalWrite(pin_Riscaldatore, LOW);
    digitalWrite(pin_MAT_Tavola, LOW);
    digitalWrite(pin_CilindroPneumatico, HIGH);
    digitalWrite(pin_Lampada, LOW);
  }
}
