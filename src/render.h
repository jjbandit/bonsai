#ifndef RENDER_H
#define RENDER_H

#if BONSAI_INTERNAL
#define Draw(VertexCount) \
  Draw_(VertexCount, __FUNCTION__);

void
Draw_(u64 N, const char * Caller)
{
  TIMED_FUNCTION();
  u64 Index = ((u64)Caller) % Global_DrawCallArrayLength;

  debug_draw_call *DrawCall = &Global_DrawCalls[Index];

  if ( DrawCall->Caller )
  {
    // If this assert fires, we need to implement hash collisions here
    Assert(StringsMatch(DrawCall->Caller, Caller));
  }
  else
  {
    DrawCall->Caller = Caller;
  }

  DrawCall->Count++;
  glDrawArrays(GL_TRIANGLES, 0, N);
}
#else
void
Draw(u64 N)
{
  glDrawArrays(GL_TRIANGLES, 0, N);
}
#endif

inline void
SetViewport(v2 Dim)
{
  glViewport(0, 0, Dim.x, Dim.y);
  return;
}

// TODO(Jesse): This only gets used when computing the shadow map, so I'm not
// even sure if it works ATM
inline m4
Orthographic( r32 X, r32 Y, r32 Zmin, r32 Zmax, v3 Translate )
{
  r32 r = X;
  r32 l = -X;

  r32 t = Y;
  r32 b = -Y;

  r32 f = Zmax;
  r32 n = Zmin;

  m4 Result = {
    V4(2/(r-l) , 0       , 0        , -1*((r+l)/(r-l)) ) ,
    V4(0       , 2/(t-b) , 0        , -1*((t+b)/(t-b)) ) ,
    V4(0       , 0       , -2/(f-n) , -1*((f+n)/(f-n)) ) ,
    V4(0       , 0       , 0        , 1)
  };

  return Result;
}

inline m4
Perspective(radians FOV, v2 WindowDim, r32 NearClip, r32 FarClip)
{
  r32 TanHalfFOV = tan(FOV / 2.0f);
  r32 Aspect = WindowDim.x/WindowDim.y;

  r32 Z = (-(FarClip+NearClip)) / (FarClip-NearClip);
  r32 ZZ = (-(2.0f*FarClip*NearClip)) / (FarClip - NearClip);

  m4 Result = {
    V4(1.0f/(Aspect*TanHalfFOV) , 0               , 0  , 0)     ,
    V4(0                        , 1.0f/TanHalfFOV , 0  , 0)     ,
    V4(0                        , 0               , Z  , -1.0f) ,
    V4(0                        , 0               , ZZ , 0)     ,
  };

  return Result;
}

inline radians
Rads(degrees Degrees)
{
  radians Result = (Degrees/180);
  return Result;
}

inline m4
GetProjectionMatrix(camera *Camera, int WindowWidth, int WindowHeight)
{
  m4 Projection = Perspective(
      Rads(Camera->Frust.FOV),
      V2(WindowWidth, WindowHeight),
      Camera->Frust.nearClip,
      Camera->Frust.farClip);

  return Projection;
}

inline v3
GetRenderP(canonical_position P, camera *Camera)
{
  v3 CameraOffset = Camera->Target.Offset + (Camera->Target.WorldP * Global_WorldChunkDim);
  v3 Result = P.Offset + (P.WorldP * Global_WorldChunkDim) - CameraOffset;
  return Result;
}

inline v3
GetRenderP( chunk_dimension WorldChunkDim, canonical_position P, camera *Camera)
{
  TIMED_FUNCTION();
  v3 CameraOffset = Camera->Target.Offset + (Camera->Target.WorldP * WorldChunkDim);
  v3 Result = P.Offset + (P.WorldP * WorldChunkDim) - CameraOffset;
  return Result;
}

inline v3
GetRenderP( chunk_dimension WorldChunkDim, v3 Offset, camera *Camera)
{
  v3 Result = GetRenderP(WorldChunkDim, Canonical_Position(Offset, World_Position(0)), Camera);
  return Result;
}

inline v3
GetRenderP( chunk_dimension WorldChunkDim, world_position WorldP, camera *Camera)
{
  v3 Result = GetRenderP(WorldChunkDim, Canonical_Position(V3(0,0,0), WorldP), Camera);
  return Result;
}

inline v3
GetRenderP( chunk_dimension WorldChunkDim, entity *entity, camera *Camera)
{
  v3 Result = GetRenderP(WorldChunkDim, entity->P, Camera);
  return Result;
}

inline aabb
GetRenderSpaceAABB(chunk_dimension WorldChunkDim, entity *Entity, camera *Camera)
{
  v3 Radius = Entity->CollisionVolumeRadius;
  v3 Center = GetRenderP(WorldChunkDim, Entity->P, Camera) + Radius;

  aabb Result(Center, Radius);
  return Result;
}

inline m4
LookAt(v3 P, v3 Target, v3 Up)
{
  v3 Front = Normalize(Target - P);
  v3 Right = Normalize(Cross(Front, Up));
  Up       = Normalize(Cross(Right, Front));

  m4 Result = {
    V4(Right.x       , Up.x       , -Front.x      , 0) ,
    V4(Right.y       , Up.y       , -Front.y      , 0) ,
    V4(Right.z       , Up.z       , -Front.z      , 0) ,
    V4(-Dot(Right,P) , -Dot(Up,P) ,  Dot(Front,P) , 1)
  };

  return Result;
}

inline m4
GetViewMatrix(chunk_dimension WorldChunkDim, camera *Camera)
{
  m4 Result = LookAt(
    GetRenderP(WorldChunkDim, Camera->P, Camera),
    GetRenderP(WorldChunkDim, Camera->Target, Camera),
    Camera->Up
  );

  return Result;
}

void
BufferVertsToCard(umm BufferId, textured_2d_geometry_buffer *Mesh, u32 AttributeIndex)
{
  glEnableVertexAttribArray(AttributeIndex);
  glBindBuffer(GL_ARRAY_BUFFER, BufferId);
  glBufferData(GL_ARRAY_BUFFER, Mesh->CurrentIndex*sizeof(v3), Mesh->Verts, GL_STATIC_DRAW);
  glVertexAttribPointer(AttributeIndex, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
  return;
}

void
BufferVertsToCard(umm BufferId, untextured_2d_geometry_buffer *Mesh, u32 AttributeIndex)
{
  glEnableVertexAttribArray(AttributeIndex);
  glBindBuffer(GL_ARRAY_BUFFER, BufferId);
  glBufferData(GL_ARRAY_BUFFER, Mesh->CurrentIndex*sizeof(v3), Mesh->Verts, GL_STATIC_DRAW);
  glVertexAttribPointer(AttributeIndex, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
  return;
}

void
BufferVertsToCard(umm BufferId, untextured_3d_geometry_buffer *Mesh, u32 AttributeIndex)
{
  glEnableVertexAttribArray(AttributeIndex);
  glBindBuffer(GL_ARRAY_BUFFER, BufferId);
  glBufferData(GL_ARRAY_BUFFER, Mesh->CurrentIndex*sizeof(v3), Mesh->Verts, GL_STATIC_DRAW);
  glVertexAttribPointer(AttributeIndex, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
  return;
}

void
BufferColorsToCard(umm BufferId, textured_2d_geometry_buffer *Mesh, u32 AttributeIndex)
{
  glEnableVertexAttribArray(AttributeIndex);
  glBindBuffer(GL_ARRAY_BUFFER, BufferId);
  glBufferData(GL_ARRAY_BUFFER, Mesh->CurrentIndex*sizeof(v3), Mesh->Colors, GL_STATIC_DRAW);
  glVertexAttribPointer(AttributeIndex, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
  return;
}

void
BufferColorsToCard(umm BufferId, untextured_2d_geometry_buffer *Mesh, u32 AttributeIndex)
{
  glEnableVertexAttribArray(AttributeIndex);
  glBindBuffer(GL_ARRAY_BUFFER, BufferId);
  glBufferData(GL_ARRAY_BUFFER, Mesh->CurrentIndex*sizeof(v3), Mesh->Colors, GL_STATIC_DRAW);
  glVertexAttribPointer(AttributeIndex, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
  return;
}

void
BufferColorsToCard(umm BufferId, untextured_3d_geometry_buffer *Mesh, u32 AttributeIndex)
{
  glEnableVertexAttribArray(AttributeIndex);
  glBindBuffer(GL_ARRAY_BUFFER, BufferId);
  glBufferData(GL_ARRAY_BUFFER, Mesh->CurrentIndex*sizeof(v3), Mesh->Colors, GL_STATIC_DRAW);
  glVertexAttribPointer(AttributeIndex, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
  return;
}

void
BufferNormalsToCard(umm BufferId, untextured_3d_geometry_buffer *Mesh, u32 AttributeIndex)
{
  glEnableVertexAttribArray(AttributeIndex);
  glBindBuffer(GL_ARRAY_BUFFER, BufferId);
  glBufferData(GL_ARRAY_BUFFER, Mesh->CurrentIndex*sizeof(v3), Mesh->Normals, GL_STATIC_DRAW);
  glVertexAttribPointer(AttributeIndex, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
  return;
}


void
GenBuffers(u32 NumBuffers, u32 *BufferName)
{
  Print(NumBuffers);
  glGenBuffers(NumBuffers, BufferName);
  Print(*BufferName);
  Assert(*BufferName);
  return;
}


#define BUFFER_UVS_TO_CARD(BufferId, Mesh, AttributeIndex)                                           \
  Assert(Mesh->Verts);                                                                               \
  GL_Global->glEnableVertexAttribArray(AttributeIndex);                                              \
  GL_Global->glBindBuffer(GL_ARRAY_BUFFER, RG->UVBuffer);                                            \
  GL_Global->glBufferData(GL_ARRAY_BUFFER, Mesh->CurrentIndex*sizeof(v2), Geo->UVs, GL_STATIC_DRAW); \
  GL_Global->glVertexAttribPointer(AttributeIndex, 2, GL_FLOAT, GL_FALSE, 0, (void*)0 );



#endif
