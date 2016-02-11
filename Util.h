#ifndef APPKIT_UTIL_H_
#define APPKIT_UTIL_H_
#include <Foundation/NSString.h>
#include <QString>

NSString* NSStringFromQString(const QString& str);
QString QStringFromNSString(NSString* str);

#endif

