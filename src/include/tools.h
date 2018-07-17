#ifndef TOOLS_H_INC
#define TOOLS_H_INC

#include <string>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include "lint.h"

#include <iostream>

using namespace std;

static const int NUMBER_OF_ITERS = 12;

std::string hexToBin(std::string num){
    std::string ans = "";
    for(size_t i = 0; i < num.size(); i++){
        int t;
        if(num[i] >= '0' && num[i] <= '9'){
            t = num[i] - '0';
        }else{
            t = num[i] - 'a' + 10;
        }
        for(int j = 3; j >= 0; j--){
            if(t&(1<<j))ans = ans + '1';
            else ans = ans + '0';
        }
    }
    return ans;
}

char* parsePath(char *buf){
    int i = 0, ending = -1;
    while(buf[i] != '\0'){
        if(buf[i] == '/'){
            ending = i;
        }
        i++;
    }
    return (buf + ending + 1);
}

long long int getFileSize(char *filePath){
    std::ifstream file(filePath, std::ios::binary |  std::ios::in);
    std::streampos  begin, end;

    begin = file.tellg();
    file.seekg( 0, std::ios::end );
    end = file.tellg();
    file.close();
    return static_cast<long long int>(end - begin);
}

void lintToBinStr(lint num, char *packet, int bitLength){
    for(int k = bitLength - 1; k >= 0; k--){
        if(num%2 == 0){
            packet[k] = '0';
        }else{
            packet[k] = '1';
        }
        num = num >> 1;
    }
}

lint modPow(lint a, lint p, lint mod){
    //cout<<p.dig[1]<<" "<<p.dig[0]<<endl;
    if(p == 0)return 1;
    lint t = modPow(a, p>>1, mod);
    t = (t*t)%mod;
    if(p%2 == 0)return t;
    return (t*a)%mod;
}

lint gcd(lint a, lint b, lint &x, lint &y){
    if(a == 0){
        x = 0;
        y = 1;
        return b;
    }
    lint p, q;
    lint g = gcd(b%a, a, p, q);
    x = q - (b/a)*p;
    y = p;
    return g;
}

std::string genRandBinStr(int bitLength){
    srand(time(NULL));
    std::string ans = "";
    for(int i = 0; i < bitLength; i++){
        ans = ans + char(rand()%2 + '0');
    }
    return ans;
}

bool millerRabin(lint prime, int bitLength){
    int r = 0;
    lint d = prime - 1;
    while(d%2 == 0){
        r++;
        d = d>>1;
    }
    for(int i = 0; i < NUMBER_OF_ITERS; i++){
        lint a = genRandBinStr(bitLength);
        if(a >= prime){
            i--;
            continue;
        }
        lint b = modPow(a, d, prime);
        if(b == 1 || b == prime-1)continue;
        bool flag = false;
        for(int j = 0; j < r-1; j++){
            b = (b*b)%prime;
            if(b == 1)return false;
            if(b == prime-1){
                flag = true;
                break;
            }
        }
        if(!flag)return false;
    }
    return true;
}

lint genPrime(int bitLength){
    std::string s = genRandBinStr(bitLength);
    s[0] = '1';
    s[bitLength - 1] = '1';
    lint prime(s);
    while(!millerRabin(prime, bitLength)){
        prime = prime + 2;
    }
    return prime;
}

#endif
