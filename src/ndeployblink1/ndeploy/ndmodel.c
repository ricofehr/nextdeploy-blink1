/**
 *  @file nd.c
 *  @brief NextDeploy Models Implementation
 *  @author Eric Fehr (ricofehr@nextdeploy.io, github: ricofehr)
 */

#include "ndeployblink1/ndeploy/ndmodel.h"

#include <stdio.h>
#include <stdlib.h>

struct vm_tree *vm_tree_insert(struct vm_tree *root, struct vm_tree *node)
{
    if (root == NULL) {
        root = node;
        if (root == NULL) {
            fprintf(stderr, "NOKKKKKKK!\n\n");
            return NULL;
        }
        root->left = root->right = NULL;
    } else {
        if (node->project_id <= root->project_id) {
            root->left = vm_tree_insert(root->left, node);
        } else {
            root->right = vm_tree_insert(root->right, node);
        }
    }

    return root;
}

void vm_tree_destroy(struct vm_tree *root)
{
    struct vm_probe_list *probe, *tmp;

    if (root) {
        vm_tree_destroy(root->left);
        vm_tree_destroy(root->right);

        probe = root->probes;
        while (probe != NULL) {
            tmp = probe;
            probe = probe->next;
            free(tmp);
        }
        root->probes = NULL;

        free(root->name);
        free(root);
        root = NULL;
    }
}

int vm_tree_check(struct vm_tree *root)
{
    int ret = ND_IDLE, retl = ND_IDLE, retr = ND_IDLE;
    struct vm_probe_list *probe;

    if (root) {
        retl = vm_tree_check(root->left);

        if (root->status == -1) {
            ret = ND_VM_ERROR;
        }

        if (root->status < -1) {
            ret = ND_WORKING;
        }

        probe = root->probes;
        while (probe != NULL) {
            if (probe->status == 0) {
                ret = ND_VM_DOWN;
                fprintf(stderr, "Vm alert on %s for techno %s\n", root->name, probe->techno->name);
            }
            probe = probe->next;
        }

        retr = vm_tree_check(root->right);
    }

    if (retl > ret) {
        ret = retl;
    }

    if (retr > ret) {
        ret = retr;
    }

    //vm_tree_destroy(root);

    return ret;
}
