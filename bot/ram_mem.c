#include <stdlib.h>
#include <string.h>
#include <stdio.h> 
#include "ram_mem.h"

#define XOR_KEY 0xAA

struct dict_item dict[DICT_MAX_ITEMS];

void dict_init(void) {
    for (int i = 0; i < DICT_MAX_ITEMS; i++) {
        dict[i].value = NULL;
        dict[i].value_len = 0;
    }

    dict_add(DICT_DOMAIN_NAME, "\xC6\xC5\xC9\xCB\xC6\xC2\xC5\xD9\xDE"); // domain name
}

void xor_encrypt_decrypt(char *data, size_t len, char key) {
    for (size_t i = 0; i < len; i++) {
        data[i] ^= key;
    }
}


void dict_add(int index, const char *value) {
    if (index < 0 || index >= DICT_MAX_ITEMS) {
#ifdef DEBUG
        printf("[dictionary]: Index out of range!\n");
#endif
        return;
    }

    dict[index].value_len = strlen(value);
    dict[index].value = (char *)malloc(dict[index].value_len + 1);

    if (!dict[index].value) {
#ifdef DEBUG
        printf("[dictionary] Error during memory allocation!\n");
#endif
        return;
    }

    strcpy(dict[index].value, value);

}

const char *dict_get(int index) {
    if (index < 0 || index >= DICT_MAX_ITEMS || dict[index].value == NULL) {
        return NULL;
    }

    char *decrypted_value = (char *)malloc(dict[index].value_len + 1); 
    if (decrypted_value == NULL) {
#ifdef DEBUG
        printf("[dectionary] Error during memory allocation\n");
#endif
        return NULL;
    }

    memcpy(decrypted_value, dict[index].value, dict[index].value_len + 1);  
    xor_encrypt_decrypt(decrypted_value, dict[index].value_len, XOR_KEY);

    return decrypted_value;
}

void dict_free(void) {
    for (int i = 0; i < DICT_MAX_ITEMS; i++) {
        if (dict[i].value) {
            free(dict[i].value);
            dict[i].value = NULL;
        }
    }
}

