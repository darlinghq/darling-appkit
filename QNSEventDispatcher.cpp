#include "QNSEventDispatcher.h"
#include <cstdint>
#include <fcntl.h>
#include <QTimerEvent>
#include <iostream>
#include <QCoreApplication>
#include <QSocketNotifier>
#include <CoreFoundation/CFRunLoop.h>
#include <QPair>
//#include <qpa/qwindowsysteminterface.h>
#include <QtDebug>

#define DISTANT_FUTURE	63113990400.0

void QNSEventDispatcher::CFSocketCallback(CFSocketRef s, CFSocketCallBackType callbackType, CFDataRef address, const void *data, void *info)
{
	QEvent event(QEvent::SockAct);
        int socket = CFSocketGetNative(s);
	QSocketNotifier* notifier;
	
	if (callbackType == kCFSocketReadCallBack)
		notifier = QNSEventDispatcher::instance()->m_sockets[socket].readNotifier;
	else if (callbackType == kCFSocketWriteCallBack)
		notifier = QNSEventDispatcher::instance()->m_sockets[socket].writeNotifier;
	else
		return;
	
        qDebug() << "Sending socket event";
	QCoreApplication::sendEvent(notifier, &event);
}

static void CFRunLoopTimerCallback(CFRunLoopTimerRef timer, void *info)
{
	QNSEventDispatcher::instance()->fireTimer((int)(uintptr_t) info);
}

QNSEventDispatcher* QNSEventDispatcher::instance()
{
	static QNSEventDispatcher instance;
	return &instance;
}

QNSEventDispatcher::QNSEventDispatcher()
{
	m_runLoop = CFRunLoopGetCurrent();
}

QNSEventDispatcher::~QNSEventDispatcher()
{
}

void QNSEventDispatcher::flush()
{
}

bool QNSEventDispatcher::hasPendingEvents()
{
	extern Q_CORE_EXPORT uint qGlobalPostedEventsCount();
	return qGlobalPostedEventsCount() > 0 || !CFRunLoopIsWaiting(m_runLoop);
}

void QNSEventDispatcher::interrupt()
{
        qDebug() << "Interrupt";
	CFRunLoopStop(m_runLoop);
}

class QWindowSystemInterface
{
public:
        static void sendWindowSystemEvents(QEventLoop::ProcessEventsFlags flags);
};

bool QNSEventDispatcher::processEvents(QEventLoop::ProcessEventsFlags flags)
{
	emit awake();
	
	// NSLog(@"QNSEventDispatcher::processEvents, WaitForMore: %d", flags & QEventLoop::WaitForMoreEvents);
	
	QCoreApplication::sendPostedEvents();
	
	if (flags & QEventLoop::ExcludeSocketNotifiers)
		; // TODO
	if (flags & QEventLoop::X11ExcludeTimers)
		; // TODO
	
	if (flags & QEventLoop::WaitForMoreEvents)
	{
		CFRunLoopRunInMode(kCFRunLoopDefaultMode, DISTANT_FUTURE, true);
	}
	else
	{
		CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0, false);
	}
	qDebug() << "Run loop end";
	
	QCoreApplication::sendPostedEvents();
        QWindowSystemInterface::sendWindowSystemEvents(flags);
	
	return true;
}

void QNSEventDispatcher::registerSocketNotifier(QSocketNotifier* notifier)
{
	int socket = notifier->socket();
	CFSocketRef cfSocket;
	CFOptionFlags cbTypes;
	
	if (!m_sockets.contains(socket))
	{
		CFRunLoopSourceRef src;
		CFSocketContext ctxt;
		MySocketInfo info;
		
		CFOptionFlags flags = kCFSocketAutomaticallyReenableReadCallBack | kCFSocketAutomaticallyReenableWriteCallBack;
		
		memset(&ctxt, 0, sizeof(ctxt));
		cfSocket = CFSocketCreateWithNative(NULL, socket, 0, CFSocketCallback, &ctxt);
		CFSocketSetSocketFlags(cfSocket, flags);
		
		src = CFSocketCreateRunLoopSource(NULL, cfSocket, 0);
		CFRunLoopAddSource(m_runLoop, src, kCFRunLoopCommonModes);
		
		info.socket = cfSocket;
		m_sockets[socket] = info;
	}
	
	MySocketInfo& info = m_sockets[socket];
	
	// NSLog(@"registerSocketNotifier: %p", notifier);
	std::cout << "registerSocketNotifier: fd=" << socket << " type=" << notifier->type() << std::endl;
	
	switch (notifier->type())
	{
	case QSocketNotifier::Read:
	// case QSocketNotifier::Exception:
		cbTypes = kCFSocketReadCallBack;
		info.readNotifier = notifier;
		break;
	case QSocketNotifier::Write:
		cbTypes = kCFSocketWriteCallBack;
		info.writeNotifier = notifier;
		break;
	default:
		cbTypes = 0;
	}
	
	CFSocketEnableCallBacks(cfSocket, cbTypes);
}

void QNSEventDispatcher::registerTimer(int timerId, int interval, Qt::TimerType timerType, QObject* object)
{
	CFRunLoopTimerRef timer;

	MyTimerInfo info = { interval, timerType, object };
	double dblInterval = double(interval)/1000.0;
	CFRunLoopTimerContext ctxt;

	memset(&ctxt, 0, sizeof(ctxt));
	ctxt.info = (void*)(uintptr_t) timerId;
	
	timer = CFRunLoopTimerCreate(NULL, CFAbsoluteTimeGetCurrent()+dblInterval, dblInterval, 0, 0,
								CFRunLoopTimerCallback, &ctxt);

	m_timers[timerId] = info;
	m_nsTimers[timerId] = timer;

	CFRunLoopAddTimer(m_runLoop, timer, kCFRunLoopCommonModes);
}

void QNSEventDispatcher::fireTimer(int timerId)
{
	QTimerEvent ev(timerId);
	QCoreApplication::sendEvent(m_timers[timerId].target, &ev);
}

int QNSEventDispatcher::remainingTime(int timerId)
{
	if (!m_nsTimers.contains(timerId))
		return -1;
	
	CFRunLoopTimerRef timer = m_nsTimers[timerId];
	CFAbsoluteTime nextDate = CFRunLoopTimerGetNextFireDate(timer);
	CFAbsoluteTime now = CFAbsoluteTimeGetCurrent();
	
	if (nextDate < now)
		return 0;
	else
		return int((nextDate - now) * 1000);
}

QList<QAbstractEventDispatcher::TimerInfo> QNSEventDispatcher::registeredTimers(QObject* object) const
{
	QList<TimerInfo> rv;

	for (auto it = m_timers.begin(); it != m_timers.end(); it++)
	{
		if (it.value().target == object)
			rv << TimerInfo(it.key(), it.value().interval, it.value().type);
	}

	return rv;
}

void QNSEventDispatcher::unregisterSocketNotifier(QSocketNotifier* notifier)
{
        qDebug() << "Unregister notifier" << notifier;
	int socket = notifier->socket();
	
	if (!m_sockets.contains(socket))
		return;
	
	MySocketInfo& info = m_sockets[socket];
	
	if (info.readNotifier == notifier)
	{
		CFSocketDisableCallBacks(info.socket, kCFSocketReadCallBack);
		info.readNotifier = nullptr;
	}
	else if (info.writeNotifier == notifier)
	{
		CFSocketDisableCallBacks(info.socket, kCFSocketWriteCallBack);
		info.writeNotifier = nullptr;
	}
	
	if (!info.readNotifier && !info.writeNotifier)
	{
		CFSocketInvalidate(info.socket);
		CFRelease(info.socket);
		
		m_sockets.remove(socket);
	}
}

bool QNSEventDispatcher::unregisterTimer(int timerId)
{
	if (!m_nsTimers.contains(timerId))
		return false;

	CFRunLoopTimerRef timer = m_nsTimers.take(timerId);
	CFRunLoopTimerInvalidate(timer);
	CFRelease(timer);
	m_nsTimers.remove(timerId);
	m_timers.remove(timerId);

	return true;
}

bool QNSEventDispatcher::unregisterTimers(QObject* object)
{
	QList<int> ids;
	bool rv = false;

	for (auto it = m_timers.begin(); it != m_timers.end(); it++)
	{
		if (it.value().target == object)
			ids << it.key();
			//rv |= unregisterTimer(it.key());
	}

	for (int _id : ids)
		rv |= unregisterTimer(_id);

	return rv;
}

void QNSEventDispatcher::wakeUp()
{
	CFRunLoopWakeUp(m_runLoop);
}


