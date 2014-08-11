#pragma once

#include <xnamath.h>
#include <windows.h>

class Camera {
 public:
  Camera()
      : camYaw(0.0f)
      , camPitch(0.0f)
      , moveBackForward(0.0f)
      , moveLeftRight(0.0f)    {
    DefaultForward = XMVectorSet(0.0f,0.0f,1.0f, 0.0f);
    DefaultRight = XMVectorSet(1.0f,0.0f,0.0f, 0.0f);
    camForward = XMVectorSet(0.0f,0.0f,1.0f, 0.0f);
    camRight = XMVectorSet(1.0f,0.0f,0.0f, 0.0f);

    camPosition = XMVectorSet( 0.0f, 2.0f, -8.0f, 0.0f );
    camTarget = XMVectorSet( 0.0f, 0.0f, 0.0f, 0.0f );
    camUp = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );

    moveLeftRight = 0.0f;
    moveBackForward = 0.0f;
    moveUp = 0.0f;
  }
  void walk(float step) {
    moveBackForward += step;
  }

  void strfde(float step) {
    moveLeftRight += step;
  }
  void fly(float step) {
    moveUp += step;
  }

  void OnInput() {
    const float speed = 0.05f;
    if( ::GetAsyncKeyState('A') & 0x8000f ) {
      strfde(-speed);
    }
    if( ::GetAsyncKeyState('D') & 0x8000f ) {
      strfde(speed);
    }
    if( ::GetAsyncKeyState('W') & 0x8000f ) {
      walk(+speed);
    }
    if( ::GetAsyncKeyState('S') & 0x8000f ) {
      walk(-speed);
    }


    if( ::GetAsyncKeyState('H') & 0x8000f ) {
      fly(+speed);
    }
    if( ::GetAsyncKeyState('J') & 0x8000f ) {
      fly(-speed);
    }

    if( ::GetAsyncKeyState('Q') & 0x8000f ) {
      camYaw -= 0.001f;
    }

    if( ::GetAsyncKeyState('E') & 0x8000f ) {
      camYaw += 0.001f;
    }

    if( ::GetAsyncKeyState('Z') & 0x8000f ) {
      camPitch -= 0.001f;
    }

    if( ::GetAsyncKeyState('X') & 0x8000f ) {
      camPitch += 0.001f;
    }

    GenMatrix();
  }

  void GenMatrix() {
    camRotationMatrix = XMMatrixRotationRollPitchYaw(camPitch, camYaw, 0);
    camTarget = XMVector3TransformCoord(DefaultForward, camRotationMatrix );
    camTarget = XMVector3Normalize(camTarget);

    XMMATRIX RotateYTempMatrix;
    RotateYTempMatrix = XMMatrixRotationY(camYaw);

    camRight = XMVector3TransformCoord(DefaultRight, RotateYTempMatrix);
    camUp = XMVector3TransformCoord(camUp, RotateYTempMatrix);
    camForward = XMVector3TransformCoord(DefaultForward, RotateYTempMatrix);

    camPosition += moveLeftRight*camRight;
    camPosition += moveBackForward*camForward;
    camPosition += moveUp*camUp;

    moveLeftRight = 0.0f;
    moveBackForward = 0.0f;
    moveUp = 0.0f;

    camTarget = camPosition + camTarget;

    camView = XMMatrixLookAtLH( camPosition, camTarget, camUp );

    camProjection = XMMatrixPerspectiveFovLH(0.4f*3.14f, 4.0f / 3.0f, 1.0f, 1000.0f);
  }

  XMMATRIX& view() { return camView;}
  const XMMATRIX& view() const { return camView;}
  const XMVECTOR& position() const { return camPosition;}
  const XMMATRIX& projection() const { return camProjection;}

  void SetPosition(const azer::Vector3& pos) {
    camPosition = XMVectorSet(pos.x, pos.y, pos.z, 0.0);
  }

  void SetTarget(const azer::Vector3& pos) {
    camTarget = XMVectorSet(pos.x, pos.y, pos.z, 0.0);
  }
 private:
  XMMATRIX camView;
  XMMATRIX camRotationMatrix;
  XMVECTOR camPosition;
  XMMATRIX camProjection;
  XMVECTOR camTarget;
  XMVECTOR camRight;
  XMVECTOR camForward;
  XMVECTOR camUp;
  XMVECTOR DefaultRight;
  XMVECTOR DefaultForward;

  float moveBackForward;
  float moveLeftRight;
  float moveUp;
  float camYaw;
  float camPitch;
};
