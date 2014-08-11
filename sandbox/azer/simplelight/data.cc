#include "sandbox/azer/simplelight/data.h"


Vertex v[] = {
  // Front Face
  Vertex(azer::Vector4(-1.0f, -1.0f, -1.0f, 1.0f),
         azer::Vector2(0.0f, 1.0f)),
  Vertex(azer::Vector4(-1.0f,  1.0f, -1.0f, 1.0f),
         azer::Vector2(0.0f, 0.0f)),
  Vertex(azer::Vector4( 1.0f,  1.0f, -1.0f, 1.0f),
         azer::Vector2(1.0f, 0.0f)),
  Vertex(azer::Vector4( 1.0f, -1.0f, -1.0f, 1.0f),
         azer::Vector2(1.0f, 1.0f)),

  // Back Face
  Vertex(azer::Vector4(-1.0f, -1.0f, 1.0f, 1.0f),
         azer::Vector2(1.0f, 1.0f)),
  Vertex(azer::Vector4( 1.0f, -1.0f, 1.0f, 1.0f),
         azer::Vector2(0.0f, 1.0f)),
  Vertex(azer::Vector4( 1.0f,  1.0f, 1.0f, 1.0f),
         azer::Vector2(0.0f, 0.0f)),
  Vertex(azer::Vector4(-1.0f,  1.0f, 1.0f, 1.0f),
         azer::Vector2(1.0f, 0.0f)),

  // Top Face
  Vertex(azer::Vector4(-1.0f, 1.0f, -1.0f, 1.0f),
         azer::Vector2(0.0f, 1.0f)),
  Vertex(azer::Vector4(-1.0f, 1.0f,  1.0f, 1.0f),
         azer::Vector2(0.0f, 0.0f)),
  Vertex(azer::Vector4( 1.0f, 1.0f,  1.0f, 1.0f),
         azer::Vector2(1.0f, 0.0f)),
  Vertex(azer::Vector4( 1.0f, 1.0f, -1.0f, 1.0f),
         azer::Vector2(1.0f, 1.0f)),

  // Bottom Face
  Vertex(azer::Vector4(-1.0f, -1.0f, -1.0f, 1.0f),
         azer::Vector2(1.0f, 1.0f)),
  Vertex(azer::Vector4( 1.0f, -1.0f, -1.0f, 1.0f),
         azer::Vector2(0.0f, 1.0f)),
  Vertex(azer::Vector4( 1.0f, -1.0f,  1.0f, 1.0f),
         azer::Vector2(0.0f, 0.0f)),
  Vertex(azer::Vector4(-1.0f, -1.0f,  1.0f, 1.0f),
         azer::Vector2(1.0f, 0.0f)),

  // Left Face
  Vertex(azer::Vector4(-1.0f, -1.0f,  1.0f, 1.0f),
         azer::Vector2(0.0f, 1.0f)),
  Vertex(azer::Vector4(-1.0f,  1.0f,  1.0f, 1.0f),
         azer::Vector2(0.0f, 0.0f)),
  Vertex(azer::Vector4(-1.0f,  1.0f, -1.0f, 1.0f),
         azer::Vector2(1.0f, 0.0f)),
  Vertex(azer::Vector4(-1.0f, -1.0f, -1.0f, 1.0f),
         azer::Vector2(1.0f, 1.0f)),

  // Right Face
  Vertex(azer::Vector4( 1.0f, -1.0f, -1.0f, 1.0f),
         azer::Vector2(0.0f, 1.0f)),
  Vertex(azer::Vector4( 1.0f,  1.0f, -1.0f, 1.0f),
         azer::Vector2(0.0f, 0.0f)),
  Vertex(azer::Vector4( 1.0f,  1.0f,  1.0f, 1.0f),
         azer::Vector2(1.0f, 0.0f)),
  Vertex(azer::Vector4( 1.0f, -1.0f,  1.0f, 1.0f),
         azer::Vector2(1.0f, 1.0f)),
};

const uint32 kVertexNum = arraysize(v);

uint32 indices[] = {
  // Front Face
  0,  1,  2,
  0,  2,  3,

  // Back Face
  4,  5,  6,
  4,  6,  7,

  // Top Face
  8,  9, 10,
  8, 10, 11,

  // Bottom Face
  12, 13, 14,
  12, 14, 15,

  // Left Face
  16, 17, 18,
  16, 18, 19,

  // Right Face
  20, 21, 22,
  20, 22, 23
};

const uint32 kIndicesNum = arraysize(indices);
