#ifndef NSQEVENT_H
#define NSQEVENT_H
#include <AppKit/NSEvent.h>
#include <memory>
#include <QWindow>
#include <Foundation/NSDate.h>

@interface NSQEvent : NSEvent
{
	@public
	std::shared_ptr<QEvent> m_event;
	WId m_wid;
	NSTimeInterval m_ts;
}

-(id) initWithQEvent: (std::shared_ptr<QEvent>) event
                 WId: (WId) id;

-(CGEventRef) CGEvent;
- (NSTimeInterval)timestamp;
- (NSEventType)type;
- (NSInteger)windowNumber;
- (NSWindow*) window;
- (NSPoint)locationInWindow;
- (NSInteger)buttonNumber;
- (unsigned short)keyCode;

- (NSString *)characters;
- (NSString *)charactersIgnoringModifiers;
@end

#endif

