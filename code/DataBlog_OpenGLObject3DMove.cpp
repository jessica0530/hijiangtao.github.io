#include <windows.h>		// Windows��ͷ�ļ�
#include <gl/GL.h>
#include <gl/GLU.h>
#include <gl/glut.h>		// ����OpenGLʵ�ÿ�
#include <stdio.h>			// ��׼���������ͷ�ļ�
#include <gl/GLAUX.H>			// GLaux���ͷ�ļ�

HGLRC           hRC=NULL;							// ������ɫ��������
HDC             hDC=NULL;							// OpenGL��Ⱦ��������
HWND            hWnd=NULL;							// �������ǵĴ��ھ��
HINSTANCE       hInstance;							// ��������ʵ��

BOOL	twinkle;					// ��˸������
BOOL	tp;							// 'T' ������ô? 
const	int num=50;					// ���Ƶ�������

bool	keys[256];								    // ������̰���������
bool	active=TRUE;								// ���ڵĻ��־��ȱʡΪTRUE
bool	fullscreen=TRUE;							// ȫ����־ȱʡ��ȱʡ�趨��ȫ��ģʽ

LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);		// WndProc�Ķ���

typedef struct						// Ϊ���Ǵ���һ���ṹ
{
	int r, g, b;					// ���ǵ���ɫ
	GLfloat dist;					// ���Ǿ������ĵľ���
	GLfloat angle;					// ��ǰ���������ĽǶ�
}
stars;								// �ṹ����Ϊstars
stars star[num];					// ʹ�� 'stars' �ṹ����һ������ 'num'��Ԫ�ص� 'star'����

GLfloat	zoom=-15.0f;						// ������۲��ߵľ���
GLfloat tilt=90.0f;						// ���ǵ����
GLfloat	spin;							// ��˸���ǵ���ת

GLuint	loop;							// ȫ�� Loop ����
GLuint	texture[1];						// ���һ������

AUX_RGBImageRec *LoadBMP(char *Filename)			// ����λͼ�ļ�
{
	FILE *File=NULL;					// �ļ����

	if (!Filename)						// ȷ���Ѹ����ļ���
	{
		return NULL;					// ���޷��� NULL
	}

	File=fopen(Filename,"r");				// ����ļ��Ƿ����

	if (File)						// �ļ�����ô?
	{
		fclose(File);					// �ر��ļ����
		return auxDIBImageLoad(Filename);		// ����λͼ������ָ��
	}
	return NULL;						// �������ʧ�ܷ��� NULL
}

int LoadGLTextures()						// ����λͼ��ת��������
{
	int Status=FALSE;					// ״ָ̬ʾ��

	AUX_RGBImageRec *TextureImage[1];			// Ϊ�������洢�ռ�

	memset(TextureImage,0,sizeof(void *)*1);		// ��ָ����Ϊ NULL

	// ����λͼ��������δ�ҵ�λͼ�ļ����˳�
	if (TextureImage[0]=LoadBMP("3.bmp"))
	{
		Status=TRUE;					// �� Status ��ΪTRUE

		glGenTextures(1, &texture[0]);			// ����һ������

		// ����һ�������˲�����
		glBindTexture(GL_TEXTURE_2D, texture[0]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);
	}

	if (TextureImage[0])					// ����������
	{
		if (TextureImage[0]->data)			// �������ͼ�����
		{
			free(TextureImage[0]->data);		// �ͷ�����ͼ����ռ���ڴ�
		}

		free(TextureImage[0]);				// �ͷ�ͼ��ṹ
	}

	return Status;						// ���� Status��ֵ
}

int InitGL(GLvoid)						// �˴���ʼ��OpenGL������������
{
	if (!LoadGLTextures())					// ������������������
	{
		return FALSE;					// ���δ�����룬����FALSE
	}

	glEnable(GL_TEXTURE_2D);				// ��������ӳ��
	glShadeModel(GL_SMOOTH);				// ������Ӱƽ��
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);			// ��ɫ����
	glClearDepth(1.0f);					// ������Ȼ���
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// ������ϸ��͸������
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);			// ���û�ɫ����ȡ�ð�͸��Ч��
	glEnable(GL_BLEND);					// ���û�ɫ
	for (loop=0; loop<num; loop++)				// ����ѭ������ȫ������
	{
		star[loop].angle=0.0f;				// �������Ƕ�����Ƕȿ�ʼ
		star[loop].dist=(float(loop)/num)*5.0f;		// �������������ĵľ���
		star[loop].r=rand()%256;			// Ϊstar[loop]���������ɫ����
		star[loop].g=rand()%256;			// Ϊstar[loop]���������ɫ����
		star[loop].b=rand()%256;			// Ϊstar[loop]���������ɫ����
	}
	return TRUE;						// ��ʼ��һ��OK
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

int DrawGLScene(GLvoid)						// �˹����а������еĻ��ƴ���
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// �����Ļ����Ȼ���
	glBindTexture(GL_TEXTURE_2D, texture[0]);		// ѡ������

	for (loop=0; loop<num; loop++)				// ѭ���������е�����
	{
		glLoadIdentity();				// ����ÿ������֮ǰ������ģ�͹۲����
		glTranslatef(0.0f,0.0f,zoom);			// ������Ļ����
		glRotatef(tilt,1.0f,0.0f,0.0f);			// ��б�ӽ�
		glRotatef(star[loop].angle,0.0f,1.0f,0.0f);	// ��ת����ǰ�������ǵĽǶ�
		glTranslatef(star[loop].dist,0.0f,0.0f);	// ��X�������ƶ�
		glRotatef(-star[loop].angle,0.0f,1.0f,0.0f);	// ȡ����ǰ���ǵĽǶ�
		glRotatef(-tilt,1.0f,0.0f,0.0f);		// ȡ����Ļ��б

		if (twinkle)					// ������˸Ч��
		{
			// ʹ��byte����ֵָ��һ����ɫ
			glColor4ub(star[(num-loop)-1].r,star[(num-loop)-1].g,star[(num-loop)-1].b,255);
			glBegin(GL_QUADS);			// ��ʼ��������ӳ������ı���
				glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,-1.0f, 0.0f);
				glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,-1.0f, 0.0f);
				glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f, 1.0f, 0.0f);
				glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 0.0f);
			glEnd();				// �ı��λ��ƽ���
		}

		glRotatef(spin,0.0f,0.0f,1.0f);			// ��z����ת����
		// ʹ��byte����ֵָ��һ����ɫ
		glColor4ub(star[loop].r,star[loop].g,star[loop].b,255);
		glBegin(GL_QUADS);				// ��ʼ��������ӳ������ı���
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,-1.0f, 0.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,-1.0f, 0.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f, 1.0f, 0.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 0.0f);
		glEnd();					// �ı��λ��ƽ���
		spin+=0.01f;					// ���ǵĹ�ת
		star[loop].angle+=float(loop)/num;		// �ı����ǵ���ת�Ƕ�
		star[loop].dist-=0.01f;				// �ı����������ĵľ���

		if (star[loop].dist<0.0f)			// ���ǵ���������ô
		{
			star[loop].dist+=5.0f;			// ������5����λ
			star[loop].r=rand()%256;		// ��һ���º�ɫ����
			star[loop].g=rand()%256;		// ��һ������ɫ����
			star[loop].b=rand()%256;		// ��һ������ɫ����
		}
	}
	return TRUE;						// һ������
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
	if (!CreateGLWindow("NeHe's OpenGL������",640,480,16,fullscreen))
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
					if (keys['T'] && !tp)				// �Ƿ�T ���Ѱ��²��� tpֵΪ FALSE
					{
						tp=TRUE;				// ���ǣ���tp��ΪTRUE
						twinkle=!twinkle;			// ��ת twinkle��ֵ
					}

					if (!keys['T'])					// T �����ɿ���ô��
					{
						tp=FALSE;				// ���� ��tpΪ FALSE
					}
					if (keys[VK_UP])				// �Ϸ����������ô��
					{
						tilt-=0.5f;				// ��Ļ������б
					}

					if (keys[VK_DOWN])				// �·����������ô��
					{
						tilt+=0.5f;				// ��Ļ������б
					}

					if (keys[VK_PRIOR])				// ���Ϸ�ҳ��������ô
					{
						zoom-=0.2f;				// ��С
					}

					if (keys[VK_NEXT])				// ���·�ҳ��������ô��
					{
						zoom+=0.2f;				// �Ŵ�
					}
				}
			}

			if (keys[VK_F1])				// F1��������ô?
			{
				keys[VK_F1]=FALSE;			// ���ǣ�ʹ��Ӧ��Key�����е�ֵΪ FALSE
				KillGLWindow();				// ���ٵ�ǰ�Ĵ���
				fullscreen=!fullscreen;			// �л� ȫ�� / ���� ģʽ
				// �ؽ� OpenGL ����
				if (!CreateGLWindow("͸������ʵ��",640,480,16,fullscreen))
				{
					return 0;			// �������δ�ܴ����������˳�
				}
			}
		}
	}
}

