#include <Wire.h>

int cont = 0;

void setup(){
  Serial.begin(9600);
  Wire.begin(0x08); //Inicia i2c con direccion esclava 0x08
  Wire.onRequest(requestEvent); //Evento para enviar datos
  pinMode(A0, INPUT);
}
  
void loop(){
  delay(10);
}
  
void requestEvent(){
    int value = analogRead(A0); //A0 0 1023 10 BITS
    Wire.write(value & 0xFF); //Byte menos significativo
    Wire.write((value >> 8) & 0xFF);
}
