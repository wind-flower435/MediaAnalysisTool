<!--
 * @Author: your name
 * @Date: 2021-09-24 15:03:40
 * @LastEditTime: 2021-09-24 15:57:00
 * @LastEditors: your name
 * @Description: In User Settings Edit
 * @FilePath: \undefinede:\论文\Media analysis tool\MediaAnalysisTool\readme.md
-->

![avatar](https://img.shields.io/badge/2.1.0-A%20tiny%20Media%20Analysis%20Tool-brightgreen)

# Media analysis tools

#### 介绍
该项目主要分为4部分：
* <b>音频播放模块</b>
* <b>视频播放模块</b>
* <b>控制模块</b>
* <b>管理音频设备模块</b>

采用ffmpeg，sdl和qt构建的简单媒体分析工具。采用动态加载ffmpeg与sdl库的方式来播放视/音频。可正常播放视频或音频，解决视音频同步的问题，支持一键开启/关闭当前音频设备以及显示媒体相关信息的功能。


## 编译环境
* Windows10 + MSVC2019 + qmake
* ffmpeg-3.0.9-win32 + SDL2 -2.0.14-VC-x86
## 1 编译
`qmake`

`nmake Debug Release`

## 2 程序界面介绍与使用
* Select: 选择ffmpeg库与sdl库的路径(ffmpeg库与sdl库放在同一路径下)
* Media：选择要播放的媒体文件
* info：显示媒体信息
* 开启/关闭音频：开启/关闭当前音频设备
* Qt5Core.dll; Qt5Gui.dll; Qt5Widgets.dll; platforms 先加载动态库，才能进行播放媒体文件
  
## 3 note:
1. ffmpeg 与 sdl库只支持32位版本的，因为程序是32位的。
<font size = 2>$\color{red}{该程序兼容ffmpeg 3.0.9 和 ffmpeg 4.3.1动态库，但是二者依赖的头文件不同，如果需要加载不同版本的ffmpeg库需要重新编译程序。}$</font>
2. ffmpeg 3.0.9 经测试不支持视频编码格式为hevc 和 rawvideo类型。
3. 需要用到的库：
* avcodec-xx.dll
* avformat-xx.dll
* avutil-xx.dll
* swresample-xx.dll
* swscale-xx.dll
* SDL2.dll
