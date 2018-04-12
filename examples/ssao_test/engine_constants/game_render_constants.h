#define CD_X 4
#define CD_Y 4
#define CD_Z 4

// Visible Region XYZ - Must be > (3,3,3)
#define VR_X 4
#define VR_Y 4
#define VR_Z 4


#define WORLD_CHUNK_DIM Chunk_Dimension(CD_X,CD_Y,CD_Z)
global_variable chunk_dimension Global_WorldChunkDim = WORLD_CHUNK_DIM;

// Texture Resolution
#define SHADOW_MAP_RESOLUTION_X (4096)
#define SHADOW_MAP_RESOLUTION_Y (4096)

// World-Space draw resolution
#define SHADOW_MAP_X (50)
#define SHADOW_MAP_Y (50)

#define SHADOW_MAP_Z_MIN (-100.0f)
#define SHADOW_MAP_Z_MAX ( 100.0f)

debug_global v3 CameraInitialFront = {{ 1, 1,-1 }};
