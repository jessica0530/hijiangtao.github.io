---
date: 2014-02-16
layout: post
title: “软件包*没有可供安装的候选者”解决方案
thread: 34
categories: 教程
tags: linux
---

当在ubuntu命令行中敲入`sudo apt-get install vim`后显示信息如下：

>正在读取软件包列表... 完成

>正在分析软件包的依赖关系树       

>正在读取状态信息... 完成       

>现在没有可用的软件包 vim，但是它被其它的软件包引用了。

>这可能意味着这个缺失的软件包可能已被废弃，

>或者只能在其他发布源中找到

真是纳了闷以前安装都没问题，为什么会突然这样呢？我检查了一下网络正常连接啊，难道是姿势不对？

网上查到如下一种解决方法：

>去系统-->系统管理-->更新管理器-->设置-->其他软件，都选上。然后再执行`apt-get install vim`

试过之后还是有问题，我觉得应该是我的软件源有问题，于是：

```
    sudo apt-get update
```

然后就看到一大串的更新信息出现，之后再安装vim就没问题了。