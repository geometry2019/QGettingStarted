# QGettingStarted
(重构中2333)
An open-source library for launching and downloading 'Minecraft'.

 * 启动全版本Minecraft
 * 可拓展登录系统
   * 支持Offline（离线）
   * 支持Yggdrasil（开发中）
 * 下载Minecraft（开发中）
 * 下载Forge/Liteloader（开发中）
   * 支持Liteloader的快照版本
   * 自定义下载源
 * Natives解压（开发中）

# 编译
Visual Studio 2015/Qt 5.10.0 下编译通过。

# 开始
```cpp
/*启动1.11.2原版为例*/

//Qt
#include <QtCore/QCoreApplication>
#include <QSharedPointer>
//QGettingStarted
#include "QGettingStarted.h"

using namespace std;
using namespace qgs;//核心的名称空间

int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);

  QGSGameDirectory gameDirectory{ QDir(".minecraft") };

  //启动器工厂
	QSharedPointer<QGSLauncherFactory> launcherFactory{ new QGSLauncherFactory };
  //生成合适的启动器
	QSharedPointer<QGSILauncher>launcher{ launcherFactory ->createLauncher("1.11.2",gameDirectory)};
	
  QGSLaunchOptionsBuilder launchOptionsBuilder;//启动选项建造者
	launchOptionsBuilder.setJavaPath("C:/Program Files/Java/jre1.8.0_121/bin/javaw.exe");//Java路径
	launchOptionsBuilder.setMaxMemory(1024);//最大内存（MB）
	launchOptionsBuilder.setMinMemory(128);//最小内存（MB）
	launchOptionsBuilder.setAuthInfo(QGSOfflineAccountFactory().createAccount()->authenticate("gou"));//用户，这里是离线用户
	//launchOptionsBuilder.setJVMArguments("-XX:+UseG1GC -XX:-UseAdaptiveSizePolicy -XX:-OmitStackTraceInFastThrow");//可选的JVM虚拟机参数
  
  //生成启动命令，生成的命令可以直接用QProcess执行
	QString launchCommand;//启动命令
  launcher->generateLaunchCommand(launchOptionsBuilder.getLaunchOptions(), launchCommand);
  
  return 0;
}

```
