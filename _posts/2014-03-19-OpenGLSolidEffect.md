---
date: 2014-03-19 14:30:00
layout: post
title: OpenGL学习笔记18：粒子系统
thread: 87
categories: 教程
tags: OpenGL
---

前言：在老师的推荐下准备系统学习下Nehe关于OpenGL的教程，在打开chm的第一眼就被作者的开源奉献精神所深深打动，这种思想与我不谋而合。

以下内容为我学习了教程之后自己的操作与总结，由于硬件与软件环境的差异，我的笔记内容在代码和描述上与原文可能存在一定出路。

----

本笔记记录的信息有：如何创建一个简单的粒子系统，并用它来创建一种喷射的效果。此处的代码基于笔记《[OpenGL学习笔记6：将纹理映射到立方体](http://hijiangtao.github.io/2014/03/07/OpenGLTexture/)》。

首先，我们将要增加5行新的代码在我们程序的前面。第一行"stdio.h"允许我们读文件中的数据，它和我们以前用在纹理映射当中是一样的；第二行定义了一些我们要在屏幕上显示的粒子的数目，告诉程序MAX_PARTICLES在这里的数值为1000。第三条行将不断分离的彩色的粒子栓牢在一起，并设置为默认情况。sp和rp用来确定空格键和返回键是否有按住。

```
#include <stdio.h>
#define	MAX_PARTICLES	1000// 定义最大的粒子数
bool	rainbow=true;	// 是否为彩虹模式
bool	sp;				// 空格键是否被按下
bool	rp;				// 回车键是否被按下
```

下面四行是复杂的变量。变量slowdown控制粒子移动的快慢。数值愈高，移动越慢；数值越底，移动越快。如果数值降低，粒子将快速的移动。粒子的速度影响它们在荧屏中移动的距离，记住速度慢的粒子不会射很远的。变量xspeed和yspeed控制尾部的方向，xspeed将会增加粒子在x轴上速度。

如果xspeed是正值粒子将会向右边移动多，如果xspeed负价值粒子将会向左边移动多。值越高，就向该方向移动比较多。yspeed工作相同的方法，但是在y轴上。

```
float	slowdown=2.0f;		// 减速粒子
float	xspeed;				// X方向的速度
float	yspeed;				// Y方向的速度
float	zoom=-40.0f;		// 沿Z轴缩放
```

我们定义了一个复杂的循环变量叫做loop，我们用这变量预先定义粒子并在屏幕中画粒子。col用来给予粒子不同的颜色。delay用来控制在彩虹模式中圆的颜色变化。最后，我们设定一个存储空间(粒子纹理)。

```
GLuint	loop;				// 循环变量
GLuint	col;				// 当前的颜色
GLuint	delay;				// 彩虹效果延迟
```

下段程序描述单一粒子结构，这是我们给予粒子的属性。active为true,我们的粒子为活跃的，如果为false则粒子为死的，此时我们就删除它。在程序中我没有使用活跃的，因为它很好的实现。变量life和fade用来控制粒子显示多久以及显示时候的亮度。

而变量r,g和b用来表示粒子的红、绿和蓝色的强度；变量x,y和z控制粒子在屏幕上显示的位置；xi,yi,zi三个变量控制粒子在每个轴上移动的快慢和方向；最后三个变量每一个变量可被看成加速度，只不过方向不同罢了。结构的名字为particles，声明并创建1000个粒子，存储空间为每个粒子提供相应的信息。

```
typedef struct					// 创建粒子数据结构
{
	bool	active;				// 是否激活
	float	life;				// 粒子生命
	float	fade;				// 衰减速度

	float	r;					// 红色值
	float	g;					// 绿色值
	float	b;					// 蓝色值

	float	x;					// X 位置
	float	y;					// Y 位置
	float	z;					// Z 位置

	float	xi;					// X 方向
	float	yi;					// Y 方向
	float	zi;					// Z 方向

	float	xg;					// X 方向重力加速度
	float	yg;					// Y 方向重力加速度
	float	zg;					// Z 方向重力加速度
}particles;						// 粒子数据结构
particles particle[MAX_PARTICLES];				// 保存1000个粒子的数组
```

在颜色数组上我们减少一些代码来存储12中不同的颜色.对每一个颜色从0到11我们存储亮红,亮绿,和亮蓝.下面的颜色表里包含12个渐变颜色从红色到紫罗兰色：

```
static GLfloat colors[12][3]=				// 彩虹颜色
{
	{1.0f,0.5f,0.5f},{1.0f,0.75f,0.5f},{1.0f,1.0f,0.5f},{0.75f,1.0f,0.5f},
	{0.5f,1.0f,0.5f},{0.5f,1.0f,0.75f},{0.5f,1.0f,1.0f},{0.5f,0.75f,1.0f},
	{0.5f,0.5f,1.0f},{0.75f,0.5f,1.0f},{1.0f,0.5f,1.0f},{1.0f,0.5f,0.75f}
};
```

对了记得改变一下调入位图的名称，我这里新用的图片名字是：Particle.bmp。接下来，我们修改InitGL()。

我们使用光滑的阴影，清除背景为黑色，关闭深度测试，绑定并映射纹理。启用映射位图后我们选择粒子纹理，并初始化每个粒子。

```
	glDisable(GL_DEPTH_TEST);//禁止深度测试

	for (loop=0;loop<MAX_PARTICLES;loop++)//初始化所有的粒子
	{
		particle[loop].active=true;	// 使所有的粒子为激活状态
		particle[loop].life=1.0f;// 所有的粒子生命值为最大
		particle[loop].fade=float(rand()%100)/1000.0f+0.003f;// 随机生成衰减速率

		particle[loop].r=colors[loop*(12/MAX_PARTICLES)][0];// 粒子的红色颜色
		particle[loop].g=colors[loop*(12/MAX_PARTICLES)][1];// 粒子的绿色颜色
		particle[loop].b=colors[loop*(12/MAX_PARTICLES)][2];// 粒子的蓝色颜色

		particle[loop].xi=float((rand()%50)-26.0f)*10.0f;// 随机生成X轴方向速度
		particle[loop].yi=float((rand()%50)-25.0f)*10.0f;// 随机生成Y轴方向速度
		particle[loop].zi=float((rand()%50)-25.0f)*10.0f;// 随机生成Z轴方向速度

		particle[loop].xg=0.0f;	// 设置X轴方向加速度为0
		particle[loop].yg=-0.8f;//  设置Y轴方向加速度为-0.8
		particle[loop].zg=0.0f;	//  设置Z轴方向加速度为0
	}
```

接下来，我们产生粒子，并检查加速度。我们重置Modelview矩阵，用glVertex3f()绘制粒子位置，这样画的粒子就不会改变modelview矩阵。这一段代码增加了大段注释，故不详细描述，但重要性一点不减。

```
int DrawGLScene(GLvoid)	// 绘制粒子
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);// 以黑色背景清楚
	glLoadIdentity();// 重置模型变换矩阵

	for (loop=0;loop<MAX_PARTICLES;loop++)// 循环所有的粒子
	{
		if (particle[loop].active)			// 如果粒子为激活的
		{
			float x=particle[loop].x;		// 返回X轴的位置
			float y=particle[loop].y;		// 返回Y轴的位置
			float z=particle[loop].z+zoom;	// 返回Z轴的位置
			// 设置粒子颜色
			glColor4f(particle[loop].r,particle[loop].g,particle[loop].b,particle[loop].life);
			glBegin(GL_TRIANGLE_STRIP);	// 绘制三角形带
				glTexCoord2d(1,1); glVertex3f(x+0.5f,y+0.5f,z); 
				glTexCoord2d(0,1); glVertex3f(x-0.5f,y+0.5f,z); 
				glTexCoord2d(1,0); glVertex3f(x+0.5f,y-0.5f,z); 
				glTexCoord2d(0,0); glVertex3f(x-0.5f,y-0.5f,z); 
			glEnd();

			particle[loop].x+=particle[loop].xi/(slowdown*1000);	// 更新X坐标的位置
			particle[loop].y+=particle[loop].yi/(slowdown*1000);	// 更新Y坐标的位置
			particle[loop].z+=particle[loop].zi/(slowdown*1000);	// 更新Z坐标的位置

			particle[loop].xi+=particle[loop].xg;	// 更新X轴方向速度大小
			particle[loop].yi+=particle[loop].yg;	// 更新Y轴方向速度大小
			particle[loop].zi+=particle[loop].zg;	// 更新Z轴方向速度大小

			particle[loop].life-=particle[loop].fade;// 减少粒子的生命值

			if (particle[loop].life<0.0f)	// 如果粒子生命值小于0
			{
				particle[loop].life=1.0f;	// 产生一个新的粒子
				particle[loop].fade=float(rand()%100)/1000.0f+0.003f;	// 随机生成衰减速率

				particle[loop].x=0.0f;	// 新粒子出现在屏幕的中央
				particle[loop].y=0.0f;					
				particle[loop].z=0.0f;					

				particle[loop].xi=xspeed+float((rand()%60)-32.0f);// 随机生成粒子速度
				particle[loop].yi=yspeed+float((rand()%60)-30.0f);	
				particle[loop].zi=float((rand()%60)-30.0f);		

				particle[loop].r=colors[col][0];// 设置粒子颜色
				particle[loop].g=colors[col][1];			
				particle[loop].b=colors[col][2];			
			}

			// 如果小键盘8被按住，增加Y轴方向的加速度
			if (keys[VK_NUMPAD8] && (particle[loop].yg<1.5f)) particle[loop].yg+=0.01f;

			// 如果小键盘2被按住，减少Y轴方向的加速度
			if (keys[VK_NUMPAD2] && (particle[loop].yg>-1.5f)) particle[loop].yg-=0.01f;

			// 如果小键盘6被按住，增加X轴方向的加速度
			if (keys[VK_NUMPAD6] && (particle[loop].xg<1.5f)) particle[loop].xg+=0.01f;

			// 如果小键盘4被按住，减少X轴方向的加速度
			if (keys[VK_NUMPAD4] && (particle[loop].xg>-1.5f)) particle[loop].xg-=0.01f;

			if (keys[VK_TAB])// 按Tab键，使粒子回到原点
			{
				particle[loop].x=0.0f;					
				particle[loop].y=0.0f;					
				particle[loop].z=0.0f;					
				particle[loop].xi=float((rand()%50)-26.0f)*10.0f;	// 随机生成速度
				particle[loop].yi=float((rand()%50)-25.0f)*10.0f;	
				particle[loop].zi=float((rand()%50)-25.0f)*10.0f;	
			}
		}
    }
	return TRUE;		// 绘制完毕成功返回
}
```

接下来跳到WinMain()增加代码。在keys[VK_F1]判断之后添加如下代码，其中检查"+"是否被按下，如果它和slowdown一起实现则slowdown减少0.01f，粒子就可以较快速地移动；检查"-"是否被按下，如果它和slowdown一起实现则slowdown增加0.01f，粒子就可以较慢速地移动；包括检测Page Up和Page Down是否被按下，从而影响粒子靠近或是离开我们。

其中有一段代码检验enter键是否被按下。如果是，并且没有被一直按着，我们将让计算机把rp变为true，然后我们固定彩虹模式。如果彩虹模式为true，将其变成false。如果为false，将其变成true。最后一行检测enter是否被释放，如果释放rp为false并告诉计算机该键不被按下；而space键用于变换颜色。

```
if (keys[VK_ADD] && (slowdown>1.0f)) slowdown-=0.01f;		// 按+号，加速粒子
if (keys[VK_SUBTRACT] && (slowdown<4.0f)) slowdown+=0.01f;	// 按-号，减速粒子
if (keys[VK_PRIOR]) zoom+=0.1f;		// 按Page Up键，让粒子靠近视点
if (keys[VK_NEXT]) zoom-=0.1f;		// 按Page Down，让粒子远离视点

if (keys[VK_RETURN] && !rp)		// 按住回车键，切换彩虹模式
{
	rp=true;
	rainbow=!rainbow;
}
if (!keys[VK_RETURN]) rp=false;

if ((keys[' '] && !sp) || (rainbow && (delay>25)))	// 空格键，变换颜色
{
	if (keys[' ']) rainbow=false;	
	sp=true;			
	delay=0;			
	col++;				
	if (col>11) col=0;
}
if (!keys[' '])	sp=false;		// 如果释放空格键，记录这个状态
// 按上增加粒子Y轴正方向的速度
if (keys[VK_UP] && (yspeed<200)) yspeed+=1.0f;

// 按下减少粒子Y轴正方向的速度
if (keys[VK_DOWN] && (yspeed>-200)) yspeed-=1.0f;

// 按右增加粒子X轴正方向的速度
if (keys[VK_RIGHT] && (xspeed<200)) xspeed+=1.0f;

// 按左减少粒子X轴正方向的速度
if (keys[VK_LEFT] && (xspeed>-200)) xspeed-=1.0f;

delay++;			// 增加彩虹模式的颜色切换延迟
```

----

利用各种按键进行简单粒子效果的切换，程序运行效果如下：

![效果图](/assets/2014-03-19-OpenGLSolidEffect.png "效果图")

本笔记运行代码：[下载地址](/code/DataBlog_OpenGLSolidEffect.cpp)

----

最后，如果你也对OpenGL感兴趣或者你在学习中有任何想法，欢迎在下方留言、与我交流！祝愿我们共同成长、一起进步！