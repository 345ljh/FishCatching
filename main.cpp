#include "graphics.h"
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include <time.h>
#include <vector>

const double pi = 3.14159;
class _fish_t;
class _bullet_t;

double frand(double min, double max) {return rand() / 32767.0 * (max - min) + min;}

int fire_x = 250;
int fire_y = 250;

typedef class _fish_t
{
private:
    double x;
    double y;
    double speed;
    double w_acc;
    double direct; //运动方向角度
    double len;
    long color;

    friend class _bullet_t;
public:
    void init(void)
    {
        switch(rand() % 4)
        {
        case 0:
            this->x = 5;
            this->y = rand() % 500;
            this->direct = frand(-pi / 2, pi / 2);
            break;
        case 1:
            this->x = rand() % 500;
            this->y = 5;
            this->direct = frand(-pi, 0);
            break;
        case 2:
            this->x = 495;
            this->y = rand() % 500;
            this->direct = frand(pi / 2, 1.5 * pi);
            break;
        case 3:
            this->x = rand() % 500;
            this->y = 495;
            this->direct = frand(0, pi);
            break;
        }
        this->speed = frand(1.2, 5.4);
        this->w_acc = frand(-0.05, 0.05);
        this->len = frand(20, 50);
        this->color = rand() % 255 + (rand() % 255 << 8) + (rand() % 255 << 16);
    }

    void move_step(void)
    {
        this->x += this->speed * cos(this->direct);
        this->y -= this->speed * sin(this->direct);
        this->direct += this->w_acc;
    }

    void draw()
    {
        setcolor(this->color);
        circle(this->x, this->y, 6);
        line(this->x, this->y, this->x - len * cos(this->direct), this->y + len * sin(this->direct));
    }

    bool out()
    {
        return this->x < -this->len || this->x > 500 + this->len || this->y < -this->len || this->y > 500 + this->len;
    }
}fish_t;

typedef class _bullet_t
{
private:
    double x;
    double y;
    double direct;
public:
    void init(double direct)
    {
        this->x = fire_x + 20 * cos(this->direct);
        this->y = fire_y + 20 * sin(this->direct);
        this->direct = direct;
    }

    void move_step(void)
    {
        this->x += 4 * cos(this->direct);
        this->y += 4 * sin(this->direct);
        this->direct += frand(-0.06, 0.06);
    }

    void draw()
    {
        setcolor(EGERGB(255, 60, 60));
        circle(this->x, this->y, 25);
    }

    bool out()
    {
        return this->x < -25 || this->x > 525 || this->y < -25 || this->y > 525;
    }

    bool iscatch(_fish_t* tar)
    {
        return sqrt(pow(this->x - tar->x, 2) + pow(this->y - tar->y, 2)) <= 25;
    }
}bullet_t;

char* Myitoa(int num)
{
    static char p[11];
    memset(p, 0, sizeof(p));
    itoa(num, p, 10);
    return p;
}

using namespace std;

int main()
{
    initgraph(500, 500);
    srand(time(NULL));
    vector<fish_t> fish;
    vector<bullet_t> bullet;
    int score = 0;
    int cd = 0;
    int maxfish = 4;

    //开始界面
    setcolor(WHITE);
    setfont(100, 0, "宋体");
    outtextxy(150, 50, (char*)"捕鱼");
    outtextxy(150, 150, (char*)"达人");
    setfont(15, 0, "宋体");
    outtextxy(0, 485, (char*)"按任意键进入教程...");
    getch();

    cleardevice();
    outtextxy(50, 100, (char*)"按空格或点击鼠标发射");
    outtextxy(50, 150, (char*)"按W A S D移动");
    outtextxy(50, 200, (char*)"当你的分数比较高时，水中将出现更多的鱼！");
    outtextxy(50, 250, (char*)"恭喜你，你已经学会了基本操作");
    outtextxy(50, 300, (char*)"那么接下来，尽可能多地把鱼抓上来吧！");
    outtextxy(0, 485, (char*)"按任意键开始...");
    getch();

    for(; is_run(); delay_fps(60))
    {
        mouse_msg msg = {0};
        key_msg k = {0};
        int mouse_x, mouse_y;

        //获取鼠标键盘信息
        msg = getmouse();
        if(kbmsg()) k = getkey();
        mousepos(&mouse_x, &mouse_y);

        //绘制底层
        cleardevice();
        setcolor(WHITE);
        circle(fire_x, fire_y, 50);
        setfont(18, 0, "宋体");
        outtextxy(fire_x - 15, fire_y, Myitoa(score));

        setcolor(EGERGB(157, 213, 234));
        for(int y = 30; y <= 500; y += 40)
        {
            for(int x = -10 - y % 80; x <= 500; x += 80) line(x, y, x + 40, y);
        }

        //判断鼠标方向
        setcolor(GREEN);
        double shoot_dir = mouse_x == fire_x ? pi / 2 : atan((double)(mouse_y - fire_y) / (mouse_x - fire_x));
        shoot_dir -= (mouse_x < fire_x) * pi;
        line(fire_x, fire_y, fire_x + cos(shoot_dir) * 40, fire_y + sin(shoot_dir) * 40);

        //鱼移动
        for(auto obj = fish.begin(); obj != fish.end();)
        {
            obj->move_step();
            if(obj->out()) fish.erase(obj);
            else obj++;
            obj->draw();
        }

        //子弹移动并判断是否抓到鱼
        for(auto obj = bullet.begin(); obj != bullet.end();)
        {
            obj->move_step();
            obj->draw();

            for(size_t i = 0; i < fish.size(); i++)
            {
                auto tar = fish.begin() + i;
                if(obj->iscatch(&fish[i]))
                {
                    fish.erase(tar);
                    score += rand() % 7 + 4;
                    maxfish -= 4;
                }
            }
            if(obj->out()) bullet.erase(obj);
            else obj++;
        }

        //发射
        if( (k.key == ' ' || msg.is_down()) && cd <= 0)
        {
            bullet_t temp;
            bullet.push_back(temp);
            bullet[bullet.size() - 1].init(shoot_dir);
            score -= 5;
            cd = 10;
        }
        else cd--, maxfish += !(rand() % 15) * (maxfish < (12 + (score > 0 ? score / 10 : 0) ));

        //移动
        if((k.key == 'w' || k.key == 'W') && fire_y > 25) fire_y -= 5;
        if((k.key == 'a' || k.key == 'A') && fire_x > 25) fire_x -= 5;
        if((k.key == 's' || k.key == 'S') && fire_y < 475) fire_y += 5;
        if((k.key == 'd' || k.key == 'D') && fire_x < 475) fire_x += 5;

        //鱼生成
        while((int)fish.size() < maxfish && !(rand() % 15))
        {
            fish_t temp;
            fish.push_back(temp);
            fish[fish.size() - 1].init();
        }
    }
    getch();
    return 0;
}
