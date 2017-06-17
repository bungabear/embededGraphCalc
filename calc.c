#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<math.h>
#include<errno.h>
#include<fcntl.h>
#include<linux/fb.h>
#include<sys/mman.h>
#include<sys/ioctl.h>
#include<sys/types.h>
#include<linux/input.h>
#include<sys/time.h>
#include<limits.h>
#include"queue.h"
#include"pointqueue.h"
#include"doublequeue.h"
#include"equation.h"
#include"calc.h"

unsigned short *pfbdata;
struct fb_var_screeninfo fbvar;
int buttonsYPos;
int pressedFlag = 0;
int graphWidth;
int graphHeight;

// x, y's start value in virtual pixel value. ex) x = -160(-16)
int graphXstart;
int graphYstart;
// 10pixel per inteager x, y
int xScale = 20;
int yScale = 20;
int mode = 0;
Queue *equationQueue = NULL;
PointQueue *pointQueue = NULL;
Queue *graphQueue = NULL;

void connectPoint(int x1, int y1, int x2, int y2)
{
    int i, half, tmp, x, offset;
    printf("draw line (%d, %d) to (%d, %d)\n", x1, y1, x2, y2);
    if(y1 > y2)
    {
        tmp = x1;
        x1 = x2;
        x2 = tmp;
        tmp = y1;
        y1 = y2;
        y2 = tmp;
    }
    half = y2 - (y2 - y1)/2;
    for(i = y1; i <= y2; i++)
    {
        if(i< half)
            x = x1;
        else x = x2;
        // TODO apply scales to calculate y's range.
        if(i < fontSize || i > graphHeight + fontSize)
        {
            continue;
        }
        offset = x + fbvar.xres*i;
        *(pfbdata+offset) = 0x00ff;
    }
}
double calcEquation(double x, Queue *postfix, int *errorno)
{
    double y = 0, first, second;
    int err = 0;
    char *str;
    DoubleQueue *stack;
    stack = malloc(sizeof(DoubleQueue));
    Init_DoubleQueue(stack);
    Node *tmp = postfix->head;
    while(tmp != NULL)
    {
        errno = 0;
        str = tmp->context;
        switch(Confirm_Word(str))
        {
            // FUNCTION
            case 2:
                first = Pop_DoubleQueue(stack);
                if(strncmp(str, "sin" ,5) == 0)
                {
                    first = sin(first);
                }
                else if(strncmp(str, "cos" ,5) == 0)
                {
                    first = cos(first);
                }
                else if(strncmp(str, "tan" ,5) == 0)
                {
                    first = tan(first);
                    // tan() doesn't return infinity.
                    // showable y :  graphYstart < Y < graphHeight + graphYstart 
                    if(graphYstart > first || graphYstart + graphHeight < first)
                    {
                        err = 1; 
                    }
                }
                else if(strncmp(str, "ln" ,5) == 0)
                {
                    first = log(first);
                    if(isnan(first))
                        err = errno;
                }
                else if(strncmp(str, "log" ,5) == 0)
                {
                    first = log10(first);
                    if(isnan(first))
                        err = errno;
                }
                else if(strncmp(str, "abs", 5) == 0)
                {
                    first = fabs(first);
                }   
                else if(strncmp(str, "R", 5) == 0)
                {
                    first = sqrt(first);
                    if(isnan(first))
                        err = errno;
                }   
                if(err != 0)
                {
                    *errorno = err;
                    Clear_DoubleQueue(stack);
                    free(stack);
                    return -1;
                }   
                Push_DoubleQueue(stack,first);
                break;
                //OPERATION
            case 3:
                second = Pop_DoubleQueue(stack);
                first = Pop_DoubleQueue(stack);
                switch(str[0])
                {
                    case '+':
                        first = first + second;
                        break;
                    case '-':
                        first = first - second;
                        break;
                    case '*':
                        first = first * second;
                        break;
                    case '/':
                        if(second == 0)
                        {
                            *errorno = 1;
                            Clear_DoubleQueue(stack);
                            free(stack);
                            return -1;
                        }
                        else
                            first = first / second;
                        break;
                    case '%':
                        first = first + second;
                        break;
                    case '^':
                        first = pow(first, second);
                        break;
                }
                Push_DoubleQueue(stack,first);
                break;
                // XVALUE
            case 4:
                Push_DoubleQueue(stack, x);
                break;
                // CONSTANT
            case 5:
                switch(str[0])
                {
                    case 'P':
                        Push_DoubleQueue(stack, M_PI);
                        break;
                    case 'e':
                        Push_DoubleQueue(stack, M_E);
                        break;
                }
                break;
                //NUMBER
            case 6:
                Push_DoubleQueue(stack, atof(str));
                break;
        }
        tmp = tmp->right;
    }
    y = Pop_DoubleQueue(stack);
    Clear_DoubleQueue(stack);
    free(stack);
    *errorno = err;
    return y;
}

void drawGraph(Queue *postfix)
{
    int xPadding = 0, offset, x, y, z;
    if(graphWidth < fbvar.xres)
    {
        xPadding = fbvar.xres - graphWidth;
        xPadding = xPadding >> 2;
    }
    // paint background to white
    for(y = fontSize; y < graphHeight + fontSize; y++)
    {
        for(x = xPadding; x < graphWidth - xPadding; x++)
        {
            offset = x+y*fbvar.xres;
            *(pfbdata+offset) = 0xffff;
        }
    }

    //xStart is pixels. 
    for(y = fontSize; y < graphHeight + fontSize; y++)
    {
        for(x = xPadding; x < graphWidth; x++)
        {
            // draw Y Line
            if(graphXstart + x == 0)
            {
                *(pfbdata + x + fbvar.xres*y) = 0x0;	
                // draw scale
                if((graphYstart + y)%yScale == 0)
                {
                    for(z = -2; z <= 2; z++)
                    {
                        *(pfbdata + x+z + fbvar.xres*y) = 0x0;	
                    }
                }
            }
            // draw X Line
            else if(graphYstart + y - fontSize == 0)
            {
                *(pfbdata + x + fbvar.xres*y) = 0x0;	
                // draw scale
                if((graphXstart + x)%xScale == 0)
                {
                    for(z = -2; z <= 2; z++)
                    {
                        *(pfbdata + x + fbvar.xres*(z+y)) = 0x0;	
                    }
                }
            }
        }
    }
    if(postfix != NULL)
    {
        double i,j;
        int errorno = 0;
        for(x = graphXstart; x < graphWidth+graphXstart; x++)
        {
            // TODO calculate equation and draw line with in points 
            i = x/(double)xScale;
            j = calcEquation(i, postfix, &errorno);
            if(errorno != 0)
            {
                errorno = 0;
                continue;
            }
            j *= yScale;
            Enqueue_PointQueue(pointQueue, x, (int)j);
        }
        // draw Line
        // if Queue is empty, Dequeue return -1
        int prex, prey;
	int preyover = 0, yover = 0;
        Dequeue_PointQueue(pointQueue, &prex,&prey);
        prex = prex - graphXstart;
	if(prey == INT_MIN)
	{
		prey = graphHeight + fontSize;
		preyover = 1;
	}
	else if(prey == INT_MAX)
	{
		prey = fontSize;
		preyover = 1;
	}
	else
		prey = graphHeight - prey + graphYstart + fontSize;
        while(Dequeue_PointQueue(pointQueue, &x,&y) != -1)
        {
            x = x - graphXstart;
	    if(y == INT_MIN)
	    {
		    y = graphHeight + fontSize;
		    yover = -1;
	    }
	    else if(y == INT_MAX)
	    {
		    y = fontSize;
		    yover = -1;
	    }
	    else
		    y = graphHeight - y + graphYstart + fontSize;
            //printf("set  x : %d, y : %d\n", x, y);
            // if point x is not continual, skip draw a line.
            if(x - prex == 1)
            {
                if(prey < fontSize)
		{
                    prey = fontSize;
		    preyover = 1;
                }
		else if(prey > graphHeight + fontSize)
		{
                    prey = graphHeight + fontSize;
		    preyover = 1;
                }
		if(y < fontSize)
		{
                    y = fontSize;
		    yover = -1;
                }
		else if(y > graphHeight + fontSize)
		{
                    y = graphHeight + fontSize;
		    yover = -1;
		}
		if(preyover != 1 && yover != -1)
			connectPoint(prex + preyover, prey, x + yover, y);
            }
            //else
            //offset = x + fbvar.xres*y;
            //*(pfbdata+offset) = 0x00ff;
            prex = x;
            prey = y;
	    preyover = 0;
	    yover = 0;
        }
    }
}
int main()
{
    int fd;
    equationQueue = malloc(sizeof(Queue));
    pointQueue = malloc(sizeof(PointQueue));
    Init_Queue(equationQueue);
    Init_PointQueue(pointQueue);
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
            }
            else if(ie.code == 1)
            { 
                y=ie.value;
            }
            if(x > -1 && y > -1 )
            {
                if(pressedFlag)
                {
                    tmpTime = elapsedTime(previousTime);
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
                    buttonTouch(touchButton);

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
    free(equationQueue);
    free(pointQueue);
    close(fd);
    return 0;
}
void buttonTouch(int buttonNum)
{
    printf("Button %s touched\n", buttonChar[buttonNum]);
    // "Draw" button
    if(buttonNum == 31)
    {
        // touch Draw
        if(mode == 0)
        {
            // clear equation and empty queue
            if(equationQueue->size < 2)
                return;

	    Queue *tmp = Copy_Queue(equationQueue);
            Queue *infix = Reguler_equation(tmp);
	    Clear_Queue(tmp);
	    free(tmp);
            if(infix != NULL)
            {
                Queue *postfix = Infix_To_Postfix(infix);
                drawGraph(postfix);
                Clear_Queue(infix);
		free(infix);
                //memset(pfbdata, 0x0, fbvar.xres*fontSize*2);
                if(graphQueue != NULL)
                {
                    Clear_Queue(graphQueue);
                    free(graphQueue);
                }
                graphQueue = postfix;
                mode = 1;
                printStr("Edit", buttonWidth*7+(buttonWidth-fontSize*4)/2 , buttonsYPos + buttonHeight*3 + (buttonHeight-fontSize)/2 , 0xffff, 0x0000);
                printStr("Cls", buttonWidth*5+(buttonWidth-fontSize*3)/2 , buttonsYPos + buttonHeight*0 + (buttonHeight-fontSize)/2 , 0xffff, 0x0000);
            }
        }
        // touch Edit
        else
        {
            mode = 0;
            printStr("Draw", buttonWidth*7+(buttonWidth-fontSize*4)/2 , buttonsYPos + buttonHeight*3 + (buttonHeight-fontSize)/2 , 0xffff, 0x0000);
            printStr("    ", buttonWidth*5+(buttonWidth-fontSize*4)/2 , buttonsYPos + buttonHeight*0 + (buttonHeight-fontSize)/2 , 0xffff, 0x0000);
            printStr("<-", buttonWidth*5+(buttonWidth-fontSize*2)/2 , buttonsYPos + buttonHeight*0 + (buttonHeight-fontSize)/2 , 0xffff, 0x0000);
        }
	return;
    }
    // Del button
    else if(buttonNum == 5)
    {
    	if(mode == 0)
	{
		// Pop For delete one
		Pop(equationQueue);
	}
	else
	{
		printf("Cls\n");
		graphXstart = (-graphWidth/2);
		graphYstart = (-graphHeight/2);
		xScale = 20;
		yScale = 20;
		drawGraph(NULL);
		Clear_Queue(graphQueue);
		Clear_Queue(equationQueue);
		free(graphQueue);
		graphQueue = NULL;
                memset(pfbdata, 0x0, fbvar.xres*fontSize*2);
		mode = 0;
		printStr("Draw", buttonWidth*7+(buttonWidth-fontSize*4)/2 , buttonsYPos + buttonHeight*3 + (buttonHeight-fontSize)/2 , 0xffff, 0x0000);
		printStr("    ", buttonWidth*5+(buttonWidth-fontSize*4)/2 , buttonsYPos + buttonHeight*0 + (buttonHeight-fontSize)/2 , 0xffff, 0x0000);
		printStr("<-", buttonWidth*5+(buttonWidth-fontSize*2)/2 , buttonsYPos + buttonHeight*0 + (buttonHeight-fontSize)/2 , 0xffff, 0x0000);
		return;
	}
    }
    else if(mode == 0)
    {
	    // block y= twice
	    if(buttonNum == 0 && equationQueue->size != 0)
		    return;
	    if(buttonNum != 0 && equationQueue->size == 0)
		    return;
	    // mush y= is first input
	    // else buttons
	    Enqueue(equationQueue, buttonChar[buttonNum]);

	    // check equation is correct, if not correct Dqeueue.
	    //if(equationQueue->size > 1)
	    //{
	    //    Queue *infix = Reguler_equation(equationQueue);
	    //    if(infix == NULL)
	    //    {
	    //        Dequeue(equationQueue);
	    //        return;
	    //    }
	    //}
	    if(state[buttonNum] == FUNCTION)
	    {
		    // add ( for Function
		    Enqueue(equationQueue, "(");

	    }
    }
    else
    {
	    // - in view mode
	    if(buttonNum == 15)
	    {
		    xScale /=2;
		    yScale /=2;
		    drawGraph(graphQueue);
		    return;
	    }
	    // + in view mode
	    else if(buttonNum == 23)
	    {
		    xScale *=2;
		    yScale *=2;
		    drawGraph(graphQueue);
		    return;
	    }
    }
    int i,j;
    char *str;
    Node *tmp;
    if(equationQueue->size !=0)
    {
	    tmp = equationQueue->head;
    }
    for(i = 0, j = 0 ; i < equationQueue->size; i++)
    {
	    j += strlen(tmp->context);
	    tmp = tmp->right;
    }
    tmp = equationQueue->head;
    str = malloc(j+1);
    memset(str, '\0', j+1);
    for(i = 0, j = 0 ; i < equationQueue->size; i++)
    {
	    strcpy(&str[j], tmp->context);
	    j += strlen(tmp->context);
	    tmp = tmp->right;
    }

    if(j > 40)
	    j -= 40;
    else j = 0;

    memset(pfbdata, 0x0, fbvar.xres*fontSize*2);
    printStr(&str[j], 0, 0, 0xffff, 0x0000);
    printf("%s\n", str);
    fflush(stdout);
    free(str);
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
