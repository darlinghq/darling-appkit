#include <AppKit/NSEvent.h>
#include <Foundation/NSException.h>

@implementation NSEvent

@synthesize type = _type;
@synthesize timestamp = _timestamp;
@synthesize window = _window;
@synthesize windowNumber = _windowNumber;
@synthesize context = _context;
@synthesize locationInWindow = _location;

- (NSEventModifierFlags) modifierFlags
{
	return _modifierFlags;
}

- (NSEventSubtype) subtype
{
	return _data.misc.subtype;
}

- (NSInteger) data1
{
	return _data.misc.data1;
}

- (NSInteger) data2
{
	return _data.misc.data2;
}

- (void*) userData
{
	return _data.tracking.userData;
}

- (NSInteger) trackingNumber
{
	return _data.tracking.trackingNumber;
}

- (unsigned short) keyCode
{
	return _data.key.keyCode;
}

- (BOOL) isARepeat
{
	return _data.key.isARepeat;
}

- (NSInteger) clickCount
{
	return _data.mouse.clickCount;
}

- (NSInteger) buttonNumber
{
	return _data.mouse.buttonNumber;
}

- (NSInteger) eventNumber
{
	return _data.mouse.eventNumber;
}

- (float) pressure
{
	return _data.mouse.pressure;
}

- (CGFloat) scrollingDeltaX
{
	return _data.scrollWheel.deltaX;
}

- (CGFloat) scrollingDeltaY
{
	return _data.scrollWheel.deltaY;
}

- (NSString*) charactersIgnoringModifiers
{
	return _data.key.unmodKeys;
}

- (NSString*) characters
{
	return _data.key.keys;
}

- (BOOL)hasPreciseScrollingDeltas
{
	return NO;
}

- (NSEventPhase)momentumPhase
{
	return NSEventPhaseNone;
}

- (NSTrackingArea *)trackingArea
{
	return NULL;
}

- (CGFloat) deltaX
{
	return (self.type == NSScrollWheel) ?
		_data.scrollWheel.deltaX : _data.mouse.deltaX;
}

- (CGFloat) deltaY
{
	return (self.type == NSScrollWheel) ?
		_data.scrollWheel.deltaY : _data.mouse.deltaY;
}

- (CGFloat) deltaZ
{
	return (self.type == NSScrollWheel) ?
		_data.scrollWheel.deltaZ : 0;
}

+ (NSEvent *)eventWithCGEvent:(CGEventRef)cgEvent
{
	return NULL; // TODO
}

+ (NSEvent *)keyEventWithType:(NSEventType)type
                     location:(NSPoint)location
                modifierFlags:(NSEventModifierFlags)flags
                    timestamp:(NSTimeInterval)time
                 windowNumber:(NSInteger)windowNum
                      context:(NSGraphicsContext *)context
                   characters:(NSString *)characters
  charactersIgnoringModifiers:(NSString *)unmodCharacters
                    isARepeat:(BOOL)repeatKey
                      keyCode:(unsigned short)code
{
	NSEvent* event;
	
	if (type != NSKeyDown && type != NSKeyUp)
	{
		[NSException raise: NSInvalidArgumentException
			format: @"Invalid type %d for keyEventWithType:", (int)type];
	}
	
	event = [[NSEvent alloc] init];
	
	event->_type = type;
	event->_location = location;
	event->_modifierFlags = flags;
	event->_timestamp = time;
	event->_windowNumber = windowNum;
	event->_window = NULL; // TODO: lookup
	event->_context = [context retain];
	
	event->_data.key.keys = [characters retain];
	event->_data.key.unmodKeys = [unmodCharacters retain];
	event->_data.key.isARepeat = repeatKey;
	event->_data.key.keyCode = code;
	
	return [event autorelease];
}

+ (NSEvent *)mouseEventWithType:(NSEventType)type
                       location:(NSPoint)location
                  modifierFlags:(NSEventModifierFlags)flags
                      timestamp:(NSTimeInterval)time
                   windowNumber:(NSInteger)windowNum
                        context:(NSGraphicsContext *)context
                    eventNumber:(NSInteger)eventNumber
                     clickCount:(NSInteger)clickNumber
                       pressure:(float)pressure
{
	NSEvent* event;
	
	if ((type >= NSLeftMouseDown && type <= NSRightMouseDragged)
			|| type == NSCursorUpdate || type == NSScrollWheel
			|| (type >= NSOtherMouseDown && type <= NSOtherMouseDragged))
	{
		event = [[NSEvent alloc] init];
		
		event->_type = type;
		event->_location = location;
		event->_modifierFlags = flags;
		event->_timestamp = time;
		event->_windowNumber = windowNum;
		event->_window = NULL; // TODO: lookup
		event->_context = [context retain];
		
		event->_data.mouse.eventNumber = eventNumber;
		event->_data.mouse.clickCount = clickNumber;
		event->_data.mouse.pressure = pressure;
		
		return [event autorelease];
	}
	else
	{
		[NSException raise: NSInvalidArgumentException
			format: @"Invalid type %d for mouseEventWithType:", (int)type];
		
		__builtin_unreachable();
	}
}

+ (NSEvent *)enterExitEventWithType:(NSEventType)type
                           location:(NSPoint)location
                      modifierFlags:(NSEventModifierFlags)flags
                          timestamp:(NSTimeInterval)time
                       windowNumber:(NSInteger)windowNum
                            context:(NSGraphicsContext *)context
                        eventNumber:(NSInteger)eventNumber
                     trackingNumber:(NSInteger)trackingNumber
                           userData:(void *)userData
{
	// TODO
}

+ (NSEvent *)otherEventWithType:(NSEventType)type
                       location:(NSPoint)location
                  modifierFlags:(NSEventModifierFlags)flags
                      timestamp:(NSTimeInterval)time
                   windowNumber:(NSInteger)windowNum
                        context:(NSGraphicsContext *)context
                        subtype:(short)subtype
                          data1:(NSInteger)data1
                          data2:(NSInteger)data2
{
	// TODO
}

- (id)initWithCoder:(NSCoder *)decoder
{
	return [self init]; // TODO
}

- (void)encodeWithCoder:(NSCoder *)encoder
{
	// TODO
}

- (id)copyWithZone:(NSZone *)zone
{
	return [self retain]; // TODO
}

- (void) dealloc
{
	if (_type == NSKeyDown || _type == NSKeyUp)
	{
		[_data.key.keys release];
		[_data.key.unmodKeys release];
	}
	
	[_context release];
	[super dealloc];
}

@end
