#include "azer/afx/compiler/builtin.h"

#include "azer/afx/compiler/context.h"
#include "azer/afx/compiler/parser.h"
#include "azer/base/string.h"
#include "base/logging.h"
#include "base/lazy_instance.h"
#include "base/file_util.h"

using ::base::FilePath;

namespace azer {
namespace afx {

namespace {
base::LazyInstance<BuiltinContext>::Leaky lazy_builtin;
const std::string g_builtin_str =
    "void clip(float v) {}\n"
    "float fmod(float x, float y) { return 0.0;}\n"
    "float degree(float a) {return 0.0;}\n"
    "float radians(float a) { return 0.0;}\n"
    "int abs(int a) { return 0;}\n"
    "float abs(float a) { return 0.0;}\n"
    "vec3 abs(vec3 a) { return vec3(1.0, 1.0f, 1.0f);}\n"
    "vec4 abs(vec4 a) { return vec4(1.0, 1.0f, 1.0f, 1.0f);}\n"
    "float floor(float a) { return 0.0;}\n"
    "float ceil(float a) { return 0.0;}\n"
    "float sin(float a) { return 0.0;}\n"
    "float cos(float a) { return 0.0;}\n"
    "float asin(float a) { return 0.0;}\n"
    "float acos(float a) { return 0.0;}\n"
    "float tan(float a) { return 0.0;}\n"
    "float atan(float a) { return 0.0;}\n"
    "float sqrt(float a) { return 0.0;}\n"
    "float dot(vec3 v1, vec3 v2) { return 1.0;}\n"
    "float dot(vec4 v1, vec4 v2) { return 1.0;}\n"
    "float length(vec3) { return 1.0;}\n"
    "float length(vec4) { return 1.0;}\n"
    "float clamp(float v, float min, float max) { return vec3(1, 1, 1);}\n"
    "vec2 clamp(vec2 v, float min, float max) { return vec3(1, 1, 1);}\n"
    "vec3 clamp(vec3 v, float min, float max) { return vec3(1, 1, 1);}\n"
    "vec4 clamp(vec4 v, float min, float max) { return vec3(1, 1, 1);}\n"
    "vec3 normalize(vec3 v) { return vec3(1, 1, 1);}\n"
    "vec4 normalize(vec4 v) { return vec4(1, 1, 1, 1);}\n"
    "vec3 reflect(vec3 r, vec3 n) { return vec3(1, 1, 1);}\n"
    "vec4 reflect(vec4 r, vec4 n) { return vec4(1, 1, 1, 0.0f);}\n"
    "vec4 sample1D(Texture1D tex, float texcoord) { return vec4(1, 1, 1, 1);}\n"
    "vec4 sample2D(Texture2D tex, vec2 texcoord) { return vec4(1, 1, 1, 1);}\n"
    "vec4 sample3D(Texture3D tex, vec3 texcoord) { return vec4(1, 1, 1, 1);}\n"
    "vec4 sampleCube(TextureCube tex, vec3 texcoord) { return vec4(1, 1, 1, 1);}\n"
    "vec4 load(Texture2D, ivec3 coord) { return vec4(1.0, 1.0, 1.0, 1.0);}\n"
    "float frac(float v) { return 1.0;}\n"
    "vec3 frac(vec3 v) { return vec3(1, 1, 1);}\n"
    "vec4 frac(vec4 v) { return vec4(1, 1, 1, 1);}\n"
    "float max(float v, float v2) {}\n"
    "vec3 max(vec3 v, float v2) {}\n"
    "vec4 max(vec4 v, float v2) {}\n"
    "float min(float v, float v2) {}\n"
    "vec3 min(vec3 v, float v2) {}\n"
    "vec4 min(vec4 v, float v2) {}\n"
    "float pow(float v1, int v2) {}\n"
    "float pow(float v1, float v2) {}\n"
    "vec3 lerp(vec3 v1, vec3 v2, float s) {}\n"
    "vec4 lerp(vec4 v1, vec4 v2, float s) {}\n"
    ;
}  // namespace

BuiltinContext* BuiltinContext::get() {
  return lazy_builtin.Pointer();
}

BuiltinContext::BuiltinContext()
    : context_(NULL)
    , factory_(NULL) {
  Init();
}

BuiltinContext::~BuiltinContext() {
}

void BuiltinContext::Init() {
  DCHECK(factory_ == NULL && context_ == NULL);
  factory_ = new ASTNodeFactory();
  context_ = new ParseContext(FilePath(FILE_PATH_LITERAL("builtin")), "__builtin__",
                              g_builtin_str, factory_);
  Parser parser;
  CHECK(parser.Parse(context_)) << "Built-in compile failed";
}

ASTNode* BuiltinContext::LookupSymbol(const std::string& name) {
  return context_->LookupFunction("__builtin__", name);
}

ASTNode* BuiltinContext::Lookup(const std::string& name) {
  if (name.find("::") != std::string::npos) {
    return NULL;
  } else {
    return lazy_builtin.Pointer()->LookupSymbol(name);
  }
}
}  // namespace afx
}  // namespace azer
