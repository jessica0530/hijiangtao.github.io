---
date: 2014-03-12 19:30:00
layout: post
title: OpenGL学习笔记15：实现烟雾效果
thread: 79
categories: 教程
tags: OpenGL
---

前言：在老师的推荐下准备系统学习下Nehe关于OpenGL的教程，在打开chm的第一眼就被作者的开源奉献精神所深深打动，这种思想与我不谋而合。

以下内容为我学习了教程之后自己的操作与总结，由于硬件与软件环境的差异，我的笔记内容在代码和描述上与原文可能存在一定出路。

----

本笔记记录的信息有：显示雾的效果。此处的代码基于笔记《[OpenGL学习笔记7：光照和键盘控制](http://hijiangtao.github.io/2014/03/07/OpenGLObjectInteract/)》。本片教程原作者是Chris Aliotta。

首先从设置保存雾的信息的变量开始。变量fogMode用来保存三种类型的雾:GL_EXP,GL_EXP2和GL_LINEAR。我将在稍后解释这三种类型的区别。这些变量位于程序开头GLuint texture[3]这行后。变量fogfilter将用来表示我们使用的是哪种雾类型。变量fogColor保存雾的颜色。在程序的顶部我还加了一个布尔类型的变量gp用来记录'g'键是否被按下。

```
bool   gp;						// G健是否按下
GLuint filter;						// 使用哪一个纹理过滤器
GLuint fogMode[]= { GL_EXP, GL_EXP2, GL_LINEAR };		// 雾气的模式
GLuint fogfilter= 0;					// 使用哪一种雾气
GLfloat fogColor[4]= {0.5f, 0.5f, 0.5f, 1.0f};		// 雾的颜色设为白色
```

接下来看InitGL函数。为了获得更好的效果，glClearColor()这行已经被修改为将屏幕清为同雾相同的颜色。使用雾效只需很少的代码。完整的函数代码如下所示：

```
int InitGL(GLvoid)								            // 此处开始对OpenGL进行所有设置
{
	if (!LoadGLTextures())							// 调用纹理载入子例程
	{
		return FALSE;							// 如果未能载入，返回FALSE
	}

	glEnable(GL_TEXTURE_2D);						// 启用纹理映射
	glShadeModel(GL_SMOOTH);								// 启用阴影平滑
	glClearColor(0.5f,0.5f,0.5f,1.0f);			// 设置背景的颜色为雾气的颜色
	glClearDepth(1.0f);                                     // 设置深度缓存

	glFogi(GL_FOG_MODE, fogMode[fogfilter]);		// 设置雾气的模式
	glFogfv(GL_FOG_COLOR, fogColor);			// 设置雾的颜色
	glFogf(GL_FOG_DENSITY, 0.35f);			// 设置雾的密度
	glHint(GL_FOG_HINT, GL_DONT_CARE);			// 设置系统如何计算雾气
	glFogf(GL_FOG_START, 1.0f);				// 雾气的开始位置
	glFogf(GL_FOG_END, 5.0f);				// 雾气的结束位置
	glEnable(GL_FOG);					// 使用雾气

	glEnable(GL_DEPTH_TEST);                                // 启用深度测试
    glDepthFunc(GL_LEQUAL);                                 // 所作深度测试的类型
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);      // 告诉系统对透视进行修正

    glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);             // 设置环境光
    glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);             // 设置漫射光
    glLightfv(GL_LIGHT1, GL_POSITION,LightPosition);            // 设置光源位置
    glEnable(GL_LIGHT1);                            // 启用一号光源

	return TRUE;									//初始化OK
}
```

在上面中，需要注意的是：`glEnable(GL_FOG);`主要是初始化雾效。`glFogi(GL_FOG_MODE, fogMode[fogfilter]);`建立雾的过滤模式。之前我们声明了数组fogMode，它保存了值GL_EXP, GL_EXP2, and GL_LINEAR。现在是使用他们的时候了。具体原理可以参考红皮书，这是计算雾效混合因子的三种方式：

1. GL_EXP - 充满整个屏幕的基本渲染的雾。它能在较老的PC上工作，因此并不是特别像雾。 
2. GL_EXP2 - 比GL_EXP更进一步。它也是充满整个屏幕，但它使屏幕看起来更有深度。 
3. GL_LINEAR - 最好的渲染模式。物体淡入淡出的效果更自然。

`glFogfv(GL_FOG_COLOR, fogcolor);`是设置雾的颜色。之前我们已将变量fogcolor设为(0.5f,0.5f,0.5f,1.0f)，这是一个很棒的灰色。

接下来看后面四行。`glFogf(GL_FOG_DENSITY, 0.35f);`这行设置雾的密度。增加数字会让雾更密，减少它则雾更稀。`glHint (GL_FOG_HINT, GL_DONT_CARE);` 设置修正。我使用了GL_DONT_CARE。

gl_dont_care：由OpenGL决定使用何种雾效（对每个顶点还是每个像素执行雾效计算）和一个未知的公式？文章中写到未知公式，但我不太清楚这里是什么意思。

gl_nicest：对每个像素执行雾效计算（效果好）。

gl_fastest：对每个顶点执行雾效计算 （更快，但效果不如上面的好）。

下一行`glFogf(GL_FOG_START, 1.0f);`设定雾效距屏幕多近开始。你可以根据你的需要随意改变这个值。下一行类似，`glFogf(GL_FOG_END, 5.0f);`告诉OpenGL程序雾效持续到距屏幕多远。

现在我们建立了绘制雾的代码，我们将加入键盘指令在不同的雾效模式间循环。这段代码及其它的按键处理代码在程序的最后。

```
if (keys['G'] && !gp)					// G键是否 按下
{
	gp=TRUE;						// 是
	fogfilter+=1;					// 变换雾气模式
	if (fogfilter>2)					// 模式是否大于2
	{
		fogfilter=0;				// 置零
	}
	glFogi (GL_FOG_MODE, fogMode[fogfilter]);		// 设置雾气模式
}
if (!keys['G'])						// G键是否释放
{
	gp=FALSE;						// 是，设置为释放
}
```

当然窗口标题啊什么的小细节也别忘了修改。

编译运行效果如下所示：

![](/assets/2014-03-12-OpenGLFogEffect.png)

本笔记运行代码：[下载地址](/code/DataBlog_OpenGLFogEffect.cpp)

----

最后，如果你也对OpenGL感兴趣或者你在学习中有任何想法，欢迎在下方留言、与我交流！祝愿我们共同成长、一起进步！