#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
#define PACKED
#pragma pack(push,1)
#endif // defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM) 

typedef struct 
#if defined(FT900_PLATFORM)
__attribute__ ((__packed__))
#else
#endif
{
  unsigned char fileMarker1;       /* 'B' */                       
  unsigned char fileMarker2;       /* 'M' */ 
  unsigned int   bfSize;             
  unsigned short unused1;           
  unsigned short unused2;           
  unsigned int   imageDataOffset;  /* Offset to the start of image data */
} FILEHEADER;

typedef struct 
#if defined(FT900_PLATFORM)
__attribute__((__packed__))
#else
#endif
{ 
  unsigned int   biSize;            
  signed int     width;            /* Width of the image */ 
  signed int     height;           /* Height of the image */ 
  unsigned short planes;             
  unsigned short bitPix;             
  unsigned int   biCompression;      
  unsigned int   biSizeImage;        
  int            biXPelsPerMeter;    
  int            biYPelsPerMeter;    
  unsigned int   biClrUsed;          
  unsigned int   biClrImportant;     
} INFOHEADER;

#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
#pragma pack(pop)
#undef PACKED
#endif // defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM) 
