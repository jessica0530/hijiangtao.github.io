#include <windows.h>		// Windows��ͷ�ļ�
#include <gl/GL.h>
#include <gl/GLU.h>
#include <gl/glut.h>		// ����OpenGLʵ�ÿ�
#include <math.h>
#include <stdio.h>
#include <gl/GLAUX.H>

HGLRC           hRC=NULL;							// ������ɫ��������
HDC             hDC=NULL;							// OpenGL��Ⱦ��������
HWND            hWnd=NULL;							// �������ǵĴ��ھ��
HINSTANCE       hInstance;							// ��������ʵ��

bool	keys[256];								    // ������̰���������
bool	active=TRUE;								// ���ڵĻ��־��ȱʡΪTRUE
bool	fullscreen=TRUE;							// ȫ����־ȱʡ��ȱʡ�趨��ȫ��ģʽ

LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);		// WndProc�Ķ���

GLuint	base;			// �����������ʾ�б�Ŀ�ʼλ��
GLuint	texture[2];		// ������������
GLuint	loop;			// ͨ��ѭ������

GLfloat	cnt1;			// �����ƶ�������1
GLfloat	cnt2;			// �����ƶ�������2

AUX_RGBImageRec *LoadBMP(char *Filename)					// ����λͼͼ��
{
	FILE *File=NULL;							// �ļ����
	if (!Filename)								// ȷ���ļ������ṩ
	{
		return NULL;							// ���û�ṩ������ NULL
	}
	File=fopen(Filename,"r");						// ���Դ��ļ�

	if (File)								// �ļ�����ô?
	{
		fclose(File);							// �رվ��
		return auxDIBImageLoad(Filename);				// ����λͼ������ָ��
	}

	return NULL;								// �������ʧ�ܣ����� NULL
}

int LoadGLTextures()							// ����λͼ(��������Ĵ���)��ת��������
{
	int Status=FALSE;							// ״ָ̬ʾ��
	AUX_RGBImageRec *TextureImage[2];					// ��������Ĵ洢�ռ�
	memset(TextureImage,0,sizeof(void *)*2);					// ��ָ����Ϊ NULL

	if ((TextureImage[0]=LoadBMP("Font.bmp")) &&			// ��������ͼ��
		(TextureImage[1]=LoadBMP("Bumps.bmp")))			// ��������ͼ��
	{
		Status=TRUE;						// �� Status ��Ϊ TRUE
		glGenTextures(2, &texture[0]);							// ��������

		for (loop=0; loop<2; loop++)					// ѭ���������е�����
		{
			// ������������
			glBindTexture(GL_TEXTURE_2D, texture[loop]);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[loop]->sizeX, TextureImage[loop]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[loop]->data);
		}
	}

	for (loop=0; loop<2; loop++)
		{
	        if (TextureImage[loop])						// �����Ƿ����
			{
				if (TextureImage[loop]->data)			// ����ͼ���Ƿ����
				{
					free(TextureImage[loop]->data);		// �ͷ�����ͼ��ռ�õ��ڴ�
				}
				free(TextureImage[loop]);				// �ͷ�ͼ��ṹ
			}
		}
	return Status;								// ���� Status
}

GLvoid BuildFont(GLvoid)								// �������ǵ��ַ���ʾ�б�
{
	float	cx;							// �ַ���X����
	float	cy;							// �ַ���Y����
	base=glGenLists(256);				// ����256����ʾ�б�
	glBindTexture(GL_TEXTURE_2D, texture[0]);// ѡ���ַ�ͼ��

	for (loop=0; loop<256; loop++)		// ѭ��256����ʾ�б�
	{
		cx=float(loop%16)/16.0f;		// ��ǰ�ַ���X����
		cy=float(loop/16)/16.0f;		// ��ǰ�ַ���Y����
		glNewList(base+loop,GL_COMPILE);//��ʼ������ʾ�б�
		glBegin(GL_QUADS);				// ʹ���ı�����ʾÿһ���ַ�

		glTexCoord2f(cx,1-cy-0.0625f);	// ���½ǵ���������
		glVertex2i(0,0);				// ���½ǵ�����
		glTexCoord2f(cx+0.0625f,1-cy-0.0625f);// ���½ǵ���������
		glVertex2i(16,0);				// ���½ǵ�����
		glTexCoord2f(cx+0.0625f,1-cy);	// ���Ͻǵ���������
		glVertex2i(16,16);				// ���Ͻǵ�����
		glTexCoord2f(cx,1-cy);			// ���Ͻǵ���������
		glVertex2i(0,16);				// ���Ͻǵ�����
	glEnd();						// �ı����ַ��������
	glTranslated(10,0,0);					// ������һ���ַ�������ƽ��16����λ
		glEndList();							// �ַ���ʾ�б����
	}									// ѭ������256����ʾ�б�
}

GLvoid KillFont(GLvoid)								
{
	glDeleteLists(base,256);						// ���ڴ���ɾ��256����ʾ�б�
}

GLvoid glPrint(GLint x, GLint y, char *string, int set)				// �����ַ�
{
	if (set>1)								// ����ַ�������1
	{
		set=1;								// ������Ϊ1
	}
	glBindTexture(GL_TEXTURE_2D, texture[0]);				// ��Ϊ��������
	glDisable(GL_DEPTH_TEST);						// ��ֹ��Ȳ���
	
	glMatrixMode(GL_PROJECTION);						// ѡ��ͶӰ����
	glPushMatrix();								// ���浱ǰ��ͶӰ����

	glLoadIdentity();								// ����ͶӰ����
	glOrtho(0,640,0,480,-1,1);							// ������ͶӰ�Ŀ�������

	glMatrixMode(GL_MODELVIEW);							// ѡ��ģ�ͱ任����
	glPushMatrix();								// ���浱ǰ��ģ�ͱ任����
	glLoadIdentity();								// ����ģ�ͱ任����

	glTranslated(x,y,0);							// ���ַ�ԭ���ƶ���(x,y)λ��
	glListBase(base-32+(128*set));						// ѡ���ַ���
	glCallLists(strlen(string),GL_BYTE,string);					// ���ַ���д�뵽��Ļ 
	glMatrixMode(GL_PROJECTION);						// ѡ��ͶӰ����
	glPopMatrix();								// ����Ϊ����ľ���

	glMatrixMode(GL_MODELVIEW);							// ѡ��ģ�;���
	glPopMatrix();								// ����Ϊ����ľ���
}

GLvoid ReSizeGLScene(GLsizei width, GLsizei height)			// ����OpenGL���ڴ�С
{
	if (height==0)							// ��ֹ�����
	{
		height=1;							// ��Height��Ϊ1
	}

	glViewport(0, 0, width, height);		// ���õ�ǰ���ӿ�
	glMatrixMode(GL_PROJECTION);			// ѡ��ͶӰ����
	glLoadIdentity();						// ����ͶӰ����

	// �����ӿڵĴ�С
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);

	glMatrixMode(GL_MODELVIEW);				// ѡ��ģ�͹۲����
	glLoadIdentity();						// ����ģ�͹۲����
}

int InitGL(GLvoid)								// �˴���ʼ��OpenGL������������
{
	if (!LoadGLTextures())							// ������������������
	{
		return FALSE;							// ���δ�����룬����FALSE
	}
	BuildFont();								// �����ַ���ʾ�б�
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);					// ��ɫ���� 
	glClearDepth(1.0);								// ������Ȼ���
	glDepthFunc(GL_LEQUAL);							// ������Ȳ��Ե�����
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);						// ���û������
	glShadeModel(GL_SMOOTH);							// ������Ӱƽ��
	glEnable(GL_TEXTURE_2D);							// ��������ӳ��
	return TRUE;								// ��ʼ���ɹ�
}

int DrawGLScene(GLvoid)								// �����￪ʼ�������еĻ���
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);				// �����Ļ����Ȼ���
	glLoadIdentity();								// ���õ�ǰ��ģ�͹۲����
	glBindTexture(GL_TEXTURE_2D, texture[1]);					// ����Ϊͼ������
	glTranslatef(0.0f,0.0f,-5.0f);						// ������Ļ5����λ
	glRotatef(45.0f,0.0f,0.0f,1.0f);						// ��Z����ת45��
	glRotatef(cnt1*30.0f,1.0f,1.0f,0.0f);					//  ��(1,1,0)����ת30��
	glDisable(GL_BLEND);							// �رջ��
	glColor3f(1.0f,1.0f,1.0f);							//������ɫΪ��ɫ
	glBegin(GL_QUADS);								// ���������ı���
		glTexCoord2d(0.0f,0.0f);					
		glVertex2f(-1.0f, 1.0f);					
		glTexCoord2d(1.0f,0.0f);					
		glVertex2f( 1.0f, 1.0f);					
		glTexCoord2d(1.0f,1.0f);					
		glVertex2f( 1.0f,-1.0f);					
		glTexCoord2d(0.0f,1.0f);					
		glVertex2f(-1.0f,-1.0f);					
	glEnd();		
	glRotatef(90.0f,1.0f,1.0f,0.0f);					//  ��(1,1,0)����ת90��
	glBegin(GL_QUADS);							// ���Ƶڶ����ı��Σ����һ���ı��δ�ֱ
		glTexCoord2d(0.0f,0.0f);					
		glVertex2f(-1.0f, 1.0f);					
		glTexCoord2d(1.0f,0.0f);					
		glVertex2f( 1.0f, 1.0f);					
		glTexCoord2d(1.0f,1.0f);					
		glVertex2f( 1.0f,-1.0f);					
		glTexCoord2d(0.0f,1.0f);					
		glVertex2f(-1.0f,-1.0f);					
	glEnd();		
	glEnable(GL_BLEND);							// ���û�ϲ���
	glLoadIdentity();								// �����ӿ�

	// ��������λ��������ɫ
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
    
	cnt1+=0.01f;								// ���Ӽ�����ֵ
	cnt2+=0.0081f;								// ���Ӽ�����ֵ
	return TRUE;								// �ɹ�����
}

GLvoid KillGLWindow(GLvoid)							// �������ٴ���
{
	if (fullscreen)
	{
		ChangeDisplaySettings(NULL, 0);				// �ǵĻ����л�������
		ShowCursor(TRUE);							// ��ʾ���ָ��
	}

	if (hRC)										// ����ӵ��OpenGL��Ⱦ��������?
	{
		if (!wglMakeCurrent(NULL, NULL))			// �����ܷ��ͷ�DC��RC������?
		{
			MessageBox(NULL,  "�ͷ�DC��RCʧ�ܡ�",  "�رմ���", MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))					// �����ܷ�ɾ��RC?
		{
			MessageBox(NULL, "�ͷ�RCʧ�ܡ�",  "�رմ���", MB_OK | MB_ICONINFORMATION);
		}

		hRC=NULL;									// ��RC��Ϊ NULL
	}

	if (hDC && !ReleaseDC(hWnd,hDC))					// �����ܷ��ͷ� DC?
	{
		MessageBox(NULL, "�ͷ�DCʧ�ܡ�",  "�رմ���", MB_OK | MB_ICONINFORMATION);
		hDC=NULL;							// �� DC ��Ϊ NULL
	}

	if (hWnd && !DestroyWindow(hWnd))					// �ܷ����ٴ���?
	{
		MessageBox(NULL, "�ͷŴ��ھ��ʧ�ܡ�",  "�رմ���", MB_OK | MB_ICONINFORMATION);
		hWnd=NULL;							// �� hWnd ��Ϊ NULL
	}
	
	if (!UnregisterClass( "OpenG",hInstance))				// �ܷ�ע����?
	{
		MessageBox(NULL, "����ע�������ࡣ", "�رմ���",MB_OK | MB_ICONINFORMATION);
		hInstance=NULL;							// �� hInstance ��Ϊ NULL
	}
	KillFont();
}

BOOL CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag)
{
	GLuint		PixelFormat;						// �������ƥ��Ľ��
	WNDCLASS	wc;							// ������ṹ
	DWORD		dwExStyle;						// ��չ���ڷ��
	DWORD		dwStyle;						// ���ڷ��

	RECT WindowRect;							// ȡ�þ��ε����ϽǺ����½ǵ�����ֵ
	WindowRect.left=(long)0;						// ��Left   ��Ϊ 0
	WindowRect.right=(long)width;						// ��Right  ��ΪҪ��Ŀ��
	WindowRect.top=(long)0;							// ��Top    ��Ϊ 0
	WindowRect.bottom=(long)height;						// ��Bottom ��ΪҪ��ĸ߶�

	fullscreen=fullscreenflag;						// ����ȫ��ȫ����־

	hInstance		= GetModuleHandle(NULL);			// ȡ�����Ǵ��ڵ�ʵ��
	wc.style		= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;		// �ƶ�ʱ�ػ�����Ϊ����ȡ��DC
	wc.lpfnWndProc		= (WNDPROC) WndProc;				// WndProc������Ϣ
	wc.cbClsExtra		= 0;						// �޶��ⴰ������
	wc.cbWndExtra		= 0;						// �޶��ⴰ������
	wc.hInstance		= hInstance;					// ����ʵ��
	wc.hIcon		= LoadIcon(NULL, IDI_WINLOGO);			// װ��ȱʡͼ��
	wc.hCursor		= LoadCursor(NULL, IDC_ARROW);			// װ�����ָ��
	wc.hbrBackground	= NULL;						// GL����Ҫ����
	wc.lpszMenuName		= NULL;						// ����Ҫ�˵�
	wc.lpszClassName	=  "OpenG";					// �趨������

	if (!RegisterClass(&wc))						// ����ע�ᴰ����
	{
		MessageBox(NULL, "ע�ᴰ��ʧ��", "����",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// �˳�������FALSE
	}

	if (fullscreen)								// Ҫ����ȫ��ģʽ��?
	{
		DEVMODE dmScreenSettings;						// �豸ģʽ
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));			// ȷ���ڴ����Ϊ��
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);			// Devmode �ṹ�Ĵ�С
		dmScreenSettings.dmPelsWidth	= width;				// ��ѡ��Ļ���
		dmScreenSettings.dmPelsHeight	= height;				// ��ѡ��Ļ�߶�
		dmScreenSettings.dmBitsPerPel	= bits;					// ÿ������ѡ��ɫ�����
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		// ����������ʾģʽ�����ؽ����ע: CDS_FULLSCREEN ��ȥ��״̬����
		if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		{
			// ��ģʽʧ�ܣ��ṩ����ѡ��˳����ڴ��������С�
			if (MessageBox(NULL, "ȫ��ģʽ�ڵ�ǰ�Կ�������ʧ�ܣ�\nʹ�ô���ģʽ��", "NeHe G",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
			{
				fullscreen=FALSE;				// ѡ�񴰿�ģʽ(Fullscreen=FALSE)
			}
			else
			{
				// ����һ���Ի��򣬸����û��������
				MessageBox(NULL, "���򽫱��ر�", "����",MB_OK|MB_ICONSTOP);
				return FALSE;					//  �˳������� FALSE
			}
		}
	}

	if (fullscreen)								// �Դ���ȫ��ģʽ��?
	{
		dwExStyle=WS_EX_APPWINDOW;					// ��չ������
		dwStyle=WS_POPUP;						// ������
		ShowCursor(FALSE);						// �������ָ��
	}
	else
	{
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// ��չ������
		dwStyle=WS_OVERLAPPEDWINDOW;					//  ������
	}
	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// �������ڴﵽ����Ҫ��Ĵ�С

	if (!(hWnd=CreateWindowEx(	dwExStyle,				// ��չ������
					 "OpenG",				// ������
					 title,					// ���ڱ���
					WS_CLIPSIBLINGS |			// ����Ĵ���������
					WS_CLIPCHILDREN |			// ����Ĵ���������
					dwStyle,				// ѡ��Ĵ�������
					0, 0,					// ����λ��
					WindowRect.right-WindowRect.left,	// ��������õĴ��ڿ��
					WindowRect.bottom-WindowRect.top,	// ��������õĴ��ڸ߶�
					NULL,					// �޸�����
					NULL,					// �޲˵�
					hInstance,				// ʵ��
					NULL)))					// ����WM_CREATE�����κζ���
	{
		KillGLWindow();							// ������ʾ��
		MessageBox(NULL, "���ܴ���һ�������豸������", "����",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// ���� FALSE
	}

	static	PIXELFORMATDESCRIPTOR pfd=					// /pfd ���ߴ���������ϣ���Ķ�����������ʹ�õ����ظ�ʽ
	{
		sizeof(PIXELFORMATDESCRIPTOR),					// ������ʽ�������Ĵ�С
		1,								// �汾��
		PFD_DRAW_TO_WINDOW |						// ��ʽ֧�ִ���
		PFD_SUPPORT_OPENGL |						// ��ʽ����֧��OpenGL
		PFD_DOUBLEBUFFER,						// ����֧��˫����
		PFD_TYPE_RGBA,							// ���� RGBA ��ʽ
		bits,								// ѡ��ɫ�����
		0, 0, 0, 0, 0, 0,						// ���Ե�ɫ��λ
		0,								// ��Alpha����
		0,								// ����Shift Bit
		0,								// ���ۼӻ���
		0, 0, 0, 0,							// ���Ծۼ�λ
		16,								// 16λ Z-���� (��Ȼ���)
		0,								// ���ɰ建��
		0,								// �޸�������
		PFD_MAIN_PLANE,							// ����ͼ��
		0,								// Reserved
		0, 0, 0								// ���Բ�����
	};

	if (!(hDC=GetDC(hWnd)))							// ȡ���豸��������ô?
	{
		KillGLWindow();							// ������ʾ��
		MessageBox(NULL, "���ܴ���һ����ƥ������ظ�ʽ", "����",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// ���� FALSE
	}

	if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))				// Windows �ҵ���Ӧ�����ظ�ʽ����?
	{
		KillGLWindow();							// ������ʾ��
		MessageBox(NULL, "�����������ظ�ʽ", "����",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// ���� FALSE
	}

	if(!SetPixelFormat(hDC,PixelFormat,&pfd))				// �ܹ��������ظ�ʽô?
	{
		KillGLWindow();							// ������ʾ��
		MessageBox(NULL, "�����������ظ�ʽ", "����",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// ���� FALSE
	}

	if (!(hRC=wglCreateContext(hDC)))					// �ܷ�ȡ����ɫ������?
	{
		KillGLWindow();							// ������ʾ��
		MessageBox(NULL, "���ܴ���OpenGL��Ⱦ������", "����",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// ���� FALSE
	}

	if (!(hRC=wglCreateContext(hDC)))					// �ܷ�ȡ����ɫ������?
	{
		KillGLWindow();							// ������ʾ��
		MessageBox(NULL, "���ܴ���OpenGL��Ⱦ������", "����",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// ���� FALSE
	}

	if(!wglMakeCurrent(hDC,hRC))						// ���Լ�����ɫ������
	{
		KillGLWindow();							// ������ʾ��
		MessageBox(NULL, "���ܼ��ǰ��OpenGL��Ȼ������", "����",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// ���� FALSE
	}

	ShowWindow(hWnd,SW_SHOW);						// ��ʾ����
	SetForegroundWindow(hWnd);						// ����������ȼ�
	SetFocus(hWnd);								// ���ü��̵Ľ������˴���
	ReSizeGLScene(width, height);						// ����͸�� GL ��Ļ

	if (!InitGL())								// ��ʼ���½���GL����
	{
		KillGLWindow();							// ������ʾ��
		MessageBox(NULL, "Initialization Failed.", "ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// ���� FALSE
	}

	return TRUE;								// �ɹ�
}

LRESULT CALLBACK WndProc(	HWND	hWnd,					// ���ڵľ��
				UINT	uMsg,					// ���ڵ���Ϣ
				WPARAM	wParam,					// ���ӵ���Ϣ����
				LPARAM	lParam)					// ���ӵ���Ϣ����
{
	switch (uMsg)								// ���Windows��Ϣ
	{
		case WM_ACTIVATE:						// ���Ӵ��ڼ�����Ϣ
		{
			if (!HIWORD(wParam))					// �����С��״̬
			{
				active=TRUE;					// �����ڼ���״̬
			}
			else
			{
				active=FALSE;					// �����ټ���
			}

			return 0;						// ������Ϣѭ��
		}

		case WM_SYSCOMMAND:						// ϵͳ�ж�����
		{
			switch (wParam)						// ���ϵͳ����
			{
				case SC_SCREENSAVE:				// ����Ҫ����?
				case SC_MONITORPOWER:				// ��ʾ��Ҫ����ڵ�ģʽ?
				return 0;					// ��ֹ����
			}
			break;							// �˳�
		}

		case WM_CLOSE:							// �յ�Close��Ϣ?
		{
			PostQuitMessage(0);					// �����˳���Ϣ
			return 0;						// ����
		}

		case WM_KEYDOWN:						// �м�����ô?
		{
			keys[wParam] = TRUE;					// ����ǣ���ΪTRUE
			return 0;						// ����
		}

		case WM_KEYUP:							// �м��ſ�ô?
		{
			keys[wParam] = FALSE;					// ����ǣ���ΪFALSE
			return 0;						// ����
		}

		case WM_SIZE:							// ����OpenGL���ڴ�С
		{
			ReSizeGLScene(LOWORD(lParam),HIWORD(lParam));		// LoWord=Width,HiWord=Height
			return 0;						// ����
		}
	}

	// �� DefWindowProc��������δ�������Ϣ��
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

int WINAPI WinMain(	HINSTANCE	hInstance,				// ��ǰ����ʵ��
			HINSTANCE	hPrevInstance,				// ǰһ������ʵ��
			LPSTR		lpCmdLine,				// �����в���
			int		nCmdShow)				// ������ʾ״̬
{
	MSG	msg;								// Windowsx��Ϣ�ṹ
	BOOL	done=FALSE;							// �����˳�ѭ����Bool ����
	
	// ��ʾ�û�ѡ������ģʽ
	if (MessageBox(NULL,"������ȫ��ģʽ������ô��",  "����ȫ��ģʽ",MB_YESNO|MB_ICONQUESTION)==IDNO)
	{
		fullscreen=FALSE;						// FALSEΪ����ģʽ
	}

	// ����OpenGL����
	if (!CreateGLWindow("OpenGL 2D PicText",640,480,16,fullscreen))
	{
		return 0;							// ʧ���˳�
	}

	while(!done)								// ����ѭ��ֱ�� done=TRUE
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))			// ����Ϣ�ڵȴ���?
		{
			if (msg.message==WM_QUIT)				// �յ��˳���Ϣ?
			{
				done=TRUE;					// �ǣ���done=TRUE
			}
			else							// ���ǣ���������Ϣ
			{
				TranslateMessage(&msg);				// ������Ϣ
				DispatchMessage(&msg);				// ������Ϣ
			}
		}
		else								// ���û����Ϣ
		{
			// ���Ƴ���������ESC��������DrawGLScene()���˳���Ϣ
			if (active)						// ���򼤻��ô?
			{
				if (keys[VK_ESCAPE])				// ESC ������ô?
				{
					done=TRUE;				// ESC �����˳��ź�
				}
				else						// �����˳���ʱ��ˢ����Ļ
				{
					DrawGLScene();				// ���Ƴ���
					SwapBuffers(hDC);			// �������� (˫����)
				}
			}

			if (keys[VK_F1])					// F1��������ô?
			{
				keys[VK_F1]=FALSE;				// ���ǣ�ʹ��Ӧ��Key�����е�ֵΪ FALSE
				KillGLWindow();					// ���ٵ�ǰ�Ĵ���
				fullscreen=!fullscreen;				// �л� ȫ�� / ���� ģʽ
				// �ؽ� OpenGL ����
				if (!CreateGLWindow("OpenGL 2D PicText",640,480,16,fullscreen))
				{
					return 0;				// �������δ�ܴ����������˳�
				}
			}
		}
	}

	// �رճ���
	KillGLWindow();								// ���ٴ���
	return (msg.wParam);							// �˳�����
}