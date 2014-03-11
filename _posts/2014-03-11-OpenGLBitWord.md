---
date: 2014-03-11 11:30:00
layout: post
title: OpenGL学习笔记12：创建2D图像字体
thread: 76
categories: 教程
tags: OpenGL
---

前言：在老师的推荐下准备系统学习下Nehe关于OpenGL的教程，在打开chm的第一眼就被作者的开源奉献精神所深深打动，这种思想与我不谋而合。

以下内容为我学习了教程之后自己的操作与总结，由于硬件与软件环境的差异，我的笔记内容在代码和描述上与原文可能存在一定出路。

----

本笔记记录的信息有：如何使用位图字体，并让其实现基本的移动与变色效果。可能会有人想用一张有文字的图片就能实现这个功能，但是相比之下，文字的展现速度与效果都是更为便捷的。另外需要注意的是，教程的实现方法是专门针对Windows写的，它使用了Windows的wgl函数来创建字体。

首先，添加一些需要使用到的头文件：

```
#include <stdarg.h>		// 用来定义可变参数的头文件
#include <math.h>
#include <stdio.h>
```

 另外，我们还要添加3个变量。base将保存我们创建的第一个显示列表的编号。每个字符都需要有自己的显示列表。例如，字符‘A’在显示列表中是65，‘B’是66，‘C’是67，等等。所以，字符‘A’应保存在显示列表中的base + 65这个位置。

然后添加两个计数器（cnt1 和 cnt2），它们采用不用的累加速度，通过SIN和COS函数来改变文字在屏幕上的位置。在屏幕上创造出一种看起来像是半随机的移动方式。同时，我们用这两个计数器来改变文字的颜色。

```
GLuint	base;			// 绘制字体的显示列表的开始位置
GLfloat	cnt1;			// 字体移动计数器1
GLfloat	cnt2;			// 字体移动计数器2
```

下面这段代码用来构建真实的字体，这是相当难理解的一部分代码。‘HFONT font’告诉Windows我们将要使用一个Windows字体。Oldfont用来存放字体。 

```
GLvoid BuildFont(GLvoid)				// 创建位图字体
{
	HFONT	font;						// 字体句柄
	HFONT	oldfont;					// 旧的字体句柄

	base = glGenLists(96);				// 创建96个显示列表
	font = CreateFont(	-24, 0, 0, 0, FW_BOLD, FALSE,  FALSE, FALSE, 
                        ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, 
                        ANTIALIASED_QUALITY, FF_DONTCARE|DEFAULT_PITCH, 
                        "Courier New");
	oldfont = (HFONT)SelectObject(hDC, font);// 选择我们需要的字体
	wglUseFontBitmaps(hDC, 32, 96, base);// 创建96个显示列表，绘制从ASCII码为32-128的字符
	SelectObject(hDC, oldfont);			// 选择原来的字体
	DeleteObject(font);					// 删除字体
}
```

其中，在创建字体的时候，需要注意CreateFont()函数中的众多参数设置：

```
BOOL CreateFont(
   int nHeight,
   int nWidth,
   int nEscapement,
   int nOrientation,
   int nWeight,
   BYTE bItalic,
   BYTE bUnderline,
   BYTE cStrikeOut,
   BYTE nCharSet,
   BYTE nOutPrecision,
   BYTE nClipPrecision,
   BYTE nQuality,
   BYTE nPitchAndFamily,
   LPCTSTR lpszFacename 
);
```

**int nHeight**, // 以逻辑单位指定字体字符元或字符的高度值（字符高度值是字符元高度值减去内部行距值）当nHeight>0时，字体映射程序将该值转换为设备单位，并将它与可用字体的字符元高度进行匹配，当nHeight=0时，则将其转换为设备单位，并将其绝对值与可用字体的字符高度进行匹配，当nHeight<0时，则匹配一个默认的高度值

**int nWidth**, // 以逻辑单位指定字符的平均宽度，nWidth=0时，则根据设备的纵横比从可用字体的数字转换纵横中选取最接近的匹配值，该值通过比较两者之间的差异的绝对值得出

**int nEscapement**, // 出口矢量与X轴的角度：以十分之一度为单位指定每一行文本输出时相对于页面底端的角度

**int nOrientation**, // 字符基线与X轴的角度：以十分之一度为单位指定字符基线相对于页面底端的角度

**int nWeight**, // 字体磅值（字体重量）：在Windows中，指字体的粗细程度，其范围为0到1000，正常情况下的字体重量为400，粗体为700，如果为0，则使用默认的字体重量，为方便定义，可使用如下值：
FW_DONTCARE：0；FW_THIN；100；FW_EXTRALIGHT；200
FW_ULTRALIGHT；200；FW_LIGHT；300；FW_NORMAL：400
FW_REGULAR；400； FW_MEDIUM；500； FW_SEMIBOLD；600
FW_DEMIBOLD；600；FW_BOLD：700；FW_EXTRABOLD；800
FW_ULTRABOLD；800；FW_HEAVY；900；FW_BLACK；900

**BYTE bItalic**, // 为TRUE时则为斜体

**BYTE bUnderline**, // 为TRUE时则加下划线

**BYTE cStrikeOut**, //为TRUE时则加删除线

**BYTE nCharSet**, // 指定字体的字符集

**BYTE nOutPrecision**, // 输出精度：定义了输出与所要求的字体高度、宽度、字符方向等的接近程度                                           

**BYTE nClipPrecision**, // 裁剪精度：定义如何裁剪部分超出裁剪区的字符

**BYTE nQuality**, // 输出质量：定义了图形设备接口在匹配逻辑字体属性到实际的物理字体的所使用的方式

**BYTE nPitchAndFamily**, // 调距和字体族，低端二位指定字体的字符间距，字体族描述一种字体的普通外观，当所有的精确字样都不能使用时，可用它们来指定字体

**LPCTSTR lpszFacename )**; // 字体的字型名：指向指定字体的字样名的、以\0结束的字符串指针，字符串的长度不能超过32个字符（包括字符\0），函数EnumFontFamilies可用来列举所有当前可用字体的字样名

**返回值**：如果函数调用成功，返回值是一种逻辑字体句柄；如果函数调用失败，返回值为NULL

接下来的代码很简单。它在内存中从base开始删除96个显示列表。

```
GLvoid KillFont(GLvoid)					// 删除显示列表
{
 	glDeleteLists(base, 96);			//删除96个显示列表
}
```

在GL文字程序里，你可以通过调用glPrint(“需要写的文字”)来调用这段代码。文字被存储在字符串 * fmt中。

```
GLvoid glPrint(const char *fmt, ...)// 自定义GL输出字体函数
{
	char text[256];			// 保存文字串
	va_list	ap;				// 指向一个变量列表的指针
	if (fmt == NULL)		// 如果无输入则返回
		return;			
	va_start(ap, fmt);		// 分析可变参数
	vsprintf(text, fmt, ap);// 把参数值写入字符串
	va_end(ap);				// 结束分析
	glPushAttrib(GL_LIST_BIT);// 把显示列表属性压入属性堆栈
	glListBase(base - 32);	// 设置显示列表的基础值
	glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);// 调用显示列表绘制字符串
	glPopAttrib();			// 弹出属性堆栈
}
```

初始化代码中加入如下一段：

```
BuildFont();
```

下面就是画图的代码了。我们从清除屏幕和深度缓存开始。调用glLoadIdentity()来重置所有东西。然后我们将坐标系向屏幕里移动一个单位。如果不移动的话无法显示出文字。当你使用透视投影而不是ortho投影的时候位图字体表现的更好。由于ortho看起来不好，所以我用透视投影，并移动坐标系。

你会注意到如果把坐标系在屏幕里放的更深远，字体并不会想你想象的那样缩小，只是你可以在控制文字位置时有更多的选择。如果你将坐标系移入屏幕一个单位，你就可以字X轴上-0.5到+0.5的范围内调整文字的位置。如果深入10个单位的话，移动范围就从-5到+5。它给了你更多的选择来替代使用小数指定文字的精确位置。什么都不能改变文字的大小，即使是调用glScale(x,y,z)函数.如果你想改变字体的大小，只能在创建它的时候改变它。

```
int DrawGLScene(GLvoid)						// 此过程中包括所有的绘制代码
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// 清除屏幕及深度缓存
	glLoadIdentity();						// 重置当前的模型观察矩阵
	glTranslatef(0.0f,0.0f,-1.0f);			// 移入屏幕一个单位
	// 根据字体位置设置颜色
	glColor3f(1.0f*float(cos(cnt1)),1.0f*float(sin(cnt2)),1.0f-0.5f*float(cos(cnt1+cnt2)));
	// 设置光栅化位置，即字体的位置
	glRasterPos2f(-0.45f+0.05f*float(cos(cnt1)), 0.35f*float(sin(cnt2)));

	glPrint("This is a test for OpenGL Text - %7.2f", cnt1);// 输出文字到屏幕
	cnt1+=0.051f;						// 增加计数器值
	cnt2+=0.005f;						// 增加计数器值
	return TRUE;						// 继续运行
}
```

最后，如下所示，就是增加在KillGLWindow()函数中增加KillFont()函数，这很重要，它在我们退出程序之前做清理工作。

```
KillFont();							// 删除字体
```

最后编译运行，使用位图文字的程序运行效果如下：

![](/assets/2014-03-11-OpenGLBitWord.png)

本笔记运行代码：[下载地址](/code/DataBlog_OpenGLBitWord.cpp)

----

最后，如果你也对OpenGL感兴趣或者你在学习中有任何想法，欢迎在下方留言、与我交流！祝愿我们共同成长、一起进步！
