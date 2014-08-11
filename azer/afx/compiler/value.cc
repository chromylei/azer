#include "azer/afx/compiler/value.h"

#include "azer/afx/compiler/astnode.h"
#include "azer/afx/compiler/type.h"
#include "azer/afx/compiler/errno.h"
#include "azer/math/math.h"
#include "base/logging.h"
#include "base/basictypes.h"

namespace azer {
namespace afx {

#define AFX_INT_VALUE(v, ret)                                           \
  ret = 0;                                                              \
  if ((v).type()->type() == kInt32) {ret = v.GetInt32();}               \
  else if ((v).type()->type() == kUint32) { ret = v.GetUint32();}       \
  else if ((v).type()->type() == kShort) { ret = v.GetShort();}         \
  else if ((v).type()->type() == kBoolean) { ret = (int)v.GetBoolean();} \
  else NOTREACHED();

#define AFX_FLOAT_VALUE(v, ret)                                         \
  ret = 0;                                                              \
  if ((v).type()->type() == kInt32) {ret = v.GetInt32();}               \
  else if ((v).type()->type() == kUint32) { ret = v.GetUint32();}       \
  else if ((v).type()->type() == kShort) { ret = v.GetShort();}         \
  else if ((v).type()->type() == kBoolean) { ret = (int)v.GetBoolean();} \
  else if ((v).type()->type() == kFloat) { ret = v.GetFloat();}         \
  else if ((v).type()->type() == kDouble) { ret = v.GetDouble();}       \
  else NOTREACHED();



namespace {
double ToDouble(const Value& value);
bool IsReal(const TypePtr& type);
BasicType CalcTypeForBasicOper(const Value& v1, const Value& v2);
BasicType MaxScalarType(BasicType t1, BasicType t2);
}  // namespace

void Value::SetErrno(CompileErrno err) {
  valid_ = false;
  errno_ = err;
}

Value& Value::operator ++() {
  DCHECK(valid());
  DCHECK(type_.get() != NULL);
  if (!IsIntegerScalar(type_)) {
    SetErrno(kOperUnacceptableOper);
    return *this;
  } else if (!assigned()) {
    return *this;
  }

  if (type_->type() == kInt32) {
    value_.i32++;
  } else if (type_->type() == kUint32) {
    value_.u32++;
  } else if (type_->type() == kShort) {
    value_.s++;
  } else {
    NOTREACHED();
  }

  return *this;
}

Value& Value::operator --() {
  if (!IsIntegerScalar(type_)) {
    SetErrno(kOperUnacceptableOper);
    return *this;
  } else if (!assigned()) {
    return *this;
  }

  if (type_->type() == kInt32) {
    value_.i32--;
  } else if (type_->type() == kUint32) {
    value_.u32--;
  } else if (type_->type() == kShort) {
    value_.s--;
  } else {
    NOTREACHED();
  }
  return *this;
}

Value& Value::operator ++(int) {
  DCHECK(valid());
  return ++(*this);
}

Value& Value::operator --(int) {
  DCHECK(valid());
  return --(*this);
}

Value& Value::operator !() {
  DCHECK(valid());
  if (type()->IsArray() || type()->IsTexture() || type()->IsMatrix()
      || type()->IsVector() || type()->IsIntVec()) {
    SetErrno(kOperUnacceptableOper);
    assigned_ = false;
    return *this;
  }

  if (!assigned()) {
    if (type()->type() == kBoolean || type()->type() == kShort
        || type()->type() == kBoolean || type()->type() == kInt32
        || type()->type() == kUint32) {
      type_.reset(new Type(kBoolean));
    } else {
      SetErrno(kOperUnacceptableOper);
      assigned_ = false;
    }
    return *this;
  }

  if (type()->type() == kBoolean) {
    value_.b = !value_.b;
  } else if (type()->type() == kShort) {
    value_.s = !value_.s;
  } else if (type()->type() == kUint32) {
    value_.u32 = !value_.u32;
  } else if (type()->type() == kInt32) {
    value_.i32 = !value_.i32;
  } else {
    SetErrno(kOperUnacceptableOper);
    assigned_ = false;
  }
  return *this;
}

Value& Value::operator -() {
  DCHECK(valid());

  if (type_->type() == kInt32) {
    value_.i32 = -value_.i32;
  } else if (type_->type() == kShort) {
    value_.s = -value_.s;
  } else if (type_->type() == kFloat) {
    value_.f = -value_.f;
  } else if (type_->IsVector()) {
    for (size_t i = 0; i < arraysize(value_.vec); ++i) {
      value_.vec[i] = -value_.vec[i];
    }
  } else if (type_->IsIntVec()) {
    for (size_t i = 0; i < arraysize(value_.intvec); ++i) {
      value_.vec[i] = -value_.vec[i];
    }
  } else if (type_->IsMatrix()) {
    for (size_t i = 0; i < arraysize(value_.mat); ++i) {
      value_.vec[i] = -value_.mat[i];
    }
  } else {
    SetErrno(kOperUnacceptableOper);
  }
  
  return *this;
}

Value& Value::operator ~() {
  DCHECK(valid());
  if (!IsIntegerScalar(type())) {
    SetErrno(kOperUnacceptableOper);
    return *this;
  }

  if (type_->type() == kInt32) {
    value_.i32 = ~value_.i32;
  } else if (type_->type() == kUint32) {
    value_.u32 = ~value_.u32;
  } else if (type_->type() == kShort) {
    value_.s = ~value_.s;
  } else {
    SetErrno(kOperUnacceptableOper);
  }
  
  return *this;
}

Value operator + (const Value& v1, const Value& v2) {
  DCHECK(v1.valid() && v2.valid());
  if (v1.type()->IsArray() || v2.type()->IsArray() || v1.type()->IsTexture()
      || v2.type()->IsTexture()) {
    return Value();
  } else if (v1.type()->IsVector() && v2.type()->IsVector()) {
    if (v1.type()->type() == v2.type()->type()) {
      return Value(v1.type());
    } else {
      return Value();
    }
  } else if (v1.type()->IsIntVec() && v2.type()->IsIntVec()) {
    if (v1.type()->type() == v2.type()->type()) {
      return Value(v1.type());
    } else {
      return Value();
    }
  } else if (!v1.assigned() || !v2.assigned()) {
    return Value(CalcTypeForBasicOper(v1, v2));
  }

  if (IsIntegerScalar(v1.type()) && IsIntegerScalar(v2.type())) {
    int64 value = 0;
    int64 tmp = 0;
    AFX_INT_VALUE(v1, tmp);
    value += tmp;
    AFX_INT_VALUE(v2, tmp);
    value += tmp;
    return Value((int32)value);
  } else if (v1.type()->IsScalar() && v2.type()->IsScalar()) {
    double value = 0;
    double tmp = 0;
    AFX_FLOAT_VALUE(v1, tmp);
    value += tmp;
    AFX_FLOAT_VALUE(v2, tmp);
    value += tmp;
    return Value((float)value);
  } else if (v1.type()->IsIntVec() && v2.type()->IsIntVec()) {
    // if vector's dim not sample
    DCHECK_EQ (v1.type()->type(), v2.type()->type());
    Value value(v1.type());
    for (size_t i = 0; i < arraysize(value.value_.vec); ++i) {
      value.value_.intvec[i] = v1.value_.intvec[i] + v2.value_.intvec[i];
    }
    return value;
  } else if (v1.type()->IsVector() && v2.type()->IsVector()) {
    // if vector's dim not sample
    DCHECK_EQ (v1.type()->type(), v2.type()->type());
    Value value(v1.type());
    for (size_t i = 0; i < arraysize(value.value_.vec); ++i) {
      value.value_.vec[i] = v1.value_.vec[i] + v2.value_.vec[i];
    }
    return value;
  } else if (v1.type()->IsMatrix() && v2.type()->IsMatrix()) {
    DCHECK_EQ (v1.type()->type(), v2.type()->type());
    Value value(v1.type());
    for (size_t i = 0; i < arraysize(value.value_.mat); ++i) {
      value.value_.mat[i] = v1.value_.mat[i] + v2.value_.mat[i];
    }
    return value;
  } else {
    return Value();
  }
}

Value operator - (const Value& v1, const Value& v2) {
  DCHECK(v1.valid() && v2.valid());
  if (v1.type()->IsArray() || v2.type()->IsArray() || v1.type()->IsTexture()
      || v2.type()->IsTexture()) {
    return Value();
  } else if (v1.type()->IsVector() && v2.type()->IsVector()) {
    if (v1.type()->type() == v2.type()->type()) {
      return Value(v1.type());
    } else {
      return Value();
    }
  } else if (v1.type()->IsIntVec() && v2.type()->IsIntVec()) {
    if (v1.type()->type() == v2.type()->type()) {
      return Value(v1.type());
    } else {
      return Value();
    }
  } else if (!v1.assigned() || !v2.assigned()) {
    return Value(CalcTypeForBasicOper(v1, v2));
  }

  if (IsIntegerScalar(v1.type()) && IsIntegerScalar(v2.type())) {
    int64 value = 0;
    int64 tmp = 0;
    AFX_INT_VALUE(v1, tmp);
    value = tmp;
    AFX_INT_VALUE(v2, tmp);
    value -= tmp;
    return Value((int32)value);
  } else if (v1.type()->IsScalar() && v2.type()->IsScalar()) {
    double value = 0;
    double tmp = 0;
    AFX_FLOAT_VALUE(v1, tmp);
    value = tmp;
    AFX_FLOAT_VALUE(v2, tmp);
    value -= tmp;
    return Value((float)value);
  } else if (v1.type()->IsIntVec() && v2.type()->IsIntVec()) {
    // if vector's dim not sample
    DCHECK_EQ (v1.type()->type(), v2.type()->type());
    Value value(v1.type());
    for (size_t i = 0; i < arraysize(value.value_.vec); ++i) {
      value.value_.intvec[i] = v1.value_.intvec[i] - v2.value_.intvec[i];
    }
    return value;
  } else if (v1.type()->IsVector() && v2.type()->IsVector()) {
    // if vector's dim not sample
    DCHECK_EQ (v1.type()->type(), v2.type()->type());
    Value value(v1.type());
    for (size_t i = 0; i < arraysize(v1.value_.vec); ++i) {
      value.value_.vec[i] = v1.value_.vec[i] - v2.value_.vec[i];
    }
  } else if (v1.type()->IsMatrix() && v2.type()->IsMatrix()) {
    DCHECK_EQ (v1.type()->type(), v2.type()->type());
    Value value(v1.type());
    for (size_t i = 0; i < arraysize(value.value_.mat); ++i) {
      value.value_.mat[i] = v1.value_.mat[i] - v2.value_.mat[i];
    }
    return value;
  } 
  return Value();
}

Value operator * (const Value& v1, const Value& v2) {
  if (v1.type()->IsVector() && v2.type()->IsScalar()) {
    if (!v1.assigned() || !v2.assigned()) { return Value(v1.type());}
    Value v(v1.type()->type());
    double d = 0.0;
    AFX_FLOAT_VALUE(v2, d);
    for (size_t i = 0; i < arraysize(v.value_.vec); ++i) {
      v.value_.vec[i] = v1.value_.vec[i] *d;
    }
    return v;
  } else if(v1.type()->IsScalar() && v2.type()->IsVector()) {
    if (!v1.assigned() || !v2.assigned()) { return Value(v2.type());}
    Value v(v2.type()->type());
    double d = 0.0;
    AFX_FLOAT_VALUE(v1, d);
    for (size_t i = 0; i < arraysize(v.value_.vec); ++i) {
      v.value_.vec[i] = v2.value_.vec[i] * d;
    }
    return v;
  } else if (v1.type()->IsMatrix() && v2.type()->IsScalar()) {
    if (!v1.assigned() || !v2.assigned()) { return Value(v1.type());}
    Value v(v1.type()->type());
    double d = 0.0;
    AFX_FLOAT_VALUE(v2, d);
    for (size_t i = 0; i < arraysize(v.value_.mat); ++i) {
      v.value_.mat[i] = v1.value_.mat[i] * d;
    }
    return v;
  } else if (v1.type()->IsScalar() && v2.type()->IsMatrix()) {
    if (!v1.assigned() || !v2.assigned()) { return Value(v2.type());}
    Value v(v2.type()->type());
    double d = 0.0;
    AFX_FLOAT_VALUE(v1, d);
    for (size_t i = 0; i < arraysize(v.value_.mat); ++i) {
      v.value_.mat[i] = v2.value_.mat[i] * d;
    }
    return v;
  } else if (v1.type()->IsVector() && v2.type()->IsMatrix()) {
    if (v1.type()->type() == kVector2 && v2.type()->type() == kMatrix2) {
      return Value(kVector2);
    } else if (v1.type()->type() == kVector3 && v2.type()->type() == kMatrix3) {
      return Value(kVector3);
    } else if (v1.type()->type() == kVector4 && v2.type()->type() == kMatrix4) {
      return Value(kVector4);
    } else {
      return Value();
    }
  } else if (v1.type()->IsMatrix() && v2.type()->IsVector()) {
    if (v1.type()->type() == kMatrix2 && v2.type()->type() == kVector2) {
      return Value(kVector2);
    } else if (v1.type()->type() == kMatrix3 && v2.type()->type() == kVector3) {
      return Value(kVector3);
    } else if (v1.type()->type() == kMatrix4 && v2.type()->type() == kVector4) {
      return Value(kVector4);
    } else {
      return Value();
    }
  } else if (v1.type()->IsVector() && v2.type()->IsVector()) {
    // attention: not doc_product
    // v1 * v2 = vec3(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
    if (v1.type()->type() == v2.type()->type()) {
      return Value(v1.type());
    } else {
      return Value();
    }
  } else if (v1.type()->IsIntVec() && v2.type()->IsIntVec()) {
    // attention: not doc_product
    // v1 * v2 = vec3(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
    if (v1.type()->type() == v2.type()->type()) {
      return Value(v1.type());
    } else {
      return Value();
    }
  } else if (v1.type()->IsMatrix() && v2.type()->IsMatrix()) {
    if (!v1.assigned() || !v2.assigned()) { return Value(v2.type());}
    azer::Matrix4 mat1(v1.value_.mat);
    azer::Matrix4 mat2(v1.value_.mat);
    azer::Matrix4 mat = std::move(mat1 * mat2);
    Value v(v1.type()->type());
    memcpy(v.value_.mat, &mat, sizeof(v.value_.mat));
    return v;
  } else if (IsIntegerScalar(v1.type()) && IsIntegerScalar(v2.type())) {
    if (!v1.assigned() || !v2.assigned()) { return Value(kInt32);}
    int32 i1, i2;
    AFX_FLOAT_VALUE(v1, i1);
    AFX_FLOAT_VALUE(v2, i2);
    return Value((int32)(i1 * i2));
  } else if (v1.type()->IsScalar() && v2.type()->IsScalar()) {
    if (!v1.assigned() || !v2.assigned()) { return Value(kFloat);}
    double d1, d2;
    AFX_FLOAT_VALUE(v1, d1);
    AFX_FLOAT_VALUE(v2, d2);
    return Value((float)(d1 * d2));
  } else {
    return Value();
  }
}

Value operator / (const Value& v1, const Value& v2) {
  if (v1.type()->IsVector() && v2.type()->IsScalar()) {
    if (!v1.assigned() || !v2.assigned()) { return Value(v1.type()); }

    Value v(v1.type()->type());
    double d = 0.0;
    AFX_FLOAT_VALUE(v2, d);
    for (size_t i = 0; i < arraysize(v.value_.vec); ++i) {
      v.value_.vec[i] = v1.value_.vec[i] / d;
    }
    return v;
  } else if (v1.type()->IsMatrix() && v2.type()->IsScalar()) {
    if (!v1.assigned() || !v2.assigned()) { return Value(v1.type()); }

    Value v(v1.type()->type());
    double d = 0.0;
    AFX_FLOAT_VALUE(v2, d);
    for (size_t i = 0; i < arraysize(v.value_.mat); ++i) {
      v.value_.mat[i] = v1.value_.mat[i] / d;
    }
    return v;
  } else if (v1.type()->IsScalar() && v2.type()->IsScalar()) {
    BasicType new_type = MaxScalarType(v1.type()->type(), v2.type()->type());
    if (!v1.assigned() || !v2.assigned()) {
      if (new_type == kChar || new_type == kBoolean) {
        return Value();
      } else {
        return Value(new_type);
      }
    }

    double d1, d2;
    AFX_FLOAT_VALUE(v1, d1);
    AFX_FLOAT_VALUE(v2, d2);
    
    double new_val = d1 / d2;
    if (new_type == kBoolean) {
      return Value();
    } else if (new_type == kChar) {
      return Value();
    } else if (new_type == kShort) {
      return Value((short)(d1 / d2));
    } else if (new_type == kUint32) {
      return Value((uint32)(d1 / d2));
    } else if (new_type == kInt32) {
      return Value((int32)(d1 / d2));
    } else if (new_type == kFloat) {
      return Value((float)(d1 / d2));
    } else if (new_type == kDouble) {
      return Value((double)(d1 / d2));
    } else {
      NOTREACHED();
      return Value();
    }
  } else {
    return Value();
  }
}

Value operator % (const Value& v1, const Value& v2) {
  DCHECK(v1.valid() && v2.valid());
  if (!IsIntegerScalar(v1.type()) || !IsIntegerScalar(v1.type())) {
    return Value();
  } else if (!v1.assigned() || !v2.assigned()) {
    return Value(TypePtr(new Type(kInt32)));
  }

  int64 i1 = 0;
  int64 i2 = 0;
  AFX_INT_VALUE(v1, i1);
  AFX_INT_VALUE(v2, i2);
  return Value((int)(i1 % i2));
}

#define AFX_LOGIC_VALID()                                       \
  DCHECK(v1.valid() && v2.valid());                             \
  if (!v1.type()->IsScalar() || !v2.type()->IsScalar()) {       \
    return Value();                                             \
  } else if (!v1.assigned() || !v2.assigned()) {                \
    return Value(TypePtr(new Type(kBoolean)));                  \
  }

Value operator < (const Value& v1, const Value& v2) {
  AFX_LOGIC_VALID();

  double d1, d2;
  AFX_FLOAT_VALUE(v1, d1);
  AFX_FLOAT_VALUE(v2, d2);
  bool ret = d1 < d2;
  return Value(ret);
}

Value operator <= (const Value& v1, const Value& v2) {
  AFX_LOGIC_VALID();
  DCHECK(v1.valid() && v2.valid()); 
  if (!v1.type()->IsScalar() || !v2.type()->IsScalar()) {
    return Value();
  } else if (!v1.assigned() || !v2.assigned()) {
    return Value(TypePtr(new Type(kBoolean)));
  }

  double d1, d2;
  AFX_FLOAT_VALUE(v1, d1);
  AFX_FLOAT_VALUE(v2, d2);
  bool ret = (d1 <= d2);
  return Value(ret);
}

Value operator > (const Value& v1, const Value& v2) {
  AFX_LOGIC_VALID();
  return !(v1 <= v2);
}

Value operator >= (const Value& v1, const Value& v2) {
  AFX_LOGIC_VALID();
  return !(v1 < v2);
}

Value operator != (const Value& v1, const Value& v2) {
  return !(v1 == v2);
}

Value operator == (const Value& v1, const Value& v2) {
  AFX_LOGIC_VALID();

  double d1, d2;
  AFX_FLOAT_VALUE(v1, d1);
  AFX_FLOAT_VALUE(v2, d2);
  bool ret = std::abs(d1 - d2) < 0.00001;
  return Value(ret);
}

namespace {
bool IsLogicalAndAcceptable(const TypePtr& ptr) {
  return IsIntegerScalar(ptr) || ptr->type() == kBoolean;
}
}

Value operator && (const Value& v1, const Value& v2) {
  if (IsLogicalAndAcceptable(v1.type()) && IsLogicalAndAcceptable(v2.type())) {
    int64 i1, i2;
    AFX_INT_VALUE(v1, i1);
    AFX_INT_VALUE(v2, i2);
    return Value((bool)(i1 && i2));
  } else {
    return Value();
  }
}

Value operator || (const Value& v1, const Value& v2) {
  if (IsLogicalAndAcceptable(v1.type()) && IsLogicalAndAcceptable(v2.type())) {
    int64 i1, i2;
    AFX_INT_VALUE(v1, i1);
    AFX_INT_VALUE(v2, i2);
    return Value((bool)(i1 || i2));
  } else {
    return Value();
  }
}

Value operator & (const Value& v1, const Value& v2) {
  AFX_LOGIC_VALID();

  int64 i1, i2;
  AFX_INT_VALUE(v1, i1);
  AFX_INT_VALUE(v2, i2);
  return Value((uint32)(i1 & i2));
}

Value operator | (const Value& v1, const Value& v2) {
  AFX_LOGIC_VALID();

  int64 i1, i2;
  AFX_INT_VALUE(v1, i1);
  AFX_INT_VALUE(v2, i2);
  return Value((uint32)(i1 | i2));
}

Value operator ^ (const Value& v1, const Value& v2) {
  AFX_LOGIC_VALID();
  int64 i1, i2;
  AFX_INT_VALUE(v1, i1);
  AFX_INT_VALUE(v2, i2);
  return Value((uint32)(i1 ^ i2));
}

Value operator << (const Value& v1, const Value& v2) {
  AFX_LOGIC_VALID();
  int64 i1, i2;
  AFX_INT_VALUE(v1, i1);
  AFX_INT_VALUE(v2, i2);
  return Value((uint32)(i1 << i2));
}

Value operator >> (const Value& v1, const Value& v2) {
  AFX_LOGIC_VALID();
  int64 i1, i2;
  AFX_INT_VALUE(v1, i1);
  AFX_INT_VALUE(v2, i2);
  return Value((uint32)(i1 >> i2));
}

Value Value::operator [] (const Value& v) {
  DCHECK(type_.get() != NULL);
  if(!type_->IsArray() && !type_->IsMatrix() && !type_->IsVector()) {
    SetErrno(kOperUnacceptableOper);
    return *this;
  } else if (!v.assigned()) {
    return Value(TypePtr(new Type(type_->type())));
  }

  if (!IsIntegerScalar(v.type())) {
    return Value();
  }

  return Value();
}

namespace {
bool IsReal(const TypePtr& t) {
  return t->type() >= kShort && t->type() <= kDouble && !t->IsArray();
}

double ToDouble(const Value& value) {
  DCHECK(value.assigned()&& value.valid());
  switch(value.type()->type()) {
    case kBoolean: return (double)value.GetBoolean();
    case kShort: return (double)value.GetShort();
    case kInt32: return (double)value.GetInt32();
    case kUint32: return (double)value.GetUint32();
    case kFloat: return (double)value.GetFloat();
    case kDouble: return (double)value.GetDouble();
    default: NOTREACHED(); return 0.0f;
  }
}

BasicType CalcTypeForBasicOper(const Value& v1, const Value& v2) {
  if (IsIntegerScalar(v1.type()) && IsIntegerScalar(v2.type())) {
    return kInt32;
  } else if (v1.type()->IsScalar() && v2.type()->IsScalar()) {
    return kFloat;
  } else if (v1.type()->IsVector() && v2.type()->IsVector()) {
    // if vector's dim not sample
    DCHECK_EQ (v1.type()->type(), v2.type()->type());
    return v1.type()->type();
  } else if (v1.type()->IsVector() && v2.type()->IsScalar()) {
    return v1.type()->type();
  } else if (v1.type()->IsScalar() && v2.type()->IsVector()) {
    return v2.type()->type();
  } else if (v1.type()->IsMatrix() && v2.type()->IsScalar()) {
    return v1.type()->type();
  }
  return kTypeNotSpec;
}

BasicType MaxScalarType(BasicType t1, BasicType t2) {
  DCHECK(t1 >= kBoolean && t1 <= kDouble);
  DCHECK(t2 >= kBoolean && t2 <= kDouble);
  if (t1 > t2) {
    return t1;
  } else {
    return t2;
  }
}
}  // namespace
}  // namespace afx
}  // namespace azer
