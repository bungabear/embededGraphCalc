#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<linux/fb.h>
#include<sys/mman.h>
#include<sys/ioctl.h>
#include<sys/types.h>
#include<linux/input.h>
#include<sys/time.h>
#include"queue.h"

#define FBDEVFILE "/dev/fb2"
// ascii + pi,root font
// size is 8*8 = 64bits.
const long long font[130] = {
	0x7E7E7E7E7E7E0000,	/* NUL */
	0x7E7E7E7E7E7E0000,	/* SOH */
	0x7E7E7E7E7E7E0000,	/* STX */
	0x7E7E7E7E7E7E0000,	/* ETX */
	0x7E7E7E7E7E7E0000,	/* EOT */
	0x7E7E7E7E7E7E0000,	/* ENQ */
	0x7E7E7E7E7E7E0000,	/* ACK */
	0x7E7E7E7E7E7E0000,	/* BEL */
	0x7E7E7E7E7E7E0000,	/* BS */
	0x0,			/* TAB */
	0x7E7E7E7E7E7E0000,	/* LF */
	0x7E7E7E7E7E7E0000,	/* VT */
	0x7E7E7E7E7E7E0000,	/* FF */
	0x7E7E7E7E7E7E0000,	/* CR */
	0x7E7E7E7E7E7E0000,	/* SO */
	0x7E7E7E7E7E7E0000,	/* SI */
	0x7E7E7E7E7E7E0000,	/* DLE */
	0x7E7E7E7E7E7E0000,	/* DC1 */
	0x7E7E7E7E7E7E0000,	/* DC2 */
	0x7E7E7E7E7E7E0000,	/* DC3 */
	0x7E7E7E7E7E7E0000,	/* DC4 */
	0x7E7E7E7E7E7E0000,	/* NAK */
	0x7E7E7E7E7E7E0000,	/* SYN */
	0x7E7E7E7E7E7E0000,	/* ETB */
	0x7E7E7E7E7E7E0000,	/* CAN */
	0x7E7E7E7E7E7E0000,	/* EM */
	0x7E7E7E7E7E7E0000,	/* SUB */
	0x7E7E7E7E7E7E0000,	/* ESC */
	0x7E7E7E7E7E7E0000,	/* FS */
	0x7E7E7E7E7E7E0000,	/* GS */
	0x7E7E7E7E7E7E0000,	/* RS */
	0x7E7E7E7E7E7E0000,	/* US */
	0x0,			/* (space) */
	0x808080800080000,	/* ! */
	0x2828000000000000,	/* " */
	0x287C287C280000,	/* # */
	0x81E281C0A3C0800,	/* $ */
	0x6094681629060000,	/* % */
	0x1C20201926190000,	/* & */
	0x808000000000000,	/* ' */
	0x810202010080000,	/* ( */
	0x1008040408100000,	/* ) */
	0x2A1C3E1C2A000000,	/* * */
	0x8083E08080000,	/* + */
	0x81000,		/* , */
	0x3C00000000,		/* - */
	0x80000,		/* . */
	0x204081020400000,	/* / */
	0x1824424224180000,	/* 0 */
	0x8180808081C0000,	/* 1 */
	0x3C420418207E0000,	/* 2 */
	0x3C420418423C0000,	/* 3 */
	0x81828487C080000,	/* 4 */
	0x7E407C02423C0000,	/* 5 */
	0x3C407C42423C0000,	/* 6 */
	0x7E04081020400000,	/* 7 */
	0x3C423C42423C0000,	/* 8 */
	0x3C42423E023C0000,	/* 9 */
	0x80000080000,		/* : */
	0x80000081000,		/* ; */
	0x6186018060000,	/* < */
	0x7E007E000000,		/* = */
	0x60180618600000,	/* > */
	0x3844041800100000,	/* ? */
	0x3C449C945C201C,	/* @ */
	0x1818243C42420000,	/* A */
	0x7844784444780000,	/* B */
	0x3844808044380000,	/* C */
	0x7844444444780000,	/* D */
	0x7C407840407C0000,	/* E */
	0x7C40784040400000,	/* F */
	0x3844809C44380000,	/* G */
	0x42427E4242420000,	/* H */
	0x3E080808083E0000,	/* I */
	0x1C04040444380000,	/* J */
	0x4448507048440000,	/* K */
	0x40404040407E0000,	/* L */
	0x4163554941410000,	/* M */
	0x4262524A46420000,	/* N */
	0x1C222222221C0000,	/* O */
	0x7844784040400000,	/* P */
	0x1C222222221C0200,	/* Q */
	0x7844785048440000,	/* R */
	0x1C22100C221C0000,	/* S */
	0x7F08080808080000,	/* T */
	0x42424242423C0000,	/* U */
	0x8142422424180000,	/* V */
	0x4141495563410000,	/* W */
	0x4224181824420000,	/* X */
	0x4122140808080000,	/* Y */
	0x7E040810207E0000,	/* Z */
	0x3820202020380000,	/* [ */
	0x4020100804020000,	/* \ */
	0x3808080808380000,	/* ] */
	0x1028000000000000,	/* ^ */
	0x7E0000,		/* _ */
	0x1008000000000000,	/* ` */
	0x3C023E463A0000,	/* a */
	0x40407C42625C0000,	/* b */
	0x1C20201C0000,		/* c */
	0x2023E42463A0000,	/* d */
	0x3C427E403C0000,	/* e */
	0x18103810100000,	/* f */
	0x344C44340438,		/* g */
	0x2020382424240000,	/* h */
	0x800080808080000,	/* i */
	0x800180808080870,	/* j */
	0x20202428302C0000,	/* k */
	0x1010101010180000,	/* l */
	0x665A42420000,		/* m */
	0x2E3222220000,		/* n */
	0x3C42423C0000,		/* o */
	0x5C62427C4040,		/* p */
	0x3A46423E0202,		/* q */
	0x2C3220200000,		/* r */
	0x1C201804380000,	/* s */
	0x103C1010180000,	/* t */
	0x2222261A0000,		/* u */
	0x424224180000,		/* v */
	0x81815A660000,		/* w */
	0x422418660000,		/* x */
	0x422214081060,		/* y */
	0x3C08103C0000,		/* z */
	0x1C103030101C0000,	/* { */
	0x808080808080800,	/* | */
	0x38080C0C08380000,	/* } */
	0x324C000000,		/* ~ */
	0x7E7E7E7E7E7E0000,	/* DEL */
	0x7E242424242464C4,	/* pi */
	0x0F08080850503010	/* root */
};

// Buttons of screen. P is Pi, R is Root.
const char buttonChar[32][5] =
{
	"y=", "x", "P", "(", ")", "Del", "^", "R",
	"sin", "abs", "e", "7", "8", "9", "/", "-",
	"cos", "ln", "%", "4", "5", "6", "*", "+",
	"tan", "log", ".", "1", "2", "3", "0", "Draw"
};
// It is for touch screen calibration. [0] is start value, [1] is calib. value
const int touchX[] = {180, 11};
const int touchY[] = {190, 15};
// Button's size in screen
const int buttonWidth = 40;
const int buttonHeight = 20;
// Font size is8*8
const int fontSize = 8;
// Delay is microsecond
const int touchDelay = 2000000;

void initScreen();
void printStr(const char *str, int x, int y, unsigned short color, unsigned short bgcolor);
void printChar(char charNum, int x, int y, unsigned short color, unsigned short bgcolor);
struct timeval elapsedTime(struct timeval prev);
void buttonTouch(const char *buttonStr);
void drawGraph(char *str);

unsigned short *pfbdata;
struct fb_var_screeninfo fbvar;
int buttonsYPos;
int pressedFlag = 0;
int graphWidth;
int graphHeight;
int graphXstart;
int graphYstart;
int xScale = 20;
int yScale = 20;
char equation[100] = {0,};

void drawGraph(char *str)
{
	int xPadding = 0, offset, x, y, z;
	if(graphWidth < fbvar.xres)
	{
		xPadding = fbvar.xres - graphWidth;
		xPadding = xPadding >> 2;
	}
	for(y = fontSize; y < graphHeight; y++)
	{
		for(x = xPadding; x < graphWidth - xPadding; x++)
		{
			offset = x+y*fbvar.xres;
			*(pfbdata+offset) = 0xffff;
		}
	}

	//xStart is pixels. 
	for(y = fontSize; y < graphHeight; y++)
	{
		for(x = xPadding; x < graphWidth; x++)
		{
			//draw Y Line
			if(graphXstart + x == 0)
			{
				*(pfbdata + x + fbvar.xres*y) = 0x0;	
				if((graphYstart + y)%yScale == 0)
				{
					for(z = -4; z <= 4; z++)
					{
						*(pfbdata + x+z + fbvar.xres*y) = 0x0;	
					}
				}
			}
			//draw X Line
			else if(graphYstart + y == 0)
			{
				*(pfbdata + x + fbvar.xres*y) = 0x0;	
				if((graphXstart + x)%xScale == 0)
				{
					for(z = -4; z <= 4; z++)
					{
						*(pfbdata + x + fbvar.xres*(z+y)) = 0x0;	
					}
				}
			}
		}
	}
	/*
	for(x = xPadding; x < graphWidth; x++)
	{
		*(pfbdata + x + fbvar.xres*(graphHeight/2 + fontSize)) = 0x0;	
	}
	for(y = fontSize; y < graphHeight; y++)
	{
		*(pfbdata + xPadding + graphWidth/2 + fbvar.xres*y) = 0x0;	
	}
	*/
	//str is NULL, Draw only graph's back 
	if(str != NULL)
	{
		//apply equation
	}
}
int main()
{
	int fd;
	initScreen();	
	fd = open("/dev/input/event4", O_RDWR);
	if(fd<0) return -1;
	int x = -1,y = -1, offset, touchButton;
	struct input_event ie;
	struct timeval previousTime, tmpTime;
	unsigned short color = 0xffff;
	gettimeofday(&previousTime, NULL);
	while(1)
	{
		read(fd, &ie, sizeof(struct input_event));
		if(ie.type==3)
		{
			if(ie.code == 0) 
			{
				x = ie.value;
				//printf("x . %ld\n", previousTime.tv_usec);
			}
			else if(ie.code == 1)
			{ 
				y=ie.value;
			}
			/* else if(ie.code == 24)
			   {
			   touchButton = ie.value;
			   }*/
			if(x > -1 && y > -1 )
			{
				if(pressedFlag)
				{
					tmpTime = elapsedTime(previousTime);
					//printf("pressed detect %d.%ld\n", tmpTime.tv_sec, tmpTime.tv_usec);
					if(tmpTime.tv_sec == 0 && tmpTime.tv_usec<= touchDelay)
					{
						x = -1;
						y = -1;
						gettimeofday(&previousTime, NULL);
					}
					else
					{
						pressedFlag = 0;
					}
					continue;
				}
				x=(x-touchX[0])/touchX[1];
				y=240 - ( (y-touchY[0])/touchY[1]);
				if(x>=320)
					x=319;
				if(y>=240)
					y=239;
				if(y > buttonsYPos)
				{
					touchButton = x/buttonWidth + ((y-buttonsYPos)/buttonHeight)*8;
					buttonTouch(buttonChar[touchButton]);
					
					//usleep(touchDelay);
					//read(fd, &ie, 100000);
					touchButton = -1;
				} 
				else
				{
					printf("x = %d, y = %d\n",x,y);
				}
				x = -1;
				y = -1;
				pressedFlag = 1;
				gettimeofday(&previousTime, NULL);
			}
			else
			{
				// fix x,y buffer problem
				tmpTime = elapsedTime(previousTime);
				if(tmpTime.tv_sec > 0 || tmpTime.tv_usec > touchDelay)
				{
					if(pressedFlag)
					{
						pressedFlag = 0;
					}
					else
					{   
						x = -1;
						y = -1;
						gettimeofday(&previousTime, NULL);
					}
				}
			}
		}
	}
	close(fd);
	return 0;
}
void buttonTouch(const char *buttonStr)
{
	printf("Button %s touched\n", buttonStr);
	// "Draw" button
	if(strncmp(buttonStr, buttonChar[31],5) == 0)
	{
		// Draw Graph
		memset(equation, '\0', 100);
	}
	// Del
	else if(strncmp(buttonStr, buttonChar[5],5) == 0)
	{
		// dequeue
	}
	int i,j,k = 0;
	int len = strlen(equation);

	for(i = len, j = 0 ; i < 99 ; i++)
	{
		if(buttonStr[j] != 0)
		{
			equation[i] = buttonStr[j++];
		}
		else
		{
			break;
		}
	}
	if(i>40)
	{
		k = i-40;
	}
	printStr(&equation[k], 0, 0, 0xffff, 0x0000);
	printf("%s\n", equation);
	fflush(stdout);

}
struct timeval elapsedTime(struct timeval prev)
{
	struct timeval ret;
	gettimeofday(&ret, NULL);
	ret.tv_sec = ret.tv_sec - prev.tv_sec;
	ret.tv_usec = ret.tv_usec - prev.tv_usec;
	if(ret.tv_sec != 0)
	{
		ret.tv_usec += 1000000;
	}
	return ret;
}
void initScreen()
{
	int fbfd, ret, x, y;
	fbfd = open(FBDEVFILE, O_RDWR);
	if(fbfd < 0)
	{
		perror("fbdev open");
		exit(1);
	}
	ret = ioctl(fbfd, FBIOGET_VSCREENINFO, &fbvar);
	if(ret < 0)
	{
		perror("fbdev ioctl");
		exit(1);
	}
	if(fbvar.bits_per_pixel != 16)
	{
		fprintf(stderr,"bpp is not 16\n");
		exit(1);
	}

	pfbdata = mmap(0, fbvar.xres*fbvar.yres*16/8, PROT_READ|PROT_WRITE, MAP_SHARED, fbfd, 0);

	buttonsYPos = fbvar.yres-(buttonHeight*4);
	graphWidth = fbvar.xres;
	graphHeight = fbvar.yres - buttonHeight*4 - fontSize;

	memset(pfbdata, 0, fbvar.xres*fbvar.yres*2);
	int offset, buttonCount = 0;
	for(y = fbvar.yres-(buttonHeight*4)-1 ; y < fbvar.yres; y++)
	{
		//4row each height is 20
		for(x = 0; x < fbvar.xres; x++)
		{
			offset = x + y*fbvar.xres;
			if(x%buttonWidth == 0 || y%buttonHeight == buttonHeight-1 || x == fbvar.xres-1)
			{
				*(offset+pfbdata) = 0xffff;
			}
			else 
			{
				if(y%buttonHeight == (buttonHeight-fontSize)/2 -1)
				{
					if((buttonCount%fontSize)*buttonWidth < x)
					{
						if(buttonHeight - (int)strlen(buttonChar[buttonCount])*4 == x%40)
						{
							if(buttonCount<32)
							{
								printStr(buttonChar[buttonCount], x, y, 0xffff, 0x0000);
								buttonCount++;
							}
						}
					}

				}
			}

			//8col each width is 40
		}
	}
	graphXstart = (-graphWidth/2);
	graphYstart = (-graphHeight/2);
	drawGraph(NULL);
}
void printStr(const char *str, int x, int y, unsigned short color, unsigned short bgcolor)
{
	int i = 0;
	while(str[i] != '\0')
	{
		printChar(str[i], x+i*8, y, color, bgcolor);
		i++;
	}
}
void printChar(char charNum, int x, int y, unsigned short color, unsigned short bgcolor)
{
	if(charNum == 'P')
		charNum = 128;
	else if(charNum == 'R')
		charNum = 129;
	int i, j, offset;
	long long ascii = font[charNum];
	long long tmp = ascii;
	//printf("%d : %llx\n",charNum, font[charNum]);
	for(i = 7 ; i >=0 ; i--)
	{
		if(y+i >= 240)
			continue;
		for(j = 7 ; j >= 0; j--)
		{
			if(x+j >= 320)
				continue;
			offset = (x+j) + (y+i)*fbvar.xres;
			if(tmp & 0x1)
			{
				*(pfbdata+offset) = color;
			}
			else
			{
				*(pfbdata+offset) = bgcolor;
			}
			tmp = tmp >> 1;
		}
	}

}
