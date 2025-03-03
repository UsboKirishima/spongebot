#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define XOR_KEY 0xAA

void xor_encrypt_decrypt(char *data, size_t len, char key) {
    for (size_t i = 0; i < len; i++) {
        data[i] ^= key;
    }
}

int main(int argc, char **argv) {
    
    if(argc < 2) {
        printf("\nUsage: %s <data>", argv[0]);
        return 1;
    }

    unsigned char *data = argv[1];
    xor_encrypt_decrypt(data, strlen(data), XOR_KEY);
    
    printf("\n%s", "XORing data...");
    printf("\nXORed data: ");

    for(int i = 0; i < strlen(data); i++) {
        printf("\\x%02X", ((unsigned char *)data)[i]);
    }

    printf("\n");
    
    return 0;
}
