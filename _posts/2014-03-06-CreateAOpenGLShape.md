---
date: 2014-03-06 10:00:00
layout: post
title: OpenGL学习笔记2：创建一个多边形
thread: 60
categories: 教程
tags: OpenGL

---

前言：在老师的推荐下准备系统学习下Nehe关于OpenGL的教程，在打开chm的第一眼就被作者的开源奉献精神所深深打动，这种思想与我不谋而合。

以下内容为我学习了教程之后自己的操作与总结，由于硬件与软件环境的差异，我的笔记内容在代码和描述上与原文可能存在一定出路。

----

本部分笔记记录的是使用OpenGL创建一个多边形并在窗口中显示。代码部分比较9简单，基于前一部分的笔记代码上只需要改写两个函数即可实现。之前的笔迹可以参考[OpenGL学习笔记1：创建一个OpenGL窗口
](http://hijiangtao.github.io/2014/03/06/CreateAOpenGLWindow/)

在DrawGLScene()过程中增加代码如下，其中有关glTranslatef(x, y, z)的使用推荐查看OpenGL坐标转换部分内容：

```
int DrawGLScene(GLvoid)								// 从这里开始进行所有的绘制
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);// 清除屏幕和深度缓存
	glLoadIdentity();								// 重置当前的模型观察矩阵

	glTranslatef(-1.5f,0.0f,-6.0f);					// 左移 1.5 单位，并移入屏幕 6.0

	glBegin(GL_TRIANGLES);							// 绘制三角形
		glVertex3f( 0.0f, 1.0f, 0.0f);				// 上顶点
		glVertex3f(-1.0f,-1.0f, 0.0f);				// 左下
		glVertex3f( 1.0f,-1.0f, 0.0f);				// 右下
	glEnd();										// 三角形绘制结束

	glTranslatef(3.0f,0.0f,0.0f);					// 右移3单位

	glBegin(GL_QUADS);								//  绘制正方形
		glVertex3f(-1.0f, 1.0f, 0.0f);				// 左上
		glVertex3f( 1.0f, 1.0f, 0.0f);				// 右上
		glVertex3f( 1.0f,-1.0f, 0.0f);				// 左下
		glVertex3f(-1.0f,-1.0f, 0.0f);				// 右下
	glEnd();										// 正方形绘制结束
	return TRUE;									// 继续运行
}
```

其次还需要换掉窗口模式下的标题内容，修改如下：

```
if (keys[VK_F1])					// F1键按下了么?
{
	keys[VK_F1]=FALSE;				// 若是，使对应的Key数组中的值为 FALSE
	KillGLWindow();					// 销毁当前的窗口
	fullscreen=!fullscreen;				// 切换 全屏 / 窗口 模式
	// 重建 OpenGL 窗口(修改)
	if (!CreateGLWindow("你的第一个多边形程序",640,480,16,fullscreen))
	{
		return 0;				// 如果窗口未能创建，程序退出
	}
}
```

依旧感谢Nehe众多无私奉献的开发者。