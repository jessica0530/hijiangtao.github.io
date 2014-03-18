---
date: 2014-03-18 23:30:00
layout: post
title: Aspectj操作与语法基础学习笔记（五）
thread: 85
categories: 文档
tags: Aspectj
---

**前言：**本学习笔记原稿为CSDN大胡子的[aspectj的学习专栏](http://my.csdn.net/zl3450341)，想到根据自己的思想进一步凝练其中内容和为了以后巩固知识的需要，于是有了自己的这个学习笔记系列。

**Aspectj学习笔记系列：**

1. 《[Aspectj操作与语法基础学习笔记（一）](http://hijiangtao.github.io/2014/03/18/AspectjStudyNode1/)》
2. 《[Aspectj操作与语法基础学习笔记（二）](http://hijiangtao.github.io/2014/03/18/AspectjStudyNode2/)》
3. 《[Aspectj操作与语法基础学习笔记（三）](http://hijiangtao.github.io/2014/03/18/AspectjStudyNode3/)》 
4. 《[Aspectj操作与语法基础学习笔记（四）](http://hijiangtao.github.io/2014/03/18/AspectjStudyNode4/)》
5. 《[Aspectj操作与语法基础学习笔记（五）](http://hijiangtao.github.io/2014/03/18/AspectjStudyNode5/)》

----

本系列笔记会根据内容多少进行调整，并且其中的代码等内容与原专栏内容会有所改变，请注意。

我坚信开源与分享是助力学习与成长的最重要元素，所以也欢迎你基于我的笔记基础进行进一步增补与修改，以此帮助更多的人。

----

##一、advice实践

Advice在AspectJ有三种：before、 after、Around之分，是真正的执行代码，或者说关注的实现，类似Action。

有关Advice的用法，我研究的也不是特别清楚，所以这里就不好整理出来了，有关权威的介绍可以参考[官方文档-Advice](https://www.eclipse.org/aspectj/doc/next/progguide/semantics-advice.html)。

接下来重点整理的是around通知：

```
package com.aspectj.demo.aspect;
import com.aspectj.demo.test.HelloAspectDemo;;

public aspect HelloAspect {  
	pointcut HelloWorldPointCut(int x) : execution(* main(int)) && !within(HelloAspectDemo) && !within(Testfow) && args(x);  

	int around(int x) : HelloWorldPointCut(x){  
		System.out.println("Entering : " + thisJoinPoint.getSourceLocation());  
		int newValue = proceed(x*3);  
		return newValue;-
	}
}
```

修改HelloWorld.java文件如下：

```
package com.aspectj.demo.aspect;
import com.aspectj.demo.test.HelloAspectDemo;;

public aspect HelloAspect {  
	pointcut HelloWorldPointCut(int x) : execution(* main(int)) && !within(HelloAspectDemo) && args(x);  

	int around(int x) : HelloWorldPointCut(x){  
		System.out.println("Entering : " + thisJoinPoint.getSourceLocation());  
		int newValue = proceed(x*3);  
		return newValue;
	}    
}  
```

最主要的就是 proceed（）这个方法，运行时结果如下所示：

```
Entering : HelloWorld.java:6
in the main method  i = 15
```

----

##二、Annotation参考资料与Demo

自从JDK5.0加入了annotation以后，asepctj也提供对annotaion的支持，而且命名也模仿JDK，从1.4的版本改为5.0  也就是Aspectj5，或者称@Aspectj。其中最重要的一项就是pointcut  支持对Annotaion的选取了。

Join Point Matching based on Annotations - [Chapter 2 Annotations](http://www.eclipse.org/aspectj/doc/released/adk15notebook/annotations-pointcuts-and-advice.html)

----

好了，之后的内容就不再整理了，我也没有过多的去查看，毕竟还有别的事情要忙，更深入的学习就等到以后慢慢找时间来补了吧。

如果有任何疑问欢迎在下方留言，希望能认识到同样热爱技术的你。

希望有一天，当我回来查看自己曾经写过的笔记时能看到你在下方的留言。我甚至无法想象，那种与你一起交流、共同进步的那种快感。