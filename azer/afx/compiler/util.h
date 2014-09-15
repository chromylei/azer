#pragma once

#include "azer/afx/compiler/type.h"
#include "azer/afx/compiler/astnode.h"
#include "azer/afx/compiler/parser.h"

namespace azer {
namespace afx {
DeclarationNode* CreateDeclSymbolNode(const std::string& identifier,
                                      TypedNode* typed, const SourceLoc& loc, 
                                      ParseContext* parser);


DeclarationNode* CreateDeclSymbolNode(const std::string& identifier,
                                      TypedNode* typed, ASTNode* initializer,
                                      const SourceLoc& loc, ParseContext* parser);

// 创建一个 SymbolNode
SymbolNode* CreateSymbolDecl(const std::string& identifier, TypedNode* typed,
                             DeclarationNode* node, const SourceLoc& loc,
                             ParseContext* context);

// 创建带有初始化的 SymbolNode
SymbolNode* CreateSymbolDecl(const std::string& identifier, TypedNode* typed,
                             ASTNode* initializer, DeclarationNode* node,
                             const SourceLoc& loc, ParseContext* context);
// 根据函数体的参数创建 Symbol
ActParamNode* CreateForFuncDefParams(ParamNode* param, ParseContext* context);
TypedNode* CreateTypedNode(TypePtr type, const SourceLoc& loc, ParseContext* ctx);
TypedNode* CreateTypedNode(BasicType type, const SourceLoc& loc, ParseContext* ctx);
TypedNode* CreateTypedNode(const std::string& name, const SourceLoc& loc,
                           ParseContext* context);
TypedNode* CreateTypedNode(StructDeclNode* node, const SourceLoc& loc,
                           ParseContext* context);
TypedNode* CreateArraySpecifierTypedNode(const SourceLoc& loc, ParseContext* ctx);
TypedNode* CreateArraySpecifierTypedNode(ASTNode* node, const SourceLoc& loc,
                                         ParseContext* context);

FieldNode* CreateFieldNode(const std::string& name, TypedNode* typed,
                           const SourceLoc& loc, ParseContext* context);
ParamNode* CreateParamNode(const std::string& name, TypedNode* typed,
                           const SourceLoc& loc, ParseContext* context);
FuncProtoNode* CreateFuncProto(const std::string& name, TypedNode* rettype,
                               const SourceLoc& loc, ParseContext* context);

StatementNode* CreateStatement(ASTNode* children, ParseContext* context);
ScopedNode* CreateScoped(ASTNode* children, ParseContext* context);

RefSymbolNode* CreateRefSymbolNode(const std::string* identifier,
                                   const SourceLoc& loc, ParseContext* context);

/**
 * 创建 FuncCallNode
 */
FuncCallNode* CreateFuncCallNode(RefSymbolNode* ref, const SourceLoc& loc,
                                 ParseContext* context);
}  // namespace afx
}  // namespace azer
