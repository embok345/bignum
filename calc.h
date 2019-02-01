#include "src/bignum.h"
#include <readline/readline.h>
#include <readline/history.h>

/* strings.c */
int remove_whitespace(const char*, char*);
int isalnum_str(const char*);
//int isdigit_str(const char*);
char* find_longest_prefix(char**, int32_t);

/* util.c */
typedef struct int_list_t int_list_t;
int min(int, int);
int max(int, int);
int_list_t* push(int, int_list_t*);
int pop(int_list_t**);

/* variables.c */
/* Binary sorted tree data structure for storing variables.
 * -----------------------------------------------------------------------------
 * char* label - The name of the variable being stored. The tree is sorted
 *     lexographically by this.
 * bignum* value - The bignum (a pointer to a bignum) stored in the variable.
 * tree_node_t* left - A pointer to the left subtree of this node (null if it
 *     doesn't have one).
 * tree_node_t* right - As above, for the right subtree.
 */
typedef struct tree_node_t tree_node_t;
struct tree_node_t {
  char* label;
  bignum* value;
  tree_node_t* left;
  tree_node_t* right;
};
int is_valid_var_name(const char*);
void new_node(char*, char*, tree_node_t*);
tree_node_t* get_variable(char*, tree_node_t*);
tree_node_t* save_variable(char*, char*, tree_node_t**);
void show_vars(tree_node_t*);
tree_node_t* find_prefix_node(const char*, tree_node_t*);
int32_t tree_size(const tree_node_t*);
int32_t get_var_completion(const char*, tree_node_t*, char**, int32_t);
