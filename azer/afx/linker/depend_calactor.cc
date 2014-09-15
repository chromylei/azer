#include "azer/afx/linker/depend_calactor.h"

#include <algorithm>
#include "azer/afx/compiler/astnode.h"
#include "azer/afx/compiler/context.h"
#include "azer/afx/codegen/tex_util.h"

namespace azer {
namespace afx {

namespace {
struct UniformSizeSort {
  bool operator() (ASTNode* node1, ASTNode* node2) const {
    DCHECK(node1 && node1->IsDeclarationNode());
    DCHECK(node2 && node2->IsDeclarationNode());
    DCHECK(node1->ToDeclarationNode()->first_child());
    DCHECK(node2->ToDeclarationNode()->first_child());
    SymbolNode* s1 = node1->ToDeclarationNode()->last_child()->ToSymbolNode();
    SymbolNode* s2 = node2->ToDeclarationNode()->last_child()->ToSymbolNode();
        
    return SizeofType(*s1->GetType().get()) > SizeofType(*s2->GetType().get());
  }
};
bool IsUniformSymbol(ASTNode* node);
}  // namespace

void DependCalactor::Swap(TechniqueParser::StageInfo* stage) {
  depend_.swap(stage->depend);
  uniforms_.swap(stage->uniforms);
  std::sort(stage->uniforms.begin(), stage->uniforms.end(), UniformSizeSort());
  textures_.swap(stage->uni_textures);
  uni_depend_.swap(stage->uni_depend);
  inserted_.clear();
  tex_inserted_.clear();
}

bool DependCalactor::Add(const std::string& name, ASTNode* node) {
  DCHECK(node != NULL);
  if (inserted_.find(name) != inserted_.end()) return false;
  inserted_.insert(name);
  if (IsUniformSymbol(node)) {
    DCHECK(node->IsDeclarationNode());
    DeclarationNode* decl = node->ToDeclarationNode();
    if (!decl->GetType()->IsTexture()) {
      uniforms_.push_back(decl);
    }
  } else {
    depend_.push_back(node);
  }
  return true;
}

void DependCalactor::AddFuncDep(ASTNode* node) {
  DCHECK(node->IsFuncCallNode());
  FuncCallNode* callnode = node->ToFuncCallNode();
  if (node->type() == ASTNode::kFuncCallTypeInitNode) {
    return;
  } else if (callnode->IsBuiltIn()) {
    return;
  }

  DCHECK(callnode->GetFuncDefNode() != NULL);
  Add(node->ToFuncCallNode()->funcname(), callnode->GetFuncDefNode());
}

void DependCalactor::AddStructDeclDep(ASTNode* node) {
  DCHECK(node->GetContext());
  DCHECK(node->IsStructDeclNode());
  Add(StructFullName(node), node);
}

void DependCalactor::AddFieldNodeDep(ASTNode* node) {
  ASTNode* pnode = node->parent();
  // if try to access a symbol's member, need to calculator
  // the dependency of the field
  if (pnode->IsBinaryOpNode()) return;
  DCHECK(node->IsFieldNode());
  FieldNode* field = node->ToFieldNode();
  DCHECK(field->GetType().get() != NULL);
  AddTypeDep(field->GetType(), node);
}

namespace {
bool IsGlobalVariable(SymbolNode* node) {
  DCHECK(node->parent());
  
  if(!node->parent()->IsDeclarationNode()) {
    // parameter of function
    return false;
  }

  DCHECK(node->GetContext());
  return node->parent()->parent() == node->GetContext()->root();
}
}

void DependCalactor::AddSymbolNodeDep(ASTNode* node) {
  DCHECK(node->IsSymbolNode());
  SymbolNode* symbol = node->ToSymbolNode();
  TypePtr type = symbol->GetType();
  // add symbol before struct declaration
  // when dump dependency, reverse iterator the node
  if (IsGlobalVariable(symbol)) {
    Add(symbol->symbolname(), node->parent());
  }

  // symbol->GetDeclarationNode may be NULL, if symbol defiend in
  // function's parameters list
  if (type->IsStructure()) {
    ASTNode* type_definition = node->GetContext()->LookupType(type->name());
    DCHECK(type_definition) << "cannot find type: \"" << type->name() << "\"";
    if (type->storage_qualifier() == kUniform) {
      AddUniTypeDep(type, type_definition);
    } else {
      AddTypeDep(type, type_definition);
    }
  }
}

void DependCalactor::AddUniTypeDep(TypePtr type, ASTNode* node) {
  DCHECK(type.get() != NULL);
  if (!type->IsStructure()) return;
  if (type->IsAnomyousStruct()) return;
  auto iter = uni_dep_inserted_.find(StructFullName(node));
  if (iter == uni_dep_inserted_.end()) {
    uni_dep_inserted_.insert(StructFullName(node));
    uni_depend_.push_back(node);
  }
}

void DependCalactor::AddRefSymbolNode(ASTNode* node) {
  DCHECK(node->IsRefSymbolNode());
  RefSymbolNode* ref = node->ToRefSymbolNode();
  DCHECK(ref->GetDeclNode());
  SymbolNode* symbol = ref->GetDeclNode();
  AddSymbolNodeDep(symbol);
}

void DependCalactor::AddTypeDep(TypePtr type, ASTNode* node) {
  DCHECK(type.get() != NULL);
  if (!type->IsStructure()) return;
  if (type->IsAnomyousStruct()) return;
  // type name is fullname here.
  Add(type->name(), node);
}

bool DependCalactor::OnVisitBegin(ASTNode* node) {
  if (node->type() == ASTNode::kFuncCallNode) {
    AddFuncDep(node);
    FuncCallNode* func = node->ToFuncCallNode();
    for (auto iter = func->GetParams().begin();
       iter != func->GetParams().end(); ++iter) {
      AddTexture(*iter);
    }
  } else if (node->type() == ASTNode::kStructDeclNode) {
    AddStructDeclDep(node);
  } else if (node->IsSymbolNode()) {
    AddSymbolNodeDep(node);
  } else if (node->IsFieldNode()) {
    AddFieldNodeDep(node);
  } else if (node->IsRefSymbolNode()) {
    AddRefSymbolNode(node);
  }
  return true;
}

void DependCalactor::AddTexture(ASTNode* node)  {
  if (!IsUniformTexture(node)) {
    return;
  }

  std::string var = GenReferredTextureVarName(node);
  auto iter = tex_inserted_.find(var);
  if (iter == tex_inserted_.end()) {
    textures_.push_back(node);
    tex_inserted_.insert(var);
  }
}

void DependCalactor::Calc(ASTNode* node) {
  DCHECK_EQ(depend_.size(), 0u);
  int prev_size = 0;
  node->traverse(this);
  if (depend_.size() == 0u) return;
  do {
    ASTNode* depnode = depend_[prev_size];
    depnode->traverse(this);
    prev_size++;
  } while (prev_size < depend_.size());
}

namespace {
bool IsUniformSymbol(ASTNode* node) {
  if (node->IsDeclarationNode()) {
    TypePtr typeptr = node->ToDeclarationNode()->GetType();
    if (typeptr->storage_qualifier() == kUniform) {
      return true;
    }
  }
  return false;
}
}  // namespace
}  // namespace afx
}  // namespace azer
