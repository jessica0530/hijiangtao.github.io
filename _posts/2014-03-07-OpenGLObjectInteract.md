---
date: 2014-03-07 22:30:00
layout: post
title: OpenGL学习笔记7：光照和键盘控制
thread: 67
categories: 教程
tags: OpenGL
---

前言：在老师的推荐下准备系统学习下Nehe关于OpenGL的教程，在打开chm的第一眼就被作者的开源奉献精神所深深打动，这种思想与我不谋而合。

以下内容为我学习了教程之后自己的操作与总结，由于硬件与软件环境的差异，我的笔记内容在代码和描述上与原文可能存在一定出路。

----

本笔记记录的内容包括：如何添加光照和键盘控制，让程序看起来更美观。这一课作者教了如何使用三种不同的纹理滤波方式，如何使用键盘来移动场景中的对象，还有在OpenGL场景中应用简单的光照。

最后完成的效果包括：

1. 利用F键对物体纹理进行切换；
2. 利用L键进行物体所受光照方式的切换；
3. 利用PgUp和PgDn两个按键对物体距离屏幕远近进行调节；
4. 利用上下左右四个方向键进行物体旋转方向以及速度的调整。

----

代码最好在第一课的基础上进行修改，但是具体的添加与替换的函数我会尽量记录清楚。

增加三个布尔变量。light 变量跟踪光照是否打开。变量lp和fp用来存储'L' 和'F'键是否按下的状态。

```
BOOL	light;					// 光源的开/关
BOOL	lp;						// L键按下了么?
BOOL	fp;						// F键按下了么?
```

设置5个变量来控制绕x轴和y轴旋转角度的步长，以及绕x轴和y轴的旋转速度。另外创建了一个z变量来控制进入屏幕深处的距离。

```
GLfloat	xrot;								// X 旋转
GLfloat	yrot;								// Y 旋转
GLfloat xspeed;								// X 旋转速度
GLfloat yspeed;								// Y 旋转速度

GLfloat	z=-5.0f;							// 深入屏幕的距离
```

接着设置用来创建光源的数组。我们使用两种不同的光。第一种称为环境光。环境光来自于四面八方。所有场景中的对象都处于环境光的照射中。第二种类型的光源叫做漫射光。漫射光由特定的光源产生，并在您的场景中的对象表面上产生反射。处于漫射光直接照射下的任何对象表面都变得很亮，而几乎未被照射到的区域就显得要暗一些。

创建光源的过程和颜色的创建完全一致。前三个参数分别是RGB三色分量，最后一个是alpha通道参数。然后保存光源的位置。

```
GLfloat LightAmbient[]= { 0.5f, 0.5f, 0.5f, 1.0f }; // 环境光参数
GLfloat LightDiffuse[]= { 1.0f, 1.0f, 1.0f, 1.0f }; // 漫射光参数
GLfloat LightPosition[]= { 0.0f, 0.0f, 2.0f, 1.0f };// 光源位置
```

filter 变量跟踪显示时所采用的纹理类型。第一种纹理(texture 0) 使用gl_nearest(不光滑)滤波方式构建。第二种纹理 (texture 1) 使用gl_linear(线性滤波) 方式，离屏幕越近的图像看起来就越光滑。第三种纹理 (texture 2) 使用 mipmapped滤波方式,这将创建一个外观十分优秀的纹理。根据使用类型，filter 变量的值分别等于 0, 1 或 2 。

GLuint texture[3] 为三种不同纹理分配储存空间。它们分别位于在 texture[0], texture[1] 和 texture[2]中。

```
GLuint	filter;									// 滤波类型
GLuint	texture[3];								// 3种纹理的储存空间
```

加上载入位图图象的函数：

```
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
```

下面这段代码调用前面的代码载入位图，并将其转换成3个纹理。Status 变量跟踪纹理是否已载入并被创建了。

```
int LoadGLTextures()								// 载入位图并转换成纹理
{
	int Status=FALSE;							// 状态指示器

	AUX_RGBImageRec *TextureImage[1];					// 创建纹理的存储空间

	memset(TextureImage,0,sizeof(void *)*1);				// 将指针设为 NULL
	
	// 载入位图，检查有错，或位图不存在的话退出
	if (TextureImage[0]=LoadBMP("1.bmp"))
	{
		Status=TRUE;							// 状态设为 TRUE
		glGenTextures(3, &texture[0]);					// 创建纹理

		// 创建 Nearest 滤波贴图
		glBindTexture(GL_TEXTURE_2D, texture[0]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST); 
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST); 
		glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);

		// 创建线性滤波纹理
		glBindTexture(GL_TEXTURE_2D, texture[1]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);

		// 创建 MipMapped 纹理
		glBindTexture(GL_TEXTURE_2D, texture[2]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST); 
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data); 
	}

	if (TextureImage[0])							// 纹理是否存在
	{
		if (TextureImage[0]->data)					// 纹理图像是否存在
		{
			free(TextureImage[0]->data);				// 释放纹理图像占用的内存
		}

		free(TextureImage[0]);						// 释放图像结构
	}
	return Status;								// 返回 Status 变量
}
```

对了，以上代码中需要注意的是：作者曾经说过有办法可以绕过OpenGL对纹理宽度和高度所加的限制——64、128、256，等等。办法就是 gluBuild2DMipmaps。据他的发现，其实我们可以使用任意的位图来创建纹理。OpenGL将自动将它缩放到正常的大小。

而当您告诉OpenGL创建一个 mipmapped的纹理后，OpenGL将尝试创建不同尺寸的高质量纹理。当您向屏幕绘制一个 mipmapped纹理的时候，OpenGL将选择它已经创建的外观最佳的纹理(带有更多细节)来绘制，而不仅仅是缩放原先的图像(这将导致细节丢失)。

接着应该载入纹理并初始化OpenGL设置了。InitGL函数的第一行使用上面的代码载入纹理。创建纹理之后，我们调用glEnable(GL_TEXTURE_2D)启用2D纹理映射。阴影模式设为平滑阴影。背景色设为黑色，启用深度测试，然后启用优化透视计算。

然后设置环境光的发光量。修改过的int InitGL(GLvoid)函数如下：

```
int InitGL(GLvoid)								            // 此处开始对OpenGL进行所有设置
{
	if (!LoadGLTextures())							// 调用纹理载入子例程
	{
		return FALSE;							// 如果未能载入，返回FALSE
	}

	glEnable(GL_TEXTURE_2D);						// 启用纹理映射
	glShadeModel(GL_SMOOTH);								// 启用阴影平滑
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);					// 黑色背景
	glClearDepth(1.0f);										// 设置深度缓存

	glEnable(GL_DEPTH_TEST);								// 启用深度测试
	glDepthFunc(GL_LEQUAL);									// 所作深度测试的类型
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);		// 告诉系统对透视进行修正

	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);				// 设置环境光
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);				// 设置漫射光
	glLightfv(GL_LIGHT1, GL_POSITION,LightPosition);			// 设置光源位置
	glEnable(GL_LIGHT1);							// 启用一号光源

	return TRUE;											//初始化OK
}
```

下一段代码绘制贴图立方体。这一部分除了增加纹理，其他都与笔记6中内容相似。

```
int DrawGLScene(GLvoid)								// 从这里开始进行所有的绘制
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// 清除屏幕和深度缓存
	glLoadIdentity();								// 重置当前的模型观察矩阵
	glTranslatef(0.0f,0.0f,z);						// 移入/移出屏幕 z 个单位

	glRotatef(xrot,1.0f,0.0f,0.0f);					// 绕X轴旋转
	glRotatef(yrot,0.0f,1.0f,0.0f);					// 绕Y轴旋转
	glBindTexture(GL_TEXTURE_2D, texture[filter]);	// 选择由filter决定的纹理

	glBegin(GL_QUADS);								// 开始绘制四边形
		// 前侧面
		glNormal3f( 0.0f, 0.0f, 1.0f);					// 法线指向观察者
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);	
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);	
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);	
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);	
		// 后侧面
		glNormal3f( 0.0f, 0.0f,-1.0f);					// 法线背向观察者
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);	
		glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);	
		glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);	
		// 顶面
		glNormal3f( 0.0f, 1.0f, 0.0f);					// 法线向上
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);	
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);	
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);	
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);	
		// 底面
		glNormal3f( 0.0f,-1.0f, 0.0f);					// 法线朝下
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	
		glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);	
		glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);	
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);	
		// 右侧面
		glNormal3f( 1.0f, 0.0f, 0.0f);					// 法线朝右
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);	
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);	
		glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);	
		glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);	
		// 左侧面
		glNormal3f(-1.0f, 0.0f, 0.0f);					// 法线朝左
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);	
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);	
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);	
	glEnd();								// 四边形绘制结束
	xrot+=xspeed;								// xrot 增加 xspeed 单位
	yrot+=yspeed;								// yrot 增加 yspeed 单位
	return TRUE;	
}
```

glNormal3f是这一课的新东西。Normal就是法线的意思，所谓法线是指经过面(多边形）上的一点且垂直于这个面(多边形)的直线。使用光源的时候必须指定一条法线。法线告诉OpenGL这个多边形的朝向，并指明多边形的正面和背面。如果没有指定法线，什么怪事情都可能发生：不该照亮的面被照亮了，多边形的背面也被照亮....。对了，法线应该指向多边形的外侧。

现在转入WinMain()主函数。我们将在这里增加开关光源、旋转木箱、切换过滤方式以及将木箱移近移远的控制代码。在接近WinMain()函数结束的地方你会看到SwapBuffers(hDC)这行代码。然后就在这一行后面添加如下的代码。

```
SwapBuffers(hDC);				// 交换缓存
if (keys['L'] && !lp)			// L 键已按下并且松开了?
{
	lp=TRUE;					// lp 设为 TRUE
	light=!light;				// 切换光源的 TRUE/FALSE
	if (!light)					// 如果没有光源
	{
		glDisable(GL_LIGHTING);	// 禁用光源
	}
	else						// 否则
	{
		glEnable(GL_LIGHTING);	// 启用光源
	}
}
if (!keys['L'])					// L键松开了么?
{
	lp=FALSE;					// 若是，则将lp设为FALSE
}

if (keys['F'] && !fp)			// F键按下了么?
{
	fp=TRUE;					// fp 设为 TRUE
	filter+=1;					// filter的值加一
	if (filter>2)				// 大于2了么?
	{
		filter=0;				// 若是重置为0
	}
}
if (!keys['F'])					// F键放开了么?
{
	fp=FALSE;					// 若是fp设为FALSE
}

if (keys[VK_PRIOR])				// PageUp按下了?
{
	z-=0.02f;					// 若按下，将木箱移向屏幕内部
}
if (keys[VK_NEXT])				// PageDown按下了么
{
	z+=0.02f;					// 若按下的话，将木箱移向观察者
}

if (keys[VK_UP])				// Up方向键按下了么?
{
	xspeed-=0.01f;				// 若是,减少xspeed
}
if (keys[VK_DOWN])				// Down方向键按下了么?
{
	xspeed+=0.01f;				// 若是,增加xspeed
}
if (keys[VK_RIGHT])				// Right方向键按下了么?
{
	yspeed+=0.01f;				// 若是,增加yspeed
}
if (keys[VK_LEFT])				// Left方向键按下了么?
{
	yspeed-=0.01f;				// 若是, 减少yspeed
}
```

对了，最后依旧要把OpenGL窗口的文字替换掉：

```
if (keys[VK_F1])				// F1按下了么?
{
	keys[VK_F1]=FALSE;			// 若是将其设为FALSE
	KillGLWindow();				// 销毁当前窗口
	fullscreen=!fullscreen;			// 切换全屏/窗口模式
	// 重建GL窗口
	if (!CreateGLWindow("Textures, Lighting & Keyboard Tutorial",640,480,16,fullscreen))
	{
		return 0;			// 若无法创建窗口，程序退出
	}
}
```

![效果图](/assets/2014-03-07-OpenGLObjectInteract.png "效果图")
<center>效果图</center>

本例代码：[下载地址](/code/DataBlog_OpenGLObjectInteract.cpp)

最后，如果你也对OpenGL感兴趣或者你在学习中有任何想法，欢迎在下方留言、与我交流！祝愿我们共同成长、一起进步！