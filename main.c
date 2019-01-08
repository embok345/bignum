#include "src/bignum.h"
#include <readline/readline.h>
#include <readline/history.h>

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

/* The root node for the variable tree */
tree_node_t* rootPointer = NULL;

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
  if(!isalpha(*label) || !isalnum_str(label+1)) {
    return NULL;
  }
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
  //If the tree is empty, just insert the new variable in
  //the root.
  if(rootPointer == NULL) {
    rootPointer = malloc(sizeof(tree_node_t));
    new_node(label, value, rootPointer);
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


void evaluate_addition(char* lhs, char* rhs) {
  bignum left, right;
  if(isdigit_str(lhs)) {
    left = bn_conv_str2bn(lhs);
  } else {
    tree_node_t* lvar = get_variable(lhs);
    if(lvar==NULL) {
      printf("Not a valid expression\n");
      return;
    }
    left = *(lvar->value);
  }
  if(isdigit_str(rhs)) {
    right = bn_conv_str2bn(rhs);
  } else {
    tree_node_t* rvar = get_variable(rhs);
    if(rvar==NULL) {
      printf("Not a valid expression\n");
      return;
    }
    right = *(rvar->value);
  }

  bignum result = bn_add(left, right);
  bn_prnt_dec(result);
  bn_destroy(&result);
}

void evaluate_expression(char* expression) {
  char* rhs = strstr(expression, "+");
  rhs++;
  char* lhs = malloc(strlen(expression)-strlen(rhs));
  memcpy(lhs, expression, strlen(expression)-strlen(rhs)-1);
  lhs[strlen(expression)-strlen(rhs)-1] = '\0';
  //printf("%s, %s\n", lhs, rhs);
  evaluate_addition(lhs, rhs);
}

int main(int argc, char *argv[]){
  srand(time(NULL));

  while(1) {
    //Continuously prompt for user input.
    char *input;
    input = readline("> ");

    //Stop if the user requests a close
    //TODO: we should free all of the variables as well.
    if(strcmp(input, "quit")==0 ||
       strcmp(input, "exit")==0 ||
       strcmp(input, "q") == 0) {
      free(input);
      exit(0);
    }

    //Strip the whitespace from user input. If the input
    //was only whitespace, then get some more input.
    remove_whitespace(input, input);
    if(strlen(input)==0) {
      free(input);
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
      if(var!=NULL) {
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
      if(!isalpha(*lhs) || !isalnum_str(lhs+1)) {
        printf("Invalid variable name: must be alphanumeric"
               " and begin with a letter.\n");
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
