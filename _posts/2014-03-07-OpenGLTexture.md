---
date: 2014-03-07 09:30:00
layout: post
title: OpenGL学习笔记6：将纹理映射到立方体
thread: 66
categories: 教程
tags: OpenGL
---

前言：在老师的推荐下准备系统学习下Nehe关于OpenGL的教程，在打开chm的第一眼就被作者的开源奉献精神所深深打动，这种思想与我不谋而合。

以下内容为我学习了教程之后自己的操作与总结，由于硬件与软件环境的差异，我的笔记内容在代码和描述上与原文可能存在一定出路。

----

本笔记记录的是如何把一个纹理映射到立方体的六个面。

纹理所具有的好处不仅仅是外观美观，而且由于其可以替代复杂的面片实现一些效果，于是使得程序运行起来会更快速与轻盈。在笔记最初我想记上作者的一段话：

>关于用作纹理的图像我想有几点十分重要，并且您必须明白。此图像的宽和高必须是2的n次方；宽度和高度最小必须是64象素；并且出于兼容性的原因，图像的宽度和高度不应超过256象素。如果您的原始素材的宽度和高度不是64,128,256象素的话，使用图像处理软件重新改变图像的大小。可以肯定有办法能绕过这些限制，但现在我们只需要用标准的纹理尺寸。 

首先，在之前代码基础上开头加上下面几行，用来定义存储纹理等变量。（注意：如果之前没有载入glaux库，请先网上下载这个库的资源包，安放位置与之前OpenGL安装类似，额外需要注意的是需要自己手动到项目设置-LINK-附加资源中填入glaux.lib资源才能使相应操作生效）

```
#include	<stdio.h>							// 标准输入/输出库的头文件
#include	<glaux.h>							// GLaux库的头文件

GLfloat		xrot;								// X 旋转量
GLfloat		yrot;								// Y 旋转量
GLfloat		zrot;								// Z 旋转量

GLuint		texture[1];							// 存储一个纹理
```

然后，创建一个文件句柄。句柄是个用来鉴别资源的数值，它使程序能够访问此资源。创建一个文件句柄。句柄是个用来鉴别资源的数值，它使程序能够访问此资源。

在这里需要注意的是，因为 LoadBMP() 可以无参数调用，所以不得不检查一下。接下来还有一个操作时需要你去检查文件是否能打开，如果可以的话，很显然文件是存在的。使用 fclose(File) 关闭文件。 auxDIBImageLoad(Filename) 读取图象数据并将其返回。

```
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
```

下一部分代码载入位图(调用上面的代码)并转换成纹理。在如下代码中需要注意的是：LoadBMP()函数中的参数我设置的是OpenGLTest.bmp，这是一个相对路径，它相对的是你当前cpp文件所在的文件位置。当然从网上的资料来看，我们也可以使用绝对路径，**但当我使用C:\Program Files\OpenGLTest.bmp，不知道为什么程序运行起来一直报错Initialized Failed!如果你知道的话，非常欢迎指点一二，这个问题困扰了我很久很久。**

```
int LoadGLTextures()								// 载入位图(调用上面的代码)并转换成纹理
{
	int Status=FALSE;							// 状态指示器
	AUX_RGBImageRec *TextureImage[1];					// 创建纹理的存储空间
	memset(TextureImage,0,sizeof(void *)*1);				// 将指针设为 NULL

	// 载入位图，检查有无错误，如果位图没找到则退出
	if (TextureImage[0]=LoadBMP("OpenGLTest.bmp"))
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
```

接下来是改写InitGL。if (!LoadGLTextures()) 这行代码调用上面讲的子例程载入位图并生成纹理。如果因为任何原因 LoadGLTextures() 调用失败，接着的一行返回FALSE。如果一切OK，并且纹理创建好了，我们启用2D纹理映射。如果您忘记启用的话，您的对象看起来永远都是纯白色，这一定不是什么好事。

```
int InitGL(GLvoid)								// 此处开始对OpenGL进行所有设置
{
	if (!LoadGLTextures())						// 调用纹理载入子例程
	{
		return FALSE;							// 如果未能载入，返回FALSE
	}

	glEnable(GL_TEXTURE_2D);					// 启用纹理映射
	glShadeModel(GL_SMOOTH);					// 启用阴影平滑
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);		// 黑色背景
	glClearDepth(1.0f);							// 设置深度缓存
	glEnable(GL_DEPTH_TEST);					// 启用深度测试
	glDepthFunc(GL_LEQUAL);						// 所作深度测试的类型
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// 真正精细的透视修正
	return TRUE;								// 初始化 OK
}

```

本例中的代码是建立一个立方体，并且使立方体绕X、Y、Z轴旋转。同时贴上了我们设置的纹理。试着玩玩 glTexCoord2f 的X，Y坐标参数。把 1.0f 改为 0.5f 将只显示纹理的左半部分，把 0.0f 改为 0.5f 将只显示纹理的右半部分。

```
int DrawGLScene(GLvoid)							// 从这里开始进行所有的绘制
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);// 清除屏幕和深度缓存
	glLoadIdentity();							// 重置当前的模型观察矩阵
	glTranslatef(0.0f,0.0f,-5.0f);				// 移入屏幕 5 个单位

	glRotatef(xrot,1.0f,0.0f,0.0f);				// 绕X轴旋转
	glRotatef(yrot,0.0f,1.0f,0.0f);				// 绕Y轴旋转
	glRotatef(zrot,0.0f,0.0f,1.0f);				// 绕Z轴旋转

	glBindTexture(GL_TEXTURE_2D, texture[0]);	// 选择纹理

	glBegin(GL_QUADS);
		// 前面
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);	// 纹理和四边形的左下
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);	// 纹理和四边形的右下
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);	// 纹理和四边形的右上
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);	// 纹理和四边形的左上
		// 后面
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	// 纹理和四边形的右下
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);	// 纹理和四边形的右上
		glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);	// 纹理和四边形的左上
		glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);	// 纹理和四边形的左下
		// 顶面
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);	// 纹理和四边形的左上
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);	// 纹理和四边形的左下
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);	// 纹理和四边形的右下
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);	// 纹理和四边形的右上
		// 底面
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	// 纹理和四边形的右上
		glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);	// 纹理和四边形的左上
		glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);	// 纹理和四边形的左下
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);	// 纹理和四边形的右下
		// 右面
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);	// 纹理和四边形的右下
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);	// 纹理和四边形的右上
		glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);	// 纹理和四边形的左上
		glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);	// 纹理和四边形的左下
		// 左面
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	// 纹理和四边形的左下
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);	// 纹理和四边形的右下
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);	// 纹理和四边形的右上
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);	// 纹理和四边形的左上
	glEnd();

	xrot+=0.3f;								// X 轴旋转
	yrot+=0.2f;								// Y 轴旋转
	zrot+=0.4f;								// Z 轴旋转
	return true;							// 继续运行
}
```

点击运行，如果成功的话就会出现如下类似的一个旋转立方体。

![效果图](/assets/2014-03-07-OpenGLTexture.bmp "效果图")
<center>效果图</center>

本例代码：[下载地址](/code/DataBlog_OpenGLTexture.cpp)（注意素材的尺寸与参数替换）

最后，如果你也对OpenGL感兴趣或者你在学习中有任何想法，欢迎在下方留言、与我交流！祝愿我们共同成长、一起进步！