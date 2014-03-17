#include <windows.h>		// Windows的头文件
#include <gl/GL.h>
#include <gl/GLU.h>
#include <gl/glut.h>		// 包含OpenGL实用库
#include <math.h>
#include <stdio.h>
#include <gl/GLAUX.H>

HGLRC           hRC=NULL;							// 窗口着色描述表句柄
HDC             hDC=NULL;							// OpenGL渲染描述表句柄
HWND            hWnd=NULL;							// 保存我们的窗口句柄
HINSTANCE       hInstance;							// 保存程序的实例

bool	keys[256];								    // 保存键盘按键的数组
bool	active=TRUE;								// 窗口的活动标志，缺省为TRUE
bool	fullscreen=TRUE;							// 全屏标志缺省，缺省设定成全屏模式

LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);		// WndProc的定义

GLuint	base;			// 绘制字体的显示列表的开始位置
GLuint	texture[2];		// 保存字体纹理
GLuint	loop;			// 通用循环变量

GLfloat	cnt1;			// 字体移动计数器1
GLfloat	cnt2;			// 字体移动计数器2

AUX_RGBImageRec *LoadBMP(char *Filename)					// 载入位图图象
{
	FILE *File=NULL;							// 文件句柄
	if (!Filename)								// 确保文件名已提供
	{
		return NULL;							// 如果没提供，返回 NULL
	}
	File=fopen(Filename,"r");						// 尝试打开文件

	if (File)								// 文件存在么?
	{
		fclose(File);							// 关闭句柄
		return auxDIBImageLoad(Filename);				// 载入位图并返回指针
	}

	return NULL;								// 如果载入失败，返回 NULL
}

int LoadGLTextures()							// 载入位图(调用上面的代码)并转换成纹理
{
	int Status=FALSE;							// 状态指示器
	AUX_RGBImageRec *TextureImage[2];					// 创建纹理的存储空间
	memset(TextureImage,0,sizeof(void *)*2);					// 将指针设为 NULL

	if ((TextureImage[0]=LoadBMP("Font.bmp")) &&			// 载入字体图像
		(TextureImage[1]=LoadBMP("Bumps.bmp")))			// 载入纹理图像
	{
		Status=TRUE;						// 将 Status 设为 TRUE
		glGenTextures(2, &texture[0]);							// 创建纹理

		for (loop=0; loop<2; loop++)					// 循环设置所有的纹理
		{
			// 生成所有纹理
			glBindTexture(GL_TEXTURE_2D, texture[loop]);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[loop]->sizeX, TextureImage[loop]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[loop]->data);
		}
	}

	for (loop=0; loop<2; loop++)
		{
	        if (TextureImage[loop])						// 纹理是否存在
			{
				if (TextureImage[loop]->data)			// 纹理图像是否存在
				{
					free(TextureImage[loop]->data);		// 释放纹理图像占用的内存
				}
				free(TextureImage[loop]);				// 释放图像结构
			}
		}
	return Status;								// 返回 Status
}

GLvoid BuildFont(GLvoid)								// 创建我们的字符显示列表
{
	float	cx;							// 字符的X坐标
	float	cy;							// 字符的Y坐标
	base=glGenLists(256);				// 创建256个显示列表
	glBindTexture(GL_TEXTURE_2D, texture[0]);// 选择字符图象

	for (loop=0; loop<256; loop++)		// 循环256个显示列表
	{
		cx=float(loop%16)/16.0f;		// 当前字符的X坐标
		cy=float(loop/16)/16.0f;		// 当前字符的Y坐标
		glNewList(base+loop,GL_COMPILE);//开始创建显示列表
		glBegin(GL_QUADS);				// 使用四边形显示每一个字符

		glTexCoord2f(cx,1-cy-0.0625f);	// 左下角的纹理坐标
		glVertex2i(0,0);				// 左下角的坐标
		glTexCoord2f(cx+0.0625f,1-cy-0.0625f);// 右下角的纹理坐标
		glVertex2i(16,0);				// 右下角的坐标
		glTexCoord2f(cx+0.0625f,1-cy);	// 右上角的纹理坐标
		glVertex2i(16,16);				// 右上角的坐标
		glTexCoord2f(cx,1-cy);			// 左上角的纹理坐标
		glVertex2i(0,16);				// 左上角的坐标
	glEnd();						// 四边形字符绘制完成
	glTranslated(10,0,0);					// 绘制完一个字符，向右平移16个单位
		glEndList();							// 字符显示列表结束
	}									// 循环建立256个显示列表
}

GLvoid KillFont(GLvoid)								
{
	glDeleteLists(base,256);						// 从内存中删除256个显示列表
}

GLvoid glPrint(GLint x, GLint y, char *string, int set)				// 绘制字符
{
	if (set>1)								// 如果字符集大于1
	{
		set=1;								// 设置其为1
	}
	glBindTexture(GL_TEXTURE_2D, texture[0]);				// 绑定为字体纹理
	glDisable(GL_DEPTH_TEST);						// 禁止深度测试
	
	glMatrixMode(GL_PROJECTION);						// 选择投影矩阵
	glPushMatrix();								// 保存当前的投影矩阵

	glLoadIdentity();								// 重置投影矩阵
	glOrtho(0,640,0,480,-1,1);							// 设置正投影的可视区域

	glMatrixMode(GL_MODELVIEW);							// 选择模型变换矩阵
	glPushMatrix();								// 保存当前的模型变换矩阵
	glLoadIdentity();								// 重置模型变换矩阵

	glTranslated(x,y,0);							// 把字符原点移动到(x,y)位置
	glListBase(base-32+(128*set));						// 选择字符集
	glCallLists(strlen(string),GL_BYTE,string);					// 把字符串写入到屏幕 
	glMatrixMode(GL_PROJECTION);						// 选择投影矩阵
	glPopMatrix();								// 设置为保存的矩阵

	glMatrixMode(GL_MODELVIEW);							// 选择模型矩阵
	glPopMatrix();								// 设置为保存的矩阵
}

GLvoid ReSizeGLScene(GLsizei width, GLsizei height)			// 重置OpenGL窗口大小
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

int InitGL(GLvoid)								// 此处开始对OpenGL进行所有设置
{
	if (!LoadGLTextures())							// 调用纹理载入子例程
	{
		return FALSE;							// 如果未能载入，返回FALSE
	}
	BuildFont();								// 创建字符显示列表
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);					// 黑色背景 
	glClearDepth(1.0);								// 设置深度缓存
	glDepthFunc(GL_LEQUAL);							// 所作深度测试的类型
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);						// 设置混合因子
	glShadeModel(GL_SMOOTH);							// 启用阴影平滑
	glEnable(GL_TEXTURE_2D);							// 启用纹理映射
	return TRUE;								// 初始化成功
}

int DrawGLScene(GLvoid)								// 从这里开始进行所有的绘制
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);				// 清除屏幕和深度缓存
	glLoadIdentity();								// 重置当前的模型观察矩阵
	glBindTexture(GL_TEXTURE_2D, texture[1]);					// 设置为图像纹理
	glTranslatef(0.0f,0.0f,-5.0f);						// 移入屏幕5个单位
	glRotatef(45.0f,0.0f,0.0f,1.0f);						// 沿Z轴旋转45度
	glRotatef(cnt1*30.0f,1.0f,1.0f,0.0f);					//  沿(1,1,0)轴旋转30度
	glDisable(GL_BLEND);							// 关闭混合
	glColor3f(1.0f,1.0f,1.0f);							//设置颜色为白色
	glBegin(GL_QUADS);								// 绘制纹理四边形
		glTexCoord2d(0.0f,0.0f);					
		glVertex2f(-1.0f, 1.0f);					
		glTexCoord2d(1.0f,0.0f);					
		glVertex2f( 1.0f, 1.0f);					
		glTexCoord2d(1.0f,1.0f);					
		glVertex2f( 1.0f,-1.0f);					
		glTexCoord2d(0.0f,1.0f);					
		glVertex2f(-1.0f,-1.0f);					
	glEnd();		
	glRotatef(90.0f,1.0f,1.0f,0.0f);					//  沿(1,1,0)轴旋转90度
	glBegin(GL_QUADS);							// 绘制第二个四边形，与第一个四边形垂直
		glTexCoord2d(0.0f,0.0f);					
		glVertex2f(-1.0f, 1.0f);					
		glTexCoord2d(1.0f,0.0f);					
		glVertex2f( 1.0f, 1.0f);					
		glTexCoord2d(1.0f,1.0f);					
		glVertex2f( 1.0f,-1.0f);					
		glTexCoord2d(0.0f,1.0f);					
		glVertex2f(-1.0f,-1.0f);					
	glEnd();		
	glEnable(GL_BLEND);							// 启用混合操作
	glLoadIdentity();								// 重置视口

	// 根据字体位置设置颜色
	glColor3f(1.0f*float(cos(cnt1)),1.0f*float(sin(cnt2)),1.0f-0.5f*float(cos(cnt1+cnt2)));
	glPrint(int((280+250*cos(cnt1))),int(235+200*sin(cnt2)),"Hello",0);	

	glColor3f(1.0f*float(sin(cnt2)),1.0f-0.5f*float(cos(cnt1+cnt2)),1.0f*float(cos(cnt1)));
	glPrint(int((280+230*cos(cnt2))),int(235+200*sin(cnt1)),"OpenGL",1);

	/*glColor3f(1.0f*float(sin(cnt2)),1.0f-0.5f*float(cos(cnt1)),1.0f*float(cos(cnt1+cnt2)));
	glPrint(int((280+250*cos(cnt2))),int(235+200*sin(cnt2)),"What",2);*/
	glColor3f(0.0f,0.0f,1.0f);						
	glPrint(int(240+200*cos((cnt2+cnt1)/5)),2,"Thx for Giuseppe D'Agata's Efforts!",0);	

	glColor3f(1.0f,1.0f,1.0f);						
	glPrint(int(242+200*cos((cnt2+cnt1)/5)),2,"Thx for Giuseppe D'Agata's Efforts!",0);	
    
	cnt1+=0.01f;								// 增加计数器值
	cnt2+=0.0081f;								// 增加计数器值
	return TRUE;								// 成功返回
}

GLvoid KillGLWindow(GLvoid)							// 正常销毁窗口
{
	if (fullscreen)
	{
		ChangeDisplaySettings(NULL, 0);				// 是的话，切换回桌面
		ShowCursor(TRUE);							// 显示鼠标指针
	}

	if (hRC)										// 我们拥有OpenGL渲染描述表吗?
	{
		if (!wglMakeCurrent(NULL, NULL))			// 我们能否释放DC和RC描述表?
		{
			MessageBox(NULL,  "释放DC或RC失败。",  "关闭错误", MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))					// 我们能否删除RC?
		{
			MessageBox(NULL, "释放RC失败。",  "关闭错误", MB_OK | MB_ICONINFORMATION);
		}

		hRC=NULL;									// 将RC设为 NULL
	}

	if (hDC && !ReleaseDC(hWnd,hDC))					// 我们能否释放 DC?
	{
		MessageBox(NULL, "释放DC失败。",  "关闭错误", MB_OK | MB_ICONINFORMATION);
		hDC=NULL;							// 将 DC 设为 NULL
	}

	if (hWnd && !DestroyWindow(hWnd))					// 能否销毁窗口?
	{
		MessageBox(NULL, "释放窗口句柄失败。",  "关闭错误", MB_OK | MB_ICONINFORMATION);
		hWnd=NULL;							// 将 hWnd 设为 NULL
	}
	
	if (!UnregisterClass( "OpenG",hInstance))				// 能否注销类?
	{
		MessageBox(NULL, "不能注销窗口类。", "关闭错误",MB_OK | MB_ICONINFORMATION);
		hInstance=NULL;							// 将 hInstance 设为 NULL
	}
	KillFont();
}

BOOL CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag)
{
	GLuint		PixelFormat;						// 保存查找匹配的结果
	WNDCLASS	wc;							// 窗口类结构
	DWORD		dwExStyle;						// 扩展窗口风格
	DWORD		dwStyle;						// 窗口风格

	RECT WindowRect;							// 取得矩形的左上角和右下角的坐标值
	WindowRect.left=(long)0;						// 将Left   设为 0
	WindowRect.right=(long)width;						// 将Right  设为要求的宽度
	WindowRect.top=(long)0;							// 将Top    设为 0
	WindowRect.bottom=(long)height;						// 将Bottom 设为要求的高度

	fullscreen=fullscreenflag;						// 设置全局全屏标志

	hInstance		= GetModuleHandle(NULL);			// 取得我们窗口的实例
	wc.style		= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;		// 移动时重画，并为窗口取得DC
	wc.lpfnWndProc		= (WNDPROC) WndProc;				// WndProc处理消息
	wc.cbClsExtra		= 0;						// 无额外窗口数据
	wc.cbWndExtra		= 0;						// 无额外窗口数据
	wc.hInstance		= hInstance;					// 设置实例
	wc.hIcon		= LoadIcon(NULL, IDI_WINLOGO);			// 装入缺省图标
	wc.hCursor		= LoadCursor(NULL, IDC_ARROW);			// 装入鼠标指针
	wc.hbrBackground	= NULL;						// GL不需要背景
	wc.lpszMenuName		= NULL;						// 不需要菜单
	wc.lpszClassName	=  "OpenG";					// 设定类名字

	if (!RegisterClass(&wc))						// 尝试注册窗口类
	{
		MessageBox(NULL, "注册窗口失败", "错误",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// 退出并返回FALSE
	}

	if (fullscreen)								// 要尝试全屏模式吗?
	{
		DEVMODE dmScreenSettings;						// 设备模式
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));			// 确保内存清空为零
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);			// Devmode 结构的大小
		dmScreenSettings.dmPelsWidth	= width;				// 所选屏幕宽度
		dmScreenSettings.dmPelsHeight	= height;				// 所选屏幕高度
		dmScreenSettings.dmBitsPerPel	= bits;					// 每象素所选的色彩深度
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
		dwExStyle=WS_EX_APPWINDOW;					// 扩展窗体风格
		dwStyle=WS_POPUP;						// 窗体风格
		ShowCursor(FALSE);						// 隐藏鼠标指针
	}
	else
	{
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// 扩展窗体风格
		dwStyle=WS_OVERLAPPEDWINDOW;					//  窗体风格
	}
	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// 调整窗口达到真正要求的大小

	if (!(hWnd=CreateWindowEx(	dwExStyle,				// 扩展窗体风格
					 "OpenG",				// 类名字
					 title,					// 窗口标题
					WS_CLIPSIBLINGS |			// 必须的窗体风格属性
					WS_CLIPCHILDREN |			// 必须的窗体风格属性
					dwStyle,				// 选择的窗体属性
					0, 0,					// 窗口位置
					WindowRect.right-WindowRect.left,	// 计算调整好的窗口宽度
					WindowRect.bottom-WindowRect.top,	// 计算调整好的窗口高度
					NULL,					// 无父窗口
					NULL,					// 无菜单
					hInstance,				// 实例
					NULL)))					// 不向WM_CREATE传递任何东东
	{
		KillGLWindow();							// 重置显示区
		MessageBox(NULL, "不能创建一个窗口设备描述表", "错误",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// 返回 FALSE
	}

	static	PIXELFORMATDESCRIPTOR pfd=					// /pfd 告诉窗口我们所希望的东东，即窗口使用的像素格式
	{
		sizeof(PIXELFORMATDESCRIPTOR),					// 上述格式描述符的大小
		1,								// 版本号
		PFD_DRAW_TO_WINDOW |						// 格式支持窗口
		PFD_SUPPORT_OPENGL |						// 格式必须支持OpenGL
		PFD_DOUBLEBUFFER,						// 必须支持双缓冲
		PFD_TYPE_RGBA,							// 申请 RGBA 格式
		bits,								// 选定色彩深度
		0, 0, 0, 0, 0, 0,						// 忽略的色彩位
		0,								// 无Alpha缓存
		0,								// 忽略Shift Bit
		0,								// 无累加缓存
		0, 0, 0, 0,							// 忽略聚集位
		16,								// 16位 Z-缓存 (深度缓存)
		0,								// 无蒙板缓存
		0,								// 无辅助缓存
		PFD_MAIN_PLANE,							// 主绘图层
		0,								// Reserved
		0, 0, 0								// 忽略层遮罩
	};

	if (!(hDC=GetDC(hWnd)))							// 取得设备描述表了么?
	{
		KillGLWindow();							// 重置显示区
		MessageBox(NULL, "不能创建一种相匹配的像素格式", "错误",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// 返回 FALSE
	}

	if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))				// Windows 找到相应的象素格式了吗?
	{
		KillGLWindow();							// 重置显示区
		MessageBox(NULL, "不能设置像素格式", "错误",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// 返回 FALSE
	}

	if(!SetPixelFormat(hDC,PixelFormat,&pfd))				// 能够设置象素格式么?
	{
		KillGLWindow();							// 重置显示区
		MessageBox(NULL, "不能设置像素格式", "错误",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// 返回 FALSE
	}

	if (!(hRC=wglCreateContext(hDC)))					// 能否取得着色描述表?
	{
		KillGLWindow();							// 重置显示区
		MessageBox(NULL, "不能创建OpenGL渲染描述表", "错误",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// 返回 FALSE
	}

	if (!(hRC=wglCreateContext(hDC)))					// 能否取得着色描述表?
	{
		KillGLWindow();							// 重置显示区
		MessageBox(NULL, "不能创建OpenGL渲染描述表", "错误",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// 返回 FALSE
	}

	if(!wglMakeCurrent(hDC,hRC))						// 尝试激活着色描述表
	{
		KillGLWindow();							// 重置显示区
		MessageBox(NULL, "不能激活当前的OpenGL渲然描述表", "错误",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// 返回 FALSE
	}

	ShowWindow(hWnd,SW_SHOW);						// 显示窗口
	SetForegroundWindow(hWnd);						// 略略提高优先级
	SetFocus(hWnd);								// 设置键盘的焦点至此窗口
	ReSizeGLScene(width, height);						// 设置透视 GL 屏幕

	if (!InitGL())								// 初始化新建的GL窗口
	{
		KillGLWindow();							// 重置显示区
		MessageBox(NULL, "Initialization Failed.", "ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// 返回 FALSE
	}

	return TRUE;								// 成功
}

LRESULT CALLBACK WndProc(	HWND	hWnd,					// 窗口的句柄
				UINT	uMsg,					// 窗口的消息
				WPARAM	wParam,					// 附加的消息内容
				LPARAM	lParam)					// 附加的消息内容
{
	switch (uMsg)								// 检查Windows消息
	{
		case WM_ACTIVATE:						// 监视窗口激活消息
		{
			if (!HIWORD(wParam))					// 检查最小化状态
			{
				active=TRUE;					// 程序处于激活状态
			}
			else
			{
				active=FALSE;					// 程序不再激活
			}

			return 0;						// 返回消息循环
		}

		case WM_SYSCOMMAND:						// 系统中断命令
		{
			switch (wParam)						// 检查系统调用
			{
				case SC_SCREENSAVE:				// 屏保要运行?
				case SC_MONITORPOWER:				// 显示器要进入节电模式?
				return 0;					// 阻止发生
			}
			break;							// 退出
		}

		case WM_CLOSE:							// 收到Close消息?
		{
			PostQuitMessage(0);					// 发出退出消息
			return 0;						// 返回
		}

		case WM_KEYDOWN:						// 有键按下么?
		{
			keys[wParam] = TRUE;					// 如果是，设为TRUE
			return 0;						// 返回
		}

		case WM_KEYUP:							// 有键放开么?
		{
			keys[wParam] = FALSE;					// 如果是，设为FALSE
			return 0;						// 返回
		}

		case WM_SIZE:							// 调整OpenGL窗口大小
		{
			ReSizeGLScene(LOWORD(lParam),HIWORD(lParam));		// LoWord=Width,HiWord=Height
			return 0;						// 返回
		}
	}

	// 向 DefWindowProc传递所有未处理的消息。
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

int WINAPI WinMain(	HINSTANCE	hInstance,				// 当前窗口实例
			HINSTANCE	hPrevInstance,				// 前一个窗口实例
			LPSTR		lpCmdLine,				// 命令行参数
			int		nCmdShow)				// 窗口显示状态
{
	MSG	msg;								// Windowsx消息结构
	BOOL	done=FALSE;							// 用来退出循环的Bool 变量
	
	// 提示用户选择运行模式
	if (MessageBox(NULL,"你想在全屏模式下运行么？",  "设置全屏模式",MB_YESNO|MB_ICONQUESTION)==IDNO)
	{
		fullscreen=FALSE;						// FALSE为窗口模式
	}

	// 创建OpenGL窗口
	if (!CreateGLWindow("OpenGL 2D PicText",640,480,16,fullscreen))
	{
		return 0;							// 失败退出
	}

	while(!done)								// 保持循环直到 done=TRUE
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))			// 有消息在等待吗?
		{
			if (msg.message==WM_QUIT)				// 收到退出消息?
			{
				done=TRUE;					// 是，则done=TRUE
			}
			else							// 不是，处理窗口消息
			{
				TranslateMessage(&msg);				// 翻译消息
				DispatchMessage(&msg);				// 发送消息
			}
		}
		else								// 如果没有消息
		{
			// 绘制场景。监视ESC键和来自DrawGLScene()的退出消息
			if (active)						// 程序激活的么?
			{
				if (keys[VK_ESCAPE])				// ESC 按下了么?
				{
					done=TRUE;				// ESC 发出退出信号
				}
				else						// 不是退出的时候，刷新屏幕
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
				// 重建 OpenGL 窗口
				if (!CreateGLWindow("OpenGL 2D PicText",640,480,16,fullscreen))
				{
					return 0;				// 如果窗口未能创建，程序退出
				}
			}
		}
	}

	// 关闭程序
	KillGLWindow();								// 销毁窗口
	return (msg.wParam);							// 退出程序
}