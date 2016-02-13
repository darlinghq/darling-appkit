#ifndef QWINDOWSUBCLASS_H
#define QWINDOWSUBCLASS_H
#include <QQuickWindow>
#include <AppKit/NSEvent.h>

@class NSWindow;

class QWindowSubclass : public QQuickWindow
{
public:
	QWindowSubclass(NSWindow* win);
protected:
	virtual void exposeEvent(QExposeEvent * ev) override;
	virtual void focusInEvent(QFocusEvent * ev) override;
	virtual void focusOutEvent(QFocusEvent * ev) override;
	virtual void hideEvent(QHideEvent * ev) override;
	virtual void keyPressEvent(QKeyEvent * ev) override;
	virtual void keyReleaseEvent(QKeyEvent * ev) override;
	virtual void mouseDoubleClickEvent(QMouseEvent * ev) override;
	virtual void mouseMoveEvent(QMouseEvent * ev) override;
	virtual void mousePressEvent(QMouseEvent * ev) override;
	virtual void mouseReleaseEvent(QMouseEvent * ev) override;
	virtual void moveEvent(QMoveEvent * ev) override;
	virtual void resizeEvent(QResizeEvent * ev) override;
	virtual void showEvent(QShowEvent * ev) override;
	virtual void tabletEvent(QTabletEvent * ev) override;
	virtual void touchEvent(QTouchEvent * ev) override;
	virtual void wheelEvent(QWheelEvent * ev) override;
private:
	void keyEvent(NSEventType type, QKeyEvent* ev);
	void mouseEvent(NSEventType type, int clickCount, QMouseEvent* ev);
	
	static NSEventModifierFlags convertModifiers(Qt::KeyboardModifiers mod);
	static NSTimeInterval timestamp();
private:
	NSWindow* m_window;
};

#endif /* QWINDOWSUBCLASS_H */

