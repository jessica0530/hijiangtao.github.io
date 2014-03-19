#include <windows.h>		// Windows的头文件
#include <gl/GL.h>
#include <gl/GLU.h>
#include <gl/glut.h>		// 包含OpenGL实用库
#include <stdio.h>			// 标准输入/输出库的头文件
#include <gl/GLAUX.H>		// 包含GLaux库的头文件

#define	MAX_PARTICLES	1000// 定义最大的粒子数
bool	rainbow=true;	// 是否为彩虹模式
bool	sp;				// 空格键是否被按下
bool	rp;				// 回车键是否被按下

float	slowdown=2.0f;			// 减速粒子
float	xspeed;				// X方向的速度
float	yspeed;				// Y方向的速度
float	zoom=-40.0f;			// 沿Z轴缩放
GLuint	loop;				// 循环变量
GLuint	col;				// 当前的颜色
GLuint	delay;				// 彩虹效果延迟

GLfloat		xrot;			// X 旋转量
GLfloat		yrot;			// Y 旋转量
GLfloat		zrot;			// Z 旋转量
GLuint		texture[1];		// 存储一个纹理

HGLRC           hRC=NULL;	// 窗口着色描述表句柄
HDC             hDC=NULL;	// OpenGL渲染描述表句柄
HWND            hWnd=NULL;	// 保存我们的窗口句柄
HINSTANCE       hInstance;	// 保存程序的实例

bool	keys[256];			// 保存键盘按键的数组
bool	active=TRUE;		// 窗口的活动标志，缺省为TRUE
bool	fullscreen=TRUE;	// 全屏标志缺省，缺省设定成全屏模式

LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);		// WndProc的定义

GLfloat		rtri;						// 用于三角形的角度
GLfloat		rquad;						// 用于四边形的角度

typedef struct					// 创建粒子数据结构
{
	bool	active;				// 是否激活
	float	life;				// 粒子生命
	float	fade;				// 衰减速度

	float	r;					// 红色值
	float	g;					// 绿色值
	float	b;					// 蓝色值

	float	x;					// X 位置
	float	y;					// Y 位置
	float	z;					// Z 位置

	float	xi;					// X 方向
	float	yi;					// Y 方向
	float	zi;					// Z 方向

	float	xg;					// X 方向重力加速度
	float	yg;					// Y 方向重力加速度
	float	zg;					// Z 方向重力加速度
}particles;						// 粒子数据结构
particles particle[MAX_PARTICLES];				// 保存1000个粒子的数组

static GLfloat colors[12][3]=				// 彩虹颜色
{
	{1.0f,0.5f,0.5f},{1.0f,0.75f,0.5f},{1.0f,1.0f,0.5f},{0.75f,1.0f,0.5f},
	{0.5f,1.0f,0.5f},{0.5f,1.0f,0.75f},{0.5f,1.0f,1.0f},{0.5f,0.75f,1.0f},
	{0.5f,0.5f,1.0f},{0.75f,0.5f,1.0f},{1.0f,0.5f,1.0f},{1.0f,0.5f,0.75f}
};

AUX_RGBImageRec *LoadBMP(char *Filename)	// 载入位图图象
{
	FILE *File=NULL;						// 文件句柄
	if (!Filename)							// 确保文件名已提供
	{
		return NULL;						// 如果没提供，返回 NULL
	}

	File=fopen(Filename,"r");				// 尝试打开文件
	if (File)								// 文件存在么?
	{
		fclose(File);						// 关闭句柄
		return auxDIBImageLoad(Filename);	// 载入位图并返回指针
	}

	return NULL;							// 如果载入失败，返回 NULL
}

int LoadGLTextures()								// 载入位图(调用上面的代码)并转换成纹理
{
	int Status=FALSE;							// 状态指示器
	AUX_RGBImageRec *TextureImage[1];					// 创建纹理的存储空间
	memset(TextureImage,0,sizeof(void *)*1);				// 将指针设为 NULL

	// 载入位图，检查有无错误，如果位图没找到则退出
	if (TextureImage[0]=LoadBMP("Particle.bmp"))	// 载入粒子纹理
	{
		Status=TRUE;							// 将 Status 设为 TRUE
		glGenTextures(1, &texture[0]);					// 创建纹理

		// 使用来自位图数据生成 的典型纹理
		glBindTexture(GL_TEXTURE_2D, texture[0]);

		// 生成纹理
		glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);

		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	// 线形滤波
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	// 线形滤波
	}

	if (TextureImage[0])							// 纹理是否存在
	{
		if (TextureImage[0]->data)					// 纹理图像是否存在
		{
			free(TextureImage[0]->data);				// 释放纹理图像占用的内存
		}

		free(TextureImage[0]);						// 释放图像结构
	}
	return Status;								// 返回 Status
}

int InitGL(GLvoid)								// 此处开始对OpenGL进行所有设置
{
	if (!LoadGLTextures())							// 调用纹理载入子例程
	{
		return FALSE;							// 如果未能载入，返回FALSE
	}

	glDisable(GL_DEPTH_TEST);						//禁止深度测试

	for (loop=0;loop<MAX_PARTICLES;loop++)			//初始化所有的粒子
	{
		particle[loop].active=true;					// 使所有的粒子为激活状态
		particle[loop].life=1.0f;					// 所有的粒子生命值为最大
		particle[loop].fade=float(rand()%100)/1000.0f+0.003f;// 随机生成衰减速率

		particle[loop].r=colors[loop*(12/MAX_PARTICLES)][0];		// 粒子的红色颜色
		particle[loop].g=colors[loop*(12/MAX_PARTICLES)][1];		// 粒子的绿色颜色
		particle[loop].b=colors[loop*(12/MAX_PARTICLES)][2];		// 粒子的蓝色颜色

		particle[loop].xi=float((rand()%50)-26.0f)*10.0f;		// 随机生成X轴方向速度
		particle[loop].yi=float((rand()%50)-25.0f)*10.0f;		// 随机生成Y轴方向速度
		particle[loop].zi=float((rand()%50)-25.0f)*10.0f;		// 随机生成Z轴方向速度

		particle[loop].xg=0.0f;						// 设置X轴方向加速度为0
		particle[loop].yg=-0.8f;						//  设置Y轴方向加速度为-0.8
		particle[loop].zg=0.0f;						//  设置Z轴方向加速度为0
	}

	glEnable(GL_TEXTURE_2D);						// 启用纹理映射
	glShadeModel(GL_SMOOTH);						// 启用阴影平滑
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);					// 黑色背景
	glClearDepth(1.0f);							// 设置深度缓存
	glEnable(GL_DEPTH_TEST);						// 启用深度测试
	glDepthFunc(GL_LEQUAL);							// 所作深度测试的类型
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);			// 真正精细的透视修正
	return TRUE;								// 初始化 OK
}

GLvoid ReSizeGLScene(GLsizei width, GLsizei height)// 重置OpenGL窗口大小
{
	if (height==0)							// 防止被零除
	{
		height=1;							// 将Height设为1
	}

	glViewport(0, 0, width, height);		// 重置当前的视口
	glMatrixMode(GL_PROJECTION);			// 选择投影矩阵
	glLoadIdentity();						// 重置投影矩阵

	// 设置视口的大小
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);

	glMatrixMode(GL_MODELVIEW);				// 选择模型观察矩阵
	glLoadIdentity();						// 重置模型观察矩阵
}

int DrawGLScene(GLvoid)			// 绘制粒子
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);// 以黑色背景清楚
	glLoadIdentity();			// 重置模型变换矩阵

	for (loop=0;loop<MAX_PARTICLES;loop++)					// 循环所有的粒子
	{
		if (particle[loop].active)					// 如果粒子为激活的
		{
			float x=particle[loop].x;				// 返回X轴的位置
			float y=particle[loop].y;				// 返回Y轴的位置
			float z=particle[loop].z+zoom;			// 返回Z轴的位置
			// 设置粒子颜色
			glColor4f(particle[loop].r,particle[loop].g,particle[loop].b,particle[loop].life);
			glBegin(GL_TRIANGLE_STRIP);				// 绘制三角形带
				glTexCoord2d(1,1); glVertex3f(x+0.5f,y+0.5f,z); 
				glTexCoord2d(0,1); glVertex3f(x-0.5f,y+0.5f,z); 
				glTexCoord2d(1,0); glVertex3f(x+0.5f,y-0.5f,z); 
				glTexCoord2d(0,0); glVertex3f(x-0.5f,y-0.5f,z); 
			glEnd();

			particle[loop].x+=particle[loop].xi/(slowdown*1000);	// 更新X坐标的位置
			particle[loop].y+=particle[loop].yi/(slowdown*1000);	// 更新Y坐标的位置
			particle[loop].z+=particle[loop].zi/(slowdown*1000);	// 更新Z坐标的位置

			particle[loop].xi+=particle[loop].xg;			// 更新X轴方向速度大小
			particle[loop].yi+=particle[loop].yg;			// 更新Y轴方向速度大小
			particle[loop].zi+=particle[loop].zg;			// 更新Z轴方向速度大小

			particle[loop].life-=particle[loop].fade;		// 减少粒子的生命值

			if (particle[loop].life<0.0f)					// 如果粒子生命值小于0
			{
				particle[loop].life=1.0f;				// 产生一个新的粒子
				particle[loop].fade=float(rand()%100)/1000.0f+0.003f;	// 随机生成衰减速率

				particle[loop].x=0.0f;					// 新粒子出现在屏幕的中央
				particle[loop].y=0.0f;					
				particle[loop].z=0.0f;					

				particle[loop].xi=xspeed+float((rand()%60)-32.0f);	// 随机生成粒子速度
				particle[loop].yi=yspeed+float((rand()%60)-30.0f);	
				particle[loop].zi=float((rand()%60)-30.0f);		

				particle[loop].r=colors[col][0];			// 设置粒子颜色
				particle[loop].g=colors[col][1];			
				particle[loop].b=colors[col][2];			
			}

			// 如果小键盘8被按住，增加Y轴方向的加速度
			if (keys[VK_NUMPAD8] && (particle[loop].yg<1.5f)) particle[loop].yg+=0.01f;

			// 如果小键盘2被按住，减少Y轴方向的加速度
			if (keys[VK_NUMPAD2] && (particle[loop].yg>-1.5f)) particle[loop].yg-=0.01f;

			// 如果小键盘6被按住，增加X轴方向的加速度
			if (keys[VK_NUMPAD6] && (particle[loop].xg<1.5f)) particle[loop].xg+=0.01f;

			// 如果小键盘4被按住，减少X轴方向的加速度
			if (keys[VK_NUMPAD4] && (particle[loop].xg>-1.5f)) particle[loop].xg-=0.01f;

			if (keys[VK_TAB])						// 按Tab键，使粒子回到原点
			{
				particle[loop].x=0.0f;					
				particle[loop].y=0.0f;					
				particle[loop].z=0.0f;					
				particle[loop].xi=float((rand()%50)-26.0f)*10.0f;	// 随机生成速度
				particle[loop].yi=float((rand()%50)-25.0f)*10.0f;	
				particle[loop].zi=float((rand()%50)-25.0f)*10.0f;	
			}
		}
    }
	return TRUE;									// 绘制完毕成功返回
}

GLvoid KillGLWindow(GLvoid)					// 正常销毁窗口
{
	if (fullscreen)
	{
		ChangeDisplaySettings(NULL, 0);		// 是的话，切换回桌面
		ShowCursor(TRUE);					// 显示鼠标指针
	}

	if (hRC)								// 我们拥有OpenGL渲染描述表吗?
	{
		if (!wglMakeCurrent(NULL, NULL))	// 我们能否释放DC和RC描述表?
		{
			MessageBox(NULL,  "释放DC或RC失败。",  "关闭错误", MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))			// 我们能否删除RC?
		{
			MessageBox(NULL, "释放RC失败。",  "关闭错误", MB_OK | MB_ICONINFORMATION);
		}

		hRC=NULL;							// 将RC设为 NULL
	}

	if (hDC && !ReleaseDC(hWnd,hDC))		// 我们能否释放 DC?
	{
		MessageBox(NULL, "释放DC失败。",  "关闭错误", MB_OK | MB_ICONINFORMATION);
		hDC=NULL;							// 将 DC 设为 NULL
	}

	if (hWnd && !DestroyWindow(hWnd))		// 能否销毁窗口?
	{
		MessageBox(NULL, "释放窗口句柄失败。",  "关闭错误", MB_OK | MB_ICONINFORMATION);
		hWnd=NULL;							// 将 hWnd 设为 NULL
	}
	
	if (!UnregisterClass( "OpenG",hInstance))// 能否注销类?
	{
		MessageBox(NULL, "不能注销窗口类。", "关闭错误",MB_OK | MB_ICONINFORMATION);
		hInstance=NULL;						// 将 hInstance 设为 NULL
	}
}

BOOL CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag)
{
	GLuint		PixelFormat;						// 保存查找匹配的结果
	WNDCLASS	wc;									// 窗口类结构
	DWORD		dwExStyle;							// 扩展窗口风格
	DWORD		dwStyle;							// 窗口风格

	RECT WindowRect;								// 取得矩形的左上角和右下角的坐标值
	WindowRect.left=(long)0;						// 将Left   设为 0
	WindowRect.right=(long)width;					// 将Right  设为要求的宽度
	WindowRect.top=(long)0;							// 将Top    设为 0
	WindowRect.bottom=(long)height;					// 将Bottom 设为要求的高度

	fullscreen=fullscreenflag;						// 设置全局全屏标志

	hInstance		= GetModuleHandle(NULL);		// 取得我们窗口的实例
	wc.style		= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;// 移动时重画，并为窗口取得DC
	wc.lpfnWndProc		= (WNDPROC) WndProc;			// WndProc处理消息
	wc.cbClsExtra		= 0;						// 无额外窗口数据
	wc.cbWndExtra		= 0;						// 无额外窗口数据
	wc.hInstance		= hInstance;				// 设置实例
	wc.hIcon		= LoadIcon(NULL, IDI_WINLOGO);	// 装入缺省图标
	wc.hCursor		= LoadCursor(NULL, IDC_ARROW);	// 装入鼠标指针
	wc.hbrBackground	= NULL;						// GL不需要背景
	wc.lpszMenuName		= NULL;						// 不需要菜单
	wc.lpszClassName	=  "OpenG";					// 设定类名字

	if (!RegisterClass(&wc))						// 尝试注册窗口类
	{
		MessageBox(NULL, "注册窗口失败", "错误",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// 退出并返回FALSE
	}

	if (fullscreen)									// 要尝试全屏模式吗?
	{
		DEVMODE dmScreenSettings;					// 设备模式
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));// 确保内存清空为零
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);// Devmode 结构的大小
		dmScreenSettings.dmPelsWidth	= width;	// 所选屏幕宽度
		dmScreenSettings.dmPelsHeight	= height;	// 所选屏幕高度
		dmScreenSettings.dmBitsPerPel	= bits;		// 每象素所选的色彩深度
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		// 尝试设置显示模式并返回结果。注: CDS_FULLSCREEN 移去了状态条。
		if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		{
			// 若模式失败，提供两个选项：退出或在窗口内运行。
			if (MessageBox(NULL, "全屏模式在当前显卡上设置失败！\n使用窗口模式？", "NeHe G",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
			{
				fullscreen=FALSE;				// 选择窗口模式(Fullscreen=FALSE)
			}
			else
			{
				// 弹出一个对话框，告诉用户程序结束
				MessageBox(NULL, "程序将被关闭", "错误",MB_OK|MB_ICONSTOP);
				return FALSE;					//  退出并返回 FALSE
			}
		}
	}

	if (fullscreen)								// 仍处于全屏模式吗?
	{
		dwExStyle=WS_EX_APPWINDOW;				// 扩展窗体风格
		dwStyle=WS_POPUP;						// 窗体风格
		ShowCursor(FALSE);						// 隐藏鼠标指针
	}
	else
	{
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;	// 扩展窗体风格
		dwStyle=WS_OVERLAPPEDWINDOW;					//  窗体风格
	}
	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);// 调整窗口达到真正要求的大小

	if (!(hWnd=CreateWindowEx(	dwExStyle,		// 扩展窗体风格
					 "OpenG",					// 类名字
					 title,						// 窗口标题
					WS_CLIPSIBLINGS |			// 必须的窗体风格属性
					WS_CLIPCHILDREN |			// 必须的窗体风格属性
					dwStyle,					// 选择的窗体属性
					0, 0,						// 窗口位置
					WindowRect.right-WindowRect.left,	// 计算调整好的窗口宽度
					WindowRect.bottom-WindowRect.top,	// 计算调整好的窗口高度
					NULL,						// 无父窗口
					NULL,						// 无菜单
					hInstance,					// 实例
					NULL)))						// 不向WM_CREATE传递任何东东
	{
		KillGLWindow();							// 重置显示区
		MessageBox(NULL, "不能创建一个窗口设备描述表", "错误",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// 返回 FALSE
	}

	static	PIXELFORMATDESCRIPTOR pfd=			// /pfd 告诉窗口我们所希望的东东，即窗口使用的像素格式
	{
		sizeof(PIXELFORMATDESCRIPTOR),			// 上述格式描述符的大小
		1,										// 版本号
		PFD_DRAW_TO_WINDOW |					// 格式支持窗口
		PFD_SUPPORT_OPENGL |					// 格式必须支持OpenGL
		PFD_DOUBLEBUFFER,						// 必须支持双缓冲
		PFD_TYPE_RGBA,							// 申请 RGBA 格式
		bits,									// 选定色彩深度
		0, 0, 0, 0, 0, 0,						// 忽略的色彩位
		0,										// 无Alpha缓存
		0,										// 忽略Shift Bit
		0,										// 无累加缓存
		0, 0, 0, 0,								// 忽略聚集位
		16,										// 16位 Z-缓存 (深度缓存)
		0,										// 无蒙板缓存
		0,										// 无辅助缓存
		PFD_MAIN_PLANE,							// 主绘图层
		0,										// Reserved
		0, 0, 0									// 忽略层遮罩
	};

	if (!(hDC=GetDC(hWnd)))						// 取得设备描述表了么?
	{
		KillGLWindow();							// 重置显示区
		MessageBox(NULL, "不能创建一种相匹配的像素格式", "错误",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// 返回 FALSE
	}

	if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))// Windows 找到相应的象素格式了吗?
	{
		KillGLWindow();							// 重置显示区
		MessageBox(NULL, "不能设置像素格式", "错误",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// 返回 FALSE
	}

	if(!SetPixelFormat(hDC,PixelFormat,&pfd))	// 能够设置象素格式么?
	{
		KillGLWindow();							// 重置显示区
		MessageBox(NULL, "不能设置像素格式", "错误",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// 返回 FALSE
	}

	if (!(hRC=wglCreateContext(hDC)))			// 能否取得着色描述表?
	{
		KillGLWindow();							// 重置显示区
		MessageBox(NULL, "不能创建OpenGL渲染描述表", "错误",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// 返回 FALSE
	}

	if (!(hRC=wglCreateContext(hDC)))			// 能否取得着色描述表?
	{
		KillGLWindow();							// 重置显示区
		MessageBox(NULL, "不能创建OpenGL渲染描述表", "错误",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// 返回 FALSE
	}

	if(!wglMakeCurrent(hDC,hRC))				// 尝试激活着色描述表
	{
		KillGLWindow();							// 重置显示区
		MessageBox(NULL, "不能激活当前的OpenGL渲然描述表", "错误",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// 返回 FALSE
	}

	ShowWindow(hWnd,SW_SHOW);					// 显示窗口
	SetForegroundWindow(hWnd);					// 略略提高优先级
	SetFocus(hWnd);								// 设置键盘的焦点至此窗口
	ReSizeGLScene(width, height);				// 设置透视 GL 屏幕

	if (!InitGL())								// 初始化新建的GL窗口
	{
		KillGLWindow();							// 重置显示区
		MessageBox(NULL, "Initialization Failed.", "ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// 返回 FALSE
	}

	return TRUE;								// 成功
}

LRESULT CALLBACK WndProc(	HWND	hWnd,		// 窗口的句柄
				UINT	uMsg,					// 窗口的消息
				WPARAM	wParam,					// 附加的消息内容
				LPARAM	lParam)					// 附加的消息内容
{
	switch (uMsg)								// 检查Windows消息
	{
		case WM_ACTIVATE:						// 监视窗口激活消息
		{
			if (!HIWORD(wParam))				// 检查最小化状态
			{
				active=TRUE;					// 程序处于激活状态
			}
			else
			{
				active=FALSE;					// 程序不再激活
			}

			return 0;							// 返回消息循环
		}

		case WM_SYSCOMMAND:						// 系统中断命令
		{
			switch (wParam)						// 检查系统调用
			{
				case SC_SCREENSAVE:				// 屏保要运行?
				case SC_MONITORPOWER:			// 显示器要进入节电模式?
				return 0;						// 阻止发生
			}
			break;								// 退出
		}

		case WM_CLOSE:							// 收到Close消息?
		{
			PostQuitMessage(0);					// 发出退出消息
			return 0;							// 返回
		}

		case WM_KEYDOWN:						// 有键按下么?
		{
			keys[wParam] = TRUE;				// 如果是，设为TRUE
			return 0;							// 返回
		}

		case WM_KEYUP:							// 有键放开么?
		{
			keys[wParam] = FALSE;				// 如果是，设为FALSE
			return 0;							// 返回
		}

		case WM_SIZE:							// 调整OpenGL窗口大小
		{
			ReSizeGLScene(LOWORD(lParam),HIWORD(lParam));// LoWord=Width,HiWord=Height
			return 0;							// 返回
		}
	}

	// 向 DefWindowProc传递所有未处理的消息。
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

int WINAPI WinMain(	HINSTANCE	hInstance,		// 当前窗口实例
			HINSTANCE	hPrevInstance,			// 前一个窗口实例
			LPSTR		lpCmdLine,				// 命令行参数
			int		nCmdShow)					// 窗口显示状态
{
	MSG	msg;									// Windowsx消息结构
	BOOL	done=FALSE;							// 用来退出循环的Bool 变量
	
	// 提示用户选择运行模式
	if (MessageBox(NULL,"你想在全屏模式下运行么？",  "设置全屏模式",MB_YESNO|MB_ICONQUESTION)==IDNO)
	{
		fullscreen=FALSE;						// FALSE为窗口模式
	}

	// 创建OpenGL窗口
	if (!CreateGLWindow("OpenGL粒子系统",640,480,16,fullscreen))
	{
		return 0;								// 失败退出
	}

	while(!done)								// 保持循环直到 done=TRUE
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))// 有消息在等待吗?
		{
			if (msg.message==WM_QUIT)			// 收到退出消息?
			{
				done=TRUE;						// 是，则done=TRUE
			}
			else								// 不是，处理窗口消息
			{
				TranslateMessage(&msg);			// 翻译消息
				DispatchMessage(&msg);			// 发送消息
			}
		}
		else									// 如果没有消息
		{
			// 绘制场景。监视ESC键和来自DrawGLScene()的退出消息
			if (active)							// 程序激活的么?
			{
				if (keys[VK_ESCAPE])			// ESC 按下了么?
				{
					done=TRUE;					// ESC 发出退出信号
				}
				else							// 不是退出的时候，刷新屏幕
				{
					DrawGLScene();				// 绘制场景
					SwapBuffers(hDC);			// 交换缓存 (双缓存)
				}
			}

			if (keys[VK_F1])					// F1键按下了么?
			{
				keys[VK_F1]=FALSE;				// 若是，使对应的Key数组中的值为 FALSE
				KillGLWindow();					// 销毁当前的窗口
				fullscreen=!fullscreen;				// 切换 全屏 / 窗口 模式
				// 重建 OpenGL 窗口(修改)
				if (!CreateGLWindow("OpenGL粒子系统",640,480,16,fullscreen))
				{
					return 0;				// 如果窗口未能创建，程序退出
				}
			}

			if (keys[VK_ADD] && (slowdown>1.0f)) slowdown-=0.01f;		// 按+号，加速粒子
			if (keys[VK_SUBTRACT] && (slowdown<4.0f)) slowdown+=0.01f;	// 按-号，减速粒子
			if (keys[VK_PRIOR]) zoom+=0.1f;		// 按Page Up键，让粒子靠近视点
			if (keys[VK_NEXT]) zoom-=0.1f;		// 按Page Down，让粒子远离视点

			if (keys[VK_RETURN] && !rp)		// 按住回车键，切换彩虹模式
			{
				rp=true;
				rainbow=!rainbow;
			}
			if (!keys[VK_RETURN]) rp=false;

			if ((keys[' '] && !sp) || (rainbow && (delay>25)))	// 空格键，变换颜色
			{
				if (keys[' ']) rainbow=false;	
				sp=true;			
				delay=0;			
				col++;				
				if (col>11) col=0;
			}
			if (!keys[' '])	sp=false;		// 如果释放空格键，记录这个状态
			// 按上增加粒子Y轴正方向的速度
			if (keys[VK_UP] && (yspeed<200)) yspeed+=1.0f;

			// 按下减少粒子Y轴正方向的速度
			if (keys[VK_DOWN] && (yspeed>-200)) yspeed-=1.0f;

			// 按右增加粒子X轴正方向的速度
			if (keys[VK_RIGHT] && (xspeed<200)) xspeed+=1.0f;

			// 按左减少粒子X轴正方向的速度
			if (keys[VK_LEFT] && (xspeed>-200)) xspeed-=1.0f;

			delay++;			// 增加彩虹模式的颜色切换延迟
		}
	}
	// 关闭程序
	KillGLWindow();								// 销毁窗口
	return (msg.wParam);						// 退出程序
}
