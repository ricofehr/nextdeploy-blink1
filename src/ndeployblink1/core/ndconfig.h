/**
 *  @file ndconfig.h
 *  @brief Config Header File
 *  @author Eric Fehr (ricofehr@nextdeploy.io, github: ricofehr)
 */

#ifndef NDEPLOYBLINK1_CORE_NDCONFIG_H_
#define NDEPLOYBLINK1_CORE_NDCONFIG_H_

#include <libconfig.h>

/**
 *  The nextdeploy credentials object
 *  - username: login value
 *  - password: password value
 *  - endpoint: the nextdeploy api endpoint
 */
struct ndconfig {
    char *username;
    char *password;
    char *endpoint;
};

/** The global user config object */
extern struct ndconfig *g_ndconfig;

/**
 *  Parse nextdeploy config file and init the global config object
 */
void nd_config_init_config();

/**
 *  Deallocate memory for the global object
 */
void nd_config_destroy_global();

#endif // NDEPLOYBLINK1_CORE_NDCONFIG_H_
