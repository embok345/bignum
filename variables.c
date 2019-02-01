#include "calc.h"

/* Check if a string is a valid variable name, that is, the first character is a
 * lower case letter, and the remainder is alpha-numeric.
 * -----------------------------------------------------------------------------
 * const char* str - The string to check if it is a valid variable name.
 *
 * return int - 1 if the string is a valid variable name, 0 otherwise.
 */
int is_valid_var_name(const char* str) {
  return (isalpha(*str) && islower(*str) && isalnum_str(str+1));
}


/* Create a new leaf node, with the specified label and value.
 * -----------------------------------------------------------------------------
 * char* label - The name of the variable.
 * char* value - A string representing the value to be stored in the variable.
 * tree_node_t* node - The tree node in which to store the variable.
 */
void new_node(char* label,
              char* value,
              tree_node_t* node) {

  node->label = malloc(strlen(label));
  memcpy(node->label, label, strlen(label));
  node->label[strlen(label)] = '\0';
  /*bignum newNum = bn_conv_str2bn(value);
  //I hate this, but it seems the only way this will work
  node->value = malloc(sizeof(bignum));
  (node->value)->noBlocks = newNum.noBlocks;
  (node->value)->blocks = newNum.blocks;
  (node->value)->sign = newNum.sign;*/
  bn_init(&node->value);
  bn_conv_str2bn(value, node->value);
  node->left = NULL;
  node->right = NULL;

}

/* Try to retrieve a variable from the tree. Returns a null pointer if the
 * variable does not exist.
 * -----------------------------------------------------------------------------
 * char* label - The variable name to search for.
 * tree_node_t* root - The root of the tree to search from.
 *
 * return tree_node_t* - A pointer to the node in which the variable is stored,
 *     or a null pointer if it is not found
 */
tree_node_t* get_variable(char* label,
                          tree_node_t* root) {

  //If the label is not a valid variable name, don't bother looking for it.
  if(!is_valid_var_name(label)) return NULL;

  //Start at the root node.
  tree_node_t* currentNode = root;
  do {
    //If we have reached the bottom of the tree, the variable doesn't exist.
    if(currentNode == NULL) break;

    //Compare the searched for variable name with the label of the current node.
    int comp = strcmp(label, currentNode->label);

    //If the labels are the same, we have found the node.
    if(comp == 0) break;

    //Otherwise, we go down the left or right tree.
    if(comp<0)
      currentNode = currentNode->left;
    else
      currentNode = currentNode->right;

  } while(1);

  return currentNode;
}

/* Saves the variable in the tree.
 * -----------------------------------------------------------------------------
 * char* label - The name of the variable.
 * char* value - A string representing the value to be stored in the variable.
 * tree_node_t* root - The root of the tree in which to store the variable.
 */
tree_node_t* save_variable(char* label,
                           char* value,
                           tree_node_t** root) {

  //If the variable name is not valid, don't save it.
  if(!is_valid_var_name(label)) {
    printf("Invalid variable name: must be alphanumeric"
           " and begin with a lower case letter.\n");
    return NULL;
  }

  tree_node_t* nextNode;
  tree_node_t* currentNode = *root;

  //If the tree is empty, just insert the new variable in the root.
  if(!currentNode) {
    *root = malloc(sizeof(tree_node_t));
    new_node(label, value, *root);
    return *root;
  }

  do {
    //Compare the new variable name with the variable name in the current node.
    int comp = strcmp(label, currentNode->label);

    //If they are the same, we are overwriting the current variable.
    if(comp==0) {
      //Insert the new number.
      bn_conv_str2bn(value, currentNode->value);
      return currentNode;
    }
    //Otherwise we go to either the left subtree...
    else if(comp<0) {
      //If the left subtree is empty, we insert the new
      //variable as a new leaf there.
      if(!currentNode->left) {
        nextNode = malloc(sizeof(tree_node_t));
        new_node(label, value, nextNode);
        currentNode->left = nextNode;
        return nextNode;
      }
      //If the left subtree exists, we move down to its root
      //and continue.
      currentNode = currentNode->left;
      continue;
    }
    //...or the right subtree, precisely similar as above.
    else {
      if(!currentNode->right) {
        nextNode = malloc(sizeof(tree_node_t));
        new_node(label, value, nextNode);
        currentNode->right = nextNode;
        return nextNode;
      }
      currentNode = currentNode->right;
      continue;
    }
  } while(1);
  return NULL;
}

/* Prints out all of the currently saved variables, and their values, so long as
 * it is not too long, in order.
 * -----------------------------------------------------------------------------
 * tree_node_t* node - A pointer to the root node below which all vars are
 *     printed.
 */
void show_vars(tree_node_t* node) {
  if(node) {
    show_vars(node->left);
    printf("%s = ", node->label);
    if(bn_length(node->value) < 100)
      bn_prnt_dec(node->value);
    else
      printf("~256^%"PRIu32"\n", bn_length(node->value));
    show_vars(node->right);
  }
}

/* Find the first node including and below the given root node whose name has
 * the given prefix.
 * -----------------------------------------------------------------------------
 * const char* prefix - The prefix which the variable name needs to start with.
 * tree_node_t* root - The node at which to start searching for the given
 *     prefix.
 *
 * return tree_node_t* - A pointer to the first node below (or including) the
 *     root, whose label starts with the given prefix.
 */
tree_node_t* find_prefix_node(const char* prefix,
                              tree_node_t* root) {
  tree_node_t* currentNode = root;
  int len = strlen(prefix);
  while(currentNode) {
    int comp = strncmp(prefix, currentNode->label, len);
    if(comp == 0)
      return currentNode;
    if(comp<0)
      currentNode = currentNode->left;
    else
      currentNode = currentNode->right;
  }
  return currentNode;
}

/* Finds the total number of nodes in the tree below the given root node.
 * -----------------------------------------------------------------------------
 * const tree_node_t* root - The root of the tree.
 *
 * return int32_t - The number of nodes in the tree.
 */
int32_t tree_size(const tree_node_t* root) {
  if(!root) return 0;
  return 1+tree_size(root->left)+tree_size(root->right);
}

/* Gets all of the variables which have the given string as a prefix, which
 * occur below the given root node. Applied recursively, starting with the root
 * node, then applied to the left and right sub tree of any node which has the
 * given prefix, until we reach the end of the tree.
 * -----------------------------------------------------------------------------
 * const char* name - The prefix of the variable name to search for.
 * tree_node_t* root - The root of the tree, below which the variables are
 *     searched for.
 * char** completions - The string array in which the found variable names are
 *     stored. It is assumed enough space is malloced.
 * int32_t index - The position in the array in which to place the new found
 *     variables.
 *
 * return int32_t - The number of variables that were found with the prefix and
 *     added to the string array.
 */
int32_t get_var_completion(const char* name,
                           tree_node_t* root,
                           char** completions,
                           int32_t index) {
  //Find the first node which has the given prefix (may be the root).
  tree_node_t* prefixNode = find_prefix_node(name, root);
  //If there is no node with the prefix, no new variables are added.
  if(!prefixNode) return 0;

  //Add the current nodes label to the array.
  int addedVars = 1;
  completions[index] = strdup(prefixNode->label);

  //Apply recursively to the left and right sub trees.
  addedVars+=get_var_completion(name, prefixNode->left,
                                completions, index+addedVars);
  addedVars+=get_var_completion(name, prefixNode->right,
                                completions, index+addedVars);

  return addedVars;
}
