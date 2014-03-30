---
date: 2014-03-30 10:00:00
layout: post
title: 计算机图形学学习笔记：窗口与图形绘制
thread: 96
categories: 文档
tags: OpenGL
---

**图形显示设备的显色原理分为两种：**

1. 加色
通过把三基色的值加在一起形成一种颜色（彩色显示器、投影仪、照片正片(positive film)），原色为红(red), 绿(green), 蓝(blue)。

2. 减色
通过在白光中过滤掉蓝绿色(cyan), 洋红色(magenta)和黄色(yellow)形成最终的颜色：
    * 光线与物体的作用
    * 彩色打印
    * 照片底片(negative film)

----

**创建窗口代码简单介绍：**

{% highlight C++ linenos %}
class CmyOpenGL : public COpenGL
{
public:
	CmyOpenGL();		        //构造函数
	virtual ~CmyOpenGL();     //析构函数
	virtual void InDraw(void);	//绘制过程
	virtual void PostInit(void);//绘制相关初始化数据
};

void CmyOpenGL::PostInit(void)
{
    //设置背景颜色
	glClearColor(0.4,0.6,0.8,1);	
    //设置摄像机初始位置
	m_pCamera->SetCamera(CglVector3(15,7,30),CglVector3(0,0,0),true);
    //保存摄像机初始位置
	m_pCamera->SaveCamera();
	return ;
}

void CmyOpenGL::InDraw(void)
{	
	//绘制坐标轴
	m_pShape->DrawCoord(CglVector3(0,0,0),110,true);
	//绘制地板
	m_pShape->DrawFloor(100,10,1);
}
{% endhighlight %}

----

**绘制简单的几何图形：**点、直线和多边形

在OpenGL中指定顶点glVertex：glVertex3d, glVertex3f, glVertex3i, glVertex3fv

开始绘制

{% highlight C++ linenos %}
glBegin(GLenum mode);
	glVertex
glEnd();
{% endhighlight %}

其中可选模式GLenum mode有：GL_POINTS 、GL_LINES 、GL_LINE_STRIP 、GL_LINE_LOOP 、GL_TRIANGLES 、GL_TRIANGLE_STRIP 、GL_TRIANGLE_FAN 、GL_QUADS 、GL_QUAD_STRIP 、GL_POLYGON。
