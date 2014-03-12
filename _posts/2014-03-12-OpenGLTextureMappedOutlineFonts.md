---
date: 2014-03-12 17:30:00
layout: post
title: OpenGL学习笔记14：图形字体的纹理映射
thread: 78
categories: 教程
tags: OpenGL
---

前言：在老师的推荐下准备系统学习下Nehe关于OpenGL的教程，在打开chm的第一眼就被作者的开源奉献精神所深深打动，这种思想与我不谋而合。

以下内容为我学习了教程之后自己的操作与总结，由于硬件与软件环境的差异，我的笔记内容在代码和描述上与原文可能存在一定出路。

----

本笔记记录的信息有：如何绘制一个像3D模型一样会旋转的3D文字。

此处的代码基于上一篇笔记《[OpenGL学习笔记13：创建3D图形字体](http://hijiangtao.github.io/2014/03/12/OpenGL3DText/)》的代码，这样来创作纹理字体的演示会省力不少。

首先要添加一个叫做texture[]的整型变量。它用于保存纹理。

```
GLuint	texture[1];	// One Texture Map
```

下面的部分做了一些小改动，这是由于作者在教程中说想使用wingdings字体来显示一个海盗旗（骷髅头和十字骨头）的标志。如果你想显示文字的话，就不用改动第14课中的代码了，也可以选择另一种字体。
 
wingdings是一种符号字体，使用它时需要做一些改动。告诉Windows使用wingdings字体并不太简单。如果你把字体的名字改为wingdings，你会注意到字体其实并没有选到。你必须告诉Windows这种字体是一种符号字体而不是一种标准字符字体。

以下函数中除了CreateFont有较大改动，其他的几乎和上篇中的代码相似。其中比较有魔力的一行就是设置字符集。不使用第14课中的ANSI_CHARSET，我们将使用SYMBOL_CHARSET。这会告诉Windows我们创建的字体并不是由标准字符组成的典型字体。所谓符号字体通常是由一些小图片（符号）组成的。如果你忘了改变这行，wingdings,webdings以及你想用的其它符号字体就不会工作。

```
GLvoid BuildFont(GLvoid)				// 创建位图字体
{
	HFONT	font;						// 字体句柄
	base = glGenLists(256);						// 创建256个显示列表	
	font = CreateFont(	-12,							// Height Of Font
						0,								// Width Of Font
						0,								// Angle Of Escapement
						0,								// Orientation Angle
						FW_BOLD,						// Font Weight
						FALSE,							// Italic
						FALSE,							// Underline
						FALSE,							// Strikeout
						SYMBOL_CHARSET,			// 设置字符集
						OUT_TT_PRECIS,					// Output Precision
						CLIP_DEFAULT_PRECIS,			// Clipping Precision
						ANTIALIASED_QUALITY,			// Output Quality
						FF_DONTCARE|DEFAULT_PITCH,		// Family And Pitch
						"Wingdings");				// Font Name
	SelectObject(hDC, font);
	wglUseFontOutlines(	hDC,					// 设置当前窗口设备描述表的句柄
				0,				// 用于创建显示列表字体的第一个字符的ASCII值
				255,				// 字符数
				base,				// 第一个显示列表的名称
				0.1f,				// 字体的光滑度，越小越光滑，0.0为最光滑的状态
				0.2f,				// 在z方向突出的距离
				WGL_FONT_POLYGONS,			// 使用多边形来生成字符，每个顶点具有独立的法线
				gmf);				//一个接收字形度量数据的数组的地址，每个数组元素用它对应的显示列表字符的数据填充 
}
```

我们允许有更多的误差，这意味着GL不会严格的遵守字体的轮廓线。如果你把误差设置为0.0f，你就会发现严格地在曲面上贴图存在一些问题。但是如果你允许一定的误差，很多问题都可以避免。这就是上面在字体光滑度上为什么把数值设为0.1而不是0.0的原因了。

在ReSizeGLScene()函数之前，我们要加上下面一段代码来读取纹理。我们创建一个保存位图的地方，读取位图，告诉Windows生成一个纹理，并把它保存在texture[0]中。所以先将这些曾经的代码加上。

```
AUX_RGBImageRec *LoadBMP(char *Filename)					// 载入位图图象
{
	FILE *File=NULL;							// 文件句柄
	if (!Filename)								// 确保文件名已提供
	{
		return NULL;							// 如果没提供，返回 NULL
	}
	File=fopen(Filename,"r");						// 尝试打开文件

	if (File)								// 文件存在么?
	{
		fclose(File);							// 关闭句柄
		return auxDIBImageLoad(Filename);				// 载入位图并返回指针
	}

	return NULL;								// 如果载入失败，返回 NULL
}
```

下面来设置LoadGLTextures()函数。其中比较重要的四行是设置纹理映射模式的四行。它为我们绘制在屏幕上的任何物体自动生成纹理坐标。函数glTexGeni非常强大，而且复杂，这里就不详细介绍了。不过，你只要知道GL_S和GL_T是纹理坐标就可以了。默认状态下，它被设置为提取物体此刻在屏幕上的x坐标和y坐标，并把它们转换为顶点坐标。你会发现到物体在z平面没有纹理，只显示一些斑纹。正面和反面都被赋予了纹理，这些都是由glTexGeni函数产生的。X(GL_S)用于从左到右映射纹理，Y(GL_T)用于从上到下映射纹理。 

GL_TEXTURE_GEN_MODE允许我们选择我们想在S和T纹理坐标上使用的纹理映射模式。你有3种选择：

1. GL_EYE_LINEAR - 纹理会固定在屏幕上。它永远不会移动。物体将被赋予处于它通过的地区的那一块纹理。
2. GL_OBJECT_LINEAR - 这种就是我们使用的模式。纹理被固定于在屏幕上运动的物体上。
3. GL_SPHERE_MAP - 每个人都喜欢。创建一种有金属质感的物体。

完整代码如下：

```
int LoadGLTextures()		// Load Bitmaps And Convert To Textures
{
	int Status=FALSE;				// Status Indicator

	AUX_RGBImageRec *TextureImage[1];	// Create Storage Space For The Texture

	memset(TextureImage,0,sizeof(void *)*1);   // Set The Pointer To NULL

	// Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit
	if (TextureImage[0]=LoadBMP("Lights.bmp"))   // 载入位图
	{
		Status=TRUE;		// Set The Status To TRUE

		glGenTextures(1, &texture[0]);	// Create The Texture

		glBindTexture(GL_TEXTURE_2D, texture[0]);
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
		// 设置纹理映射模式
		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
		glEnable(GL_TEXTURE_GEN_S);		// 使用自动生成纹理
		glEnable(GL_TEXTURE_GEN_T);	
	}

	if (TextureImage[0])					// If Texture Exists
	{
		if (TextureImage[0]->data)	// If Texture Image Exists
		{
			free(TextureImage[0]->data);		// Free The Texture Image Memory
		}

		free(TextureImage[0]);		// Free The Image Structure
	}

	return Status;			// Return The Status
}
```

在InitGL()的最后有几行新代码。BuildFont()被放到了读取纹理的代码之后。glEnable(GL_COLOR_MATERIAL) 这行被删掉了，如果你想使用glColor3f(r,g,b)来改变纹理的颜色，那么就把glEnable(GL_COLOR_MATERIAL)这行重新加到这部分代码中。

最后三行的作用是启动2D纹理映射，并选择第一个纹理。这样就把第一个纹理映射到我们绘制在屏幕上的3D物体上了。如果你想加入更多的操作，可以按自己的意愿启动或禁用纹理映射。

```
int InitGL(GLvoid)							// 此处开始对OpenGL进行所有设置
{
	if (!LoadGLTextures())					// 载入纹理
	{
		return FALSE;					// 失败则返回
	}
	BuildFont();						// 创建字体显示列表

	glShadeModel(GL_SMOOTH);					// 启用阴影平滑
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);			// 黑色背景
	glClearDepth(1.0f);					// 设置深度缓存
	glEnable(GL_DEPTH_TEST);					// 启用深度测试
	glDepthFunc(GL_LEQUAL);					// 所作深度测试的类型
	glEnable(GL_LIGHT0);					// 使用第0号灯
	glEnable(GL_LIGHTING);					// 使用光照
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);		// 告诉系统对透视进行修正
	glEnable(GL_TEXTURE_2D);					// 使用二维纹理
	glBindTexture(GL_TEXTURE_2D, texture[0]);			// 选择使用的纹理
	return TRUE;						// 初始化成功
}
```

接下来修改DrawGLScene这部分代码。

比较重要的几处修改有如下几点：

* 我们打算使用COS和SIN让物体绕着屏幕旋转而不是把它固定在屏幕中间。我们将把物体向屏幕里移动3个单位。在x轴，我们将移动范围限制在-1.1到+1.1之间。我们使用rot变量来控制左右移动。我们把上下移动的范围限制在+0.8到-0.8之间。同样使用rot变量来控制上下移动（最好充分利用你的变量）。

```
// 设置字体的位置
glTranslatef(1.1f*float(cos(rot/16.0f)),0.8f*float(sin(rot/20.0f)),-3.0f);
```

* 下面做常规的旋转。这会使符号在X，Y和Z轴旋转。

```
glRotatef(rot,1.0f,0.0f,0.0f);				// 沿X轴旋转
glRotatef(rot*1.2f,0.0f,1.0f,0.0f);				// 沿Y轴旋转
glRotatef(rot*1.4f,0.0f,0.0f,1.0f);				// 沿Z轴旋转
```

* 最后，我们绘制海盗旗的符号，然后增加rot变量，从而使这个符号在屏幕中旋转和移动。

```
glPrint("N");						// 绘制海盗旗符号
rot+=0.1f;						// 增加旋转变量
```

此部分完整函数代码如下：

```
int DrawGLScene(GLvoid)					// 此过程中包括所有的绘制代码
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// 清除屏幕及深度缓存
	glLoadIdentity();					// 重置当前的模型观察矩阵
	// 设置字体的位置
	glTranslatef(1.1f*float(cos(rot/16.0f)),0.8f*float(sin(rot/20.0f)),-3.0f);
	glRotatef(rot,1.0f,0.0f,0.0f);				// 沿X轴旋转
	glRotatef(rot*1.2f,0.0f,1.0f,0.0f);				// 沿Y轴旋转
	glRotatef(rot*1.4f,0.0f,0.0f,1.0f);				// 沿Z轴旋转
	glTranslatef(-0.35f,-0.35f,0.1f);				// 移动到可以显示的位置
	// 根据字体位置设置颜色
	glColor3f(1.0f*float(cos(rot/20.0f)),1.0f*float(sin(rot/25.0f)),1.0f-0.5f*float(cos(rot/17.0f)));
	glPrint("N");						// 绘制海盗旗符号
	rot+=0.1f;						// 增加旋转变量
	return TRUE;
}
```

最后要做的事就是在KillGLWindow()的最后添加KillFont()函数，但由于上个笔记我们已经做了这个工作，所以这里省略。

当然窗口标题啊什么的小细节也别忘了修改。

编译运行效果如下所示：

![](/assets/2014-03-12-OpenGLTextureMappedOutlineFonts.png)

本笔记运行代码：[下载地址](/code/DataBlog_OpenGLTextureMappedOutlineFonts.cpp)

----

最后，如果你也对OpenGL感兴趣或者你在学习中有任何想法，欢迎在下方留言、与我交流！祝愿我们共同成长、一起进步！
