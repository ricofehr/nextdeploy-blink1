/**
 *  @file ndapiclient.c
 *  @brief NextDeploy Api Connector Implementation
 *  @author Eric Fehr (ricofehr@nextdeploy.io, github: ricofehr)
 */

#include "ndeployblink1/ndeploy/ndapiclient.h"

#include <curl/curl.h>
#include <string.h>

#include "ndeployblink1/ndeploy/ndmodel.h"


/* Api global vars */
static const char *s_token;
static const int s_debug = 0;

/**
 *  Write the response buffer into response_json object
 *  @param ptr the memory location of the data stored by curl
 *  @param size of one element to write
 *  @param nmemb count of elements to write
 *  @param stream the current response object for output the datas
 *  @return bytes reads
 */
static size_t write_resp(void *ptr, size_t size, size_t nmemb, void *stream)
{
    struct response_json *resp = (struct response_json *)stream;

    if (resp->pos + size * nmemb >= BUFFER_SIZE - 1) {
        return 0;
    }

    memcpy(resp->data + resp->pos, ptr, size * nmemb);
    resp->pos += size * nmemb;

    return size * nmemb;
}

/**
 *  Manage an api request for authentification
 *  @param apiurl the api endpoint targeted
 *  @param email the login value
 *  @param password the password value
 *  @return the response text or NULL if error occurs
 */
static char *http_request_auth(char *apiurl, char *email, char *password)
{
    CURL *curl = NULL;
    CURLcode status;
    struct curl_slist *headers = NULL;
    char *data = NULL;
    char post[128];
    char ndurl[192];
    long code;

    /* Allocate Memory object for response */
    data = (char*)malloc(BUFFER_SIZE * sizeof(char));
    if (data == NULL) {
        fprintf(stderr, "Memory Error\n");
        return NULL;
    }

    struct response_json resp = {
        .data = data,
        .pos = 0
    };

    /* prepare post request and url */
    sprintf(post, "{\"user\":{\"email\":\"%s\", \"password\":\"%s\"}}", email, password);
    sprintf(ndurl, "%s%s", apiurl, URL_SIGNIN);

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();

    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, ndurl);

        headers = curl_slist_append(headers, "User-Agent: NextDeploy-Blink");
        headers = curl_slist_append(headers, "Accept: application/json");
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        /* Specify email and password */
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_resp);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp);

        /* Perform the request, res will get the return code */
        status = curl_easy_perform(curl);
        /* Check for errors */
        if(status != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(status));
            free(data);
            return NULL;
        } else {
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
            if (code != 200) {
                fprintf(stderr, "Wrong status code from API");
                free(data);
                return NULL;
            }
        }

        /* curl cleanup */
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }

    curl_global_cleanup();
    data[resp.pos] = '\0';

    if (s_debug > 0) {
        fprintf(stderr, "%s\n", data);
    }

    return data;
}

/**
 *  Manage a GET api request
 *  @param apiurl the api endpoint targeted
 *  @param uripath the request path targetted
 *  @return the response text or NULL if error occurs
 */
static char *http_request_get(char *apiurl, char *urlpath)
{
    CURL *curl = NULL;
    CURLcode status;
    struct curl_slist *headers = NULL;
    char *data = NULL;
    char post[128];
    char auth[128];
    char ndurl[192];
    long code;

    /* Allocate Memory object for response */
    data = (char*)malloc(BUFFER_SIZE * sizeof(char));
    if (!data) {
        fprintf(stderr, "Memory Error\n");
        exit(1);
    }

    struct response_json resp = {
        .data = data,
        .pos = 0
    };

    /* prepare post request and url */
    sprintf(ndurl, "%s%s", apiurl, urlpath);
    sprintf(auth, "Authorization: Token token=%s", s_token);

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();

    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, ndurl);

        headers = curl_slist_append(headers, "User-Agent: NextDeploy-Blink");
        headers = curl_slist_append(headers, "Accept: application/json");
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, auth);

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_resp);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp);

        /* Perform the request, res will get the return code */
        status = curl_easy_perform(curl);
        /* Check for errors */
        if(status != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(status));
            free(data);
            return NULL;
        } else {
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
            if (code != 200) {
                fprintf(stderr, "Wrong status code from API");
                free(data);
                return NULL;
            }
        }

        /* always cleanup */
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }

    curl_global_cleanup();
    data[resp.pos] = '\0';

    if (s_debug > 0) {
        fprintf(stderr, "%s\n", data);
    }
    
    return data;
}

/**
 *  Return all probes object from NextDeploy
 *  @param apiurl the api endpoint targeted
 *  @param probes_len is the count of probes, to be updated in this function
 *  @return the array of probes
 */
static struct probe *get_probes(char *apiurl, int *probes_len)
{
    json_t *probes_root, *probes, *id, *vm_id, *techno_id, *status, *row;
    char *json_resp;
    json_error_t error;
    struct probe *ret;
    int i;

    json_resp = http_request_get(apiurl, URL_PROBES);
    if (json_resp == NULL) {
        fprintf(stderr, "Json resp is NULL\n");
        return NULL;
    }

    probes_root = json_loads(json_resp, 0, &error);
    free(json_resp);
    probes = json_object_get(probes_root, "supervises");

    if (!probes) {
        fprintf(stderr, "Json root is NULL\n");
        json_decref(probes_root);
        return NULL;
    }

    if (!json_is_array(probes)) {
        fprintf(stderr, "Json root is not an array\n");
        json_decref(probes_root);
        return NULL;
    }

    *probes_len = json_array_size(probes);
    ret = malloc(json_array_size(probes) * sizeof(struct probe));
    if (ret == NULL) {
        fprintf(stderr, "Malloc on probes failed");
        json_decref(probes_root);
        free(ret);
        return NULL;
    }

    for (i = 0; i < json_array_size(probes); i++) {
        row = json_array_get(probes, i);

        status = json_object_get(row, "status");
        if (!json_is_boolean(status)) {
            fprintf(stderr, "status is not a boolean\n");
            json_decref(probes_root);
            free(ret);
            return NULL;
        }

        id = json_object_get(row, "id");
        if (!json_is_integer(id)) {
            fprintf(stderr, "id is not an integer\n");
            json_decref(probes_root);
            free(ret);
            return NULL;
        }

        vm_id = json_object_get(row, "vm");
        if (!json_is_integer(vm_id)) {
            fprintf(stderr, "vm_id is not an integer\n");
            json_decref(probes_root);
            free(ret);
            return NULL;
        }

        techno_id = json_object_get(row, "techno");
        if (!json_is_integer(techno_id)) {
            fprintf(stderr, "techno_id is not an integer\n");
            json_decref(probes_root);
            free(ret);
            return NULL;
        }

        ret[i].id = (int)json_integer_value(id);
        ret[i].status = (int)json_boolean_value(status);
        ret[i].vm_id = (int)json_integer_value(vm_id);
        ret[i].techno_id = (int)json_integer_value(techno_id);
    }

    json_decref(probes_root);
    return ret;
}

/**
 *  Return all technos object from NextDeploy
 *  @param apiurl the api endpoint targeted
 *  @param technos_len is the count of technos, to be updated in this function
 *  @return the array of technos
 */
static struct techno *get_technos(char *apiurl, int *technos_len)
{
    json_t *technos_root, *technos, *id, *name, *row;
    char *json_resp;
    json_error_t error;
    struct techno *ret;
    int i, j;

    json_resp = http_request_get(apiurl, URL_TECHNOS);
    if (json_resp == NULL) {
        fprintf(stderr, "Json resp is NULL\n");
        return NULL;
    }

    technos_root = json_loads(json_resp, 0, &error);
    free(json_resp);
    technos = json_object_get(technos_root, "technos");

    if (!technos) {
        fprintf(stderr, "Json root is NULL\n");
        json_decref(technos_root);
        return NULL;
    }

    if (!json_is_array(technos)) {
        fprintf(stderr, "Json root is not an array\n");
        json_decref(technos_root);
        return NULL;
    }

    *technos_len = json_array_size(technos);
    ret = malloc(json_array_size(technos) * sizeof(struct techno));
    if (ret == NULL) {
        fprintf(stderr, "Malloc on technos failed");
        json_decref(technos_root);
        free(ret);
        return NULL;
    }

    for (i = 0; i < json_array_size(technos); i++) {
        row = json_array_get(technos, i);

        id = json_object_get(row, "id");
        if (!json_is_integer(id)) {
            fprintf(stderr, "id is not an integer\n");
            json_decref(technos_root);
            for (j = 0; j < i; j++) {
                free(ret[j].name);
            }
            free(ret);
            return NULL;
        }

        name = json_object_get(row, "name");
        if (!json_is_string(name)) {
            fprintf(stderr, "name is not a string\n");
            json_decref(technos_root);
            for (j = 0; j < i; j++) {
                free(ret[j].name);
            }
            free(ret);
            return NULL;
        }

        ret[i].id = (int)json_integer_value(id);
        ret[i].name = malloc(strlen(json_string_value(name)) + 1);
        if (ret[i].name == NULL) {
            fprintf(stderr, "name allocate failed\n");
            json_decref(technos_root);
            for (j = 0; j < i; j++) {
                free(ret[j].name);
            }
            free(ret);
            return NULL;
        }
        strcpy(ret[i].name, json_string_value(name));
    }

    json_decref(technos_root);
    return ret;
}

/**
 *  Return a list of probes binded with a vm
 *  @param probes the json object which lists the vm probes id
 *  @param all_probes all probes object from NextDeploy (request made before)
 *  @param all_technos all technos object from NextDeploy (request made before)
 *  @param probes_len size of all_probes array
 *  @param technos_len size of all_technos array
 *  @return the list of probes for the targetted vm
 */
static struct vm_probe_list *init_vm_probes(json_t *probes, struct probe *all_probes, struct techno *all_technos, int probes_len, int technos_len)
{
    json_t *id, *vm_id, *techno_id, *status, *row;
    struct vm_probe_list *ret = NULL, *tmp = NULL;
    struct techno techno_join;
    struct probe probe_join;
    int i, j, k;

    for (i = 0; i < json_array_size(probes); i++) {
        tmp = malloc(sizeof(struct vm_probe_list));
        if (tmp == NULL) {
            fprintf(stderr, "Memory issue on probes list\n");
            while(ret) {
                tmp = ret;
                ret = ret->next;
                free(tmp);
            }
            return NULL;
        }

        id = json_array_get(probes, i);
        if (!json_is_integer(id)) {
            fprintf(stderr, "id is not an integer\n");
            /* Deallocate the list */
            free(tmp);
            while(ret) {
                tmp = ret;
                ret = ret->next;
                free(tmp);
            }
            return NULL;
        }

        for (j = 0; j < probes_len; j++) {
            if (all_probes[j].id == (int)json_integer_value(id)) {
                tmp->status = all_probes[j].status;
                for (k = 0; k < technos_len; k++) {
                    if (all_technos[k].id == all_probes[j].techno_id) {
                        tmp->techno = &all_technos[k];
                        break;
                    }
                }
                break;
            }
        }

        tmp->next = ret;
        ret = tmp;
    }

    return ret;
}

int nd_apiclient_auth(char *apiurl, char *email, char *password)
{
    json_t *root, *token;
    json_error_t error;
    char *json_resp = NULL;

    json_resp = http_request_auth(apiurl, email, password);
    if (json_resp == NULL) {
        fprintf(stderr, "Resp is NULL\n");
        return JSON_ERROR;
    }

    root = json_loads(json_resp, 0, &error);
    free(json_resp);

    if (!root) {
        fprintf(stderr, "root is NULL\n");
        return JSON_ERROR;
    }

    token = json_object_get(root, "token");
    if (!json_is_string(token)) {
        fprintf(stderr, "token is not a string\n");
        return JSON_ERROR;
    }

    s_token = json_string_value(token);

    if (s_debug > 0) {
        fprintf(stderr, "%s\n", s_token);
    }
    
    return SUCCESS;
}

int nd_apiclient_check_status(char *apiurl)
{
    json_t *vms_root, *vms, *id, *name, *status, *row, *project_id, *probes;
    json_error_t error;
    char *json_resp = NULL;
    int i, j, ret;
    int probes_len, technos_len;
    struct vm_tree *root = NULL, *node = NULL;
    struct probe *all_probes = NULL;
    struct techno *all_technos = NULL;

    all_probes = get_probes(apiurl, &probes_len);
    all_technos = get_technos(apiurl, &technos_len);

    json_resp = http_request_get(apiurl, URL_VMS);
    if (json_resp == NULL) {
        fprintf(stderr, "Json resp is NULL\n");
        free(all_probes);
        free(all_technos);
        return JSON_ERROR;
    }

    vms_root = json_loads(json_resp, 0, &error);
    free(json_resp);
    vms = json_object_get(vms_root, "vms");

    if (!vms) {
        fprintf(stderr, "Json root is NULL\n");
        json_decref(vms_root);
        free(all_probes);
        for (j = 0; j < technos_len; j++) {
            free(all_technos[j].name);
        }
        free(all_technos);
        return JSON_ERROR;
    }

    if (!json_is_array(vms)) {
        fprintf(stderr, "Json root is not an array\n");
        json_decref(vms_root);
        free(all_probes);
        for (j = 0; j < technos_len; j++) {
            free(all_technos[j].name);
        }
        free(all_technos);
        return JSON_ERROR;
    }

    for (i = 0; i < json_array_size(vms); i++) {
        row = json_array_get(vms, i);

        id = json_object_get(row, "id");
        if (!json_is_integer(id)) {
            fprintf(stderr, "id is not a string\n");
            json_decref(vms_root);
            free(all_probes);
            for (j = 0; j < technos_len; j++) {
                free(all_technos[j].name);
            }
            free(all_technos);
            vm_tree_destroy(root);
            return JSON_ERROR;
        }

        status = json_object_get(row, "status");
        if (!json_is_integer(status)) {
            fprintf(stderr, "status is not an integer\n");
            json_decref(vms_root);
            free(all_probes);
            for (j = 0; j < technos_len; j++) {
                free(all_technos[j].name);
            }
            free(all_technos);
            vm_tree_destroy(root);
            return JSON_ERROR;
        }

        project_id = json_object_get(row, "project");
        if (!json_is_integer(project_id)) {
            fprintf(stderr, "project_id is not an integer %s\n", json_string_value(project_id));
            json_decref(vms_root);
            free(all_probes);
            for (j = 0; j < technos_len; j++) {
                free(all_technos[j].name);
            }
            free(all_technos);
            vm_tree_destroy(root);
            return JSON_ERROR;
        }

        name = json_object_get(row, "name");
        if (!json_is_string(name)) {
            fprintf(stderr, "name is not a string\n");
            json_decref(vms_root);
            free(all_probes);
            for (j = 0; j < technos_len; j++) {
                free(all_technos[j].name);
            }
            free(all_technos);
            vm_tree_destroy(root);
            return JSON_ERROR;
        }

        if (s_debug > 0) {
            fprintf(stderr, "%d::%d\n",
                    (int)json_integer_value(id), (int)json_integer_value(status));
        }

        node = (struct vm_tree*)malloc(sizeof(struct vm_tree));
        if (node == NULL) {
            fprintf(stderr, "Memory issue on vms tree\n");
            json_decref(vms_root);
            free(all_probes);
            for (j = 0; j < technos_len; j++) {
                free(all_technos[j].name);
            }
            free(all_technos);
            vm_tree_destroy(root);
            return JSON_ERROR;
        }

        node->id = (int)json_integer_value(id);

        node->name = malloc(strlen(json_string_value(name)) + 1);
        if (node == NULL) {
            fprintf(stderr, "Memory issue on vms tree\n");
            free(node);
            json_decref(vms_root);
            free(all_probes);
            for (j = 0; j < technos_len; j++) {
                free(all_technos[j].name);
            }
            free(all_technos);
            vm_tree_destroy(root);
            return JSON_ERROR;
        }
        strcpy(node->name, json_string_value(name));

        node->status = (int)json_integer_value(status);
        node->project_id = (int)json_integer_value(project_id);

        node->probes = NULL;
        if (node->status > 0) {
            node->probes = init_vm_probes(json_object_get(row, "supervises"), all_probes, all_technos, probes_len, technos_len);
        }

        root = vm_tree_insert(root, node);
    }

    ret = vm_tree_check(root);

    /* Clean up before return */
    json_decref(vms_root);
    free(all_probes);
    for (j = 0; j < technos_len; j++) {
        free(all_technos[j].name);
    }
    free(all_technos);
    vm_tree_destroy(root);

    return ret;
}
