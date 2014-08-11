#pragma once

#include "base/basictypes.h"
#include "azer/afx/compiler/type.h"
#include "azer/afx/compiler/errno.h"

namespace azer {
namespace afx {
/**
 * Value
 * for calculate the value of expression
 * it may be invalid, for example array + array
 */
class Value {
 public:
  Value();
  explicit Value(bool v);
  explicit Value(char v);
  explicit Value(short v);
  explicit Value(int32 v);
  explicit Value(int32* v, int dim);
  explicit Value(uint32 v);
  explicit Value(uint32* v, int dim);
  explicit Value(float v);
  explicit Value(float* v, int dim);
  explicit Value(double v);
  Value(BasicType type);
  Value(TypePtr type);
  Value(const Value& v) { *this = v;}
  ~Value();

  TypePtr& type() { return type_;}
  const TypePtr& type() const { return type_;}
  bool valid() const { return valid_;}
  bool assigned() const { return assigned_;}

  Value& operator ++(int);
  Value& operator --(int);
  Value& operator ++();
  Value& operator --();
  Value& operator !();
  Value& operator -();
  Value& operator ~();
  friend Value operator + (const Value& v1, const Value& v2);
  friend Value operator - (const Value& v1, const Value& v2);
  friend Value operator * (const Value& v1, const Value& v2);
  friend Value operator / (const Value& v1, const Value& v2);
  friend Value operator % (const Value& v1, const Value& v2);

  friend Value operator > (const Value& v1, const Value& v2);
  friend Value operator >= (const Value& v1, const Value& v2);
  friend Value operator < (const Value& v1, const Value& v2);
  friend Value operator <= (const Value& v1, const Value& v2);
  friend Value operator != (const Value& v1, const Value& v2);
  friend Value operator == (const Value& v1, const Value& v2);
  friend Value operator && (const Value& v1, const Value& v2);
  friend Value operator || (const Value& v1, const Value& v2);
  friend Value operator & (const Value& v1, const Value& v2);
  friend Value operator | (const Value& v1, const Value& v2);
  friend Value operator ^ (const Value& v1, const Value& v2);
  friend Value operator >> (const Value& v1, const Value& v2);
  friend Value operator << (const Value& v1, const Value& v2);

  Value& operator += (const Value& v);
  Value& operator -= (const Value& v);
  Value& operator *= (const Value& v);
  Value& operator /= (const Value& v);
  Value& operator %= (const Value& v);
  Value& operator &= (const Value& v);
  Value& operator |= (const Value& v);
  Value& operator ^= (const Value& v);
  Value& operator >>= (const Value& v);
  Value& operator <<= (const Value& v);
  Value operator [] (const Value& v);
  
  Value& operator = (const Value& v);

  bool GetBoolean() const;
  char GetChar() const;
  short GetShort() const;
  int32 GetInt32() const;
  uint32 GetUint32() const;
  float GetFloat() const;
  float GetDouble() const;
  const float* GetVec() const;
  const int* GetIntVec() const;

  void SetBasicType(BasicType type);
  void SetBoolean(bool b);
  void SetChar(char b);
  void SetShort(short s);
  void SetUint32(uint32 i32);
  void SetInt32(int32 i);
  void SetFloat(float f);
  void SetDouble(double d);
  void SetString(const std::string& str);
  void SetType(TypePtr type);
  void SetVec2(const float *v);
  void SetVec3(const float *v);
  void SetVec4(const float *v);
  void reset();
 private:
  void SetErrno(CompileErrno err);
  TypePtr type_;
  struct Array {
    union {
      bool* barr;
      int32* iarr;
      uint32* uarr;
      float* farr;
      double* darr;      
    };
    int dim;
  };
  union {
    bool b;
    char c;
    short s;
    uint32 u32;
    int32 i32;
    float f;
    double d;
    int intvec[4];
    int intmat[16];
    float vec[4];
    float mat[16];
    Array arr;
    std::string* str;
  } value_;
  bool valid_;
  bool assigned_;  // value is assigned or not
  CompileErrno errno_;
};

typedef std::shared_ptr<Value> ValuePtr;

inline Value::Value()
    : assigned_(false)
    , valid_(false)
    , errno_(kOperUnacceptableOper) {
  memset(&value_, 0, sizeof(value_));
}

inline Value::Value(bool v) {
  memset(&value_, 0, sizeof(value_));
  type_.reset(new Type(kBoolean));
  value_.b = v;
  assigned_ = true;
  valid_ = true;
  errno_ = kNoError;
}

inline Value::Value(char v) {
  memset(&value_, 0, sizeof(value_));
  type_.reset(new Type(kChar));
  value_.c = v;
  assigned_ = true;
  valid_ = true;
  errno_ = kNoError;
}

inline Value::Value(short v) {
  memset(&value_, 0, sizeof(value_));
  type_.reset(new Type(kShort));
  value_.s = v;
  assigned_ = true;
  valid_ = true;
  errno_ = kNoError;
}

inline Value::Value(int32 v) {
  memset(&value_, 0, sizeof(value_));
  type_.reset(new Type(kInt32));
  value_.i32 = v;
  assigned_ = true;
  valid_ = true;
  errno_ = kNoError;
}

inline Value::Value(uint32 v) {
  memset(&value_, 0, sizeof(value_));
  type_.reset(new Type(kUint32));
  value_.u32 = v;
  assigned_ = true;
  valid_ = true;
  errno_ = kNoError;
}

inline Value::Value(float v) {
  memset(&value_, 0, sizeof(value_));
  type_.reset(new Type(kFloat));
  value_.f = v;
  assigned_ = true;
  valid_ = true;
  errno_ = kNoError;
}

inline Value::Value(double v) {
  memset(&value_, 0, sizeof(value_));
  type_.reset(new Type(kDouble));
  value_.d = v;
  assigned_ = true;
  valid_ = true;
  errno_ = kNoError;
}

inline Value::Value(BasicType type)
  : type_(TypePtr(new Type(type)))
  , valid_(true)
  , errno_(kNoError) {
  memset(&value_, 0, sizeof(value_));
}

inline Value::Value(TypePtr type)
    : type_(type)
    , valid_(true)
    , errno_(kNoError) {
  memset(&value_, 0, sizeof(value_));
}

inline Value::Value(int32* v, int dim) {
  errno_ = kNoError;
  type_.reset(new Type(kInt32));
  type_->PushDim(dim);
  value_.arr.iarr = new int32[dim];
  memcpy(value_.arr.iarr, v, dim * sizeof(int32));
}

inline Value::Value(uint32* v, int dim) {
  errno_ = kNoError;
  type_.reset(new Type(kUint32));
  type_->PushDim(dim);
  value_.arr.uarr = new uint32[dim];
  memcpy(value_.arr.iarr, v, dim * sizeof(int32));
}

inline Value::Value(float* v, int dim) {
  errno_ = kNoError;
  type_.reset(new Type(kFloat));
  type_->PushDim(dim);
  // type_.Set
  value_.arr.farr = new float[dim];
  memcpy(value_.arr.farr, v, dim * sizeof(float));
}

inline Value::~Value() {
  reset();
}

inline void Value::reset() {
  if (type_.get() == NULL) {
  } else if (type_->IsArray()) {
    if (type_->type() == kFloat) {
      if (value_.arr.farr) {
        delete[] value_.arr.farr;
      }
    } else if (type_->type() == kUint32) {
      if (value_.arr.uarr) {
        delete[] value_.arr.uarr;
      }
    } else if (type_->type() == kInt32) {
      if (value_.arr.iarr) {
        delete[] value_.arr.iarr;
      }
    } else {
    }
  }

  assigned_ = false;
  valid_ = true;
  memset(&value_, 0, sizeof(value_));
}

inline Value& Value::operator = (const Value& v) {
  this->type_ = v.type_;
  this->value_ = v.value_;
  this->errno_ = v.errno_;
  this->valid_ = v.valid_;
  return *this;
}

inline void Value::SetBoolean(bool b) {
  reset();
  value_.b = b;
  this->type_.reset(new Type(kBoolean));
  this->assigned_ = true;
  this->valid_ = true;
}

inline void Value::SetChar(char c) {
  reset();
  value_.c = c;
  this->type_.reset(new Type(kChar));
  this->assigned_ = true;
  this->valid_ = true;
}

inline void Value::SetShort(short s) {
  reset();
  value_.s = s;
  this->type_.reset(new Type(kShort));
  this->assigned_ = true;
  this->valid_ = true;
}

inline void Value::SetUint32(uint32 u32) {
  reset();
  value_.u32 = u32;
  this->type_.reset(new Type(kUint32));
  this->assigned_ = true;
  this->valid_ = true;
}

inline void Value::SetInt32(int32 i32) {
  reset();
  value_.i32 = i32;
  this->type_.reset(new Type(kInt32));
  this->assigned_ = true;
  this->valid_ = true;
}

inline void Value::SetFloat(float f) {
  reset();
  value_.f = f;
  this->type_.reset(new Type(kFloat));
  this->assigned_ = true;
  this->valid_ = true;
}

inline void Value::SetDouble(double d) {
  reset();
  value_.d = d;
  this->type_.reset(new Type(kDouble));
  this->assigned_ = true;
  this->valid_ = true;
}

inline void Value::SetString(const std::string& str) {
  reset();
  NOTREACHED();
}

inline void Value::SetVec2(const float *v) {
  DCHECK_EQ(type_->type(), kVector2);
}

inline void Value::SetVec3(const float *v) {
  DCHECK_EQ(type_->type(), kVector3);
}

inline void Value::SetVec4(const float *v) {
  DCHECK_EQ(type_->type(), kVector4);
}

inline void Value::SetType(TypePtr type) {
  reset();
  type_ = type;
}

inline Value& Value::operator += (const Value& v) {
  Value ret = *this + v;
  *this = ret;
  return *this;
}

inline Value& Value::operator -= (const Value& v) {
  Value ret = *this - v;
  *this = ret;
  return *this;
}

inline Value& Value::operator *= (const Value& v) {
  Value ret = *this * v;
  *this = ret;
  return *this;
}

inline Value& Value::operator /= (const Value& v) {
  Value ret = *this / v;
  *this = ret;
  return *this;
}

inline Value& Value::operator %= (const Value& v) {
  Value ret = *this % v;
  *this = ret;
  return *this;
}

inline Value& Value::operator &= (const Value& v) {
  Value ret = *this & v;
  *this = ret;
  return *this;
}

inline Value& Value::operator |= (const Value& v) {
  Value ret = *this | v;
  *this = ret;
  return *this;
}

inline Value& Value::operator ^= (const Value& v) {
  Value ret = *this ^ v;
  *this = ret;
  return *this;
}

inline Value& Value::operator >>= (const Value& v) {
  Value ret = *this >> v;
  *this = ret;
  return *this;
}

inline Value& Value::operator <<= (const Value& v) {
  Value ret = *this << v;
  *this = ret;
  return *this;
}

inline bool Value::GetBoolean() const {
  DCHECK(type_.get() && type_->type() == kBoolean);
  return value_.b;
}

inline char Value::GetChar() const {
  DCHECK(type_.get() && type_->type() == kChar);
  return value_.c;
}

inline short Value::GetShort() const {
  DCHECK(type_.get() && type_->type() == kShort);
  return value_.s;
}

inline int32 Value::GetInt32() const {
  DCHECK(type_.get() && type_->type() == kInt32);
  return value_.i32;
}

inline uint32 Value::GetUint32() const {
  DCHECK(type_.get() && type_->type() == kUint32);
  return value_.u32;
}

inline float Value::GetFloat() const {
  DCHECK(type_.get() && type_->type() == kFloat);
  return value_.f;
}

inline float Value::GetDouble() const {
  DCHECK(type_.get() && type_->type() == kDouble);
  return value_.d;
}

inline const float* Value::GetVec() const {
  DCHECK(type_.get() && type_->IsVector());
  return value_.vec;
}

inline const int* Value::GetIntVec() const {
  DCHECK(type_.get() && type_->IsIntVec());
  return value_.intvec;
}

inline int32 GetInteger(const ValuePtr& value) {
  CHECK(value->type().get() != NULL);
  CHECK(IsIntegerScalar(value->type()));
  const TypePtr& type = value->type(); 
  if (type->type() == kInt32) {
    return value->GetInt32();
  } else if (type->type() == kUint32) {
    return value->GetUint32();
  } else if (type->type() == kShort) {
    return value->GetShort();
  } else {
    NOTREACHED();
    return 0;
  }
}
}  // namespace afx
}  // namespace azer
