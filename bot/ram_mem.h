#pragma once
#include <stdint.h>

struct dict_item
{
  char *value;
  uint16_t value_len;
};

#define DICT_DOMAIN_NAME 0
#define DICT_MAX_ITEMS 10

void dict_init(void);
void dict_add(int index, const char *value);
const char *dict_get(int index);
void dict_free(void);

void xor_encrypt_decrypt(char *data, size_t len, char key);
