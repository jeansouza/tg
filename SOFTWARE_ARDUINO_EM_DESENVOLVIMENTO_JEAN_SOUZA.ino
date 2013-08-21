#include <SPI.h>
#include <Xively.h>
#include <AirQuality.h>
#include <Ethernet.h>
#include <HttpClient.h>

#define FEED_ID 140271570
#define API_KEY "bhDBsmj4fi2aj3AE821eoiuNo7wcvUAGmivZwxrg5nfF0Kks"

AirQuality sensor;
int cur_quality = -1;
byte mac[] = { 0x90, 0xA2, 0xDA, 0x0D, 0x26, 0xAD };

unsigned long lastCon = 0;
const unsigned long conInterval = 900000;
char sensorId[] = "sensor_reading";

XivelyDatastream datastreams[] = { XivelyDatastream(sensorId, strlen(sensorId), DATASTREAM_FLOAT), };
XivelyFeed feed(FEED_ID, datastreams, 1);

EthernetClient client;
XivelyClient xivelyclient(client);

void setup() {
	Serial.begin(9600);
	sensor.init(14);
	if (Ethernet.begin(mac) == 0) {   	
		Ethernet.begin(mac);
	}
}

void loop() { 
	cur_quality = sensor.slope();
	if (millis() - lastCon > conInterval) {
		if (cur_quality >= 0) {        		
			sendData(cur_quality);
                        enviarDados(cur_quality);
			lastCon = millis();
		} 
	}
}

ISR(TIMER2_OVF_vect) { 

	if(sensor.counter == 122) {
		sensor.last_vol = sensor.first_vol;
		sensor.first_vol = analogRead(A0);
		sensor.counter = 0;
		sensor.timer_index = 1;
		PORTB = PORTB^0x20;
	} else {
		sensor.counter++;
	}
}

void sendData(float sensorValue) {

	datastreams[0].setFloat(sensorValue);
	int ret = xivelyclient.put(feed, API_KEY);
}

void enviarDados(int info) {

	char server[] = "airquality-jfs.rhcloud.com";
	String data = "info=" + String(info);

	client.connect(server, 80);
	client.println("POST /data/webservice HTTP/1.1");
	client.println("Host: airquality-jfs.rhcloud.com");
	client.println("Content-Type: application/x-www-form-urlencoded");
	client.println("User-Agent: Arduino/1.0");
	client.println("Connection: close");
	client.print("Content-Length: ");
	client.println(data.length());
	client.println();
	client.println(data);
	client.println(); 
	client.stop();
}
