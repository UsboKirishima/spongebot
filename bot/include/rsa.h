// SPDX-License-Identifier: MIT
#ifndef __RSA_H__
#define __RSA_H__

#include <stdint.h>

// This is the header file for the library librsaencrypt.a

// Change this line to the file you'd like to use as a source of primes.
// The format of the file should be one prime per line.
//TODO: Remove file and generates random primes hardcoded
char *PRIME_SOURCE_FILE = "primes.txt";


struct public_key_class{
  long long modulus;
  long long exponent;
};

struct private_key_class{
  long long modulus;
  long long exponent;
};

void rsa_gen_keys(struct public_key_class *pub, struct private_key_class *priv, const char *PRIME_SOURCE_FILE);
long long *rsa_encrypt(const char *message, const unsigned long message_size, const struct public_key_class *pub);
char *rsa_decrypt(const long long *message, const unsigned long message_size, const struct private_key_class *pub);

#endif