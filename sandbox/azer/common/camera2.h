#pragma once

#include "azer/render/render.h"
#include "azer/math/math.h"

class Camera2 {
 public:
  Camera2() {
    projection_ = azer::PerspectiveRHD3D(azer::Degree(45.0f),
                                         4.0f / 3.0f,
                                         1.0f, 1000.0f);
  }
  void OnInput() {
    const float speed = 0.05f;
    if( ::GetAsyncKeyState('A') & 0x8000f ) {
      camera_.strafe(-speed);
    }
    if( ::GetAsyncKeyState('D') & 0x8000f ) {
      camera_.strafe(speed);
    }
    if( ::GetAsyncKeyState('W') & 0x8000f ) {
      camera_.walk(+speed);
    }
    if( ::GetAsyncKeyState('S') & 0x8000f ) {
      camera_.walk(-speed);
    }


    if( ::GetAsyncKeyState('H') & 0x8000f ) {
      camera_.fly(+speed);
    }
    if( ::GetAsyncKeyState('J') & 0x8000f ) {
      camera_.fly(-speed);
    }

    if( ::GetAsyncKeyState('Q') & 0x8000f ) {
      camera_.yaw(azer::Radians(0.001f));
    }

    if( ::GetAsyncKeyState('E') & 0x8000f ) {
      camera_.yaw(azer::Radians(-0.001f));
    }

    if( ::GetAsyncKeyState('Z') & 0x8000f ) {
      camera_.pitch(azer::Radians(-0.001f));
    }

    if( ::GetAsyncKeyState('X') & 0x8000f ) {
      camera_.pitch(azer::Radians(0.001f));
    }
  }

  const azer::Matrix4 view() const { return camera_.GenViewMat();}
  const azer::Vector3& position() const { return camera_.position();}
  const azer::Matrix4& projection() const { return projection_;}

  void SetPosition(const azer::Vector3& pos) {
    camera_.SetPosition(pos);
  }

  void SetTarget(const azer::Vector3& pos) {
    camera_.SetLookAt(pos);
  }
 private:
  azer::Matrix4 projection_;
  azer::Camera camera_;
};
