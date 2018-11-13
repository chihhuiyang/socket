all: servera.cpp client.cpp aws.cpp monitor.cpp
	g++ -o servera servera.cpp  -lnsl -lresolv
	g++ -o serverb serverb.cpp  -lnsl -lresolv
	g++ -o serverc serverc.cpp  -lnsl -lresolv
	g++ -o client client.cpp  -lnsl -lresolv
	g++ -o aws aws.cpp  -lnsl -lresolv
	g++ -o monitor monitor.cpp  -lnsl -lresolv

servera: 
	./servera

serverb: 
	./serverb

serverc: 
	./serverc

aws:
	./aws

monitor: 
	./monitor

clean:
	-rm -f servera serverb serverc client aws monitor			