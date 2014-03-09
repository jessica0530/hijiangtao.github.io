---
date: 2014-03-09 22:00:00
layout: post
title: OpenGL学习笔记11：使用OpenGL的显示列表
thread: 72
categories: 教程
tags: OpenGL
---

前言：在老师的推荐下准备系统学习下Nehe关于OpenGL的教程，在打开chm的第一眼就被作者的开源奉献精神所深深打动，这种思想与我不谋而合。

以下内容为我学习了教程之后自己的操作与总结，由于硬件与软件环境的差异，我的笔记内容在代码和描述上与原文可能存在一定出路。

----

本笔记记录的内容包括：如何通过OpenGL的显示列表来加速你的程序，这样可以省去你很多重复的代码。

为什么会有这样的需求呢？当你在制作游戏里的小行星场景时，每一层上至少需要两个行星，你可以用OpenGL中的多边形来构造每一个行星。聪明点的做法是做一个循环，每个循环画出行星的一个面，最终你用几十条语句画出了一个行星。每次把行星画到屏幕上都是很困难的。那么，解决的办法是什么呢？用现实列表，你只需要一次性建立物体，你可以贴图，用颜色，想怎么弄就怎么弄。给现实列表一个名字，比如给小行星的显示列表命名为“asteroid”。现在，任何时候我想在屏幕上画出行星，我只需要调用glCallList(asteroid)。之前做好的小行星就会立刻显示在屏幕上了。因为小行星已经在显示列表里建造好了，OpenGL不会再计算如何构造它。它已经在内存中建造好了。这将大大降低CPU的使用，让你的程序跑的更快。

本笔记的代码依旧是基于笔记[OpenGL学习笔记6：将纹理映射到立方体](http://hijiangtao.github.io/2014/03/07/OpenGLTexture/)的代码基础上进行的修改。

----

首先，设置变量存储纹理与显示列表：

```
GLuint	box;						// 保存盒子的显示列表
GLuint	top;						// 保存盒子顶部的显示列表
GLuint	xloop;						// X轴循环变量
GLuint	yloop;						// Y轴循环变量
```

接下来建立两个颜色数组：

```
static GLfloat boxcol[5][3]=				// 盒子的颜色数组
{
	// 亮:红，橙，黄，绿，蓝
	{1.0f,0.0f,0.0f},{1.0f,0.5f,0.0f},{1.0f,1.0f,0.0f},{0.0f,1.0f,0.0f},{0.0f,1.0f,1.0f}
};

static GLfloat topcol[5][3]=				// 顶部的颜色数组
{
	// 暗:红，橙，黄，绿，蓝
	{.5f,0.0f,0.0f},{0.5f,0.25f,0.0f},{0.5f,0.5f,0.0f},{0.0f,0.5f,0.0f},{0.0f,0.5f,0.5f}
};
```

接下来正式开始建立显示列表。其中，所有创造盒子的代码都在第一个显示列表里，所有创造顶部的代码都在另一个列表里。

其中对于glNewList()函数来说，box是第一个参数，这表示OpenGL将把列表存储到box所指向的内存空间。第二个参数GL_COMPILE告诉OpenGL我们想预先在内存中构造这个列表，这样每次画的时候就不必重新计算怎么构造物体了。

GL_COMPILE类似于编程。在你写程序的时候，把它装载到编译器里，你每次运行程序都需要重新编译。而如果他已经编译成了.exe文件，那么每次你只需要点击那个.exe文件就可以运行它了，不需要编译。当OpenGL编译过显示列表后，就不需要再每次显示的时候重新编译它了。这就是为什么用显示列表可以加快速度。

你可以在glNewList()和glEndList()中间加上任何你想加上的代码。可以设置颜色，贴图等等。唯一不能加进去的代码就是会改变显示列表的代码。显示列表一旦建立，你就不能改变它。

在以下的代码中可以看到，`top=box+1;`之后的部分是建立第二个显示列表。

```
GLvoid BuildLists() //创建盒子的显示列表
{
	box=glGenLists(2); //创建两个显示列表的名称
	glNewList(box, GL_COMPILE); //创建第一个显示列表
		glBegin(GL_QUADS); //开始绘制四边形
			// 底面
			glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	
			glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);	
			glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);	
			glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);	
			// 前面
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);	
			glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);	
			glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);	
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);	
			// 后面
			glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	
			glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);	
			glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);	
			glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);	
			// 右面
			glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);	
			glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);	
			glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);	
			glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);	
			// 左面
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	
			glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);	
			glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);	
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
		glEnd();								// 四边形绘制结束
	glEndList();								// 第一个显示列表结束
	top=box+1;									// 第二个显示列表的名称

	glNewList(top, GL_COMPILE); //盒子顶部的显示列表
		glBegin(GL_QUADS);						// 开始绘制四边形
			// 上面
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);	
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);	
			glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);	
			glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);	
		glEnd();								// 结束绘制四边形
		glEndList(); //结束绘制四边形
}
```

贴图纹理的代码和之前教程里的代码是一样的。我们需要一个可以贴在立方体上的纹理。根据纹理的文件名是“Cube.bmp”，所以这里还是需要更改相应的LoadBMP的参数，由于太过简单我这里就省略了，假设你可以轻松完成这部分工作。

在初始化代码中，加入如下一段：

```
BuildLists();						// 创建显示列表
glEnable(GL_LIGHT0);					// 使用默认的0号灯
glEnable(GL_LIGHTING);					// 使用灯光
glEnable(GL_COLOR_MATERIAL);				// 使用颜色材质
```

现在到了绘制代码的地方了。

```
int DrawGLScene(GLvoid)						// 绘制操作开始
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// 清除背景颜色

	glBindTexture(GL_TEXTURE_2D, texture[0]);		// 选择纹理
	for (yloop=1;yloop<6;yloop++)				// 沿Y轴循环
	{
		for (xloop=0;xloop<yloop;xloop++)		// 沿X轴循环
		{
			glLoadIdentity();			// 重置模型变化矩阵
			// 设置盒子的位置
			glTranslatef(1.4f+(float(xloop)*2.8f)-(float(yloop)*1.4f),((6.0f-float(yloop))*2.4f)-7.0f,-20.0f);
			glRotatef(45.0f-(2.0f*yloop)+xrot,1.0f,0.0f,0.0f);	
			glRotatef(45.0f+yrot,0.0f,1.0f,0.0f);
			glColor3fv(boxcol[yloop-1]);
			glCallList(box);			// 绘制盒子

			glColor3fv(topcol[yloop-1]);		// 选择顶部颜色
			glCallList(top);			// 绘制顶部
		}
	}
	return TRUE;						// 成功返回
}
```

下面的代码是键盘控制的一些东西

```
SwapBuffers(hDC);			// 交换缓存 (双缓存)
if (keys[VK_LEFT])				// 左键是否按下
{
	yrot-=0.2f;				// 如果是，向左旋转
}
if (keys[VK_RIGHT])				// 右键是否按下
{
	yrot+=0.2f;				// 如果是向右旋转
}
if (keys[VK_UP])				// 上键是否按下
{
	xrot-=0.2f;				// 如果是向上旋转
}
if (keys[VK_DOWN])				// 下键是否按下
{
	xrot+=0.2f;				// 如果是向下旋转
}
```

最后编译，物块的效果显示如下，通过键盘上下左右键可以对其进行方向改变的操作：

![效果图](/assets/2014-03-09-OpenGLSpeedUp.bmp "效果图")
<center>效果图</center>

本例代码：[下载地址](/code/DataBlog_OpenGLSpeedUp.cpp)

----

最后，如果你也对OpenGL感兴趣或者你在学习中有任何想法，欢迎在下方留言、与我交流！祝愿我们共同成长、一起进步！