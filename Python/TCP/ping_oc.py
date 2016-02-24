import signal
import socket
import sys
import os
import time
import time

os.system('clear')

def signal_handler(signal, frame):
	print('\nSe ha pulsado Ctrl+C. Se cierra el socket.')
	s.close()

signal.signal(signal.SIGINT, signal_handler)


if len(sys.argv) != 5:
	print '\nEL USO CORRECTO ES: python %s <IP/Host> <Puerto> <Repeticiones> <Bytes>' % (sys.argv[0])
	sys.exit()

try:
	host_ip = socket.gethostbyname(sys.argv[1])
except socket.gaierror:
	print 'No se ha podido realizar la resolucion. Saliendo...'
	sys.exit()
try:
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
except socket.error, msg:
	print '\nNo se ha podido crear el socket.\nTipo de error: ' + msg[1]
	sys.exit()

try:
	s.connect((host_ip, int(sys.argv[2])))
except socket.error, msg:
	print '\nNo se ha podido crear el socket.\nTipo de error: ' + msg[1]
	sys.exit()

print '->Mandando una rafaga de '+sys.argv[3]+' pings de '+sys.argv[4]+' bytes a la direcion IP '+ host_ip +':'+ sys.argv[2]+'<-'
mensaje = 0
sumador_rtt = 0
contador_recibidos = 0
contador_enviados = 0
v_max = 0
v_min = 0

for x in xrange(int(sys.argv[3])):

	mensaje = str(mensaje).zfill(int(sys.argv[4]))
	try: 
		s.send(mensaje)
		time_u = time.clock()
		contador_enviados += 1
		#print '>>>Se ha enviado '+ str(len(mensaje)) +' bytes a la IP '+host_ip+':'+sys.argv[2]+'.'
	except socket.error, msg:
		print '\nNo se ha podido enviar los datos o se ha pulsado CTRL+C.\nTipo de error: ' + msg[1]
		break

	#Fijar timeout de 1 segundo
	s.settimeout(1)

	try:
		mensaje = s.recv((len(mensaje)))
		rtt = float((time.clock() - time_u))*1000000
		print '<<<Se ha recibido '+ str(len(mensaje)) +' bytes de la IP '+ host_ip +':'+ sys.argv[2]+'. Seq= '+str(x+1)+' RTT: '+ str(float(rtt)) +' us'
		sumador_rtt += rtt
		contador_recibidos += 1
		if contador_recibidos == 1:
			v_max = v_min = rtt
		if v_max < rtt:
			v_max = rtt
		if v_min > rtt:
			v_min = rtt
	except socket.error, msg:		
		print 'No se ha podido recibir datos'
		#break#Si queremos que cuando un dato se pierda, cerrar el socket. Descomente esta linea
	if len(mensaje) == 0:
		break
	time.sleep(1)
	#Estadistica

contador_perdidos = contador_enviados - contador_recibidos
print '\n\n-------------------------------------------------------------'
print 'Paquetes: Mandados '+str(contador_enviados)+'. Recibidos '+str(contador_recibidos)+'. Perdidos '+str(contador_perdidos)+'.'
if contador_recibidos > 0:
	print 'El RTT de las '+str(contador_recibidos)+' muestras es de '+ str(float(sumador_rtt)/float(contador_recibidos)) +' us.'
	print 'Tiempo de rafaga total es: '+ str(float(sumador_rtt)) +' us.'
	print 'RTT max: '+str(float(v_max))+' us. RTT min: '+str(float(v_min))+' us.'
print '------------------------------------- --------------------------'
s.close()