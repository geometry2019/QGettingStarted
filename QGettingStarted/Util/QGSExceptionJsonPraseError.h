#pragma once

#include "QGSException.h"
#include <QJsonDocument>

class QGSExceptionJsonPraseError :public QGSException
{
public:
	QGSExceptionJsonPraseError() = default;

	QGSExceptionJsonPraseError(const QJsonParseError & jsonPraseError);

	virtual ~QGSExceptionJsonPraseError();

	virtual QGSExceptionJsonPraseError * clone()const override;

	virtual void raise()const override;

	QJsonParseError getJsonPraseError()const;
private:
	QJsonParseError mJsonPraseError;
};

