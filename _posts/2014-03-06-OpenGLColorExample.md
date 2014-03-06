---
date: 2014-03-06 15:00:00
layout: post
title: OpenGL学习笔记3：如何使用颜色
thread: 61
categories: 教程
tags: OpenGL

---

前言：在老师的推荐下准备系统学习下Nehe关于OpenGL的教程，在打开chm的第一眼就被作者的开源奉献精神所深深打动，这种思想与我不谋而合。

以下内容为我学习了教程之后自己的操作与总结，由于硬件与软件环境的差异，我的笔记内容在代码和描述上与原文可能存在一定出路。

----

本部分笔记记录的是告诉大家如何使用OpenGL的着色方法。作为前一节课的扩展，这里会记录两种着色方法：光滑着色和平面着色。之前的笔迹可以参考[OpenGL学习笔记1：创建一个OpenGL窗口
](http://hijiangtao.github.io/2014/03/06/CreateAOpenGLWindow/)以及[OpenGL学习笔记2：创建一个多边形
](http://hijiangtao.github.io/2014/03/06/CreateAOpenGLShape/)

这一部分的修改内容与上一节笔记相似，其实就是重写了DrawGLScene(GLvoid)这个函数，其中添加的是用函数也只有一个glColor3f()，这个函数的作用是为画笔设置颜色。通过不同方式的使用达到平滑着色或者单调着色的目的。

绘制平滑着色的三角形：让三角形的三个定点分别为红色、绿色、蓝色，然后向中间扩散时三个不同的颜色融合在一起，代码如下：

```
	glBegin(GL_TRIANGLES);					    // 绘制三角形
	    glColor3f(1.0f,0.0f,0.0f);				// 设置当前色为红色
	    glVertex3f( 0.0f, 1.0f, 0.0f);			// 上顶点

	    glColor3f(0.0f,1.0f,0.0f);				// 设置当前色为绿色
	    glVertex3f(-1.0f,-1.0f, 0.0f);			// 左下

	    glColor3f(0.0f,0.0f,1.0f);				// 设置当前色为蓝色
	    glVertex3f( 1.0f,-1.0f, 0.0f);			// 右下
	glEnd();								    // 三角形绘制结束
```

单调着色四边形，代码如下：

```
	glColor3f(0.5f,0.5f,1.0f);						// 一次性将当前色设置为蓝色
	glBegin(GL_QUADS);								//  绘制正方形
		glVertex3f(-1.0f, 1.0f, 0.0f);				// 左上
		glVertex3f( 1.0f, 1.0f, 0.0f);				// 右上
		glVertex3f( 1.0f,-1.0f, 0.0f);				// 左下
		glVertex3f(-1.0f,-1.0f, 0.0f);				// 右下
	glEnd();										// 正方形绘制结束
```

最后，换掉窗口模式下的标题内容：

```
if (keys[VK_F1])					// F1键按下了么?
{
	keys[VK_F1]=FALSE;				// 若是，使对应的Key数组中的值为 FALSE
	KillGLWindow();					// 销毁当前的窗口
	fullscreen=!fullscreen;				// 切换 全屏 / 窗口 模式
	// 重建 OpenGL 窗口(修改)
	if (!CreateGLWindow("OpenGL颜色实例",640,480,16,fullscreen))
	{
		return 0;				// 如果窗口未能创建，程序退出
	}
}
```

本章完整代码下载见：[如何使用颜色CODE下载](/code/DataBlog_OpenGLColorExample.cpp)

最后效果呈现如下所示：

![OpenGLColorExample](/assets/2014-03-06-OpenGLColorExample.png "OpenGLColorExample")
<center>OpenGL ColorExample</center>

----

如果你也对OpenGL感兴趣或者你在学习中有任何想法，欢迎在下方留言、与我交流！祝愿我们共同成长、一起进步！