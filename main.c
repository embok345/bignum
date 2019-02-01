#include "calc.h"

/* The root node for the variable tree */
tree_node_t* rootPointer = NULL;

/* The number of variables currently in the tree TODO change this */
int32_t noVars = 0;

/* Create an array of strings for use with tab completion from readline library.
 * The string array is one of three types:
 *     NULL, if there are no possible completions;
 *     A single command, followed by NULL, if there is only one possible
 *         completion; or,
 *     The longest common prefix of all of the possible completions, followed
 *         by all of the possible completions, followed by NULL.
 * -----------------------------------------------------------------------------
 * const char* command - The command to try and complete.
 * int start - The start index of the command to complete in the input string.
 * int end - The end index of the command to complete in the the input string.
 *
 * return char** - An array of strings of the possible completions of the given
 *     command.
 */
char** command_completion(const char* command,
                          int start,
                          int end) {

  //Tell readline that we are done with command completion.
  rl_attempted_completion_over = 1;

  //If the command to complete is a valid variable name, try to complete it as a
  //variable name.
  if(is_valid_var_name(command)) {
    //Find the first variable node in the tree which has the given command as
    //prefix.
    tree_node_t* prefixRoot = find_prefix_node(command, rootPointer);
    //If there are no such nodes, there are no possible completions.
    if(!prefixRoot)
      return NULL;

    //Otherwise, there are some possibilities, so get them.
    char** completions = malloc(sizeof(char*) * (tree_size(prefixRoot)+2));
    int32_t addedVars = get_var_completion(command, prefixRoot, completions, 1);

    //If there was precisely one possibility, return it.
    if(addedVars == 1) {
      completions[0] = strdup(completions[1]);
      completions[1] = NULL;
    }
    //Otherwise, find the longest common prefix.
    else {
      completions[0] = find_longest_prefix(completions+1, addedVars);
      completions[addedVars+1] = NULL;
    }
    return completions;
  }
}

void do_operation(const bignum *left,
                  const bignum *right,
                  char operation,
                  bignum *result) {
  switch(operation) {
    case '+': bn_add(left, right, result);
              break;
    case '-': bn_subtract(left, right, result);
              break;
    case '*': bn_mul(left, right, result);
              break;
    default: printf("Unknown operation\n");
             result = NULL;
  }
}


void expand_expression(char* expression, char operation, bignum *result) {
  char op[2];
  op[0] = operation;
  op[1] = '\0';

  if(!strchr(expression, operation)) {
    bn_clone(result, &ZERO);
    return;
  }

  size_t lhsLength = strcspn(expression, op);
  size_t rhsLength = strlen(expression) - lhsLength-1;
  if(lhsLength <= 0 || rhsLength <= 0) {
    printf("Malformed expression\n");
    bn_clone(result, &ZERO);
    return;
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
    bignum *leftNum, *rightNum;
    bn_init(&leftNum);
    bn_init(&rightNum);
    bn_conv_str2bn(lhs, leftNum);
    bn_conv_str2bn(rhs, rightNum);
    do_operation(leftNum, rightNum, operation, result);
    bn_nuke(&leftNum);
    bn_nuke(&rightNum);
    return;
  }
  if(isdigit_str(lhs) && is_valid_var_name(rhs)) {
    if(right = get_variable(rhs, rootPointer)) {
      bignum *leftNum;
      bn_init(&leftNum);
      bn_conv_str2bn(lhs, leftNum);
      do_operation(leftNum, right->value, operation, result);
      bn_nuke(&leftNum);
      return;
    }
    printf("Variable doesn't exist: %s\n", rhs);
    bn_clone(result, &ZERO);
    return;
  }
  if(is_valid_var_name(lhs) && is_valid_var_name(rhs)) {
    if((left = get_variable(lhs, rootPointer)) &&
       (right = get_variable(rhs, rootPointer))) {
      do_operation(left->value, right->value, operation, result);
      return;
    }
    if(!left) {
      printf("Variable doesn't exist: %s\n", lhs);
    }
    if(!right) {
      printf("Variable doesn't exist: %s\n", rhs);
    }
    bn_clone(result, &ZERO);
    return;
  }
  if(is_valid_var_name(lhs) && isdigit_str(rhs)) {
    if(left = get_variable(lhs, rootPointer)) {
      bignum *rightNum;
      bn_init(&rightNum);
      bn_conv_str2bn(rhs, rightNum);
      do_operation(left->value, rightNum, operation, result);
      bn_nuke(&rightNum);
      return;
    }
    printf("Variable doesn't exist: %s\n", lhs);
    bn_clone(result, &ZERO);
    return;
  }

  //Otherwise, we need to further expand either the left or
  //right hand side.
}

void evaluate_expression(char* expression, bignum *result) {
  char* str = expression;

  tree_node_t* var = get_variable(expression, rootPointer);
  if(var) {
    bn_clone(result, var->value);
    return;
  }

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
  if(strchr(str, '^'))
    expand_expression(str, '^', result);
  else if(strchr(str, '*'))
    expand_expression(str, '*', result);
  else if(strchr(str, '/'))
    expand_expression(str, '/', result);
  else if(strchr(str, '+'))
    expand_expression(str, '+', result);
  else if(strchr(str, '-'))
    expand_expression(str, '-', result);
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
      tree_node_t* var = get_variable(input, rootPointer);
      //If the expression was indeed a variable, print
      //its value.
      if(var) {
        bn_prnt_dec(var->value);
        free(input);
        continue;
      }

      //TODO: deal with any other expression.
      bignum *result;
      bn_init(&result);
      evaluate_expression(input, result);
      bn_prnt_dec(result);
      bn_nuke(&result);

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
        save_variable(lhs, rhs, &rootPointer);
      } else {
        //Otherwise, try to evaluate the expression, and
        //then save it in the variable TODO
        tree_node_t *var = get_variable(lhs, rootPointer);
        if(!var) {
          var = save_variable(lhs, "0", &rootPointer);
        }
        evaluate_expression(rhs, var->value);
      }

      free(lhs);
      free(rhs);
    }
  }

  return 0;
}
