/*****************************************************************************
* Copyright (c) Future Technology Devices International 2014
* propriety of Future Technology devices International.
*
* Software License Agreement
*
* This code is provided as an example only and is not guaranteed by FTDI. 
* FTDI accept no responsibility for any issues resulting from its use. 
* The developer of the final application incorporating any parts of this 
* sample project is responsible for ensuring its safe and correct operation 
* and for any consequences resulting from its use.
*****************************************************************************/
/**
* @file                           FTGesture
* @brief                          Gesture Library for MultiTouch
* @version                        0.1
* @date                           2014/9/15
* 
*/
#include "math.h"

#define FTGL_MAXTOUCH_PTS					(5)
#define FTGL_NOTOUCH						(-32768L)

#define FTGL_MINTOUCHPENDOWNCOUNT			(5)
#define FTGL_MAXTOUCHPENDOWNCOUNT			(20)
#define FTGL_MAXTOUCHPENUPCOUNT				(5)
#define FTGL_MAXLONGDURATIONCOUNT			(50)

#define FTGL_MAXPXLSDIRECTION_RANGE			(10)
#define FTGL_SWIPE_MIN_PXLS					(40)
#define FTGL_SWIPE_MAX_PXLS					(250)
#define FTGL_FLICK_CTS						(30)

#define FTGL_MAXTAPS						(3)

#define FTGL_SCROLLER_RATE					(5)
#define FTGL_SUBPIXEL_PRESCISON				(4)

#define RAD2DEGREE(rad)						(rad)*(180/3.14159)
/********************************************************************************************/

#define max(a,b) (a>b?a:b)
#define min(a,b) (a<b?a:b)

#if ! defined(FT9XX_PLATFORM) && ! defined(RP2040_PLATFORM)
typedef ft_uchar8_t ft_uint8_t;
typedef short  ft_int16_t;
typedef unsigned short ft_uint16_t;
typedef unsigned int ft_uint32_t;
typedef int ft_int32_t;
typedef long long ft_int64_t;
typedef unsigned long long ft_uint64_t;

typedef signed char int8_t;
typedef short int16_t;
typedef int int32_t;
typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
#endif

typedef enum FTBOOL
{
	FTGL_FALSE = 0,
	FTGL_TRUE = 1,
}FTBOOL;
typedef enum FTENABLE
{
	FTGL_DISABLE = 0,
	FTGL_ENABLE = 1,
}FTENABLE;
typedef enum FTVALID
{
	FTGL_INVALID = 0,
	FTGL_VALID = 1,
}FTVALID;

/*************************************STATUS ENUM********************************************/
typedef enum FTGLSTATUS
{
	FTGL_OK = 0x00,
	FTGL_ERR = 0x01,
	FTGL_WARNG = 0x02,
	FTGL_FATAL = 0x04,	
	FTGL_SETPARAM_ERROR = 0x08,
	FTGL_MODE_ERROR = 0x10,	
	FTGL_TOUCHPOINTCHANGE = 0x20,	
	FTGL_TAPCHANGE = 0x40,
	FTGL_SWIPECHANGE = 0x80,	
	FTGL_FLICKCHANGE = 0x100,
	FTGL_SCROLLERCHANGE = 0x200,
	FTGL_TRANSFORMCHANGE = 0x400,	
}FTGLSTATUS;
/*************************************TPOINTS ENUM********************************************/
typedef enum FTGLENUM_TPOINTS
{
	FTGL_TPOINTS_NONE 		= 0,	
	FTGL_TPOINTS_ONE 		= 1,
	FTGL_TPOINTS_TWO 		= 2,
	FTGL_TPOINTS_THREE 		= 3,
	FTGL_TPOINTS_FOUR 		= 4,
	FTGL_TPOINTS_FIVE 		= 5,
}FTGLENUM_TPOINTS;
/*************************************SWIPE ENUM**********************************************/
typedef enum FTGLENUM_TSWIPE
{
	FTGL_TSWIPE_NONE = 0,	
	FTGL_TSWIPE_SWIPEUP = 1,
	FTGL_TSWIPE_SWIPEDOWN = 2,
	FTGL_TSWIPE_SWIPELEFT = 3,
	FTGL_TSWIPE_SWIPERIGHT = 4,	
	FTGL_TSWIPE_SWIPEIGNORE = 5,
}FTGLENUM_TSWIPE;

/*************************************FLICK ENUM**********************************************/
typedef enum FTGLENUM_TFLICK
{
	FTGL_TFLICK_NONE = 0,	
	FTGL_TFLICK_FLICKUP = 1,
	FTGL_TFLICK_FLICKDOWN = 2,
	FTGL_TFLICK_FLICKLEFT = 3,
	FTGL_TFLICK_FLICKRIGHT = 4,	
	FTGL_TFLICK_FLICKIGNORE = 5,
}FTGLENUM_TFLICK;

/*************************************TAP ENUM************************************************/
typedef enum FTGLENUM_TTAP
{	
	FTGL_TTAP_NONE = 0,		
	FTGL_TTAP_ONETAP = 1,	
	FTGL_TTAP_TWOTAP = 2,	
	FTGL_TTAP_THREETAP = 3,	
	FTGL_TTAP_LONGTAP = 4,	
	FTGL_TTAP_TAPIGNORE = 6,	
}FTGLENUM_TTAP;
/*************************************SCROLLER MODE ENUM**************************************/
typedef enum FTGLSCROLLERMODE
{
	FTGL_SCROLLER_NONE = 0x00,
	FTGL_SCROLLER_HORIZONTAL = 0x01,
	FTGL_SCROLLER_VERTICAL = 0x02,		 	
}FTGLSCROLLERMODE;
/*************************************TAP CONTEXT*********************************************/
typedef struct  
{
	FTGLENUM_TTAP tap;
	FTGLENUM_TPOINTS numberofTouches;
}FTGLtapType;
/************************************SWIPE CONTEXT********************************************/
typedef struct  
{
	FTGLENUM_TSWIPE swipe;
	FTGLENUM_TPOINTS numberofTouches;
}FTGLswipeType;

/************************************FLICK CONTEXT********************************************/
typedef struct  
{
	FTGLENUM_TFLICK flick;
	FTGLENUM_TPOINTS numberofTouches;
}FTGLflickType;

/***********************************TRANSFORM CONTEXT*****************************************/
typedef struct  
{
	int16_t angleofRotation;
	int16_t scaleX,maxScaleX,minScaleX;
	int16_t scaleY,maxScaleY,minScaleY;
	int16_t dragX,maxDragX,minDragX;
	int16_t dragY,maxDragY,minDragY;
}FTGLtransform;
/***********************************SCROLLER CONTEXT*****************************************/
typedef struct
{
	int32_t baseX,minLimitX,maxLimitX;				 
	int32_t baseY,minLimitY,maxLimitY;
	int16_t velocityX,velocityY;
	int8_t mode;
}FTGLscroll;
/************************************FTGL CONTEXT*********************************************/
typedef struct 
{
	int16_t	startX[FTGL_MAXTOUCH_PTS];			// Starting X Cordinate 
	int16_t	startY[FTGL_MAXTOUCH_PTS];			// Starting Y Cordinate 
	int16_t	currX[FTGL_MAXTOUCH_PTS];			// Current X Cordinate 
	int16_t	currY[FTGL_MAXTOUCH_PTS];			// Current Y Cordinate 
	int16_t	prevX[FTGL_MAXTOUCH_PTS];			// Previous X Cordinate 
	int16_t	prevY[FTGL_MAXTOUCH_PTS];			// Previous Y Cordinate 
	uint8_t currNumTchs;					// Current Number of Touches
	uint8_t prevNumTchs;					// Previous Number of Touches
	uint8_t startNumTchs;					// Starting Number of Touches
	FTGLtapType tapType;						// Tap Parameters
	FTGLswipeType swipeType;					// Swipe Parameters		
	FTGLflickType flickType;					// Swipe Parameters		
	FTGLtransform transform;					// Transform Parameters
	FTGLscroll scroller;							// Scroller Parameters
	int16_t centerX;								// Avg.Center X 
	int16_t centerY;								// Avg.Center Y 
	int16_t penUpcts,penDowncts;			// For Internal Purpose Only 
	int8_t tap_tchs,tap_cts;
	int16_t tap_cx,tap_cy;	
	int8_t swipe_tchs;
	int16_t swipe_cx,swipe_cy;	
	int16_t scrollerprev_centery;
	int16_t scrollerprev_centerx;
	int16_t prev_angle,prev_scale;
	int8_t transform_function;
}FTGLContext;

FTGLContext FTGesture;
/*******************************GESTURE PRIVATE APIS*****************************************/	
void updatePrevstate();
void updateStartstate();
void updateCurrstate(int16_t x[],int16_t y[]);
FTGLENUM_TTAP detectTap();	
FTGLENUM_TSWIPE detectSwipe();	
/***********************************UTILITY APIS*********************************************/
int16_t avgCenter(uint8_t tchs,int16_t d[]);
FTVALID Isvalidtouch(int16_t data2cmp); 
int16_t distbwPoints( int16_t x1,int16_t y1,int16_t x2,int16_t y2);
int16_t avgAngle(uint8_t tchs,int16_t cx,int16_t cy,int16_t currx[],int16_t curry[],\
				int16_t prevx[],int16_t prevy[]);
int16_t smallestAngleDiff(int16_t a, int16_t b);
int16_t avgScale(uint8_t tchs,int16_t ctr,int16_t d1[],int16_t d2[]);
int16_t anglebwPoints(int16_t cx,int16_t cy,int16_t x,int16_t y);	
int16_t avgDistance(uint8_t tchs,int16_t curr[],int16_t prev[]);
/************************************RUN API*************************************************/
uint32_t FTGLRun(int16_t x[],int16_t y[]);	
FTGLSTATUS FTGLInit();
FTGLSTATUS FTGLExit();
int16_t FTGLScrollerRun(uint8_t tchs,int16_t cx,int16_t cy);
/************************************SET APIS************************************************/
FTGLSTATUS FTGLSetScaleRange(int16_t minX,int16_t maxX,int16_t minY,int16_t maxY);
FTGLSTATUS FTGLSetScale(int16_t sx,int16_t sy);
FTGLSTATUS FTGLSetDragRange(int16_t minX,int16_t maxX,int16_t minY,int16_t maxY);
FTGLSTATUS FTGLSetDrag(int16_t dx,int16_t dy);
FTGLSTATUS FTGLSetAngle(int16_t angle);
FTGLSTATUS FTGLSetScrollerRange(int32_t minX,int32_t maxX,int32_t minY,int32_t maxY);
FTGLSTATUS FTGLSetScrollermode(uint8_t hor,uint8_t ver);
FTGLSTATUS FTGLSetScrollerXY(int32_t scx,int32_t scy);
FTGLSTATUS FTGLSetTransform(int16_t angle,int16_t sx,int16_t sy,int16_t dx,int16_t dy);	
/************************************GET APIS************************************************/
FTBOOL FTGLIsTouch();
FTGLSTATUS FTGLGetCenter(int16_t *x,int16_t *y);
FTGLSTATUS FTGLGetAngle(int16_t *angle);
FTGLSTATUS FTGLGetScale(int16_t *x,int16_t *y);
FTGLSTATUS FTGLGetTransform(int16_t *angle,int16_t *sx,int16_t *sy,int16_t *dx,int16_t *dy);
FTGLSTATUS FTGLGetDrag(int16_t *x,int16_t *y);
FTGLSTATUS FTGLGetTaptype(uint8_t *tap,uint8_t *nooftchs);
FTGLSTATUS FTGLGetSwipetype(uint8_t *swipe,uint8_t *nooftchs);	
FTGLSTATUS FTGLGetNoofTchs(uint8_t *nooftchs);
FTGLSTATUS FTGLGetScroller(int32_t *scx,int32_t *scy);
FTGLSTATUS FTGLGetScrollerX(int32_t *scx);
FTGLSTATUS FTGLGetScrollerY(int32_t *scy);
FTGLSTATUS FTGLGetCoordinates(int16_t *startx,int16_t *starty,int16_t *currx,int16_t *curry,\
							  int16_t *prevx,int16_t *prevy);
FTGLSTATUS FTGLGetFlicktype(uint8_t *flick,uint8_t *nooftchs);
FTGLSTATUS FTGLGetTapCoordinates(int16_t *startx,int16_t *starty);
/********************************************************************************************
**Usage			: Api to Check the Touch is Valid or Invalid
**Return		: VALID or INVALID
*********************************************************************************************/
FTVALID Isvalidtouch(int16_t data2cmp) 
{
	if(data2cmp!=FTGL_NOTOUCH) return FTGL_VALID;
	return FTGL_INVALID;
}
/**********************************************************************************************
**Usage			: Api to get the distance Average centre
**tchs			: number of touches
**d[]			: array of values
**Return		: Average centre				
*********************************************************************************************/
int16_t avgCenter(uint8_t tchs,int16_t d[])
{
	int16_t avg = 0,i;
	for(i = 0;i<tchs;i++)
	{
		if(Isvalidtouch(d[i]))
		avg += d[i];
	}	
	return avg/tchs;		
}
/*********************************************************************************************
**Usage			: Api to get the distance between two points
**x1/y1			: Ist Coordinate (X,Y)
**x2/y2			: 2nd Coordinate (X,Y)
**Return		: Distance				
*********************************************************************************************/
int16_t distbwPoints( int16_t x1,int16_t y1,int16_t x2,int16_t y2)
{
	int16_t dx,dy;	
	dx = x1-x2;
	dy = y1-y2;	
	return sqrt(pow((double)dx,2)+pow((double)dy,2));
}
/*********************************************************************************************
**Usage			: Api to get the Average Angle of touches 
**tchs			: number of touches
**ctrx/ctry		: Center X and Y of all Co-ordinates
**currx/prevx	: Current and Previous X Coordinates
**curry/prevy	: Current and Previous Y Coordinates
**Return		: Average angle				
*********************************************************************************************/
int16_t avgAngle(uint8_t tchs,int16_t cx,int16_t cy,int16_t currx[],\
				int16_t curry[],int16_t prevx[],int16_t prevy[])
{
	int16_t i,a,b,angle = 0;
	for(i=0;i<tchs;i++)
	{	
		if(Isvalidtouch(currx[i]) && Isvalidtouch(prevx[i]))
		{						
			a = anglebwPoints(cx,cy,currx[i],curry[i]); 				
			b = anglebwPoints(cx,cy,prevx[i],prevy[i]);
			angle += smallestAngleDiff(a,b);	
		}	
	}	
	return angle/=tchs;
}
/*********************************************************************************************
**Usage			: Api to get the smallest angle difference
*********************************************************************************************/
int16_t smallestAngleDiff(int16_t a, int16_t b)
{	
	int16_t d = a-b;
	if(d>180)
	d = d-360;
	else if(d<-180)
	d = d+360;
	return d;
}
/********************************************************************************************
**Usage			: Api to get the Angle between two points
**cx/cy			: Center x and Center y
**x/y			: X and Y Coordinate
**Return		: angle			
*********************************************************************************************/
int16_t anglebwPoints(int16_t cx,int16_t cy,int16_t x,int16_t y)
{
	int16_t dx = x-cx;
	int16_t dy = y-cy;	
	return RAD2DEGREE(atan2((double)dx,(double)dy));
}
/********************************************************************************************
**Usage			: Api to get the Average Scale of touches 
**tchs			: number of touches
**ctr			: Center of all Co-ordinates
**curr/prev		: Current and Previous Coordinates
**Return		: Average Scale				
*********************************************************************************************/
int16_t avgScale(uint8_t tchs,int16_t ctr,int16_t curr[],int16_t prev[])
{
	int16_t distance = 0,i;	
	for(i=0;i<tchs;i++)
	{	
		if(Isvalidtouch(curr[i]) && Isvalidtouch(prev[i]))
		{						
			int16_t a = curr[i]-ctr;
			int16_t b = prev[i]-ctr;
			if(b<0) b = -b;
			if(a<0) a = -a;				
			distance += (a-b);			
		}else
		{
			return 0;
		}	
	}	
	return distance/=tchs;
}
/********************************************************************************************
**Usage			: Api to get the Average distance of touches 
**tchs			: number of touches
**curr/prev		: Current and Previous Coordinates
**Return		: Average Distance				
*********************************************************************************************/
int16_t avgDistance(uint8_t tchs,int16_t curr[],int16_t prev[])
{
	int16_t a,b,i,distance = 0;	
	for(i=0;i<tchs;i++)
	{	
		if(Isvalidtouch(prev[i]) && Isvalidtouch(curr[i]))
		distance += (prev[i]-curr[i]);					
	}	
	return distance/=tchs;
}
/********************************************************************************************
**Usage			: Calculate the velocity of X and Y direction based on the mode setting
*********************************************************************************************/
int16_t FTGLScrollerRun(uint8_t tchs,int16_t centerx,int16_t centery)
{
	int16_t change;
	if(tchs)
	{		
		if(Isvalidtouch(centerx))
		{		
			if(FTGesture.scroller.mode&FTGL_SCROLLER_HORIZONTAL)
			{	
				if(FTGesture.scrollerprev_centerx!=0)
				FTGesture.scroller.velocityX = (FTGesture.scrollerprev_centerx - centerx) << FTGL_SUBPIXEL_PRESCISON;	
			}	
		} 
		if(Isvalidtouch(centery))
		{
			if(FTGesture.scroller.mode&FTGL_SCROLLER_VERTICAL)
			{
				if(FTGesture.scrollerprev_centery!=0)
				FTGesture.scroller.velocityY = (FTGesture.scrollerprev_centery - centery) << FTGL_SUBPIXEL_PRESCISON;	
			}
		}
		if(Isvalidtouch(centerx))
		FTGesture.scrollerprev_centerx = centerx;
		if(Isvalidtouch(centery))
		FTGesture.scrollerprev_centery = centery;
	}
	else
	{
		FTGesture.scrollerprev_centerx = 0;
		FTGesture.scrollerprev_centery = 0;		
		if(FTGesture.scroller.mode&FTGL_SCROLLER_HORIZONTAL)
		{
			change = max(1, abs(FTGesture.scroller.velocityX) >> FTGL_SCROLLER_RATE);						
			if(FTGesture.scroller.velocityX<0)
			FTGesture.scroller.velocityX+=change;
			if(FTGesture.scroller.velocityX>0)
			FTGesture.scroller.velocityX-=change;		
		}
		if(FTGesture.scroller.mode&FTGL_SCROLLER_VERTICAL)
		{	
			change = max(1, abs(FTGesture.scroller.velocityY) >> FTGL_SCROLLER_RATE);
			if(FTGesture.scroller.velocityY<0)
			FTGesture.scroller.velocityY+=change;
			if(FTGesture.scroller.velocityY>0)
			FTGesture.scroller.velocityY-=change;
		}
	}
	return 0;
}
/********************************************************************************************
**Usage			: Update the Previous State of the gesture
*********************************************************************************************/
void updatePrevstate()
{
	int16_t i;
	FTGesture.prevNumTchs = FTGesture.currNumTchs;
	for(i=0;i<FTGL_MAXTOUCH_PTS;i++)
	{
		FTGesture.prevX[i] = FTGesture.currX[i];
		FTGesture.prevY[i] = FTGesture.currY[i];
	}	
}
/********************************************************************************************
**Usage			: Update the Current State of the gesture
**x[]/y[]		: X and Y Co-ordinates of Touches
*********************************************************************************************/
void updateCurrstate(int16_t x[],int16_t y[])
{	
	int16_t i;
	FTGesture.currNumTchs = 0;
	for(i=0;i<FTGL_MAXTOUCH_PTS;i++)
	{
		FTGesture.currX[i] = x[i];	
		FTGesture.currY[i] = y[i];
		if(Isvalidtouch(FTGesture.currX[i]))FTGesture.currNumTchs++;
	}
}
/********************************************************************************************
**Usage			: Update the Starting State of the gesture
*********************************************************************************************/
void updateStartstate()
{
	int16_t i;
	FTGesture.startNumTchs = FTGesture.currNumTchs;
	for(i=0;i<FTGL_MAXTOUCH_PTS;i++)
	{
		FTGesture.startX[i] = FTGesture.currX[i];
		FTGesture.startY[i] = FTGesture.currY[i];
	}	
}
/********************************************************************************************
**Usage			: API to Detect Tap
**Return 		: Tap type
*********************************************************************************************/
ft_uint16_t ax,ay;
FTGLENUM_TTAP detectTap()
{
	int16_t distx;
	if(FTGesture.currNumTchs)
	{
		ax = FTGesture.centerX;
		ay = FTGesture.centerY;
		if(FTGesture.penDowncts < FTGL_MINTOUCHPENDOWNCOUNT/2)								// Ignore if number of touch change			
		{
			FTGesture.tap_tchs = FTGesture.currNumTchs;
			FTGesture.tap_cx = FTGesture.centerX;
			FTGesture.tap_cy = FTGesture.centerY;		
			return FTGL_TTAP_NONE;
		}	
		distx = distbwPoints(FTGesture.tap_cx,FTGesture.tap_cy,FTGesture.centerX,FTGesture.centerY);
		if(FTGesture.penDowncts > FTGL_MAXTOUCHPENDOWNCOUNT &&\
			FTGesture.penDowncts < FTGL_MAXLONGDURATIONCOUNT)
		{			
			FTGesture.tap_cts = -1;			
			if(distx < FTGL_MAXPXLSDIRECTION_RANGE)	
			{
				FTGesture.tapType.numberofTouches = (FTGLENUM_TPOINTS)FTGesture.tap_tchs;	
				return FTGL_TTAP_LONGTAP;				
			}		
		}	
	}else if(FTGesture.tap_tchs!=0)
	{
		if(FTGesture.penDowncts > 0 && FTGesture.penDowncts <= FTGL_MAXTOUCHPENDOWNCOUNT)
		{
			distx = 0;//distbwPoints(FTGesture.tap_cx,FTGesture.tap_cy,FTGesture.centerX,FTGesture.centerY);			
			if(distx < FTGL_MAXPXLSDIRECTION_RANGE)
			FTGesture.tap_cts += 1;
			else
			{
				FTGesture.tap_tchs = 0; 
				FTGesture.tap_cts = 0;
				return FTGL_TTAP_TAPIGNORE;
			}
		}else if(FTGesture.penUpcts>FTGL_MAXTOUCHPENUPCOUNT)
		{	
			if(FTGesture.tap_cts>0 && FTGesture.tap_cts <= FTGL_MAXTAPS)
			{			
				FTGLENUM_TTAP temp_cts = (FTGLENUM_TTAP)FTGesture.tap_cts;					
				FTGesture.tapType.numberofTouches = (FTGLENUM_TPOINTS)FTGesture.tap_tchs;				
				FTGesture.tap_tchs = 0;
				FTGesture.tap_cts = 0;
				return temp_cts;
			}
			FTGesture.tap_cts = 0;
		}
	}
	return FTGL_TTAP_NONE;
}
/********************************************************************************************
**Usage			: API to Detect Swipe
**Return 		: Swipe type
*********************************************************************************************/
FTGLENUM_TSWIPE detectSwipe()
{	
	int16_t dx,dy,sx,sy;	
	int16_t cx1,cy1;		
	if(FTGesture.currNumTchs)
	{
		if(!FTGesture.swipe_tchs || FTGesture.penDowncts < 2)
		{
			FTGesture.swipe_tchs = FTGesture.startNumTchs;
			FTGesture.swipe_cx = FTGesture.centerX;
			FTGesture.swipe_cy = FTGesture.centerY;

		}	
	}else if(FTGesture.swipe_tchs)
	{
		FTGesture.swipeType.numberofTouches = (FTGLENUM_TPOINTS)FTGesture.swipe_tchs;
		FTGesture.swipe_tchs = 0; 
		if(FTGesture.penDowncts > FTGL_MAXTOUCHPENDOWNCOUNT)
		return FTGL_TSWIPE_SWIPEIGNORE;		
		if(FTGesture.penDowncts < 2)
		return FTGL_TSWIPE_SWIPEIGNORE;		
		cx1 = FTGesture.centerX;
		cy1 = FTGesture.centerY;		
		dx = sx = FTGesture.swipe_cx-cx1;
		dy = sy = FTGesture.swipe_cy-cy1;	
		FTGesture.swipe_cy = 0;
		FTGesture.swipe_cx = 0;
		if(sx < 0) dx = -sx;  
		if(sy < 0) dy = -sy;  		
		if(dx > FTGL_SWIPE_MIN_PXLS && dx < FTGL_SWIPE_MAX_PXLS)
		{
			if(sx>0)
			return FTGL_TSWIPE_SWIPELEFT;
			else if(sx<0)
			return FTGL_TSWIPE_SWIPERIGHT;
		}else if(dy > FTGL_SWIPE_MIN_PXLS && dy < FTGL_SWIPE_MAX_PXLS)
		{
			if(sy>0)
			return FTGL_TSWIPE_SWIPEUP;
			else if(sy<0)
			return FTGL_TSWIPE_SWIPEDOWN;									
		}
	}	
	return FTGL_TSWIPE_NONE;	
}
/********************************************************************************************
**Usage			: API to Run the Gesture
**x[]/y[]		: X and Y Co-ordinates of Touches
**Return 		: Status of the Gesture
*********************************************************************************************/
uint32_t FTGLRun(int16_t x[],int16_t y[])
{		
	int16_t i;	
	uint32_t gstatus = 0;
	int8_t transform = 0;
	FTGLENUM_TTAP tap=FTGL_TTAP_NONE;
	FTGLENUM_TSWIPE swipe=FTGL_TSWIPE_NONE;
	updatePrevstate();
	updateCurrstate(x,y);
	if(FTGesture.prevNumTchs==0 && FTGesture.currNumTchs!=0)	
	updateStartstate();

	if(FTGesture.currNumTchs)
	{
		if(FTGesture.currNumTchs==FTGesture.startNumTchs)
		{
			for(i=0;i<FTGesture.currNumTchs;i++)
			{
				if(Isvalidtouch(FTGesture.currX[i]) && Isvalidtouch(FTGesture.startX[i]))
				{
					if((FTGesture.currX[i]-FTGesture.startX[i])>0)
					transform+=1;
					else if((FTGesture.currX[i]-FTGesture.startX[i])<0)
					transform-=1;
				}
			}		
		}else
		{			
			updateStartstate();
			transform = -1;
		}
	}
	
	if(FTGesture.currNumTchs)
	{
		if(1)
		{
			i = avgCenter(FTGesture.currNumTchs,FTGesture.currX);	
			if(i)FTGesture.centerX = i; 
			i = avgCenter(FTGesture.currNumTchs,FTGesture.currY);	
			if(i)FTGesture.centerY = i; 		
			FTGesture.transform.dragX += avgDistance(FTGesture.currNumTchs,FTGesture.currX,FTGesture.prevX);
			FTGesture.transform.dragX = max(FTGesture.transform.minDragX, min(FTGesture.transform.dragX, FTGesture.transform.maxDragX));
			FTGesture.transform.dragY += avgDistance(FTGesture.currNumTchs,FTGesture.currY,FTGesture.prevY);
			FTGesture.transform.dragY = max(FTGesture.transform.minDragY, min(FTGesture.transform.dragY, FTGesture.transform.maxDragY));	
			if(FTGesture.currNumTchs>1 && transform==0)
			{
				FTGesture.transform.angleofRotation +=  avgAngle(FTGesture.currNumTchs,FTGesture.centerX,FTGesture.centerY,\
																 FTGesture.currX,FTGesture.currY,\
																 FTGesture.prevX,FTGesture.prevY);
				if(FTGesture.transform.angleofRotation > 360)
				FTGesture.transform.angleofRotation-=360;
				if(FTGesture.transform.angleofRotation < -360)
				FTGesture.transform.angleofRotation+=360;
				if((FTGesture.transform.angleofRotation+10)!=FTGesture.prev_angle)
				{
					i = avgScale(FTGesture.currNumTchs,FTGesture.centerX,FTGesture.currX,FTGesture.prevX);
					FTGesture.transform.scaleX+= i;			
					i = avgScale(FTGesture.currNumTchs,FTGesture.centerY,FTGesture.currY,FTGesture.prevY);
					FTGesture.transform.scaleY+= i;
					if(FTGesture.transform.scaleX>FTGesture.transform.maxScaleX) FTGesture.transform.scaleX = FTGesture.transform.maxScaleX;
					if(FTGesture.transform.scaleY>FTGesture.transform.maxScaleY) FTGesture.transform.scaleY = FTGesture.transform.maxScaleY;
					if(FTGesture.transform.scaleX<FTGesture.transform.minScaleX) FTGesture.transform.scaleX = FTGesture.transform.minScaleX;
					if(FTGesture.transform.scaleY<FTGesture.transform.minScaleY) FTGesture.transform.scaleY = FTGesture.transform.minScaleY;	
				}
				if((FTGesture.transform.angleofRotation)!=FTGesture.prev_angle || FTGesture.prev_scale !=(FTGesture.transform.scaleY+FTGesture.transform.scaleX))
				{		
					FTGesture.swipe_tchs=0;
					FTGesture.tapType.tap = FTGL_TTAP_NONE;
					FTGesture.penUpcts=0;
					FTGesture.penDowncts=-32768L;		
					FTGesture.prev_angle = FTGesture.transform.angleofRotation;
					FTGesture.prev_scale = (FTGesture.transform.scaleY+FTGesture.transform.scaleX);
					gstatus |= FTGL_TRANSFORMCHANGE; 	
					return gstatus;
				}
			}
		}
	}

	tap = detectTap();	
	if(tap)
	FTGesture.tapType.tap = tap;		
	swipe = detectSwipe();	
	if(swipe)
	{
		FTGesture.swipeType.swipe = swipe;				
		FTGesture.transform_function = 0;
	}
		FTGLScrollerRun(FTGesture.currNumTchs,FTGesture.currX[0],FTGesture.currY[0]);
	if(FTGesture.scroller.mode&FTGL_SCROLLER_HORIZONTAL)
	{
		FTGesture.scroller.baseX += FTGesture.scroller.velocityX;	
		FTGesture.scroller.baseX = max(FTGesture.scroller.minLimitX, min(FTGesture.scroller.baseX, FTGesture.scroller.maxLimitX));
	}
	if(FTGesture.scroller.mode&FTGL_SCROLLER_VERTICAL)	
	{
		FTGesture.scroller.baseY += FTGesture.scroller.velocityY;	
		FTGesture.scroller.baseY = max(FTGesture.scroller.minLimitY, min(FTGesture.scroller.baseY, FTGesture.scroller.maxLimitY));
	}	
	if(FTGesture.currNumTchs==0)
	{
		if(FTGesture.tapType.tap!=FTGL_TTAP_NONE)
		gstatus = FTGL_TAPCHANGE;		
		if(FTGesture.swipeType.swipe!=FTGL_TSWIPE_NONE)
		{				
			if(FTGesture.penDowncts < FTGL_FLICK_CTS && FTGesture.penDowncts>0)
			{
				if(FTGesture.swipeType.swipe==FTGL_TFLICK_FLICKIGNORE)				
				FTGesture.flickType.flick = FTGL_TFLICK_NONE;
				else
				{		
					FTGesture.tapType.tap = FTGL_TTAP_NONE;	
					FTGesture.tap_tchs = 0;
					FTGesture.flickType.flick = (FTGLENUM_TFLICK)FTGesture.swipeType.swipe;
					FTGesture.flickType.numberofTouches =  (FTGLENUM_TPOINTS)FTGesture.swipeType.numberofTouches;
					FTGesture.swipeType.swipe = FTGL_TSWIPE_NONE;
					gstatus = FTGL_FLICKCHANGE;
				}	
			}else
			{
				if(FTGesture.swipeType.swipe==FTGL_TSWIPE_SWIPEIGNORE)
				FTGesture.swipeType.swipe = FTGL_TSWIPE_NONE;
				else
				gstatus = FTGL_SWIPECHANGE;
			}	
		}	
		FTGesture.penUpcts++;
		FTGesture.penDowncts=0;
          	if(FTGesture.penUpcts > 2)
		FTGesture.swipe_tchs = 0;
	}else 
	{		
		if(FTGesture.transform_function<0)
		FTGesture.transform_function = -transform;
		else 
		FTGesture.transform_function = transform;
		if(FTGesture.tapType.tap==FTGL_TTAP_LONGTAP)
		gstatus = FTGL_TAPCHANGE;			
		FTGesture.penUpcts=0;
		FTGesture.penDowncts++;		
	}
	if(FTGesture.scroller.velocityY!=0 || FTGesture.scroller.velocityX!=0)		
	gstatus |= FTGL_SCROLLERCHANGE;		
	return gstatus;
}
/********************************************************************************************
**Usage			: API to Initialize the Gesture Variables
**Return 		: FTGL_OK	
*********************************************************************************************/
FTGLSTATUS FTGLInit()
{
	int16_t i;
	for(i=0;i<FTGL_MAXTOUCH_PTS;i++)
	{
		FTGesture.startX[i] = FTGesture.currX[i] = FTGesture.prevX[i] = FTGL_NOTOUCH;
		FTGesture.startY[i] = FTGesture.currY[i] = FTGesture.prevY[i] = FTGL_NOTOUCH;
	}	
	FTGesture.penDowncts = 0;
	FTGesture.penUpcts = 0;
	FTGesture.currNumTchs = FTGesture.startNumTchs = FTGesture.prevNumTchs = 0;
	FTGesture.swipeType.swipe = FTGL_TSWIPE_NONE;
	FTGesture.tapType.tap = FTGL_TTAP_NONE;
	FTGesture.flickType.flick = FTGL_TFLICK_NONE;
	FTGesture.transform.angleofRotation = 0;
	FTGesture.transform.scaleX = 100;
	FTGesture.transform.scaleY = 100;
	FTGesture.transform.dragX = 0;
	FTGesture.transform.dragY = 0;
	FTGesture.scroller.velocityY = 0;
	FTGesture.scroller.velocityX = 0;
	FTGesture.scroller.baseX = 0;
	FTGesture.scroller.baseY = 0;	
	FTGesture.tap_tchs = 0;
	FTGesture.tap_cts = 0;
	FTGesture.tap_cx = 0;
	FTGesture.tap_cy = 0;	
	FTGesture.swipe_tchs = 0;
	FTGesture.swipe_cx =0;
	FTGesture.swipe_cy = 0;	
	FTGesture.scrollerprev_centery = 0;
	FTGesture.scrollerprev_centerx = 0;
	FTGesture.prev_angle = 0;
	FTGesture.prev_scale = 0;
	return FTGL_OK;
}
/********************************************************************************************
**Usage			: API to Exit the Gesture 
**Return 		: FTGL_OK	
*********************************************************************************************/
FTGLSTATUS FTGLExit()
{
	return FTGL_OK;
}
/********************************************************************************************
**Usage			: API to set the Scale Range(Max/Min) of Both X and Y direction
**minX/maxX		: Minimum/Maximum of X direction
**minY/maxY		: Minimum/Maximum of Y direction
**Return 		: FTGL_OK	
*********************************************************************************************/
FTGLSTATUS FTGLSetScaleRange(int16_t minX,int16_t maxX,int16_t minY,int16_t maxY)
{
	FTGesture.transform.minScaleX = minX;	
	FTGesture.transform.maxScaleX = maxX;
	FTGesture.transform.minScaleY = minY;
	FTGesture.transform.maxScaleY = maxY;			
	return FTGL_OK;
}
/********************************************************************************************
**Usage			: API to Set the Scale values of X and Y Direction
**x/y			: Scale Values of X and Y
**Return 		: if set value is less than min or greater than max 
				  api returns "FTGL_SETPARAM_ERROR"	Otherwise "FTGL_OK"
*********************************************************************************************/
FTGLSTATUS FTGLSetScale(int16_t sx,int16_t sy)
{
	FTGesture.transform.scaleX = sx;
	FTGesture.transform.scaleY = sy;			
	if(sx<FTGesture.transform.minScaleX || sx>FTGesture.transform.maxScaleX) return FTGL_WARNG;
	if(sy<FTGesture.transform.minScaleY || sy>FTGesture.transform.maxScaleY) return FTGL_WARNG;
	return FTGL_OK;
}
/********************************************************************************************
**Usage			: API to set the Drag Range(Max/Min) of Both X and Y direction
**minX/maxX		: Minimum/Maximum of X direction
**minY/maxY		: Minimum/Maximum of Y direction
**Return 		: FTGL_OK	
*********************************************************************************************/
FTGLSTATUS FTGLSetDragRange(int16_t minX,int16_t maxX,int16_t minY,int16_t maxY)
{
	FTGesture.transform.minDragX = minX;
	FTGesture.transform.maxDragX = maxX;
	FTGesture.transform.minDragY = minY;	
	FTGesture.transform.maxDragY = maxY;
	return FTGL_OK;
}
/********************************************************************************************
**Usage			: API to Set the Drag values of X and Y Direction
**x/y			: Drag Values of X and Y
**Return 		: if set value is less than min or greater than max 
				  api returns "FTGL_SETPARAM_ERROR"	Otherwise "FTGL_OK"
*********************************************************************************************/
FTGLSTATUS FTGLSetDrag(int16_t dx,int16_t dy)
{	
	FTGesture.transform.dragX = dx;
	FTGesture.transform.dragY = dy;	
	if(dx<FTGesture.transform.minDragX || dx>FTGesture.transform.maxDragX) return FTGL_WARNG;
	if(dy<FTGesture.transform.minDragY || dy>FTGesture.transform.maxDragY) return FTGL_WARNG;
	return FTGL_OK;
}
/********************************************************************************************
**Usage			: API to Set angle of rotation
**angle			: Angle of Rotation
**Return 		: FTGL_OK	
*********************************************************************************************/
FTGLSTATUS FTGLSetAngle(int16_t angle)
{
	FTGesture.transform.angleofRotation = angle;	
	if(angle<=360) return FTGL_OK; 
	return FTGL_WARNG;
}
/********************************************************************************************
**Usage			: API to set the Scroller Range(Max/Min) of Both X and Y direction
**minX/maxX		: Minimum/Maximum of X direction
**minY/maxY		: Minimum/Maximum of Y direction
**Return 		: FTGL_OK	
*********************************************************************************************/
FTGLSTATUS FTGLSetScrollerRange(int32_t minX,int32_t maxX,int32_t minY,int32_t maxY)
{
	FTGesture.scroller.minLimitX = minX*16;	
	FTGesture.scroller.maxLimitX = maxX*16;
	FTGesture.scroller.minLimitY = minY*16;
	FTGesture.scroller.maxLimitY = maxY*16;	
	return FTGL_OK;
}
/********************************************************************************************
**Usage			: API to set the Scroller Mode(Horizontal/Vertical)
**hor			: Horizontal(ENABLE/DISABLE)
**ver	 		: Vertical(ENABLE/DISABLE)
**Return 		: FTGL_OK	
*********************************************************************************************/
FTGLSTATUS FTGLSetScrollerMode(uint8_t hor,uint8_t ver)
{
	FTGesture.scroller.mode = 0;
	FTGesture.scroller.mode = ver<<1|hor;
	return FTGL_OK;
}

/********************************************************************************************
**Usage			: API to set the both Scroller X & Y direction base
**scx			: X Direction data
**scy	 		: Y Direction data
**Return 		: FTGL_OK	
*********************************************************************************************/
FTGLSTATUS FTGLSetScrollerXY(int32_t scrollx,int32_t scrolly)
{	
	FTGesture.scroller.baseX = scrollx;
	FTGesture.scroller.baseY = scrolly;
	return FTGL_OK;
}

/********************************************************************************************
**Usage			: API to set the Transform(scale/angle)
**tap			: Tap Type
**angle 		: Angle of Rotation		
**sx/sy 		: Scale x/y direction
**Return 		: FTGL_OK	
*********************************************************************************************/
FTGLSTATUS FTGLSetTransform(int16_t angle,int16_t sx,int16_t sy,int16_t dx,int16_t dy)
{	
	FTGLSetScale(sx,sy);
	FTGLSetDrag(dx,dy);
	FTGLSetAngle(angle);
	return FTGL_OK;		
}
/********************************************************************************************
**Usage			: API to get the Type of Tap and Number of Touches
**tap			: Tap Type
**nooftouches	: Number of Touches
**Return 		: FTGL_OK  
*********************************************************************************************/
FTGLSTATUS FTGLGetTaptype(uint8_t *tap,uint8_t *nooftchs)
{
	*nooftchs = (uint8_t)FTGesture.tapType.numberofTouches;
	*tap = (uint8_t)FTGesture.tapType.tap;
	FTGesture.tapType.tap = FTGL_TTAP_NONE;
	FTGesture.tapType.numberofTouches = FTGL_TPOINTS_NONE;
	return FTGL_OK;
}
/********************************************************************************************
**Usage			: API to get the Direction of Swipe and Number of Touches
**swipe			: Swipe Type
**nooftouches	: Number of Touches
**Return 		: FTGL_OK  
*********************************************************************************************/
FTGLSTATUS FTGLGetSwipetype(uint8_t *swipe,uint8_t *nooftchs)
{
	*nooftchs = (uint8_t)FTGesture.swipeType.numberofTouches;
	*swipe = (uint8_t)FTGesture.swipeType.swipe;
	FTGesture.swipeType.swipe = FTGL_TSWIPE_NONE;
	FTGesture.swipeType.numberofTouches = FTGL_TPOINTS_NONE;
	return FTGL_OK;
}
/********************************************************************************************
**Usage			: API to get the Direction of Swipe and Number of Touches
**swipe			: Swipe Type
**nooftouches	: Number of Touches
**Return 		: FTGL_OK  
*********************************************************************************************/
FTGLSTATUS FTGLGetFlicktype(uint8_t *flick,uint8_t *nooftchs)
{	
	*nooftchs = (uint8_t)FTGesture.flickType.numberofTouches;
	*flick = (uint8_t)FTGesture.flickType.flick;
	FTGesture.flickType.flick = FTGL_TFLICK_NONE;
	FTGesture.flickType.numberofTouches = FTGL_TPOINTS_NONE;
	return FTGL_OK;
}
/*******************************************************************************************
**Usage			: API to get an Average centre of all touch points 
**cx 			: Centre x direction
**cy 			: Centre y direction
**Return 		: FTGL_OK
********************************************************************************************/
FTGLSTATUS FTGLGetCenter(int16_t *cx,int16_t *cy)
{
	*cx = FTGesture.centerX;
	*cy = FTGesture.centerY;
	return FTGL_OK;
}
/*******************************************************************************************
**Usage			: API to get the Transform values(angle/scale/drag) of both X & Y-Direction
**angle 		: Angle of Rotation		
**sx/sy 		: Scale x/y direction
**dx/dy 		: Drag x/y direction
**Return 		: FTGL_OK	
********************************************************************************************/
FTGLSTATUS FTGLGetTransform(int16_t *angle,int16_t *sx,int16_t *sy,int16_t *dx,int16_t *dy)
{
	*angle = FTGesture.transform.angleofRotation;
	*sx = FTGesture.transform.scaleX;
	*sy = FTGesture.transform.scaleY;
	*dx = FTGesture.transform.dragX;
	*dy = FTGesture.transform.dragY;
	return FTGL_OK;
}
/*******************************************************************************************
**Usage			: API to get the Transform Angle of Rotation 	
**angle 		: Angle of Rotation		
**Return 		: FTGL_OK	
********************************************************************************************/
FTGLSTATUS FTGLGetAngle(int16_t *angle)
{
	*angle = FTGesture.transform.angleofRotation;
	return FTGL_OK;
}
/*******************************************************************************************
**Usage			: API to get the Transform - Scale values of both X & Y
**sx 			: Scale x direction
**sy 			: Scale y direction
**Return 		: FTGL_OK
********************************************************************************************/
FTGLSTATUS FTGLGetScale(int16_t *sx,int16_t *sy)
{
	*sx = FTGesture.transform.scaleX;
	*sy = FTGesture.transform.scaleY;
	return FTGL_OK;
}
/********************************************************************************************
**Usage			: API to get the Transform-Draging values of both X & Y-Direction
**dx 			: Drag x direction
**dy 			: Drag y direction
**Return 		: FTGL_OK
*********************************************************************************************/
FTGLSTATUS FTGLGetDrag(int16_t *dx,int16_t *dy)
{
	*dx = FTGesture.transform.dragX;
	*dy = FTGesture.transform.dragY;
	return FTGL_OK;
}
/********************************************************************************************
**Usage			: API to get the Scrolling values of both X & Y-Direction
**scx 			: x - scroller base
**scy 			: y - scroller base
**Return 		: FTGL_OK
*********************************************************************************************/
FTGLSTATUS FTGLGetScroller(int32_t *scx,int32_t *scy)
{
	*scx = FTGesture.scroller.baseX;
	*scy = FTGesture.scroller.baseY;
	return FTGL_OK;
}
/********************************************************************************************
**Usage			: API to get the Scrolling value of X-Direction
**scx 			: x - scroller base
**Return 		: FTGL_OK
*********************************************************************************************/
FTGLSTATUS FTGLGetScrollerX(int32_t *scx)
{
	*scx = FTGesture.scroller.baseX;	
	return FTGL_OK;
}
/********************************************************************************************
**Usage			: API to get the Scrolling value of Y- Direction
**scy 			: y - scroller base
**Return 		: FTGL_OK	
*********************************************************************************************/
FTGLSTATUS FTGLGetScrollerY(int32_t *scy)
{
	*scy = FTGesture.scroller.baseY;	
	return FTGL_OK;
}
/********************************************************************************************
**Usage			: API to get the Coordinates of three States of touch points
**startx/starty	: Starting Coordinate of X/Y
**currx/curry 	: Current Coordinate of X/Y
**prevx/prevy 	: Previous Coordinate of X/Y
**Return 		: FTGL_OK
*********************************************************************************************/
FTGLSTATUS FTGLGetCoordinates(int16_t *startx,int16_t *starty,int16_t *currx,int16_t *curry,\
								int16_t *prevx,int16_t *prevy)
{
	int16_t i;	
	for(i = 0;i < FTGL_MAXTOUCH_PTS;i++)
	{
		startx[i] = FTGesture.startX[i];
		starty[i] = FTGesture.startY[i];
		currx[i]  = FTGesture.currX[i];
		curry[i]  = FTGesture.currY[i];
		prevx[i]  = FTGesture.prevX[i];
		prevy[i]  = FTGesture.prevY[i];
	}
	return FTGL_OK;	
}

/********************************************************************************************
**Usage			: API to get the Coordinates of Tap
**tapx/tapy		: Tap Coordinate of X/Y
**Return 		: FTGL_OK
*********************************************************************************************/

FTGLSTATUS FTGLGetTapCoordinates(int16_t *tapx,int16_t *tapy)
{
	int16_t i;	
	for(i = 0;i < FTGL_MAXTOUCH_PTS;i++)
	{
		tapx[i] = FTGesture.startX[i];
		tapy[i] = FTGesture.startY[i];
	}
	return FTGL_OK;	
}
/********************************************************************************************
**Usage			: API to get the Current Number of Touches is present on the Screen
**nooftouches	: Number of Touches present 
**Return 		: FTGL_OK
*********************************************************************************************/
FTGLSTATUS FTGLGetNoOfTchs(uint8_t *nooftouches)
{
	*nooftouches = FTGesture.currNumTchs;
	return FTGL_OK;
}
/********************************************************************************************	
**Usage			: API to get the Touch Status
**Return 		: TRUE or FALSE 
*********************************************************************************************/
FTBOOL FTGLIsTouch()
{
	if(FTGesture.currNumTchs) return FTGL_TRUE; 	
	return FTGL_FALSE;
}
/*************************************END****************************************************/	
