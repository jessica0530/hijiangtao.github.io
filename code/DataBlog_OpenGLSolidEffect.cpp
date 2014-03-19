#include <windows.h>		// Windows��ͷ�ļ�
#include <gl/GL.h>
#include <gl/GLU.h>
#include <gl/glut.h>		// ����OpenGLʵ�ÿ�
#include <stdio.h>			// ��׼����/������ͷ�ļ�
#include <gl/GLAUX.H>		// ����GLaux���ͷ�ļ�

#define	MAX_PARTICLES	1000// ��������������
bool	rainbow=true;	// �Ƿ�Ϊ�ʺ�ģʽ
bool	sp;				// �ո���Ƿ񱻰���
bool	rp;				// �س����Ƿ񱻰���

float	slowdown=2.0f;			// ��������
float	xspeed;				// X������ٶ�
float	yspeed;				// Y������ٶ�
float	zoom=-40.0f;			// ��Z������
GLuint	loop;				// ѭ������
GLuint	col;				// ��ǰ����ɫ
GLuint	delay;				// �ʺ�Ч���ӳ�

GLfloat		xrot;			// X ��ת��
GLfloat		yrot;			// Y ��ת��
GLfloat		zrot;			// Z ��ת��
GLuint		texture[1];		// �洢һ������

HGLRC           hRC=NULL;	// ������ɫ��������
HDC             hDC=NULL;	// OpenGL��Ⱦ��������
HWND            hWnd=NULL;	// �������ǵĴ��ھ��
HINSTANCE       hInstance;	// ��������ʵ��

bool	keys[256];			// ������̰���������
bool	active=TRUE;		// ���ڵĻ��־��ȱʡΪTRUE
bool	fullscreen=TRUE;	// ȫ����־ȱʡ��ȱʡ�趨��ȫ��ģʽ

LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);		// WndProc�Ķ���

GLfloat		rtri;						// ���������εĽǶ�
GLfloat		rquad;						// �����ı��εĽǶ�

typedef struct					// �����������ݽṹ
{
	bool	active;				// �Ƿ񼤻�
	float	life;				// ��������
	float	fade;				// ˥���ٶ�

	float	r;					// ��ɫֵ
	float	g;					// ��ɫֵ
	float	b;					// ��ɫֵ

	float	x;					// X λ��
	float	y;					// Y λ��
	float	z;					// Z λ��

	float	xi;					// X ����
	float	yi;					// Y ����
	float	zi;					// Z ����

	float	xg;					// X �����������ٶ�
	float	yg;					// Y �����������ٶ�
	float	zg;					// Z �����������ٶ�
}particles;						// �������ݽṹ
particles particle[MAX_PARTICLES];				// ����1000�����ӵ�����

static GLfloat colors[12][3]=				// �ʺ���ɫ
{
	{1.0f,0.5f,0.5f},{1.0f,0.75f,0.5f},{1.0f,1.0f,0.5f},{0.75f,1.0f,0.5f},
	{0.5f,1.0f,0.5f},{0.5f,1.0f,0.75f},{0.5f,1.0f,1.0f},{0.5f,0.75f,1.0f},
	{0.5f,0.5f,1.0f},{0.75f,0.5f,1.0f},{1.0f,0.5f,1.0f},{1.0f,0.5f,0.75f}
};

AUX_RGBImageRec *LoadBMP(char *Filename)	// ����λͼͼ��
{
	FILE *File=NULL;						// �ļ����
	if (!Filename)							// ȷ���ļ������ṩ
	{
		return NULL;						// ���û�ṩ������ NULL
	}

	File=fopen(Filename,"r");				// ���Դ��ļ�
	if (File)								// �ļ�����ô?
	{
		fclose(File);						// �رվ��
		return auxDIBImageLoad(Filename);	// ����λͼ������ָ��
	}

	return NULL;							// �������ʧ�ܣ����� NULL
}

int LoadGLTextures()								// ����λͼ(��������Ĵ���)��ת��������
{
	int Status=FALSE;							// ״ָ̬ʾ��
	AUX_RGBImageRec *TextureImage[1];					// ��������Ĵ洢�ռ�
	memset(TextureImage,0,sizeof(void *)*1);				// ��ָ����Ϊ NULL

	// ����λͼ��������޴������λͼû�ҵ����˳�
	if (TextureImage[0]=LoadBMP("Particle.bmp"))	// ������������
	{
		Status=TRUE;							// �� Status ��Ϊ TRUE
		glGenTextures(1, &texture[0]);					// ��������

		// ʹ������λͼ�������� �ĵ�������
		glBindTexture(GL_TEXTURE_2D, texture[0]);

		// ��������
		glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);

		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	// �����˲�
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	// �����˲�
	}

	if (TextureImage[0])							// �����Ƿ����
	{
		if (TextureImage[0]->data)					// ����ͼ���Ƿ����
		{
			free(TextureImage[0]->data);				// �ͷ�����ͼ��ռ�õ��ڴ�
		}

		free(TextureImage[0]);						// �ͷ�ͼ��ṹ
	}
	return Status;								// ���� Status
}

int InitGL(GLvoid)								// �˴���ʼ��OpenGL������������
{
	if (!LoadGLTextures())							// ������������������
	{
		return FALSE;							// ���δ�����룬����FALSE
	}

	glDisable(GL_DEPTH_TEST);						//��ֹ��Ȳ���

	for (loop=0;loop<MAX_PARTICLES;loop++)			//��ʼ�����е�����
	{
		particle[loop].active=true;					// ʹ���е�����Ϊ����״̬
		particle[loop].life=1.0f;					// ���е���������ֵΪ���
		particle[loop].fade=float(rand()%100)/1000.0f+0.003f;// �������˥������

		particle[loop].r=colors[loop*(12/MAX_PARTICLES)][0];		// ���ӵĺ�ɫ��ɫ
		particle[loop].g=colors[loop*(12/MAX_PARTICLES)][1];		// ���ӵ���ɫ��ɫ
		particle[loop].b=colors[loop*(12/MAX_PARTICLES)][2];		// ���ӵ���ɫ��ɫ

		particle[loop].xi=float((rand()%50)-26.0f)*10.0f;		// �������X�᷽���ٶ�
		particle[loop].yi=float((rand()%50)-25.0f)*10.0f;		// �������Y�᷽���ٶ�
		particle[loop].zi=float((rand()%50)-25.0f)*10.0f;		// �������Z�᷽���ٶ�

		particle[loop].xg=0.0f;						// ����X�᷽����ٶ�Ϊ0
		particle[loop].yg=-0.8f;						//  ����Y�᷽����ٶ�Ϊ-0.8
		particle[loop].zg=0.0f;						//  ����Z�᷽����ٶ�Ϊ0
	}

	glEnable(GL_TEXTURE_2D);						// ��������ӳ��
	glShadeModel(GL_SMOOTH);						// ������Ӱƽ��
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);					// ��ɫ����
	glClearDepth(1.0f);							// ������Ȼ���
	glEnable(GL_DEPTH_TEST);						// ������Ȳ���
	glDepthFunc(GL_LEQUAL);							// ������Ȳ��Ե�����
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);			// ������ϸ��͸������
	return TRUE;								// ��ʼ�� OK
}

GLvoid ReSizeGLScene(GLsizei width, GLsizei height)// ����OpenGL���ڴ�С
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

int DrawGLScene(GLvoid)			// ��������
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);// �Ժ�ɫ�������
	glLoadIdentity();			// ����ģ�ͱ任����

	for (loop=0;loop<MAX_PARTICLES;loop++)					// ѭ�����е�����
	{
		if (particle[loop].active)					// �������Ϊ�����
		{
			float x=particle[loop].x;				// ����X���λ��
			float y=particle[loop].y;				// ����Y���λ��
			float z=particle[loop].z+zoom;			// ����Z���λ��
			// ����������ɫ
			glColor4f(particle[loop].r,particle[loop].g,particle[loop].b,particle[loop].life);
			glBegin(GL_TRIANGLE_STRIP);				// ���������δ�
				glTexCoord2d(1,1); glVertex3f(x+0.5f,y+0.5f,z); 
				glTexCoord2d(0,1); glVertex3f(x-0.5f,y+0.5f,z); 
				glTexCoord2d(1,0); glVertex3f(x+0.5f,y-0.5f,z); 
				glTexCoord2d(0,0); glVertex3f(x-0.5f,y-0.5f,z); 
			glEnd();

			particle[loop].x+=particle[loop].xi/(slowdown*1000);	// ����X�����λ��
			particle[loop].y+=particle[loop].yi/(slowdown*1000);	// ����Y�����λ��
			particle[loop].z+=particle[loop].zi/(slowdown*1000);	// ����Z�����λ��

			particle[loop].xi+=particle[loop].xg;			// ����X�᷽���ٶȴ�С
			particle[loop].yi+=particle[loop].yg;			// ����Y�᷽���ٶȴ�С
			particle[loop].zi+=particle[loop].zg;			// ����Z�᷽���ٶȴ�С

			particle[loop].life-=particle[loop].fade;		// �������ӵ�����ֵ

			if (particle[loop].life<0.0f)					// �����������ֵС��0
			{
				particle[loop].life=1.0f;				// ����һ���µ�����
				particle[loop].fade=float(rand()%100)/1000.0f+0.003f;	// �������˥������

				particle[loop].x=0.0f;					// �����ӳ�������Ļ������
				particle[loop].y=0.0f;					
				particle[loop].z=0.0f;					

				particle[loop].xi=xspeed+float((rand()%60)-32.0f);	// ������������ٶ�
				particle[loop].yi=yspeed+float((rand()%60)-30.0f);	
				particle[loop].zi=float((rand()%60)-30.0f);		

				particle[loop].r=colors[col][0];			// ����������ɫ
				particle[loop].g=colors[col][1];			
				particle[loop].b=colors[col][2];			
			}

			// ���С����8����ס������Y�᷽��ļ��ٶ�
			if (keys[VK_NUMPAD8] && (particle[loop].yg<1.5f)) particle[loop].yg+=0.01f;

			// ���С����2����ס������Y�᷽��ļ��ٶ�
			if (keys[VK_NUMPAD2] && (particle[loop].yg>-1.5f)) particle[loop].yg-=0.01f;

			// ���С����6����ס������X�᷽��ļ��ٶ�
			if (keys[VK_NUMPAD6] && (particle[loop].xg<1.5f)) particle[loop].xg+=0.01f;

			// ���С����4����ס������X�᷽��ļ��ٶ�
			if (keys[VK_NUMPAD4] && (particle[loop].xg>-1.5f)) particle[loop].xg-=0.01f;

			if (keys[VK_TAB])						// ��Tab����ʹ���ӻص�ԭ��
			{
				particle[loop].x=0.0f;					
				particle[loop].y=0.0f;					
				particle[loop].z=0.0f;					
				particle[loop].xi=float((rand()%50)-26.0f)*10.0f;	// ��������ٶ�
				particle[loop].yi=float((rand()%50)-25.0f)*10.0f;	
				particle[loop].zi=float((rand()%50)-25.0f)*10.0f;	
			}
		}
    }
	return TRUE;									// ������ϳɹ�����
}

GLvoid KillGLWindow(GLvoid)					// �������ٴ���
{
	if (fullscreen)
	{
		ChangeDisplaySettings(NULL, 0);		// �ǵĻ����л�������
		ShowCursor(TRUE);					// ��ʾ���ָ��
	}

	if (hRC)								// ����ӵ��OpenGL��Ⱦ��������?
	{
		if (!wglMakeCurrent(NULL, NULL))	// �����ܷ��ͷ�DC��RC������?
		{
			MessageBox(NULL,  "�ͷ�DC��RCʧ�ܡ�",  "�رմ���", MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))			// �����ܷ�ɾ��RC?
		{
			MessageBox(NULL, "�ͷ�RCʧ�ܡ�",  "�رմ���", MB_OK | MB_ICONINFORMATION);
		}

		hRC=NULL;							// ��RC��Ϊ NULL
	}

	if (hDC && !ReleaseDC(hWnd,hDC))		// �����ܷ��ͷ� DC?
	{
		MessageBox(NULL, "�ͷ�DCʧ�ܡ�",  "�رմ���", MB_OK | MB_ICONINFORMATION);
		hDC=NULL;							// �� DC ��Ϊ NULL
	}

	if (hWnd && !DestroyWindow(hWnd))		// �ܷ����ٴ���?
	{
		MessageBox(NULL, "�ͷŴ��ھ��ʧ�ܡ�",  "�رմ���", MB_OK | MB_ICONINFORMATION);
		hWnd=NULL;							// �� hWnd ��Ϊ NULL
	}
	
	if (!UnregisterClass( "OpenG",hInstance))// �ܷ�ע����?
	{
		MessageBox(NULL, "����ע�������ࡣ", "�رմ���",MB_OK | MB_ICONINFORMATION);
		hInstance=NULL;						// �� hInstance ��Ϊ NULL
	}
}

BOOL CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag)
{
	GLuint		PixelFormat;						// �������ƥ��Ľ��
	WNDCLASS	wc;									// ������ṹ
	DWORD		dwExStyle;							// ��չ���ڷ��
	DWORD		dwStyle;							// ���ڷ��

	RECT WindowRect;								// ȡ�þ��ε����ϽǺ����½ǵ�����ֵ
	WindowRect.left=(long)0;						// ��Left   ��Ϊ 0
	WindowRect.right=(long)width;					// ��Right  ��ΪҪ��Ŀ��
	WindowRect.top=(long)0;							// ��Top    ��Ϊ 0
	WindowRect.bottom=(long)height;					// ��Bottom ��ΪҪ��ĸ߶�

	fullscreen=fullscreenflag;						// ����ȫ��ȫ����־

	hInstance		= GetModuleHandle(NULL);		// ȡ�����Ǵ��ڵ�ʵ��
	wc.style		= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;// �ƶ�ʱ�ػ�����Ϊ����ȡ��DC
	wc.lpfnWndProc		= (WNDPROC) WndProc;			// WndProc������Ϣ
	wc.cbClsExtra		= 0;						// �޶��ⴰ������
	wc.cbWndExtra		= 0;						// �޶��ⴰ������
	wc.hInstance		= hInstance;				// ����ʵ��
	wc.hIcon		= LoadIcon(NULL, IDI_WINLOGO);	// װ��ȱʡͼ��
	wc.hCursor		= LoadCursor(NULL, IDC_ARROW);	// װ�����ָ��
	wc.hbrBackground	= NULL;						// GL����Ҫ����
	wc.lpszMenuName		= NULL;						// ����Ҫ�˵�
	wc.lpszClassName	=  "OpenG";					// �趨������

	if (!RegisterClass(&wc))						// ����ע�ᴰ����
	{
		MessageBox(NULL, "ע�ᴰ��ʧ��", "����",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// �˳�������FALSE
	}

	if (fullscreen)									// Ҫ����ȫ��ģʽ��?
	{
		DEVMODE dmScreenSettings;					// �豸ģʽ
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));// ȷ���ڴ����Ϊ��
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);// Devmode �ṹ�Ĵ�С
		dmScreenSettings.dmPelsWidth	= width;	// ��ѡ��Ļ���
		dmScreenSettings.dmPelsHeight	= height;	// ��ѡ��Ļ�߶�
		dmScreenSettings.dmBitsPerPel	= bits;		// ÿ������ѡ��ɫ�����
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
		dwExStyle=WS_EX_APPWINDOW;				// ��չ������
		dwStyle=WS_POPUP;						// ������
		ShowCursor(FALSE);						// �������ָ��
	}
	else
	{
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;	// ��չ������
		dwStyle=WS_OVERLAPPEDWINDOW;					//  ������
	}
	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);// �������ڴﵽ����Ҫ��Ĵ�С

	if (!(hWnd=CreateWindowEx(	dwExStyle,		// ��չ������
					 "OpenG",					// ������
					 title,						// ���ڱ���
					WS_CLIPSIBLINGS |			// ����Ĵ���������
					WS_CLIPCHILDREN |			// ����Ĵ���������
					dwStyle,					// ѡ��Ĵ�������
					0, 0,						// ����λ��
					WindowRect.right-WindowRect.left,	// ��������õĴ��ڿ��
					WindowRect.bottom-WindowRect.top,	// ��������õĴ��ڸ߶�
					NULL,						// �޸�����
					NULL,						// �޲˵�
					hInstance,					// ʵ��
					NULL)))						// ����WM_CREATE�����κζ���
	{
		KillGLWindow();							// ������ʾ��
		MessageBox(NULL, "���ܴ���һ�������豸������", "����",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// ���� FALSE
	}

	static	PIXELFORMATDESCRIPTOR pfd=			// /pfd ���ߴ���������ϣ���Ķ�����������ʹ�õ����ظ�ʽ
	{
		sizeof(PIXELFORMATDESCRIPTOR),			// ������ʽ�������Ĵ�С
		1,										// �汾��
		PFD_DRAW_TO_WINDOW |					// ��ʽ֧�ִ���
		PFD_SUPPORT_OPENGL |					// ��ʽ����֧��OpenGL
		PFD_DOUBLEBUFFER,						// ����֧��˫����
		PFD_TYPE_RGBA,							// ���� RGBA ��ʽ
		bits,									// ѡ��ɫ�����
		0, 0, 0, 0, 0, 0,						// ���Ե�ɫ��λ
		0,										// ��Alpha����
		0,										// ����Shift Bit
		0,										// ���ۼӻ���
		0, 0, 0, 0,								// ���Ծۼ�λ
		16,										// 16λ Z-���� (��Ȼ���)
		0,										// ���ɰ建��
		0,										// �޸�������
		PFD_MAIN_PLANE,							// ����ͼ��
		0,										// Reserved
		0, 0, 0									// ���Բ�����
	};

	if (!(hDC=GetDC(hWnd)))						// ȡ���豸��������ô?
	{
		KillGLWindow();							// ������ʾ��
		MessageBox(NULL, "���ܴ���һ����ƥ������ظ�ʽ", "����",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// ���� FALSE
	}

	if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))// Windows �ҵ���Ӧ�����ظ�ʽ����?
	{
		KillGLWindow();							// ������ʾ��
		MessageBox(NULL, "�����������ظ�ʽ", "����",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// ���� FALSE
	}

	if(!SetPixelFormat(hDC,PixelFormat,&pfd))	// �ܹ��������ظ�ʽô?
	{
		KillGLWindow();							// ������ʾ��
		MessageBox(NULL, "�����������ظ�ʽ", "����",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// ���� FALSE
	}

	if (!(hRC=wglCreateContext(hDC)))			// �ܷ�ȡ����ɫ������?
	{
		KillGLWindow();							// ������ʾ��
		MessageBox(NULL, "���ܴ���OpenGL��Ⱦ������", "����",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// ���� FALSE
	}

	if (!(hRC=wglCreateContext(hDC)))			// �ܷ�ȡ����ɫ������?
	{
		KillGLWindow();							// ������ʾ��
		MessageBox(NULL, "���ܴ���OpenGL��Ⱦ������", "����",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// ���� FALSE
	}

	if(!wglMakeCurrent(hDC,hRC))				// ���Լ�����ɫ������
	{
		KillGLWindow();							// ������ʾ��
		MessageBox(NULL, "���ܼ��ǰ��OpenGL��Ȼ������", "����",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// ���� FALSE
	}

	ShowWindow(hWnd,SW_SHOW);					// ��ʾ����
	SetForegroundWindow(hWnd);					// ����������ȼ�
	SetFocus(hWnd);								// ���ü��̵Ľ������˴���
	ReSizeGLScene(width, height);				// ����͸�� GL ��Ļ

	if (!InitGL())								// ��ʼ���½���GL����
	{
		KillGLWindow();							// ������ʾ��
		MessageBox(NULL, "Initialization Failed.", "ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// ���� FALSE
	}

	return TRUE;								// �ɹ�
}

LRESULT CALLBACK WndProc(	HWND	hWnd,		// ���ڵľ��
				UINT	uMsg,					// ���ڵ���Ϣ
				WPARAM	wParam,					// ���ӵ���Ϣ����
				LPARAM	lParam)					// ���ӵ���Ϣ����
{
	switch (uMsg)								// ���Windows��Ϣ
	{
		case WM_ACTIVATE:						// ���Ӵ��ڼ�����Ϣ
		{
			if (!HIWORD(wParam))				// �����С��״̬
			{
				active=TRUE;					// �����ڼ���״̬
			}
			else
			{
				active=FALSE;					// �����ټ���
			}

			return 0;							// ������Ϣѭ��
		}

		case WM_SYSCOMMAND:						// ϵͳ�ж�����
		{
			switch (wParam)						// ���ϵͳ����
			{
				case SC_SCREENSAVE:				// ����Ҫ����?
				case SC_MONITORPOWER:			// ��ʾ��Ҫ����ڵ�ģʽ?
				return 0;						// ��ֹ����
			}
			break;								// �˳�
		}

		case WM_CLOSE:							// �յ�Close��Ϣ?
		{
			PostQuitMessage(0);					// �����˳���Ϣ
			return 0;							// ����
		}

		case WM_KEYDOWN:						// �м�����ô?
		{
			keys[wParam] = TRUE;				// ����ǣ���ΪTRUE
			return 0;							// ����
		}

		case WM_KEYUP:							// �м��ſ�ô?
		{
			keys[wParam] = FALSE;				// ����ǣ���ΪFALSE
			return 0;							// ����
		}

		case WM_SIZE:							// ����OpenGL���ڴ�С
		{
			ReSizeGLScene(LOWORD(lParam),HIWORD(lParam));// LoWord=Width,HiWord=Height
			return 0;							// ����
		}
	}

	// �� DefWindowProc��������δ�������Ϣ��
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

int WINAPI WinMain(	HINSTANCE	hInstance,		// ��ǰ����ʵ��
			HINSTANCE	hPrevInstance,			// ǰһ������ʵ��
			LPSTR		lpCmdLine,				// �����в���
			int		nCmdShow)					// ������ʾ״̬
{
	MSG	msg;									// Windowsx��Ϣ�ṹ
	BOOL	done=FALSE;							// �����˳�ѭ����Bool ����
	
	// ��ʾ�û�ѡ������ģʽ
	if (MessageBox(NULL,"������ȫ��ģʽ������ô��",  "����ȫ��ģʽ",MB_YESNO|MB_ICONQUESTION)==IDNO)
	{
		fullscreen=FALSE;						// FALSEΪ����ģʽ
	}

	// ����OpenGL����
	if (!CreateGLWindow("OpenGL����ϵͳ",640,480,16,fullscreen))
	{
		return 0;								// ʧ���˳�
	}

	while(!done)								// ����ѭ��ֱ�� done=TRUE
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))// ����Ϣ�ڵȴ���?
		{
			if (msg.message==WM_QUIT)			// �յ��˳���Ϣ?
			{
				done=TRUE;						// �ǣ���done=TRUE
			}
			else								// ���ǣ���������Ϣ
			{
				TranslateMessage(&msg);			// ������Ϣ
				DispatchMessage(&msg);			// ������Ϣ
			}
		}
		else									// ���û����Ϣ
		{
			// ���Ƴ���������ESC��������DrawGLScene()���˳���Ϣ
			if (active)							// ���򼤻��ô?
			{
				if (keys[VK_ESCAPE])			// ESC ������ô?
				{
					done=TRUE;					// ESC �����˳��ź�
				}
				else							// �����˳���ʱ��ˢ����Ļ
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
				// �ؽ� OpenGL ����(�޸�)
				if (!CreateGLWindow("OpenGL����ϵͳ",640,480,16,fullscreen))
				{
					return 0;				// �������δ�ܴ����������˳�
				}
			}

			if (keys[VK_ADD] && (slowdown>1.0f)) slowdown-=0.01f;		// ��+�ţ���������
			if (keys[VK_SUBTRACT] && (slowdown<4.0f)) slowdown+=0.01f;	// ��-�ţ���������
			if (keys[VK_PRIOR]) zoom+=0.1f;		// ��Page Up���������ӿ����ӵ�
			if (keys[VK_NEXT]) zoom-=0.1f;		// ��Page Down��������Զ���ӵ�

			if (keys[VK_RETURN] && !rp)		// ��ס�س������л��ʺ�ģʽ
			{
				rp=true;
				rainbow=!rainbow;
			}
			if (!keys[VK_RETURN]) rp=false;

			if ((keys[' '] && !sp) || (rainbow && (delay>25)))	// �ո�����任��ɫ
			{
				if (keys[' ']) rainbow=false;	
				sp=true;			
				delay=0;			
				col++;				
				if (col>11) col=0;
			}
			if (!keys[' '])	sp=false;		// ����ͷſո������¼���״̬
			// ������������Y����������ٶ�
			if (keys[VK_UP] && (yspeed<200)) yspeed+=1.0f;

			// ���¼�������Y����������ٶ�
			if (keys[VK_DOWN] && (yspeed>-200)) yspeed-=1.0f;

			// ������������X����������ٶ�
			if (keys[VK_RIGHT] && (xspeed<200)) xspeed+=1.0f;

			// �����������X����������ٶ�
			if (keys[VK_LEFT] && (xspeed>-200)) xspeed-=1.0f;

			delay++;			// ���Ӳʺ�ģʽ����ɫ�л��ӳ�
		}
	}
	// �رճ���
	KillGLWindow();								// ���ٴ���
	return (msg.wParam);						// �˳�����
}
