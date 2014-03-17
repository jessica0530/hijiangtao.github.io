---
date: 2014-03-17 14:30:00
layout: post
title: OpenGL学习笔记16：2D图像文字
thread: 81
categories: 教程
tags: OpenGL
---

前言：在老师的推荐下准备系统学习下Nehe关于OpenGL的教程，在打开chm的第一眼就被作者的开源奉献精神所深深打动，这种思想与我不谋而合。

以下内容为我学习了教程之后自己的操作与总结，由于硬件与软件环境的差异，我的笔记内容在代码和描述上与原文可能存在一定出路。

----

这几天事情特别多，忙的没时间整理笔记。本笔记记录的信息有：如何使用四边形纹理贴图把文字显示在屏幕上。本教程的作者是NeHe和Giuseppe D'Agata，程序基于第一篇笔记《[OpenGL学习笔记1：创建一个OpenGL窗口](http://hijiangtao.github.io/2014/03/06/CreateAOpenGLWindow/)》的代码。

在程序的第一段，我们包括数学（math）和标准输入输出库（stdio）等信息。

```
#include <windows.h>		// Windows的头文件
#include <gl/GL.h>
#include <gl/GLU.h>
#include <gl/glut.h>		// 包含OpenGL实用库
#include <math.h>
#include <stdio.h>
#include <gl/GLAUX.H>
```

我们将要加入一个变量base来指向我们的显示列表，还加入texture[2]来保存我们将要创建的两个纹理。Texture 1将是字体纹理，texture 2将是用来创建简单3D物体的凹凸纹理。 我们加入用来执行循环的变量loop。最后我们加入用来绕屏幕移动文字和旋转3D物体的cnt1和cnt2。

```
GLuint	base;			// 绘制字体的显示列表的开始位置
GLuint	texture[2];		// 保存字体纹理
GLuint	loop;			// 通用循环变量

GLfloat	cnt1;			// 字体移动计数器1
GLfloat	cnt2;			// 字体移动计数器2
```

接下来是读取纹理代码，这和之前的笔记所述内容相同。

```
AUX_RGBImageRec *LoadBMP(char *Filename)		// 载入位图图象
{
	FILE *File=NULL;							// 文件句柄
	if (!Filename)								// 确保文件名已提供
	{
		return NULL;							// 如果没提供，返回 NULL
	}
	File=fopen(Filename,"r");					// 尝试打开文件

	if (File)								    // 文件存在么?
	{
		fclose(File);							// 关闭句柄
		return auxDIBImageLoad(Filename);		// 载入位图并返回指针
	}

	return NULL;								// 如果载入失败，返回 NULL
}
```

读取纹理第二部分的代码如下所示，但是其中需要注意几个问题：

1. 注意TextureImage[ ]将保存2个rgb图像记录。复查处理读取或存储纹理的纹理很重要。一个错误的数字可能导致内存溢出或崩溃！
2. 对于代码`glGenTextures(2, &texture[0]);`，如果你用1替换2，那么将只创建一个纹理，第二个纹理将显示为全白。如果你用3替换2，你的程序可能崩溃！你应该只调用glGenTextures()一次。调用glGenTextures()后你应该创建你的所有纹理。我曾见过有人在每创建一个纹理前都加上一行glGenTextures()。这通常导致新建的纹理覆盖了你之前创建的。决定你需要创建多少个纹理是个好主意，调用glGenTextures()一次，然后创建所有的纹理。

```
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
```

现在我们将创建字体。其中，cx将用来保存纹理中水平方向的位置，cy将用来保存纹理中竖直方向的位置。接着变量base将指向第一个显示列表的位置。第二个显示列表将是base+1，第三个是base+2，以此类推。 

其中，设置正交投影屏幕的方式是，(0,0)将是屏幕的左下角，(640,480)是屏幕的右上角。x轴上0是屏幕的左边界，639是右边界。y轴上0时下便捷，479是上便捷。基本上我们避免了负坐标。

剩余的信息都写在如下代码中：

```
GLvoid BuildFont(GLvoid)			    // 创建我们的字符显示列表
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
	glEnd();						    // 四边形字符绘制完成
	glTranslated(10,0,0);				// 绘制完一个字符，向右平移16个单位
		glEndList();					// 字符显示列表结束
	}									// 循环建立256个显示列表
}
```

在程序退出前释放显示列表，所有自base开始的256个显示列表都将被销毁。 

```
GLvoid KillFont(GLvoid)								
{
	glDeleteLists(base,256);// 从内存中删除256个显示列表
}
```

下一段代码将完成绘图。glPrint()有三个参数。第一个是屏幕上x轴上的位置（从左至右的位置），下一个是y轴上的位置（从下到上...0是底部，越往上越大）。然后是字符串（我们想打印的文字），最后是一个叫做set的变量。如果你看过Giuseppe D'Agata制作的位图，你会注意到有两个不同的字符集。第一个字符集是普通的，第二个是斜体的。如果set为0，第一个字符集被选中。若set为1则选择第二个字符集。

```
GLvoid glPrint(GLint x, GLint y, char *string, int set)	// 绘制字符
{
	if (set>1)								// 如果字符集大于1
	{
		set=1;								// 设置其为1
	}
	glBindTexture(GL_TEXTURE_2D, texture[0]);	// 绑定为字体纹理
	glDisable(GL_DEPTH_TEST);						// 禁止深度测试
	
	glMatrixMode(GL_PROJECTION);			// 选择投影矩阵
	glPushMatrix();								// 保存当前的投影矩阵

	glLoadIdentity();					// 重置投影矩阵
	glOrtho(0,640,0,480,-1,1);			// 设置正投影的可视区域

	glMatrixMode(GL_MODELVIEW);			// 选择模型变换矩阵
	glPushMatrix();						// 保存当前的模型变换矩阵
	glLoadIdentity();					// 重置模型变换矩阵

	glTranslated(x,y,0);				// 把字符原点移动到(x,y)位置
	glListBase(base-32+(128*set));		// 选择字符集
	glCallLists(strlen(string),GL_BYTE,string);	// 把字符串写入到屏幕 
	glMatrixMode(GL_PROJECTION);		// 选择投影矩阵
	glPopMatrix();						// 设置为保存的矩阵

	glMatrixMode(GL_MODELVIEW);		// 选择模型矩阵
	glPopMatrix();						// 设置为保存的矩阵
}
```

将InitGL()换成如下代码：

```
int InitGL(GLvoid)					// 此处开始对OpenGL进行所有设置
{
	if (!LoadGLTextures())				// 调用纹理载入子例程
	{
		return FALSE;					// 如果未能载入，返回FALSE
	}
	BuildFont();						// 创建字符显示列表
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);			// 黑色背景 
	glClearDepth(1.0);						// 设置深度缓存
	glDepthFunc(GL_LEQUAL);				// 所作深度测试的类型
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);			// 设置混合因子
	glShadeModel(GL_SMOOTH);				// 启用阴影平滑
	glEnable(GL_TEXTURE_2D);				// 启用纹理映射
	return TRUE;						// 初始化成功
}
```

下面这段代码将完成绘图。我们先绘制3D物体最后绘制文字，这样文字将显示在3D物体上面，而不会被3D物体遮住。我之所以加入一个3D物体是为了演示透视投影和正交投影可同时使用。

```
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
```

最后，在KillGLWindow()的最后添加KillFont()。

```
KillFont();	// 删除字体
```

----

程序运行效果如下：

![效果图](/assets/2014-03-17-OpenGL2DText.png "效果图")

本笔记运行代码：[下载地址](/code/DataBlog_OpenGL2DText.cpp)

----

最后，如果你也对OpenGL感兴趣或者你在学习中有任何想法，欢迎在下方留言、与我交流！祝愿我们共同成长、一起进步！