#include <AppKit/NSApplication.h>
#include <Foundation/NSRunLoop.h>
#include <Foundation/NSProcessInfo.h>
#include <Foundation/NSBundle.h>
#include <Foundation/NSDictionary.h>
#include <Foundation/NSNotification.h>
#include <QVector>
#include <QQueue>
#include <unistd.h>
#include <mutex>
#include <memory>
#include <QtCore/qabstracteventdispatcher.h>

id NSApp = nullptr;
const double NSAppKitVersionNumber = NSAppKitVersionNumber10_7_2;

DEFINE_CONSTANT(NSApplicationDidBecomeActiveNotification);
DEFINE_CONSTANT(NSApplicationDidHideNotification);
DEFINE_CONSTANT(NSApplicationDidFinishLaunchingNotification);
DEFINE_CONSTANT(NSApplicationDidResignActiveNotification);
DEFINE_CONSTANT(NSApplicationDidUnhideNotification);
DEFINE_CONSTANT(NSApplicationDidUpdateNotification);
DEFINE_CONSTANT(NSApplicationWillBecomeActiveNotification);
DEFINE_CONSTANT(NSApplicationWillHideNotification);
DEFINE_CONSTANT(NSApplicationWillFinishLaunchingNotification);
DEFINE_CONSTANT(NSApplicationWillResignActiveNotification);
DEFINE_CONSTANT(NSApplicationWillUnhideNotification);
DEFINE_CONSTANT(NSApplicationWillUpdateNotification);
DEFINE_CONSTANT(NSApplicationWillTerminateNotification);
DEFINE_CONSTANT(NSApplicationDidChangeScreenParametersNotification);


@implementation NSApplication
+(NSApplication*) sharedApplication
{
	static std::mutex mutex;
	
	if (NSApp != nullptr)
		return NSApp;
	
	std::lock_guard<std::mutex> lock(mutex);
	if (NSApp == nullptr)
		NSApp = [self new];
	return NSApp;
}

-(id) init
{
	self = [super init];
	
	m_delegate = nullptr;
	
	@autoreleasepool
	{
		NSArray* args = [[NSProcessInfo processInfo] arguments];
		QVector<char*> qargs;
		int argc = [args count];
		
		for (NSString* arg in args)
			qargs << (char*) [arg UTF8String];
		
		qargs << nullptr;
		
		m_running = false;
		QCoreApplication::setEventDispatcher(QNSEventDispatcher::instance());
		m_application = new QGuiApplication(argc, qargs.data());
	}
	
	m_pendingEvents = [NSMutableArray new];
	return self;
}

-(void) dealloc
{
	[m_pendingEvents release];
	delete m_application;
	[super dealloc];
}

-(BOOL) isRunning
{
	return m_running;
}

- (void) setDelegate:(id<NSApplicationDelegate>)anObject
{
	NSNotificationCenter* nc = [NSNotificationCenter defaultCenter];
	
	if (m_delegate)
		[nc removeObserver: m_delegate name: nil object: self];
		
	m_delegate = anObject;
	
	if (m_delegate != nullptr)
	{
#define ADD_OBSERVER(event) \
		[nc addObserver: m_delegate \
			   selector: @selector(application##event) \
				   name: NSApplication##event##Notification \
				 object: self]
		
		ADD_OBSERVER(DidBecomeActive);
		ADD_OBSERVER(DidHide);
		ADD_OBSERVER(DidFinishLaunching);
		ADD_OBSERVER(DidResignActive);
		ADD_OBSERVER(DidUnhide);
		ADD_OBSERVER(DidUpdate);
		ADD_OBSERVER(WillBecomeActive);
		ADD_OBSERVER(WillHide);
		ADD_OBSERVER(WillFinishLaunching);
		ADD_OBSERVER(WillResignActive);
		ADD_OBSERVER(WillUnhide);
		ADD_OBSERVER(WillUpdate);
		ADD_OBSERVER(WillTerminate);
		ADD_OBSERVER(DidChangeScreenParameters);
		
#undef ADD_OBSERVER
	}
}

- (id<NSApplicationDelegate>) delegate
{
	return m_delegate;
}

-(void) stop: (id) sender
{
	m_running = false;
}

-(void) run
{
	NSDate* distantFuture = [NSDate distantFuture];
	NSNotificationCenter* nc = [NSNotificationCenter defaultCenter];
	
	[nc postNotificationName: NSApplicationWillFinishLaunchingNotification
	                  object: self];
	                  
	// TODO: open file notifications etc.
	
	[nc postNotificationName: NSApplicationDidFinishLaunchingNotification
	                  object: self];
	
	m_running = true;
	
	while (m_running)
	{
		@autoreleasepool
		{
			@try
			{
				NSEvent* event;
				
				event = [self nextEventMatchingMask: NSAnyEventMask
										  untilDate: distantFuture
											 inMode: NSDefaultRunLoopMode
											dequeue: true];
				
				if (event != nullptr)
					[self sendEvent: event];
			}
			@catch (NSException* e)
			{
				// TODO: handle exceptions
			}
		}
	}
}

- (NSEvent *)nextEventMatchingMask:(NSUInteger)mask untilDate:(NSDate *)expiration inMode:(NSString *)mode dequeue:(BOOL)flag
{
	NSTimeInterval remaining;
	
	do
	{
		for (NSUInteger i = 0; i < [m_pendingEvents count]; i++)
		{
			NSEvent* event = [m_pendingEvents objectAtIndex: i];
			
			if ((1 << [event type]) & mask)
			{
				if (flag)
					[m_pendingEvents removeObjectAtIndex: 0];
			
				return [event autorelease];
			}
		}
		
		remaining = expiration ? [expiration timeIntervalSinceNow] : 0;
		
		CFRunLoopRunInMode((CFStringRef) mode, remaining, true);
		QCoreApplication::sendPostedEvents();
		// QWindowSystemInterface::sendWindowSystemEvents(flags);
	}
	while (remaining > 0);
	
	return NULL;
}

- (void)postEvent:(NSEvent *)anEvent
          atStart:(BOOL)flag;
{
	if (flag)
	{
		[m_pendingEvents insertObject: anEvent
							  atIndex: 0];
	}
	else
	{
		[m_pendingEvents addObject: anEvent];
	}
}

- (void)sendEvent:(NSEvent *)anEvent
{
	// TODO: deliver event
}
@end

BOOL NSApplicationLoad(void)
{
	return true;
}

int NSApplicationMain(int argc, const char *argv[])
{
	@autoreleasepool
	{
		NSDictionary* dict;
		NSString* principialClassName;
		Class principialClass;
		
		[NSProcessInfo initializeWithArguments: (char**) argv
										count: argc
								environment: environ];
		
		dict = [[NSBundle mainBundle] infoDictionary];
		principialClassName = [dict objectForKey: @"NSPrincipialClass"];
		principialClass = NSClassFromString(principialClassName);
		
		[principialClass sharedApplication];
		
		// TODO: load main NIB
	}
	
	@autoreleasepool
	{
		[NSApp run];
		[NSApp release];
	}
	
	return 0;
}


