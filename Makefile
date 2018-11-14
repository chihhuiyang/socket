all: servera.cpp serverb.cpp serverc.cpp client.cpp aws.cpp monitor.cpp
	g++ -o serverA servera.cpp  -lnsl -lresolv
	g++ -o serverB serverb.cpp  -lnsl -lresolv
	g++ -o serverC serverc.cpp  -lnsl -lresolv
	g++ -o client client.cpp  -lnsl -lresolv
	g++ -o aws aws.cpp  -lnsl -lresolv
	g++ -o monitor monitor.cpp  -lnsl -lresolv	
.PHONY: serverA serverB serverC aws monitor
serverA: 
	./serverA
serverB: 
	./serverB
serverC: 
	./serverC
aws:
	./aws
monitor: 
	./monitor
clean:
	-rm -f servera serverb serverc client aws monitor