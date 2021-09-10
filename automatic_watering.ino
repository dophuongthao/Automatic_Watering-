
#include <SoftwareSerial.h>
#include <dht11.h>
#define RX 3
#define TX 2
#define DEBUG true  
#define DHT_PIN 11
// tạo đối tượng dhtObject
dht11 dhtObject;
// tên Wifi local 
String gAp = "CaoPhuong";  
//password wifi      
String gPass = "66669999"; 
// write API của thinkspeak 
String gApi = "UYJZXNCC0IXH1MEQ";
String gHost = "api.thingspeak.com";
String gPort = "80";
int CountTrueCommand;
int CountTimeCommand; 
boolean found = false; 
SoftwareSerial esp8266(TX,RX);

  
void setup()
{
  Serial.begin(9600);
  esp8266.begin(115200);
  //reset cho ESP8266
  SendCommand("AT+RST",2000,"OK");
  // chọn chức năng station cho esp
  SendCommand("AT+CWMODE=1",3000,"OK");
  //kết nối với wifi 
  SendCommand("AT+CWJAP=\""+ gAp +"\",\""+ gPass +"\"",20,"OK");
  // chân nối led
  pinMode (13, OUTPUT);
  //chế độ mặc định của Led 
  digitalWrite (13, LOW);
  // xem ip là bao nhiêu 
  SendCommand("AT+CIFSR\r\n", 1000,"OK");
  //configure for multiple connection
  SendCommand( "AT+CIPMUX=1\r\n", 1000,"OK");
}

void loop() 
{
 String getData = "GET /update?api_key="+ gApi +"&field1="+GetTemperatureValue()+"&field2="+GetHumidityValue();
 SendCommand("AT+CIPMUX=1",5,"OK");
 //thiết lập kết nối TCP
 SendCommand("AT+CIPSTART=0,\"TCP\",\""+ gHost +"\","+ gPort,15,"OK");
 // gửi dữ liệu thông qua ICP/IP
 SendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">");
 esp8266.println(getData);
 delay(1500);
 CountTrueCommand++;
 //kết thúc kết nối TCP/IP
 SendCommand("AT+CIPCLOSE=0",5,"OK");
 String a = GetTemperatureValue();
 String b = GetHumidityValue();
 if(( a.toInt()>20)||(b.toInt()<40))
 {
  digitalWrite(13,HIGH);
 }
 else 
 {
  digitalWrite(13,LOW);
 }
}


String GetTemperatureValue()
{
   dhtObject.read(DHT_PIN);
   Serial.print(" Temperature(C)= ");
   int temp = dhtObject.temperature;
   Serial.println(temp); 
   delay(50);
   return String(temp); 
}


String GetHumidityValue(){
   dhtObject.read(DHT_PIN);
   Serial.print(" Humidity in %= ");
   int humidity = dhtObject.humidity;
   Serial.println(humidity);
   delay(50);
   return String(humidity); 
  
}

void SendCommand(String command, int MaxTime, char ReadReplay[]) {
  Serial.print(CountTrueCommand);
  //Serial.print(". at command => ");
  Serial.print(command);
  //Serial.print(" ");
  while(CountTimeCommand < (MaxTime*1))
  {
    esp8266.println(command);
    if(esp8266.find(ReadReplay))
    {
      found = true;
      break;
    }
  
    CountTimeCommand++;
  }
  
  if(found == true)
  {
    Serial.println("OK");
    CountTrueCommand++;
    CountTimeCommand = 0;
  }
  
  if(found == false)
  {
    Serial.println("Fail");
    CountTrueCommand = 0;
    CountTimeCommand = 0;
  }
  
  found = false;
 }
