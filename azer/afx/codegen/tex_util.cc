#include "azer/afx/codegen/tex_util.h"
#include "azer/afx/compiler/astnode.h"
#include "azer/afx/codegen/hlsl_codegen.h"
#include "azer/afx/codegen/hlsl_util.h"
#include "base/logging.h"

namespace azer {
namespace afx {
namespace {

const char* AfxD3DSamplerSupfix = "__d3d_sampler";

class TextureNameGenerator : public TreeNode<ASTNode>::Traverser {
 public:
  TextureNameGenerator() {}

  void GenCode(ASTNode* node);
  const std::string& GetName() const { return code_gen_;}
  const std::vector<std::string>& GetFields() const { return fields_;}
 protected:
  virtual bool OnVisitBegin(ASTNode* node) OVERRIDE;
  virtual void OnVisitEnd(ASTNode* node) OVERRIDE;
 private:
  std::string code_gen_;
  std::vector<std::string> fields_;
  DISALLOW_COPY_AND_ASSIGN(TextureNameGenerator);
};

void TextureNameGenerator::GenCode(ASTNode* node) {
  code_gen_.clear();
  node->traverse(this);
}

bool TextureNameGenerator::OnVisitBegin(ASTNode* node) {
  if (node->IsBinaryOpNode()) {
    TextureNameGenerator gen1;
    TextureNameGenerator gen2;
    BinaryOpNode* binary = node->ToBinaryOpNode();
    if (binary->GetOperator() == kOpMember) {
      gen1.GenCode(binary->GetOper1());
      gen2.GenCode(binary->GetOper2());
      code_gen_.append(gen1.GetName());
      code_gen_.append("_");
      code_gen_.append(gen2.GetName());

      fields_.push_back(gen1.GetName());
      fields_.push_back(gen2.GetName());
    } else {
      NOTREACHED();
    }
    return false;
  } else if (node->IsRefSymbolNode()) {
    RefSymbolNode* symbol = node->ToRefSymbolNode();
    code_gen_.append(symbol->symbolname());
    fields_.push_back(symbol->symbolname());
    return false;
  } else if (node->IsFieldNode()) {
    FieldNode* field = node->ToFieldNode();
    code_gen_.append("_");
    code_gen_.append(field->fieldname());
    fields_.push_back(field->fieldname());
    return false;
  } else {
    NOTREACHED();
    return false;
  }
}

void TextureNameGenerator::OnVisitEnd(ASTNode* node)  {
}
}

std::string GenReferredTextureVarName(ASTNode* node) {
  DCHECK(IsParameterTextureNode(node));
  if (node->IsRefSymbolNode()) {
    return node->ToRefSymbolNode()->symbolname();
  } else if (node->IsSymbolNode()) {
    return node->ToSymbolNode()->symbolname();
  } else if (node->IsParamNode()) {
    return node->ToParamNode()->paramname();
  } else if (node->IsBinaryOpNode()) {
    TextureNameGenerator gen;
    BinaryOpNode* binary = node->ToBinaryOpNode();
    DCHECK(binary);
    gen.GenCode(binary->GetOper1());
    return gen.GetName();
  } else {
    NOTREACHED();
    return "";
  }
}

std::string HLSLUniformTextureSamplerDeclFullName(ASTNode* node) {
  std::stringstream ss;
  ss << GenReferredTextureVarName(node) << AfxD3DSamplerSupfix;
  TypedNode* typed = GetTexNode(node);
  if (typed->GetType()->IsArray()) {
    ss << DumpArraySpecifier(typed->GetType());
  }
  return ss.str();
}

std::string HLSLDeclaredTextureFullName(ASTNode* node) {
  DCHECK(IsParameterTextureNode(node));
  TypedNode* typed = GetTexNode(node);
  DCHECK(typed != NULL);
  std::stringstream ss;
  ss << GenReferredTextureVarName(node);
  if (typed->GetType()->IsArray()) {
    ss << DumpArraySpecifier(typed->GetType());
  }

  return ss.str();
}

TypedNode* GetTexNode(ASTNode* node) {
  DCHECK(IsParameterTextureNode(node));
  if (node->IsRefSymbolNode()) {
    SymbolNode* symbol = node->ToRefSymbolNode()->GetDeclNode();
    return symbol->GetTypedNode();
  } else if (node->IsSymbolNode()) {
    SymbolNode* symbol = node->ToSymbolNode();
    return symbol->GetTypedNode();
  } else if (node->IsFieldNode()) {
    FieldNode* field = node->ToFieldNode();
    return field->GetTypedNode();
  } else if (node->IsBinaryOpNode()) {
    BinaryOpNode* binary = node->ToBinaryOpNode();
    if (binary->GetOperator() == kOpMember) {
      return GetTexNode(binary->GetOper2());
    } else if (binary->GetOperator() == kOpIndex) {
      return GetTexNode(binary->GetOper1());
    } else {
      NOTREACHED();
      return NULL;
    }
  } else {
    NOTREACHED();
    return NULL;
  }
}

TypePtr GetTextureType(ASTNode* node) {
  DCHECK(IsParameterTextureNode(node));
  if (node->IsRefSymbolNode()) {
    SymbolNode* symbol = node->ToRefSymbolNode()->GetDeclNode();
    TypePtr ptr = symbol->GetType();
    return ptr;
  } else if (node->IsSymbolNode()) {
    SymbolNode* symbol = node->ToSymbolNode();
    TypePtr ptr = symbol->GetType();
    return ptr;
  } else if (node->IsBinaryOpNode()) {
    TypePtr ptr = node->ToBinaryOpNode()->GetResultType();
    return ptr;
  } else {
    NOTREACHED();
    return NULL;
  }
}

std::string HLSLTextureTypeName(ASTNode* node) {
  DCHECK(IsParameterTextureNode(node));
  TypePtr ptr = GetTextureType(node);
  return HLSLTypeName(ptr);
}

namespace {
}

std::string HLSLRefferedTextureFullName(ASTNode* node) {
  std::stringstream ss;
  ss << GenReferredTextureVarName(node);
  if (node->IsBinaryOpNode()) {
    BinaryOpNode* binary = node->ToBinaryOpNode();
    if (binary->GetOperator() == kOpIndex) {
      HLSLCodeGeneratorFactory factory;
      SnippetCodeGenerator codegen(&factory);
      codegen.GenCode(binary->GetOper2());
      ss << "[" << codegen.GetCode() << "]";
    } else if (binary->GetOperator() == kOpMember) {
    } else {
      NOTREACHED();
    }
  }

  return ss.str();
}

bool IsTextureArray(ASTNode* node) {
  if (node->IsRefSymbolNode()) {
    SymbolNode* symbol = node->ToRefSymbolNode()->GetDeclNode();
    TypedNode* typed = symbol->GetTypedNode();
    DCHECK(typed != NULL);
    
    return typed->IsArrayTyped();
  } else if (node->IsFieldNode()) {
    FieldNode* field = node->ToFieldNode();
    TypedNode* typed = field->GetTypedNode();
    DCHECK(typed != NULL);
    
    return typed->IsArrayTyped();
  } else if (node->IsBinaryOpNode()) {
    BinaryOpNode* binary = node->ToBinaryOpNode();
    if (binary->GetOperator() == kOpMember) {
      return IsTextureArray(binary->GetOper2());
    } else if (binary->GetOperator() == kOpIndex) {
      return IsTextureArray(binary->GetOper1());
    } else {
      NOTREACHED();
      return false;
    }
  } else {
    NOTREACHED();
    return false;
  }
}

void GetTexRefferPath(ASTNode* node, std::vector<std::string>* path) {
  DCHECK(IsParameterTextureNode(node));
  if (node->IsRefSymbolNode()) {
    path->push_back(node->ToRefSymbolNode()->symbolname());
    return;
  } else if (node->IsSymbolNode()) {
    path->push_back(node->ToSymbolNode()->symbolname());
    return;
  } else if (node->IsBinaryOpNode()) {
    TextureNameGenerator gen;
    BinaryOpNode* binary = node->ToBinaryOpNode();
    DCHECK(binary);
    gen.GenCode(binary->GetOper1());
    *path = gen.GetFields();
  } else {
    NOTREACHED();
  }
}

std::string HLSLUniformTextureSamplerFullName(ASTNode* node) {
  std::stringstream ss;
  ss << GenReferredTextureVarName(node) << AfxD3DSamplerSupfix;
  if (node->IsBinaryOpNode()) {
    BinaryOpNode* binary = node->ToBinaryOpNode();
    if (binary->GetOperator() == kOpIndex) {
      HLSLCodeGeneratorFactory factory;
      SnippetCodeGenerator codegen(&factory);
      codegen.GenCode(binary->GetOper2());
      ss << "[" << codegen.GetCode() << "]";
    } else if (binary->GetOperator() == kOpMember) {
    } else {
      NOTREACHED();
    }
  }

  return ss.str();
}

std::string HLSLTextureArraySpecifier(ASTNode* node) {
  if (node->IsRefSymbolNode()) {
    SymbolNode* symbol = node->ToRefSymbolNode()->GetDeclNode();
    TypedNode* typed = symbol->GetTypedNode();
    DCHECK(typed != NULL);
    
    if (typed->IsArrayTyped()) {
      return DumpArraySpecifier(typed->GetType());
    } else {
      return "";
    }
  } else if (node->IsFieldNode()) {
    FieldNode* field = node->ToFieldNode();
    TypedNode* typed = field->GetTypedNode();
    DCHECK(typed != NULL);
    
    if (typed->IsArrayTyped()) {
      return DumpArraySpecifier(typed->GetType());
    } else {
      return "";
    }
  } else if (node->IsBinaryOpNode()) {
    BinaryOpNode* binary = node->ToBinaryOpNode();
    if (binary->GetOperator() == kOpMember) {
      return HLSLTextureArraySpecifier(binary->GetOper2());
    } else if (binary->GetOperator() == kOpIndex) {
      return HLSLTextureArraySpecifier(binary->GetOper1());
    } else {
      NOTREACHED();
      return "";
    }
  } else {
    NOTREACHED();
    return "";
  }
}

ASTNode* GetTexDeclNode(ASTNode* node) {
  if (node->IsRefSymbolNode()) {
    SymbolNode* symbol = node->ToRefSymbolNode()->GetDeclNode();
    return symbol;
  } else if (node->IsFieldNode()) {
    if (node->parent() && node->parent()->IsBinaryOpNode()) {
      return GetTexDeclNode(node->parent());
    } else {
      NOTREACHED();
      return NULL;
    }
  } else if (node->IsBinaryOpNode()) {
    BinaryOpNode* binary = node->ToBinaryOpNode();
    if (binary->GetOperator() == kOpMember) {
      return GetTexDeclNode(binary->GetOper1());
    } else if (binary->GetOperator() == kOpIndex) {
      return GetTexDeclNode(binary->GetOper1());
    } else {
      NOTREACHED();
      return NULL;
    }
  } else {
    NOTREACHED();
    return NULL;
  }
}

bool IsMaterialTex(ASTNode* node) {
  ASTNode* decl = GetTexDeclNode(node);
  DCHECK(decl != NULL);
  DCHECK(decl->IsSymbolNode());
  SymbolNode* symbol = decl->ToSymbolNode();
  TypedNode* typed = symbol->GetTypedNode();
  DCHECK(typed != NULL);
  if (typed->GetType()->IsStructure()) {
    StructDeclNode* decl = typed->GetStructDecl();
    return decl->GetContext()->package() == "afx"
        && decl->struct_name() == "Material";
  } else {
    return false;
  }
}

std::string HLSLTextureUniformFuncName(ASTNode* node) {
  if (node->IsRefSymbolNode()) {
    SymbolNode* symbol = node->ToRefSymbolNode()->GetDeclNode();
    DeclarationNode* decl = symbol->GetDeclarationNode();
    DCHECK(decl->GetType()->IsTexture());
    DCHECK(decl->last_child()->IsSymbolNode());
    if (decl->attributes() && decl->attributes()->HasAttr("uniform_func")) {
      return decl->attributes()->GetAttrValue("uniform_func");
    } else {
      return "";
    }
  } else if (node->IsFieldNode()) {
    FieldNode* field = node->ToFieldNode();
    DCHECK(field->GetType()->IsTexture());
    if (field->attributes() && field->attributes()->HasAttr("uniform_func")) {
      return field->attributes()->GetAttrValue("uniform_func");
    } else {
      return "";
    }
  } else if (node->IsBinaryOpNode()) {
    BinaryOpNode* binary = node->ToBinaryOpNode();
    if (binary->GetOperator() == kOpMember) {
      return HLSLTextureUniformFuncName(binary->GetOper2());
    } else if (binary->GetOperator() == kOpIndex) {
      return HLSLTextureUniformFuncName(binary->GetOper1());
    } else {
      NOTREACHED();
      return "";
    }
  } else {
    NOTREACHED();
    return "";
  }
}

bool IsParameterTextureNode(ASTNode* node) {
  if (node->IsBinaryOpNode()) {
    TypePtr ptr = node->ToBinaryOpNode()->GetResultType();
    DCHECK(ptr.get() != NULL);
    if (ptr->IsTexture()) {
      return true;
    } else {
      return false;
    }
  } else if (node->IsSymbolNode()) {
    // to generator SamplerState, symbolNode maybe as a parameters
    SymbolNode* symbol = node->ToSymbolNode();
    DCHECK(symbol != NULL);
    TypePtr ptr = symbol->GetType();
    DCHECK(ptr.get() != NULL);
    if (ptr->IsTexture()) {
      return true;
    } else {
      return false;
    }
  } else if (node->IsRefSymbolNode()) {
    SymbolNode* symbol = node->ToRefSymbolNode()->GetDeclNode();
    DCHECK(symbol != NULL);
    TypePtr ptr = symbol->GetType();
    DCHECK(ptr.get() != NULL);
    if (ptr->IsTexture()) {
      return true;
    } else {
      return false;
    }
  } else if (node->IsFieldNode()) {
    TypedNode* typed = node->ToFieldNode()->GetTypedNode();
    return typed->GetType()->IsTexture();
  } else if (node->IsParamNode()) {
    TypedNode* typed = node->ToParamNode()->GetTypedNode();
    return typed->GetType()->IsTexture();
  } else if (node->IsConstNode()) {
    return false;
  } else if (node->IsFuncCallNode()) {
    return false;
  } else if (node->IsUnaryOpNode()) {
    return IsParameterTextureNode(node->ToUnaryOpNode()->GetOper());
  } else {
    NOTREACHED();
    return false;
  }

  return false;
}

bool HasTextureField(ASTNode* node) {
  DCHECK(node->IsStructDeclNode());
  ASTNode* cur = node->first_child();
  while (cur) {
    if (cur->IsFieldNode()) {
      FieldNode* field = cur->ToFieldNode();
      if (field->GetType()->IsTexture()) {
        return true;
      } else if (field->GetType()->IsStructure()) {
        return HasTextureField(field->GetTypedNode()->GetStructDecl());
      }
    }
    cur = cur->next_sibling();
  }
  return false;
}

bool HasOnlyTextureField(ASTNode* node) {
  DCHECK(node->IsStructDeclNode());
  ASTNode* cur = node->first_child();
  while (cur) {
    if (cur->IsFieldNode()) {
      FieldNode* fieldnode = cur->ToFieldNode();
      if (!fieldnode->GetType()->IsTexture()) {
        return false;
      }
    }
    cur = cur->next_sibling();
  }
  return true;
}
}  // namespace afx
}  // namespace azer
