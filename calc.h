#ifndef CALC_H
#define CALC_H

#define OPEN		0
#define CLOSE		1
#define FUNTION		2
#define OPERATION	3
#define UNKNOWN		4
#define CONSTANT	5
#define NUMBER		6
#define DOT			7

// Buttons of screen. P is Pi, R is Root.
const char buttonChar[32][5] = {
	"y=", "x", "P", "(", ")", "Del", "^", "R",
	"sin", "abs", "e", "7", "8", "9", "/", "-",
	"cos", "ln", "%", "4", "5", "6", "*", "+",
	"tan", "log", ".", "1", "2", "3", "0", "Draw"
};

const int state[32] = {
	-1, UNKNOWN, CONSTANT, OPEN, CLOSE, -1, OPERATION, FUNTION,
	FUNTION, FUNTION, CONSTANT, NUMBER, NUMBER, NUMBER, OPERATION, OPERATION,
	FUNTION, FUNTION, OPERATION, NUMBER, NUMBER, NUMBER, OPERATION, OPERATION,
	FUNTION, FUNTION, DOT, NUMBER, NUMBER, NUMBER, NUMBER, -1
};

#endif