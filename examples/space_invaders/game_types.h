struct game_state
{
  world         *World;
  platform      *Plat;
  entity        *Players;
  camera        *Camera;

  g_buffer_render_group *gBuffer;
  ao_render_group *AoGroup;
  shadow_render_group     *SG;

  memory_arena *Memory;

  model   *Models;

  random_series Entropy;
  game_mode Mode;

  event_queue EventQueue;

  aabb *FolieTable;

  entity *EntityTable[TOTAL_ENTITY_COUNT];
};
