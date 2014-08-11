#pragma once

#include "azer/math/math.h"

struct Vertex {
  azer::Vector4 position;
  azer::Vector2 tex;
  azer::Vector3 normal;

  Vertex(const azer::Vector4& p, const azer::Vector2& t)
      : position(p), tex(t) {
  }
};


extern Vertex v[];
extern const uint32 kVertexNum;
extern uint32 indices[];
extern const uint32 kIndicesNum;
