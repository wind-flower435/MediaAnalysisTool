# Media analysis tools

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
2. 程序兼容ffmpeg 3.0.9 和 ffmpeg 4.3.1动态库，但是二者依赖的头文件不同，如果需要加载不同版本的ffmpeg库需要重新编译程序。
3. ffmpeg 3.0.9 经测试不支持视频编码格式为hevc 和 rawvideo类型。
4. 需要用到的库：
* avcodec-xx.dll
* avformat-xx.dll
* avutil-xx.dll
* swresample-xx.dll
* swscale-xx.dll
* SDL2.dll

## 4存在问题
1. 加载ffmpeg4.3.1库，播放hevc视频时有较大杂音。