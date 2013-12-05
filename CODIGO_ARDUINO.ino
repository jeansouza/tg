/*
 *
 * Descricao: Software responsavel por controlar o Arduino utilizado no dispositivo para medicao
 * da qualidade do ar. Esse dispositivo foi desenvolvido como parte do Trabalho de Graduacao para 
 * obtencao do titulo de Tecnologo em Informatica com Enfase em Banco de Dados.
 *
 * Data: 04/12/2013
 * Autor: Jean Souza
 * E-mail para contato: jeancfsouza@gmail.com
 * 
 */

/* <Bibliotecas e constantes> */

#include <SPI.h>
#include <Xively.h>
#include <Ethernet.h>
#include <HttpClient.h>

#define FEED_ID 140271570

/* </Bibliotecas e constantes> */

/* <Variaveis> */

byte mac[] = { 0x90, 0xA2, 0xDA, 0x0D, 0x26, 0xAD };
char server[] = "airquality-jfs.rhcloud.com";
char API_KEY[] = "bhDBsmj4fi2aj3AE821eoiuNo7wcvUAGmivZwxrg5nfF0Kks";

float sensorOne = 0;
float sensorTwo = 0;
float sensorThree = 0;
float sensorFour = 0;

unsigned long lastConnectionTime = 0;
const unsigned long connectionInterval = 900000;

char sensorOneId[] = "sensorOne";
char sensorTwoId[] = "sensorTwo";
char sensorThreeId[] = "sensorThree";
char sensorFourId[] = "sensorFour";

XivelyDatastream datastreams[] = {
	XivelyDatastream(sensorThreeId, strlen(sensorThreeId), DATASTREAM_FLOAT),
	XivelyDatastream(sensorTwoId, strlen(sensorTwoId), DATASTREAM_FLOAT),
	XivelyDatastream(sensorFourId, strlen(sensorFourId), DATASTREAM_FLOAT),
	XivelyDatastream(sensorOneId, strlen(sensorOneId), DATASTREAM_FLOAT)
};

XivelyFeed feed(FEED_ID, datastreams, 4);

EthernetClient client;
EthernetClient c;
XivelyClient xivelyclient(client);

/* </Variaveis> */

/* <Metodo de inicializacao> */

void setup() {  
	if (Ethernet.begin(mac) == 0) {   	
		Ethernet.begin(mac);
	}
}

/* </Metodo de inicializacao> */

/* <Metodo que fica em loop executando as medicoes> */

void loop() {

	if (millis() - lastConnectionTime >= connectionInterval) {    
  
                float sensorOne = analogRead(0); 
                float sensorTwo = analogRead(1); 
                float sensorThree = analogRead(2); 
                float sensorFour = analogRead(3); 
                
		sendData(sensorOne, sensorTwo, sensorThree, sensorFour);
		sendDados(sensorOne, sensorTwo, sensorThree, sensorFour);

		lastConnectionTime = millis();
	}
}

/* </Metodo que fica em loop executando as medicoes> */

/* <Metodo que envia os dados para o Xively> */

void sendData(float sensorOne, float sensorTwo, float sensorThree, float sensorFour) {
  
	datastreams[0].setFloat(sensorThree);
	datastreams[1].setFloat(sensorTwo);
	datastreams[2].setFloat(sensorFour);
	datastreams[3].setFloat(sensorOne);

	int ret = xivelyclient.put(feed, API_KEY);
}

/* </Metodo que envia os dados para o Xively> */

/* <Metodo que envia os dados para Web Service RESTful para que eles sejam salvos no banco MongoDB> */

void sendDados(float sensorOne, float sensorTwo, float sensorThree, float sensorFour) {    
  

	if (c.connect(server, 80)) {
  
		char sensorOneAux[6];
		char sensorTwoAux[6];
		char sensorThreeAux[6];
		char sensorFourAux[6];

		dtostrf(sensorOne, 1, 2, sensorOneAux);
		dtostrf(sensorTwo, 1, 2, sensorTwoAux);
		dtostrf(sensorThree, 1, 2, sensorThreeAux);
		dtostrf(sensorFour, 1, 2, sensorFourAux);

		String data = "info=";
		data += sensorOneAux;
                data += ":";
		data += sensorTwoAux;
		data += ":";
		data += sensorThreeAux;
		data += ":";
		data += sensorFourAux;

		c.println("POST /data/webservice HTTP/1.1");
		c.println("Host: airquality-jfs.rhcloud.com");
		c.println("Content-Type: application/x-www-form-urlencoded");
		c.println("User-Agent: Arduino/1.0");
		c.println("Connection: close");
		c.print("Content-Length: ");
		c.println(data.length());
		c.println();
		c.println(data);
		c.println();
		c.stop();   
	}   
}

/* </Metodo que envia os dados para Web Service RESTful para que eles sejam salvos no banco MongoDB> */
