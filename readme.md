### 1. 致谢
&emsp;&emsp;感谢 草飘灵sky（qq：2550688560）在 FFT 对基数限制理解的讲解以及解决方案的提供。以及某个不愿意透露姓名的大佬对 FFT 中涉及到的位运算代码的理解。
### 2. 资料参考
#### 视频：
1. [微分方程概论-第四章：但什么是傅立叶级数呢？-从热流到画圈圈](https://www.bilibili.com/video/av62763042)
2. [如何用傅里叶级数绘制出任意图像来?双语](https://www.bilibili.com/video/av49238862/?spm_id_from=333.788.videocard.7)
3. [【算法】并查集（Disjoint Set）[共3讲]](https://www.bilibili.com/video/av38498175?from=search&seid=18147424545102834054)
4. [canny edge detection (canny边缘检测)](https://b23.tv/av44979367
)
#### 文本：
1. [手把手教你用傅立叶变换画可达鸭 -- 知乎](https://zhuanlan.zhihu.com/p/72632360)
2. [Delaunay三角化实现原理](https://blog.csdn.net/renhaofan/article/details/82261681)
3. [一小时学会快速傅里叶变换（Fast Fourier Transform）](https://zhuanlan.zhihu.com/p/31584464)
4. [Canny边缘检测算法的实现](https://www.cnblogs.com/yzl050819/p/8018022.html)
5. [win7-64位系统下，VC++2010中设置FFTW时遇到的若干问题及解决办法](http://blog.sina.com.cn/s/blog_b528d7c70102w9f7.html)
#### 函数库：
1. Delaunay（三角剖分）：https://github.com/Bl4ckb0ne/delaunay-triangulation
2. FFTW：http://www.fftw.org/install/windows.html
### 3. 运行前的准备工作
已知的可运行系统为 Windows 10，环境为 Visual Studio 2017。
#### 环境
1. OpenGL（GLFW GLAD） 相关配置
2. glm 库
3. 三角剖分库 https://github.com/Bl4ckb0ne/delaunay-triangulation
4. FFTW 库（Windows）: http://www.fftw.org/install/windows.html
FFTW 库（macOS）: https://download.csdn.net/download/qq_38130710/10608395
macOS 下解压 .deb 文件: https://mrmad.com.tw/macos-dpkg-deb
【32位还是64位请查看 vs2017 所建立项目中默认的引用目录，例如 VC_referencesPath_x86 为32位。另外，一定要将三个 xxx.dll 放在 xxx.vcxproj 同路径下，项目中默认放置的是32位文件。目前还不支持 macOS Catalina 10.15 [请参阅此链接](http://www.finkproject.org/)】

5. macOS（Retina 屏） 系统中，渲染非默认 FBO 时，调整视口函数为当前窗口尺寸的两倍，在渲染默认 FBO 时，调整视口函数为当前窗口尺寸。
### 4. 思路
#### 1. 描点
&emsp;&emsp;从 3B1B 的视频中了解到，如果要使用傅里叶变换绘制出形状，首先需要构成形状的样本点，根据样本点计算出向量及其周期，具体思路可以参阅 [手把手教你用傅立叶变换画可达鸭 -- 知乎](https://zhuanlan.zhihu.com/p/72632360) 。如果要取采样点，首先需要识别出图像的轮廓，这里具体的做法可以参阅博客 [Canny边缘检测算法的实现](https://www.cnblogs.com/yzl050819/p/8018022.html) 以及视频 [canny edge detection (canny边缘检测)](https://b23.tv/av44979367)。对于图像的处理，我是使用片段着色器和 FBO 完成的，在 [Canny边缘检测算法的实现](https://www.cnblogs.com/yzl050819/p/8018022.html) 的最后一步滞后边缘追踪这里，我只是去除了 3*3格子中边缘像素点不足两个的像素点，因此去噪效果不是很好。

大致的步骤为：
1. 对图像进行灰度处理（因为二值化之后只需要黑白两种颜色）
2. 对图像进行高斯模糊处理（去除部分噪点）
3. 使用 Sobel 算子计算梯度，并选取梯度最大的像素点作为边缘
4. 非最大值抑制，保留颜色值较明显的线条，再一次去除噪点
5. 二值化处理，对要留下来的边缘统一设置为一个颜色值。
6. 去除周围 3*3 格子中边缘不足 2 的像素点
#### 2. 获取路径（描点顺序）
&emsp;&emsp;形状的构成是由点连接的，但这里要考虑如何连接？怎样连接不会走重复路径？怎样连接不会形成环路？这些问题以及解决方案在 [手把手教你用傅立叶变换画可达鸭 -- 知乎](https://zhuanlan.zhihu.com/p/72632360) 博客中阐述的很清楚，因此我参照着他的思路，实现了点的连接。

大致步骤为：
1. 通过三角剖分连接所有点（C++ 可使用这个库 https://github.com/Bl4ckb0ne/delaunay-triangulation）
2. 使用 Kruskal 算法和并查集构建最小生成树
3. 使用无环路的最长路径查找算法（这里的使用的是迭代实现，递归层数太多，大致思路为，先从随机一个点找到该点处于的最长路径，这条路径的另一个端点为整个图最长路径两端的某一个点。在通过这个点即在执行上述算法可获得整个图的最长路径。之后，根据这条最长路径以此遍历路径中点所连接的所有支路，即可绘制完整个图）
#### 3. 绘制向量
&emsp;&emsp;根据 [手把手教你用傅立叶变换画可达鸭 -- 知乎](https://zhuanlan.zhihu.com/p/72632360) 可知一个坐标系中的二维点，是可以由一系列不同长度和周期的向量通过首尾相连得到的，这些向量则是可以通过傅里叶变换得到的。我最初是自己实现了 O(n<sup>2</sup>) 的离散傅里叶变换，但是这个复杂度效率很低，因此我参考 [一小时学会快速傅里叶变换（Fast Fourier Transform）](https://zhuanlan.zhihu.com/p/31584464) 学习了快速傅里叶变换，但是它对基数有限制，必须为 2<sup>n</sup> 个顶点。在 草飘灵sky 的帮助下，我使用了由 MIT 开发的 FFTW 库，解决了这个问题。
&emsp;&emsp;解决了向量位置的问题，接下来就是绘制向量本身。利用 OpenGL，通过构建三个三角形，绘制出箭头的形状，并配置好设置顶点，旋转等等功能，封装成类（具体参阅源代码 Arrow.h）。
&emsp;&emsp;由于在绘制过程中，向量是不留下痕迹的，墨迹是要留下痕迹的。因此在这里使用 OpenGL 的离屏渲染，保留每一次绘制的墨迹，刷新每一次绘制的向量，从而实现绘制。
#### 4. 效果和效率
&emsp;&emsp;由于未作采样点简化，导致在窗口尺寸为 700 *700 时，样例图片中的 img1.jpg 需要绘制 7158 个像素点，每个像素点需要绘制 7158 个箭头，因此共需要绘制 7158 * 7158 次数，目前还没有想到好的解决方案。测试视频（延时摄影）和所用时间如下：
img1.jpg（700 * 700 窗口尺寸）:
<img src="/example/img1.gif">
![img1](/example/img1.png)

img2.jpg（300 * 300 窗口尺寸）:
<img src="/example/img2.gif">
![img2](/example/img2.png)
