/**
 *  @file ndapiclient.h
 *  @brief NextDeploy Api Connector Header
 *  @author Eric Fehr (ricofehr@nextdeploy.io, github: ricofehr)
 */

#ifndef NDEPLOYBLINK1_NDEPLOY_NDAPICLIENT_H_
#define NDEPLOYBLINK1_NDEPLOY_NDAPICLIENT_H_

#include <jansson.h>

#define BUFFER_SIZE (256 * 1024)
#define URL_SIGNIN  "/api/v1/users/sign_in"
#define URL_VMS  "/api/v1/vms"
#define URL_PROBES  "/api/v1/supervises"
#define URL_TECHNOS  "/api/v1/technos"
#define URL_SIZE 256

#define SUCCESS 0
#define JSON_ERROR 4

/**
 *  Record response text from the api
 *  - data: the response text
 *  - pos: the pointer position
 */
struct response_json {
    char *data;
    int pos;
};

/**
 *  Perform an authentification with NextDeploy and store the token for other requests later
 *  @param apiurl the api endpoint targeted
 *  @param email the login value
 *  @param password the password value
 *  @return 0 if success, >0 if error occurs
 */
int nd_apiclient_auth(char *apiurl, char *email, char *password);

/**
 *  Check status of all vms and all of their probes
 *  @param apiurl the api endpoint targeted
 *  @return 0 if all probes are up, or >1 if one or more are down
 */
int nd_apiclient_check_status(char *apiurl);

#endif // NDEPLOYBLINK1_NDEPLOY_NDAPICLIENT_H_
