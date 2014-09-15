#include "azer/afx/compiler/context_validator.h"

#include <sstream>
#include "azer/afx/compiler/astnode.h"
#include "azer/afx/compiler/builtin.h"
#include "azer/afx/compiler/context.h"
#include "azer/afx/codegen/tex_util.h"
#include "azer/afx/compiler/errno.h"

namespace azer {
namespace afx {
// class ContextValidator

bool ContextValidator::Valid(ASTNode* node) {
  if (failed_) {
    return !failed_;
  } else if (node->IsRefSymbolNode()) {
    // lookup symbol declaratin astnode
    if (!LookupSymbolDecl(node->ToRefSymbolNode())) {
      return false;
    }

    if (!CheckRefTexture(node->ToRefSymbolNode())) {
      return false;
    }
  } else if (node->IsSymbolNode()) {
    SymbolNode* symbol = node->ToSymbolNode();
    // add symbol into symbol_table
    if (!AddSymbolToScoped(symbol)) return false;
    // check type
    if (!LookupTypeDecl(symbol)) return false;
    if (!CheckSymbolTexture(symbol)) return false;
  } else if (node->IsFuncCallNode()) {
    // attention:
    FuncCallNode* func = node->ToFuncCallNode();
    if (node->type() != ASTNode::kFuncCallTypeInitNode) {
      if (!LookupFunctionDecl(func)) {
        return false;
      }
    }
  } else if (node->IsBinaryOpNode()) {
    BinaryOpNode* binary = node->ToBinaryOpNode();
    if (binary->GetOperator() == kOpMember) {
      DCHECK(binary->GetOper2()->IsFieldNode());
      TypePtr oper1_typeptr = GetNodeType(binary->GetOper1());
      LookupField(binary->GetOper1(), binary->GetOper2()->ToFieldNode());
    }
  } else if (node->IsDeclarationNode()) {
    DeclarationNode* decl = node->ToDeclarationNode();
    if (!ValidUniformDeclarationNode(decl)) return false;
  } else if (node->IsFieldNode()) {
  }
  return !failed_;
}

bool ContextValidator::LookupField(ASTNode* node, FieldNode* field) {
  TypedNode* typed = GetTypedNode(node);
  if (typed == NULL) {
    std::stringstream ss;
    ss << " no operator \".\" supported.";
    ReportError(field, ss.str());
    return false;
  }
  
  DCHECK(typed->GetType().get() != NULL);
  if (typed->GetType()->IsVector()) {
    // IntVec not support
    // swizze expression
    const SourceLoc& loc = field->loc();
    field->SetTypedNode(field->GetContext()->Create(ASTNode::kTypedNode, loc));
    if (field->fieldname().length() == 1u) {
      field->GetTypedNode()->SetBasicType(kFloat);
      return true;
    } else if (field->fieldname().length() == 2u) {
      field->GetTypedNode()->SetBasicType(kVector2);
      return true;
    } else if (field->fieldname().length() == 3u) {
      field->GetTypedNode()->SetBasicType(kVector3);
      return true;
    } else if (field->fieldname().length() == 4u) {
      field->GetTypedNode()->SetBasicType(kVector4);
      return true;
    } else {
      return false;
    }
  } else if (typed->GetType()->IsStructure()) {
    StructDeclNode* decl = typed->GetStructDecl();
    DCHECK(decl != NULL);
    FieldNode* declfield = decl->GetField(field->fieldname());
    if (!declfield) {
      std::stringstream ss;
      ss << "structure \"" << decl->struct_name() <<  "\" has no field named \""
         << field->fieldname() << "\"";
      ReportError(field, ss.str());
      return false;
    }
    field->SetTypedNode(declfield->GetTypedNode());
    return true;
  } else {
    std::stringstream ss;
    ss << "operator \".\" is not supported, near by \"" << field->fieldname()
       << "\"";
    ReportError(field, ss.str());
    return false;
  }
}

bool ContextValidator::LookupTypeDecl(SymbolNode* node) {
  DCHECK(node->IsSymbolNode());
  if (!node->GetType()->IsStructure()) return true;
  if (node->GetType()->IsAnomyousStruct()) return true;

  const std::string& type_name = node->GetType()->name();
  ASTNode* symbol = context_->LookupType(type_name);

  if (symbol != NULL) {
    TypedNode* typed = node->GetTypedNode();
    DCHECK(NULL != typed);
    typed->SetStructDecl(symbol->ToStructDeclNode());
    return true;
  } else {
    std::stringstream ss;
    ss << "cannot find type \"" << type_name << "\"'s declaration.";
    ReportError(node, ss.str());
    return false;
  }
}

bool ContextValidator::LookupSymbolDecl(RefSymbolNode* node) {
  DCHECK_EQ(node->type(), ASTNode::kRefSymbolNode);
  ScopedNode* scoped = GetScopedNode(node);
  ASTNode* decl_node = scoped->LookupSymbol(node->symbolname());
  if (decl_node == NULL) {
    std::stringstream ss;
    ss << "symbol \"" << node->symbolname() << "\" not declared.";
    ReportError(node, ss.str());
    return false;
  }

  if (node->GetDeclNode() == NULL) {
    node->SetDeclNode((SymbolNode*)decl_node);
  } else {
    CHECK_EQ(decl_node, node->GetDeclNode());
  }
  return true;
}

bool ContextValidator::LookupFunctionDecl(FuncCallNode* node) {
  DCHECK(context_ != NULL);
  ASTNode* funcdef = BuiltinContext::Lookup(node->funcsync());
  if (!funcdef) {
    funcdef = context_->LookupFunction(node->funcsync());
  }

  if (funcdef != NULL) {
    node->SetFuncDefNode(funcdef->ToFuncDefNode());
    return true;
  } else {
    std::stringstream ss;
    ss << "cannot find function \"" << node->funcsync() << "\"'s definition.";
    ReportError(node, ss.str());
    return false;
  }
}

bool ContextValidator::AddSymbolToScoped(SymbolNode* node) {
  DCHECK(node->IsSymbolNode());
  ScopedNode* scoped = GetScopedNode(node);
  ASTNode* decl = scoped->LookupSymbolLocally(node->symbolname());
  if (decl != NULL) {
    std::stringstream ss;
    ss << "symbol " << node->symbolname() << " has been declared"
       << " at line " << decl->loc().lineno;
    ReportError(node, ss.str());
    return false;
  } else {
    scoped->RegisteSymbol(node);
    return true;
  }
}

/**
 * get the scoped node
 * for lookup symbol or registe symbol
 */
ScopedNode* ContextValidator::GetScopedNode(ASTNode* node) {
  ASTNode* cur = node;
  while (cur->type() != ASTNode::kScopedNode) {
    cur = cur->parent();
    CHECK(cur != NULL);
  }

  DCHECK(cur != NULL);
  DCHECK_EQ(cur->type(), ASTNode::kScopedNode);
  return (ScopedNode*)cur;
}

bool ContextValidator::ValidUniformDeclarationNode(DeclarationNode* decl) {
  if (decl->GetType()->storage_qualifier() == kUniform) {
    if (decl->first_child()->IsAttributesNode()) {
      DCHECK(decl->last_child()->IsSymbolNode());
      if( decl->last_child()->prev_sibling() != decl->first_child()) {
        std::string msg = "uniform declaration can only has one variable";
        ReportError(decl, msg, kMultiUniformDeclInStatement);
        return false;
      }
    } else if (decl->first_child()->IsSymbolNode()) {
      if (decl->first_child() != decl->last_child()) {
        std::string msg = "uniform declaration can only has one variable";
        ReportError(decl, msg, kMultiUniformDeclInStatement);
        return false;
      }
    } else {
      NOTREACHED();
      return false;
    }
  }

  return true;
}

bool ContextValidator::CheckSymbolTexture(SymbolNode* symbol) {
  DCHECK(symbol->IsSymbolNode());
  if (symbol->GetType()->IsStructure()) {
    TypedNode* typed = symbol->GetTypedNode();
    return !HasTextureField(symbol->GetTypedNode()->GetStructDecl());
  }
  return true;
}

bool ContextValidator::CheckRefTexture(RefSymbolNode* reftype) {
  DCHECK(reftype->IsRefSymbolNode());
  DCHECK(reftype->GetDeclNode() != NULL);
  SymbolNode* decl = reftype->GetDeclNode();
  if (decl->GetType()->IsTexture()) {
    if (decl->GetType()->storage_qualifier() & kUniform) {
      return true;
    } else if (decl->IsActParamNode()) {
      // actural param of function
      return true;
    } else {
      std::stringstream ss;
      ss << reftype->symbolname() << "'s texture must be uniform";
      ReportError(reftype, ss.str(), kTextureMustbeUniformed);
    }
  }
  return true;
}

void ContextValidator::ReportError(ASTNode* node, const std::string& msg, int err) {
  DCHECK(context_ != NULL);
  context_->ReportError(node->loc(), msg, (CompileErrno)err);
  failed_ = true;
}

void ContextValidator::ReportError(ASTNode* node, const std::string& msg) {
  DCHECK(context_ != NULL);
  context_->ReportError(node->loc(), msg);
  failed_ = true;
}
}  // namespace afx
}  // namespace azer
