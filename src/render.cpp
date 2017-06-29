#include <cstring>
#include <csignal>

#include <sstream>
#include <string>

#include <bonsai.h>
#include <render.h>

#include <stdio.h>

#include <colors.h>

inline void
RightFaceVertexData( v3 MinP, r32 Diameter, r32* Result)
{
  r32 Temp[] = {
    MinP.x + Diameter , MinP.y +  Diameter , MinP.z +  Diameter ,
    MinP.x + Diameter , MinP.y             , MinP.z             ,
    MinP.x + Diameter , MinP.y +  Diameter , MinP.z             ,
    MinP.x + Diameter , MinP.y             , MinP.z             ,
    MinP.x + Diameter , MinP.y +  Diameter , MinP.z +  Diameter ,
    MinP.x + Diameter , MinP.y             , MinP.z +  Diameter ,
  };

  memcpy(Result, Temp, sizeof(Temp));

  return;
}

r32 RightFaceNormalData[] =
{
   1, 0, 0,
   1, 0, 0,
   1, 0, 0,
   1, 0, 0,
   1, 0, 0,
   1, 0, 0
};

inline void
LeftFaceVertexData( v3 MinP, r32 Diameter, r32 *Result)
{
  r32 Temp[] = {
    MinP.x , MinP.y +  Diameter , MinP.z +  Diameter ,
    MinP.x , MinP.y             , MinP.z             ,
    MinP.x , MinP.y +  Diameter , MinP.z             ,
    MinP.x , MinP.y             , MinP.z             ,
    MinP.x , MinP.y +  Diameter , MinP.z +  Diameter ,
    MinP.x , MinP.y             , MinP.z +  Diameter ,
  };

  memcpy(Result, Temp, sizeof(Temp));

  return;
}

r32 LeftFaceNormalData[] =
{
   -1, 0, 0,
   -1, 0, 0,
   -1, 0, 0,
   -1, 0, 0,
   -1, 0, 0,
   -1, 0, 0
};

inline void
BottomFaceVertexData( v3 MinP, r32 Diameter, r32 *Result)
{
  r32 Temp[] = {
    MinP.x + Diameter , MinP.y , MinP.z +  Diameter ,
    MinP.x            , MinP.y , MinP.z             ,
    MinP.x + Diameter , MinP.y , MinP.z             ,
    MinP.x            , MinP.y , MinP.z             ,
    MinP.x + Diameter , MinP.y , MinP.z +  Diameter ,
    MinP.x            , MinP.y , MinP.z +  Diameter ,
  };

  memcpy(Result, Temp, sizeof(Temp));

  return;
}

r32 BottomFaceNormalData[] =
{
   0,-1, 0,
   0,-1, 0,
   0,-1, 0,
   0,-1, 0,
   0,-1, 0,
   0,-1, 0
};

inline void
TopFaceVertexData( v3 MinP, r32 Diameter, r32 *Result)
{
  r32 Temp[] = {
    MinP.x + Diameter , MinP.y + Diameter , MinP.z + Diameter ,
    MinP.x            , MinP.y + Diameter , MinP.z            ,
    MinP.x + Diameter , MinP.y + Diameter , MinP.z            ,
    MinP.x            , MinP.y + Diameter , MinP.z            ,
    MinP.x + Diameter , MinP.y + Diameter , MinP.z + Diameter ,
    MinP.x            , MinP.y + Diameter , MinP.z + Diameter ,
  };

  memcpy(Result, Temp, sizeof(Temp));

  return;
}

r32 TopFaceNormalData[] =
{
  0, 1, 0,
  0, 1, 0,
  0, 1, 0,
  0, 1, 0,
  0, 1, 0,
  0, 1, 0
};

inline void
FrontFaceVertexData( v3 MinP, r32 Diameter, r32 *Result)
{
  r32 Temp[] = {
    MinP.x +  Diameter , MinP.y +  Diameter , MinP.z + Diameter ,
    MinP.x             , MinP.y             , MinP.z + Diameter ,
    MinP.x +  Diameter , MinP.y             , MinP.z + Diameter ,
    MinP.x             , MinP.y             , MinP.z + Diameter ,
    MinP.x +  Diameter , MinP.y +  Diameter , MinP.z + Diameter ,
    MinP.x             , MinP.y +  Diameter , MinP.z + Diameter ,
  };

  memcpy(Result, Temp, sizeof(Temp));

  return;
}

r32 FrontFaceNormalData[] =
{
  0, 0, 1,
  0, 0, 1,
  0, 0, 1,

  0, 0, 1,
  0, 0, 1,
  0, 0, 1
};

inline void
BackFaceVertexData( v3 MinP, r32 Diameter, r32 *Result)
{
  r32 Temp[] = {
    MinP.x + Diameter , MinP.y + Diameter , MinP.z ,
    MinP.x            , MinP.y            , MinP.z ,
    MinP.x + Diameter , MinP.y            , MinP.z ,
    MinP.x            , MinP.y            , MinP.z ,
    MinP.x + Diameter , MinP.y + Diameter , MinP.z ,
    MinP.x            , MinP.y + Diameter , MinP.z ,
  };

  memcpy(Result, Temp, sizeof(Temp));

  return;
}

r32 BackFaceNormalData[] =
{
  0, 0, -1,
  0, 0, -1,
  0, 0, -1,

  0, 0, -1,
  0, 0, -1,
  0, 0, -1,
};















GLOBAL_VARIABLE m4 IdentityMatrix = {V4(1, 0, 0 ,0),
                                     V4(0, 1, 0 ,0),
                                     V4(0, 0, 1 ,0),
                                     V4(0, 0, 0 ,0)};

bool
InitializeRenderGroup( platform *Plat, RenderGroup *RG )
{
  RG->Basis.ModelMatrix = IdentityMatrix;

  GL_Global->glGenFramebuffers(1, &RG->FBO);
  GL_Global->glBindFramebuffer(GL_FRAMEBUFFER, RG->FBO);

  glGenTextures(1, &RG->ColorTexture);
  glBindTexture(GL_TEXTURE_2D, RG->ColorTexture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Plat->WindowWidth, Plat->WindowHeight, 0, GL_RGBA, GL_FLOAT, NULL);
  glBindTexture(GL_TEXTURE_2D, 0);

  glGenTextures(1, &RG->NormalTexture);
  glBindTexture(GL_TEXTURE_2D, RG->NormalTexture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Plat->WindowWidth, Plat->WindowHeight, 0, GL_RGBA, GL_FLOAT, NULL);
  glBindTexture(GL_TEXTURE_2D, 0);

  glGenTextures(1, &RG->PositionTexture);
  glBindTexture(GL_TEXTURE_2D, RG->PositionTexture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Plat->WindowWidth, Plat->WindowHeight, 0, GL_RGBA, GL_FLOAT, NULL);
  glBindTexture(GL_TEXTURE_2D, 0);

  // Depth texture
  glGenTextures(1, &RG->DepthTexture);
  glBindTexture(GL_TEXTURE_2D, RG->DepthTexture);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32,
      Plat->WindowWidth, Plat->WindowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
  //

  GL_Global->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,  GL_TEXTURE_2D, RG->DepthTexture,    0);
  GL_Global->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, RG->ColorTexture,    0);
  GL_Global->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, RG->NormalTexture,   0);
  GL_Global->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, RG->PositionTexture, 0);

  u32 attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
  GL_Global->glDrawBuffers(3, attachments);


  if (GL_Global->glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    return false;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  GL_Global->glBindFramebuffer(GL_FRAMEBUFFER, 0);

  RG->ShaderID = LoadShaders( "SimpleVertexShader.vertexshader",
                              "SimpleFragmentShader.fragmentshader" );

  RG->MVPID                = GL_Global->glGetUniformLocation(RG->ShaderID, "MVP");
  RG->ModelMatrixID        = GL_Global->glGetUniformLocation(RG->ShaderID, "M");
  /* RG->LightPID             = glGetUniformLocation(RG->ShaderID, "LightP_worldspace"); */


  RG->LightingShader = LoadShaders( "Lighting.vertexshader",
                                    "Lighting.fragmentshader" );


  RG->DepthBiasMVPID          = GL_Global->glGetUniformLocation(RG->LightingShader, "DepthBiasMVP");
  RG->ShadowMapTextureUniform = GL_Global->glGetUniformLocation(RG->LightingShader, "shadowMap");
  RG->ColorTextureUniform     = GL_Global->glGetUniformLocation(RG->LightingShader, "gColor");
  RG->NormalTextureUniform    = GL_Global->glGetUniformLocation(RG->LightingShader, "gNormal");
  RG->PositionTextureUniform  = GL_Global->glGetUniformLocation(RG->LightingShader, "gPosition");
  RG->DepthTextureUniform     = GL_Global->glGetUniformLocation(RG->LightingShader, "gDepth");
  RG->GlobalLightDirectionID  = GL_Global->glGetUniformLocation(RG->LightingShader, "GlobalLightDirection");
  RG->ViewMatrixUniform       = GL_Global->glGetUniformLocation(RG->LightingShader, "ViewMatrix");
  RG->CameraPosUniform        = GL_Global->glGetUniformLocation(RG->LightingShader, "CameraPosUniform");


  RG->SimpleTextureShaderID = LoadShaders( "Passthrough.vertexshader",
                                           "SimpleTexture.fragmentshader" );

  RG->SimpleTextureUniform = GL_Global->glGetUniformLocation(RG->SimpleTextureShaderID, "Texture");
  //
  // Quad vertex buffer
  GL_Global->glGenBuffers(1, &RG->quad_vertexbuffer);
  GL_Global->glBindBuffer(GL_ARRAY_BUFFER, RG->quad_vertexbuffer);
  GL_Global->glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);



  //
  // World Data buffers
  u32 vertexbuffer;
  u32 colorbuffer;
  u32 normalbuffer;

  GL_Global->glGenBuffers(1, &vertexbuffer);
  GL_Global->glGenBuffers(1, &colorbuffer);
  GL_Global->glGenBuffers(1, &normalbuffer);

  RG->vertexbuffer = vertexbuffer;
  RG->colorbuffer  = colorbuffer;
  RG->normalbuffer = normalbuffer;

  return true;
}

bool
InitializeShadowBuffer(ShadowRenderGroup *ShadowGroup)
{
  // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
  GL_Global->glGenFramebuffers(1, &ShadowGroup->FramebufferName);
  GL_Global->glBindFramebuffer(GL_FRAMEBUFFER, ShadowGroup->FramebufferName);

  AssertNoGlErrors;

  // Depth texture. Slower than a depth buffer, but you can sample it later in your shader
  glGenTextures(1, &ShadowGroup->DepthTexture);
  glBindTexture(GL_TEXTURE_2D, ShadowGroup->DepthTexture);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16,
      SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

  AssertNoGlErrors;

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

  AssertNoGlErrors;

  // No color output in the bound framebuffer, only depth.
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);

  AssertNoGlErrors;

  GL_Global->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, ShadowGroup->DepthTexture, 0);
  AssertNoGlErrors;

  ShadowGroup->ShaderID = LoadShaders( "DepthRTT.vertexshader", "DepthRTT.fragmentshader");
  ShadowGroup->MVP_ID   = GL_Global->glGetUniformLocation(ShadowGroup->ShaderID, "depthMVP");

  AssertNoGlErrors;

  if(GL_Global->glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    return false;

  AssertNoGlErrors;


  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  GL_Global->glBindFramebuffer(GL_FRAMEBUFFER, 0);
  AssertNoGlErrors;

 return true;
}

void
RenderQuad(RenderGroup *RG)
{
  // TODO(Jesse): Please explain to me why I cannot draw two of these to the screen between clears
  /* glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); */

  GL_Global->glEnableVertexAttribArray(0);
  GL_Global->glBindBuffer(GL_ARRAY_BUFFER, RG->quad_vertexbuffer);
  GL_Global->glVertexAttribPointer(
    0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
    3,                  // size
    GL_FLOAT,           // type
    GL_FALSE,           // normalized?
    0,                  // stride
    (void*)0            // array buffer offset
  );

  glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles

  GL_Global->glDisableVertexAttribArray(0);
}

v3 GlobalLightDirection = V3( -1.0f, -1.0f, 6.0f);

inline m4
GetDepthMVP(chunk_dimension WorldChunkDim, Camera_Object *Camera)
{
  GlobalLightDirection = Normalize( GlobalLightDirection );

  // Compute the MVP matrix from the light's point of view
  m4 depthProjectionMatrix = Orthographic(SHADOW_MAP_XY, SHADOW_MAP_Z);

  v3 P = GetRenderP(WorldChunkDim, Camera->Target+GlobalLightDirection, Camera);
  v3 Target = GetRenderP(WorldChunkDim, Camera->Target, Camera);

  v3 Front = Normalize(Target-P);
  v3 Right = Cross( Front, V3(0,1,0) );
  v3 Up = Cross(Right, Front);

  m4 depthViewMatrix = LookAt(P, Target, Up);

  return depthProjectionMatrix * depthViewMatrix;
}

void
DrawWorldToFullscreenQuad( platform *Plat, chunk_dimension WorldChunkDim, RenderGroup *RG, ShadowRenderGroup *SG, Camera_Object *Camera)
{
  GL_Global->glBindFramebuffer(GL_FRAMEBUFFER, 0);
  GL_Global->glUseProgram(RG->LightingShader);
  glViewport(0, 0, Plat->WindowWidth, Plat->WindowHeight);

  GlobalLightDirection = Normalize( GlobalLightDirection );

  GL_Global->glUniform3fv(RG->GlobalLightDirectionID, 1, &GlobalLightDirection.E[0]);

  m4 biasMatrix = {
    V4(0.5, 0.0, 0.0, 0.0),
    V4(0.0, 0.5, 0.0, 0.0),
    V4(0.0, 0.0, 0.5, 0.0),
    V4(0.5, 0.5, 0.5, 1.0)
  };

  m4 depthBiasMVP = biasMatrix * GetDepthMVP(WorldChunkDim, Camera);
  GL_Global->glUniformMatrix4fv(RG->DepthBiasMVPID, 1, GL_FALSE, &depthBiasMVP.E[0].E[0]);

  m4 VP = RG->Basis.ViewMatrix;

  GL_Global->glUniformMatrix4fv(RG->ViewMatrixUniform, 1, GL_FALSE, &VP.E[0].E[0]);

  v3 CameraRenderP = GetRenderP(WorldChunkDim, Camera->P, Camera);
  GL_Global->glUniform3fv(RG->CameraPosUniform, 1, &CameraRenderP.E[0]);

  GL_Global->glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, RG->ColorTexture);
  GL_Global->glUniform1i(RG->ColorTextureUniform, 0);

  GL_Global->glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, RG->NormalTexture);
  GL_Global->glUniform1i(RG->NormalTextureUniform, 1);

  GL_Global->glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, RG->PositionTexture);
  GL_Global->glUniform1i(RG->PositionTextureUniform, 2);

#if DEBUG_DRAW_SHADOW_MAP_TEXTURE
  glUseProgram(RG->SimpleTextureShaderID);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, RG->DepthTexture);
  glUniform1i(RG->SimpleTextureUniform, 0);
#endif

  RenderQuad(RG);

  return;
}

void
RenderShadowMap(World *world, ShadowRenderGroup *SG, RenderGroup *RG, Camera_Object *Camera)
{
  glViewport(0, 0, SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION);

  m4 depthMVP = GetDepthMVP(world->ChunkDim, Camera);

  GL_Global->glBindFramebuffer(GL_FRAMEBUFFER, SG->FramebufferName);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  GL_Global->glUseProgram(SG->ShaderID);
  GL_Global->glUniformMatrix4fv(SG->MVP_ID, 1, GL_FALSE, &depthMVP.E[0].E[0]);

  /* glBindTexture(GL_TEXTURE_2D, SG->Texture); */

  // 1rst attribute buffer : vertices
  GL_Global->glEnableVertexAttribArray(0);
  GL_Global->glBindBuffer(GL_ARRAY_BUFFER, RG->vertexbuffer);
  GL_Global->glBufferData(GL_ARRAY_BUFFER, world->Mesh.filled, world->Mesh.VertexData, GL_STATIC_DRAW);
  GL_Global->glVertexAttribPointer(
    0,                  // The attribute we want to configure
    3,                  // size
    GL_FLOAT,           // type
    GL_FALSE,           // normalized?
    0,                  // stride
    (void*)0            // array buffer offset
  );

  glDrawArrays(GL_TRIANGLES, 0, world->Mesh.VertexCount);

  GL_Global->glDisableVertexAttribArray(0);

  GL_Global->glBindFramebuffer(GL_FRAMEBUFFER, 0);

  return;
}

void
RenderWorld( platform *Plat, World *world, RenderGroup *RG)
{
  GL_Global->glBindFramebuffer(GL_FRAMEBUFFER, RG->FBO);

  GL_Global->glUseProgram(RG->ShaderID);
  glViewport(0, 0, Plat->WindowWidth, Plat->WindowHeight);

  m4 mvp = RG->Basis.ProjectionMatrix * RG->Basis.ViewMatrix;

  GL_Global->glUniformMatrix4fv(RG->MVPID,         1, GL_FALSE, &mvp.E[0].E[0]);
  GL_Global->glUniformMatrix4fv(RG->ModelMatrixID, 1, GL_FALSE, &RG->Basis.ModelMatrix.E[0].E[0]);

  // Vertices
  GL_Global->glEnableVertexAttribArray(0);
  GL_Global->glBindBuffer(GL_ARRAY_BUFFER, RG->vertexbuffer);
  GL_Global->glBufferData(GL_ARRAY_BUFFER, world->Mesh.filled, world->Mesh.VertexData, GL_STATIC_DRAW);
  GL_Global->glVertexAttribPointer(
    0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
    3,                  // size
    GL_FLOAT,           // type
    GL_FALSE,           // normalized?
    0,                  // stride
    (void*)0            // array buffer offset
  );

  // Colors
  GL_Global->glEnableVertexAttribArray(1);
  GL_Global->glBindBuffer(GL_ARRAY_BUFFER, RG->colorbuffer);
  GL_Global->glBufferData(GL_ARRAY_BUFFER, world->Mesh.filled, world->Mesh.ColorData, GL_STATIC_DRAW);
  GL_Global->glVertexAttribPointer(
    1,                  // attribute 1. No particular reason for 1, but must match the layout in the shader.
    3,                  // size
    GL_FLOAT,           // type
    GL_FALSE,           // normalized?
    0,                  // stride
    (void*)0            // array buffer offset
  );

  // Normals
  GL_Global->glEnableVertexAttribArray(2);
  GL_Global->glBindBuffer(GL_ARRAY_BUFFER, RG->normalbuffer);
  GL_Global->glBufferData(GL_ARRAY_BUFFER, world->Mesh.filled, world->Mesh.NormalData, GL_STATIC_DRAW);
  GL_Global->glVertexAttribPointer(
    2,
    3,                  // size
    GL_FLOAT,           // type
    GL_FALSE,           // normalized?
    0,                  // stride
    (void*)0            // array buffer offset
  );

  glDrawArrays(GL_TRIANGLES, 0, world->Mesh.VertexCount);

  GL_Global->glDisableVertexAttribArray(0);
  GL_Global->glDisableVertexAttribArray(1);
  GL_Global->glDisableVertexAttribArray(2);

}


inline void
FlushRenderBuffers(
    platform *Plat,
    World *world,
    RenderGroup *RG,
    ShadowRenderGroup *SG,
    Camera_Object *Camera
  )
{

  RenderShadowMap(world, SG, RG, Camera);

  RenderWorld(Plat, world, RG);

  AssertNoGlErrors;

  world->Mesh.VertexCount = 0;

  world->Mesh.filled = 0;

  return;
}

inline void
BufferVerts(
    mesh_buffer_target *target,

    s32 NumVerts,

    float* VertsPositions,
    float* Normals,
    const float* VertColors,

    s32 sizeofData
  )
{

  if ( target->filled + sizeofData > target->bytesAllocd )
  {
    // TODO(Jesse): Instead of Asserting, do this ?
    /* FlushRenderBuffers( Plat, world, RG, SG, Camera); */

    // Out of memory, panic!
    Assert(!"Out of memory");
    return;
  }

  target->filled += sizeofData;

  memcpy( &target->VertexData[target->VertexCount*3],  VertsPositions,  sizeofData );
  memcpy( &target->NormalData[target->VertexCount*3],  Normals,         sizeofData );
  memcpy( &target->ColorData[target->VertexCount*3],   VertColors,      sizeofData );

  target->VertexCount += NumVerts;

  return;
}

inline bool IsRightChunkBoundary( chunk_dimension ChunkDim, int idx )
{
  return (idx+1) % (int)ChunkDim.x == 0;
}

inline bool IsLeftChunkBoundary( chunk_dimension ChunkDim, int idx )
{
  return (idx) % (int)ChunkDim.x == 0;
}

inline bool IsTopChunkBoundary( chunk_dimension ChunkDim, int idx )
{
  return ((idx/(int)ChunkDim.x)+1) % (int)ChunkDim.y == 0;
}

inline bool IsBottomChunkBoundary( chunk_dimension ChunkDim, int idx )
{
  return (idx/(int)ChunkDim.x) % (int)ChunkDim.y == 0;
}

inline float
GetTheta(v3 P1, v3 P2, v3 Axis)
{
  float DotP1P2 = Dot(P1,P2);

  float LP1, LP2;

  LP1 = Length(P1);
  LP2 = Length(P2);

  Assert(LP1 != 0);
  Assert(LP2 != 0);

  float cosTheta = DotP1P2 / (LP1*LP2);
  cosTheta = ClampMinus1To1(cosTheta);
  float theta = acos( cosTheta );

  Assert(theta >= -1 || theta <= 1);
  return theta;
}

inline Quaternion
RotatePoint(v3 P1, v3 P2)
{
  P1 = Normalize(P1);
  P2 = Normalize(P2);
  v3 Axis = Normalize(Cross(P1, P2));

  float theta = GetTheta(P1, P2, Axis);

  Quaternion Result( (Axis*sin(theta/2)), cos(theta/2) );

  if (Length(Result.xyz) == 0)  // The resulting rotation was inconsequential
    Result = Quaternion();

  return Result;
}

inline void
DEBUG_DrawLine(World *world, v3 P1, v3 P2, int ColorIndex, float Thickness )
{
  // 2 verts per line, 3 floats per vert

  float localNormalData[] =
  {
     0, 0, 0,
     0, 0, 0,
     0, 0, 0,

     0, 0, 0,
     0, 0, 0,
     0, 0, 0
  };

  P1.x = P1.x - (Thickness/2.0f);
  P2.x = P2.x - (Thickness/2.0f);

  float FaceColors[32];
  GetColorData( ColorIndex, FaceColors);

  {
    float localVertexData[] =
    {
      P1.x, P1.y, P1.z,
      P2.x, P2.y, P2.z,
      P1.x + Thickness, P1.y,  P1.z,

      P2.x, P2.y, P2.z,
      P1.x + Thickness, P1.y, P1.z,
      P2.x + Thickness, P2.y, P2.z
    };


    BufferVerts(&world->Mesh,
        6,
        localVertexData,
        localNormalData,
        FaceColors,
        sizeof(localVertexData));
  }

  P1.x = P1.x + (Thickness/2.0f);
  P2.x = P2.x + (Thickness/2.0f);

  P1.y = P1.y - (Thickness/2.0f);
  P2.y = P2.y - (Thickness/2.0f);

  {
    float localVertexData[] =
    {
      P1.x, P1.y, P1.z,
      P2.x, P2.y, P2.z,
      P1.x, P1.y + Thickness,  P1.z,

      P2.x, P2.y, P2.z,
      P1.x, P1.y + Thickness, P1.z,
      P2.x, P2.y + Thickness, P2.z
    };


    BufferVerts(&world->Mesh,
        6,
        localVertexData,
        localNormalData,
        FaceColors,
        sizeof(localVertexData));
  }

#if 0
  // This is for anti-aliasing the lines; it draws extra triangles along the edges which can be set to alpha 0
  {
    float localVertexData[] =
    {
      P1.x + Thickness, P1.y + Thickness, P1.z + Thickness,
      P2.x, P2.y, P2.z,
      P2.x + Thickness, P2.y + Thickness, P2.z + Thickness,

      P2.x + Thickness, P2.y + Thickness, P2.z + Thickness,
      P1.x + Thickness, P1.y + Thickness, P1.z + Thickness,
      P1.x, P1.y, P1.z,
    };

    BufferVerts(world,
        6
        localVertexData,
        sizeof(localVertexData),
        localNormalData,
        sizeof(localVertexData),
        FaceColors);
  }
#endif

  return;
}

inline void
DEBUG_DrawVectorAt(World *world, v3 Offset, v3 Vector, int ColorIndex, float Thickness )
{
  DEBUG_DrawLine(world, Offset, Vector + Offset, ColorIndex, Thickness );
}

inline void
DEBUG_DrawLine(World *world, line Line, int ColorIndex, float Thickness )
{
  DEBUG_DrawLine(world, Line.MinP, Line.MaxP, ColorIndex, Thickness);
  return;
}

void
DEBUG_DrawAABB( World *world, v3 MinP, v3 MaxP, Quaternion Rotation, int ColorIndex, float Thickness = DEFAULT_LINE_THICKNESS )
{
  /* v3 HalfDim = (GetRenderP(world, MaxCP) - GetRenderP(world, MinCP)) / 2; */

  // Start in model space
  /* v3 MinP = HalfDim * -1; */
  /* v3 MaxP = HalfDim; */

  // Compute verticies
  v3 TopRL = V3(MinP.x, MaxP.y, MinP.z);
  v3 TopRR = V3(MaxP.x, MaxP.y, MinP.z);
  v3 TopFL = V3(MinP.x, MaxP.y, MaxP.z);
  v3 TopFR = V3(MaxP.x, MaxP.y, MaxP.z);
  v3 BotRL = V3(MinP.x, MinP.y, MinP.z);
  v3 BotRR = V3(MaxP.x, MinP.y, MinP.z);
  v3 BotFL = V3(MinP.x, MinP.y, MaxP.z);
  v3 BotFR = V3(MaxP.x, MinP.y, MaxP.z);

  /* // Apply rotation to verts */
  /* TopRL = ((Rotation * Quaternion(TopRL, 1)) * Conjugate(Rotation)).xyz; */
  /* TopRR = ((Rotation * Quaternion(TopRR, 1)) * Conjugate(Rotation)).xyz; */
  /* TopFL = ((Rotation * Quaternion(TopFL, 1)) * Conjugate(Rotation)).xyz; */
  /* TopFR = ((Rotation * Quaternion(TopFR, 1)) * Conjugate(Rotation)).xyz; */
  /* BotRL = ((Rotation * Quaternion(BotRL, 1)) * Conjugate(Rotation)).xyz; */
  /* BotRR = ((Rotation * Quaternion(BotRR, 1)) * Conjugate(Rotation)).xyz; */
  /* BotFL = ((Rotation * Quaternion(BotFL, 1)) * Conjugate(Rotation)).xyz; */
  /* BotFR = ((Rotation * Quaternion(BotFR, 1)) * Conjugate(Rotation)).xyz; */

  /* // Translate into world space */
  /* TopRL += HalfDim + MinCP.Offset; */
  /* TopRR += HalfDim + MinCP.Offset; */
  /* TopFL += HalfDim + MinCP.Offset; */
  /* TopFR += HalfDim + MinCP.Offset; */
  /* BotRL += HalfDim + MinCP.Offset; */
  /* BotRR += HalfDim + MinCP.Offset; */
  /* BotFL += HalfDim + MinCP.Offset; */
  /* BotFR += HalfDim + MinCP.Offset; */

  /* // Translate into render space */
  /* TopRL = GetRenderP(world, Canonical_Position(TopRL, MinCP.WorldP)); */
  /* TopRR = GetRenderP(world, Canonical_Position(TopRR, MinCP.WorldP)); */
  /* TopFL = GetRenderP(world, Canonical_Position(TopFL, MinCP.WorldP)); */
  /* TopFR = GetRenderP(world, Canonical_Position(TopFR, MinCP.WorldP)); */
  /* BotRL = GetRenderP(world, Canonical_Position(BotRL, MinCP.WorldP)); */
  /* BotRR = GetRenderP(world, Canonical_Position(BotRR, MinCP.WorldP)); */
  /* BotFL = GetRenderP(world, Canonical_Position(BotFL, MinCP.WorldP)); */
  /* BotFR = GetRenderP(world, Canonical_Position(BotFR, MinCP.WorldP)); */

  // Render
  //
  // Top
  DEBUG_DrawLine(world, TopRL, TopRR, ColorIndex, Thickness);
  DEBUG_DrawLine(world, TopFL, TopFR, ColorIndex, Thickness);
  DEBUG_DrawLine(world, TopFL, TopRL, ColorIndex, Thickness);
  DEBUG_DrawLine(world, TopFR, TopRR, ColorIndex, Thickness);

  // Right
  DEBUG_DrawLine(world, TopFR, BotFR, ColorIndex, Thickness);
  DEBUG_DrawLine(world, TopRR, BotRR, ColorIndex, Thickness);

  // Left
  DEBUG_DrawLine(world, TopFL, BotFL, ColorIndex, Thickness);
  DEBUG_DrawLine(world, TopRL, BotRL, ColorIndex, Thickness);

  // Bottom
  DEBUG_DrawLine(world, BotRL, BotRR, ColorIndex, Thickness);
  DEBUG_DrawLine(world, BotFL, BotFR, ColorIndex, Thickness);
  DEBUG_DrawLine(world, BotFL, BotRL, ColorIndex, Thickness);
  DEBUG_DrawLine(world, BotFR, BotRR, ColorIndex, Thickness);

  return;
}

inline void
DEBUG_DrawAABB( World *world, aabb Rect, Quaternion Rotation, int ColorIndex, float Thickness = DEFAULT_LINE_THICKNESS )
{
  v3 MinP = Rect.Center - Rect.Radius;
  v3 MaxP = Rect.Center + Rect.Radius;
  DEBUG_DrawAABB( world, MinP, MaxP, Rotation, ColorIndex, Thickness );
  return;
}

inline v3
GetModelSpaceP(chunk_data *chunk, v3 P)
{
  /* v3 HalfDim = chunk->Dim/2; */
  /* v3 Result = P - HalfDim; */

  v3 Result = P;

  return Result;
}

inline void
DEBUG_DrawChunkAABB( World *world, world_position WorldP, Camera_Object *Camera,
                     Quaternion Rotation, s32 ColorIndex , r32 Thickness = DEFAULT_LINE_THICKNESS)
{
  v3 MinP = GetRenderP(world->ChunkDim, Canonical_Position(V3(0,0,0), WorldP), Camera);
  v3 MaxP = GetRenderP(world->ChunkDim, Canonical_Position(WORLD_CHUNK_DIM, WorldP), Camera);

  DEBUG_DrawAABB(world, MinP, MaxP , Rotation, ColorIndex, Thickness);
  return;
}

inline void
DEBUG_DrawChunkAABB( World *world, world_chunk *chunk, Camera_Object *Camera,
                     Quaternion Rotation, s32 ColorIndex, r32 Thickness = DEFAULT_LINE_THICKNESS)
{
  v3 MinP = GetRenderP(world->ChunkDim, Canonical_Position(V3(0,0,0), chunk->WorldP), Camera);
  v3 MaxP = GetRenderP(world->ChunkDim, Canonical_Position(world->ChunkDim, chunk->WorldP), Camera);

  DEBUG_DrawAABB(world, MinP, MaxP , Rotation, ColorIndex, Thickness);
  return;
}

inline v3
Rotate(v3 P, Quaternion Rotation)
{
  v3 Result = ((Rotation * Quaternion(P,0)) * Conjugate(Rotation)).xyz;
  return Result;
}

inline line
Rotate(line Line, Quaternion Rotation)
{
  line Result;

  Result.MinP = Rotate(Line.MinP, Rotation);
  Result.MaxP = Rotate(Line.MaxP, Rotation);

  return Result;
}

inline void
DEBUG_DrawPointMarker( World *world, v3 RenderP, int ColorIndex, float Diameter)
{
  float FaceColors[FACE_COLOR_SIZE];
  GetColorData(ColorIndex, &FaceColors[0]);;

  v3 BasisP = RenderP - (Diameter/2);
  v3 Offset = V3(0,0,0);

  r32 Scale = 1.0f;

  /* r32 VertexData[BYTES_PER_FACE]; */

  /* BufferRightFace(  world, RenderP, FaceColors, Diameter, Scale); */
  /* BufferLeftFace(   world, RenderP, FaceColors, Diameter, Scale); */
  /* BufferBottomFace( world, RenderP, FaceColors, Diameter, Scale); */
  /* BufferTopFace(    world, RenderP, FaceColors, Diameter, Scale); */
  /* BufferFrontFace(  world, RenderP, FaceColors, Diameter, Scale); */
  /* BufferBackFace(   world, RenderP, FaceColors, Diameter, Scale); */

  return;
}

inline float
DistanceToPlane(plane *Plane, v3 P)
{
  float x = Plane->P.x;
  float y = Plane->P.y;
  float z = Plane->P.z;

  float a = Plane->Normal.x;
  float b = Plane->Normal.y;
  float c = Plane->Normal.z;

  float d = Plane->d;
  Assert(a*x + b*y + c*z + d == 0);

  float Distance = a*P.x + b*P.y + c*P.z + d;
  return Distance;
}

inline bool
IsInFrustum(chunk_dimension WorldChunkDim, Camera_Object *Camera, canonical_position P)
{
  bool Result = true;

  v3 TestP = GetRenderP(WorldChunkDim, P, Camera);

  Result &= (DistanceToPlane(&Camera->Frust.Top, TestP)   > -1*WorldChunkDim.y);
  Result &= (DistanceToPlane(&Camera->Frust.Bot, TestP)   > -1*WorldChunkDim.y);
  Result &= (DistanceToPlane(&Camera->Frust.Left, TestP)  > -1*WorldChunkDim.x);
  Result &= (DistanceToPlane(&Camera->Frust.Right, TestP) > -1*WorldChunkDim.x);

  return Result;
}

inline voxel_position
Clamp01( voxel_position V )
{
  voxel_position Result = V;
  if (Result.x < 0) Result.x = 0;
  if (Result.y < 0) Result.y = 0;
  if (Result.z < 0) Result.z = 0;

  if (Result.x > 1) Result.x = 1;
  if (Result.y > 1) Result.y = 1;
  if (Result.z > 1) Result.z = 1;

  return Result;
}

inline void
PushBoundaryVoxel( chunk_data *chunk, packed_voxel *Voxel, chunk_dimension Dim)
{
  Assert( chunk->BoundaryVoxelCount < Volume(Dim) );

  chunk->BoundaryVoxels[chunk->BoundaryVoxelCount] = *Voxel;
  chunk->BoundaryVoxelCount++;
}

void
BuildExteriorBoundaryVoxels( World *world, world_chunk *chunk, chunk_dimension Dim, world_chunk *Neighbor, voxel_position NeighborVector )
{
  voxel_position nvSq = (NeighborVector*NeighborVector);

  voxel_position AbsInvNeighborVector = ((nvSq-1)*(nvSq-1));

  voxel_position LocalPlane = ClampPositive(Dim-1) * AbsInvNeighborVector + 1;

  voxel_position LocalOffset = ClampPositive(Dim*NeighborVector - nvSq);

  voxel_position Start = Voxel_Position(0,0,0);

  for ( int z = Start.z; z < LocalPlane.z; ++z )
  {
    for ( int y = Start.y; y < LocalPlane.y; ++y )
    {
      for ( int x = Start.x; x < LocalPlane.x; ++x )
      {
        voxel_position LocalVoxelP = Voxel_Position(x+LocalOffset.x, y+LocalOffset.y, z+LocalOffset.z);

        if ( chunk && !IsFilledInChunk(chunk->Data, LocalVoxelP, Dim) )
          continue;

        voxel_position NeighborP = ClampPositive(
            (LocalVoxelP - (Dim * NeighborVector))
            - nvSq);

        if ( Neighbor && !IsFilledInChunk(Neighbor->Data, NeighborP, Dim) )
        {
          packed_voxel *Voxel = &chunk->Data->Voxels[GetIndex(LocalVoxelP, chunk->Data, Dim)];

          if (NeighborVector.x > 0)
            Voxel->Data = SetFlag(Voxel->Data, Voxel_RightFace);

          if (NeighborVector.x < 0)
            Voxel->Data = SetFlag(Voxel->Data, Voxel_LeftFace);

          if (NeighborVector.y > 0)
            Voxel->Data = SetFlag(Voxel->Data, Voxel_TopFace);

          if (NeighborVector.y < 0)
            Voxel->Data = SetFlag(Voxel->Data, Voxel_BottomFace);

          if (NeighborVector.z > 0)
            Voxel->Data = SetFlag(Voxel->Data, Voxel_FrontFace);

          if (NeighborVector.z < 0)
            Voxel->Data = SetFlag(Voxel->Data, Voxel_BackFace);

          voxel_position P = GetVoxelP(Voxel);
          Assert( P == LocalVoxelP);
          PushBoundaryVoxel( chunk->Data, Voxel, Dim);

        }
      }
    }
  }


  return;
}

inline b32
IsInsideDim( voxel_position Dim, voxel_position P )
{
  b32 Result = False;

  Result = (
              P.x >= 0 &&
              P.y >= 0 &&
              P.z >= 0 &&

              P.x < Dim.x &&
              P.y < Dim.y &&
              P.z < Dim.z
           );

  return Result;
}

inline b32
IsInsideDim( voxel_position Dim, v3 P )
{
  b32 Result = IsInsideDim(Dim, Voxel_Position(P) );
  return Result;
}

void
BuildEntityBoundaryVoxels(chunk_data *chunk, world_position WorldP, chunk_dimension Dim)
{
  chunk->flags = UnSetFlag( chunk->flags, Chunk_RebuildBoundary );

  for ( int z = 0; z < Dim.z ; ++z )
  {
    for ( int y = 0; y < Dim.y ; ++y )
    {
      for ( int x = 0; x < Dim.x ; ++x )
      {
        voxel_position VoxelP = Voxel_Position(x,y,z);

        if ( NotFilled( chunk, VoxelP, Dim) )
          continue;

        voxel_position rightVoxel = VoxelP + Voxel_Position(1, 0, 0);
        voxel_position leftVoxel = VoxelP - Voxel_Position(1, 0, 0);

        voxel_position topVoxel = VoxelP + Voxel_Position(0, 1, 0);
        voxel_position botVoxel = VoxelP - Voxel_Position(0, 1, 0);

        voxel_position frontVoxel = VoxelP + Voxel_Position(0, 0, 1);
        voxel_position backVoxel = VoxelP - Voxel_Position(0, 0, 1);

        packed_voxel *Voxel = &chunk->Voxels[GetIndex(Voxel_Position(x,y,z), chunk, Dim)];

        bool DidPushVoxel = false;

        if ( IsInsideDim( Dim, rightVoxel  ) && NotFilled( chunk, rightVoxel, Dim))
        {
          Voxel->Data = SetFlag(Voxel->Data, Voxel_RightFace);
          DidPushVoxel = true;
        }
        if ( IsInsideDim( Dim, leftVoxel  ) && NotFilled( chunk, leftVoxel, Dim))
        {
          Voxel->Data = SetFlag(Voxel->Data, Voxel_LeftFace);
          DidPushVoxel = true;
        }
        if ( IsInsideDim( Dim, botVoxel   ) && NotFilled( chunk, botVoxel, Dim))
        {
          Voxel->Data = SetFlag(Voxel->Data, Voxel_BottomFace);
          DidPushVoxel = true;
        }
        if ( IsInsideDim( Dim, topVoxel   ) && NotFilled( chunk, topVoxel, Dim))
        {
          Voxel->Data = SetFlag(Voxel->Data, Voxel_TopFace);
          DidPushVoxel = true;
        }
        if ( IsInsideDim( Dim, frontVoxel ) && NotFilled( chunk, frontVoxel, Dim))
        {
          Voxel->Data = SetFlag(Voxel->Data, Voxel_FrontFace);
          DidPushVoxel = true;
        }
        if ( IsInsideDim( Dim, backVoxel  ) && NotFilled( chunk, backVoxel, Dim))
        {
          Voxel->Data = SetFlag(Voxel->Data, Voxel_BackFace);
          DidPushVoxel = true;
        }

        if (DidPushVoxel)
        {
          voxel_position P = GetVoxelP(Voxel);
          Assert( P == Voxel_Position(x,y,z));
          PushBoundaryVoxel(chunk, Voxel, Dim);
        }

      }
    }
  }
}

void
BuildWorldChunkBoundaryVoxels(World *world, world_chunk *WorldChunk)
{
  chunk_data *chunk = WorldChunk->Data;
  chunk_dimension Dim = world->ChunkDim;

  chunk->BoundaryVoxelCount = 0;

  chunk->flags = UnSetFlag( chunk->flags, Chunk_RebuildBoundary );

  for ( int z = 0; z < Dim.z ; ++z )
  {
    for ( int y = 0; y < Dim.y ; ++y )
    {
      for ( int x = 0; x < Dim.x ; ++x )
      {
        canonical_position CurrentP  = Canonical_Position(world->ChunkDim, V3(x,y,z), WorldChunk->WorldP);

        if ( NotFilledInWorld( world, WorldChunk, CurrentP ) )
          continue;

        canonical_position rightVoxel = Canonicalize(Dim, CurrentP + V3(1, 0, 0));
        canonical_position leftVoxel  = Canonicalize(Dim, CurrentP - V3(1, 0, 0));

        canonical_position topVoxel   = Canonicalize(Dim, CurrentP + V3(0, 1, 0));
        canonical_position botVoxel   = Canonicalize(Dim, CurrentP - V3(0, 1, 0));

        canonical_position frontVoxel = Canonicalize(Dim, CurrentP + V3(0, 0, 1));
        canonical_position backVoxel  = Canonicalize(Dim, CurrentP - V3(0, 0, 1));

        packed_voxel *Voxel = &chunk->Voxels[GetIndex(CurrentP.Offset, chunk, Dim)];

        bool DidPushVoxel = false;

        if ( NotFilledInWorld( world, WorldChunk, rightVoxel ) )
        {
          Voxel->Data = SetFlag(Voxel->Data, Voxel_RightFace);
          DidPushVoxel = true;
        }
        if ( NotFilledInWorld( world, WorldChunk, leftVoxel ) )
        {
          Voxel->Data = SetFlag(Voxel->Data, Voxel_LeftFace);
          DidPushVoxel = true;
        }
        if ( NotFilledInWorld( world, WorldChunk, botVoxel   ) )
        {
          Voxel->Data = SetFlag(Voxel->Data, Voxel_BottomFace);
          DidPushVoxel = true;
        }
        if ( NotFilledInWorld( world, WorldChunk, topVoxel   ) )
        {
          Voxel->Data = SetFlag(Voxel->Data, Voxel_TopFace);
          DidPushVoxel = true;
        }
        if ( NotFilledInWorld( world, WorldChunk, frontVoxel ) )
        {
          Voxel->Data = SetFlag(Voxel->Data, Voxel_FrontFace);
          DidPushVoxel = true;
        }
        if ( NotFilledInWorld( world, WorldChunk, backVoxel  ) )
        {
          Voxel->Data = SetFlag(Voxel->Data, Voxel_BackFace);
          DidPushVoxel = true;
        }

        if (DidPushVoxel)
        {
          voxel_position P = GetVoxelP(Voxel);
          Assert( P == Voxel_Position(CurrentP.Offset));
          PushBoundaryVoxel(chunk, Voxel, Dim);
        }

      }
    }
  }
}

inline bool
IsInFrustum( chunk_dimension WorldChunkDim, Camera_Object *Camera, world_chunk *Chunk )
{
  v3 ChunkMid = V3(CD_X, CD_Y, CD_Z)/2;
  canonical_position P1 = Canonical_Position(  ChunkMid, Chunk->WorldP );
  bool Result = IsInFrustum(WorldChunkDim, Camera, P1 );
  return Result;
}

inline void
ClearFramebuffers(RenderGroup *RG, ShadowRenderGroup *SG)
{
  /* TIMED_FUNCTION(); */
  glClearColor(0.25f, 0.25f, 0.25f, 0.0f);

  // FIXME(Jesse): This is taking _forever_ on Linux (GLES) .. does it take
  // forever on other Linux systems?
  GL_Global->glBindFramebuffer(GL_FRAMEBUFFER, RG->FBO);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  GL_Global->glBindFramebuffer(GL_FRAMEBUFFER, SG->FramebufferName);
  glClear(GL_DEPTH_BUFFER_BIT);

  GL_Global->glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  return;
}

void
BufferChunkMesh(
    platform *Plat,
    World *world,
    chunk_data *chunk,
    world_position WorldP,
    RenderGroup *RG,
    ShadowRenderGroup *SG,
    Camera_Object *Camera,
    r32 Scale,
    v3 Offset = V3(0,0,0)
  )
{
  r32 FaceColors[FACE_COLOR_SIZE];

  v3 ModelBasisP =
    GetRenderP( world->ChunkDim, Canonical_Position(Offset, WorldP), Camera);

  for (int VoxIndex = 0;
       VoxIndex < chunk->BoundaryVoxelCount;
       ++VoxIndex )
  {
    r32 VertexData[BYTES_PER_FACE];
    VoxelsIndexed ++;

    packed_voxel V = chunk->BoundaryVoxels[VoxIndex];

    GetColorData(GetVoxelColor(&V), &FaceColors[0]);;

    r32 Diameter = 1.0f*Scale;

    v3 VoxelOffset = V3(GetVoxelP(&V)) * Scale;
    v3 MinP = ModelBasisP + VoxelOffset ;

    if ( IsSet( V.Data, Voxel_RightFace ) )
    {
      RightFaceVertexData( MinP, Diameter, VertexData);
      BufferVerts(&world->Mesh, 6, VertexData, RightFaceNormalData, FaceColors, sizeof(VertexData));
    }

    if ( IsSet( V.Data, Voxel_LeftFace ) )
    {
      LeftFaceVertexData( MinP, Diameter, VertexData);
      BufferVerts(&world->Mesh, 6, VertexData, LeftFaceNormalData, FaceColors, sizeof(VertexData));
    }

    if ( IsSet( V.Data, Voxel_BottomFace ) )
    {
      BottomFaceVertexData( MinP, Diameter, VertexData);
      BufferVerts(&world->Mesh, 6, VertexData, BottomFaceNormalData, FaceColors, sizeof(VertexData));
    }

    if ( IsSet( V.Data, Voxel_TopFace ) )
    {
      TopFaceVertexData( MinP, Diameter, VertexData);
      BufferVerts(&world->Mesh, 6, VertexData, TopFaceNormalData, FaceColors, sizeof(VertexData));
    }

    if ( IsSet( V.Data, Voxel_FrontFace ) )
    {
      FrontFaceVertexData( MinP, Diameter, VertexData);
      BufferVerts(&world->Mesh, 6, VertexData, FrontFaceNormalData, FaceColors, sizeof(VertexData));
    }

    if ( IsSet( V.Data, Voxel_BackFace ) )
    {
      BackFaceVertexData( MinP, Diameter, VertexData);
      BufferVerts(&world->Mesh, 6, VertexData, BackFaceNormalData, FaceColors, sizeof(VertexData));
    }

  }


  return;
}

line
FindIntersectingLine(
  game_state *GameState,
  world_chunk *Chunk,
  voxel_position OffsetVector,
  int FirstFilledIndex)
{
  voxel_position MinP = GetVoxelP(&Chunk->Data->BoundaryVoxels[FirstFilledIndex]);
  voxel_position MaxP = GetVoxelP(&Chunk->Data->BoundaryVoxels[FirstFilledIndex]);

  int CurrentMaxLen = 0;
  int CurrentMinLen = 0;

  for (int VoxelIndex = FirstFilledIndex;
      VoxelIndex < Chunk->Data->BoundaryVoxelCount;
      ++ VoxelIndex)
  {
    packed_voxel V = Chunk->Data->BoundaryVoxels[VoxelIndex];
    voxel_position P  = GetVoxelP(&V);

    int OriginToP = LengthSq(P);

    // Max
    if ( OriginToP > CurrentMaxLen )
    {
      MaxP = P + 1;
      CurrentMaxLen = OriginToP;
    }

    // Min
    if ( OriginToP < CurrentMinLen )
    {
      MinP = P;
      CurrentMinLen = OriginToP;
    }

  }

  line Result(MinP, MaxP);

  return Result;
}

inline void
SetupAndBuildExteriorBoundary(
  World *world,
  world_chunk *Chunk,
  voxel_position OffsetVector,
  chunk_flag Flag)
{
  if ( IsSet(Chunk->Data->flags, Flag ) )
  {
    world_chunk *Neighbor = GetWorldChunk( world, Chunk->WorldP + OffsetVector );

    if ( Neighbor && IsSet( Neighbor->Data->flags, Chunk_Initialized) )
    {
      Chunk->Data->flags = UnSetFlag( Chunk->Data->flags, Flag );
      BuildExteriorBoundaryVoxels( world, Chunk, world->ChunkDim, Neighbor, OffsetVector);
    }
  }

}

aabb
FindBoundaryVoxelsAABB(chunk_data *Chunk, chunk_dimension Dim)
{

  chunk_dimension MinP = Dim;
  chunk_dimension MaxP = {};

  for ( s32 VoxelIndex = 0;
        VoxelIndex < Chunk->BoundaryVoxelCount;
        ++VoxelIndex)
  {
    voxel_position P = GetVoxelP(&Chunk->BoundaryVoxels[VoxelIndex]);

    if (P.x < MinP.x)
      MinP.x = P.x;
    if (P.x > MaxP.x)
      MaxP.x = P.x;

    if (P.y < MinP.y)
      MinP.y = P.y;
    if (P.y > MaxP.y)
      MaxP.y = P.y;

    if (P.z < MinP.z)
      MinP.z = P.z;
    if (P.z > MaxP.z)
      MaxP.z = P.z;
  }

  v3 Center = V3(MaxP - MinP);
  v3 Radius = Center/2;

  return aabb( Center, Radius );
}

inline v3
GetSign(v3 P)
{
  v3 Result =
    V3( GetSign(P.x), GetSign(P.y), GetSign(P.z));

  return Result;
}

inline voxel_position
GetSign(voxel_position P)
{
  voxel_position Result =
    Voxel_Position( GetSign(P.x), GetSign(P.y), GetSign(P.z));

  return Result;
}

voxel_position
RayTraceCollision(chunk_data *Chunk, chunk_dimension Dim, v3 StartingP, v3 Ray, v3 CenteringRay)
{
  Assert(LengthSq(Ray) == 1);
  v3 Result = V3(-1,-1,-1);

  // Clamp magnitude of this ray to 1 in each axis
  CenteringRay = GetSign(CenteringRay);

  v3 CurrentP = StartingP;
  while ( IsInsideDim(Dim, CurrentP) )
  {
    v3 CachedP = CurrentP;

    while ( IsInsideDim(Dim, CurrentP) )
    {
      if ( IsFilledInChunk(Chunk, Voxel_Position(CurrentP), Dim) )
      {
        Result = CurrentP;
        goto finished;
      }

      CurrentP += Ray;
    }

    CurrentP = CachedP;
    CurrentP += CenteringRay;
  }

  finished:

  return Voxel_Position(Result);
}

inline void
BufferTriangle(World *world, v3 *Verts, v3 Normal, s32 ColorIndex)
{
  r32 VertBuffer[9];
  v3 NormalBuffer[3] = {Normal, Normal, Normal};

  // TODO(Jesse): Is this necessary to avoid some pointer aliasing bug?
  memcpy( VertBuffer, Verts, 9 * sizeof(r32) );

  float FaceColors[32];
  GetColorData( ColorIndex, FaceColors);

  BufferVerts(
    &world->Mesh,
    3,
    VertBuffer,
    (float*)&NormalBuffer[0],
    FaceColors,
    sizeof(VertBuffer));

}

inline void
FindBoundaryVoxelsAlongEdge(chunk_data *Data, chunk_dimension Dim, voxel_position Start, voxel_position Iter, point_buffer *PB)
{
  voxel_position CurrentP = Start;
  b32 StartIsFilled = IsFilledInChunk(Data, CurrentP, Dim);

  Assert(Length(V3(Iter)) == 1.0f);

  while ( IsInsideDim(Dim, CurrentP) )
  {

    b32 CurrentPIsFilled = IsFilledInChunk(Data, CurrentP, Dim);
    if (CurrentPIsFilled != StartIsFilled)
    {
      Assert(PB->Count < POINT_BUFFER_SIZE);
      PB->Points[PB->Count++] = CurrentP;
      StartIsFilled = CurrentPIsFilled;
    }

    CurrentP += Iter;
  }

  return;
}

void
Compute0thLod(game_state *GameState, world_chunk *WorldChunk)
{
  World *world = GameState->world;
  chunk_dimension WorldChunkDim = world->ChunkDim;

  v3 RenderOffset = GetRenderP( WorldChunkDim, WorldChunk->WorldP, GameState->Camera);

  v3 SurfaceNormal = {};
  v3 ChunkMidpoint = WorldChunkDim/2.0f;
  /* DEBUG_DrawPointMarker(world, ChunkMidpoint + RenderOffset, GREEN, 0.5f); */

  s32 WorldChunkVolume = Volume(WorldChunkDim);


  b32 HalfFull = WorldChunk->Filled >= WorldChunkVolume/2 ? True : False ;
  b32 HalfEmpty = !HalfFull;


  // Loop through the chunk and find the surface normal
  for ( s32 VoxelIndex = 0;
      VoxelIndex < WorldChunkVolume;
      ++VoxelIndex)
  {
    voxel_position VoxelP = GetVoxelP(WorldChunkDim, VoxelIndex);

    /* if (IsBoundaryVoxel(chunk, VoxelP, WorldChunkDim)) */
    /*   DEBUG_DrawPointMarker(world, V3(VoxelP) + RenderOffset, PINK, 0.25f); */

    // TODO(Jesse): Pretty sure we can do some XOR trickery or something
    // here to avoid this branch, which could be a large perf win
    if ( HalfFull && NotFilledInChunk( WorldChunk->Data, VoxelP, WorldChunkDim ) )
    {
      SurfaceNormal += Normalize( VoxelP - ChunkMidpoint );
    }
    else if ( HalfEmpty && IsFilledInChunk( WorldChunk->Data, VoxelP, WorldChunkDim ) )
    {
      SurfaceNormal += Normalize( VoxelP - ChunkMidpoint );
    }
  }

  if (HalfEmpty)
    SurfaceNormal *= -1.0f;

  // FIXME(Jesse): Sometimes the surface is perfectly balanced within the
  // chunk, in which case the normal turns out to be zero.
  SurfaceNormal = Normalize(SurfaceNormal);
  if ( Length(SurfaceNormal) == 0 )
  {
    DEBUG_DrawChunkAABB( world, WorldChunk, GameState->Camera, Quaternion() , RED );
    return;
  }

  WorldChunk->Normal = SurfaceNormal;

  /* DEBUG_DrawVectorAt(world, RenderOffset + ChunkMidpoint - (SurfaceNormal*10), SurfaceNormal*20, BLACK, 0.5f ); */

  point_buffer *PB = &WorldChunk->PB;

  {
    voxel_position Start = Voxel_Position(0, 0, 0);

    {
      voxel_position Iter  = Voxel_Position(1, 0, 0);
      FindBoundaryVoxelsAlongEdge(WorldChunk->Data, WorldChunkDim, Start, Iter, PB);
    }
    {
      voxel_position Iter  = Voxel_Position(0, 1, 0);
      FindBoundaryVoxelsAlongEdge(WorldChunk->Data, WorldChunkDim, Start, Iter, PB);
    }
    {
      voxel_position Iter  = Voxel_Position(0, 0, 1);
      FindBoundaryVoxelsAlongEdge(WorldChunk->Data, WorldChunkDim, Start, Iter, PB);
    }
  }

  {
    voxel_position Start = Voxel_Position(0, WorldChunkDim.y-1, WorldChunkDim.z-1);

    {
      voxel_position Iter  = Voxel_Position(1, 0, 0);
      FindBoundaryVoxelsAlongEdge(WorldChunk->Data, WorldChunkDim, Start, Iter, PB);
    }
    {
      voxel_position Iter  = Voxel_Position(0,-1, 0);
      FindBoundaryVoxelsAlongEdge(WorldChunk->Data, WorldChunkDim, Start, Iter, PB);
    }
    {
      voxel_position Iter  = Voxel_Position(0, 0,-1);
      FindBoundaryVoxelsAlongEdge(WorldChunk->Data, WorldChunkDim, Start, Iter, PB);
    }
  }

  {
    voxel_position Start = Voxel_Position(WorldChunkDim.x-1, WorldChunkDim.y-1, 0);

    {
      voxel_position Iter  = Voxel_Position(-1, 0, 0);
      FindBoundaryVoxelsAlongEdge(WorldChunk->Data, WorldChunkDim, Start, Iter, PB);
    }
    {
      voxel_position Iter  = Voxel_Position(0,-1, 0);
      FindBoundaryVoxelsAlongEdge(WorldChunk->Data, WorldChunkDim, Start, Iter, PB);
    }
    {
      voxel_position Iter  = Voxel_Position(0, 0, 1);
      FindBoundaryVoxelsAlongEdge(WorldChunk->Data, WorldChunkDim, Start, Iter, PB);
    }
  }

  {
    voxel_position Start = Voxel_Position(WorldChunkDim.x-1, 0, WorldChunkDim.z-1);

    {
      voxel_position Iter  = Voxel_Position(-1, 0, 0);
      FindBoundaryVoxelsAlongEdge(WorldChunk->Data, WorldChunkDim, Start, Iter, PB);
    }
    {
      voxel_position Iter  = Voxel_Position(0, 1, 0);
      FindBoundaryVoxelsAlongEdge(WorldChunk->Data, WorldChunkDim, Start, Iter, PB);
    }
    {
      voxel_position Iter  = Voxel_Position(0, 0,-1);
      FindBoundaryVoxelsAlongEdge(WorldChunk->Data, WorldChunkDim, Start, Iter, PB);
    }
  }



  // Sort the vertices based on distance to closest points
  for (s32 PBIndexOuter = 0;
      PBIndexOuter < PB->Count;
      ++PBIndexOuter)
  {
    voxel_position CurrentVert = PB->Points[PBIndexOuter];
    s32 ShortestLength = INT_MAX;

    // Loop through remaining points and find next closest point, collapsing
    // points that are very close to each other
    for (s32 PBIndexInner = PBIndexOuter + 1;
        PBIndexInner < PB->Count;
        ++PBIndexInner)
    {
      s32 TestLength = LengthSq(CurrentVert - PB->Points[PBIndexInner]);

      /* if ( TestLength < 11 ) */
      /* { */
      /*   PB->Points[PBIndexOuter] = PB->Points[PB->Count-1]; */
      /*   PBIndexInner--; */
      /*   PB->Count--; */
      /*   continue; */
      /* } */

      if ( TestLength < ShortestLength )
      {
        ShortestLength = TestLength;

        voxel_position Temp = PB->Points[PBIndexOuter + 1];
        PB->Points[PBIndexOuter + 1] = PB->Points[PBIndexInner];
        PB->Points[PBIndexInner] = Temp;
      }
    }

  }

    /* if (PB->Count == 5) */
#if 0
    b32 FoundMidpoint = false;
    v3 SurfaceMid = {};

    //
    // Raytrace along the surface normal starting at the BoundaryVoxels AABB Midpoint
    // to find the surfaces mid point
    //
    aabb BoundaryVoxelsAABB = FindBoundaryVoxelsAABB(WorldChunk->Data, WorldChunkDim);
    v3 MidAABB = Midpoint(BoundaryVoxelsAABB);
    b32 MidpointIsFilled = IsFilledInChunk(WorldChunk->Data, Voxel_Position(MidAABB), WorldChunkDim);

    {
      v3 CurrentP = MidAABB;
      while( IsInsideDim(WorldChunkDim, CurrentP) )
      {
        DEBUG_DrawPointMarker(world, CurrentP + RenderOffset, PINK, 0.25f);

        b32 CurrentPIsFilled = IsFilledInChunk(WorldChunk->Data, Voxel_Position(CurrentP), WorldChunkDim);
        if (CurrentPIsFilled != MidpointIsFilled)
        {
          FoundMidpoint = true;
          SurfaceMid = CurrentPIsFilled ? CurrentP : CurrentP - SurfaceNormal;
          break;
        }
        CurrentP += SurfaceNormal;
      }
    }


    {
      if ( !FoundMidpoint )
      {
        v3 CurrentP = MidAABB;
        while( IsInsideDim(WorldChunkDim, CurrentP) )
        {
          DEBUG_DrawPointMarker(world, CurrentP + RenderOffset, PINK, 0.25f);

          b32 CurrentPIsFilled = IsFilledInChunk(WorldChunk->Data, Voxel_Position(CurrentP), WorldChunkDim);
          if (CurrentPIsFilled != MidpointIsFilled)
          {
            FoundMidpoint = true;
            SurfaceMid = CurrentPIsFilled ? CurrentP : CurrentP + SurfaceNormal;
            break;
          }
          CurrentP -= SurfaceNormal;
        }
      }
    }

    // FIXME(Jesse): Why would we fail to find a surface midpoint pray tell?
    if ( !FoundMidpoint )
    {
      DEBUG_DrawChunkAABB( world, WorldChunk, GameState->Camera, Quaternion() , RED );
    }
    else
    {
      v3 Front = SurfaceNormal;
      v3 Right = Cross( WORLD_Y, Front );
      v3 Up = Cross( Front, Right );

      DEBUG_DrawVectorAt(world, RenderOffset + SurfaceMid, Front*10, RED, 0.5f );
      DEBUG_DrawVectorAt(world, RenderOffset + SurfaceMid, Right*10, TEAL, 0.5f );
      DEBUG_DrawVectorAt(world, RenderOffset + SurfaceMid, Up*10, GREEN, 0.5f );

      v3 MidpointRenderP = RenderOffset + SurfaceMid;

      DEBUG_DrawPointMarker(world, MidpointRenderP, GREEN, 0.5f);

      {

        v3 UpRight = Normalize(Up + Right);
        v3 UpLeft = Normalize(Up - Right);

        v3 DownRight = Normalize(Right - Up);
        v3 DownLeft = Normalize((Up + Right) * -1.0f);

        v3 VertMaxMax = SurfaceMid;
        while ( IsInsideDim(WorldChunkDim, VertMaxMax) ) VertMaxMax += UpRight;
        VertMaxMax -= UpRight;

        v3 VertMaxMin = SurfaceMid;
        while ( IsInsideDim(WorldChunkDim, VertMaxMin) ) VertMaxMin += UpLeft;
        VertMaxMin -= UpLeft;

        v3 VertMinMax = SurfaceMid;
        while ( IsInsideDim(WorldChunkDim, VertMinMax) ) VertMinMax += DownRight;
        VertMinMax -= DownRight;

        v3 VertMinMin = SurfaceMid;
        while ( IsInsideDim(WorldChunkDim, VertMinMin) ) VertMinMin += DownLeft;
        VertMinMin -= DownLeft;

        v3 Verts[3];

        Verts[0] = VertMaxMax + RenderOffset;
        Verts[1] = VertMaxMin + RenderOffset;
        Verts[2] = VertMinMin + RenderOffset;
        BufferTriangle(world, &Verts[0], SurfaceNormal, 42);

        Verts[0] = VertMinMin + RenderOffset;
        Verts[1] = VertMinMax + RenderOffset;
        Verts[2] = VertMaxMax + RenderOffset;
        BufferTriangle(world, &Verts[0], SurfaceNormal, 42);

      }
    }

#endif

#if 0
    aabb BoundaryVoxelsAABB = FindBoundaryVoxelsAABB(WorldChunk->Data, WorldChunkDim);

    float xLen = BoundaryVoxelsAABB.Max.x - BoundaryVoxelsAABB.Min.x;
    float yLen = BoundaryVoxelsAABB.Max.y - BoundaryVoxelsAABB.Min.y;
    float zLen = BoundaryVoxelsAABB.Max.z - BoundaryVoxelsAABB.Min.z;

    // Find the widest axies and iterate orthogonal to them

    voxel_position IterAxis = {}; // Axis to iterate along

    // X Axis is widest
    if (xLen >= yLen && xLen >= zLen)
    {
      // Y is next widest
      if (yLen > zLen)
      {
        IterAxis.z = 1;
      }
      else // Z is next widest
      {
        IterAxis.y = 1;
      }
    }

    // Y axis is widest
    else if (yLen >= xLen && yLen >= zLen)
    {
      // x is next widest
      if (xLen > zLen)
      {
        IterAxis.z = 1;
      }
      else // z is next widest
      {
        IterAxis.x = 1;
      }
    }

    // Z is widest
    else if (zLen >= yLen && zLen >= xLen)
    {
      // X is next widest
      if (xLen > yLen)
      {
        IterAxis.y = 1;
      }
      else // Y is next widest
      {
        IterAxis.x = 1;
      }
    }
    else // Use an arbitrary axis
    {
      if (xLen > yLen)
      {
        IterAxis.y = 1;
      }
      else
      {
        IterAxis.x = 1;
      }
    }

    /* Assert(Length(IterAxis) == 1); */

    if ( GameState->Player->P.WorldP == WorldChunk->WorldP)
      DEBUG_DrawAABB( world, BoundaryVoxelsAABB + RenderOffset, Quaternion(), RED, 0.25f );

    //
    // Start iterating along the axis orthogonal to the widest axies
    voxel_position MaxP = IterAxis * world->ChunkDim;


    s32 AxisMax = max(MaxP.x, MaxP.y);
    AxisMax = max(AxisMax, MaxP.z);


    // Vector pointing towards the position to iterate from
    voxel_position MaxAxies = IterAxis ^ Voxel_Position(1,1,1);


    voxel_position StartingP = Voxel_Position(BoundaryVoxelsAABB.MinCorner + (((BoundaryVoxelsAABB.MaxCorner-BoundaryVoxelsAABB.MinCorner)*MaxAxies)/2));

    s32 MaxIterations = (s32)Length( (BoundaryVoxelsAABB.MaxCorner*IterAxis) - (BoundaryVoxelsAABB.MinCorner*IterAxis) );

    voxel_position CurrentP = StartingP;

    b32 FoundMidpoint = False;

    for (s32 Iterations = 0;
        Iterations < MaxIterations;
        ++Iterations)
    {

      if ( GameState->Player->P.WorldP == WorldChunk->WorldP)
        DEBUG_DrawPointMarker(world, V3(CurrentP + RenderOffset), BLUE, 0.25f);

      if (IsBoundaryVoxel(chunk, CurrentP, world->ChunkDim))
      {
        DEBUG_DrawPointMarker(world, V3(CurrentP + RenderOffset), GREEN, 1.2f);
        FoundMidpoint = True;
        break;
      }

      CurrentP += IterAxis;
    }

    if (FoundMidpoint)
    {
      voxel_position BoundaryPosition =
        TraverseSurfaceToBoundary(world, chunk, CurrentP, Voxel_Position(-1, 0, 0));
      voxel_position BoundaryPosition2 =
        TraverseSurfaceToBoundary(world, chunk, CurrentP, Voxel_Position(1, 0, 0));

      DEBUG_DrawPointMarker(world, V3(BoundaryPosition + RenderOffset)+0.1f, RED, 1.2f);
      DEBUG_DrawPointMarker(world, V3(BoundaryPosition2 + RenderOffset)-0.1f, RED, 1.2f);



      BoundaryPosition =
        TraverseSurfaceToBoundary(world, chunk, CurrentP, Voxel_Position(0, 0, -1));
      BoundaryPosition2 =
        TraverseSurfaceToBoundary(world, chunk, CurrentP, Voxel_Position(0, 0, 1));

      DEBUG_DrawPointMarker(world, V3(BoundaryPosition + RenderOffset)+0.1f, TEAL, 1.2f);
      DEBUG_DrawPointMarker(world, V3(BoundaryPosition2 + RenderOffset)-0.1f, TEAL, 1.2f);



      BoundaryPosition =
        TraverseSurfaceToBoundary(world, chunk, CurrentP, Voxel_Position(0, -1, 0));
      BoundaryPosition2 =
        TraverseSurfaceToBoundary(world, chunk, CurrentP, Voxel_Position(0, 1, 0));

      DEBUG_DrawPointMarker(world, V3(BoundaryPosition + RenderOffset), GREEN, 1.2f);
      DEBUG_DrawPointMarker(world, V3(BoundaryPosition2 + RenderOffset), GREEN, 1.2f);


    }


    v3 MaxMaxStart = BoundaryVoxelsAABB.MaxCorner;

    v3 MaxMinStart = BoundaryVoxelsAABB.MaxCorner;
    MaxMinStart.x = BoundaryVoxelsAABB.MinCorner.x;

    v3 MinMaxStart = BoundaryVoxelsAABB.MaxCorner;
    MinMaxStart.z = BoundaryVoxelsAABB.MinCorner.z;

    v3 MinMinStart = BoundaryVoxelsAABB.MinCorner;
    MinMinStart.y = BoundaryVoxelsAABB.MaxCorner.y;


    voxel_position MaxMax = RayTraceCollision( chunk,
                                               WorldChunkDim,
                                               MaxMaxStart,
                                               V3(0,-1,0),
                                               MinMinStart - MaxMaxStart );

    voxel_position MaxMin = RayTraceCollision( chunk,
                                               WorldChunkDim,
                                               MaxMinStart,
                                               V3(0,-1,0),
                                               MinMaxStart - MaxMinStart );

    voxel_position MinMax = RayTraceCollision( chunk,
                                               WorldChunkDim,
                                               MinMaxStart,
                                               V3(0,-1,0),
                                               MaxMinStart - MinMaxStart );

    voxel_position MinMin = RayTraceCollision( chunk,
                                               WorldChunkDim,
                                               MinMinStart,
                                               V3(0,-1,0),
                                               MaxMaxStart - MinMinStart );


    v3 VertMaxMax = V3(MaxMax + RenderOffset + GetSign(MaxMax) );
    v3 VertMaxMin = V3(MaxMin + RenderOffset + GetSign(MaxMin) );
    v3 VertMinMin = V3(MinMin + RenderOffset + GetSign(MinMin) );
    v3 VertMinMax = V3(MinMax + RenderOffset + GetSign(MinMax) );

    /* DEBUG_DrawLine(world, VertMaxMax, VertMaxMin, BLUE, 0.1f); */
    /* DEBUG_DrawLine(world, VertMinMin, VertMinMax, RED,  0.1f); */

    /* DEBUG_DrawLine(world, VertMaxMax, VertMinMax, BLUE, 0.1f); */
    /* DEBUG_DrawLine(world, VertMinMin, VertMaxMin, RED,  0.1f); */

    v3 Verts[3];

    Verts[0] = VertMaxMax;
    Verts[1] = VertMaxMin;
    Verts[2] = VertMinMin;
    BufferTriangle(world, &Verts[0], 42);

    Verts[0] = VertMinMin;
    Verts[1] = VertMinMax;
    Verts[2] = VertMaxMax;
    BufferTriangle(world, &Verts[0], 42);

    // Draw Boundary voxels aabb
    /* aabb RenderCorrectedAABB = BoundaryVoxelsAABB + RenderOffset; */
    /* RenderCorrectedAABB.MaxCorner += V3(1,1,1); */
    /* DEBUG_DrawAABB( world, RenderCorrectedAABB, Quaternion(), TEAL, 0.10f ); */

#endif


/* chunk->flags = SetFlag(chunk->flags, Chunk_LodGenerated); */

  return;
}

inline b32
IsBoundaryVoxel(chunk_data *Chunk, voxel_position Offset, chunk_dimension Dim)
{
  s32 VoxelIndex = GetIndex(Offset, Chunk, Dim);
  packed_voxel V = Chunk->Voxels[VoxelIndex];

  b32 Result = False;
  Result |= IsSet( V.Data, Voxel_BackFace);
  Result |= IsSet( V.Data, Voxel_FrontFace);
  Result |= IsSet( V.Data, Voxel_TopFace);
  Result |= IsSet( V.Data, Voxel_BottomFace);
  Result |= IsSet( V.Data, Voxel_LeftFace);
  Result |= IsSet( V.Data, Voxel_RightFace);

  return Result;
}

inline void
CheckAndIncrementCurrentP(chunk_data *Chunk,
                          chunk_dimension Dim,
                          voxel_position *CurrentP,
                          s32 *CurrentClosestDistanceSq,
                          voxel_position TargetP,
                          voxel_position TestP)
{
  if ( IsInsideDim(Dim, TestP) )
  {
    s32 DistSq = LengthSq(TargetP - TestP);

    if ( (DistSq > *CurrentClosestDistanceSq) && IsBoundaryVoxel(Chunk, TestP, Dim) )
    {
      *CurrentP = TestP;
      *CurrentClosestDistanceSq = DistSq;
    }
  }

  return;
}

voxel_position
TraverseSurfaceToBoundary(World *world,
                          chunk_data *Chunk,
                          voxel_position StartingP,
                          voxel_position IterDir)
{
  s32 CurrentClosestDistanceSq = 0;
  voxel_position TargetP = (IterDir * world->ChunkDim) - IterDir;

  voxel_position CurrentP = StartingP;

  voxel_position Up      = Voxel_Position(WORLD_Y);
  voxel_position Right   = Voxel_Position(WORLD_X);
  voxel_position Forward = Voxel_Position(WORLD_Z);


  voxel_position LoopStartingP = {};
  while (IsInsideDim(world->ChunkDim, CurrentP) )
  {
    LoopStartingP = CurrentP;

    // Single axies
    voxel_position PUp      = CurrentP + Up;
    voxel_position PDown    = CurrentP - Up;

    voxel_position PRight    = CurrentP + Right;
    voxel_position PLeft     = CurrentP - Right;

    voxel_position PForward = CurrentP + Forward;
    voxel_position PBack    = CurrentP - Forward;

    // Diagonal axies
    voxel_position PUpRight   = PUp   + Right;
    voxel_position PUpLeft    = PUp   - Right;
    voxel_position PDownRight = PDown + Right;
    voxel_position PDownLeft  = PDown - Right;

    // Forward diagonals
    voxel_position FwdUpRight   = PUpRight   + Forward;
    voxel_position FwdUpLeft    = PUpLeft    + Forward;
    voxel_position FwdDownRight = PDownRight + Forward;
    voxel_position FwdDownLeft  = PDownLeft  + Forward;

    // Backward diagonals
    voxel_position BackUpRight   = PUpRight   - Forward;
    voxel_position BackUpLeft    = PUpLeft    - Forward;
    voxel_position BackDownRight = PDownRight - Forward;
    voxel_position BackDownLeft  = PDownLeft  - Forward;

    CheckAndIncrementCurrentP(Chunk, world->ChunkDim, &CurrentP, &CurrentClosestDistanceSq, TargetP, PUp);
    CheckAndIncrementCurrentP(Chunk, world->ChunkDim, &CurrentP, &CurrentClosestDistanceSq, TargetP, PDown);
    CheckAndIncrementCurrentP(Chunk, world->ChunkDim, &CurrentP, &CurrentClosestDistanceSq, TargetP, PLeft);
    CheckAndIncrementCurrentP(Chunk, world->ChunkDim, &CurrentP, &CurrentClosestDistanceSq, TargetP, PRight);
    CheckAndIncrementCurrentP(Chunk, world->ChunkDim, &CurrentP, &CurrentClosestDistanceSq, TargetP, PForward);
    CheckAndIncrementCurrentP(Chunk, world->ChunkDim, &CurrentP, &CurrentClosestDistanceSq, TargetP, PBack);

    CheckAndIncrementCurrentP(Chunk, world->ChunkDim, &CurrentP, &CurrentClosestDistanceSq, TargetP, PUpRight);
    CheckAndIncrementCurrentP(Chunk, world->ChunkDim, &CurrentP, &CurrentClosestDistanceSq, TargetP, PUpLeft);
    CheckAndIncrementCurrentP(Chunk, world->ChunkDim, &CurrentP, &CurrentClosestDistanceSq, TargetP, PDownRight);
    CheckAndIncrementCurrentP(Chunk, world->ChunkDim, &CurrentP, &CurrentClosestDistanceSq, TargetP, PDownLeft);

    CheckAndIncrementCurrentP(Chunk, world->ChunkDim, &CurrentP, &CurrentClosestDistanceSq, TargetP, FwdUpRight);
    CheckAndIncrementCurrentP(Chunk, world->ChunkDim, &CurrentP, &CurrentClosestDistanceSq, TargetP, FwdUpLeft);
    CheckAndIncrementCurrentP(Chunk, world->ChunkDim, &CurrentP, &CurrentClosestDistanceSq, TargetP, FwdDownRight);
    CheckAndIncrementCurrentP(Chunk, world->ChunkDim, &CurrentP, &CurrentClosestDistanceSq, TargetP, FwdDownLeft);

    CheckAndIncrementCurrentP(Chunk, world->ChunkDim, &CurrentP, &CurrentClosestDistanceSq, TargetP, BackUpRight);
    CheckAndIncrementCurrentP(Chunk, world->ChunkDim, &CurrentP, &CurrentClosestDistanceSq, TargetP, BackUpLeft);
    CheckAndIncrementCurrentP(Chunk, world->ChunkDim, &CurrentP, &CurrentClosestDistanceSq, TargetP, BackDownRight);
    CheckAndIncrementCurrentP(Chunk, world->ChunkDim, &CurrentP, &CurrentClosestDistanceSq, TargetP, BackDownLeft);

    if (LoopStartingP == CurrentP)
      break;
  }

  return CurrentP;
}

inline void
Draw0thLod(game_state *GameState, world_chunk *Chunk, v3 RenderOffset)
{
  /* for ( s32 PointIndex = 0; PointIndex < Chunk->PB.Count; ++PointIndex ) */
  /*   DEBUG_DrawPointMarker(world, V3(Chunk->PB.Points[PointIndex]) + RenderOffset, Color--, 1.0f); */


  v3 Verts[3] = {};
  Verts[0] = V3(Chunk->PB.Points[0]) + RenderOffset;


  s32 Color = 42;
  s32 VertIndex = 1;
  while ( (VertIndex + 1) < Chunk->PB.Count )
  {
    Verts[1] = V3(Chunk->PB.Points[VertIndex]) + RenderOffset;
    Verts[2] = V3(Chunk->PB.Points[++VertIndex]) + RenderOffset;
    BufferTriangle(GameState->world, &Verts[0], Chunk->Normal, Color);
  }

  return;
}

b32
CanBuildWorldChunkBoundary(World *world, world_chunk *Chunk)
{
  // FIXME(Jesse): This is _real_ bad for the cache!
  b32 Result = True;

  world_position ChunkP = Chunk->WorldP;

  Result &= IsSet(Chunk->Data->flags, Chunk_RebuildBoundary);

  // Bail early to save the cache most of the time.. does this even help?
  if (!Result) return Result;

  world_position Left  = ChunkP - Voxel_Position(1, 0, 0);
  world_position Right = ChunkP + Voxel_Position(1, 0, 0);
  world_position Top   = ChunkP + Voxel_Position(0, 1, 0);
  world_position Bot   = ChunkP - Voxel_Position(0, 1, 0);
  world_position Front = ChunkP + Voxel_Position(0, 0, 1);
  world_position Back  = ChunkP - Voxel_Position(0, 0, 1);

  world_chunk *TestChunk = GetWorldChunk( world, Left );
  Result &= TestChunk && IsSet(TestChunk->Data->flags, Chunk_Initialized);

  TestChunk = GetWorldChunk( world, Right );
  Result &= TestChunk && IsSet(TestChunk->Data->flags, Chunk_Initialized);

  TestChunk = GetWorldChunk( world, Top );
  Result &= TestChunk && IsSet(TestChunk->Data->flags, Chunk_Initialized);

  TestChunk = GetWorldChunk( world, Bot );
  Result &= TestChunk && IsSet(TestChunk->Data->flags, Chunk_Initialized);

  TestChunk = GetWorldChunk( world, Front );
  Result &= TestChunk && IsSet(TestChunk->Data->flags, Chunk_Initialized);

  TestChunk = GetWorldChunk( world, Back );
  Result &= TestChunk && IsSet(TestChunk->Data->flags, Chunk_Initialized);


  return Result;
}

void
DrawWorldChunk( game_state *GameState,
                world_chunk *Chunk,
                RenderGroup *RG,
                ShadowRenderGroup *SG)
{
  if (Chunk->Data->BoundaryVoxelCount == 0)
    return;

  chunk_data *ChunkData = Chunk->Data;

  if (NotSet(ChunkData->flags, Chunk_Initialized))
    return;

  World *world = GameState->world;
  /* chunk_dimension Dim = world->ChunkDim; */

  // if (!IsInFrustum(Dim, GameState->Camera, Chunk))
    // return;

  Camera_Object *Camera = GameState->Camera;
  /* v3 ChunkRenderOffset = GetRenderP( Dim, Chunk->WorldP, Camera); */
  /* v3 CameraRenderOffset = GetRenderP( Dim, Camera->P, Camera); */

  if ( IsSet( Chunk->Data->flags, Chunk_RebuildBoundary ) )
    BuildWorldChunkBoundaryVoxels(world, Chunk);

  /* if (CanBuildWorldChunkBoundary(world, Chunk)) */
  /* { */
  /*   BuildWorldChunkBoundaryVoxels(world, Chunk); */
  /*   Compute0thLod(GameState, Chunk); */
  /* } */

  /* if ( Length(ChunkRenderOffset - CameraRenderOffset ) < MIN_LOD_DISTANCE ) */
  {
    r32 Scale = 1.0f;
    BufferChunkMesh( GameState->Plat, world, ChunkData, Chunk->WorldP, RG, SG, Camera, Scale);
  }

  /* else */
  /* { */
  /*   Draw0thLod( GameState, Chunk, ChunkRenderOffset); */
  /* } */

  /* DEBUG_DrawChunkAABB( GameState->world, Chunk, GameState->Camera, Quaternion(), 0); */

  return;
}

void
DrawEntity(
    platform *Plat,
    World *world,
    entity *Entity,
    Camera_Object *Camera,
    RenderGroup *RG,
    ShadowRenderGroup *SG
  )
{
  // Debug light code
  /* v3 LightP = GetRenderP(world, Entity->P + Entity->Model.Dim/2); */
  /* glUniform3fv(RG->LightPID, 1, &LightP[0]); */
  //

  if (!Spawned(Entity))
    return;

  chunk_data *Model = Entity->Model;

#if DEBUG_DRAW_COLLISION_VOLUMES
  aabb AABB = GetRenderSpaceAABB(world->ChunkDim, Entity, Camera);
  DEBUG_DrawAABB( world, AABB, Quaternion(), PINK);
#endif

  if ( NotSet(Model->flags, Chunk_Initialized) )
    return;

  if ( IsSet(Model->flags, Chunk_RebuildBoundary) )
    BuildEntityBoundaryVoxels(Model, Entity->P.WorldP, Entity->ModelDim);


  BufferChunkMesh(Plat, world, Model, Entity->P.WorldP, RG, SG, Camera, Entity->Scale, Entity->P.Offset);

  return;
}
