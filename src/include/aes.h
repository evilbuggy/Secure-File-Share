#ifndef AES_H_INC
#define AES_H_INC

#include <stdint.h>

void aes_encrypt(uint8_t ptxt[16], uint8_t ctxt[16], uint8_t key[16]);
void aes_decrypt(uint8_t ptxt[16], uint8_t ctxt[16], uint8_t key[16]);

void aes_cbc_encrypt(uint8_t ptxt[16], uint8_t ctxt[16], uint8_t key[16], uint8_t iv[16]);
void aes_cbc_decrypt(uint8_t ptxt[16], uint8_t ctxt[16], uint8_t key[16], uint8_t iv[16]);

#endif
