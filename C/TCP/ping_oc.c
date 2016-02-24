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
int flag_CTRL_C=0;
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
	char message[SIZE_SEND];
	memset((char*)&message, 0, sizeof(message));
	char datos[SIZE_SEND];
	int size_data;
	int flag;
	int i = 0;
	struct hostent * dstHost;
	int dstPort;
	//Datos para el timer
	struct timeval t_ini, t_fin;
	double v_max,v_min;
  	double secs;
  	double media=0;
	int contador_real=0;
	//Inicializacion de variables
	dstHost = gethostbyname((argv[1]));
	dstPort = atoi(argv[2]);
	flag = atoi(argv[3]);
	size_data=sizeof(message);

	//Interrupcion para CTRL+C
	signal(SIGINT, control);

	//Creamos el socket
	if ((fd = socket(PF_INET, SOCK_STREAM, 0)) < 0)		
	{
		perror("No se ha podido crear el socket");
		close(fd);
		return 0;
	}


	/*------Servidor--------*/
	struct sockaddr_in server;

	//Rellenamos los datos que identificaran al servidor	
	memset((char*)&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(dstPort);
	server.sin_addr.s_addr = inet_addr(inet_ntoa( *(struct in_addr *)(dstHost->h_addr_list[0])));

	//Conexion al servidor
	if (connect(fd, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		perror("No se ha podido crear el socket");
		close(fd);
		return 0;
	}
	
	printf("-------->Mandando una rafaga de %d pings a la direción IP %s:%d<--------\n\n",flag,dstHost->h_name,dstPort);
	

	/*------------Main loop--------------*/
	do
	{
		i++;
		//Envio del ping
		if (send(fd, message,size_data,0)< 0)
		{
			perror("No se ha podido responder al cliente");
			close(fd);
			return 0;
		}
		else
		{
			gettimeofday(&t_ini, NULL);
			//printf(">>> Se han enviado %d bytes a la IP %s a su puerto %d.\n",size_data,inet_ntoa(server.sin_addr),ntohs(server.sin_port));
		}


		/*--------Recepcion de respuesta---------*/

		
		
		//Recepcion
		int size_message;
		if(((size_message = recv(fd,datos,sizeof(datos),0))==-1))
		{
			perror("No se ha podido recibir los datos del cliente");
			close(fd);
			return 0;
		}
		else if (size_message==0)
		{

			printf("\nCancelada la conexion TCP por el servidor. Cerramos el socket\n\n");
			i=flag;
			close(fd);
			alarm(0);
		}
		else
		{	
			//Calculo de estadistica
			contador_real++;
			gettimeofday(&t_fin, NULL);
			secs =((double)(t_fin.tv_sec + (double)t_fin.tv_usec/1000000)-(double)(t_ini.tv_sec + (double)t_ini.tv_usec/1000000))*1000000;
			media=media+secs;

			printf("%d bytes de la IP %s:%d seq=%d RTT:  %f us\n",size_data,inet_ntoa(server.sin_addr),ntohs(server.sin_port),i,secs);

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
	printf("El RTT de las %d muestras es de  %f us\n",contador_real,media/contador_real);
	printf("Tiempo de rafaga: %f us\n",media);
	printf("RTT max: %f us RTT min: %f us\n",v_max,v_min);
	printf("------------------------------------- --------------------------\n");

	close(fd);
	return 0;
}
