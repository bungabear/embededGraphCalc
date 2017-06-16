#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<fcntl.h>
#include<linux/fb.h>
#include<sys/mman.h>
#include<sys/ioctl.h>
#include<sys/types.h>
#include<linux/input.h>
#include<sys/time.h>
#include"calc.h"
#include"queue.h"
#include"pointqueue.h"

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
Queue *equationQueue;
PointQueue *pointQueue;

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
    equationQueue = malloc(sizeof(Queue));
    pointQueue = malloc(sizeof(pointQueue));
    Init_Queue(equationQueue);
    Init_PointQueue(pointQueue);
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
        // TODO Draw graph
        // clear equation and empty queue
        Empty_Queue(equationQueue);
        memset(pfbdata, 0x0, fbvar.xres*fontSize*2);
        
        return;
    }
    // Del button
    else if(buttonNum == 5)
    {
        // Pop For delete one
        Pop(equationQueue);
        
    }
    else
    {
        // block y= twice
        if(buttonNum == 0 && equationQueue->size != 0)
            return;
        // mush y= is first input
        if(buttonNum !=0 && equationQueue->size == 0)
            return;
        // else buttons
        Enqueue(equationQueue, buttonChar[buttonNum]);
        if(state[buttonNum] == FUNTION)
        {
            // add ( for Function
            Enqueue(equationQueue, "(");

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
