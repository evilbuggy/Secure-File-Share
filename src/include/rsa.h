#ifndef RSA_H_INC
#define RSA_H_INC

#include "lint.h"
#include "tools.h"
#include "keys.h"
#include <iostream>
using namespace std;

static const int BIT_LENGTH = 128;

struct RSA_PUB_KEY{
    lint modulus;
    lint key;

    lint encrypt(lint);
};

struct RSA_PRI_KEY{
    lint prime1, prime2;
    lint modulus;
    lint key;

    lint decrypt(lint);
};

struct RSA_KEY{
    RSA_PUB_KEY publicKey;
    RSA_PRI_KEY privateKey;

    void init();
};

void RSA_KEY::init(){
    std::pair<lint, lint> primes = producePrimes(BIT_LENGTH);
    privateKey.prime1 = primes.first, privateKey.prime2 = primes.second;
    publicKey.modulus = primes.first*primes.second;
    privateKey.modulus = publicKey.modulus;
    std::pair<lint, lint> keys = produceKeys(primes.first, primes.second, 32);
    publicKey.key = keys.first, privateKey.key = keys.second;
}

lint RSA_PUB_KEY::encrypt(lint message){
    return modPow(message, key, modulus);
}

lint RSA_PRI_KEY::decrypt(lint message){
    return modPow(message, key, modulus);
}

#endif
