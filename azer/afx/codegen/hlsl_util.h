#pragma once

#include "azer/afx/compiler/type.h"
#include "azer/afx/compiler/astnode.h"
#include "azer/afx/compiler/context.h"

namespace azer {
namespace afx {
std::string DumpStorageQualifier(StorageQualifier qualifier);
std::string DumpArrayDim(ASTNode* node);
std::string HLSLTypeName(const TypePtr& type);
std::string DumpArraySpecifier(const TypePtr& type);
std::string HLSLDumpFullType(TypedNode* node);
std::string ReplaceDoubleColon(const std::string& type);
std::string PackagePrefix(ParseContext* context);
std::string AttributeSupfix(const std::string& default_name,
                            AttributesNode* attrnode);

}  // namespace afx
}  // namespace azer
