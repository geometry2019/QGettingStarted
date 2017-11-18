#pragma once

#include <QString>

#include "../QGettingStart_Util/Exception.h"

class QGSAbstractGame
{
public:
	QGSAbstractGame()
	{

	}
	virtual ~QGSAbstractGame()
	{

	}
public:
	/*获取游戏版本*/
	virtual QString getGameVersion() = 0;
	/*获取游戏路径（绝对/相对）*/
	virtual QString getGamePath() = 0;
	/*获取Java路径*/
	virtual QString getJavaPath() = 0;
	/*获取Native文件夹路径*/
	virtual QString getNativePath() = 0;
	/*获取用户名*/
	virtual QString getPlayerName() = 0;
	/*获取用户类型*/
	virtual QString getUserType() = 0;
	/*获取accessToken*/
	virtual QString getAccessToken() = 0;
	/*获取clientToken*/
	virtual QString getClientToken() = 0;
	/*获取JVM参数*/
	virtual QString getJVMArgument() = 0;
	/*获取前置指令*/
	virtual QString getPreInstruction() = 0;
	/*获取直入服务器地址*/
	virtual QString getServerAddress() = 0;
	/*获取游戏参数*/
	virtual QString getGameArgument() = 0;
	/*获取最小内存*/
	virtual int getXmn() = 0;
	/*获取最大内存*/
	virtual int getXmx() = 0;
};

