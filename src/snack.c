
/***********************【程序说明】********************************/
//                文件名：snake.c 
//                介绍：win32贪吃蛇程序
//                作者：茫静无
//                时间：2022/1/13
//                版本：2.0
/******************************************************************/

//-------------------------------【头文件包含部分】-------------------------------
// 描述：引入必要的头文件
//-------------------------------------------------------------------------------
//windows编程嘛，当然需要包含windows头文件，以便使用windosAPI函数编程
#include <windows.h> 
#include <stdlib.h> 

//-------------------------------【宏定义部分】------------------------------------
// 描述：定义一些辅助的宏
//--------------------------------------------------------------------------------
//定义两宏表示窗口的大小
#define  WND_WIDTH   800
#define  WND_HEIGHT  600

//定义表示界面元素的宏
#define   WALL      -2   //表示墙体
#define   EMPTY      0   //表示没有物体的位置
#define   HEAD       1   //表示蛇头

//-------------------------------【全局结构体声明部分】----------------------------
// 描述：声明一些结构体
//--------------------------------------------------------------------------------
//声明一个蛇的相关信息的SNAKE结构体
typedef struct{
    int head_x;//蛇头在第几格
    int head_y;
    int tail_x;//蛇尾在第几格
    int tail_y;
    char direction;//蛇前进方向
}SNAKE;

//声明一个表示食物信息的结构体
typedef struct{
    int x;
    int y;
}FOOD;


//-------------------------------【WndProc()函数声明】----------------------------
// 描述：下面声明一个窗口过程（回调）函数。当事件发生时，Windows会系统调用该函数，
//我们可以为我们感兴趣的事件写上处理代码。
//--------------------------------------------------------------------------------
LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);

//-------------------------------setNewTail()函数声明】----------------------------
// 描述：找到新的蛇尾的位置，并通过第二个和第三个参数带回
//--------------------------------------------------------------------------------
void setNewTail(const int canvas[][50] ,int row,int* tail_x, int* tail_y);




//定义一个WinMain函数，类似与c语言控制台程序要定义一个main函数来作为入口函数
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, 
                    PSTR szCmdLine, int iCmdShow)
{
    /**************************************************************************/
    //Step1:创建一个窗口类，填写窗口的特征
    /**************************************************************************/
    HWND        hwnd;   //定义一个窗口句柄
    MSG         msg;    //定义一个消息结构
    WNDCLASS   wndclass;    //定义一个窗口类
    static TCHAR AppName[] = TEXT("snake"); //给我们的窗口类取一个名字
    //给窗口类各字段赋值
    wndclass.style              = CS_HREDRAW | CS_VREDRAW; //设置为窗口大小发生改变时要重新绘制
    wndclass.lpfnWndProc        = WndProc;  //为窗口类绑定窗口过程函数
    wndclass.cbClsExtra         = 0;    //设为0即可，不必深究
    wndclass.cbWndExtra         = 0;    //设为0即可
    wndclass.hInstance          = hInstance;
    //给程序设置一个默认的图标
    wndclass.hIcon = (HICON)LoadIcon(NULL,IDI_APPLICATION);
    //也可以为应用程序设定一个自定义的图标,！！！必须要严格的ico格式图标文件，与代码文件放在同一目录下!!!
    // wndclass.hIcon              = (HICON) LoadImage( NULL, TEXT("../ico/snake.ico"), IMAGE_ICON, 
                                                    // 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
    
    wndclass.hCursor            = LoadCursor (NULL, IDC_ARROW); //使用默认的鼠标指针
    //选择一个喜欢颜色的画刷，系统会用这种颜色把我们的窗口背景刷成黑色
    wndclass.hbrBackground      = (HBRUSH) GetStockObject (BLACK_BRUSH);    
    wndclass.lpszClassName      = AppName;  //前面已经取好了的名字
    /*****************************************************************************************/


    /*******************************************************************************/
    //Step2:注册窗口类，告诉Windows我们的窗口属性已经设定好了
    /*******************************************************************************/
    if ( !RegisterClass (&wndclass) )
    {
        // 如果注册失败，输出提示信息
        MessageBox (NULL, TEXT("error!!!"), TEXT("snake"), MB_ICONERROR);

        return 0;
    }

    //创建窗口
    hwnd = CreateWindow ( AppName,               //窗口类名称
                          TEXT("snake"),         //窗口的标题
                          WS_OVERLAPPEDWINDOW,   //默认这样就好了
                          CW_USEDEFAULT,         //窗口出现的x坐标,设为默认的好了
                          CW_USEDEFAULT,         //窗口出现的y坐标
                          WND_WIDTH,             //窗口的宽度
                          WND_HEIGHT,            //窗口的高度
                          NULL,                  //剩余参数，不用深究
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow (hwnd, iCmdShow);//显示窗口，这个时候实实在在可以看到我们的窗口了
    //接下来显示内容
    UpdateWindow (hwnd);
    //消息循环
    while (GetMessage (&msg, NULL, 0 ,0))
    {
        TranslateMessage (&msg);
        DispatchMessage (&msg);
    } 

    return msg.wParam;
}



/*******************************************************************************/
//step3:在窗口过程中写游戏主体代码
/*******************************************************************************/

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC                     hdc;//定义一个设备dc，方便绘图
    PAINTSTRUCT              ps;
    //将贪吃蛇界面分成60x50个小格子，创建一个二维数组存储界面信息：
    //    -2  ------------  代表墙体  ----------  WALL 
    //    -1  ------------  代表食物  ----------  FOOD
    //     0  ------------  代表无元素----------  EMPTY
    //     1  ------------  代表蛇头  ----------  HEAD
    //     2-n -----------  代表蛇身  
    static int s_canvas[60][50] = {EMPTY}; 
    static SNAKE s_snake;
    int row,col;
    HPEN            pen;
    HBRUSH          hBrush;
    static  FOOD    food;
    switch (message)
    {
        /***********************************************/
        //step4: 在WM_CREATE中进行游戏的初始化
        /************************************************/
        case WM_CREATE:
            //初始化墙体
            for (row=0; row<60; row++)
            {
                s_canvas[row][0] = -2;
                s_canvas[row][49] = -2;
            }
            for (col=0; col<50; col++)
            {
                s_canvas[0][col] = -2;
                s_canvas[59][col] = -2;
            }
            //初始化一个蛇体，方向向右
            s_canvas[33][25] = HEAD;
            s_canvas[32][25] = 2;//蛇体大于1
            s_canvas[31][25] = 3;
            s_canvas[30][25] = 4;
            s_snake.direction = 'R';
            s_snake.head_x = 33;
            s_snake.head_y = 25;
            s_snake.tail_x = 30;
            s_snake.tail_y = 25;
            //下面来手动生成一个食物
            food.x =  45;
            food.y = 45;
            s_canvas[food.x][food.y] = -1;
            SetTimer(hwnd,1,2000,NULL);//由于机制问题，这种定时方式不准，简单起见，将就着用
            break;

        /*********************************************************************/
        //step5:在timer消息中进行状态更新，不直接更新界面，而是更新s_canvas的内容
        /********************************************************************/
        case WM_TIMER:
            for (col=0; col < 50; col++)
            { 
                for (row=0; row < 60; row++) 
                {
                    if (s_canvas[row][col] > 0)
                    {
                        s_canvas[row][col] = s_canvas[row][col] + 1;
                    }
                    
                }
            }
            //边界检查
            switch(s_snake.direction)
            {
                case 'R':
                    if ( !(s_snake.head_x == 58) && !(s_canvas[s_snake.head_x+1][s_snake.head_y] > 0))//向右没撞墙也没有撞到自己
                    {
                        s_canvas[++s_snake.head_x][s_snake.head_y] = 1;//新蛇头
                    }
                    else
                    {
                        exit(0);
                    }
                    break;
                case 'D':
                    if ( !(s_snake.head_y == 48) && !(s_canvas[s_snake.head_x][s_snake.head_y+1] > 0))
                    {
                        s_canvas[s_snake.head_x][++s_snake.head_y] = 1;
                    }
                    else
                    {
                        exit(0);
                    } 
                    break;
                case 'L':
                    if ( !(s_snake.head_x == 1) && !(s_canvas[s_snake.head_x-1][s_snake.head_y] > 0) )
                    {
                        s_canvas[--s_snake.head_x][s_snake.head_y] = 1;
                    }
                    else
                    {
                        exit(0);
                    }
                    break;
                case 'U':
                    if ( !(s_snake.head_y == 1) && !(s_canvas[s_snake.head_x][s_snake.head_y-1] > 0))
                    {
                        s_canvas[s_snake.head_x][--s_snake.head_y] = 1;
                    }
                    else
                    {
                        exit(0);
                    }
                    break;
            }
            
            if ( !(s_snake.head_x == food.x && s_snake.head_y == food.y) )//如果蛇没有吃到食物
            {
                //设置新蛇尾的位置
                s_canvas[s_snake.tail_x][s_snake.tail_y] = 0;
                setNewTail(s_canvas,60,&s_snake.tail_x,&s_snake.tail_y);
            }
            else 
            {
                //生成一个新的食物
                food.x = rand() % 58 + 1;
                food.y = rand() % 48 + 1;
                while ( s_canvas[food.x][food.y] > 0 )
                {
                    food.x = rand() % 48 + 1;
                }
                s_canvas[food.x][food.y] = -1;
            }
            //间接发送WM_PAINT消息
            //第三个参数为FALSE表示重绘时不擦除原来显示的内容，可以防止画面闪烁
            InvalidateRect(hwnd,NULL,TRUE);
            break;

        //step6:在此处进行游戏界面的绘制
        case WM_PAINT://在该消息中根据s_canvas的内容进行界面的绘制
            //应在1ms之内完成绘制
            hdc = BeginPaint (hwnd, &ps);//绘图从此处开始
            pen = CreatePen(PS_INSIDEFRAME,1,RGB(255,0,0));//创建红色画笔
            SelectObject (hdc,pen);//将画笔选入当前设备
            //绘制界面
            
            hBrush = CreateSolidBrush(RGB(0,255,0));//创建绿色画刷
            SelectObject(hdc,hBrush);//将画刷选入当前设备
            for (col=0; col < 50; col++)
            {
                for (row=0; row < 60; row++)   
                {
                    Sleep(15);
                    //绘制蛇身
                    if (s_canvas[row][col] > 1)
                    {
                        Rectangle(hdc,row*10,col*10,row*10+10,col*10+10);
                        
                    }
                    //绘制蛇头
                    if (s_canvas[row][col] == 1)
                    {
                        Ellipse(hdc,row*10,col*10,row*10+10,col*10+10);
                    }
                    //绘制食物
                    if (s_canvas[row][col] == -1 )
                    {
                        Ellipse(hdc,row*10,col*10,row*10+10,col*10+10);
                    }
                    //绘制墙体
                    if (s_canvas[row][col] == -2)
                    {
                        Rectangle(hdc,row*10,col*10,row*10+10,col*10+10);
                    }    
                }
            }
            EndPaint (hwnd, &ps);//绘图结束
            break;

        case WM_KEYDOWN://在键盘按下消息中处理蛇前进方向的设置
            switch(wParam)
            {
                case  'W'://此处的大写W只代表键盘上的字母键W被按下，与你输入时的大小写状态无关
                case VK_UP://设置蛇的运动方向为向上
                    if ( s_snake.direction  != 'D')//做判断是为了防止蛇出现反向运动
                    {
                        s_snake.direction = 'U';
                    }
                    break;
                
                case   'S':
                case VK_DOWN://设置运动方向向下
                    if ( s_snake.direction != 'U')
                    {
                        s_snake.direction = 'D';
                    }
                    break;

                case   'A':
                case VK_LEFT://设置运动方向向左
                    if ( s_snake.direction != 'R')
                    {
                        s_snake.direction = 'L';
                    }
                    break;

                case  'D':
                case VK_RIGHT://设置运动方向向右
                    if ( s_snake.direction != 'L')
                    {
                        s_snake.direction = 'R';
                    }
                    
                    break;  
            }

            break;

        case WM_DESTROY://在此处进行善后工作

            PostQuitMessage(0);//保证程序彻底退出
            break;
        
    }
    return DefWindowProc (hwnd, message, wParam, lParam);
}

//找到新蛇尾的位置
void setNewTail(const int canvas[][50] , int row, int* tail_x, int* tail_y)
{
    int i,j;
    int x,y;
    if ( *tail_x > 1 && *tail_x < 58 && *tail_y > 1 && *tail_y < 48 )
    {
        x = *tail_x;
        y = *tail_y;
        for (i=x-1;i<=x+1;i++)
        {
            for (j=y-1;j<=y+1;j++)
            {
                if ( canvas[i][j] > canvas[*tail_x][*tail_y])
                {
                    *tail_x = i;
                    *tail_y = j;
                }               
            }
        }
    }


    

}

