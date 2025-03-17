#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define XOR_KEY 0x87f08f33U

void xor_encrypt_decrypt(char *data, size_t len, uint32_t key)
{
    uint8_t keys[4] = {
        key & 0xFF,
        (key >> 8) & 0xFF,
        (key >> 16) & 0xFF,
        (key >> 24) & 0xFF};

    for (size_t i = 0; i < len; i++)
    {
        data[i] ^= keys[i % 4];
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
