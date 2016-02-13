#ifndef QNSEVENTDISPATCHER_H
#define QNSEVENTDISPATCHER_H
#include <QtCore/qabstracteventdispatcher.h>
#include <QHash>
#include <QQueue>
#include <QTimerEvent>
#include <QDateTime>
#include <dispatch/dispatch.h>
#include <CoreFoundation/CFRunLoop.h>

class QNSEventDispatcher : public QAbstractEventDispatcher
{
private:
	QNSEventDispatcher();
	~QNSEventDispatcher();
public:
	static QNSEventDispatcher* instance();

	virtual void flush() override;
	virtual bool hasPendingEvents() override;
	virtual void interrupt() override;
	virtual bool processEvents(QEventLoop::ProcessEventsFlags flags) override;
	virtual void registerSocketNotifier(QSocketNotifier* notifier) override;
	virtual void registerTimer(int timerId, int interval, Qt::TimerType timerType, QObject* object) override;
	virtual QList<TimerInfo> registeredTimers(QObject* object) const override;
	virtual void unregisterSocketNotifier(QSocketNotifier* notifier) override;
	virtual bool unregisterTimer(int timerId) override;
	virtual bool unregisterTimers(QObject* object) override;
	virtual void wakeUp() override;
	virtual int remainingTime(int timerId) override;

	void fireTimer(int timerId);
private:
	struct MyTimerInfo
	{
		int interval;
		Qt::TimerType type;
		QObject* target;
		dispatch_source_t source;
		struct timeval tv;
		QDateTime lastFired;
	};

	CFRunLoopRef m_runLoop;
	dispatch_queue_t m_queue;
	QHash<int, MyTimerInfo> m_timers;
	QHash<QSocketNotifier*, dispatch_source_t> m_sockets;
};

#endif

