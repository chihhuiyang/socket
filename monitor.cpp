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
    // client tcp
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = PF_INET;
    server_addr.sin_addr.s_addr = inet_addr(localhost);
    server_addr.sin_port = AWS_MONITOR_TCP_PORT;


    cout << "The monitor is up and running" << endl;

    // buffer
    char recv_from_server_buffer[BUFFER_SIZE];
    char send_to_server_buffer[BUFFER_SIZE];
    char recv_buffer[BUFFER_SIZE];
    char copy_buffer[BUFFER_SIZE];
    
    
    // connect
    while (true) {
        // create socket
        int sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (sock < 0) {
            perror("Failed to create TCP socket.");
            exit(1);
        }

        while (true) {
            if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
                // continue
            } else {
                break;
            }
        }
 
        // cout << "receive from aws" << endl;
        // receive input from aws
        int recv_len = recv(sock, recv_buffer, sizeof(recv_buffer), 0);
        recv_buffer[BUFFER_SIZE] = '\0';
        extract_buffer(recv_buffer);
        strcpy(copy_buffer, recv_buffer);
        vector<double> vec = split(copy_buffer, ' ');
        double link_id = vec[0];
        double bit_size = vec[1];
        double power = vec[2];
        cout << "The monitor received link ID=<" << link_id << ">, size=<" << bit_size << ">, and power=<" << power << ">";
        cout << " from the AWS" << endl;

        // receive result from aws
        memset(recv_from_server_buffer, '\0', sizeof(recv_from_server_buffer));
        int recv_from_server_len = recv(sock, recv_from_server_buffer, sizeof(recv_from_server_buffer), 0);
        recv_from_server_buffer[BUFFER_SIZE] = '\0';
        // cout << "recv_from_server_buffer:" << recv_from_server_buffer << endl;
        extract_buffer(recv_from_server_buffer);

        // case 1 (found) : 1 delay transmission_time propagation_time 
        // case 2 (no match) : 0  
        vector<double> vec_res = split(recv_from_server_buffer, ' ');
        int found = (int)vec_res[0];

        if (found == 0) {
            cout << "Found no matches for link <" << link_id << ">" << endl;
        } else if (found == 1) {
            double delay = vec_res[1];  
            double transmission_time = vec_res[2];  
            double propagation_time = vec_res[3]; 
            cout << "The result for link <" << link_id << ">:" << endl;
            cout << "Tt = <" << transmission_time << ">ms" << endl;
            cout << "Tp = <" << propagation_time << ">ms" << endl;
            cout << "Delay = <" << delay << ">ms" << endl;
        }

        close(sock);
    }

    return 0;
}