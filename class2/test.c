//俄罗斯方块
#include <stdio.h>
#include <unistd.h>
#include "keyboard.h"
#include <signal.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <time.h>

int FC=5;//前景颜色
#define BC 0//背景颜色
#define W 10//地图宽度
#define H 20//地图高度
int cur = 0;


struct shape
{
    int s[5][5];
};

int background[H][W] = {};//保存背景的数组,linux下可以不用0进行初始化

struct data
{
    int x;//列
    int y;//行
};
struct data t = {5, 0};//图形的位置

struct shape shape_arr[7] = {//装方块图案
    {0, 0, 0, 0, 0,  0, 0, 1, 0, 0,  0, 1, 1, 1, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0,},
    {0, 0, 0, 0, 0,  0, 1, 0, 0, 0,  0, 1, 1, 1, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0,}, 
    {0, 0, 0, 0, 0,  0, 0, 0, 1, 0,  0, 1, 1, 1, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0,}, 
    {0, 0, 0, 0, 0,  0, 1, 1, 0, 0,  0, 0, 1, 1, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0,}, 
    {0, 0, 0, 0, 0,  0, 0, 1, 1, 0,  0, 1, 1, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0,}, 
    {0, 0, 0, 0, 0,  0, 1, 1, 0, 0,  0, 1, 1, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0,}, 
    {0, 0, 0, 0, 0,  0, 0, 1, 0, 0,  0, 0, 1, 0, 0,  0, 0, 1, 0, 0,  0, 0, 0, 0, 0,}
};

void drow_element(int x, int y, int c)
{
    x *= 2;//x坐标占两个字符
    x++;//用户习惯(0,0)开始，但是Linux是(1,1)开始，调整符合用户使用
    y++;

    printf("\033[?25l");//隐藏光标
    printf("\033[%d;%dH", y, x);//控制光标位置
    printf("\033[3%dm\033[4%dm", c, c);//字体颜色+背景颜色
    printf("[]");//打印方块
    fflush(stdout);
    printf("\033[0m");//关闭设置的属性
   
}

void drow_shape(int x, int y, struct shape p, int c)//绘制方块图
{
    int i = 0;
    for(; i<5; i++)
    {
        int j = 0;
        for(; j<5; j++)
        {
            if(p.s[i][j] != 0)
            {
                drow_element(x+j, y+i, c);
            }
        }
    }
}

void drow_back()//绘制背景
{
    int i = 0;
    for(i=0; i<H; i++)
    {
        int j = 0;
        for(j=0; j<W; j++)
        {
            if(background[i][j] == 0)
                drow_element(j, i, BC);
            else
                drow_element(j, i, background[i][j]);
        }
    }
}

void set_back(struct data* t, struct shape p)//控制方块在下方重叠起来
{
    int i = 0;
    for(; i<5; i++)
    {
        int j = 0;
        for(; j<5; j++)
        {
            if(p.s[i][j] != 0)
                background[t->y+i][t->x+j] = FC;
        }
    }
}

int can_move(int x, int y, struct shape p)
{
    //-std=gun99(可以将int i=0定义在循环中)
    int i = 0;
    for(i=0; i<5; i++)
    {
        int j = 0;
        for(j=0; j<5; j++)
        {
            if(p.s[i][j] == 0)
                continue;
            if(x+j >= W)//右边界
                return 0;
            if(y+i >= H-1)//下边界
                return 0;
            if(x+j < 0)//左边界
                return 0;
            if(background[y+i+1][x+j] != 0)
                return 0;
        } 
    }
    return 1;
}

void mclean_line()//消去满行元素
{
    int i = 0;
    for(; i<H; i++)
    {
        int total = 0;
        int j = 0;
        for(; j<W; j++)
        {
            if(background[i][j] != 0)
                total++;
        }
        if(total == W)
        {
            int k = i;
            for(; k>0; k--)//依次将K-1行覆盖到k行
            {
                memcpy(background[k], background[k-1], sizeof(background[k]));
            }
            memset(background[cur], 0x00, sizeof(background[cur]));
        }
    }
}

void tetris_timer(struct data* t)//绘制下落
{
    drow_shape(t->x, t->y, shape_arr[cur], BC);
    if(can_move(t->x, t->y, shape_arr[cur]))
        t->y++;
    else//超过边界，就从上方重新掉落
    {
        set_back(t, shape_arr[cur]);
        mclean_line(); 
        drow_back();
        do
        {
            FC = rand()%7;
        }while(FC == BC);

        t->y = 0; 
        t->x = 0;
        cur = rand()%7;
    }
    drow_shape(t->x, t->y, shape_arr[cur], FC);
}

struct shape turn_90(struct shape p)
{
    struct shape t;
    int i = 0;
    for(; i<5; i++)
    {
        int j = 0;
        for(; j<5; j++)
        {
            t.s[i][j] = p.s[4-j][i];
        }
    }
    return t;
}

int tetris(struct data* t)//控制上下左右键盘
{
    int ret = 0;
    int c = get_key();
    if(is_left(c))
    {
        drow_shape(t->x, t->y, shape_arr[cur], BC);
        if(can_move(t->x-1, t->y, shape_arr[cur]))
            t->x--;
        drow_shape(t->x, t->y, shape_arr[cur], FC);
    }
    else if(is_right(c))
    {
        drow_shape(t->x, t->y, shape_arr[cur], BC);
        if(can_move(t->x+1, t->y, shape_arr[cur]))
            t->x++;
        drow_shape(t->x, t->y, shape_arr[cur], FC);
    }
    else if(is_down(c))//下键控制加速
    {
        drow_shape(t->x, t->y, shape_arr[cur], BC);
        //这里的+1是要判断下一个位置是否出界，是否能落点
        if(can_move(t->x, t->y+1, shape_arr[cur]))
            t->y++;
        drow_shape(t->x, t->y, shape_arr[cur], FC);
    }
    else if(is_up(c))//上键控制旋转
    {
        drow_shape(t->x, t->y, shape_arr[cur], BC);
        shape_arr[cur] = turn_90(shape_arr[cur]);
        if(can_move(t->x, t->y, shape_arr[cur]) == 0)//旋转之后的位置出边界就再转回来
        {
            shape_arr[cur] = turn_90(shape_arr[cur]);
            shape_arr[cur] = turn_90(shape_arr[cur]);
            shape_arr[cur] = turn_90(shape_arr[cur]);
        }
        drow_shape(t->x, t->y, shape_arr[cur], FC);
    }
    else if(is_esc(c))
    {
        ret = 1;
    }
    return ret;
}

void handler(int s)
{
    if(s == SIGALRM)
        tetris_timer(&t);
    else if(s == SIGINT)
    {
        printf("\033[0m");//恢复属性
        printf("\033[?25h");//恢复光标
        recover_keyboard();
        exit(0);
    }
}

int main()
{
    srand((unsigned int) time(NULL));
    init_keyboard();
    drow_back();

    struct sigaction act;
    act.sa_handler = handler;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    sigaction(SIGALRM,&act, NULL);
    sigaction(SIGINT, &act, NULL); 
    
    //设置定时器，控制方块一秒下落一次    
    struct itimerval it;
    //两个同时为0时，定时器永远启动不了
    it.it_value.tv_sec = 0;//单位为微秒
    it.it_value.tv_usec = 1;
    
    //设置间隔时间，每个一秒定时器触发一次
    it.it_interval.tv_sec = 1;
    it.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL,&it,NULL);


    while(1)
    {
     //   tetris_timer(&t);
        if(tetris(&t) == 1)
            break;
   //     sleep(1);
    }
    printf("\033[?25h");//将隐藏光标恢复
    recover_keyboard();
    return 0;
}
