#pragma once

#include <string>
#include <vector>
#include "base/logging.h"

namespace azer {
namespace afx {

enum StorageQualifier {
  kNoQualifier = 0,
  kIn         = 0x00000001,
  kOut        = 0x00000002,
  kInout      = 0x00000004,
  kConst      = 0x00000008,
  kUniform    = 0x00000010,
};

enum BasicType {
  kTypeNotSpec = -1,
  kVoid = 1,
  kBoolean,
  kChar,
  kShort,
  kUint32,
  kInt32,
  kFloat,
  kDouble,
  kString,
  kVector2,
  kVector3,
  kVector4,
  kIntVec2,
  kIntVec3,
  kIntVec4,
  kMatrix2,
  kMatrix3,
  kMatrix4,
  kTexture1D,
  kTexture1DArray,
  kTexture2D,
  kTexture2DArray,
  kTexture3D,
  kTextureCube,
  kStructure,
  kAnonymouseStructure,
  kFunctional,
};

enum Operator {
  kOpNull,           //
  kOpNegative,       // -
  kOpInc,            // ++
  kOpDec,            // --
  kOpLogicalNot,     // !
  kOpBitwiseNot,     // ~

  kOpAdd,            // +
  kOpSub,            // -
  kOpMul,            // *
  kOpDiv,            // /
  kOpMod,            // %
  kOpGe,             // >=
  kOpGt,             // >
  kOpLe,             // <=
  kOpLt,             // <
  kOpNe,             // !=
  kOpEq,             // ==
  kOpLogicalAnd,     // &&
  kOpLogicalOr,      // ||
  kOpLogicalXor,     // ^^
  kOpBitwiseAnd,     // &
  kOpBitwiseOr,      // |
  kOpBitwiseXor,     // ^
  kOpBitwiseLshift,  // <<
  kOpBitwiseRshift,  // >>
  kOpAssign,         // =
  kOpAddAssign,      // +=
  kOpSubAssign,      // -=
  kOpMulAssign,      // *=
  kOpDivAssign,      // \=
  kOpModAssign,      // %=
  kOpAndAssign,      // &=
  kOpOrAssign,       // |=
  kOpXorAssign,      // ^=
  kOpLShiftAssign,   // <<=
  kOpRShiftAssign,   // >>=
  kOpIndex,          // []
  kOpMember,         // .
};

const char* type_name(BasicType type);
const char* qualifier_name(StorageQualifier qualifier);
const char* operator_str(Operator op);

class ASTNode;
class StructDeclNode;

/**
 * Note: Type 如果保存的是 struct name, 那么他永远都应该使用完整名称
 * 即：带有 package 前缀
 */
class Type {
 public:
  Type();
  Type(BasicType type);
  Type(BasicType type, StorageQualifier q);

  bool IsStructure() const { return type_ == kStructure;}
  bool IsAnomyousStruct() const { return name_.empty();}
  bool IsBasicType() const;
  bool IsTexture() const;
  bool IsArray() const;
  bool IsVector() const;
  bool IsIntVec() const;
  bool IsMatrix() const;
  bool IsScalar() const;

  void SetName(const std::string& name) {
    name_ = name;
  }

  const std::string& name() const { return name_;}

  void SetType(BasicType t);
  BasicType type() const { return type_;}
  void SetStorageQualifier(StorageQualifier q) { storage_qualifier_ = q; }
  StorageQualifier storage_qualifier() const { return storage_qualifier_; }
  const std::vector<int>& GetDimVec() const { return dim_;}
  void PushDim(int dim) { dim_.push_back(dim); }
  void SetDim(int index, int dim) {
    DCHECK_LT(index, (int)dim_.size());
    dim_[index] = dim;
  }
  int GetDim(int index) const {
    DCHECK_LT(index, (int)dim_.size());
    return dim_[index];
  }
 private:
  StorageQualifier storage_qualifier_;
  BasicType type_;
  std::string name_;
  std::vector<int> dim_;
  DISALLOW_COPY_AND_ASSIGN(Type);
};

typedef std::shared_ptr<Type> TypePtr;

inline void Type::SetType(BasicType t) {
  DCHECK_EQ(type_, kTypeNotSpec);
  type_ = t;
}

inline bool Type::IsBasicType() const {
  return type() >= kVoid && type() < kMatrix4;
}

inline bool Type::IsTexture() const {
  if (type() >= kTexture1D && type() <= kTextureCube) { return true; }
  else { return false; }
}
inline bool Type::IsArray() const {
  return dim_.size() != 0;
}

inline bool Type::IsIntVec() const {
  return type() == kIntVec2 || type() == kIntVec3 || type() == kIntVec4;
}

inline bool Type::IsVector() const {
  return type() == kVector2 || type() == kVector3 || type() == kVector4;
}

inline bool Type::IsMatrix() const {
  return type() == kMatrix2 || type() == kMatrix3 || type() == kMatrix4;
}

inline bool Type::IsScalar() const {
  return !IsMatrix() && !IsVector() && !IsIntVec()
      && !IsArray() && !IsTexture() && !IsStructure();
}

uint32 SizeofType(const Type& type);

inline bool IsTypeSupportMemberOper(const TypePtr& type) {
  // float vector support swizzle expression
  return type->IsVector() || type->IsStructure();
}

inline bool IsIntegerScalar(const TypePtr& type) {
  if (type->IsArray()) return false;
  return type->type() == kInt32
      || type->type() == kUint32
      || type->type() == kShort;
}
}  // namespace afx
}  // namespace azer
