/**
Created on Dez 02, 2016

@author: Rodrigo Marques Almeida da Silva

- 2017-01-18: Add New Message Format
- 2017-01-28: Add Internal UDP Class

*/

#include <SPI.h>         // Arduino > 0018
#include <Ethernet.h>
#include <EthernetUdp.h>      
//#include <DHT.h> 

//#define UDP_PROTOCOL

#include "UDPMessage.h"

#ifdef UDP_PROTOCOL
// Dummy Port
unsigned int localPort = 8888;
// Remote
unsigned int RemotePort = 14654;

// Has network
boolean HasNet = false; 

// Remote IP
byte RemoteIP[] = {192, 168, 11, 100};

// MAC ADDRESS
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEE, 0xFE, 0xED };

// IP (we can change to DHCP)
IPAddress ip(192, 168, 11, 178);

// Dummy buffer
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];

// UDP Protocol
EthernetUDP Udp;

UDPMessage messageBuilder;

#endif

// External Devices
const int LM35_0 = A1; // INT
const int LM35_1 = A5; // INT

//#define DHTPIN A5
//#define DHTTYPE DHT11

//DHT dht(DHTPIN, DHTTYPE);

int pulsePin = A0;                 // Pulse Pin (A0)
int blinkPin = 13;                // Blink to Debug

// Thread Access variable
volatile int BPM;               
volatile int Signal;
volatile int IBI = 600; // 30 years old normal IBI
volatile boolean Pulse = false; // Pulse
volatile boolean QS = false; // QS Curve Done

//Endereco I2C do MPU6050
const int MPU=0x68;  

//GPS
String GPSInput = ""; 
boolean GPSCompleted = false;

#define SENSOR_INTERVAL 300

int LastSensorRead = 0;

void setup()
{
  pinMode(blinkPin,OUTPUT);

  GPSInput.reserve(200);

  //GPS
  Serial.begin(9600);  

  InterruptSetup(); 

  delay(2000);

#ifdef UDP_PROTOCOL
  
  Ethernet.begin(mac, ip); HasNet = true;
  
  //HasNet = (Ethernet.begin(mac) == 1); // For DHCP

  HasNet = HasNet && (Udp.begin(localPort) == 1);
  
#endif
  
}

void sensorUpdate()
{
  if( millis() - LastSensorRead >= SENSOR_INTERVAL)
  {
      float T0 = (float(analogRead(LM35_0))*5/(1023))/0.01;
      
      float T1 = (float(analogRead(LM35_1))*5/(1023))/0.01;
      
//      float h = dht.readHumidity();       
  //    float T1 = dht.readTemperature();

    //  sendMessage(messageBuilder.FillHumidity0Message(h));

#ifdef UDP_PROTOCOL
      sendMessage(messageBuilder.FillTemperature0Message(T1));
#endif      
      Serial.print("T1,"); Serial.println(T1);      

#ifdef UDP_PROTOCOL
      sendMessage(messageBuilder.FillTemperature1Message(T0));
#endif
      
      Serial.print("T0,"); Serial.println(T0);

      LastSensorRead = millis();
  }
}

void sendMessage(char* msg)
{
#ifdef UDP_PROTOCOL  
    if(HasNet)
    {
      Udp.beginPacket(RemoteIP, RemotePort);
      Udp.write(msg);
      Udp.endPacket();  
    }   
#endif    
}

float lat = 0;
float lon = 0;
float gspeed = 0;

#define GPS_POS_FIX "$GPGGA"
#define GPS_SPEED_FIX "$GPRMC"
//$GPGGA
int processGPSString(String msg)
{
  if(msg.startsWith(GPS_POS_FIX))
  {
    int lidx = 0;
    int idx = msg.indexOf(",");
    
    lidx = idx;    
    idx = msg.indexOf(",", idx + 1);
    
    lidx = idx; // LAT START POS
    idx = msg.indexOf(",", idx + 1);  // LAT END POS

    String slat = msg.substring(lidx + 1, idx);
    lat = slat.toFloat();

    lidx = idx; // LAT DIR START POS
    idx = msg.indexOf(",", idx + 1);  // LAT DIR END POS    

    if(msg[lidx + 1] == 'S')
    {      
      lat = -lat;
    } 

    lidx = idx; // LONG START POS
    idx = msg.indexOf(",", idx + 1);  // LONG END POS    

    String slon = msg.substring(lidx + 1, idx);
    lon = slon.toFloat();

    lidx = idx; // LONG DIR START POS
    idx = msg.indexOf(",", idx + 1);  // LONG DIR END POS        

    if(msg[lidx + 1] == 'W')
    {      
      lon = -lon;
    } 

    return 1;
  } 
  else if(msg.startsWith(GPS_SPEED_FIX))
  {
    int lidx = 0;
    int idx = msg.indexOf(",");
    
    lidx = idx;    
    idx = msg.indexOf(",", idx + 1);

    lidx = idx;    
    idx = msg.indexOf(",", idx + 1);    
    
    lidx = idx; // LAT START POS
    idx = msg.indexOf(",", idx + 1);  // LAT END POS

    lidx = idx; // LAT DIR START POS
    idx = msg.indexOf(",", idx + 1);  // LAT DIR END POS    

    lidx = idx; // LONG START POS
    idx = msg.indexOf(",", idx + 1);  // LONG END POS    

    lidx = idx; // LONG DIR START POS
    idx = msg.indexOf(",", idx + 1);  // LONG DIR END POS        

    lidx = idx; // SPEED START
    idx = msg.indexOf(",", idx + 1); // SPEED END

    String sgspeed = msg.substring(lidx + 1, idx);
    gspeed = 1.852 * sgspeed.toFloat();  // 1 Knot =1.852 Kilometers per Hour

    return 2;
  } 
  else
  {
    return 0;  
  }
}

void loop()
{ 
  if (QS == true)
  {         

#ifdef UDP_PROTOCOL    
    sendMessage(messageBuilder.FillBPMMessage(BPM));
#endif

    Serial.print("B,"); Serial.println(BPM);
    
    QS = false;           
    
    /*int val = BPM;
    ASCIIReplyBuffer[4] = char(val % 10 + '0');
    val = val / 10;
    
    ASCIIReplyBuffer[3] = char(val % 10 + '0');
    val = val / 10;        
    
    ASCIIReplyBuffer[2] = char(val % 10 + '0');
    
    QS = false;
    
    int ssum = (ASCIIReplyBuffer[1] + ASCIIReplyBuffer[2] + ASCIIReplyBuffer[3] + ASCIIReplyBuffer[4]) & 0x00FF;
    
    ReplyBuffer[5] = (char)ssum;*/    
  }  

  sensorUpdate();

  if (GPSCompleted)
  {

#ifdef UDP_PROTOCOL
    int ret = processGPSString(GPSInput);

    int newline = GPSInput.indexOf("\n");
    
    if(newline + 1 >= GPSInput.length())
    {
      GPSInput = "";  
    } else
    {
      GPSInput = GPSInput.substring(newline + 1);
    }    
#endif
    
    GPSCompleted = false;

#ifdef UDP_PROTOCOL
    if(ret == 1)
    {
      sendMessage(messageBuilder.FillGPSMessage(lat, lon)); 

      Serial.print("LAT,"); Serial.println(lat);
      Serial.print("LON,"); Serial.println(lon);
      
    } else if(ret == 2)
    {
      sendMessage(messageBuilder.FillGPSSPeedMessage(gspeed)); 
      Serial.print("S,"); Serial.println(gspeed);
    }
#endif
    
  }
  
}

void serialEvent()
{
  while (Serial.available()) 
  {
    char inChar = (char)Serial.read();
    
    GPSInput += inChar;

    Serial.write(inChar);
    
    if (inChar == '\n') 
    {
      GPSCompleted = true;
      break;
    }
  }
}




