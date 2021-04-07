#ifndef _WEMO_H
#define _WEMO_H
#include <Ethernet.h>

class Wemo {
	
	EthernetClient WemoClient;		
	int wemoPort = 49153;

	// Note: wemoIP[4] is not for the classroom, but for a remote student
	const char *wemoIP[5] = {"192.168.1.4","192.168.1.6","192.168.1.8","192.168.1.7","192.168.1.13"};
	
	public:
	
	void switchON(int wemo) {
	  
	  String data1;
	  
	  Serial.printf("SwitchON %i \n",wemo);
	  data1+="<?xml version=\"1.0\" encoding=\"utf-8\"?><s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body><u:SetBinaryState xmlns:u=\"urn:Belkin:service:basicevent:1\"><BinaryState>1</BinaryState></u:SetBinaryState></s:Body></s:Envelope>"; // Use HTML encoding for comma's
	  if (WemoClient.connect(wemoIP[wemo],wemoPort)) {
			Serial.println("Connected.");
			WemoClient.println("POST /upnp/control/basicevent1 HTTP/1.1");
			Serial.println("24");
			WemoClient.println("Content-Type: text/xml; charset=utf-8");
			Serial.println("26");
			WemoClient.println("SOAPACTION: \"urn:Belkin:service:basicevent:1#SetBinaryState\"");
			Serial.println("28");
			WemoClient.println("Connection: keep-alive");
			Serial.println("30");
			WemoClient.print("Content-Length: ");
			Serial.println("32");
			WemoClient.println(data1.length());
			WemoClient.println();
			WemoClient.print(data1);
			WemoClient.println();
		}		

	  if (WemoClient.connected()) {
		 Serial.println("Connected 2.");
		 WemoClient.stop();
	  }
	}

	void switchOFF(int wemo){
	  String data1;
	  
	  Serial.printf("switchOFF %i \n",wemo);
	  data1+="<?xml version=\"1.0\" encoding=\"utf-8\"?><s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body><u:SetBinaryState xmlns:u=\"urn:Belkin:service:basicevent:1\"><BinaryState>0</BinaryState></u:SetBinaryState></s:Body></s:Envelope>"; // Use HTML encoding for comma's
	  if (WemoClient.connect(wemoIP[wemo],wemoPort)) {
			WemoClient.println("POST /upnp/control/basicevent1 HTTP/1.1");
			WemoClient.println("Content-Type: text/xml; charset=utf-8");
			WemoClient.println("SOAPACTION: \"urn:Belkin:service:basicevent:1#SetBinaryState\"");
			WemoClient.println("Connection: keep-alive");
			WemoClient.print("Content-Length: ");
			WemoClient.println(data1.length());
			WemoClient.println();
			WemoClient.print(data1);
			WemoClient.println();
		}
	   
	  if (WemoClient.connected()) {
		 WemoClient.stop();
	  }
	}
};
#endif