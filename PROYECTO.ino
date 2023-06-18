
#include <SPI.h>      //  Libreria SPI
#include <MFRC522.h>      // Libreria  MFRC522
#include <Servo.h>     // Libreria  SERVO

#define RST_PIN  9      // Pin de reset
#define SS_PIN  10      // Pin de slave select

MFRC522 mfrc522(SS_PIN, RST_PIN); // Objeto mfrc522 enviando pines de slave select y reset

byte LecturaUID[4];         // Array para almacenar el UID leido
byte Usuario1[4]= {0x83, 0x5C, 0x5A, 0x40} ;    // NUMERO DEL USUARIO 1 
byte Usuario2[4]= {0xE7, 0x36, 0xBC, 0x7B} ;    // NUMERO DEL USUARIO 2 
int analogo5=0;                                 // Variable de lectura del Analogo5 para sensor de obstaculos
Servo servoPT;                                 // Asocia la libreria servo a  servoPT

void setup() {
  pinMode(7, OUTPUT);
  pinMode(3, OUTPUT);
  Serial.begin(9600);     // inicializa comunicacion por monitor serie a 9600 bps
  servoPT.attach(4);      // Sevo asociado al pin 4 y lleva a 170 grados
  servoPT.write(200);
  SPI.begin();        // inicializa bus SPI
  mfrc522.PCD_Init();     // inicializa modulo lector
  Serial.println("--- CONTROL DE ACCESO ---");    // Muestra texto Listo
}

void loop() {
    analogo5=analogRead(A5);
    if ( analogo5<=800) {                             // Si el sensor de obtaculos fue obstruido abre la puerta
        Serial.println("AGUJA ABIERTA");
         abrirPuerta();                           
     }
 
  if ( ! mfrc522.PICC_IsNewCardPresent())   // si no hay una tarjeta presente
    return;           // retorna al loop esperando por una tarjeta
  
  if ( ! mfrc522.PICC_ReadCardSerial())     // si no puede obtener datos de la tarjeta
    return;           // retorna al loop esperando por otra tarjeta
    
    Serial.print("UID:");       // muestra texto UID:
    for (byte i = 0; i < 4; i++) { // bucle recorre de a un byte por vez el UID
      if (mfrc522.uid.uidByte[i] < 0x10){   // si el byte leido es menor a 0x10
        Serial.print(" 0");       // imprime espacio en blanco y numero cero
        }
        else{           // sino
          Serial.print(" ");        // imprime un espacio en blanco
          }
          Serial.print(mfrc522.uid.uidByte[i], HEX);    // imprime el byte del UID leido en hexadecimal
          LecturaUID[i]=mfrc522.uid.uidByte[i];     // almacena en array el byte del UID leido      
          }
          
          Serial.print("\t");         // imprime un espacio de tabulacion             
                    
          if(comparaUID(LecturaUID, Usuario1)){    // llama a funcion comparaUID con Usuario1
            Serial.println("ACCESO APROBADO"); // si retorna verdadero muestra texto bienvenida
            abrirPuerta();                      // funcion para abrir la puerta
          }
          else if(comparaUID(LecturaUID, Usuario2)){ // llama a funcion comparaUID con Usuario2
            Serial.println("ACCESO APROBADO"); // si retorna verdadero muestra texto bienvenida
             abrirPuerta();                      // funcion para abrir la puerta
          }
           else {          // si retorna falso
            Serial.println("ACCESO NEGADO");    // muestra texto equivalente a acceso denegado          
             Mal_Registro();                    // funcion sonido de targeta no identificada
           }  
                  mfrc522.PICC_HaltA();     // detiene comunicacion con tarjeta    

   
}

void abrirPuerta() {
  digitalWrite(5, HIGH);   // Abrir la cerradura
  delay(1000);
  servoPT.write(90);       // Abrir la puerta 
  delay(3000);             // Tiempo de la puerta abierta
  digitalWrite(7, HIGH);   // Suena el buzzer para indicar que se va a cerrar la puerta
  delay(500);
  servoPT.write(190);       // Cierra puerta 
  delay(500);
  digitalWrite(7, LOW);     // apaga el buzzer 
  digitalWrite(5, LOW);     // cierra la cerradura 
}


void Mal_Registro() {      // Activa el Buzzer 2 veces por tarjeta no autorizada
  digitalWrite(7, HIGH);
  delay(200);
  digitalWrite(7, LOW);
  delay(100);
  digitalWrite(7, HIGH);
  delay(200);
  digitalWrite(7, LOW);
}

boolean comparaUID(byte lectura[],byte usuario[]) // funcion comparaUID
{
  for (byte i=0; i < 4; i++){    // bucle recorre de a un byte por vez el UID
  if(lectura[i] != usuario[i])        // si byte de UID leido es distinto a usuario
    return(false);          // retorna falso
  }
  return(true);           // si los 4 bytes coinciden retorna verdadero
}
