import signal
import socket
import sys
import os
import time

#Handler CTRL+C
def signal_handler(signal, frame):
	print('\nSe ha pulsado Ctrl+C. Se cierra el socket.')
	clientSocket.close()
	
#Senal para CTRL+C
signal.signal(signal.SIGINT, signal_handler)

os.system('clear')
if len(sys.argv) != 5:
	print '\nEL USO CORRECTO ES: python %s <IP/Host> <Puerto> <Repeticiones> <Bytes>' % (sys.argv[0])
	sys.exit()

#Resolver DNS
try:
	host_ip = socket.gethostbyname(sys.argv[1])
except socket.gaierror:
	print 'No se ha podido realizar la resolucion. Saliendo...'
	sys.exit()

#Crear socket UDP
try:
	clientSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
except socket.error, msg:
	print '\nNo se ha podido crear el socket.\n'
	sys.exit()


print '->Mandando una rafaga de '+sys.argv[3]+' pings de '+sys.argv[4]+' bytes a la direcion IP '+ host_ip +':'+ sys.argv[2]+'<-'

mensaje = 0
contador_recibidos = 0
sumador_rtt = 0
v_max = 0
v_min = 0
dstAddress = (host_ip,int(sys.argv[2]))

for x in xrange(int(sys.argv[3])):

	mensaje = str(mensaje).zfill(int(sys.argv[4]))

	#Enviar ping
	try:
		start = time.time()
		clientSocket.sendto(mensaje, dstAddress)
		#print '<<<Se han enviado '+ str(len(mensaje)) +' bytes a la IP '+host_ip+':'+sys.argv[2]
	except socket.error, msg:
		print 'No se ha podido enviar los datos.'
		break

	#Fijar timeout de 1 segundo
	clientSocket.settimeout(1)

	#Recibir respuesta
	try:
		mensaje = clientSocket.recv((len(mensaje)))
		end = time.time()
		elapsed = end - start
		contador_recibidos += 1
		sumador_rtt += elapsed
		if contador_recibidos == 1:
			v_max = v_min = elapsed
		if v_max < elapsed:
			v_max = elapsed
		if v_min > elapsed:
			v_min = elapsed
		print '->Se han recibido '+ str(len(mensaje)) +' bytes de la IP '+ host_ip +':'+ sys.argv[2] + '. Seq:'+str(x+1)+' RTTS:' + str(float((elapsed)*1000000)) + ' us'
	except socket.error, msg:		
		print 'No se ha podido recibir datos'
		continue
	time.sleep(1)

#Estadistica
contador_perdidos = int(sys.argv[3]) - contador_recibidos
print '\n\n-------------------------------------------------------------'
print 'Paquetes: Mandados '+sys.argv[3]+'. Recibidos '+str(contador_recibidos)+'. Perdidos '+str(contador_perdidos)+'.'
if contador_recibidos > 0:
	pass
	print 'El RTT de las '+str(contador_recibidos)+' muestras es de '+ str((float(sumador_rtt)/float(contador_recibidos))*1000000) +' us.'
	print 'Tiempo de rafaga total es: '+ str(sumador_rtt*1000000) +' us.'
	print 'RTT max: '+str(float(v_max)*1000000)+' us. RTT min: '+str(float(v_min*1000000))+' us.'
print '------------------------------------- --------------------------'
clientSocket.close()
