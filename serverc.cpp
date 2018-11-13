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
#define CLIENT_TCP_PORT 25068
#define MONITOR_TCP_PORT 26068
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

    struct sockaddr_in server_C_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int socket_c;

    // buffer
    char send_buffer[BUFFER_SIZE];
    char recv_buffer[BUFFER_SIZE];
    int recv_len;
    
    // create the UDP socket
    if ((socket_c = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        perror("Failed to create socket C.");
        exit(1);
    }

    memset(&server_C_addr, 0, sizeof(server_C_addr));
    memset(&client_addr, 0, sizeof(client_addr));

    // server info
    server_C_addr.sin_family = PF_INET;
    server_C_addr.sin_addr.s_addr = inet_addr(localhost); 
    server_C_addr.sin_port = SERVER_C_PORT; 
   
    // bind the socket
    if (bind(socket_c, (struct sockaddr *)&server_C_addr, sizeof(server_C_addr)) < 0) {
        perror("Failed to bind.");
        exit(1);
    }
    cout << "The server C is up and running using UDP on port <" << SERVER_C_PORT << ">" << endl;

    // loop, wait for receiving data
    while (true) {

        recv_len = recvfrom(socket_c, &recv_buffer, (size_t)BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &client_addr_len);
        recv_buffer[BUFFER_SIZE] = '\0';
        if (recv_len > 0) {
            recv_buffer[recv_len] = '\0';
            extract_buffer(recv_buffer);
            // 1 link_id bit_size power 1 71 6821.6 15.3 -96.9
            // extract all the double
            char copy_buffer[BUFFER_SIZE];
            strncpy(copy_buffer, recv_buffer, sizeof(recv_buffer));
            // cout << "copy_buffer: " << copy_buffer << endl;         
            vector<double> vec = split(copy_buffer, ' ');

            int found = (int)vec[0];
            double link_id = vec[1];  
            double bit_size = vec[2];  
            double power = vec[3];            

            cout << "The server C received link information of link <" << link_id;
            cout << ">, file size <" << bit_size << ">, and signal power <" << power << ">" << endl;


            // build send_buffer
            // case 1 (found) : 1 delay transmission_time propagation_time 
            // case 2 (no match) : 0            
            memset(send_buffer, '\0', BUFFER_SIZE);

            // calculating
            if (found == 1) {
                double bandwidth = vec[5];  
                double length = vec[6];  
                double velocity = vec[7];
                double noise_power = vec[8];

                double delay = 987;
                double transmission_time = 123;
                double propagation_time = 456;

                strcpy(send_buffer, "1");
                stringstream dd, tt, pt;
                dd << delay;  
                tt << transmission_time;
                pt << propagation_time;
                strcat(send_buffer, " ");
                strcat(send_buffer, dd.str().c_str());
                strcat(send_buffer, " ");
                strcat(send_buffer, tt.str().c_str());
                strcat(send_buffer, " ");
                strcat(send_buffer, pt.str().c_str());                                
            } else if (found == 0) {
                strcpy(send_buffer, "0");
            }
            
            cout << "The server C finished the calculation for link <" << link_id << ">" << endl;


            // waiting AWS send to server C
            recvfrom(socket_c, &recv_buffer, (size_t)BUFFER_SIZE, 0, (struct sockaddr*)&client_addr, &client_addr_len);
            recv_buffer[BUFFER_SIZE] = '\0';
            // send result to AWS : 

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

            
            // send to AWS
            fill_buffer(send_buffer);
            // cout << "send_buffer : " << send_buffer << ">" << endl;
            sendto(sock_aws, send_buffer, strlen(send_buffer), 0, (struct sockaddr*)&aws_addr, aws_addr_len);
            

            cout << "The server C finished sending the output to AWS" << endl;
            close(sock_aws);
        }

    }

  


    return 0;
}