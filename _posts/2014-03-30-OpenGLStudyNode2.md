---
date: 2014-03-30 21:00:00
layout: post
title: 计算机图形学学习笔记：向量与空间变换
thread: 97
categories: 文档
tags: OpenGL
---

OpenGL遵守的坐标系是右手坐标系，正方向定义：**从坐标轴正向负方向看去，逆时针旋转为正方向。**

多坐标系使用规则：世界坐标系适用于描述其他坐标系的参考框架，是最大的坐标系；物体坐标系（模型坐标系）与特定物体相关联，随着物体运动而运动。

**向量**：维度、数乘、标准化、加减运算、距离求解、向量点乘与叉乘。

有关向量叉乘与点乘的介绍可以参考《[向量的点乘与叉乘](http://hijiangtao.github.io/2014/03/30/VectorCal/)》。

----

绘制圆：

{% highlight C++ linenos %}
void myDisplay(void)
{
    int i;
    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_POLYGON);
    for(i=0; i<n; ++i)
        glVertex2f(R*cos(2*Pi/n*i), R*sin(2*Pi/n*i));
    glEnd();
    glFlush();
}
{% endhighlight %}

绘制点
{% highlight C++ linenos %}
//设置点大小
glPointSize(Glfloat size);

//设置点绘制模式
glBegin(GL_POINTS);
glEnd();

//设置点位置
glVertex3f(Glfloat x, Glfloat y, Glfloat z);

//设置点颜色
glColor3f(Glfloat r, Glfloat g, Glfloat b);

//相关函数
//光滑（反走样）
glEnable (GL_POINT_SMOOTH);
glEnable (GL_BLEND);
glBlendFunc (GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
{% endhighlight %}

绘制直线
{% highlight C++ linenos %}
//设置直线绘制模式
glBegin(GL_LINES);
glBegin(GL_LINE_STRIP);
glBegin(GL_LINE_LOOP);

//设置线的宽度
glLineWidth(Glfloat width);

//点画线(虚线)
glEnable(GL_LINE_STIPPLE);
glLineStipple(GLint factor, GLushort pattern);

//光滑（反走样）
glEnable (GL_LINE_SMOOTH);
glEnable (GL_BLEND);
glBlendFunc (GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
{% endhighlight %}

----

三维空间到二维空间的操作步骤：

世界坐标系中的三维物体->三维几何变换->投影->三维裁剪->视口变换->屏幕坐标系中的图形显示。

**模型视点变换**：

局部坐标：模型相对于自身的坐标位置；一般以模型中心为原点。
世界坐标：把模型放到3维世界后的绝对坐标位置；以世界中心为原点。
视觉坐标：模型相对观察者的坐标位置；以视点位置为原点。

模型变换和视点变换本质上是一致的：对一个的变换相当于对另一个反方向变换，视点变换没有缩放只有平移和旋转。

**投影变换两种方式**

* 透视投影：所有投影线交于投影中心。

```
gluPerspective(GLdouble fovy, GLdouble aspect, 
                GLdouble zNear, GLdouble zFar );
```

* 平行投影：投影线平行，投影中心在无穷远，用投影方向表示。

```
void glOrtho( GLdouble left, GLdouble right, 
                GLdouble bottom, GLdouble top, 
                GLdouble zNear, GLdouble zFar );
```

**视图转换**

```
glViewport(GLint x,GLint y,GLsizei width, GLsizei height);
```

函数参数(x, y)是视口在屏幕窗口坐标系中的左下角点坐标，参数width和height分别是视口的宽度和高度。如此一来可以实现多视图的转换。

**裁剪变换**

```
glClipPlane(GLenum plane,Const GLdouble *equation);
```

参数plane是GL_CLIP_PLANEi(i=0,1,...)，指定裁剪面号；参数equation指向裁剪平面Ax+By+Cz+D=0的A、B、C、D值。