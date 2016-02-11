#include "NSQEvent.h"
#include <QMouseEvent>
#include <Foundation/NSException.h>

static void throwInconsistency()
{
	[NSException raise: NSInternalInconsistencyException
				format: @"Invalid NSEvent type for this call"];
}

@implementation NSQEvent

-(id) initWithQEvent: (std::shared_ptr<QEvent>) event
				 WId: (WId) wid
{
	self = [super init];
	m_event = event;
	NSLog(@"NSQQvent for QEvent %p", m_event.get());
	m_wid = wid;
	m_ts = [[NSDate date] timeIntervalSince1970];
	return self;
}

-(CGEventRef) CGEvent
{
	return m_event.get();
}

- (NSTimeInterval)timestamp
{
	return m_ts;
}

- (NSEventType)type
{
	switch (m_event->type())
	{
		case QEvent::KeyPress:
			return NSKeyDown;
		case QEvent::KeyRelease:
			return NSKeyUp;
		case QEvent::MouseButtonPress:
			switch (std::static_pointer_cast<QMouseEvent>(m_event)->button())
			{
				case Qt::LeftButton:
					return NSLeftMouseDown;
				case Qt::RightButton:
					return NSRightMouseDown;
				default:
					return NSOtherMouseDown;
			}
		case QEvent::MouseButtonRelease:
			switch (std::static_pointer_cast<QMouseEvent>(m_event)->button())
			{
				case Qt::LeftButton:
					return NSLeftMouseUp;
				case Qt::RightButton:
					return NSRightMouseUp;
				default:
					return NSOtherMouseUp;
			}
		case QEvent::Enter:
			return NSMouseEntered;
		case QEvent::Leave:
			return NSMouseExited;
		case QEvent::Wheel:
			return NSScrollWheel;
		case QEvent::MouseMove:
			return NSMouseMoved; // TODO: NSLeftMouseDragged / NSRightMouseDragged
		default:
			return NSAppKitDefined;
	}
}

- (NSInteger)windowNumber
{
	return m_wid;
}

- (NSWindow*) window
{
	QWindow* win = QWindow::fromWinId(m_wid);
	return nullptr; // TODO: when we have NSWindow
}

- (NSPoint)locationInWindow
{
	std::shared_ptr<QMouseEvent> ev = std::dynamic_pointer_cast<QMouseEvent>(m_event);
	if (ev)
		return NSPoint { CGFloat(ev->x()), CGFloat(ev->y()) };
	else
		return NSPoint{0, 0};
}

- (NSInteger)buttonNumber
{
	std::shared_ptr<QMouseEvent> ev = std::dynamic_pointer_cast<QMouseEvent>(m_event);
	if (!ev)
		throwInconsistency();
	return ev->button();
}

- (unsigned short)keyCode
{
	NSEventType type = [self type];
	
	if (type != NSKeyDown && type != NSKeyUp)
		throwInconsistency();
	
	return std::static_pointer_cast<QKeyEvent>(m_event)->key();
}

- (NSString *)characters
{
	NSEventType type = [self type];
	QByteArray ba;
	
	if (type != NSKeyDown && type != NSKeyUp)
		throwInconsistency();

	ba = std::static_pointer_cast<QKeyEvent>(m_event)->text().toUtf8();

	return [NSString stringWithUTF8String: ba.data()];
}

- (NSString *)charactersIgnoringModifiers
{
	return [self characters];
}

@end


