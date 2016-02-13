#ifndef NSEVENT_H_
#define NSEVENT_H_
#include <Foundation/NSObject.h>
#include <Foundation/NSGeometry.h>
#include <CoreGraphics/CGBase.h>
#include <CoreGraphics/CGEventRef.h>
#include <AppKit/NSTrackingArea.h>

@class NSGraphicsContext;
@class NSWindow;
typedef NSUInteger NSEventPhase;
typedef NSUInteger NSEventType;
typedef NSUInteger NSPointingDeviceType;
typedef NSUInteger NSEventModifierFlags;
typedef unsigned long long NSEventMask;

typedef short NSEventSubtype;

@interface NSEvent : NSObject <NSCopying, NSCoding>
{
#ifdef DARLING_BUILD
	@public
    NSEventType _type;
	NSPoint _location;
	unsigned int _modifierFlags;
	NSTimeInterval _timestamp;
	NSInteger _windowNumber;
	NSWindow *_window;
	NSGraphicsContext* _context;
    union {
        struct {
            int eventNumber;
            int clickCount;
            float pressure;
#if __LP64__
            CGFloat deltaX;
            CGFloat deltaY;
            int subtype;
            short buttonNumber;
            short reserved1;
            int reserved2[3];
#endif
        } mouse;
        struct {
            NSString *keys;
            NSString *unmodKeys;
            unsigned short keyCode;
            BOOL isARepeat;
#if __LP64__
            int eventFlags;
            int reserved[5];
#endif
        } key;
        struct {
            int eventNumber;
            NSInteger trackingNumber;
            void *userData;
#if __LP64__
            int reserved[6];
#endif
        } tracking;
        struct {
            CGFloat deltaX;
            CGFloat deltaY;
            CGFloat deltaZ; 
#if __LP64__
            short subtype;
            short reserved1;
            int reserved2[6];
#endif
        } scrollWheel;
        struct {
            CGFloat deltaX;
            CGFloat deltaY;
            CGFloat deltaZ; 
#if __LP64__
            int reserved[7];
#endif
        } axisGesture;
        struct {
            short subtype;
            BOOL gestureEnded;
            BOOL reserved;
            int value;
            float percentage;
#if __LP64__
            int reserved2[7];
#endif
        } miscGesture;
        struct {
            int subtype;
            NSInteger data1;
            NSInteger data2;
#if __LP64__
            int reserved[6];
#endif
        } misc;
#if __LP64__
        int tabletPointData[14];
        int tabletProximityData[14];
#endif
    } _data;
    void *_eventRef;
#if __LP64__
    void *reserved1;
    void *reserved2;
#endif
#endif // DARLING_BUILD
}

@property (readonly) NSEventType type;
@property (readonly) NSEventModifierFlags modifierFlags;
@property (readonly) NSTimeInterval timestamp;
@property (readonly, assign) NSWindow *window;
@property (readonly) NSInteger windowNumber;
@property (readonly, strong) NSGraphicsContext *context;
@property (readonly) NSInteger clickCount;
@property (readonly) NSInteger buttonNumber;
@property (readonly) NSInteger eventNumber;
@property (readonly) float pressure;
@property (readonly) NSPoint locationInWindow;
@property (readonly) CGFloat deltaX;    
@property (readonly) CGFloat deltaY;    
@property (readonly) CGFloat deltaZ;
@property (readonly) BOOL hasPreciseScrollingDeltas;
@property (readonly) CGFloat scrollingDeltaX;
@property (readonly) CGFloat scrollingDeltaY;
@property (readonly) NSEventPhase momentumPhase;
@property (readonly, copy) NSString *characters;
@property (readonly, copy) NSString *charactersIgnoringModifiers;
@property (getter=isARepeat, readonly) BOOL ARepeat;
@property (readonly) unsigned short keyCode;
@property (readonly) NSInteger trackingNumber;
@property (readonly) void *userData;
@property (readonly, strong) NSTrackingArea *trackingArea;
@property (readonly) NSEventSubtype subtype;
@property (readonly) NSInteger data1;
@property (readonly) NSInteger data2;


+ (NSEvent *)keyEventWithType:(NSEventType)type
                     location:(NSPoint)location
                modifierFlags:(NSEventModifierFlags)flags
                    timestamp:(NSTimeInterval)time
                 windowNumber:(NSInteger)windowNum
                      context:(NSGraphicsContext *)context
                   characters:(NSString *)characters
  charactersIgnoringModifiers:(NSString *)unmodCharacters
                    isARepeat:(BOOL)repeatKey
                      keyCode:(unsigned short)code;

+ (NSEvent *)mouseEventWithType:(NSEventType)type
                       location:(NSPoint)location
                  modifierFlags:(NSEventModifierFlags)flags
                      timestamp:(NSTimeInterval)time
                   windowNumber:(NSInteger)windowNum
                        context:(NSGraphicsContext *)context
                    eventNumber:(NSInteger)eventNumber
                     clickCount:(NSInteger)clickNumber
                       pressure:(float)pressure;

+ (NSEvent *)enterExitEventWithType:(NSEventType)type
                           location:(NSPoint)location
                      modifierFlags:(NSEventModifierFlags)flags
                          timestamp:(NSTimeInterval)time
                       windowNumber:(NSInteger)windowNum
                            context:(NSGraphicsContext *)context
                        eventNumber:(NSInteger)eventNumber
                     trackingNumber:(NSInteger)trackingNumber
                           userData:(void *)userData;

+ (NSEvent *)otherEventWithType:(NSEventType)type
                       location:(NSPoint)location
                  modifierFlags:(NSEventModifierFlags)flags
                      timestamp:(NSTimeInterval)time
                   windowNumber:(NSInteger)windowNum
                        context:(NSGraphicsContext *)context
                        subtype:(short)subtype
                          data1:(NSInteger)data1
                          data2:(NSInteger)data2;

+ (NSEvent *)eventWithCGEvent:(CGEventRef)cgEvent;

- (id)initWithCoder:(NSCoder *)decoder;
- (void)encodeWithCoder:(NSCoder *)encoder;

- (id)copyWithZone:(NSZone *)zone;
@end

enum {
   NSEventPhaseNone        = 0,
   NSEventPhaseBegan       = 0x1 << 0,
   NSEventPhaseStationary  = 0x1 << 1,
   NSEventPhaseChanged     = 0x1 << 2,
   NSEventPhaseEnded       = 0x1 << 3,
   NSEventPhaseCancelled   = 0x1 << 4,
   NSEventPhaseMayBegin    = 0x1 << 5
};

enum {
   NSLeftMouseDown      = 1,
   NSLeftMouseUp        = 2,
   NSRightMouseDown     = 3,
   NSRightMouseUp       = 4,
   NSMouseMoved         = 5,
   NSLeftMouseDragged   = 6,
   NSRightMouseDragged  = 7,
   NSMouseEntered       = 8,
   NSMouseExited        = 9,
   NSKeyDown            = 10,
   NSKeyUp              = 11,
   NSFlagsChanged       = 12,
   NSAppKitDefined      = 13,
   NSSystemDefined      = 14,
   NSApplicationDefined = 15,
   NSPeriodic           = 16,
   NSCursorUpdate       = 17,
   NSScrollWheel        = 22,
   NSTabletPoint        = 23,
   NSTabletProximity    = 24,
   NSOtherMouseDown     = 25,
   NSOtherMouseUp       = 26,
   NSOtherMouseDragged  = 27,
   NSEventTypeGesture   = 29,
   NSEventTypeMagnify   = 30,
   NSEventTypeSwipe     = 31,
   NSEventTypeRotate    = 18,
   NSEventTypeBeginGesture = 19,
   NSEventTypeEndGesture   = 20,
   NSEventTypeSmartMagnify = 32,
   NSEventTypeQuickLook   = 33
};

enum {
   NSLeftMouseDownMask      = 1 << NSLeftMouseDown,
   NSLeftMouseUpMask        = 1 << NSLeftMouseUp,
   NSRightMouseDownMask     = 1 << NSRightMouseDown,
   NSRightMouseUpMask       = 1 << NSRightMouseUp,
   NSMouseMovedMask         = 1 << NSMouseMoved,
   NSLeftMouseDraggedMask   = 1 << NSLeftMouseDragged,
   NSRightMouseDraggedMask  = 1 << NSRightMouseDragged,
   NSMouseEnteredMask       = 1 << NSMouseEntered,
   NSMouseExitedMask        = 1 << NSMouseExited,
   NSKeyDownMask            = 1 << NSKeyDown,
   NSKeyUpMask              = 1 << NSKeyUp,
   NSFlagsChangedMask       = 1 << NSFlagsChanged,
   NSAppKitDefinedMask      = 1 << NSAppKitDefined,
   NSSystemDefinedMask      = 1 << NSSystemDefined,
   NSApplicationDefinedMask = 1 << NSApplicationDefined,
   NSPeriodicMask           = 1 << NSPeriodic,
   NSCursorUpdateMask       = 1 << NSCursorUpdate,
   NSScrollWheelMask        = 1 << NSScrollWheel,
   NSTabletPointMask           = 1 << NSTabletPoint,
   NSTabletProximityMask       = 1 << NSTabletProximity,
   NSOtherMouseDownMask     = 1 << NSOtherMouseDown,
   NSOtherMouseUpMask       = 1 << NSOtherMouseUp,
   NSOtherMouseDraggedMask  = 1 << NSOtherMouseDragged,
   NSEventMaskGesture          = 1 << NSEventTypeGesture,
   NSEventMaskMagnify          = 1 << NSEventTypeMagnify,
   NSEventMaskSwipe            = 1U << NSEventTypeSwipe,
   NSEventMaskRotate           = 1 << NSEventTypeRotate,
   NSEventMaskBeginGesture     = 1 << NSEventTypeBeginGesture,
   NSEventMaskEndGesture       = 1 << NSEventTypeEndGesture,
   NSEventMaskSmartMagnify = 1ULL << NSEventTypeSmartMagnify,
   NSAnyEventMask           = 0xffffffffU
};
inline NSUInteger NSEventMaskFromType(NSEventType type) { return (1 << type); };

enum {
    NSEventGestureAxisNone = 0,
    NSEventGestureAxisHorizontal,
    NSEventGestureAxisVertical
};
typedef NSInteger NSEventGestureAxis;

enum {
	NSWindowExposedEventType            = 0,
    NSApplicationActivatedEventType     = 1,
    NSApplicationDeactivatedEventType   = 2,
    NSWindowMovedEventType              = 4,
    NSScreenChangedEventType            = 8,
    NSAWTEventType                      = 16,
    
    NSPowerOffEventType             = 1,
    
	/*
    NSMouseEventSubtype             = NX_SUBTYPE_DEFAULT,
    NSTabletPointEventSubtype       = NX_SUBTYPE_TABLET_POINT,
    NSTabletProximityEventSubtype   = NX_SUBTYPE_TABLET_PROXIMITY,
    NSTouchEventSubtype = NX_SUBTYPE_MOUSE_TOUCH
	*/
};

enum {
    NSAlphaShiftKeyMask         = 1 << 16,
    NSShiftKeyMask              = 1 << 17,
    NSControlKeyMask            = 1 << 18,
    NSAlternateKeyMask          = 1 << 19,
    NSCommandKeyMask            = 1 << 20,
    NSNumericPadKeyMask         = 1 << 21,
    NSHelpKeyMask               = 1 << 22,
    NSFunctionKeyMask           = 1 << 23,
    NSDeviceIndependentModifierFlagsMask    = 0xffff0000UL
};


#endif
