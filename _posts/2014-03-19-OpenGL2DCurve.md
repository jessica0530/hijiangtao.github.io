---
date: 2014-03-19 09:30:00
layout: post
title: OpenGL学习笔记17：二次几何体
thread: 86
categories: 教程
tags: OpenGL
---

前言：在老师的推荐下准备系统学习下Nehe关于OpenGL的教程，在打开chm的第一眼就被作者的开源奉献精神所深深打动，这种思想与我不谋而合。

以下内容为我学习了教程之后自己的操作与总结，由于硬件与软件环境的差异，我的笔记内容在代码和描述上与原文可能存在一定出路。

----

本笔记记录的信息有：利用二次几何体，轻松地创建球，圆盘，圆柱和圆锥。

二次曲面是一种画复合对象的方法，这种方法通常并不需要很多的三角形。此处的代码基于笔记《[OpenGL学习笔记7：光照和键盘控制](http://hijiangtao.github.io/2014/03/07/OpenGLObjectInteract/)》。

首先，我们将要增加7个变量以及修改纹理以增加一些变化 ：

```
bool    sp;		// 空格键是否按下

int	part1;		// 圆盘的起始角度
int	part2;		// 圆盘的结束角度
int	p1=0;		// 增量1
int	p2=1;		// 增量1
GLUquadricObj *quadratic;// 二次几何体
GLuint  object=0;// 二次几何体标示符
```

在InitGL()函数里增加3行代码用来初始化我们的二次曲面，这3行代码将在你使1号光源有效后增加，但是要在返回之前。第一行代码将初始化二次曲面并且创建一个指向改二次曲面的指针，如果改二次曲面不能被创建的话，那么该指针就是NULL。第二行代码将在二次曲面的表面创建平滑的法向量，这样当灯光照上去的时候将会好看些。另外一些可能的取值是：GLU_NONE和GLU_FLAT。最后我们使在二次曲面表面的纹理映射有效：

```
quadratic=gluNewQuadric();				// 创建二次几何体
gluQuadricNormals(quadratic, GLU_SMOOTH);// 使用平滑法线
gluQuadricTexture(quadratic, GL_TRUE);	// 使用纹理
```

本实践过程中将保留立方体，这样你可以看到纹理是如何映射到二次曲面对象上的。以下为绘制立方体的代码定义，它是一个单独的函数：

```
GLvoid glDrawCube()					// 绘制立方体
{
		glBegin(GL_QUADS);			
		// 前面
		glNormal3f( 0.0f, 0.0f, 1.0f);		
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);	
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);	
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);	
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);	
		// 后面
		glNormal3f( 0.0f, 0.0f,-1.0f);		
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);	
		glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);	
		glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);	
		// 上面
		glNormal3f( 0.0f, 1.0f, 0.0f);		
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);	
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);	
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);	
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);	
		// 下面
		glNormal3f( 0.0f,-1.0f, 0.0f);		
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	
		glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);	
		glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);	
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);	
		// 右面
		glNormal3f( 1.0f, 0.0f, 0.0f);		
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);	
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);	
		glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);	
		glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);	
		// 左面
		glNormal3f(-1.0f, 0.0f, 0.0f);		
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);	
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);	
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);	
	glEnd();					
}
```

接下来是修改DrawGLScene()函数，我们在里面增加了6种不同的创建对象方式，将以下内容加入：

```
	switch(object)						// 绘制哪一种二次几何体
	{
	case 0:							// 绘制立方体
		glDrawCube();					
		break;	

	case 1:							// 绘制圆柱体
		glTranslatef(0.0f,0.0f,-1.5f);			
		gluCylinder(quadratic,1.0f,1.0f,3.0f,32,32);	
		break;	

	case 2:							// 绘制圆盘
		gluDisk(quadratic,0.5f,1.5f,32,32);		
		break;	

	case 3:							// 绘制球
		gluSphere(quadratic,1.3f,32,32);		
		break;

	case 4:							// 绘制圆锥
		glTranslatef(0.0f,0.0f,-1.5f);			
		gluCylinder(quadratic,1.0f,0.0f,3.0f,32,32);	
		break;	

	case 5:							// 绘制部分圆盘
		part1+=p1;					
		part2+=p2;					

		if(part1>359)					
		{
			p1=0;					
			part1=0;				
			p2=1;					
			part2=0;				
		}
		if(part2>359)					
		{
			p1=1;					
			p2=0;					
		}
		gluPartialDisk(quadratic,0.5f,1.5f,32,32,part1,part2-part1);	
		break;						
	};
```

然后在KillGLWindow(GLvoid)中加入：

```
gluDeleteQuadric(quadratic);// 删除二次几何体
```

最后，加入键盘输入代码就大功告成了。

```
				if (keys[' '] && !sp)// 空格是否按下
				{
					sp=TRUE;		// 是,则绘制下一种二次几何体
					object++;		
					if(object>5)		
						object=0;	
				}
				if (!keys[' '])		// 空格是否释放
				{
					sp=FALSE;		// 记录这个状态
				}
```

----

点击空格键进行几何体的绘制变换，程序运行效果如下：

![效果图](/assets/2014-03-19-OpenGL2DCurve1.png "效果图1")

----

![效果图](/assets/2014-03-19-OpenGL2DCurve2.png "效果图2")

----

![效果图](/assets/2014-03-19-OpenGL2DCurve3.png "效果图3")

本笔记运行代码：[下载地址](/code/DataBlog_OpenGL2DCurve.cpp)

----

最后，如果你也对OpenGL感兴趣或者你在学习中有任何想法，欢迎在下方留言、与我交流！祝愿我们共同成长、一起进步！