#include <stdio.h>
#include <string.h>
#include <bonsai.h>
#include <bonsai_math.h>
#include <bonsai_types.h>

// Number of bytes per int according to .vox file format
#define VOX_INT_BYTES 4
#define VOX_CHAR_BYTES 1


#define MV_ID( a, b, c, d ) \
  (( a ) | ( b << 8 ) | ( c << 16 ) | ( d << 24 ))

enum Chunk_ID
{
  ID_NONE = 0,

  ID_VOX  = MV_ID( 'V', 'O', 'X', ' ' ),
  ID_MAIN = MV_ID( 'M', 'A', 'I', 'N' ),
  ID_PACK = MV_ID( 'P', 'A', 'C', 'K' ),
  ID_SIZE = MV_ID( 'S', 'I', 'Z', 'E' ),
  ID_XYZI = MV_ID( 'X', 'Y', 'Z', 'I' ),
  ID_RGBA = MV_ID( 'R', 'G', 'B', 'A' )
};

inline unsigned char
ReadChar(FILE* File, int* byteCounter)
{
  unsigned char c;
  fread(&c, 1, VOX_CHAR_BYTES, File);
  *byteCounter -= VOX_CHAR_BYTES;
  return c;
}

inline int
ReadInt(FILE* File)
{
  int i;
  fread(&i, 1, VOX_INT_BYTES, File);
  return i;
}

inline int
ReadInt(FILE* File, int* byteCounter)
{
  *byteCounter -= VOX_INT_BYTES;
  return ReadInt(File);
}

inline Chunk_ID
GetHeaderType(FILE* File, int* byteCounter)
{
  int ID = ReadInt(File, byteCounter);

  Assert( ID == ID_VOX  ||
          ID == ID_MAIN ||
          ID == ID_PACK ||
          ID == ID_SIZE ||
          ID == ID_XYZI ||
          ID == ID_RGBA );

  return (Chunk_ID)ID;
}

inline void
ReadVoxChunk(FILE *File)
{
  int ID, Version = INT_MAX;

  ID = ReadInt(File);
  Assert( ID == ID_VOX );

  Version = ReadInt(File);
  Assert(Version == 150);

  return;
}

inline int
ReadMainChunk(FILE *File)
{
  int ID = ReadInt(File);
  Assert( ID == ID_MAIN );

  int selfSize = ReadInt(File);
  Assert(selfSize == 0 );

  int childSize = ReadInt(File);
  Assert(childSize > 0 );

  return childSize;
}

inline chunk_dimension
ReadSizeChunk(FILE *File, int* byteCounter)
{
  // Is throwing the chunk size away okay?
  ReadInt(File, byteCounter);
  ReadInt(File, byteCounter);

  int chunkX = ReadInt(File, byteCounter);
  int chunkY = ReadInt(File, byteCounter);
  int chunkZ = ReadInt(File, byteCounter);

  chunk_dimension Result = Chunk_Dimension(chunkX, chunkY, chunkZ);
  return Result;
}

inline int
ReadPackChunk(FILE *File, int* byteCounter)
{
  // Is throwing the chunk size away okay?
  ReadInt(File, byteCounter);
  ReadInt(File, byteCounter);

  int nChunks = ReadInt(File, byteCounter);

  return nChunks;
}

inline int
ReadXYZIChunk(FILE *File, int* byteCounter)
{
  // This is the size of teh XYZI chunk .. should always be 1 int (nVoxels)
  ReadInt(File, byteCounter);
  ReadInt(File, byteCounter);

  int nVoxels = ReadInt(File, byteCounter);
  return nVoxels;
}

void
BuildEntityBoundaryVoxels(chunk_data *chunk, chunk_dimension Dim)
{
  UnSetFlag(chunk, Chunk_RebuildBoundary);

  for ( int z = 0; z < Dim.z ; ++z )
  {
    for ( int y = 0; y < Dim.y ; ++y )
    {
      for ( int x = 0; x < Dim.x ; ++x )
      {
        voxel_position LocalVoxelP = Voxel_Position(x,y,z);

        if ( NotFilled( chunk, LocalVoxelP, Dim) )
          continue;

        voxel *Voxel = &chunk->Voxels[GetIndex(Voxel_Position(x,y,z), chunk, Dim)];

        voxel_position rightVoxel = LocalVoxelP + Voxel_Position(1, 0, 0);
        voxel_position leftVoxel = LocalVoxelP - Voxel_Position(1, 0, 0);

        voxel_position topVoxel = LocalVoxelP + Voxel_Position(0, 1, 0);
        voxel_position botVoxel = LocalVoxelP - Voxel_Position(0, 1, 0);

        voxel_position frontVoxel = LocalVoxelP + Voxel_Position(0, 0, 1);
        voxel_position backVoxel = LocalVoxelP - Voxel_Position(0, 0, 1);

        bool DidPushVoxel = false;

        if ( (!IsInsideDim(Dim, rightVoxel)) || NotFilled( chunk, rightVoxel, Dim))
        {
          SetFlag(Voxel, Voxel_RightFace);
          DidPushVoxel = true;
        }
        if ( (!IsInsideDim( Dim, leftVoxel  )) || NotFilled( chunk, leftVoxel, Dim))
        {
          SetFlag(Voxel, Voxel_LeftFace);
          DidPushVoxel = true;
        }
        if ( (!IsInsideDim( Dim, botVoxel   )) || NotFilled( chunk, botVoxel, Dim))
        {
          SetFlag(Voxel, Voxel_BottomFace);
          DidPushVoxel = true;
        }
        if ( (!IsInsideDim( Dim, topVoxel   )) || NotFilled( chunk, topVoxel, Dim))
        {
          SetFlag(Voxel, Voxel_TopFace);
          DidPushVoxel = true;
        }
        if ( (!IsInsideDim( Dim, frontVoxel )) || NotFilled( chunk, frontVoxel, Dim))
        {
          SetFlag(Voxel, Voxel_FrontFace);
          DidPushVoxel = true;
        }
        if ( (!IsInsideDim( Dim, backVoxel  )) || NotFilled( chunk, backVoxel, Dim))
        {
          SetFlag(Voxel, Voxel_BackFace);
          DidPushVoxel = true;
        }

        if (DidPushVoxel)
        {
          boundary_voxel BoundaryVoxel(Voxel, LocalVoxelP);
          PushBoundaryVoxel(chunk, &BoundaryVoxel, Dim);
        }

      }
    }
  }
}

model
LoadModel(memory_arena *WorldStorage, char const *filepath)
{
  model Result;
  s32 totalChunkBytes = 0;

  FILE *ModelFile = fopen(filepath, "r");

  if (ModelFile)
  {
    // Ensure we're dealing with a .vox file
    ReadVoxChunk(ModelFile);

    totalChunkBytes = ReadMainChunk(ModelFile);
    int bytesRemaining = totalChunkBytes;

    // TODO(Jesse) : Actually read all the data!
    while (bytesRemaining > 0)
    {
      /* Log("%d\n", bytesRemaining); */


      Chunk_ID CurrentId = GetHeaderType(ModelFile, &bytesRemaining);
      switch ( CurrentId )
      {
        case ID_RGBA:
        {
        } break;

        case ID_PACK:
        {
          /* int nChunks = */ ReadPackChunk(ModelFile, &bytesRemaining);
        } break;

        case ID_SIZE:
        {
          // Instead of trusting the model (they're always wrong), we'll loop
          // through the data and figure it out ourselves.

          /* chunk_dimension Dim = */ ReadSizeChunk(ModelFile, &bytesRemaining);
        } break;

        case ID_XYZI:
        {
          s32 ModelVoxelCount = ReadXYZIChunk(ModelFile, &bytesRemaining);

          s32 maxX = 0, maxY = 0, maxZ = 0;
          s32 minX = INT_MAX, minY = INT_MAX, minZ = INT_MAX;

          boundary_voxel *LocalVoxelCache = (boundary_voxel *)calloc(ModelVoxelCount, sizeof(boundary_voxel) );
          for( s32 VoxelCacheIndex = 0;
               VoxelCacheIndex < ModelVoxelCount;
               ++VoxelCacheIndex)
          {
            s32 X = (s32)ReadChar(ModelFile, &bytesRemaining);
            s32 Y = (s32)ReadChar(ModelFile, &bytesRemaining);
            s32 Z = (s32)ReadChar(ModelFile, &bytesRemaining);
            s32 W = (s32)ReadChar(ModelFile, &bytesRemaining); // Color

            maxX = Max(X, maxX);
            maxY = Max(Y, maxY);
            maxZ = Max(Z, maxZ);

            minX = Min(X, minX);
            minY = Min(Y, minY);
            minZ = Min(Z, minZ);

            LocalVoxelCache[VoxelCacheIndex] = boundary_voxel(X,Y,Z,W);
            SetFlag(&LocalVoxelCache[VoxelCacheIndex], Voxel_Filled);
          }

          chunk_dimension Min = Chunk_Dimension(minX, minY, minZ);

          // Add 1 to the max dimension to convert from indicies to positions
          chunk_dimension Max = Chunk_Dimension(maxX+1, maxY+1, maxZ+1);

          chunk_dimension ModelDim = Max - Min;

          // TODO(Jesse): Load models in multiple chunks instead of one
          // monolithic one. The storage for chunks must be as large as the
          // largest chunk we will EVER load, which should definately not be
          // decided at compile time.
          Result.Chunk = AllocateChunk(WorldStorage, ModelDim);
          Assert(Result.Chunk);

          Result.Dim = ModelDim;

          for( int VoxelCacheIndex = 0;
               VoxelCacheIndex < ModelVoxelCount;
               ++VoxelCacheIndex)
          {
            boundary_voxel *Voxel = &LocalVoxelCache[VoxelCacheIndex];
            s32 Index = GetIndex(Voxel->Offset, Result.Chunk, Result.Dim);
            Result.Chunk->Voxels[Index] = Voxel->V;
          }

          free(LocalVoxelCache);

          SetFlag(&Result, Chunk_Initialized);

          // TODO(Jesse): Are we really done?
          goto loaded;
        } break;

        InvalidDefaultCase;
      }
    }


  }
  else
  {
    Error("Couldn't read model file '%s' .", filepath);
  }

loaded:

  BuildEntityBoundaryVoxels(Result.Chunk, Result.Dim);

  return Result;
}
