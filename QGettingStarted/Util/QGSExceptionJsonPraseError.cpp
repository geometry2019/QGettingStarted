#include "QGSExceptionJsonPraseError.h"

QGSExceptionJsonPraseError::QGSExceptionJsonPraseError(const QJsonParseError & jsonPraseError):mJsonPraseError(jsonPraseError)
{
}

QGSExceptionJsonPraseError::~QGSExceptionJsonPraseError()
{
}

QGSExceptionJsonPraseError * QGSExceptionJsonPraseError::clone()const
{
	return new QGSExceptionJsonPraseError(*this);
}

void QGSExceptionJsonPraseError::raise()const
{
	throw *this;
}

QJsonParseError QGSExceptionJsonPraseError::getJsonPraseError()const
{
	return mJsonPraseError;
}