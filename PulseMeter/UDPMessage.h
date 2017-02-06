/*
 	UDPMessage.h - A UDP Wapper for Comunication	

	Created by Rodrigo Marques Almeida da Silva, January, 2016.
	Released into the public domain.
*/
#ifndef UDPMessage_h
#define UDPMessage_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include <Arduino.h>
#else
	#include <WProgram.h>
#endif

#include <inttypes.h>

class UDPMessage
{
	protected:
		
		char ASCIIReplyBuffer[7];
		char GPSReplyBuffer[12];
		
		void FillE2ASCIINumber(int val);
	
	public:
		UDPMessage();
		
		char *FillBPMMessage(int val);
		char *FillTemperature1Message(float val);
		char *FillTemperature0Message(float val);
		char *FillHumidity0Message(float val);
		char *FillGPSSPeedMessage(float val);
		
		char *FillGPSMessage(float lat, float lon);
};

#endif