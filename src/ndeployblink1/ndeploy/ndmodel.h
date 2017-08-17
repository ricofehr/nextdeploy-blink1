/**
 *  @file nd.h
 *  @brief NextDeploy Models Header
 *  @author Eric Fehr (ricofehr@nextdeploy.io, github: ricofehr)
 */

#ifndef NDEPLOYBLINK1_NDEPLOY_NDMODEL_H_
#define NDEPLOYBLINK1_NDEPLOY_NDMODEL_H_

#define ND_IDLE 0
#define ND_WORKING 1
#define ND_VM_ERROR 2
#define ND_VM_DOWN 3

/**
 *  Vm model object
 *  - id: unique identifiant
 *  - project_id: NextDeploy project association
 *  - name: Vm name
 *  - status: Setup time, 3 ways\n
 *      positive => Vm installed and running\n
 *      negative => installation not finished\n
 *      -1 => Installation error\n
 *  - probes: list of probes for a vm
 *  - left: left side for the tree
 *  - right: right side for the tree
 */
struct vm_tree {
    int id;
    int project_id;
    char *name;
    int status;

    struct vm_probe_list *probes;

    struct vm_tree *left;
    struct vm_tree *right;
};

/**
 *  Probe model object for vm list
 *  - status: 0 => service down, 1 => service up
 *  - techno: techno object managed by the probe
 *  - next: next item in the probes list
 */
struct vm_probe_list {
    int status;

    struct techno *techno;
    struct vm_probe_list *next;
};

/**
 *  Probe model object
 *  - id: unique identifiant
 *  - status: 0 => service down, 1 => service up
 *  - vm_id: id of the vm object owning the probe
 *  - techno_id: id of the techno object managed by the probe
 */
struct probe {
    int id;
    int status;
    int vm_id;
    int techno_id;
};


/**
 *  Techno model object
 *  - id: unique identifiant
 *  - name: techno name
 */
struct techno {
    int id;
    char *name;
};

/**
 *  Insert a vm object into the binary search tree (sorted by project_id field)
 *  @param root: the root of the tree
 *  @param node: the new object to insert
 *  @return the new root of the tree
 */
struct vm_tree *vm_tree_insert(struct vm_tree *root, struct vm_tree *node);

/**
 *  Parse the tree and check a bad status on a vm object or on the probes lists
 *  @param root: root of the Binary Search Tree of vms
 *  @return a code error if issues or 0 if all is up
 */
int vm_tree_check(struct vm_tree *root);

/**
 *  Destroy a binary search vms tree
 *  @param root is the root of the tree
 */
void vm_tree_destroy(struct vm_tree *root);

#endif // NDEPLOYBLINK1_NDEPLOY_NDMODEL_H_
