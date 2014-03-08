---
date: 2014-03-08 15:00:00
layout: post
title: OpenGL学习笔记9：3D空间中移动图像
thread: 70
categories: 教程
tags: OpenGL
---

前言：在老师的推荐下准备系统学习下Nehe关于OpenGL的教程，在打开chm的第一眼就被作者的开源奉献精神所深深打动，这种思想与我不谋而合。

以下内容为我学习了教程之后自己的操作与总结，由于硬件与软件环境的差异，我的笔记内容在代码和描述上与原文可能存在一定出路。

----

本笔记记录的内容包括：3D空间中移动图像。在3D场景中移动位图，并去除位图上的黑色象素(使用混色)。接着为黑白纹理上色，最后您将学会创建丰富的色彩，并把上过不同色彩的纹理相互混合，得到简单的动画效果。

这节课的代码实在笔记1的基础上进行修改，首先还是在源程序的开始处增加几个变量。

```
#include <stdio.h>			// 标准输入输出库头文件
#include <gl/GLAUX.H>			// GLaux库的头文件
```

twinkle和 tp是布尔变量, 表示它们只能设为 TRUE 或 FALSE。 twinkle用来跟踪 闪烁 效果是否启用。 tp用来检查 'T'键有没有被按下或松开. (按下时 tp=TRUE, 松开时 tp=FALSE).

```
BOOL	twinkle;					// 闪烁的星星
BOOL	tp;							// 'T' 按下了么? 
const	int num=50;					// 绘制的星星数
```

接下来创建一个结构。其中定义三个int变量记录星星的颜色，dist保持对距离的跟踪，angle保持对角度的追踪。

```
typedef struct						// 为星星创建一个结构
{
	int r, g, b;					// 星星的颜色
	GLfloat dist;					// 星星距离中心的距离
	GLfloat angle;					// 当前星星所处的角度
}stars;								// 结构命名为stars
stars star[num];					// 使用 'stars' 结构生成一个包含 'num'个元素的 'star'数组
```

接下来我们设置几个跟踪变量：星星离观察者的距离变量(zoom)，我们所见到的星星所处的角度(tilt)，以及使闪烁的星星绕Z轴自转的变量spin。

loop变量用来绘制50颗星星。texture[1]用来存放一个黑白纹理。如果您需要更多的纹理的话，您应该增加texture数组的大小至您决定采用的纹理个数。

```
GLfloat	zoom=-15.0f;						// 星星离观察者的距离
GLfloat tilt=90.0f;						// 星星的倾角
GLfloat	spin;							// 闪烁星星的自转

GLuint	loop;							// 全局 Loop 变量
GLuint	texture[1];						// 存放一个纹理
```

紧接着上面的代码就是我们用来载入纹理的代码。

```
AUX_RGBImageRec *LoadBMP(char *Filename)			// 载入位图文件
{
	FILE *File=NULL;					// 文件句柄

	if (!Filename)						// 确认已给出文件名
	{
		return NULL;					// 若无返回 NULL
	}

	File=fopen(Filename,"r");				// 检查文件是否存在

	if (File)						// 文件存在么?
	{
		fclose(File);					// 关闭文件句柄
		return auxDIBImageLoad(Filename);		// 载入位图并返回指针
	}
	return NULL;						// 如果载入失败返回 NULL
}
```

下面的代码(调用上面的代码)载入位图，并转换成纹理。

```
int LoadGLTextures()						// 载入位图并转换成纹理
{
	int Status=FALSE;					// 状态指示器

	AUX_RGBImageRec *TextureImage[1];			// 为纹理分配存储空间

	memset(TextureImage,0,sizeof(void *)*1);		// 将指针设为 NULL

	// 载入位图，查错，如果未找到位图文件则退出
	if (TextureImage[0]=LoadBMP("Star.bmp"))
	{
		Status=TRUE;					// 将 Status 设为TRUE

		glGenTextures(1, &texture[0]);			// 创建一个纹理

		// 创建一个线性滤波纹理
		glBindTexture(GL_TEXTURE_2D, texture[0]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);
	}

	if (TextureImage[0])					// 如果纹理存在
	{
		if (TextureImage[0]->data)			// 如果纹理图像存在
		{
			free(TextureImage[0]->data);		// 释放纹理图像所占的内存
		}

		free(TextureImage[0]);				// 释放图像结构
	}
	return Status;						// 返回 Status的值
}
```

现在设置OpenGL的渲染方式。其中需要设置每颗星星的起始角度、距离、和颜色。全部50颗星星都会被循环设置。要改变star[1]的角度我们所要做的只是star[1].angle={某个数值}。

```
int InitGL(GLvoid)						// 此处开始对OpenGL进行所有设置
{
	if (!LoadGLTextures())					// 调用纹理载入子例程
	{
		return FALSE;					// 如果未能载入，返回FALSE
	}

	glEnable(GL_TEXTURE_2D);				// 启用纹理映射
	glShadeModel(GL_SMOOTH);				// 启用阴影平滑
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);			// 黑色背景
	glClearDepth(1.0f);					// 设置深度缓存
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// 真正精细的透视修正
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);			// 设置混色函数取得半透明效果
	glEnable(GL_BLEND);					// 启用混色
	for (loop=0; loop<num; loop++)				// 创建循环设置全部星星
	{
		star[loop].angle=0.0f;				// 所有星星都从零角度开始
		star[loop].dist=(float(loop)/num)*5.0f;		// 计算星星离中心的距离
		star[loop].r=rand()%256;			// 为star[loop]设置随机红色分量
		star[loop].g=rand()%256;			// 为star[loop]设置随机红色分量
		star[loop].b=rand()%256;			// 为star[loop]设置随机红色分量
	}
	return TRUE;						// 初始化一切OK
}
```

接下来对DrawGLScene()函数操作。现在我们可以开始移动星星。星星开始时位于屏幕的中心。我们要做的第一件事是把场景沿Y轴旋转。如果我们旋转90度的话，X轴不再是自左至右的了，他将由里向外穿出屏幕。为了让大家更清楚些，举个例子。假想您站在房子中间。再设想您左侧的墙上写着-x，前面的墙上写着-z，右面墙上就是+x咯，您身后的墙上则是+z。假如整个房子向右转90度，但您没有动，那么前面的墙上将是-x而不再是-z了。所有其他的墙也都跟着移动。-z出现在右侧，+z出现在左侧，+x出现在您背后。神经错乱了吧？通过旋转场景，我们改变了x和z平面的方向。

第二行代码沿x轴移动一个正值。通常x轴上的正值代表移向了屏幕的右侧(也就是通常的x轴的正向)，但这里由于我们绕y轴旋转了坐标系，x轴的正向可以是任意方向。如果我们转180度的话，屏幕的左右侧就镜像反向了。因此，当我们沿 x轴正向移动时，可能向左，向右，向前或向后。

现在您在屏幕中心画了个平面的四边形然后贴上纹理，这看起来很不错。一切都如您所想的那样。但是当您当您沿着y轴转上个90度的话，纹理在屏幕上就只剩右侧和左侧的两条边朝着您。看起来就是一条细线。这不是我们所想要的。我们希望星星永远正面朝着我们，而不管屏幕如何旋转或倾斜。

我们通过在绘制星星之前，抵消对星星所作的任何旋转来实现这个愿望。您可以采用逆序来抵消旋转。当我们倾斜屏幕时，我们实际上以当前角度旋转了星星。通过逆序，我们又以当前角度"反旋转"星星。也就是以当前角度的负值来旋转星星。就是说，如果我们将星星旋转了10度的话，又将其旋转-10度来使星星在那个轴上重新面对屏幕。下面的第一行抵消了沿y轴的旋转。然后，我们还需要抵消掉沿x轴的屏幕倾斜。要做到这一点，我们只需要将屏幕再旋转-tilt倾角。在抵消掉x和y轴的旋转后，星星又完全面对着我们了。

```
int DrawGLScene(GLvoid)						// 此过程中包括所有的绘制代码
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// 清除屏幕及深度缓存
	glBindTexture(GL_TEXTURE_2D, texture[0]);		// 选择纹理

	for (loop=0; loop<num; loop++)				// 循环设置所有的星星
	{
		glLoadIdentity();				// 绘制每颗星星之前，重置模型观察矩阵
		glTranslatef(0.0f,0.0f,zoom);			// 深入屏幕里面
		glRotatef(tilt,1.0f,0.0f,0.0f);			// 倾斜视角
		glRotatef(star[loop].angle,0.0f,1.0f,0.0f);	// 旋转至当前所画星星的角度
		glTranslatef(star[loop].dist,0.0f,0.0f);	// 沿X轴正向移动
		glRotatef(-star[loop].angle,0.0f,1.0f,0.0f);	// 取消当前星星的角度
		glRotatef(-tilt,1.0f,0.0f,0.0f);		// 取消屏幕倾斜

		if (twinkle)					// 启用闪烁效果
		{
			// 使用byte型数值指定一个颜色
			glColor4ub(star[(num-loop)-1].r,star[(num-loop)-1].g,star[(num-loop)-1].b,255);
			glBegin(GL_QUADS);			// 开始绘制纹理映射过的四边形
				glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,-1.0f, 0.0f);
				glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,-1.0f, 0.0f);
				glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f, 1.0f, 0.0f);
				glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 0.0f);
			glEnd();				// 四边形绘制结束
		}

		glRotatef(spin,0.0f,0.0f,1.0f);			// 绕z轴旋转星星
		// 使用byte型数值指定一个颜色
		glColor4ub(star[loop].r,star[loop].g,star[loop].b,255);
		glBegin(GL_QUADS);				// 开始绘制纹理映射过的四边形
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,-1.0f, 0.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,-1.0f, 0.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f, 1.0f, 0.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 0.0f);
		glEnd();					// 四边形绘制结束
		spin+=0.01f;					// 星星的公转
		star[loop].angle+=float(loop)/num;		// 改变星星的自转角度
		star[loop].dist-=0.01f;				// 改变星星离中心的距离

		if (star[loop].dist<0.0f)			// 星星到达中心了么
		{
			star[loop].dist+=5.0f;			// 往外移5个单位
			star[loop].r=rand()%256;		// 赋一个新红色分量
			star[loop].g=rand()%256;		// 赋一个新绿色分量
			star[loop].b=rand()%256;		// 赋一个新蓝色分量
		}
	}
	return TRUE;						// 一切正常
}
```

现在我们添加监视键盘的代码。下移到WinMain()。找到SwapBuffers(hDC)一行。我们就在这一行后面增加键盘监视代码。代码将检查T键、上下方向键、向上翻页键或向下翻页键是否已按下。

```
if (keys['T'] && !tp)				// 是否T 键已按下并且 tp值为 FALSE
{
	tp=TRUE;				// 若是，将tp设为TRUE
	twinkle=!twinkle;			// 翻转 twinkle的值
}

if (!keys['T'])					// T 键已松开了么？
{
	tp=FALSE;				// 若是 ，tp为 FALSE
}
if (keys[VK_UP])				// 上方向键按下了么？
{
	tilt-=0.5f;				// 屏幕向上倾斜
}

if (keys[VK_DOWN])				// 下方向键按下了么？
{
	tilt+=0.5f;				// 屏幕向下倾斜
}

if (keys[VK_PRIOR])				// 向上翻页键按下了么
{
	zoom-=0.2f;				// 缩小
}

if (keys[VK_NEXT])				// 向下翻页键按下了么？
{
	zoom+=0.2f;				// 放大
}
```

最后还是要修改一下窗口的标题。

```
            if (keys[VK_F1])				// F1键按下了么?
			{
				keys[VK_F1]=FALSE;			// 若是，使对应的Key数组中的值为 FALSE
				KillGLWindow();				// 销毁当前的窗口
				fullscreen=!fullscreen;			// 切换 全屏 / 窗口 模式
				// 重建 OpenGL 窗口
				if (!CreateGLWindow("透明纹理实例",640,480,16,fullscreen))
				{
					return 0;			// 如果窗口未能创建，程序退出
				}
			}
```

本例代码：[下载地址](/code/DataBlog_OpenGLObject3DMove.cpp)

----

最后，如果你也对OpenGL感兴趣或者你在学习中有任何想法，欢迎在下方留言、与我交流！祝愿我们共同成长、一起进步！