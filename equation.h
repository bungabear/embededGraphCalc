#ifndef EQUATION_H
#define EQUATION_H

#include "queue.h"

int Confirm_Word(char *str);
String_Queue *Adjust_Parentheses(String_Queue *eqution);
String_Queue *Reguler_equation(String_Queue *equation);
String_Queue *Infix_To_Postfix(String_Queue *infix);

#endif
