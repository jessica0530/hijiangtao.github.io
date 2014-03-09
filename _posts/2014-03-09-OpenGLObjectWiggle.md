---
date: 2014-03-09 11:30:00
layout: post
title: OpenGL学习笔记10：制作飘动的旗帜
thread: 71
categories: 教程
tags: OpenGL
---

前言：在老师的推荐下准备系统学习下Nehe关于OpenGL的教程，在打开chm的第一眼就被作者的开源奉献精神所深深打动，这种思想与我不谋而合。

以下内容为我学习了教程之后自己的操作与总结，由于硬件与软件环境的差异，我的笔记内容在代码和描述上与原文可能存在一定出路。

----

本笔记记录的内容包括：通过掌握纹理映射与混合操作如何创建一个可以飘动的旗帜。

最近看的教程感觉和之前风格就有点不同了。似乎是换了一个叫bosco的大神来讲。这节笔记是基于笔记[OpenGL学习笔记6：将纹理映射到立方体](http://hijiangtao.github.io/2014/03/07/OpenGLTexture/)的代码的基础上进行修改的。除了代码之外，还需要准备一张图片用于作为旗帜的展现，请提前将其命名为Tim.bmp。

----

首先要做的是在代码开头添加

```
#include <math.h>
```

接下来是用数组存储各顶点的x,y,z坐标的定义。

```
float points[ 45 ][ 45 ][3];			// Points网格顶点数组
int wiggle_count = 0;					// 指定旗形波浪的运动速度
GLfloat hold;							// 临时变量
```

然后将LoadGLTextures()中要载入的图片名换为Tim.bmp，接着在InitGL()函数尾部return TRUE之前，填上：

```
glPolygonMode( GL_BACK, GL_FILL );			// 后表面完全填充
glPolygonMode( GL_FRONT, GL_LINE );			// 前表面使用线条绘制
```

接着上面的代码并在return TRUE这一句之前，添加如下的几行。

```
// 沿X平面循环
for(int x=0; x<45; x++)
{
	// 沿Y平面循环
	for(int y=0; y<45; y++)
	{
		// 向表面添加波浪效果
		points[x][y][0]=float((x/5.0f)-4.5f);
		points[x][y][1]=float((y/5.0f)-4.5f);
		points[x][y][2]=float(sin((((x/5.0f)*40.0f)/360.0f)*3.141592654*2.0f));
	}
}
```

上面的两个循环初始化网格上的点。使用整数循环可以消除由于浮点运算取整造成的脉冲锯齿的出现。我们将x和y变量都除以5，再减去4.5。这样使得我们的波浪可以“居中”。

接着重写DrawGLScene函数。首先是多边形绘制与纹理贴靠，说实话这一段代码有些没怎么搞懂，除了一些简单的函数除外。

```
int DrawGLScene(GLvoid)						// 绘制我们的GL场景
{
	int x, y;						// 循环变量
	float float_x, float_y, float_xb, float_yb;	// 用来将旗形的波浪分割成很小的四边形
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// 清除屏幕和深度缓冲
	glLoadIdentity();					// 重置当前的模型观察矩阵

	glTranslatef(0.0f,0.0f,-12.0f);				// 移入屏幕12个单位

	glRotatef(xrot,1.0f,0.0f,0.0f);				// 绕 X 轴旋转
	glRotatef(yrot,0.0f,1.0f,0.0f);				// 绕 Y 轴旋转
	glRotatef(zrot,0.0f,0.0f,1.0f);				// 绕 Z 轴旋转

	glBindTexture(GL_TEXTURE_2D, texture[0]);	// 选择纹理
	glBegin(GL_QUADS);					// 四边形绘制开始
	for( x = 0; x < 44; x++ )				// 沿 X 平面 0-44 循环(45点)
	{
		for( y = 0; y < 44; y++ )			// 沿 Y 平面 0-44 循环(45点)
		{
			float_x = float(x)/44.0f;		// 生成X浮点值
			float_y = float(y)/44.0f;		// 生成Y浮点值
			float_xb = float(x+1)/44.0f;	// X浮点值+0.0227f
			float_yb = float(y+1)/44.0f;	// Y浮点值+0.0227f
			glTexCoord2f( float_x, float_y);// 第一个纹理坐标 (左下角)
			glVertex3f( points[x][y][0], points[x][y][1], points[x][y][2] );

			glTexCoord2f( float_x, float_yb );// 第二个纹理坐标 (左上角)
			glVertex3f( points[x][y+1][0], points[x][y+1][1], points[x][y+1][2] );

			glTexCoord2f( float_xb, float_yb );// 第三个纹理坐标 (右上角)
			glVertex3f( points[x+1][y+1][0], points[x+1][y+1][1], points[x+1][y+1][2] );

			glTexCoord2f( float_xb, float_y );	// 第四个纹理坐标 (右下角)
			glVertex3f( points[x+1][y][0], points[x+1][y][1], points[x+1][y][2] );
		}
	}
	glEnd();						// 四边形绘制结束
```

上面几行使用glTexCoord2f()和glVertex3f()载入数据。提醒一点：四边形是逆时针绘制的。这就是说，您开始所见到的表面是背面。后表面完全填充了，前表面由线条组成。接着是每绘制两次场景，循环一次sine值，以产生运动效果。

说实话，下面这一段的代码中关于波浪移动的步骤我也是看了很久。越来越悔恨自己智商不够用啊，真是的。具体工作是先存储每一行的第一个值，然后将波浪左移一下，是图象产生波浪。存储的数值挪到末端以产生一个永无尽头的波浪纹理效果。然后重置计数器wiggle_count以保持动画的进行。

```
	if( wiggle_count == 2 )					// 用来降低波浪速度(每隔2帧一次)
	{
		for( y = 0; y < 45; y++ )			// 沿Y平面循环
		{
			hold=points[0][y][2];			// 存储当前左侧波浪值
			for( x = 0; x < 44; x++)		// 沿X平面循环
			{
				// 当前波浪值等于其右侧的波浪值
				points[x][y][2] = points[x+1][y][2];
			}
			points[44][y][2]=hold;			// 刚才的值成为最左侧的波浪值
		}
		wiggle_count = 0;				// 计数器清零
	}
	wiggle_count++;						// 计数器加一
	
	xrot+=0.3f;						// X 轴旋转
	yrot+=0.2f;						// Y 轴旋转
	zrot+=0.4f;						// Z 轴旋转

	return TRUE;						// 返回
}
```

最后编译，飘动的效果显示如下：

![效果图](/assets/2014-03-09-OpenGLObjectWiggle.png "效果图")
<center>效果图</center>

本例代码：[下载地址](/code/DataBlog_OpenGLObjectWiggle.cpp)

----

最后，如果你也对OpenGL感兴趣或者你在学习中有任何想法，欢迎在下方留言、与我交流！祝愿我们共同成长、一起进步！