#!/usr/bin/env python
import signal
import socket
import random
import sys
import os

os.system('clear')

#Handler CTRL+C
def signal_handler(signal, frame):
	print('\nSe ha pulsado Ctrl+C. Se cierra el socket.')
	serverSocket.close()

#Senal para CTRL+C
signal.signal(signal.SIGINT, signal_handler)

if len(sys.argv) != 2:
	print '\nEL USO CORRECTO ES: python %s <Puerto>' % (sys.argv[0])
	sys.exit()

#Crear socket UDP
try:
	serverSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
except socket.error, msg:
	print '\nNo se ha podido crear el socket.\nTipo de error: ' + msg[1]
	sys.exit()

#Bind del socket
try:
	serverSocket.bind(('', int(sys.argv[1])))
except socket.error, msg:
	print '\nNo se ha podido enlazar el socket.\nTipo de error: ' + msg[1]
	serverSocket.close()
	sys.exit()


print'------------>A la escucha de peticiones UDP por el puerto '+ str(int(sys.argv[1])) +'<------------'


while True:
	
	#Generar numero aleatorio (para un drop de mensaje)
	rand = random.randint(0, 10)	

	#Recibir ping
	try:
		mensaje, address = serverSocket.recvfrom(1024)
		print '>>>Se han recibido '+ str(len(mensaje)) +' bytes de la IP '+str(address[0])+':'+str(address[1])
	except socket.error, msg:
		print '\nNo se han podido recibir datos o se ha pulsado CTRL+C.\n'
		break

	if len(mensaje) == 0:
		break

	#Drop intencionado de un mensaje
	if rand < 3:
		continue

	#Enviar respuesta
	try:
		serverSocket.sendto(mensaje, address)
		print '<<<Se han enviado '+ str(len(mensaje)) +' bytes a la IP '+str(address[0])+':'+str(address[1])
	except socket.error, msg:
		print 'No se han podido enviar.\nTipo de error: ' + msg[1]
		break

