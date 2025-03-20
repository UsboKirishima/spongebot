#pragma once
#include <stdint.h>

struct dict_item
{
  char *value;
  uint16_t value_len;
};

struct dict_user_agent
{
  char *value;
  uint16_t value_len;
};

#define DICT_DOMAIN_NAME 0
#define DICT_SERVER_HELLO 1
#define ATK_LOCAL_ADDR 2
#define UPDATER_URL 3
#define UPDATER_TMP_FILE 4
#define UPDATER_VER_FILE 5

#define DICT_MAX_ITEMS 20
#define DICT_AGENTS_MAX 21

void dict_init(void);
void dict_agent_add(int index, const char *value);
void dict_add(int index, const char *value);
char *dict_get(int index);
void dict_free(void);
char *get_random_user_agent();

void xor_encrypt_decrypt(char *data, size_t len, uint32_t key);
