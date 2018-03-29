#pragma once

#include <QString>

#include "QGSIAccount.h"
#include "Util/QGSNetworkAccessManager.h"

class QGSYggdrasilAccount :public QGSIAccount
{
public:
	QGSYggdrasilAccount();

	virtual ~QGSYggdrasilAccount();

	QGSYggdrasilAccount(const QGSYggdrasilAccount & right) = delete;

	QGSYggdrasilAccount(QGSYggdrasilAccount && right) = delete;

	QGSYggdrasilAccount & operator=(const QGSYggdrasilAccount & right) = delete;

	QGSYggdrasilAccount & operator=(QGSYggdrasilAccount && right) = delete;

	virtual QGSAuthInfo authenticate(const QString & userName, const QString & password = "", QString clientToken = "", QNetworkProxy proxy = QNetworkProxy::NoProxy);
private:
};
