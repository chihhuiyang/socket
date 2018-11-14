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

vector<string> splitToString(const string &s, char delim) {
    vector<string> res;
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) {
        string val = item.c_str();
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

void query_server_A(string link_id) {
    // create udp socket
    struct sockaddr_in server_A;
    memset(&server_A, 0, sizeof(server_A));
    int sock_A = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock_A < 0) {
        perror("Failed to create socket");
        exit(1);
    }
    server_A.sin_family = PF_INET;
    server_A.sin_addr.s_addr = inet_addr(localhost);
    server_A.sin_port = SERVER_A_PORT;
    // send to server A
    char send_to_server_buffer[BUFFER_SIZE];
    strncpy(send_to_server_buffer, link_id.c_str(), sizeof(link_id));    
    fill_buffer(send_to_server_buffer);
    sendto(sock_A, send_to_server_buffer, sizeof(send_to_server_buffer), 0, (struct sockaddr*) &server_A, sizeof(server_A));  
    cout << "The AWS sent link ID=<" << link_id << "> to Backend-Server A using UDP over port <" << AWS_PORT << ">" << endl;
    close(sock_A);
}

void query_server_B(string link_id) {
    // create udp socket
    struct sockaddr_in server_B;
    memset(&server_B, 0, sizeof(server_B));
    int sock_B = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock_B < 0) {
        perror("Failed to create socket");
        exit(1);
    }
    server_B.sin_family = PF_INET;
    server_B.sin_addr.s_addr = inet_addr(localhost);
    server_B.sin_port = SERVER_B_PORT;
    // send to server B
    char send_to_server_buffer[BUFFER_SIZE];
    strncpy(send_to_server_buffer, link_id.c_str(), sizeof(link_id));    
    fill_buffer(send_to_server_buffer);
    sendto(sock_B, send_to_server_buffer, sizeof(send_to_server_buffer), 0, (struct sockaddr*) &server_B, sizeof(server_B));  
    cout << "The AWS sent link ID=<" << link_id << "> to Backend-Server B using UDP over port <" << AWS_PORT << ">" << endl;
    close(sock_B);
}

void query_server_C(char* input) {
    // create udp socket
    struct sockaddr_in server_C;
    memset(&server_C, 0, sizeof(server_C));
    int sock_C = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock_C < 0) {
        perror("Failed to create socket");
        exit(1);
    }
    server_C.sin_family = PF_INET;
    server_C.sin_addr.s_addr = inet_addr(localhost);
    server_C.sin_port = SERVER_C_PORT;
    // send to server C
    char send_to_server_buffer[BUFFER_SIZE];
    strncpy(send_to_server_buffer, input, sizeof(input));    
    fill_buffer(send_to_server_buffer);
    // cout << "send_to_server_buffer: " << send_to_server_buffer << endl;
    sendto(sock_C, send_to_server_buffer, sizeof(send_to_server_buffer), 0, (struct sockaddr*) &server_C, sizeof(server_C));  
    
    // extract input
    char copy_buffer[BUFFER_SIZE];
    strncpy(copy_buffer, send_to_server_buffer, sizeof(send_to_server_buffer));
    copy_buffer[BUFFER_SIZE] = '\0';
    // cout << "copy_buffer: " << copy_buffer << endl;
    vector<double> vec = split(copy_buffer, ' ');
    // convert double to string
    ostringstream v0, v1, v2;
    v0 << vec[0];
    v1 << vec[1];
    v2 << vec[2];
    string link_id = v0.str();
    string bit_size = v1.str();
    string power = v2.str();

    cout << "The AWS sent link ID=<" << link_id << ">, size=<" << bit_size << ">, power=<" << power;
    cout << ">, and link information to Backend-Server C using UDP over port <" << AWS_PORT << ">" << endl;
    close(sock_C);
}

void aws_send_to_server_A() {
    // create udp socket
    struct sockaddr_in server_A;
    memset(&server_A, 0, sizeof(server_A));
    int sock_A = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock_A < 0) {
        perror("Failed to create socket");
        exit(1);
    }
    server_A.sin_family = PF_INET;
    server_A.sin_addr.s_addr = inet_addr(localhost);
    server_A.sin_port = SERVER_A_PORT;
    string empty = "";
    sendto(sock_A, empty.c_str(), sizeof(empty), 0, (struct sockaddr*) &server_A, sizeof(server_A)); 
    close(sock_A);
}

void aws_send_to_server_B() {
    // create udp socket
    struct sockaddr_in server_B;
    memset(&server_B, 0, sizeof(server_B));
    int sock_B = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock_B < 0) {
        perror("Failed to create socket");
        exit(1);
    }
    server_B.sin_family = PF_INET;
    server_B.sin_addr.s_addr = inet_addr(localhost);
    server_B.sin_port = SERVER_B_PORT;
    string empty = "";
    sendto(sock_B, empty.c_str(), sizeof(empty), 0, (struct sockaddr*) &server_B, sizeof(server_B)); 
    close(sock_B);
}

void aws_send_to_server_C() {
    // create udp socket
    struct sockaddr_in server_C;
    memset(&server_C, 0, sizeof(server_C));
    int sock_C = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock_C < 0) {
        perror("Failed to create socket");
        exit(1);
    }
    server_C.sin_family = PF_INET;
    server_C.sin_addr.s_addr = inet_addr(localhost);
    server_C.sin_port = SERVER_C_PORT;
    string empty = "";
    sendto(sock_C, empty.c_str(), sizeof(empty), 0, (struct sockaddr*) &server_C, sizeof(server_C)); 
    close(sock_C);
}

void receive_from_server(char name, int sock_udp) {

    struct sockaddr_in server_A;
    memset(&server_A, 0, sizeof(server_A));
    socklen_t server_A_addr_len = sizeof(server_A);
    char recv_A_buffer[BUFFER_SIZE];   
    int recv_A_len = recvfrom(sock_udp, recv_A_buffer, (size_t)BUFFER_SIZE, 0, (struct sockaddr*)&server_A, &server_A_addr_len);
    int found = atoi(recv_A_buffer);
    // cout << "recv_A_buffer:" << recv_A_buffer << endl;
    extract_buffer(recv_A_buffer);
    int count_A = atoi(recv_A_buffer);
    cout << "The AWS receives <" << count_A << "> matches from Backend-Server <A> using UDP over port <" << AWS_PORT << ">" << endl;   
}



int main(){
    // UDP to server
    struct sockaddr_in aws_udp_addr;
    memset(&aws_udp_addr, 0, sizeof(aws_udp_addr));
    int sock_udp = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock_udp < 0) {
        perror("Failed to create socket");
        exit(1);
    }    
    aws_udp_addr.sin_family = PF_INET;
    aws_udp_addr.sin_addr.s_addr = inet_addr(localhost);
    aws_udp_addr.sin_port = AWS_PORT;
    bind(sock_udp, (struct sockaddr *)&aws_udp_addr, sizeof(aws_udp_addr));
    // TCP to client
    struct sockaddr_in aws_client_tcp_addr;
    memset(&aws_client_tcp_addr, 0, sizeof(aws_client_tcp_addr));
    int sock_client_tcp = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock_client_tcp < 0) {
        perror("Failed to create socket");
        exit(1);
    }      
    aws_client_tcp_addr.sin_family = PF_INET;
    aws_client_tcp_addr.sin_addr.s_addr = inet_addr(localhost);
    aws_client_tcp_addr.sin_port = AWS_CLIENT_TCP_PORT;
    bind(sock_client_tcp, (struct sockaddr *)&aws_client_tcp_addr, sizeof(aws_client_tcp_addr));
    // TCP to monitor
    struct sockaddr_in aws_monitor_tcp_addr; 
    memset(&aws_monitor_tcp_addr, 0, sizeof(aws_monitor_tcp_addr));
    int sock_monitor_tcp = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock_monitor_tcp < 0) {
        perror("Failed to create socket");
        exit(1);
    }      
    aws_monitor_tcp_addr.sin_family = PF_INET;
    aws_monitor_tcp_addr.sin_addr.s_addr = inet_addr(localhost);
    aws_monitor_tcp_addr.sin_port = AWS_MONITOR_TCP_PORT;
    bind(sock_monitor_tcp, (struct sockaddr *)&aws_monitor_tcp_addr, sizeof(aws_monitor_tcp_addr));
    
    cout<<"The AWS is up and running"<<endl;

    while (true) {
        // listen monitor : before listen client
        // cout << "###listen monitor" << endl;
        listen(sock_monitor_tcp, BACKLOG);
        struct sockaddr_in monitor_addr;
        socklen_t monitor_len = sizeof(struct sockaddr_in);
        int monitor_tcp_child = accept(sock_monitor_tcp , (struct sockaddr *)&monitor_addr, &monitor_len);
        if (monitor_tcp_child < 0){
            perror(("Failed to accept monitor."));
            exit(1);
        }
        // listen client
        // cout << "###listen client" << endl;
        listen(sock_client_tcp, BACKLOG);
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(struct sockaddr_in);
        int client_tcp_child = accept(sock_client_tcp , (struct sockaddr *)&client_addr, &client_len);
        if (client_tcp_child < 0){
            perror("Failed to accept client.");
            exit(1);
        }

        // buffer input from client
        char recv_buffer[BUFFER_SIZE];
        char copy_buffer[BUFFER_SIZE];
        int recv_len = recv(client_tcp_child, recv_buffer, sizeof(recv_buffer), 0);
        recv_buffer[BUFFER_SIZE] = '\0';
        // cout << "recive buffer from client: " << recv_buffer << endl;
        extract_buffer(recv_buffer);
        strcpy(copy_buffer, recv_buffer);
        copy_buffer[BUFFER_SIZE] = '\0';
        vector<string> vec = splitToString(copy_buffer, ' ');
        string link_id = vec[0];
        string bit_size = vec[1];
        string power = vec[2];   

        cout << "The AWS received link ID=<" << link_id << ">, size=<" << bit_size << ">, and power=<" << power << ">";
        cout << " from the client using TCP over port <" << aws_client_tcp_addr.sin_port << ">" << endl;

        // send the input to monitor
        fill_buffer(recv_buffer);
        send(monitor_tcp_child, recv_buffer, sizeof(recv_buffer), 0);
        extract_buffer(recv_buffer);
        cout << "The AWS sent link ID=<" << link_id << ">, size=<" << bit_size << ">, and power=<" << power << ">";
        cout << " to the monitor using TCP over port <" << aws_monitor_tcp_addr.sin_port << ">" << endl;

        // querying backend A
        query_server_A(link_id);
        // querying backend B
        query_server_B(link_id);
        // AWS send empty buffer to server A
        aws_send_to_server_A();
        // receive from server A
        struct sockaddr_in server_A;
        memset(&server_A, 0, sizeof(server_A));
        socklen_t server_A_addr_len = sizeof(server_A);
        char recv_A_buffer[BUFFER_SIZE];   
        int recv_A_len = recvfrom(sock_udp, recv_A_buffer, (size_t)BUFFER_SIZE, 0, (struct sockaddr*)&server_A, &server_A_addr_len);
        recv_A_buffer[BUFFER_SIZE] = '\0';
        // cout << "recv_A_buffer:" << recv_A_buffer << endl;
        extract_buffer(recv_A_buffer);
        int count_A = atoi(recv_A_buffer);  // 0 or 1
        cout << "The AWS receives <" << count_A << "> matches from Backend-Server <A> using UDP over port <" << AWS_PORT << ">" << endl;   
        
        // AWS end empty buffer to server B              
        aws_send_to_server_B();
        // receive from server B
        struct sockaddr_in server_B;
        memset(&server_B, 0, sizeof(server_B));
        socklen_t server_B_addr_len = sizeof(server_B);
        char recv_B_buffer[BUFFER_SIZE];   
        int recv_B_len = recvfrom(sock_udp, recv_B_buffer, (size_t)BUFFER_SIZE, 0, (struct sockaddr*)&server_B, &server_B_addr_len);
        recv_B_buffer[BUFFER_SIZE] = '\0';
        // cout << "recv_B_buffer:" << recv_B_buffer << endl;
        extract_buffer(recv_B_buffer);
        int count_B = atoi(recv_B_buffer);  // 0 or 1
        cout << "The AWS receives <" << count_B << "> matches from Backend-Server <B> using UDP over port <" << AWS_PORT << ">" << endl;   



        
        string server_C_input = link_id + " " + bit_size + " " + power;

        // build server_C_input
        if (count_A == 0 && count_B == 0) {
            // no found
            server_C_input = "0 " + server_C_input;
        } else {
            // found
            if (count_A == 1) {
                server_C_input = "1 " + server_C_input + " " + recv_A_buffer;
            } else if (count_B == 1) {
                server_C_input = "1 " + server_C_input + " " + recv_B_buffer;
            }
        }
        char send_to_server_C[BUFFER_SIZE];
        memset(send_to_server_C, '\0', sizeof(send_to_server_C));
        strcpy(send_to_server_C, server_C_input.c_str());
        fill_buffer(send_to_server_C);
        send_to_server_C[BUFFER_SIZE] = '\0';
        // cout << "send_to_server_C:" << send_to_server_C << endl;



        // querying backend C
        // create udp socket
        struct sockaddr_in query_server_C;
        memset(&query_server_C, 0, sizeof(query_server_C));
        int sock_C = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (sock_C < 0) {
            perror("Failed to create socket");
            exit(1);
        }
        query_server_C.sin_family = PF_INET;
        query_server_C.sin_port = SERVER_C_PORT;
        query_server_C.sin_addr.s_addr = inet_addr(localhost);
        // send to server C
        sendto(sock_C, send_to_server_C, sizeof(send_to_server_C), 0, (struct sockaddr*) &query_server_C, sizeof(query_server_C));  
        
        cout << "The AWS sent link ID=<" << link_id << ">, size=<" << bit_size << ">, power=<" << power;
        cout << ">, and link information to Backend-Server C using UDP over port <" << AWS_PORT << ">" << endl;
        close(sock_C);





        // AWS send empty buffer to server C
        aws_send_to_server_C();
        // AWS receive delay buffer from server C
        struct sockaddr_in server_C;
        memset(&server_C, 0, sizeof(server_C));
        socklen_t server_C_addr_len = sizeof(server_C);
        char recv_C_buffer[BUFFER_SIZE];
        int recv_c_len = recvfrom(sock_udp, recv_C_buffer, (size_t)BUFFER_SIZE, 0, (struct sockaddr*)&server_C, &server_C_addr_len);
        recv_C_buffer[BUFFER_SIZE] = '\0';
        // cout << "recv_C_buffer:" << recv_C_buffer << endl;
        cout << "The AWS receives outputs from Backend-Server C using UDP over port <" << AWS_PORT << ">" << endl;   
        extract_buffer(recv_C_buffer);




        // case 1 (found) : 1 delay transmission_time propagation_time 
        // case 2 (no match) : 0 
        vector<double> vec_final = split(recv_C_buffer, ' ');
        int found = (int)vec_final[0];        

        if (found == 0) {
            cout << "The AWS sent No Match to the monitor and the client using TCP over ports <" << aws_client_tcp_addr.sin_port << "> and <" << aws_monitor_tcp_addr.sin_port << ">, respectively" << endl;
        } else if (found == 1) {
            double delay = vec_final[1];  
            double transmission_time = vec_final[2];  
            double propagation_time = vec_final[3];
            cout << "The AWS sent delay=<" << delay << ">ms to the client using TCP over port <" << aws_client_tcp_addr.sin_port << ">" << endl;
            cout << "The AWS sent detailed results to the monitor using TCP over port <" << aws_monitor_tcp_addr.sin_port << ">" << endl; 

        }

        // send delay result to monitor
        if (send(monitor_tcp_child, recv_C_buffer , sizeof(recv_C_buffer), 0) < 0) {
            perror("Failed to send result to monitor.");
            exit(1);
        }

        // send delay result to client
        if (send(client_tcp_child, recv_C_buffer , sizeof(recv_C_buffer), 0) < 0) {
            perror("Failed to send result to client.");
            exit(1);
        }        

    }
    return 0;
}