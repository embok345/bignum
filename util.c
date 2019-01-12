#include "calc.h"

int min(int a, int b) {
  return a<b ? a : b;
}
int max(int a, int b) {
  return a>b ? a : b;
}

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
