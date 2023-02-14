// Autor: Rafał Olech
// Temat: Program obsługujący autonomiczny system monitorujący wyciek gazu w instalacjach gazowych LPG oraz CNG/LNG

// wymagane biblioteki
#include <LiquidCrystal.h> 
#include <SoftwareSerial.h>

const int rs=11, en=10, d4=5, d5=4, d6=3, d7=2; //stałe-numery pinów, do których podłączony jest LCD
LiquidCrystal lcd(rs, en, d4, d5, d6, d7); //konstruktor dla LiquidCrystal

SoftwareSerial mySerial(12, 13);
String cmd = "";

// przypisanie portów do komponentów 
int upperSensor = A0;
int lowerSensor = A1;
int buzzer = 9;
const int fan = 8;
int diode = 7;

// zmienne globalne 
float upperSensorValue = 0;
float lowerSensorValue = 0;
float sensorValue = 0;
float ppmValue = 0;
int fanSpeed = 0;

// zmienne potrzebne do obliczenia stężenia gazu
float RS_do_RO_upper;
float RS_do_RO_lower;
float RO_upper;
float RO_lower;
float RO;
float RL_upper = 22.0;
float RL_lower = 22.0;
float sensor_volt_upper;
float sensor_volt_lower;
float PPM_upper;
float PPM_lower;

// zmienne potrzebne do kalibracji czujników 
double number_of_calibration_samples = 60;
float air_purity_factor = 6.5;

// zmienne odpowiedzialne za odmierzanie czasu 
unsigned long millis1s;    //czasomierz co 1s
unsigned long millisAlarm; //czasomierz dla buzzera
unsigned long millisExceeded=1; //aktualnie przekroczony, ile czasu ma dzialac buzzer
unsigned long reportTimer=0;  // czasomierz raportów

// flagi stanowe
byte alarmValue=1; //modulacja buzzera
byte alarmDir=1;   //kierunek zmian modulacji
byte alarm=0;      //czy buzzer dziala
byte currentlyExceeded=0; //czy teraz jest przekroczone dopuszczalne stężenie gazu
byte smsState=0;  //stan wysylania sms - 0 - nie wysyla, 1,2,3,4 - wysylanie 5 - po wyslaniu

// deklaracje funkcji
void heatingUp();
void gasAlert(float ppmValue);
float sensorCalibration(int sensor);

void setup() 
{
  // Konfiguracja określonych pinów jako wyjściowe
  pinMode(buzzer, OUTPUT); 
  pinMode(fan, OUTPUT); 
  analogWrite(fan, fanSpeed); 
  pinMode(diode, OUTPUT); 

  // Kontrolne sprawdzenie działania wszystkich urządzeń 
  digitalWrite(diode, HIGH);
  digitalWrite(fan, HIGH);
  digitalWrite(buzzer, HIGH);
  delay(200);
  digitalWrite(diode, LOW);
  digitalWrite(fan, LOW);
  digitalWrite(buzzer, LOW);
  
  // początkowe wyświetlenie na lcd
  lcd.begin(16, 2); 
  lcd.home();  
  lcd.setCursor(0,0);
  lcd.print("Gas monitoring "); 
  lcd.setCursor(3,1);
  lcd.print("system"); 
  delay(3000);

  mySerial.begin(9600);
  Serial.begin(9600);
  heatingUp();
  RO_upper = sensorCalibration(upperSensor);
  RO_lower = sensorCalibration(lowerSensor);
  lcd.clear();
  lcd.println(RO_upper);
  lcd.print(RO_lower);
  delay(500);
}



void loop() 
{
  upperSensorValue = analogRead(upperSensor); // przypisanie odczytanej wartości z czujnika do zmiennej upperSensorValue
  sensor_volt_upper = ((1024-upperSensorValue)/upperSensorValue)*RL_upper;
  RS_do_RO_upper = sensor_volt_upper/RO_upper;
  PPM_upper = 6.2*(pow(10, (((-2.512) * log10(RS_do_RO_upper)) + 1.912 )));

  lowerSensorValue = analogRead(lowerSensor); // przypisanie odczytanej wartości z czujnika do zmiennej lowerSensorValue
  sensor_volt_lower = ((1024-lowerSensorValue)/lowerSensorValue)*RL_lower;
  RS_do_RO_lower = sensor_volt_lower/RO_lower;
  PPM_lower = 6.2*(pow(10, (((-2.512) * log10(RS_do_RO_lower)) + 1.912 )));
   
   if (millis()-reportTimer>300)
   {
     reportTimer=millis();
     // wyswietlenie odczytanych wartości z czujników w aplikacji mobilnej
     Serial.print(PPM_lower);
     Serial.print(",");  
     Serial.print(PPM_upper);
     Serial.println("");
       

    // wypisanie wartości z czujników na lcd 
    lcd.setCursor(0,0);
    lcd.print("UpperSensor:");
    lcd.setCursor(12,0);
    lcd.print(PPM_upper);  
    

    lcd.setCursor(0,1);
    lcd.print("LowerSensor:");
    lcd.setCursor(12,1);
    lcd.print(PPM_lower);
  }
  gasAlert(PPM_upper);
  gasAlert(PPM_lower);
}

// funkcja odpowiedzialna za odczekanie czasu potrzebnego na rozgrzanie czujników 
void heatingUp()
{
  for (int time=10; time>0; time--)
  {
    lcd.setCursor(0,0);
    lcd.print("Heating up...");
    lcd.setCursor(0,1);
    lcd.print("Remains: ");
    lcd.print (time);
    lcd.print(" s");
    delay(1000);
    lcd.clear();
  }    
}

float sensorCalibration(int sensor)
{
  //kalibracja czujnika
  float sum=0;
  for (int i=0; i<number_of_calibration_samples; i++)
  {
    sensorValue = analogRead(sensor);
    if(sensor == upperSensor)
    {
		  RO = ((1024 - sensorValue) / sensorValue) * RL_upper;
    }
    else
    {
      RO = ((1024 - sensorValue) / sensorValue) * RL_lower;      
    }
    sum = sum + RO;
  }
  RO = sum/number_of_calibration_samples;
	RO = RO/air_purity_factor;

  return RO;    
}

  
void gasAlert(float ppmValue)
{
  //sprawdzenie czujnika
  if (ppmValue > 2000.0)
  {
    millisExceeded=millis()+6000;
    currentlyExceeded=1;
    if (smsState==0) smsState=1;
  } 
  else
  {
    currentlyExceeded=0;
  }

    //obsluga za wysokiego stezenia gazu
  if (millis()<millisExceeded)
  {
    alarm=1;
    fanSpeed = 255; 
    analogWrite(fan, fanSpeed); //maksymalna prędkość wentylatora
     
  }
  else
  { 
    alarm=0;
    fanSpeed = 0; 
    analogWrite(fan, fanSpeed); // wyłączenie wentylatora
    
    if (smsState==5) smsState=0;
  }

  //Czasomierz sekundowy
  if(millis()-millis1s>1000)
  {
    millis1s=millis();
    //rzeczy do wykonania co 1s

    
    if (currentlyExceeded) //led, ktory miga tylko gdy stezenie jest przekroczone
    {
      digitalWrite(diode, HIGH); //dioda świeci 
    }
    else
    {
      digitalWrite(diode, LOW); //dioda nie świeci 
    }
 
    switch(smsState)
    {
        case 1:
        //pierwszy krok
        mySerial.println("AT+CMGF=1"); // to send SMS in text Mode 
        smsState++;
        break;

        case 2:
        //drugi krok
        mySerial.println("AT+CMGS=\"+48787749368\"\r"); //change to the phone number you using
        smsState++;
        break;

        case 3:
        //trzeci krok
        mySerial.println("Wyciek gazu!"); 
        smsState++;
        break;

        case 4:
        //czwarty krok
        mySerial.println((char)26); 

        smsState++;
        break;
    }
  }

  if (millis()-millisAlarm>2)
  {
    millisAlarm=millis();
    if (alarm>0)
    {
        tone(buzzer,600+alarmValue);
        alarmValue+=alarmDir;
        if (alarmValue==0)
        {
          alarmDir=1;
        }
        if (alarmValue==255)
        {
          alarmDir=-1;
        }
    }
    else
    {
      analogWrite(buzzer,0);
      noTone(buzzer);
      alarmValue=1;
      alarmDir=1;
    }
  }
}
