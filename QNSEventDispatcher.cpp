#include "QNSEventDispatcher.h"
#include <cstdint>
#include <fcntl.h>
#include <QTimerEvent>
#include <iostream>
#include <QCoreApplication>
#include <QSocketNotifier>
#include <QPair>
//#include <qpa/qwindowsysteminterface.h>
#include <QtDebug>

#define DISTANT_FUTURE	63113990400.0

QNSEventDispatcher* QNSEventDispatcher::instance()
{
	static QNSEventDispatcher instance;
	return &instance;
}

QNSEventDispatcher::QNSEventDispatcher()
{
	m_runLoop = CFRunLoopGetMain();
	m_queue = dispatch_get_main_queue();
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
	dispatch_source_t source;
	
	// NSLog(@"registerSocketNotifier: %p", notifier);
	std::cout << "registerSocketNotifier: fd=" << socket << " type=" << notifier->type() << std::endl;
	
	switch (notifier->type())
	{
	case QSocketNotifier::Read:
	case QSocketNotifier::Exception:
		source = dispatch_source_create(DISPATCH_SOURCE_TYPE_READ, socket, 0, m_queue);
		break;
	case QSocketNotifier::Write:
		source = dispatch_source_create(DISPATCH_SOURCE_TYPE_WRITE, socket, 0, m_queue);
		break;
	default:
		return;
	}
	
	dispatch_source_set_event_handler(source, ^{
		QEvent event(QEvent::SockAct);
		QCoreApplication::sendEvent(notifier, &event);
	});
	
	dispatch_resume(source);
	m_sockets[notifier] = source;
}

void QNSEventDispatcher::registerTimer(int timerId, int interval, Qt::TimerType timerType, QObject* object)
{
	MyTimerInfo timerInfo;
	dispatch_source_t source;
	int leeway;
	
	switch (timerType)
	{
		case Qt::PreciseTimer:
			leeway = 0;
			break;
		case Qt::CoarseTimer:
			leeway = int(interval * 1000000 * 0.05f);
			break;
		case Qt::VeryCoarseTimer:
			leeway = 1000 * 1000 * 1000;
			break;
		default:
			leeway = 0;
			break;
	}
	
	source = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0, m_queue);
	dispatch_source_set_timer(source, 0, interval * 1000000, leeway);
	
	timerInfo.interval = interval;
	timerInfo.type = timerType;
	timerInfo.target = object;
	timerInfo.source = source;
	timerInfo.lastFired = QDateTime::currentDateTime();
	
	m_timers[timerId] = timerInfo;
	
	dispatch_source_set_event_handler(source, ^{
		m_timers[timerId].lastFired = QDateTime::currentDateTime();
		fireTimer(timerId);
	});
	
	dispatch_resume(source);
}

void QNSEventDispatcher::fireTimer(int timerId)
{
	QTimerEvent ev(timerId);
	QCoreApplication::sendEvent(m_timers[timerId].target, &ev);
}

int QNSEventDispatcher::remainingTime(int timerId)
{
	if (!m_timers.contains(timerId))
		return -1;
	
	MyTimerInfo& timer = m_timers[timerId];
	QDateTime next = timer.lastFired.addMSecs(timer.interval);
	
	return QDateTime::currentDateTime().msecsTo(next);
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
	dispatch_source_t source;
	
	if (!m_sockets.contains(notifier))
		return;
	
	source = m_sockets[notifier];
	dispatch_source_cancel(source);
	dispatch_release(source);
}

bool QNSEventDispatcher::unregisterTimer(int timerId)
{
	if (!m_timers.contains(timerId))
		return false;

	MyTimerInfo& timer = m_timers[timerId];
	
	dispatch_source_cancel(timer.source);
	dispatch_release(timer.source);

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


