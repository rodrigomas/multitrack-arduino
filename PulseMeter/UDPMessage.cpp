#include "UDPMessage.h"

UDPMessage::UDPMessage()
{	
	ASCIIReplyBuffer[0] = 0x01;
	
	ASCIIReplyBuffer[1] = 'B';
	ASCIIReplyBuffer[2] = '0';
	ASCIIReplyBuffer[3] = '0';
	ASCIIReplyBuffer[4] = '0';
	
	ASCIIReplyBuffer[5] = 0;

	ASCIIReplyBuffer[6] = 0x04;
	
	//GPS	
	GPSReplyBuffer[0] = 0x01;
	GPSReplyBuffer[1] = 'G';
	
	//LAT
	GPSReplyBuffer[2] = 0;
	GPSReplyBuffer[3] = 0;
	GPSReplyBuffer[4] = 0;
	GPSReplyBuffer[5] = 0;
	
	//LONG
	GPSReplyBuffer[6] = 0;
	GPSReplyBuffer[7] = 0;
	GPSReplyBuffer[8] = 0;
	GPSReplyBuffer[9] = 0;	
	
	//LONG
	GPSReplyBuffer[10] = 0;
	GPSReplyBuffer[11] = 0x04;	
}

void UDPMessage::FillE2ASCIINumber(int val)
{
	ASCIIReplyBuffer[4] = char(val % 10 + '0');
    val = val / 10;
    
    ASCIIReplyBuffer[3] = char(val % 10 + '0');
    val = val / 10;        
    
    ASCIIReplyBuffer[2] = char(val % 10 + '0');
    
    int ssum = (ASCIIReplyBuffer[1] + ASCIIReplyBuffer[2] + ASCIIReplyBuffer[3] + ASCIIReplyBuffer[4]) & 0x00FF;
    
    ASCIIReplyBuffer[5] = (char)ssum;	
}

char *UDPMessage::FillBPMMessage(int val)
{
	ASCIIReplyBuffer[1] = 'B';
	FillE2ASCIINumber(val);
}

char *UDPMessage::FillTemperature1Message(float val)
{
	ASCIIReplyBuffer[1] = 'F';
	FillE2ASCIINumber((int)(val * 10));	
}

char *UDPMessage::FillTemperature0Message(float val)
{
	ASCIIReplyBuffer[1] = 'E';
	FillE2ASCIINumber((int)(val * 10));	
}

char *UDPMessage::FillHumidity0Message(float val)
{
	ASCIIReplyBuffer[1] = 'H';
	FillE2ASCIINumber((int)(val * 10));	
}

char *UDPMessage::FillGPSSPeedMessage(float val)
{
	ASCIIReplyBuffer[1] = 'X';
	FillE2ASCIINumber((int)(val * 10));	
}

char *UDPMessage::FillGPSMessage(float lat, float lon)
{	
	//LAT
	unsigned int val = (unsigned int)lat;
	GPSReplyBuffer[5] = (char)(val & 0xFF); val = val >> 8;
	GPSReplyBuffer[4] = (char)(val & 0xFF); val = val >> 8;
	GPSReplyBuffer[3] = (char)(val & 0xFF); val = val >> 8;
	GPSReplyBuffer[2] = (char)(val & 0xFF);
	
	//LONG
	val = (unsigned int)lon;
	GPSReplyBuffer[6] = (char)(val & 0xFF); val = val >> 8;
	GPSReplyBuffer[7] = (char)(val & 0xFF); val = val >> 8;
	GPSReplyBuffer[8] = (char)(val & 0xFF); val = val >> 8;
	GPSReplyBuffer[9] = (char)(val & 0xFF);	
		
	int ssum = 0;
	
	for(int i = 1; i < 10; i++)
	{
		ssum += GPSReplyBuffer[i];
	}
	
	ssum = ssum & 0x00FF;
	GPSReplyBuffer[10] = (char)ssum;
}

