#include "azer/afx/compiler/util.h"

#include <sstream>
#include "base/strings/stringprintf.h"

namespace azer {
namespace afx {
DeclarationNode* CreateDeclSymbolNode(const std::string& identifier,
                                      TypedNode* typed, const SourceLoc& loc, 
                                      ParseContext* context) {
  DCHECK(typed != NULL && typed->GetType().get() != NULL);
  DeclarationNode* node = context->Create(ASTNode::kDeclarationNode, loc)
      ->ToDeclarationNode();
  node->SetTypedNode(typed);
  CreateSymbolDecl(identifier, typed, node, loc, context);
  return node;
}

DeclarationNode* CreateDeclSymbolNode(const std::string& ident, TypedNode* typed,
                                      ASTNode* initializer, const SourceLoc& loc, 
                                      ParseContext* context) {
  DCHECK(typed != NULL && typed->GetType().get() != NULL);
  DeclarationNode* node = context->Create(ASTNode::kDeclarationNode, loc)
      ->ToDeclarationNode();
  node->SetTypedNode(typed);
  CreateSymbolDecl(ident, typed, initializer, node, loc, context);
  return node;
}

SymbolNode* CreateSymbolDecl(const std::string& identifier, TypedNode* typed,
                             ASTNode* initializer, DeclarationNode* node,
                             const SourceLoc& loc, ParseContext* context) {
  DCHECK(typed != NULL && typed->GetType().get() != NULL);
  SymbolNode* symbol = CreateSymbolDecl(identifier, typed, node, loc, context);
  DCHECK(NULL != symbol);
  DCHECK(initializer->IsInitializerNode());
  if (initializer->next_sibling() == NULL) {
    symbol->SetInitializer(initializer);
  } else {
    InitializerNode* initnode = context->Create(ASTNode::kInitializerNode, loc)
        ->ToInitializerNode();
    initnode->AddChildren(initializer);
    symbol->SetInitializer(initnode);
  }
  return symbol;
}

SymbolNode* CreateSymbolDecl(const std::string& identifier, TypedNode* typed,
                             DeclarationNode* decl, const SourceLoc& loc,
                             ParseContext* context) {
  DCHECK(typed != NULL && typed->GetType().get() != NULL);
  SymbolNode* symbol = context->Create(ASTNode::kSymbolNode, loc)->ToSymbolNode();
  DCHECK(NULL != symbol);
  symbol->SetSymbolName(identifier);
  symbol->SetTypedNode(typed);
  decl->AddSymbol(symbol);
  symbol->SetDeclarationNode(decl);
  return symbol;
}

SymbolNode* CreateForFuncDefParams(FieldNode* field, ParseContext* context) {
  SymbolNode* symbol = context->Create(ASTNode::kSymbolNode,
                                       field->loc())->ToSymbolNode();
  symbol->SetTypedNode(field->GetTypedNode());
  symbol->SetSymbolName(field->fieldname());
  return symbol;
}

TypedNode* CreateTypedNode(TypePtr type, const SourceLoc& loc, ParseContext* ctx) {
  TypedNode* typed = ctx->Create(ASTNode::kTypedNode, loc)->ToTypedNode();
  typed->SetType(type);
  return typed;
}

TypedNode* CreateTypedNode(BasicType type, const SourceLoc& loc, ParseContext* ctx) {
  TypedNode* typed = ctx->Create(ASTNode::kTypedNode, loc)->ToTypedNode();
  typed->SetBasicType(type);
  return typed;
}

TypedNode* CreateTypedNode(const std::string& name, const SourceLoc& loc,
                           ParseContext* context) {
  TypedNode* typed = context->Create(ASTNode::kTypedNode, loc)->ToTypedNode();
  typed->SetStructType(name);
  return typed;
}

TypedNode* CreateTypedNode(StructDeclNode* node, const SourceLoc& loc,
                           ParseContext* context) {
  TypedNode* typed = context->Create(ASTNode::kTypedNode, loc)->ToTypedNode();
  typed->SetAnonymouseStruct(node);
  return typed;
}

TypedNode* CreateArraySpecifierTypedNode(const SourceLoc& loc,
                                         ParseContext* context) {
  TypedNode* typed = context->Create(ASTNode::kTypedNode, loc)->ToTypedNode();
  typed->AddDim(NULL);
  return typed;
}

TypedNode* CreateArraySpecifierTypedNode(ASTNode* node, const SourceLoc& loc,
                                         ParseContext* context) {
  TypedNode* typed = context->Create(ASTNode::kTypedNode, loc)->ToTypedNode();
  typed->AddDim(node);
  return typed;
}

FieldNode* CreateFieldNode(const std::string& name, TypedNode* typed,
                           const SourceLoc& loc, ParseContext* context) {
  FieldNode* field = context->Create(ASTNode::kFieldNode, loc)->ToFieldNode();
  field->SetFieldName(name);
  field->SetTypedNode(typed);
  return field;
}

FuncProtoNode* CreateFuncProto(const std::string& name, TypedNode* rettype,
                               const SourceLoc& loc, ParseContext* context) {
  FuncProtoNode* proto = context->Create(ASTNode::kFuncProtoNode, loc)->
      ToFuncProtoNode();
  proto->SetFuncName(name);
  proto->SetRetTyped(rettype);
  return proto;
}

StatementNode* CreateStatement(ASTNode* children, ParseContext* context) {
  SourceLoc loc;
  loc.lineno = -1;
  StatementNode* node =context->Create(ASTNode::kStatementNode, loc)
      ->ToStatementNode();
  if (NULL != children) {
    node->AddChildren(children);
  }
  return node;
}

ScopedNode* CreateScoped(ASTNode* children, ParseContext* context) {
  SourceLoc loc;
  loc.lineno = -1;
  ScopedNode* node = (ScopedNode*)context->Create(ASTNode::kScopedNode, loc);
  if (NULL != children) {
    node->AddChildren(children);
  }
  return node;
}

RefSymbolNode* CreateRefSymbolNode(const std::string* identifier,
                                   const SourceLoc& loc, ParseContext* context) {
  RefSymbolNode* node = NULL;
  node = context->Create(ASTNode::kRefSymbolNode, loc)->ToRefSymbolNode();
  node->SetSymbolName(*identifier);
  return node;
}

FuncCallNode* CreateFuncCallNode(RefSymbolNode* ref, const SourceLoc& loc,
                                 ParseContext* context) {
  FuncCallNode* node = NULL;
  const std::string& identifier = ref->symbolname();
  node = context->Create(ASTNode::kFuncCallNode, loc)->ToFuncCallNode();
  node->SetFuncName(identifier);
  return node;
}

namespace {
void chewIdent(std::stringstream& out, int depth) {
  out.seekg(-depth * 2, std::ios_base::end);
}
}  // namespace


}  // namespace afx
}  // namespace azer
