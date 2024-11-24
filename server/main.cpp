#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>  // Added for TCP options
#include <unistd.h>
#include <vector>
#include <thread>
#include <errno.h>

#define ADDRESS "0.0.0.0"
#define PORT 1200
#define BUFFER_SIZE 1024

using namespace std;

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE] = {0};
    
    // Configure keep-alive
    int keepalive = 1;
    setsockopt(client_socket, SOL_SOCKET, SO_KEEPALIVE, &keepalive, sizeof(keepalive));

    #ifdef __linux__
        int keepcnt = 5;
        int keepidle = 30; 
        int keepintvl = 5;

        setsockopt(client_socket, IPPROTO_TCP, TCP_KEEPCNT, &keepcnt, sizeof(keepcnt));
        setsockopt(client_socket, IPPROTO_TCP, TCP_KEEPIDLE, &keepidle, sizeof(keepidle));
        setsockopt(client_socket, IPPROTO_TCP, TCP_KEEPINTVL, &keepintvl, sizeof(keepintvl));
    #endif

    cout << "Um cliente conectou" << endl;

    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        int valread = read(client_socket, buffer, BUFFER_SIZE - 1);
        
        if (valread <= 0) {
            if (valread == 0) {
                cout << "um cliente desconectado" << endl;
            } else {
                cerr << "Erro na leitura do cliente: " << strerror(errno) << endl;
            }
            break;
        }

        buffer[valread] = '\0';
        cout << "Mensagem: " << buffer << endl;

        // string resposta = "Mensagem recebida com sucesso!";
        // if (send(client_socket, resposta.c_str(), resposta.length(), 0) < 0) {
        //     cerr << "Erro ao enviar resposta: " << strerror(errno) << endl;
        //     break;
        // }
    }

    close(client_socket);
}

int main() {
    int server_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    vector<thread> client_threads;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        cerr << "Erro ao criar o socket" << endl;
        return -1;
    }

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        cerr << "Erro ao definir socket options" << endl;
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        cerr << "Erro ao definir IP e Porta" << endl;
        return -1;
    }

    if (listen(server_fd, 5) < 0) {
        cerr << "Erro ao escutar comunicacao..." << endl;
        return -1;
    }

    cout << "Servidor iniciado no endereco: " << ADDRESS << ":" << PORT << endl;
    cout << "Aguardando conexoes..." << endl;

    while (true) {
        int new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
        if (new_socket < 0) {
            cerr << "Erro ao aceitar conexão do cliente: " << strerror(errno) << endl;
            continue;
        }

        client_threads.emplace_back(handle_client, new_socket);
        client_threads.back().detach();
    }

    return 0;
}