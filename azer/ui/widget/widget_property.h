#pragma once

#include <map>
#include <memory>

#include "base/basictypes.h"
#include "base/logging.h"
#include "azer/ui/widget/export.h"
#include "azer/base/string.h"

namespace azer {
namespace ui {

class AZER_WIDGET_EXPORT PropertyValue {
 public:
  enum Type {
    kUnknown,
    kInt64,
    kUint64,
    kBoolean,
    kFloat,
    kDouble,
    kString,
  };

  PropertyValue();
  explicit PropertyValue(uint32 v);
  explicit PropertyValue(uint64 v);
  explicit PropertyValue(int32 v);
  explicit PropertyValue(int64 v);
  explicit PropertyValue(bool v);
  explicit PropertyValue(float v);
  explicit PropertyValue(double v);
  explicit PropertyValue(const StringType& str);
  
  int64 GetInt64() const { CheckType(kInt64); return value_.i64;}
  uint64 GetUint64() const { CheckType(kUint64); return value_.u64;}
  bool GetBoolean() const { CheckType(kBoolean); return value_.b;}
  float GetFloat() const { CheckType(kFloat); return value_.f;}
  double GetDouble() const { CheckType(kDouble); return value_.d;}
  const StringType& GetString() const { CheckType(kString); return str_;}
  Type type() const { return type_;}
 private:
  void CheckType(Type type) const;
  union {
    int64  i64;
    uint64 u64;
    bool  b;
    float  f;
    double d;
  } value_;
  StringType str_;
  Type type_;

  friend class WidgetProperty;
  DISALLOW_COPY_AND_ASSIGN(PropertyValue);
};

typedef std::shared_ptr<PropertyValue> PropertyValuePtr;

class AZER_WIDGET_EXPORT WidgetProperty {
 public:
  WidgetProperty() {}
  typedef std::map<std::string, PropertyValuePtr> PropertyDict;

  const PropertyValuePtr& GetValue(const std::string& name) const;
  PropertyValuePtr& GetValue(const std::string& name);

  void SetValue(const std::string& name, PropertyValuePtr& val);
  static PropertyValuePtr null_;
 private:
  PropertyDict dict_;
  DISALLOW_COPY_AND_ASSIGN(WidgetProperty);
};

inline PropertyValue::PropertyValue()
    : type_(kUnknown) {
  value_.u64 = 0;
}

inline PropertyValue::PropertyValue(uint32 v)
    : type_(kUint64) {
  value_.u64 = v;
}

inline PropertyValue::PropertyValue(uint64 v)
    : type_(kUint64) {
  value_.u64 = v;
}

inline PropertyValue::PropertyValue(int32 v)
    : type_(kInt64) {
  value_.u64 = v;
}

inline PropertyValue::PropertyValue(int64 v)
    : type_(kInt64) {
  value_.u64 = v;
}

inline PropertyValue::PropertyValue(bool v)
    : type_(kBoolean) {
  value_.u64 = v;
}

inline PropertyValue::PropertyValue(float v)
    : type_(kFloat) {
  value_.u64 = v;
}

inline PropertyValue::PropertyValue(double v)
    : type_(kDouble) {
  value_.u64 = v;
}

inline PropertyValue::PropertyValue(const StringType& str)
    : str_(str)
    , type_(kString) {
  value_.d = 0;
}

inline void PropertyValue::CheckType(Type type) const {
  CHECK(type_ == type);
}

inline const PropertyValuePtr& WidgetProperty::GetValue(
    const std::string& name) const {
  auto iter = dict_.find(name);
  if (iter != dict_.end()) {
    return iter->second;
  } else {
    return null_;
  }
}
    
inline PropertyValuePtr& WidgetProperty::GetValue(const std::string& name) {
  auto iter = dict_.find(name);
  if (iter != dict_.end()) {
    return iter->second;
  } else {
    return null_;
  }
}

inline void WidgetProperty::SetValue(const std::string& name,
                                     PropertyValuePtr& val) {
  dict_[name] = val;
}
}  // namespace ui
}  // namespace azer
