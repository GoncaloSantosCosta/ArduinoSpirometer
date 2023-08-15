#include "MCP3221.h"
#include<LiquidCrystal.h>

const byte DEV_ADDR = 0x4D;                       // I2C address of the MCP3221
MCP3221 mcp3221(DEV_ADDR);

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

//Variáveis
float inputV = 0;
float V = 0;
float P = 0;
float P_err = 0;
float Q = 0;

//Constantes
float ro = 1.284; // densidade do ar em kg/m3 para 20ºC
float pi=3.1415;
float a1 = pi*(0.016*0.016); //diametro: 32mm
float a2 = pi*(0.006*0.006); //diametro: 12mm
float dt = 0.1;
const byte buttonPin = 7;

//LCD, Sensor e Botão
void setup()
{
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcd.print("Iniciando...");
  delay(3000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("FVC = ");
  lcd.setCursor(0,1);
  lcd.print("FEV1 = ");
  Serial.begin(9600);
  Wire.begin();
  Wire.setClock(100000);
  pinMode(buttonPin, INPUT);
}

void buttonWait(int buttonPin)
{
  int buttonState = 0;
  while(1){
    buttonState = digitalRead(buttonPin);
    if (buttonState == HIGH) {
      return;
    }
  }
}

//Algoritmo
void loop()
{
  int t = 0;  // contador
  float Vol = 0;
  float FVC = 0.00;
  float FEV1 = 0.00;
  //float pressum = 0;
  
  buttonWait(7);
  while(digitalRead(7) == HIGH){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Medindo...");
      inputV = mcp3221.getVoltage();
      V = inputV*5.0/4096.0; //em V
      P_err = abs(((V/5.0-0.5)/0.057)*1000.0);
      P=abs(P_err-40.45); 
      Q = 1000.0*a1*sqrt((2.0*P/(ro*abs(pow(a1/a2,2)-1))));
      Vol = Q*dt + Vol;
      if (t==19) { //começa no 0
        FEV1 = Vol;
      }
      else {
        FVC = Vol;
      }
      t = t + 1;
      //pressum = pressum + P_err;
      delay(50); //50ms=0.05s 
   }
  
  //Serial.println(pressum/t);
  
  lcd.setCursor(0,0);
  lcd.print("FVC = ");
  lcd.print(FVC,2);
  lcd.setCursor(10,0);
  lcd.print(" L");
  lcd.setCursor(0,1);
  lcd.print("FEV1 = ");
  lcd.print(FEV1,2);
  lcd.setCursor(11,1);
  lcd.print(" L");
  delay(5000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("FEV% = ");
  lcd.print((FEV1/FVC)*100.0);
  lcd.setCursor(11,0);
  lcd.print(" %");
  lcd.setCursor(0,1);
  lcd.print("Normal aprox 75%");
    
  delay(5000);
}
