#include "QWindowSubclass.h"
#include <AppKit/NSWindow.h>
#include <AppKit/NSEvent.h>
#include "Util.h"
#include <time.h>

QWindowSubclass::QWindowSubclass(NSWindow* win)
: m_window(win)
{
}

void QWindowSubclass::exposeEvent(QExposeEvent * ev)
{
	QQuickWindow::exposeEvent(ev);
}

void QWindowSubclass::focusInEvent(QFocusEvent * ev)
{
	QQuickWindow::focusInEvent(ev);
}

void QWindowSubclass::focusOutEvent(QFocusEvent * ev)
{
	QQuickWindow::focusOutEvent(ev);
}

void QWindowSubclass::hideEvent(QHideEvent * ev)
{
	QQuickWindow::hideEvent(ev);
}

void QWindowSubclass::keyEvent(NSEventType type, QKeyEvent* ev)
{
	NSEvent* event;
	NSEventModifierFlags modifiers = 0;
	
	modifiers = convertModifiers(ev->modifiers());
	
	event = [NSEvent keyEventWithType: type
			location: NSMakePoint(0, 0)
			modifierFlags: modifiers
			timestamp: timestamp()
			windowNumber: this->winId()
			context: NULL
			characters: NSStringFromQString(ev->text())
			charactersIgnoringModifiers: NSStringFromQString(ev->text()) // FIXME?
			isARepeat: ev->isAutoRepeat()
			keyCode: ev->key()
	];
	
	[NSApp postEvent: event
			atStart: false];
}

NSEventModifierFlags QWindowSubclass::convertModifiers(Qt::KeyboardModifiers mod)
{
	NSEventModifierFlags rv = 0;
	if (mod & Qt::ShiftModifier)
		rv |= NSShiftKeyMask;
	if (mod & Qt::ControlModifier)
		rv |= NSControlKeyMask;
	if (mod & Qt::AltModifier)
		rv |= NSAlternateKeyMask;
	if (mod & Qt::MetaModifier)
		rv |= NSCommandKeyMask;
	if (mod & Qt::KeypadModifier)
		rv |= NSNumericPadKeyMask;
	return rv;
}

void QWindowSubclass::keyPressEvent(QKeyEvent * ev)
{
	QQuickWindow::keyPressEvent(ev);
	
	keyEvent(NSKeyDown, ev);
}

void QWindowSubclass::keyReleaseEvent(QKeyEvent * ev)
{
	QQuickWindow::keyReleaseEvent(ev);
}

void QWindowSubclass::mouseEvent(NSEventType type, int clickCount, QMouseEvent* ev)
{
	NSEvent* event;
	Qt::KeyboardModifiers mod = QGuiApplication::keyboardModifiers();
	
	event = [NSEvent mouseEventWithType: type
				location: NSMakePoint(ev->pos().x(), ev->pos().y())
				modifierFlags: convertModifiers(mod)
				timestamp: timestamp()
				windowNumber: this->winId()
				context: NULL
				eventNumber: 0
				clickCount: clickCount
				pressure: 1];
	
	[NSApp postEvent: event
			atStart: false];
}

NSTimeInterval QWindowSubclass::timestamp()
{
	struct timespec ts;
	clock_gettime(CLOCK_BOOTTIME, &ts);
	return double(ts.tv_sec) + double(ts.tv_nsec) / 1000000000.0;
}

void QWindowSubclass::mouseDoubleClickEvent(QMouseEvent * ev)
{
	QQuickWindow::mouseDoubleClickEvent(ev);
	// Ignore
	// TODO: add custom multi-click detection
}

void QWindowSubclass::mouseMoveEvent(QMouseEvent * ev)
{
	QQuickWindow::mouseMoveEvent(ev);
	
	// TODO: detect dragging
	
	mouseEvent(NSMouseMoved, 0, ev);
}

void QWindowSubclass::mousePressEvent(QMouseEvent * ev)
{
	NSEventType type;
	QQuickWindow::mousePressEvent(ev);
	
	switch (ev->button())
	{
		case Qt::LeftButton:
			type = NSLeftMouseDown; break;
		case Qt::RightButton:
			type = NSRightMouseDown; break;
		default:
			type = NSOtherMouseDown; break;
	}
	
	mouseEvent(type, 1, ev);
}

void QWindowSubclass::mouseReleaseEvent(QMouseEvent * ev)
{
	NSEventType type;
	QQuickWindow::mouseReleaseEvent(ev);
	
	switch (ev->button())
	{
		case Qt::LeftButton:
			type = NSLeftMouseUp; break;
		case Qt::RightButton:
			type = NSRightMouseUp; break;
		default:
			type = NSOtherMouseUp; break;
	}
	
	mouseEvent(type, 1, ev);
}

void QWindowSubclass::moveEvent(QMoveEvent * ev)
{
	QQuickWindow::moveEvent(ev);
}

void QWindowSubclass::resizeEvent(QResizeEvent * ev)
{
	QQuickWindow::resizeEvent(ev);
}

void QWindowSubclass::showEvent(QShowEvent * ev)
{
	QQuickWindow::showEvent(ev);
}

void QWindowSubclass::tabletEvent(QTabletEvent * ev)
{
	QQuickWindow::tabletEvent(ev);
}

void QWindowSubclass::touchEvent(QTouchEvent * ev)
{
	QQuickWindow::touchEvent(ev);
}

void QWindowSubclass::wheelEvent(QWheelEvent * ev)
{
	QQuickWindow::wheelEvent(ev);
}

