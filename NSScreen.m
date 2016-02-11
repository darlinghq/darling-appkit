#import <AppKit/NSScreen.h>
#import <Foundation/NSArray.h>
#include <QGuiApplication>
#include <CoreGraphics/CGGeometry.h>

@implementation NSScreen

+ (NSArray *)screens
{
	NSArray* rv = [NSMutableArray new];

	for (QScreen *screen : QGuiApplication::screens())
	{
		NSScreen* scr = [NSScreen new];
		scr->_screen = screen;
		[rv addObject: scr];
		[scr release];
	}

	return [rv autorelease];
}

+ (NSScreen *)mainScreen
{
	NSScreen* scr = [NSScreen new];
	scr->_screen = QGuiApplication::primaryScreen();
	return [scr autorelease];
}

+ (NSScreen *)deepestScreen
{
	return [NSScreen mainScreen];
}

- (NSWindowDepth)depth
{
	return _screen->depth();
}

- (NSRect)frame
{
	return CGRectFromQRect(_screen->geometry());
}

- (NSRect)visibleFrame
{
	return [self frame];
}

- (NSDictionary *)deviceDescription
{
	QByteArray data = _screen->name().toUtf8();
	NSString* str = [NSString stringWithUTF8String: data.data()];
	return @{ @"Name": str };
}

- (NSColorSpace *)colorSpace;

- (const NSWindowDepth *)supportedWindowDepths ;



- (NSRect)convertRectToBacking:(NSRect)aRect;
- (NSRect)convertRectFromBacking:(NSRect)aRect;



- (NSRect)backingAlignedRect:(NSRect)aRect options:(NSAlignmentOptions)options;



- (CGFloat)backingScaleFactor;
@end


