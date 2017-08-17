/**
 *  @file ndconfig.c
 *  @brief Program Config Implementation
 *  @author Eric Fehr (ricofehr@nextdeploy.io, github: ricofehr)
 */

#include <string.h>
#include <stdlib.h>

#include "ndeployblink1/core/ndconfig.h"

struct ndconfig *g_ndconfig = NULL;

void nd_config_init_config()
{
    FILE *fp;
    char *tmp;
    int last;
    char username[255];
    char password[255];
    char endpoint[255];

    /* parse config file */
    fp = fopen("config/nextdeploy.conf", "r");
    if (!fp) {
        fp = fopen("config/nextdeploy.conf.default", "r");
        if (!fp) {
            fprintf(stderr, "No config file found !");
            exit(1);
        }
    }
    fgets(username, 255, fp);
    fgets(password, 255, fp);
    fgets(endpoint, 255, fp);
    fclose(fp);

    g_ndconfig = malloc(sizeof(struct ndconfig));
    if (g_ndconfig == NULL) {
        fprintf(stderr, "Memory Error during config file reading !");
        exit(1);
    }

    /* Split username line to isolate data and format value */
    tmp = strtok(username, ":");
    tmp = strtok(NULL, ":");
    last = strlen(tmp) - 1;
    while (tmp[last] == '\n' || tmp[last] == ' ') {
        tmp[last] = '\0';
        --last;
    }
    while (tmp[0] == ' ') {
        ++tmp;
    }
    g_ndconfig->username = (char*)malloc((strlen(tmp) + 1) * sizeof(char));
    if (g_ndconfig->username == NULL) {
        fprintf(stderr, "Memory Error during config file reading !");
        exit(1);
    }
    strcpy(g_ndconfig->username, tmp);

    /* Split password line to isolate data and format value */
    tmp = strtok(password, ":");
    tmp = strtok(NULL, ":");
    last = strlen(tmp) - 1;
    while (tmp[last] == '\n' || tmp[last] == ' ') {
        tmp[last] = '\0';
        --last;
    }
    while (tmp[0] == ' ') {
        ++tmp;
    }
    g_ndconfig->password = (char*)malloc((strlen(tmp) + 1) * sizeof(char));
    if (g_ndconfig->password == NULL) {
        fprintf(stderr, "Memory Error during config file reading !");
        exit(1);
    }
    strcpy(g_ndconfig->password, tmp);

    /* Split endpoint line to isolate data and format value */
    tmp = strtok(endpoint, ":");
    tmp = strtok(NULL, ":");
    last = strlen(tmp) - 1;
    while (tmp[last] == '\n' || tmp[last] == ' ') {
        tmp[last] = '\0';
        --last;
    }
    while (tmp[0] == ' ') {
        ++tmp;
    }
    g_ndconfig->endpoint = (char*)malloc((8 + strlen(tmp) + 1) * sizeof(char));
    if (g_ndconfig->endpoint == NULL) {
        fprintf(stderr, "Memory Error during config file reading !");
        exit(1);
    }
    strcpy(g_ndconfig->endpoint, "https://");
    strcat(g_ndconfig->endpoint, tmp);
}

void nd_config_destroy_global()
{
    free(g_ndconfig->username);
    free(g_ndconfig->password);
    free(g_ndconfig->endpoint);
    free(g_ndconfig);
}
