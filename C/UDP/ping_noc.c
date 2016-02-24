#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h> //close()
#include <string.h> //strlen
#include <stdlib.h> //system("clear")
#include <unistd.h> //sleep()
#include <sys/time.h>//struct timeval
#include <signal.h> //  Señal para capturar CTRL+C
#include <netdb.h> // gethostbyname()
#define SIZE_SEND 32
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
	if(argc!=4)
	{
		printf("ERROR:USO INCORRECTO DEL PROGRAMA ES:\n\n--->>ping_noc.c IP PUERTO N_REPETICIONES<<---\n\n");
		exit(1);
	}
	
	/*-----Definicion de variables----*/
	int i = 0;
	char message[SIZE_SEND];
	memset((char*)&message, 0, sizeof(message));
	struct hostent * dstHost;
	int dstPort;
	//Datos recibidos del servidor
	char datos[SIZE_SEND];
	int size_data;
	//Datos para el timer
	struct timeval t_ini, t_fin;
	double v_max,v_min;
  	double secs;
  	double media=0;
	int contador_real=0;
	//Inicializacion de variables
	int flag = atoi(argv[3]);
	dstHost = gethostbyname((argv[1]));
	dstPort = atoi(argv[2]);



	//Interrupcion para CTRL+C
	signal(SIGINT, control);

	//Creacion del Socket
	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)		
	{
		perror("No se ha podido crear el socket");
		return 0;
	}

	/*-----Cliente-----*/
	struct sockaddr_in client;				
	memset((char *)&client, 0, sizeof(client));
	client.sin_family = AF_INET;
	client.sin_addr.s_addr = htonl(INADDR_ANY);
	client.sin_port = htons(0);
	
	//Bind del socket	
	if (bind(fd, (struct sockaddr *)&client, sizeof(client)) < 0)
	{
		perror("No se ha podido enlazar el socket");
		close(fd);
		return 0;
	}

	/*-----Servidor-----*/
	struct sockaddr_in server;

	//Rellenamos los datos que identificaran al servidor	
	memset((char*)&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(dstPort);
	server.sin_addr.s_addr = inet_addr(inet_ntoa( *(struct in_addr *)(dstHost->h_addr_list[0])));

	printf("-------->Mandando una rafaga de %d pings a la direción IP %s:%d<--------\n\n",flag,dstHost->h_name,dstPort);
	

	/*------------Main loop--------------*/

	do{	
		i++;
		secs=0;
			
		//Envio del ping
		if (sendto(fd, message, sizeof(message), 0, (struct sockaddr *)&server, sizeof(server)) < 0)
		{
			gettimeofday(&t_ini, NULL);
			perror("Error al enviar los datos al servidor");
			close(fd);
			return 0;
		}
		else
		{
			gettimeofday(&t_ini, NULL);
			//printf(">>> Se han enviado %d bytes a la IP %s:%d.\n",(int)sizeof(message),dstHost->h_name,ntohs(server.sin_port));
		}

		//Recepcion de respuesta
		int size_client=sizeof(server);
		if((size_data=(recvfrom(fd,datos,sizeof(datos),0,(struct sockaddr *)&server,&size_client)))==-1)
		{
			perror("Se han recibido mal los datos");
			close(fd);
			return 0;
		}
		else{
			//Calculo de estadisticas
			contador_real++;
			gettimeofday(&t_fin, NULL);
			secs =((double)(t_fin.tv_sec + (double)t_fin.tv_usec/1000000)-(double)(t_ini.tv_sec + (double)t_ini.tv_usec/1000000))*1000000;
			media=media+secs;

			printf("Recibidos %d bytes de la IP %s:%d seq=%d RTT:  %f us\n",size_data,inet_ntoa(server.sin_addr),ntohs(server.sin_port),i,secs);
	
			if(contador_real==1)
				v_max=v_min=secs;
			if(v_max<secs)
				v_max=secs;
			if(v_min>secs)
				v_min=secs;
		}

		sleep(1);



	}while(i<flag);

	//Estadistica
	printf("\n-------------------------------------------------------------\n");
	printf("El RTT medio de las %d muestras es de  %f us\n",contador_real,media/contador_real);
	printf("Tiempo de rafaga: %f us\n",media);
	printf("RTT max: %f us RTT min: %f us\n",v_max,v_min);
	printf("------------------------------------- --------------------------\n");

	close(fd);
	return 0;
}
