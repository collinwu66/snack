/***********************������˵����********************************/
//                �ļ�����snake.c 
//                ���ܣ�win32̰���߳���
//                ���ߣ�ã����
//                ����ʱ�䣺2022/1/12
//                �޸��ˣ�ã����
//                ���һ���޸�ʱ�䣺2022/9/2
/******************************************************************/

//-------------------------------��ͷ�ļ��������֡�-------------------------------
// �����������Ҫ��ͷ�ļ�
//-------------------------------------------------------------------------------
//windows������Ȼ��Ҫ����windowsͷ�ļ����Ա�ʹ��windosAPI�������
#include <windows.h> 
#include <stdlib.h> 

//-------------------------------���궨�岿�֡�------------------------------------
// ����������һЩ�����ĺ�
//--------------------------------------------------------------------------------
//���������ʾ���ڵĴ�С
#define  WND_WIDTH   650
#define  WND_HEIGHT  550

//�����ʾ����Ԫ�صĺ�
#define   WALL      -2   //��ʾǽ��
#define   EMPTY      0   //��ʾû�������λ��
#define   HEAD       1   //��ʾ��ͷ

//-------------------------------��ȫ�ֽṹ���������֡�----------------------------
// ����������һЩ�ṹ��
//--------------------------------------------------------------------------------
//����һ���ߵ������Ϣ��SNAKE�ṹ��
typedef struct{
    int head_x;//��ͷ�ڵڼ���
    int head_y;
    int tail_x;//��β�ڵڼ���
    int tail_y;
    char direction;//��ǰ������
}SNAKE;

//����һ����ʾʳ����Ϣ�Ľṹ��
typedef struct{
    int x;
    int y;
}FOOD;


//-------------------------------��WndProc()����������----------------------------
// ��������������һ�����ڹ��̣��ص������������¼�����ʱ��Windows��ϵͳ���øú�����
//���ǿ���Ϊ���Ǹ���Ȥ���¼�д�ϴ�����롣
//--------------------------------------------------------------------------------
LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);

//-------------------------------setNewTail()����������----------------------------
// �������ҵ��µ���β��λ�ã���ͨ���ڶ����͵�������������
//--------------------------------------------------------------------------------
void setNewTail(const int canvas[][50] ,int row,int* tail_x, int* tail_y);




//����һ��WinMain������������c���Կ���̨����Ҫ����һ��main��������Ϊ��ں���
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, 
                    PSTR szCmdLine, int iCmdShow)
{
    /**************************************************************************/
    //Step1:����һ�������࣬��д���ڵ�����
    /**************************************************************************/
    HWND        hwnd;   //����һ�����ھ��
    MSG         msg;    //����һ����Ϣ�ṹ
    WNDCLASS   wndclass;    //����һ��������
    static TCHAR AppName[] = TEXT("snake"); //�����ǵĴ�����ȡһ������
    //����������ֶθ�ֵ
    wndclass.style              = CS_HREDRAW | CS_VREDRAW; //����Ϊ���ڴ�С�����ı�ʱҪ���»���
    wndclass.lpfnWndProc        = WndProc;  //Ϊ������󶨴��ڹ��̺���
    wndclass.cbClsExtra         = 0;    //��Ϊ0���ɣ������
    wndclass.cbWndExtra         = 0;    //��Ϊ0����
    wndclass.hInstance          = hInstance;
    //����������һ��Ĭ�ϵ�ͼ��
    // wndclass.hIcon = (HICON)LoadIcon(NULL,IDI_APPLICATION);
    // Ҳ����ΪӦ�ó����趨һ���Զ����ͼ��,����������Ҫ�ϸ��ico��ʽͼ���ļ���������ļ�����ͬһĿ¼��!!!
    wndclass.hIcon              = (HICON) LoadImage( NULL, TEXT("../ico/snake.ico"), IMAGE_ICON, 
                                                    0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
    
    wndclass.hCursor            = LoadCursor (NULL, IDC_ARROW); //ʹ��Ĭ�ϵ����ָ��
    //ѡ��һ��ϲ����ɫ�Ļ�ˢ��ϵͳ����������ɫ�����ǵĴ��ڱ���ˢ�ɺ�ɫ
    wndclass.hbrBackground      = (HBRUSH) GetStockObject (BLACK_BRUSH);    
    wndclass.lpszClassName      = AppName;  //ǰ���Ѿ�ȡ���˵�����
    /*****************************************************************************************/


    /*******************************************************************************/
    //Step2:ע�ᴰ���࣬����Windows���ǵĴ��������Ѿ��趨����
    /*******************************************************************************/
    if ( !RegisterClass (&wndclass) )
    {
        // ���ע��ʧ�ܣ������ʾ��Ϣ
        MessageBox (NULL, TEXT("error!!!"), TEXT("snake"), MB_ICONERROR);

        return 0;
    }

    //��������
    hwnd = CreateWindow ( AppName,               //����������
                          TEXT("snake"),         //���ڵı���
                          WS_OVERLAPPEDWINDOW,   //Ĭ�������ͺ���
                          CW_USEDEFAULT,         //���ڳ��ֵ�x����,��ΪĬ�ϵĺ���
                          CW_USEDEFAULT,         //���ڳ��ֵ�y����
                          WND_WIDTH,             //���ڵĿ��
                          WND_HEIGHT,            //���ڵĸ߶�
                          NULL,                  //ʣ������������
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow (hwnd, iCmdShow);//��ʾ���ڣ����ʱ��ʵʵ���ڿ��Կ������ǵĴ�����
    //��������ʾ����
    UpdateWindow (hwnd);
    //��Ϣѭ��
    while (GetMessage (&msg, NULL, 0 ,0))
    {
        TranslateMessage (&msg);
        DispatchMessage (&msg);
    } 

    return msg.wParam;
}



/*******************************************************************************/
//step3:�ڴ��ڹ�����д��Ϸ�������
/*******************************************************************************/

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC                     hdc;//����һ���豸dc�������ͼ
    PAINTSTRUCT              ps;
    static RECT            rect;
    //��̰���߽���ֳ�60x50��С���ӣ�����һ����ά����洢������Ϣ��
    //    -2  ------------  ����ǽ��  ----------  WALL 
    //    -1  ------------  ����ʳ��  ----------  FOOD
    //     0  ------------  ������Ԫ��----------  EMPTY
    //     1  ------------  ������ͷ  ----------  HEAD
    //     2-n -----------  ��������  
    static int s_canvas[60][50] = {EMPTY}; 
    static SNAKE s_snake;
    int row,col;
    HPEN            pen;
    HBRUSH          hBrush;
    static  FOOD    food;
    switch (message)
    {
        /***********************************************/
        //step4: ��WM_CREATE�н�����Ϸ�ĳ�ʼ��
        /************************************************/
        case WM_CREATE:
            GetClientRect(hwnd,&rect);
            //��ʼ��ǽ��
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
            //��ʼ��һ�����壬��������
            s_canvas[33][25] = HEAD;
            s_canvas[32][25] = 2;//�������1
            s_canvas[31][25] = 3;
            s_canvas[30][25] = 4;
            s_snake.direction = 'R';
            s_snake.head_x = 33;
            s_snake.head_y = 25;
            s_snake.tail_x = 30;
            s_snake.tail_y = 25;
            //�������ֶ�����һ��ʳ��
            food.x =  45;
            food.y = 45;
            s_canvas[food.x][food.y] = -1;
            SetTimer(hwnd,1,300,NULL);//���ڻ������⣬���ֶ�ʱ��ʽ��׼�����������������
            break;

        /*********************************************************************/
        //step5:��timer��Ϣ�н���״̬���£���ֱ�Ӹ��½��棬���Ǹ���s_canvas������
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
            //�߽���
            switch(s_snake.direction)
            {
                case 'R':
                    if ( !(s_snake.head_x == 58) && !(s_canvas[s_snake.head_x+1][s_snake.head_y] > 0))//����ûײǽҲû��ײ���Լ�
                    {
                        s_canvas[++s_snake.head_x][s_snake.head_y] = 1;//����ͷ
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
            
            if ( !(s_snake.head_x == food.x && s_snake.head_y == food.y) )//�����û�гԵ�ʳ��
            {
                //��������β��λ��
                s_canvas[s_snake.tail_x][s_snake.tail_y] = 0;
                setNewTail(s_canvas,60,&s_snake.tail_x,&s_snake.tail_y);
            }
            else 
            {
                //����һ���µ�ʳ��
                food.x = rand() % 58 + 1;
                food.y = rand() % 48 + 1;
                while ( s_canvas[food.x][food.y] > 0 )
                {
                    food.x = rand() % 48 + 1;
                }
                s_canvas[food.x][food.y] = -1;
            }
            //��ӷ���WM_PAINT��Ϣ
            //����������ΪFALSE��ʾ�ػ�ʱ������ԭ����ʾ�����ݣ����Է�ֹ������˸
            InvalidateRect(hwnd,NULL,TRUE);
            break;

        //step6:�ڴ˴�������Ϸ����Ļ���
        case WM_PAINT://�ڸ���Ϣ�и���s_canvas�����ݽ��н���Ļ���
            hdc = BeginPaint (hwnd, &ps);//��ͼ�Ӵ˴���ʼ
            pen = CreatePen(PS_INSIDEFRAME,1,RGB(255,0,0));//������ɫ����
            SelectObject (hdc,pen);//������ѡ�뵱ǰ�豸
            //���ƽ���
            hBrush = CreateSolidBrush(RGB(0,255,0));//������ɫ��ˢ
            SelectObject(hdc,hBrush);//����ˢѡ�뵱ǰ�豸
            for (col=0; col < 50; col++)
            {
                for (row=0; row < 60; row++)   
                {
                    //��������
                    if (s_canvas[row][col] > 1)
                    {
                        Rectangle(hdc,row*10,col*10,row*10+10,col*10+10);
                    }
                    //������ͷ
                    if (s_canvas[row][col] == 1)
                    {
                        Ellipse(hdc,row*10,col*10,row*10+10,col*10+10);
                    }
                    //����ʳ��
                    if (s_canvas[row][col] == -1 )
                    {
                        Ellipse(hdc,row*10,col*10,row*10+10,col*10+10);
                    }
                    //����ǽ��
                    if (s_canvas[row][col] == -2)
                    {
                        Rectangle(hdc,row*10,col*10,row*10+10,col*10+10);
                    }    
                }
            }
            EndPaint (hwnd, &ps);//��ͼ����
            break;

        case WM_KEYDOWN://�ڼ��̰�����Ϣ�д�����ǰ�����������
            switch(wParam)
            {
                case  'W'://�˴��Ĵ�дWֻ��������ϵ���ĸ��W�����£���������ʱ�Ĵ�Сд״̬�޹�
                case VK_UP://�����ߵ��˶�����Ϊ����
                    if ( s_snake.direction  != 'D')//���ж���Ϊ�˷�ֹ�߳��ַ����˶�
                    {
                        s_snake.direction = 'U';
                    }
                    break;
                
                case   'S':
                case VK_DOWN://�����˶���������
                    if ( s_snake.direction != 'U')
                    {
                        s_snake.direction = 'D';
                    }
                    break;

                case   'A':
                case VK_LEFT://�����˶���������
                    if ( s_snake.direction != 'R')
                    {
                        s_snake.direction = 'L';
                    }
                    break;

                case  'D':
                case VK_RIGHT://�����˶���������
                    if ( s_snake.direction != 'L')
                    {
                        s_snake.direction = 'R';
                    }
                    
                    break;  
            }

            break;

        case WM_DESTROY://�ڴ˴������ƺ���

            PostQuitMessage(0);//��֤���򳹵��˳�
            break;
        
    }
    return DefWindowProc (hwnd, message, wParam, lParam);
}

//�ҵ�����β��λ��
void setNewTail(const int canvas[][50] , int row, int* tail_x, int* tail_y)
{
    int r,col;
    for (r=0; r < row; r++)
    {
        for (col=0; col < 50; col++)
        {
            if (canvas[r][col] > canvas[*tail_x][*tail_y])
            {
                *tail_x = r;
                *tail_y = col;
            }
        }
    }
    
}


