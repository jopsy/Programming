all: client1 client2 dhtserver1 dhtserver2 dhtserver3

client1:client1.c
	gcc -o client1 client1.c -lsocket -lnsl -lresolv

client2:client2.c
	gcc -o client2 client2.c -lsocket -lnsl -lresolv

dhtserver1:dhtserver1.c
	gcc -o dhtserver1 dhtserver1.c -lsocket -lnsl -lresolv

dhtserver2:dhtserver2.c
	gcc -o dhtserver2 dhtserver2.c -lsocket -lnsl -lresolv

dhtserver3:dhtserver3.c
	gcc -o dhtserver3 dhtserver3.c -lsocket -lnsl -lresolv
