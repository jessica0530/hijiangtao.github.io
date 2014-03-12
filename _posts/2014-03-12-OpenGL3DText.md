---
date: 2014-03-12 11:30:00
layout: post
title: OpenGL学习笔记13：创建3D图形字体
thread: 77
categories: 教程
tags: OpenGL
---

前言：在老师的推荐下准备系统学习下Nehe关于OpenGL的教程，在打开chm的第一眼就被作者的开源奉献精神所深深打动，这种思想与我不谋而合。

以下内容为我学习了教程之后自己的操作与总结，由于硬件与软件环境的差异，我的笔记内容在代码和描述上与原文可能存在一定出路。

----

本笔记记录的信息有：如何绘制一个像3D模型一样会旋转的3D文字。

轮廓字体可以在屏幕中以3D方式运动，而且轮廓字体还可以有一定的厚度！而不是平面的2D字符。使用轮廓字体，你可以将你的计算机中的任何字体转换为OpenGL中的3D字体，加上合适的法线，在有光照的时候，字符就会被很好的照亮了。

此处依旧是从笔记《[OpenGL学习笔记1：创建一个OpenGL窗口](http://hijiangtao.github.io/2014/03/06/CreateAOpenGLWindow/)》的代码基础上来进行修改，首先是添加头文件，将原有的头文件换成如下所示：

```
#include <windows.h>		// Windows的头文件
#include <gl\GL.h>
#include <gl\GLU.h>
#include <gl\glut.h>		// 包含OpenGL实用库
#include <gl\glaux.h>
#include <math.h>
#include <stdio.h>
#include <stdarg.h>
```

另外，我们还要添加3个变量。

1. base将保存我们创建的第一个显示列表的编号。每个字符都需要有自己的显示列表。例如，字符‘A’在显示列表中是65，‘B’是66，‘C’是67，等等。所以，字符‘A’应保存在显示列表中的base + 65这个位置。 
2. rot的变量用来配合SIN和COS函数在屏幕上旋转文字，同时用它来改变文字的颜色。
3. GLYPHMETRICSFLOAT gmf[256]用来保存256个轮廓字体显示列表中对应的每一个列表的位置和方向的信息。我们通过gmf[num]来选择字母。num就是我们想要了解的显示列表的编号。

```
GLuint	base;			// 绘制字体的显示列表的开始位置
GLfloat	rot;			// 旋转字体
GLYPHMETRICSFLOAT gmf[256];	// 记录256个字符的信息
```

之后，构建真正的字体的代码类似于创建位图字体的方法，但其中需要注意的是使用wglUseFontOutlines函数替换wglUseFontBitmaps函数，并且之前的old font也不再使用，完整的BuildFont()代码如下所示：

```
GLvoid BuildFont(GLvoid)			// 创建位图字体
{
	HFONT	font;					// 字体句柄
	base = glGenLists(256);			// 创建256个显示列表	
	font = CreateFont(	-12,		// Height Of Font
						0,			// Width Of Font
						0,			// Angle Of Escapement
						0,			// Orientation Angle
						FW_BOLD,	// Font Weight
						FALSE,		// Italic
						FALSE,		// Underline
						FALSE,		// Strikeout
						ANSI_CHARSET,// Character Set Identifier
						OUT_TT_PRECIS,// Output Precision
						CLIP_DEFAULT_PRECIS,// Clipping Precision
						ANTIALIASED_QUALITY,// Output Quality
						FF_DONTCARE|DEFAULT_PITCH,// Family And Pitch
						"Comic Sans MS");// Font Name
	SelectObject(hDC, font);
	wglUseFontOutlines(	hDC,// 设置当前窗口设备描述表的句柄
				0,				    // 用于创建显示列表字体的第一个字符的ASCII值
				255,				// 字符数
				base,				// 第一个显示列表的名称
				0.0f,				// 字体的光滑度，越小越光滑，0.0为最光滑的状态
				0.2f,				// 在z方向突出的距离
				WGL_FONT_POLYGONS,	// 使用多边形来生成字符，每个顶点具有独立的法线
				gmf);				//一个接收字形度量数据的数组的地址，每个数组元素用它对应的显示列表字符的数据填充 
}
```

接下来就是被教程中誉为优异的GL文字程序了。你可以通过调用glPrint(“需要写的文字”)来调用这段代码。文字被存储在字符串text[]中。

```
GLvoid glPrint(const char *fmt, ...)	// 自定义GL输出字体函数
{
	float		length=0;				// 查询字符串的长度
	char		text[256];				// 保存我们想要的文字串
	va_list		ap;						// 指向一个变量列表的指针
	if (fmt == NULL)					// 如果无输入则返回
		return;	
	va_start(ap, fmt);					// 分析可变参数
	vsprintf(text, fmt, ap);			// 把参数值写入字符串
	va_end(ap);							// 结束分析
	for (unsigned int loop=0;loop<(strlen(text));loop++)// 查找整个字符串的长度
	{
		length+=gmf[text[loop]].gmfCellIncX;		
	}
	glTranslatef(-length/2,0.0f,0.0f);			// 把字符串置于最左边
	glPushAttrib(GL_LIST_BIT);				// 把显示列表属性压入属性堆栈
	glListBase(base);					// 设置显示列表的基础值为0
	glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);	// 调用显示列表绘制字符串
	glPopAttrib();					// 弹出属性堆栈
}
```

下面就是画图的代码了。从清除屏幕和深度缓存开始，我们调用glLoadIdentity()来重置所有东西。然后将坐标系向屏幕里移动十个单位。轮廓字体在透视图模式下表现非常好。将文字移入屏幕越深，文字开起来就更小。文字离你越近，它看起来就更大。 

也可以使用glScalef(x,y,z)命令来操作轮廓字体。如果你想把字体放大两倍，可以使用glScalef(1.0f,2.0f,1.0f). 2.0f 作用在y轴， 它告诉OpenGL将显示列表的高度绘制为原来的两倍。如果2.0f作用在x轴，那么文本的宽度将变成原来的两倍。

```
int DrawGLScene(GLvoid)					// 此过程中包括所有的绘制代码
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// 清除屏幕及深度缓存
	glLoadIdentity();					// 重置当前的模型观察矩阵
	glTranslatef(0.0f,0.0f,-10.0f);		// 移入屏幕一个单位
	glRotatef(rot,1.0f,0.0f,0.0f);		// 沿X轴旋转
	glRotatef(rot*1.5f,0.0f,1.0f,0.0f);	// 沿Y轴旋转
	glRotatef(rot*1.4f,0.0f,0.0f,1.0f);	// 沿Z轴旋转
	// 根据字体位置设置颜色
	glColor3f(1.0f*float(cos(rot/20.0f)),1.0f*float(sin(rot/25.0f)),1.0f-0.5f*float(cos(rot/17.0f)));
	glPrint("Open 3D Text - %3.2f",rot/50);				// 输出文字到屏幕
	rot+=0.5f;						// 增加旋转变量
	return TRUE;					// 成功返回
}
```

对了，记得在InitGL(GLvoid)函数中添加

```
glEnable(GL_LIGHT0);			// Enable Default Light (Quick And Dirty)
glEnable(GL_LIGHTING);			// Enable Lighting
glEnable(GL_COLOR_MATERIAL);	// Enable Coloring Of Material

BuildFont();					// Build The Font
```

编译运行效果如下所示：

![](/assets/2014-03-12-OpenGL3DText.png)

本笔记运行代码：[下载地址](/code/DataBlog_OpenGL3DText.cpp)

----

最后，如果你也对OpenGL感兴趣或者你在学习中有任何想法，欢迎在下方留言、与我交流！祝愿我们共同成长、一起进步！