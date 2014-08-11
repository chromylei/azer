#include "azer/afx/codegen/hlsl_util.h"

#include <sstream>
#include <string>

#include "azer/afx/codegen/code_generator.h"
#include "azer/afx/codegen/hlsl_codegen.h"
#include "base/strings/string_util.h"

namespace azer {
namespace afx {
std::string DumpStorageQualifier(StorageQualifier qualifier) {
  switch (qualifier) {
    case kNoQualifier:     return "";
    case kIn:              return "in";
    case kOut:             return "out";
    case kInout:           return "inout";
    case kConst:           return "const";
    case kUniform:         return "";
    default: NOTREACHED(); return "";
  }
}

std::string HLSLTypeName(const TypePtr& type) {
  switch (type->type()) {
    case kVoid: return "void";
    case kChar: return "char";
    case kString: return "string";
    case kShort: return "short";
    case kInt32: return "int";
    case kUint32: return "uint";
    case kBoolean: return "bool";
    case kFloat: return "float";
    case kVector2: return "float2";
    case kVector3: return "float3";
    case kVector4: return "float4";
    case kIntVec2: return "int2";
    case kIntVec3: return "int3";
    case kIntVec4: return "int4";
    case kMatrix2: return "float2x2";
    case kMatrix3: return "float3x3";
    case kMatrix4: return "float4x4";
    case kTexture1D: return "Texture1D";
    case kTexture1DArray: return "Texture1DArray";
    case kTexture2D: return "Texture2D";
    case kTexture2DArray: return "Texture2DArray";
    case kTexture3D: return "Texture3D";
    case kTextureCube: return "Texturecube";
    case kStructure: return std::move(ReplaceDoubleColon(type->name()));
    case kFunctional: return std::move(ReplaceDoubleColon(type->name()));
    default: NOTREACHED() << "Unknown ASTNodeType"; return "";
  }
}

std::string DumpArraySpecifier(const TypePtr& type) {
  if (type->IsArray()) {
    DCHECK(type->IsArray());
    std::stringstream ss;
    for (auto iter = type->GetDimVec().begin();
         iter != type->GetDimVec().end(); ++iter) {
      ss << "[";
      if (*iter > 0) {
        ss << *iter;
      }
      ss << "]";
    }
    return ss.str();
  } else {
    return "";
  }
}

std::string DumpFullType(const TypePtr& type) {
  std::stringstream ss;
  if (type->storage_qualifier() != kNoQualifier) {
    ss << DumpStorageQualifier(type->storage_qualifier()) << " ";
  }

  ss << HLSLTypeName(type);
  return ss.str();
}

std::string ReplaceDoubleColon(const std::string& type) {
  if (std::string::npos != type.find("::")) {
    std::string fullname;
    ::base::ReplaceChars(type, ":", "_", &fullname);
    return fullname;
  } else {
    return type;
  }
}

std::string PackagePrefix(ParseContext* context) {
  std::string prefix;
  if (!context->package().empty()) {
    ::base::ReplaceChars(context->package(), ":", "_", &prefix);
    prefix.append("__");
  }
  return prefix;
}

std::string AttributeSupfix(const std::string& dname, AttributesNode* attrnode) {
  std::string sename;
  if (attrnode) {
    sename.append(":");
    if (attrnode->GetAttrValue("system") == "true") {
      sename.append("SV_");
    }
    std::string semantic = attrnode->GetAttrValue("semantic");
    if (semantic.empty()) {
      sename.append(StringToUpperASCII(dname));
    } else {
      sename.append(StringToUpperASCII(semantic));
    }
    std::string semantic_index = attrnode->GetAttrValue("semantic_index");
    sename.append(semantic_index);
  }
  return sename;
}

}  // namespace afx
}  // namespace azer
