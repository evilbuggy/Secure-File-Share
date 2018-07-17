#ifndef PACKET_H
#define PACKET_H

#include <string>
#include <stdint.h>
#include <algorithm>
#include "lint.h"
#include "tools.h"
#include "aes.h"
#include "rsa.h"

#define BUFFER_SIZE 1024

using namespace std;

static RSA_KEY rsa_key;
static uint8_t aes_key[16], aes_iv[16];

void initKey(){
    rsa_key.init();
}

static void stouarr(string &a, uint8_t b[16]){
    for(int i = 0; i < 128; i += 8){
        uint8_t cnt = 0;
        for(int j = 0; j < 8; j++){
            cnt <<= 1;
            cnt |= (a[i+j] - '0');
        }
        b[i/16] = cnt;
    }
}

static void uarrtos(string &a, uint8_t b[16]){
    a = "";
    for(int i = 0; i < 16; i++){
        for(int j = 7; j >= 0; j--){
            if(b[i]&(1<<j)){
                a = a + '1';
            }else{
                a = a + '0';
            }
        }
    }
}

int sendPublicKey(int sockfd){
    initKey();

    lint key = rsa_key.publicKey.key;
    lint modulus = rsa_key.publicKey.modulus;

    char packet[BUFFER_SIZE];

    lintToBinStr(key, packet, 256);
    lintToBinStr(modulus, packet + 256, 256);
    packet[512] = '\0';

    if(send(sockfd, packet, BUFFER_SIZE, 0) == -1){
        return -1;
    }

    return 0;
}

int sendSecretKey(int sockfd){
    char packet[BUFFER_SIZE];

    string tmp = genRandBinStr(128);
    stouarr(tmp, aes_key);

    lint secretKey = tmp;
    lint encSecretKey = rsa_key.publicKey.encrypt(secretKey);

    lintToBinStr(encSecretKey, packet, 256);
    packet[256] = '\0';

    if(send(sockfd, packet, BUFFER_SIZE, 0) == -1){
        return -1;
    }

    return 0;
}

int sendInitVal(int sockfd){
    char packet[BUFFER_SIZE];

    string tmp = genRandBinStr(128);
    uarrtos(tmp, aes_iv);

    lint initVal = tmp;
    lint encInitVal = rsa_key.publicKey.encrypt(initVal);

    lintToBinStr(encInitVal, packet, 256);
    packet[256] = '\0';

    if(send(sockfd, packet, BUFFER_SIZE, 0) == -1){
        return -1;
    }
    return 0;
}

int recvPublicKey(int sockfd){
    char tmpBuf[BUFFER_SIZE];
    if(recv(sockfd, tmpBuf, BUFFER_SIZE, 0) == -1){
        return -1;
    }

    string tmp = string(tmpBuf);

    rsa_key.publicKey.key = tmp.substr(0, 256);
    rsa_key.publicKey.modulus = tmp.substr(256);

    return 0;
}

int recvSecretKey(int sockfd){
    char tmpBuf[BUFFER_SIZE];
    if(recv(sockfd, tmpBuf, BUFFER_SIZE, 0) == -1){
        return -1;
    }
    lint tmp = string(tmpBuf);
    tmp = rsa_key.privateKey.decrypt(tmp);

    lintToBinStr(tmp, tmpBuf, 128);
    tmpBuf[128] = '\0';

    string key(tmpBuf);

    stouarr(key, aes_key);
    return 0;
}

int recvInitVal(int sockfd){
    char tmpBuf[BUFFER_SIZE];
    if(recv(sockfd, tmpBuf, BUFFER_SIZE, 0) == -1){
        return -1;
    }

    lint tmp = string(tmpBuf);
    tmp = rsa_key.privateKey.decrypt(tmp);

    lintToBinStr(tmp, tmpBuf, 128);
    tmpBuf[128] = '\0';

    string iv(tmpBuf);

    stouarr(iv, aes_iv);
    return 0;
}

int sendFileSize(int sockfd, uint64_t fileSize){
    string packet = "";
    for(int i = 0; i < 63; i++){
        if(fileSize&(1LL<<i)){
            packet = packet + '1';
        }else{
            packet = packet + '0';
        }
    }
    packet = packet + '0';
    reverse(packet.begin(), packet.end());
    return send(sockfd, packet.c_str(), BUFFER_SIZE, 0);
}

int recvFileSize(int sockfd, long long int &fileSize){
    char packet[BUFFER_SIZE];
    if(recv(sockfd, packet, BUFFER_SIZE, 0) < 0)return -1;
    fileSize = 0;
    for(int i = 1; i < 64; i++){
        fileSize <<= 1;
        fileSize |= (packet[i] - '0');
    }
    return 0;
}

int sendDataChunk(int sockfd, uint8_t buffer[BUFFER_SIZE]){
    uint8_t packet[BUFFER_SIZE];
    for(int i = 0; i < BUFFER_SIZE; i += 16){
        aes_cbc_encrypt(buffer + i, packet + i, aes_key, aes_iv);

    }
    return send(sockfd, packet, BUFFER_SIZE, 0);
}

int recvDataChunk(int sockfd, uint8_t buffer[1024]){
    uint8_t packet[BUFFER_SIZE];
    if(recv(sockfd, packet, BUFFER_SIZE, 0) < 0)return 0;
    for(int i = 0; i < 1024; i += 16){
        aes_cbc_decrypt(buffer + i, packet + i, aes_key, aes_iv);
    }
    return 0;
}

#endif
