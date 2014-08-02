---
date: 2014-07-30 15:30:00
layout: post
title: 朴素贝叶斯分类算法
thread: 142
categories: 文档
tags: 分类算法
---

**前言：**记些笔记总归是好的，算法实现原理也不难，总结一下看过的对以后使用也算是会有些帮助。

----

##定理综述

贝叶斯分类的基础是概率推理，就是在各种条件的存在不确定，仅知其出现概率的情况下，如何完成推理和决策任务。概率推理是与确定性推理相对应的。而朴素贝叶斯分类器是基于独立假设的，即假设样本每个特征与其他特征都不相关。举个例子，如果一种水果其具有红，圆，直径大概4英寸等特征，该水果可以被判定为是苹果。

从数学角度来说，分类问题可做如下定义：

已知集合<img src="http://latex.codecogs.com/gif.latex?C=\left\{y_1,%20y_2,%20...,y_n\right\}">和<img src="http://latex.codecogs.com/gif.latex?I=\left\{x_1,%20x_2,...,%20x_m,...\right\}">确定映射规则<img src="http://latex.codecogs.com/gif.latex?y=f\left\(x\right\)">，使得任意<img src="http://latex.codecogs.com/gif.latex?x_i\inI">有且仅有一个<img src="http://latex.codecogs.com/gif.latex?y_i\inC使得y_i=f\left\(x_i\right\)">成立。其中C叫做类别集合，其中每一个元素是一个类别，而I叫做项集合，其中每一个元素是一个待分类项，f叫做分类器。分类算法的任务就是构造分类器f。

举个例子：

>医生对病人进行诊断就是一个典型的分类过程，任何一个医生都无法直接看到病人的病情，只能观察病人表现出的症状和各种化验检测数据来推断病情，这时医生就好比一个分类器，而这个医生诊断的准确率，与他当初受到的教育方式（构造方法）、病人的症状是否突出（待分类数据的特性）以及医生的经验多少（训练样本数量）都有密切关系。

----

##定理基础

贝叶斯定理本身原理简单，且同时拥有广泛应用的前景，其解决的问题大致为这样：已知某条件概率，如何得到两个事件交换后的概率，也就是在已知P(A|B)的情况下如何求得P(B|A)。

曾经在概率论课程中学过什么事条件概率：

<img src="http://latex.codecogs.com/gif.latex?P\left\(A|B\right\)">表示事件B已经发生的前提下，事件A发生的概率，叫做事件B发生下事件A的条件概率。其基本求解公式为：

<img src="http://latex.codecogs.com/gif.latex?P\left\(A|B\right\)=\frac{P\left\(AB\right\)}{P\left\(B\right\)}">

而贝叶斯定理如下：

<img src="http://latex.codecogs.com/gif.latex?P\left\(B|A\right\)=\frac{P\left\(A|B\right\)P\left\(B\right\)}{P\left\(A\right\)}">

----

##原理与流程

其中，朴素贝叶斯的思想基础是这样的：对于给出的待分类项，求解在此项出现的条件下各个类别出现的概率，哪个最大，就认为此待分类项属于哪个类别。所以说，选择概率最大的类别就是贝叶斯定理的思想核心。

朴素贝叶斯的正式定义如下所示：

1、设<img src="http://latex.codecogs.com/gif.latex?x=\left\{a_1, a_2,...,a_m\right\}">为一个待分类项，而每个a为x的一个特征属性。

2、有类别集合<img src="http://latex.codecogs.com/gif.latex?C=\left\y_1, y_2,...,y_n\right\">。

3、计算<img src="http://latex.codecogs.com/gif.latex?P\left\(y_1|x\right\), P\left\(y_2|x\right\),..., P\left\(y_n|x\right\)">。

4、如果<img src="http://latex.codecogs.com/gif.latex?P\left\(y_k|x\right\)=max\left\{P\left\(y_1|x\right\), P\left\(y_2|x\right\),..., P\left\(y_n|x\right\)\right\}">，则<img src="http://latex.codecogs.com/gif.latex?x\iny_k">。

而处理步骤中第三步为关键之处。为了求得条件概率，我们可以这么做：

1、挑选一个一已知分类的样本集；

2、统计得到在各类别下各个特征属性的条件概率估计，即<img src="http://latex.codecogs.com/gif.latex?P(a_1|y_1),P(a_2|y_1),...,P(a_m|y_1);P(a_1|y_2),P(a_2|y_2),...,P(a_m|y_2);...;P(a_1|y_n),P(a_2|y_n),...,P(a_m|y_n)">

3、如果各个特征属性是条件独立的，则根据贝叶斯定理有如下推导：

<img src="http://latex.codecogs.com/gif.latex?P(y_i|x)=\frac{P(x|y_i)P(y_i)}{P(x)}">

4、因为分母对于所有类别为常数，因为我们只要将分子最大化皆可。所以我们有：

<img src="http://latex.codecogs.com/gif.latex?P(x|y_i)P(y_i)=P(a_1|y_i)P(a_2|y_i)...P(a_m|y_i)P(y_i)=P(y_i)\prod^m_{j=1}P(a_j|y_i)">

根据定理的处理流程，分类可以分为三个阶段。

第一阶段——准备工作阶段，为朴素贝叶斯分类做必要的准备，主要是确定特征属性和对每个特征属性进行适当划分，形成训练样本集合。

第二阶段——分类器训练阶段，这个阶段主要工作是计算每个类别在训练样本中的出现频率及每个特征属性划分对每个类别的条件概率估计。工作可根据公式由程序自动计算完成。

第三阶段——应用阶段。这个阶段的任务是使用分类器对待分类项进行分类，其输入是分类器和待分类项，输出是待分类项与类别的映射关系。这一阶段也是机械性阶段，由程序完成。

----

##特征属性划分的条件概率及Laplace校准

当特征属性为离散值时，只要很方便的统计训练样本中各个划分在每个类别中出现的频率即可用来估计P(a|y)，下面重点讨论特征属性是连续值的情况。

当特征属性为连续值时，通常假定其值服从高斯分布（也称正态分布）。即：

<img src="http://latex.codecogs.com/gif.latex?g(x,\eta%20,\sigma%20)=\frac{1}{\sqrt{2\pi%20}\sigma%20}e^-\frac{(x-\eta)^2}{2\sigma^2}">，而<img src="http://latex.codecogs.com/gif.latex?P(a_k|y_i)=g(a_k,\eta_{y_i},\sigma_{y_i})">

因此只要计算出训练样本中各个类别中此特征项划分的各均值和标准差，代入上述公式即可得到需要的估计值。

另一个需要讨论的问题就是当P(a|y)=0怎么办，当某个类别下某个特征项划分没有出现时，就是产生这种现象，这会令分类器质量大大降低。这时可以引入Laplace校准。

Laplace校准：即假定训练数据库很大，以至于对每个计数加1造成的估计概率的变化忽略不计。

最后，通常分类器的正确率指分类器正确分类的项目占所有被分类项目的比率。为了使得分类器的准确率符合正常情况，一般会在构造初期就将所拿到的训练数据一分为二，一部分用来构造分类器，另一部分做检测分类器。

----

*文章学习自张洋《[算法杂货铺——分类算法之朴素贝叶斯分类(Naive Bayesian classification)](http://www.cnblogs.com/leoo2sk/archive/2010/09/17/naive-bayesian-classifier.html)》*