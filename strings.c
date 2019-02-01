#include "calc.h"

/* Remove all whitespace in a string (spaces, tabs, newline, etc.)
 * -----------------------------------------------------------------------------
 * const char* oldString - The string to remove whitespace from.
 * char* newString - The string after all whitespace has been removed. These can
 *     be the same string.
 *
 * return int - The number of whitespace characters removed.
 */
int remove_whitespace(const char* oldString,
	              char* newString) {
  int charsRemoved = 0;
  //Iterate over the characters in the string till we reach the end.
  while(*oldString != '\0') {
    //If the current char is not a whitespace, insert it into the new string.
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

/* Check if a string contains only alphanumeric characters (surely this is a
 * built in function?)
 * -----------------------------------------------------------------------------
 * const char* str - The string to check if it is alphanumeric.
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
 * -----------------------------------------------------------------------------
 * const char* str - The string to check if it is digital.
 *
 * return int - 1 if the string is digital, 0 otherwise.
 */
/*int isdigit_str(const char* str) {
  while(*str!='\0') {
    if(!isdigit(*str)) return 0;
    str++;
  }
  return 1;
}*/

/* Gets the longest prefix of all of the strings in the array.
 * -----------------------------------------------------------------------------
 * char** strings - The array of strings from which to extract the longest
 *     prefix.
 * int32_t noStrings - The number of strings in the array.
 *
 * return char* - The longest prefix in common of all of the strings.
 */
char* find_longest_prefix(char** strings,
                          int32_t noStrings) {
  //The longest prefix can be at most as long as any of the strings.
  char* prefix = strdup(strings[0]);

  for(int i=1; i<noStrings; i++) {
    //If the new string is shorter than the prefix, truncate the prefix to the
    //length of the new string.
    if(strlen(strings[i]) < strlen(prefix))
      prefix[strlen(strings[i])] = '\0';

    //Iterate through all of the characters in the prefix from the start.
    for(int j=0; j<strlen(prefix); j++) {
      //If a character differs, truncate the prefix string.
      if(prefix[j]!=strings[i][j]) {
        prefix[j] = '\0';
        //If they differ in the first character, there is no common prefix.
        if(strlen(prefix) == 0) return prefix;
        break;
      }
    }
  }
  return prefix;
}
