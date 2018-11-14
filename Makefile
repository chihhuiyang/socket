all: serverA.cpp serverB.cpp serverC.cpp client.cpp aws.cpp monitor.cpp
	g++ -o serverAoutput serverA.cpp  -lnsl -lresolv
	g++ -o serverBoutput serverB.cpp  -lnsl -lresolv
	g++ -o serverCoutput serverC.cpp  -lnsl -lresolv
	g++ -o client client.cpp  -lnsl -lresolv
	g++ -o awsoutput aws.cpp  -lnsl -lresolv
	g++ -o monitoroutput monitor.cpp  -lnsl -lresolv
.PHONY: serverA serverB serverC aws monitor
serverA: 
	./serverAoutput
serverB: 
	./serverBoutput
serverC: 
	./serverCoutput
aws:
	./awsoutput
monitor: 
	./monitoroutput
clean:
	-rm -f serverAoutput serverBoutput serverCoutput client awsoutput monitoroutput