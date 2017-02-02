/**
Created on Dez 02, 2016

@author: Rodrigo Marques Almeida da Silva

Base Theory from: Yury Gitman

- 2017-01-18: Add New Message Format

*/

#include <SPI.h>         // Arduino > 0018
#include <Ethernet.h>
#include <EthernetUdp.h>      
#include <DHT.h> 

// IP (we can change to DHCP)
IPAddress ip(192, 168, 11, 178);

// Dummy Port
unsigned int localPort = 8888;
// Remote
unsigned int RemotePort = 14654;

// Has network
boolean HasNet = false; 

// Remote IP
byte RemoteIP[] = {192, 168, 1, 100};

// External Devices
const int LM35_0 = A1;

#define DHTPIN A5
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

float T0;

int pulsePin = A0;                 // Pulse Pin (A0)
int blinkPin = 13;                // Blink to Debug

// MAC ADDRESS
byte mac[] = 
{
  0xDE, 0xAD, 0xBE, 0xEE, 0xFE, 0xED
};

// Dummy buffer
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];

// Reply 
char  ReplyBuffer[] = {0x01, 'B', '0', '0', '0', 0, 0x04};

// Thread Access variable
volatile int BPM;               
volatile int Signal;
volatile int IBI = 600; // 30 years old normal IBI
volatile boolean Pulse = false; // Pulse
volatile boolean QS = false; // QS Curve Done

// UDP Protocol
EthernetUDP Udp;

//Endereco I2C do MPU6050
const int MPU=0x68;  

#include <SD.h>
const int chipSelect = 10;

void setup()
{
  pinMode(blinkPin,OUTPUT);

  //GPS
  Serial.begin(38400);
  pinMode(10, OUTPUT);

  if (!SD.begin(chipSelect)) 
  {
      return;
  }

  InterruptSetup(); 
  
  Ethernet.begin(mac, ip) == 1; HasNet = true;
  
  //HasNet = (Ethernet.begin(mac) == 1); // For DHCP

  HasNet = HasNet && (Udp.begin(localPort) == 1);
}

void loop()
{ 
  String dataString = "";
  
  T0 = (float(analogRead(LM35_0))*5/(1023))/0.01;

  float h = dht.readHumidity(); 
  float t = dht.readTemperature();

  while(Serial.available())
  {
     temp = Serial.read();
     dataString += String(temp);

     if(stringOne.indexOf('\n') > 0)
      break;
   }
  
  if (QS == true)
  {        
    int val = BPM;
    ReplyBuffer[4] = char(val % 10 + '0');
    val = val / 10;
    
    ReplyBuffer[3] = char(val % 10 + '0');
    val = val / 10;        
    
    ReplyBuffer[2] = char(val % 10 + '0');
    
    QS = false;
    
    int ssum = (ReplyBuffer[1] + ReplyBuffer[2] + ReplyBuffer[3] + ReplyBuffer[4]) & 0x00FF;
    
    ReplyBuffer[5] = (char)ssum;
    
    if( HasNet)
    {
      Udp.beginPacket(RemoteIP, RemotePort);
      Udp.write(ReplyBuffer);
      Udp.endPacket();  
    }   
   }
   
  delay(200);
}




