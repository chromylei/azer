#include "azer/afx/compiler/type.h"

#include "azer/afx/compiler/astnode.h"
#include "azer/math/math.h"
#include "base/logging.h"

namespace azer {
namespace afx {

const char* type_name(BasicType type) {
  switch(type) {
    case kChar: return "char";
    case kString: return "string";
    case kShort: return "short";
    case kInt32: return "int32";
    case kUint32: return "uint32";
    case kBoolean: return "boolean";
    case kFloat: return "float";
    case kDouble: return "double";
    case kVector2: return "vector2";
    case kVector3: return "vector3";
    case kVector4: return "vector4";
    case kIntVec2: return "intvec2";
    case kIntVec3: return "intvec3";
    case kIntVec4: return "intvec4";
    case kMatrix2: return "matrix2";
    case kMatrix3: return "matrix3";
    case kMatrix4: return "matrix4";
    case kTexture1D: return "Texture1D";
    case kTexture2D: return "Texture2D";
    case kTexture3D: return "Texture3D";
    case kTextureCube: return "TextureCube";
    case kStructure: return "structure";
    case kAnonymouseStructure: return "AnonymouseStructure";
    case kFunctional: return "functional";
    default: NOTREACHED(); return NULL;
  }
}
  
const char* qualifier_name(StorageQualifier qualifier) {
  switch (qualifier) {
    case kIn: return "in";
    case kOut: return "out";
    case kInout: return "inout";
    case kConst: return "const";
    case kUniform: return "uniform";
    default: NOTREACHED() << "unknown StorageQualifier: " << (int)qualifier;
      return "";
  }
}

const char* operator_str(Operator op) {
  switch (op) {
    case kOpNull: return "+";                   //
    case kOpNegative: return "-";               // -
    case kOpInc: return "++";                   // ++
    case kOpDec: return "--";                   // --
    case kOpLogicalNot: return "!";             // !
    case kOpBitwiseNot: return "~";             // ~
    case kOpAdd: return "+";                    // +
    case kOpSub: return "-";                    // -
    case kOpMul: return "*";                    // *
    case kOpDiv: return "/";                    // /
    case kOpMod: return "%";                    // %
    case kOpGe: return ">=";                    // >=
    case kOpGt: return ">";                     // >
    case kOpLe: return "<=";                    // <=
    case kOpLt: return "<";                     // <
    case kOpNe: return "!=";                    // !=
    case kOpEq: return "==";                    // ==
    case kOpLogicalAnd: return "&&";            // &&
    case kOpLogicalOr: return "||";             // ||
    case kOpLogicalXor: return "^^";            // ^^
    case kOpBitwiseAnd: return "&";             // &
    case kOpBitwiseOr: return "|";              // |
    case kOpBitwiseXor: return "^";             // ^
    case kOpBitwiseLshift: return "<<";         // <<
    case kOpBitwiseRshift: return ">>";         // >>
    case kOpAssign: return "=";                 // =
    case kOpAddAssign: return "+=";             // +=
    case kOpSubAssign: return "-=";             // -=
    case kOpMulAssign: return "*=";             // *=
    case kOpDivAssign: return "\\=";            // \=
    case kOpModAssign: return "%=";             // %=
    case kOpAndAssign: return "&=";             // &=
    case kOpOrAssign: return "|=";              // |=
    case kOpXorAssign: return "^=";             // ^=
    case kOpLShiftAssign: return "<<=";         // <<=
    case kOpRShiftAssign: return ">>=";         // >>=
    case kOpIndex: return "[]";                 // []
    case kOpMember: return ".";                 // .
    default: NOTREACHED(); return "";
  }
}

Type::Type()
    : type_(kTypeNotSpec), storage_qualifier_(kNoQualifier) {
}

Type::Type(BasicType type)
    : type_(type), storage_qualifier_(kNoQualifier) {
}

Type::Type(BasicType type, StorageQualifier q)
    : type_(type), storage_qualifier_(q) {
}

uint32 BasicTypeSize(BasicType type) {
  switch (type) {
    case kChar: return 1;
    case kShort: return 2;
    case kInt32: return 4;
    case kUint32: return 4;
    case kBoolean: return sizeof(bool);
    case kFloat: return sizeof(float);
    case kDouble: return sizeof(double);
    case kVector2: return sizeof(azer::Vector2);
    case kVector3: return sizeof(azer::Vector3);
    case kVector4: return sizeof(azer::Vector4);
    case kMatrix2: return sizeof(azer::Vector2);
    case kMatrix3: return sizeof(azer::Vector3);
    case kMatrix4: return sizeof(azer::Vector4);
    case kIntVec2:
    case kIntVec3:
    case kIntVec4:
    default: NOTREACHED(); return 0;
  }
}

uint32 SizeofType(const Type& type) {
  int unit_size = 0;
  if (type.IsStructure()) {
  } else if (type.IsBasicType()) {
    unit_size = BasicTypeSize(type.type());
  } else if (type.IsTexture()) {
  } else {
    NOTREACHED();
  }

  if (type.IsArray()) {
    NOTREACHED() << "calc const expression";
    return unit_size;
  } else {
    return unit_size;
  }
}
}  // namespace afx
}  // namespace azer
