# games101

**本仓库是个人学习games101时在本机（windows10）下做的作业**

## 本机配置

**vs2019 + Eigen + opencv**

### 关于Egien配置

1.   安装vcpkg

2.   利用vcpkg安装Eigen

   ```c++
   .\vcpkg.exe install eigen3:x64-windows
   .\vcpkg.exe integrate install
   ```

### 关于opencv配置

​	**由于vcpkg中opencv库无法下载了，因此只能用传统的工程集成方式，链接如下：**

​	[visual studio2019 + opencv配置（详细教程） - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/635425811)

