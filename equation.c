#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "equation.h"
#include "calc.h"

extern const int OPEN;
extern const int CLOSE;
extern const int FUNCTION;
extern const int OPERATION;
extern const int XVALUE;
extern const int CONSTANT;
extern const int NUMBER;
extern const int DOT;

extern const int FIRST;
extern const int SECOND;
extern const int THIRD;
extern const int FOURTH;

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
	if (i == size) return NUMBER;

	return state[i];
}

int Operation_Priority(char *str) {
	int state = Confirm_Word(str);

	if (state != OPERATION) {
		return FIRST;
	} else if (strncmp(str, "+", 1) == 0 || strncmp(str, "-", 1) == 0) {
		return SECOND;
	} 
	else if (strncmp(str, "*", 1) == 0 || strncmp(str, "/", 1) == 0 || strncmp(str, "%", 1) == 0) {
		return THIRD;
	}
	else if (strncmp(str, "^", 1) == 0 ) {
		return FOURTH;
	}
	
	return -1;
}

String_Queue *Adjust_Parentheses(String_Queue *eqution) {
	int i, open=0, close=0;
	String_Node *node = eqution->head;

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
			Enqueue(eqution, ")");
		}
	} else if (open < close) {
		printf("There are more closing parentheses\n");
		return 0;
	}
	return eqution;
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
			if (cur_state == OPERATION) {
				if (strncmp(str, "-", 1) == 0) {
					last_str = str;
				} else if (strncmp(str, "+", 1) == 0) {
					last_state = cur_state;
					continue;
				} else {
					printf("irreguler equation\n");
					return 0;
				}
			}else if (cur_state == OPERATION) {
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
			else if (cur_state == NUMBER) {
				if(last_str != NULL && strncmp(last_str, "-", 1) == 0)
				{
					last_str = (char *)realloc(last_str, strlen(last_str) + strlen(str) + 1);
					strncat(last_str, str, strlen(str));
				}
				else	last_str = str;	
			}		
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
			if (cur_state == OPERATION) {
				if (strncmp(str, "-", 1) == 0) {
					last_str = str;
				} else if (strncmp(str, "+", 1) == 0) {
					last_state = cur_state;
					continue;
				} else {
					printf("irreguler equation\n");
					return 0;
				}
			}else if (cur_state == DOT) {
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

		if (cur_state == NUMBER && Is_Empty_Queue(equation)) {
			if (last_str != NULL) {
				res = Enqueue(infix, last_str);
				if (res < 0) return 0;
				free(last_str);
				break;
			}
		}
		last_state = cur_state;
	}

	while (1)
	{
		str = Pop(infix);
		cur_state = Confirm_Word(str);
		if (cur_state == CLOSE) {
			last_str = Pop(infix);
			last_state = Confirm_Word(last_str);
			if (last_state == OPEN) {
				printf("irreguler equation\n");
				return 0;
			} else {
				res = Enqueue(infix,last_str);
				if (res < 0) return 0;
				res = Enqueue(infix, str);
				if (res < 0) return 0;
				break;
			}
		} else if (cur_state == FUNCTION || cur_state == DOT || cur_state == OPERATION) {
			printf("irreguler equation\n");
			return 0;
		} else {
			res = Enqueue(infix, str);
			if (res < 0) return 0;
			break;
		}

	}

	return infix;
}

String_Queue *Infix_To_Postfix(String_Queue *infix) {
	String_Queue *postfix = (String_Queue*)malloc(sizeof(String_Queue));
	if (postfix == NULL) return postfix;

	String_Queue *opStack = (String_Queue*)malloc(sizeof(String_Queue));
	if (opStack == NULL) return 0;

	char *str, *op;
	int state, res, tmp;
	int priority, last_priority;

	Init_Queue(postfix);
	Init_Queue(opStack);

	while (!Is_Empty_Queue(infix))
	{
		str = Dequeue(infix);

		state = Confirm_Word(str);
		if (state == NUMBER || state == CONSTANT || state == XVALUE) {
			res = Enqueue(postfix, str);
			if (res < -1) return 0;
		}
		else {
			if (!Is_Empty_Queue(opStack)) {
				priority = Operation_Priority(str);
				if (priority < 0) return 0;

				if (priority == FIRST) {
					tmp = Confirm_Word(str);
					if (tmp == FUNCTION || tmp == OPEN) {
						res = Push(opStack, str);
						if (res < 0) return 0;
					}
					else if (tmp == CLOSE) {
						while (1)
						{
							op = Pop(opStack);
							if (Confirm_Word(op) == OPEN) break;
							res = Enqueue(postfix, op);
							if (res < 0) return 0;
						}

						op = Pop(opStack);
						if (Confirm_Word(op) == OPEN) {
							res = Enqueue(opStack, op);
							if (res < 0) return 0;
						}
						else {
							res = Push(postfix, op);
							if (res < 0) return 0;
						}
					}
				} else {
					while (1) {
						if (!Is_Empty_Queue(opStack)) {
							op = Pop(opStack);
							last_priority = Operation_Priority(op);
							if (last_priority >= priority) {
								res = Enqueue(postfix, op);
								if (res < 0) return 0;
								continue;
							}
							else {
								res = Push(opStack, op);
								if (res < 0) return 0;
								res = Push(opStack, str);
								if (res < 0) return 0;
								break;
							}
						} else {
							res = Push(opStack, str);
							if (res < 0) return 0;
							break;
						}
					}
				}
			}
			else
				Push(opStack, str);
		}
	}

	while (!Is_Empty_Queue(opStack)) {
		str = Pop(opStack);
		res = Enqueue(postfix, str);
		if (res < 0) return 0;
	}

	return postfix;
}
