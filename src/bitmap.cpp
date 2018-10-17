#pragma pack(push, 1)
struct bitmap_image_header
{
  u32 SizeOfImageHeader;
  s32 WidthInPixels;
  s32 HeightInPixels;
  u16 ColorPlanes = 1;      // Must be 1
  u16 BPP;                  // Bits per pixel
  u32 CompressionType = 3;  // 0 == uncompressed
  u32 SizeInBytes = 0;      // 0 for uncompressed images
  u32 xPixelsPerMeter;      // Preferred Resolution in Pixels/Meter
  u32 yPixelsPerMeter;      // Preferred Resolution in Pixels/Meter
  u32 ColorMapsUsed;        // ??
  u32 NumSignificantColors; // ??
  u32 RedMask;
  u32 GreenMask;
  u32 BlueMask;
  u32 AlphaMask; // This is not technically defined in the spec, but I observed GIMP requiring it
};

struct bitmap_header
{
  u16 Type;
  u32 FileSizeInBytes;
  u32 Ignored;
  u32 OffsetToPixelData;
  bitmap_image_header Image;
};

struct bitmap
{
  v2i Dim;
  u32_stream Pixels;
};
#pragma pack(pop)

bitmap
ReadBitmapFromDisk(const char *Filename, memory_arena *Arena)
{
  bitmap_header Header = {};
  FILE *File = fopen(Filename, "rb");
  s32 SizeReadFromDisk = 0;
  u32* Pixels = 0;
  u32 PixelCount = 0;
  if (File)
  {
    SizeReadFromDisk += fread(&Header, 1, sizeof(Header), File);

    // For now, we only support reading bitmaps that are bottom-up ie. Origin in top-left corner
    Header.Image.HeightInPixels = Header.Image.HeightInPixels;

    PixelCount = Header.Image.WidthInPixels*Header.Image.HeightInPixels;
    Pixels = Allocate(u32, Arena, PixelCount);
    SizeReadFromDisk += fread(Pixels, 1, Header.Image.SizeInBytes, File);
  }
  else { Error("Opening %s for reading", Filename); }
  fclose(File);

  Assert(Header.Image.CompressionType == 3);
  Assert(SizeReadFromDisk == Header.FileSizeInBytes);

  bitmap Result = {};
  Result.Dim = V2i(Header.Image.WidthInPixels, Header.Image.HeightInPixels);
  Result.Pixels = U32_Stream(Pixels, Pixels+PixelCount);

  return Result;
}

b32
WriteBitmapToDisk(bitmap *Bitmap, const char *Filename)
{
  bitmap_header Header = {};
  Header.Type = 0x4D42;
  Header.FileSizeInBytes = sizeof(Header) + TotalSize(&Bitmap->Pixels);
  Header.OffsetToPixelData = sizeof(Header);

  Header.Image.SizeOfImageHeader    = sizeof(bitmap_image_header);
  Header.Image.WidthInPixels        = Bitmap->Dim.x;
  Header.Image.HeightInPixels       = Bitmap->Dim.y;
  Header.Image.ColorPlanes          = 1;
  Header.Image.BPP                  = 32;
  Header.Image.CompressionType      = 3;
  Header.Image.SizeInBytes          = TotalSize(&Bitmap->Pixels);
  Header.Image.xPixelsPerMeter      = 2835;
  Header.Image.yPixelsPerMeter      = 2835;
  Header.Image.ColorMapsUsed        = 0;
  Header.Image.NumSignificantColors = 0;
  Header.Image.RedMask              = 0x000000FF;
  Header.Image.GreenMask            = 0x0000FF00;
  Header.Image.BlueMask             = 0x00FF0000;


  u32 SizeWritten = 0;
  FILE *File = fopen(Filename, "wb");
  if (File)
  {
    SizeWritten += fwrite(&Header, 1, sizeof(Header), File);
    SizeWritten += fwrite(Bitmap->Pixels.Start, 1, TotalSize(&Bitmap->Pixels), File);
  }
  else { Error("Opening %s for writing", Filename); }
  fclose(File);

  Assert(SizeWritten == Header.FileSizeInBytes);

  b32 Result = SizeWritten == Header.FileSizeInBytes;
  return Result;
}

bitmap
AllocateBitmap(v2i Dim, memory_arena *Arena)
{
  u32 Size = Dim.x*Dim.y;
  u32* PixelPtr = Allocate(u32, Arena, Size);

  u32_stream Pixels = U32_Stream(PixelPtr, PixelPtr+Size);

  bitmap Bitmap = {};
  Bitmap.Dim = Dim;
  Bitmap.Pixels = Pixels;

  return Bitmap;
}

v4
Unpack255RGBAToLinear(u32 C)
{
  u8 R =  0xFF & C;
  u8 G =  0xFF & (C>>8);
  u8 B =  0xFF & (C>>16);
  u8 A =  0xFF & (C>>24);

  v4 Color = {};
  Color.r = (r32)R/255.0f;
  Color.g = (r32)G/255.0f;
  Color.b = (r32)B/255.0f;
  Color.a = (r32)A/255.0f;

  return Color;
}

u32
PackRGBALinearTo255(v4 Color)
{
  u8 R = Color.r*255;
  u8 G = Color.g*255;
  u8 B = Color.b*255;
  u8 A = Color.a*255;

  u32 C =   A<<24 | B<<16 | G<<8 | R;
  return C;
}

inline void
FillBitmap(u32 C, bitmap *Bitmap)
{
  Bitmap->Pixels.At = Bitmap->Pixels.Start;
  while (Bitmap->Pixels.At < Bitmap->Pixels.End)
  {
    *Bitmap->Pixels.At = C;
    Bitmap->Pixels.At++;
  }
  return;
}

inline u32
PixelCount(bitmap *Bitmap)
{
  u32 Result = Bitmap->Dim.x*Bitmap->Dim.y;
  return Result;
}

inline v2
GetUVForCharCode(u32 Char)
{
  v2 Result = V2( (Char%16)/16.0f, (Char/16)/16.0f );
  return Result;
}

