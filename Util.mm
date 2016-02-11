#include "Util.h"
#include <QByteArray>


NSString* NSStringFromQString(const QString& str)
{
	QByteArray ba = str.toUtf8();
	return [NSString stringWithUTF8String: ba.data()];
}

QString QStringFromNSString(NSString* str)
{
	return QString::fromUtf8([str UTF8String]);
}

