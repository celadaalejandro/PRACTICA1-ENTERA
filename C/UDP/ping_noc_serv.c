#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h> //close()
#include <string.h> //strlen
#include <stdlib.h> //system("clear")
#include <signal.h> //  Señal para capturar CTRL+C
#define SIZE_MAX_RECV 1000
int flag_CTRL_C = 0;
int fd;

void control(int sig)
{
	printf("\nCancelada la conexion UDP. Cerramos el socket\n\n");
	close(fd);
	exit(0);
}

int main (int argc, char *argv[])
{
	system("clear");

	if(argc!=2)
	{
		printf("ERROR:USO INCORRECTO DEL PROGRAMA ES: PUERTO\n");
		exit(1);
	}

	/*-----Definicion de variables----*/
	int srcPort;

	srcPort = atoi(argv[1]);

	//Interrupcion para CTRL+C
	signal(SIGINT, control);

	//Creacion del socket
	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)		
	{
		perror("No se ha podido crear el socket");
		return 0;
	}

				
	/*-------Servidor-------*/
	struct sockaddr_in server;
	memset((char *)&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(srcPort);
	
	//Bind del Socket	
	if (bind(fd, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		perror("No se ha podido enlazar el socket");
		close(fd);
		return 0;
	}

	/*-------Cliente-------*/
	struct sockaddr_in client;
	int size_message;
	char datos[SIZE_MAX_RECV];
	unsigned int size_client = sizeof(client);

	printf("-------->Esperando datos por el puerto %d<---------\n",srcPort);

	
	/*------------Main loop--------------*/

	while(1)
	{
		//Recepcion del ping
		if((size_message = recvfrom(fd,datos,sizeof(datos),0,(struct sockaddr *)&client, &size_client))==-1)
		{
			perror("No se ha podido enlazar el socket");
			close(fd);
			return 0;
		}
		else if (size_message==0)
		{
			printf("Se ha recibido 0 bytes. Cerramos el servidor.");
			close(fd);
			return 0;
		}
		else
		{
			printf("+++ Se han recibido %d bytes de la IP %s:%d\n",size_message,inet_ntoa(client.sin_addr),ntohs(client.sin_port));
		}
	
		//Envio de respuesta
		if (sendto(fd, datos, size_message, 0, (struct sockaddr *)&client, sizeof(client)) < 0)
		{
			perror("No se ha podido responder al cliente");
			close(fd);
			return 0;
		}
		else
		{
			printf(">>> Se han enviado %d bytes a la IP %s:%d\n",size_message,inet_ntoa(client.sin_addr),ntohs(client.sin_port));
		}
		
	}
}
