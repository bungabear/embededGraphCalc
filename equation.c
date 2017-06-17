#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "equation.h"

extern const int OPEN;
extern const int CLOSE;     
extern const int FUNCTION;
extern const int OPERATION; 
extern const int XVALUE;    
extern const int CONSTANT;  
extern const int NUMBER;    
extern const int DOT;       
extern const int state[32];
extern const char buttonChar[32][5];

int Confirm_Word(char *str) {

	int i=0, len=0, size=0, index = -1;
	size = sizeof(state) / sizeof(state[0]);
	for (i = 0; i < size ; i++) {
		len = strlen(str);
		if (strncmp(str, buttonChar[i], len) == 0) {
			index = i;
			break;
		}
	}
	return state[i];
}

String_Queue *Adjust_Parentheses(String_Queue *equation) {
	int i, open=0, close=0;
	String_Node *node = equation->head;

	while(node != NULL) {
		if (strncmp(node->context, "(", 1) == 0) {
			open++;
		} else if (strncmp(node->context, ")", 1) == 0) {
			close++;
		}
		node = node->right;
	}

	if (open > close) {
		for (i = 0; i < (open - close); i++) {
			Enqueue(equation, ")");
		}
	} else if (open < close) {
		printf("There are more closing parentheses\n");
		return 0;
	}
	return equation;
}

String_Queue *Reguler_equation(String_Queue *equation) {
	
	equation = Adjust_Parentheses(equation);
	if (equation == NULL) return equation;

	String_Queue *infix = (String_Queue *)malloc(sizeof(String_Queue));

	if (infix == NULL) return infix;

	Init_Queue(infix);

	char *last_str = NULL, *str = NULL;
	int last_state = -1, cur_state = -1, res = -1;

	while (!Is_Empty_Queue(equation)) {
		str = Dequeue(equation);

		cur_state = Confirm_Word(str);
		if (cur_state < 0) continue;

		if (last_state == -1) {
			if (cur_state == OPERATION || cur_state == DOT || cur_state == CLOSE) {
				printf("irreguler equation\n");
				return 0;
			} else if (cur_state == FUNCTION || cur_state == XVALUE || cur_state == OPEN || cur_state == CONSTANT){
				res = Enqueue(infix, str);
				if (res < 0) return 0;
			} else if (cur_state == NUMBER)		last_str = str;
		} else if (last_state == OPERATION) {
			if (cur_state == OPERATION || cur_state == DOT || cur_state == CLOSE) {
				printf("irreguler equation\n");
				return 0;
			}
			else if (cur_state == XVALUE || cur_state == FUNCTION || cur_state == OPEN || cur_state == CONSTANT) {
				res = Enqueue(infix, str);
				if (res < 0) return 0;
			}
			else if (cur_state == NUMBER)		last_str = str;
		} else if(last_state == FUNCTION){
			if (cur_state == OPERATION || cur_state == DOT || cur_state == CLOSE) {
				printf("irreguler equation\n");
				return 0;
			} else if (cur_state == XVALUE || cur_state == FUNCTION || cur_state == OPEN || cur_state == CONSTANT) {
				res = Enqueue(infix, str);
				if (res < 0) return 0;
			} else if (cur_state == NUMBER)		last_str = str;
		} else if (last_state == CONSTANT) {
			if (cur_state == DOT) {
				printf("irreguler equation\n");
				return 0;
			} else if (cur_state == XVALUE || cur_state == FUNCTION || cur_state == OPEN || cur_state == CONSTANT ) {
				res = Enqueue(infix, "*");
				if (res < 0) return 0;
				res = Enqueue(infix, str);
				if (res < 0) return 0;
			} else if (cur_state == OPERATION || cur_state == CLOSE) {
				res = Enqueue(infix, str);
				if (res < 0) return 0;
			} else if (cur_state == NUMBER) {
				res = Enqueue(infix, "*");
				if (res < 0) return 0;
				last_str = str;
			}
		} else if (last_state == XVALUE) {
			if (cur_state == DOT) {
				printf("irreguler equation\n");
				return 0;
			} else if (cur_state == XVALUE || cur_state == FUNCTION || cur_state == OPEN || cur_state == CONSTANT ) {
				res = Enqueue(infix, "*");
				if (res < 0) return 0;
				res = Enqueue(infix, str);
				if (res < 0) return 0;
			} else if (cur_state == OPERATION || cur_state == CLOSE) {
				res = Enqueue(infix, str);
				if (res < 0) return 0;
			} else if (cur_state == NUMBER) {
				res = Enqueue(infix, "*");
				if (res < 0) return 0;
				last_str = str;
			}
		} else if (last_state == OPEN) {
			if (cur_state == DOT || cur_state == OPERATION) {
				printf("irreguler equation\n");
				return 0;
			} else if (cur_state == XVALUE || cur_state == FUNCTION || cur_state == CONSTANT || cur_state == OPEN || cur_state == CLOSE) {
				res = Enqueue(infix, str);
				if (res < 0) return 0;
			} else if (cur_state == NUMBER) {
				last_str = str;
			}
		} else if (last_state == CLOSE) {
			if (cur_state == DOT) {
				printf("irreguler equation\n");
				return 0;
			} else if (cur_state == XVALUE || cur_state == FUNCTION || cur_state == CONSTANT || cur_state == OPEN) {
				res = Enqueue(infix, "*");
				if (res < 0) return 0;
				res = Enqueue(infix, str);
				if (res < 0) return 0;
			} else if (cur_state == CLOSE || cur_state == OPERATION) {
				res = Enqueue(infix, str);
				if (res < 0) return 0;
			} else if (cur_state == NUMBER) {
				res = Enqueue(infix, "*");
				if (res < 0) return 0;
				last_str = str;
			}
		
		} else if (last_state == DOT) {
			if (cur_state == NUMBER) {
				last_str = (char *)realloc(last_str, strlen(last_str) + strlen(str) + 1);
				strncat(last_str, str, strlen(str));
			} else {
				printf("irreguler equation\n");
				return 0;
			}
		} else if (last_state == NUMBER) {
			if (cur_state == NUMBER || cur_state == DOT) {
				last_str = (char *)realloc(last_str, strlen(last_str) + strlen(str) + 1);
				strncat(last_str, str, strlen(str));
			} else if (cur_state == XVALUE || cur_state == FUNCTION || cur_state == CONSTANT || cur_state == OPEN) {
				res = Enqueue(infix, last_str);
				if (res < 0) return 0;
				res = Enqueue(infix, "*");
				if (res < 0) return 0;
				res = Enqueue(infix, str);
				if (res < 0) return 0;
				free(last_str);
			} else if (cur_state == CLOSE || cur_state == OPERATION) {
				res = Enqueue(infix, last_str);
				if (res < 0) return 0;
				res = Enqueue(infix, str);
				if (res < 0) return 0;
				free(last_str);
			}
		}
		last_state = cur_state;
	}

	return infix;
}

String_Queue *Infix_To_Postfix(String_Queue *infix) {
	return 0;
}
