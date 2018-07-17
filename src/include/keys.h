#ifndef KEYS_H
#define KEYS_H

#include "lint.h"
#include "keys.h"
#include "tools.h"
#include <iostream>

std::pair<lint, lint> producePrimes(int bitLength){
    lint prime1 = genPrime(bitLength);
    lint prime2 = genPrime(bitLength);
    while(prime1 == prime2){
        prime2 = genPrime(bitLength);
    }
    return std::make_pair(prime1, prime2);
}

std::pair<lint, lint> produceKeys(lint prime1, lint prime2, int bitLength){
    lint totient = (prime1 - 1)*(prime2 - 1);
    lint puKey = genRandBinStr(bitLength);
    lint prKey, tmp;
    while(gcd(puKey, totient, prKey, tmp) != 1){
        puKey = genRandBinStr(bitLength);
    }
    puKey = puKey % totient;
    prKey = prKey % totient;
    return std::make_pair(puKey, prKey);
}

#endif
