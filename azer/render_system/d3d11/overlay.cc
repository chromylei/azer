#include "azer/render_system/d3d11/overlay.h"

#include "azer/render/render.h"
#include "azer/render_system/d3d11/render_system.h"

namespace azer {

const char* D3D11OverlayEffect::kVertexShaderProg = ""
    "struct VS_OUTPUT {                                  \n"
    "  float4 Pos : SV_POSITION;                         \n"
    "  float2 texcoord : TEXCOORD;                       \n"
    "};                                                  \n"
    "VS_OUTPUT vs_main(float4 inpos : POSITION,          \n"
    "                  float2 tex : TEXCOORD) {          \n"
    "  VS_OUTPUT output;                                 \n"
    "  output.Pos = inpos;                               \n"
    "  inpos.z = 0.0f;                                   \n"
    "  output.texcoord = tex;                            \n"
    "  return output;                                    \n"
    "}";


const char* D3D11OverlayEffect::kPixelShaderProg = ""
    "struct VS_OUTPUT {                                      \n"
    "  float4 Pos : SV_POSITION;                             \n"
    "  float2 Tex : TEXTURE;                                 \n"
    "};                                                      \n"
    "Texture2D ObjTexture;                                   \n"
    "SamplerState ObjSamplerState;                           \n"
    "float4 ps_main(VS_OUTPUT input) : SV_TARGET {           \n"
    "  float4 diffuse =  ObjTexture.Sample(ObjSamplerState, input.Tex); \n"
    "  clip(diffuse.a - 0.01);                                          \n"
    "  return diffuse;                                                  \n"
    "}";

bool D3D11OverlayEffect::Init(Overlay* surface, D3D11RenderSystem* rs) {
  GpuProgramPtr vs(rs->CreateVertexGpuProgram(surface->GetVertexDesc(),
                                              kVertexShaderProg));
  DCHECK(vs.get() != NULL) << "Overlay default VertexStage Program compiled failed";
  GpuProgramPtr ps(rs->CreateGpuProgram(kPixelStage, kPixelShaderProg));
  if (ps.get() == NULL || vs.get() == NULL) {
    return false;
  }
  technique_.reset(rs->CreateTechnique());
  technique_->AddGpuProgram(vs);
  technique_->AddGpuProgram(ps);

  return true;
}

OverlayEffect* D3D11Overlay::CreateDefaultEffect() {
  DCHECK(render_system_ != NULL);
  std::unique_ptr<D3D11OverlayEffect> effect_ptr(
      new D3D11OverlayEffect(render_system_));
  if (effect_ptr->Init(this, render_system_)) {
    return effect_ptr.release();
  } else {
    return NULL;
  }
}

bool D3D11Overlay::Init(azer::RenderSystem* rs) {
  if (!Overlay::InitVertex(rs)) {
    return false;
  }
  return true;
}
}  // namespace azer
