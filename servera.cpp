#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <iostream>
#include <netdb.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <map>
#include <fstream>
#include <vector>
#include <sstream>
#include <iterator>
#include <string>

#define SERVER_A_PORT 21068
#define SERVER_B_PORT 22068
#define SERVER_C_PORT 23068
#define AWS_PORT 24068
#define AWS_CLIENT_TCP_PORT 25068
#define AWS_MONITOR_TCP_PORT 26068
#define BUFFER_SIZE 50
#define BACKLOG 10

using namespace std;

const char* localhost = "127.0.0.1";

vector<double> split(const string &s, char delim) {
    vector<double> res;
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) {
        double val = atof(item.c_str());
        res.push_back(val);
    }
    return res;
}

void extract_buffer(char *buffer) {
    int i = 0;
    while (buffer[i] != '*') {
        i++;
        if (i == BUFFER_SIZE) break;
    }
    for (int j = i; j < BUFFER_SIZE; j++) {
        *(buffer + i) = '\0';
    }
}

void fill_buffer(char *buffer) {
    int i = strlen(buffer);
    for (int j = i; j < BUFFER_SIZE; j++) {
        *(buffer + j) = '*';
    }
}

int main() {
    // // load database
    ifstream in("database_a.csv");
    string line;
    map<int, vector<double> > mapA;
    map<int, vector<double> >::iterator it, i;
    // open file to build map
    if (in.is_open()) {
        // parse each line       
        while (getline(in, line)) {
            vector<double> v = split(line, ',');
            int key = (int)v[0];
            v.erase(v.begin());
            mapA.insert(pair<int, vector<double> >(key, v));
        }     
    }
    in.close();      


    struct sockaddr_in server_A_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int socket_A;

    // buffer
    char send_buffer[BUFFER_SIZE];
    char recv_buffer[BUFFER_SIZE];
    int recv_len;
    
    // create the UDP socket
    if ((socket_A = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        perror("Failed to create socket A.");
        exit(1);
    }

    memset(&server_A_addr, 0, sizeof(server_A_addr));
    memset(&client_addr, 0, sizeof(client_addr));

    // server info
    server_A_addr.sin_family = PF_INET;
    server_A_addr.sin_addr.s_addr = inet_addr(localhost); 
    server_A_addr.sin_port = SERVER_A_PORT; 
   
    // bind the socket
    if (bind(socket_A, (struct sockaddr *)&server_A_addr, sizeof(server_A_addr)) < 0) {
        perror("Failed to bind.");
        exit(1);
    }
    cout << "The Server A is up and running using UDP on port <" << SERVER_A_PORT << ">" << endl;

    // loop, wait for receiving data
    while (true) {

        recv_len = recvfrom(socket_A, &recv_buffer, (size_t)BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &client_addr_len);
        recv_buffer[BUFFER_SIZE] = '\0';
        if (recv_len > 0) {
            recv_buffer[recv_len] = '\0';
            extract_buffer(recv_buffer);
            cout << "The Server A received input <" << recv_buffer << ">" << endl;


            memset(send_buffer, 0, BUFFER_SIZE);


            // find recv_buffer in map
            int link_id = atoi(recv_buffer);
            it = mapA.find(link_id);
            int count = 0;
            if (it == mapA.end()) {
                // cannot find in map
                count = 0;
                strcpy(send_buffer, "0");
            } else {
                // find in map
                count = 1;
                strcpy(send_buffer, "1");
                vector<double> list = mapA[link_id];
                for (vector<double>::const_iterator i = list.begin(); i != list.end(); ++i) {
                    // convert to double
                    double num = *i;
                    ostringstream v0;
                    v0 << num;
                    string num_str = v0.str();                  

                    strcat(send_buffer, " ");
                    strcat(send_buffer, num_str.c_str());
                }
            } 
            cout << "The server A has found <" << count << "> match" << endl;



            // waiting AWS send to server A
            recvfrom(socket_A, &recv_buffer, (size_t)BUFFER_SIZE, 0, (struct sockaddr*)&client_addr, &client_addr_len);
            recv_buffer[BUFFER_SIZE] = '\0';
            // send result to AWS
            int sock_aws = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
            if (sock_aws < 0){
                perror("Failed to create socket");
                exit(1);
            }            
            
            
            struct sockaddr_in aws_addr;
            socklen_t aws_addr_len = sizeof(aws_addr);
            aws_addr.sin_family = PF_INET;
            aws_addr.sin_addr.s_addr = inet_addr(localhost);
            aws_addr.sin_port = AWS_PORT;

            fill_buffer(send_buffer);
            // send to AWS
            // cout << "send_buffer : " << send_buffer << ">" << endl;
            sendto(sock_aws, send_buffer, strlen(send_buffer), 0, (struct sockaddr*)&aws_addr, aws_addr_len);
            memset(send_buffer, 0, BUFFER_SIZE);

            cout << "The Server A finished sending the output to AWS" << endl;
            close(sock_aws);
        }

    }

  


    return 0;
}