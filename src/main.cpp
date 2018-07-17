#include <iostream>
#include <stdint.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <cstring>
#include <fstream>
#include "packet.h"

#define PORT 9015
#define BUFFER_SIZE 1024

using namespace std;

int opType(){
    system("clear");
    cout<<"Please select the type of operation..."<<endl;
    cout<<"1. Send"<<endl;
    cout<<"2. Receive"<<endl;
    cout<<endl;
    cout<<"Input: ";
    string option;
    cin>>option;
    while(option != "1" && option != "2"){
        system("clear");
        cout<<"Invalid input! Please try again."<<endl<<endl;
        cout<<"Please select the type of operation..."<<endl;
        cout<<"1. Send"<<endl;
        cout<<"2. Receive"<<endl;
        cout<<endl;
        cout<<"Input: ";
        cin>>option;
    }
    return option[0] - '0';
}

int main(){
    int sockfd;
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        cout<<"Error starting application..."<<endl;
        exit(0);
    }

    srand(time(NULL));

    int operation = opType();
    if(operation == 1){
        int opt = 1;
        int addr_size = sizeof(struct sockaddr_in);

        struct sockaddr_in serv_addr, client_addr;
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(PORT);

        if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0){
            cout<<"Application error!"<<endl;
            exit(0);
        }

        if(bind(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0){
            cout<<"Application error!"<<endl;
            exit(0);
        }

        if(listen(sockfd, 1) < 0){
            cout<<"Application error!"<<endl;
            exit(0);
        }

        system("clear");
        cout<<"Enter the following passcode on receiving PC: ";

        char passCode[10];
        for(int i = 0; i < 8; i++){
            passCode[i] = char(rand()%26 + 'A');
            cout<<passCode[i];
        }
        passCode[8] = '\0';
        cout<<endl;

        int client_socket;

        while(true){
            client_socket = accept(sockfd, (struct sockaddr *) &client_addr, (socklen_t*)&addr_size);
            if(client_socket < 0){
                continue;
            }

            char recvPassCode[10];
            if(recv(client_socket, recvPassCode, 10, 0) < 0){
                cout<<"Application Error"<<endl;
            }

            if(strcmp(recvPassCode,  passCode) == 0){
                break;
            }
        }

        system("clear");
        cout<<"Connection established successfully..."<<endl;

        cout<<"Sending RSA Public Key..."<<endl;
        cout<<"This may take some time..."<<endl;
        if(sendPublicKey(client_socket) < 0){
            cout<<"Error sending RSA Public Key..."<<endl;
            exit(0);
        }

        cout<<"Receiving secret key..."<<endl;
        if(recvSecretKey(client_socket) < 0){
            cout<<"Error receiving Secret Key..."<<endl;
            exit(0);
        }

        cout<<"Receiving init value..."<<endl;
        if(recvInitVal(client_socket) < 0){
            cout<<"Error receiving Initial Value used for CBC mode of AES encryption..."<<endl;
            exit(0);
        }

        char filePath[BUFFER_SIZE];
        FILE *f = popen("zenity --file-selection", "r");
        if(f == NULL){
            cout<<"No file selected..."<<endl;
            exit(0);
        }
        fgets(filePath, BUFFER_SIZE, f);

        filePath[strlen(filePath) - 1] = '\0';
        uint64_t fileSize = getFileSize(filePath);
        FILE *file = fopen(filePath, "rb");

        if(send(client_socket, parsePath(filePath), BUFFER_SIZE, 0) < 0){
            cout<<"Application error!"<<endl;
            exit(0);
        }

        cout<<"File opened successfully"<<endl;

        if(sendFileSize(client_socket, fileSize) < 0){
            cout<<"Application error!"<<endl;
            exit(0);
        }

        cout<<"File size sent successfully"<<endl;

        long long int totBytesSent = 0, length = fileSize;
        int param = 0;
        uint8_t buffer[BUFFER_SIZE];
        while(length > 0){
            if(param == 0){
                system("clear");
                cout<<totBytesSent<<" bytes sent out of "<<fileSize<<" bytes..."<<endl;
            }
            param = (param + 1)%20;
            if(length < BUFFER_SIZE){
                fread(buffer, length, 1, file);
            }else{
                fread(buffer, BUFFER_SIZE, 1, file);
            }

            int cnt = sendDataChunk(client_socket, buffer);

            if(cnt == -1){
                cout<<"Error sending file..."<<endl;
                exit(0);
            }

            totBytesSent += cnt;
            length -= cnt;
        }
        fclose(file);
        cout<<"File sent successfully..."<<endl;

    }else{
        string ipAddr;

        system("clear");
        cout<<"Enter the IP Address of the sender : ";
        cin>>ipAddr;

        struct sockaddr_in serv_addr;
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = inet_addr(ipAddr.c_str());
        serv_addr.sin_port = htons(PORT);

        if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) < 0){
            cout<<"Error connecting to the sender..."<<endl;
            exit(0);
        }

        string passCode;

        cout<<"Enter the Pass Code : ";
        cin>>passCode;

        if(send(sockfd, passCode.c_str(), 10, 0) < 0){
            cout<<"Error connecting to the sender..."<<endl;
            exit(0);
        }

        system("clear");
        cout<<"Receiving RSA Public key..."<<endl;
        if(recvPublicKey(sockfd) < 0){
            cout<<"Application error!"<<endl;
            exit(0);
        }

        cout<<"Sending Secret Key..."<<endl;
        if(sendSecretKey(sockfd) < 0){
            cout<<"Application error!"<<endl;
            exit(0);
        }

        cout<<"Sending Initial Value used for CBC mode of AES encryption..."<<endl;
        if(sendInitVal(sockfd) < 0){
            cout<<"Application error!"<<endl;
            exit(0);
        }

        char fileName[BUFFER_SIZE];
        if(recv(sockfd, fileName,  BUFFER_SIZE, 0) < 0){
            cout<<"Application error!"<<endl;
            exit(0);
        }

        FILE *file = fopen(fileName, "wb");

        if(file == NULL){
            cout<<"File can't be created or written..."<<endl;
            exit(0);
        }

        long long int fileSize, length;
        if(recvFileSize(sockfd, fileSize) < 0){
            cout<<"Error receiving file..."<<endl;
        }
        length = fileSize;
        uint8_t buffer[BUFFER_SIZE];

        int param = 0;
        while(fileSize > 0){
            if(param == 0){
                system("clear");
                cout<<length - fileSize<<" bytes received out of "<<length<<" bytes..."<<endl;
            }
            param = (param + 1)%20;

            if(recvDataChunk(sockfd, buffer) < 0){
                cout<<"Error receiving file..."<<endl;
                exit(0);
            }

            if(fileSize < BUFFER_SIZE){
                fwrite(buffer, fileSize, 1, file);
            }else{
                fwrite(buffer, BUFFER_SIZE, 1, file);
            }
            fileSize -= BUFFER_SIZE;
        }
        fclose(file);

        cout<<"File received successfully..."<<endl;
    }
}
