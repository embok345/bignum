#include "src/bignum.h"
#include <readline/readline.h>
#include <readline/history.h>

int min(int a, int b) {
  return a<b ? a : b;
}
int max(int a, int b) {
  return a>b ? a : b;
}

typedef struct int_list_t int_list_t;

struct int_list_t {
  int val;
  int_list_t* next;
};

int_list_t* push(int i, int_list_t* stack) {
  int_list_t *newElement = malloc(sizeof(int_list_t));
  newElement->val = i;
  newElement->next = stack;
  return newElement;
}

int pop(int_list_t** stack) {
  if(stack) {
    int retVal = (*stack)->val;
    *stack = (*stack)->next;
    return retVal;
  }
  return 0;
}

typedef struct tree_node_t tree_node_t;

/* Binary sorted tree data structure for storing variables.
 * ---------------------------------------------------------
 * char* label - The name of the variable being stored. The
 *     tree is sorted lexographically by this.
 * bignum* value - The bignum (a painter to a bignum) stored
 *     in the variable.
 * tree_node_t* left - A pointer to the left subtree of this
 *     node (null if it doesn't have one).
 * tree_node_t* right - As above, for the right subtree.
 */
struct tree_node_t {
  char* label;
  bignum* value;
  tree_node_t* left;
  tree_node_t* right;
};

/* Remove all whitespace in a string (spaces, tabs, newline,
 * etc.)
 * ---------------------------------------------------------
 * const char* oldString - The string to remove whitespace
 *     from.
 * char* newString - The string after all whitespace has
 *     been removed. These can be the same string.
 *
 * return int - The number of whitespace characters removed.
 */
int remove_whitespace(const char* oldString,
                     char* newString) {
  int charsRemoved = 0;
  //Iterate over the characters in the string till we reach
  //the end.
  while(*oldString != '\0') {
    //If the current character is not a whitespace, insert
    //it into the new string
    if(!isspace(*oldString)) {
      *newString = *oldString;
      newString++;
    }
    //otherwise, we have removed a character
    else charsRemoved++;

    //Then move on to the next character in the string
    oldString++;
  }
  //Finally null terminate our new string
  *newString = '\0';
  return charsRemoved;
}

/* Check if a string contains only alphanumeric characters
 * (surely this is a built in function?)
 * ---------------------------------------------------------
 * const char* str - The string to check if it is
 *     alphanumeric.
 *
 * return int - 1 if the string is alphanumeric, 0 otherwise
 */
int isalnum_str(const char* str) {
  while(*str!='\0') {
    if(!isalnum(*str)) return 0;
    str++;
  }
  return 1;
}

/* Check if a string contains only numerals (as above?)
 * ---------------------------------------------------------
 * const char* str - The string to check if it is digital.
 *
 * return int - 1 if the string is digital, 0 otherwise.
 */
int isdigit_str(const char* str) {
  while(*str!='\0') {
    if(!isdigit(*str)) return 0;
    str++;
  }
  return 1;
}

/* Check if a string is a valid variable name, that is, the
 * first character is a lower case letter, and the remainder
 * is alpha-numeric.
 * ---------------------------------------------------------
 * const char* str - The string to check if it is a valid
 *     variable name.
 *
 * return int - 1 if the string is a valid variable name,
 *     0 otherwise.
 */
int is_valid_var_name(const char* str) {
  return (isalpha(*str) && islower(*str) &&
          isalnum_str(str+1));
}

/* The root node for the variable tree */
tree_node_t* rootPointer = NULL;

int32_t noVars = 0;

/* Create a new leaf node, with the specified label and
 * value.
 * ---------------------------------------------------------
 * char* label - The name of the variable.
 * char* value - A string representing the value to be
 *     stored in the variable.
 * tree_node_t* node - The tree node in which to store the
 *     variable.
 */
void new_node(char* label,
              char* value,
              tree_node_t* node) {

  node->label = malloc(strlen(label));
  memcpy(node->label, label, strlen(label));
  node->label[strlen(label)] = '\0';
  bignum newNum = bn_conv_str2bn(value);
  //I hate this, but it seems the only way this will work
  node->value = malloc(sizeof(bignum));
  (node->value)->noBlocks = newNum.noBlocks;
  (node->value)->blocks = newNum.blocks;
  (node->value)->sign = newNum.sign;
  node->left = NULL;
  node->right = NULL;
}

/* Try to retrieve a variable from the tree. Returns a null
 * pointer if the variable does not exist.
 * ---------------------------------------------------------
 * char* label - The variable name to search for
 *
 * return tree_node_t* - A pointer to the node in which the
 * variable is stored, or a null pointer if it is not found
 */
tree_node_t* get_variable(char* label) {
  //If the label is not a valid variable name, don't bother
  //looking for it.
  if(!is_valid_var_name(label)) return NULL;
  //Start at the root node.
  tree_node_t* currentNode = rootPointer;
  do {
    //If we have reached the bottom of the tree, the
    //variable doesn't exist.
    if(currentNode == NULL)
      break;
    //Compare the searched for variable name with the label
    //of the current node.
    int comp = strcmp(label, currentNode->label);
    //If the labels are the same, we have found the node.
    if(comp == 0)
      break;
    //Otherwise, we go down the left or right tree.
    if(comp<0)
      currentNode = currentNode->left;
    else
      currentNode = currentNode->right;

  } while(1);
  return currentNode;
}

/* Saves the variable in the tree.
 * ---------------------------------------------------------
 * char* label - The name of the variable.
 * char* value - A string representing the value to be
 *     stored in the variable.
 */
void save_variable(char* label, char* value) {

  //If the variable name is not valid, don't dave it.
  if(!is_valid_var_name(label)) {
    printf("Invalid variable name: must be alphanumeric"
           " and begin with a lower case letter.\n");
    return;
  }

  //If the tree is empty, just insert the new variable in
  //the root.
  if(rootPointer == NULL) {
    rootPointer = malloc(sizeof(tree_node_t));
    new_node(label, value, rootPointer);
    noVars++;
    return;
  }

  //Start at the root of the tree.
  tree_node_t* nextNode;
  tree_node_t* currentNode = rootPointer;
  do {
    //Compare the new variable name with the variable name
    //in the current node.
    int comp = strcmp(label, currentNode->label);

    //If they are the same, we are overwriting the current
    //variable.
    if(comp == 0) {
      //Remove the current number...
      bn_destroy(currentNode->value);
      //and insert the new number.
      bignum newNum = bn_conv_str2bn(value);
      //Again, this seems horrible.
      (currentNode->value)->noBlocks = newNum.noBlocks;
      (currentNode->value)->blocks = newNum.blocks;
      (currentNode->value)->sign = newNum.sign;
      return;
    }
    //Otherwise we go to either the left subtree...
    else if(comp<0) {
      noVars++;
      //If the left subtree is empty, we insert the new
      //variable as a new leaf there.
      if(currentNode->left == NULL) {
        nextNode = malloc(sizeof(tree_node_t));
        new_node(label, value, nextNode);
        currentNode->left = nextNode;
        return;
      }
      //If the left subtree exists, we move down to its root
      //and continue.
      currentNode = currentNode->left;
      continue;
    }
    //...or the right subtree, precisely similar as above.
    else {
      noVars++;
      if(currentNode->right == NULL) {
        nextNode = malloc(sizeof(tree_node_t));
        new_node(label, value, nextNode);
        currentNode->right = nextNode;
        return;
      }
      currentNode = currentNode->right;
      continue;
    }
  } while(1);
}

/* Prints out all of the currently saved variables, and
 * their values, so long as it is not too long, in order.
 * ---------------------------------------------------------
 * tree_node_t* node - A pointer to the root node below
 *     which all vars are printed
 */
void show_vars(tree_node_t* node) {
  if(node) {
    show_vars(node->left);
    printf("%s = ", node->label);
    if((node->value)->noBlocks < 100)
      bn_prnt_dec(*(node->value));
    else
      printf("~256^%"PRIu32"\n", (node->value)->noBlocks);
    show_vars(node->right);
  }
}

/* Find the first node whose name has the given prefix.
 * ---------------------------------------------------------
 * const char* prefix - The prefix which the variable name
 *     needs to start with.
 * tree_node_t* root - The node at which to start searching
 *     for the given prefix.
 *
 * return tree_node_t* - A pointer to the first node below
 *     (or including) the root, whose label starts with the
 *     given prefix.
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

/* Finds the total number of nodes in the tree with the
 * given root.
 * ---------------------------------------------------------
 * const tree_node_t* root - The root of the tree.
 *
 * return int32_t - The number of nodes in the tree.
 */
int32_t tree_size(const tree_node_t* root) {
  if(!root) return 0;
  return 1+tree_size(root->left)+tree_size(root->right);
}

/* Gets the longest prefix of all of the strings in the
 * array all.
 * ---------------------------------------------------------
 * char** strings - The array of strings from which to
 *     extract the longest prefix.
 * int32_t noStrings - The number of strings in the array.
 *
 * return char* - The longest prefix in common of all of the
 *     strings.
 */
char* find_longest_prefix(char** strings,
                          int32_t noStrings) {
  //The longest prefix can be at most as long as any of the
  //strings.
  char* prefix = strdup(strings[0]);

  for(int i=1; i<noStrings; i++) {
    //If the new string is shorter than the current prefix,
    //the longest prefix can be at most as long as the new
    //string.
    if(strlen(strings[i]) < strlen(prefix))
      prefix[strlen(strings[i])] = '\0';

    //Iterate through all of the characters in the prefix
    //string (which is now no longer than the current string
    for(int j=0; j<strlen(prefix); j++) {
      //If we encounter a character which is different,
      //we truncate the prefix string.
      if(prefix[j]!=strings[i][j]) {
        prefix[j] = '\0';
        //If they differ in the first character, there is no
        //common prefix.
        if(strlen(prefix) == 0) return prefix;
        break;
      }
    }
  }
  return prefix;
}

/* Gets all of the variables which have the given string as
 * a prefix, which occur below the given root node. Applied
 * recursively, starting with the root node, then applied to
 * the left and right sub tree of any node which has the
 * given prefix, until we reach the end of the tree.
 * ---------------------------------------------------------
 * const char* name - The prefix of the variable name to
 *     search for.
 * tree_node_t* root - The root of the tree, below which the
 *     variables are searched for.
 * char** completions - The string array in which the found
 *     variable names are stored.
 * int32_t index - The position in the array in which to
 *     place the new found variables.
 *
 * return int32_t - The number of variables that were found
 *     with the prefix and added to the string array.
 */
int32_t get_var_completion(const char* name,
                           tree_node_t* root,
                           char** completions,
                           int32_t index) {
  //Firstly find the first node below the root which has
  //the given prefix (may be the root).
  tree_node_t* prefixNode = find_prefix_node(name, root);
  //If there is no node with the prefix, we won't add any
  //more strings to the array.
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

/* Create an array of strings for use with tab completion
 * from readline library. The string array is one of three
 * types: NULL, if there are no possible completions;
 *        A single command, followed by NULL, if there is
 *          only one possible completion; or,
 *        The longest common prefix of all of the possible
 *          completions, followed by all of the possible
 *          completions, followed by NULL.
 * ---------------------------------------------------------
 * const char* command - The command to try and complete.
 * int start - The start index of the command to complete
 *     in the input string.
 * int end - The end index of the command to complete in the
 *     the input string.
 *
 * return char** - An array of strings of the possible
 *     completions of the given command.
 */
char** command_completion(const char* command,
                          int start,
                          int end) {
  //Tell readline that we are done with command completion.
  rl_attempted_completion_over = 1;

  //If the command to complete is a valid variable name,
  //try to complete it as a variable name.
  if(is_valid_var_name(command)) {
    //Find the first variable node in the tree which has
    //the given command as prefix.
    tree_node_t* prefixRoot =
        find_prefix_node(command, rootPointer);
    //If there are no such nodes, there are no possible
    //completions.
    if(!prefixRoot)
      return NULL;

    //Otherwise, there are some possibilities, so get them.
    char** completions = malloc(
              sizeof(char*) * (tree_size(prefixRoot)+2));
    int32_t addedVars = get_var_completion(
                    command, prefixRoot, completions, 1);

    //If there was precisely one possibility, return it.
    if(addedVars == 1) {
      completions[0] = strdup(completions[1]);
      completions[1] = NULL;
    }
    //Otherwise, find the longest common prefix.
    else {
      completions[addedVars+1] = NULL;
      completions[0] = find_longest_prefix(completions+1,
                                             addedVars);
    }
    return completions;
  }
}

bignum do_operation(bignum left,
                    bignum right,
                    char operation) {
  switch(operation) {
    case '+': return bn_add(left, right);
    case '-': return bn_subtract(left, right);
    case '*': return bn_mul(left, right);
    default: printf("Unkown operation\n");
             bignum ret;
             bn_copy(&ret, ZERO);
             return ret;
  }
}


bignum expand_expression(char* expression, char operation) {
  char op[2];
  op[0] = operation;
  op[1] = '\0';
  bignum result;

  if(!strchr(expression, operation)) {
    bn_copy(&result, ZERO);
    return result;
  }

  size_t lhsLength = strcspn(expression, op);
  size_t rhsLength = strlen(expression) - lhsLength-1;
  if(lhsLength <= 0 || rhsLength <= 0) {
    printf("Malformed expression\n");
    bn_copy(&result, ZERO);
    return result;
  }
  char* lhs = malloc(lhsLength+1);
  char* rhs = malloc(rhsLength+1);
  strncpy(lhs, expression, lhsLength);
  lhs[lhsLength] = '\0';
  strncpy(rhs, expression+lhsLength+1, rhsLength);
  rhs[rhsLength] = '\0';

  tree_node_t *right, *left;

  //If the left and right are just numbers, or variables,
  //we can do the operation.
  if(isdigit_str(lhs) && isdigit_str(rhs)) {
    bignum leftNum = bn_conv_str2bn(lhs);
    bignum rightNum = bn_conv_str2bn(rhs);
    result = do_operation(leftNum, rightNum, operation);
    bn_destroy(&leftNum);
    bn_destroy(&rightNum);
    return result;
  }
  if(isdigit_str(lhs) && is_valid_var_name(rhs)) {
    if(right = get_variable(rhs)) {
      bignum leftNum = bn_conv_str2bn(lhs);
      result = do_operation(leftNum, *(right->value),
                                                 operation);
      bn_destroy(&leftNum);
      return result;
    }
    printf("Variable doesn't exist: %s\n", rhs);
    bn_copy(&result, ZERO);
    return result;
  }
  if(is_valid_var_name(lhs) && is_valid_var_name(rhs)) {
    if((left = get_variable(lhs)) &&
       (right = get_variable(rhs))) {
      result = do_operation(*(left->value), *(right->value),
                                                 operation);
      return result;
    }
    if(!left) {
      printf("Variable doesn't exist: %s\n", lhs);
    }
    if(!right) {
      printf("Variable doesn't exist: %s\n", rhs);
    }
    bn_copy(&result, ZERO);
    return result;
  }
  if(is_valid_var_name(lhs) && isdigit_str(rhs)) {
    if(left = get_variable(lhs)) {
      bignum rightNum = bn_conv_str2bn(rhs);
      result = do_operation(*(left->value), rightNum,
                                                 operation);
      bn_destroy(&rightNum);
      return result;
    }
    printf("Variable doesn't exist: %s\n", lhs);
    bn_copy(&result, ZERO);
    return result;
  }

  //Otherwise, we need to further expand either the left or
  //right hand side.
}

void evaluate_expression(char* expression) {
  char* str = expression;

  if(strchr(str, '(') || strchr(str, ')')) {

    int open = 0, numPairs = 0;
    for(int i=0; i<strlen(expression); i++) {
      if(str[i] == '(') {
        open++;
        numPairs++;
      }
      if(str[i] == ')') open--;
      if(open<0)
        break;
    }
    if(open!=0) {
      printf("Parentheses mismatch\n");
      return;
    }

    char** expressions = malloc(numPairs);
    int_list_t* stack = NULL;
    for(int i=0; i<strlen(expression); i++) {
      if(str[i] == '(') stack = push(i, stack);
      if(str[i] == ')') {
        int j = pop(&stack);
        char* newStr = malloc(i-j);
        strncpy(newStr, expression+j+1, i-j-1);
        newStr[i-j-1] = '\0';
        printf("%s\n", newStr);
      }
    }
    return;
  }

  bignum result1 = expand_expression(str, '^');
  bignum result2 = expand_expression(str, '*');
  bignum result3 = expand_expression(str, '/');
  bignum result4 = expand_expression(str, '+');
  bignum result5 = expand_expression(str, '-');
  if(!bn_equals(result1, ZERO))
    bn_prnt_dec(result1);
  if(!bn_equals(result2, ZERO))
    bn_prnt_dec(result2);
  if(!bn_equals(result3, ZERO))
    bn_prnt_dec(result3);
  if(!bn_equals(result4, ZERO))
    bn_prnt_dec(result4);
  if(!bn_equals(result5, ZERO))
    bn_prnt_dec(result5);
}

int main(int argc, char *argv[]){
  srand(time(NULL));
  rl_attempted_completion_function = command_completion;

  while(1) {
    //Continuously prompt for user input.
    char* input;
    input = readline("> ");

    //If the entry was empty, move on.
    if(!input) {
      free(input);
      continue;
    }
    //Strip the whitespace from user input. If the input
    //was only whitespace, then get some more input.
    remove_whitespace(input, input);
    if(strlen(input)==0) {
      free(input);
      continue;
    }

    add_history(input);

    //Stop if the user requests a close
    //TODO: we should free all of the variables as well.
    if(strcmp(input, "Quit")==0 ||
       strcmp(input, "Exit")==0 ||
       strcmp(input, "Q") == 0) {
      free(input);
      exit(0);
    }

    //Print out all of the saved variable names
    if(strcmp(input, "Vars") == 0) {
      show_vars(rootPointer);
      continue;
    }

    //Separate the input into lvalues and rvalues
    size_t lhs_length = strcspn(input, "=");
    size_t rhs_length = strlen(input)-lhs_length-1;
    //We can't have expression of the form `something=',
    //or `=something'.
    if(lhs_length == 0 || rhs_length == 0) {
      printf("Malformed expression\n");
      free(input);
      continue;
    }

    //If there is nothing on the rhs, we just have to
    //evaluate an expression.
    if(rhs_length == -1) {
      //Try to retrieve the variable.
      tree_node_t* var = get_variable(input);
      //If the expression was indeed a variable, print
      //its value.
      if(var) {
        bn_prnt_dec(*(var->value));
        free(input);
        continue;
      }

      //TODO: deal with any other expression.
      evaluate_expression(input);

      //Otherwise, it is not a valid command
      //printf("Command not found\n");
      free(input);
      continue;
    }
    //Otherwise we have to evaluate the rhs, and
    //then store it in the lhs
    else {
      //Split up the input to the lhs and rhs parts
      char* lhs = malloc(lhs_length + 1);
      memcpy(lhs, input, lhs_length);
      lhs[lhs_length] = '\0';
      char* rhs = malloc(rhs_length+1);
      memcpy(rhs, input+lhs_length+1, rhs_length);
      rhs[rhs_length] = '\0';

      free(input);

      //If the lhs is not of valid format for a variable,
      //just move on.
      if(!is_valid_var_name(lhs)) {
        printf("Invalid variable name: must be alphanumeric"
               " and begin with a lower case letter.\n");
        free(lhs);
        free(rhs);
        continue;
      }

      //Otherwise, put the value in to the variable
      if(isdigit_str(rhs)) {
        //If the rhs is a number, save it in the variable.
        save_variable(lhs, rhs);
      } else {
        //Otherwise, try to evaluate the expression, and
        //then save it in the variable TODO
        evaluate_expression(rhs);
      }

      free(lhs);
      free(rhs);
    }
  }

  return 0;
}
