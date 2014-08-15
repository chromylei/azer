#include "azer/afx/compiler/expression_validator.h"

#include "azer/afx/compiler/astnode.h"
#include "azer/afx/compiler/context.h"
#include "azer/afx/compiler/value.h"
#include "base/logging.h"
#include "base/strings/stringprintf.h"
#include "base/strings/string_util.h"

namespace azer {
namespace afx {

namespace {
bool IsIntegerScalar(const Type* type) {
  return type->type() == kInt32 || type->type() == kUint32;
}
bool IsReal(const TypePtr& type);
BasicType GetSwizzleType(const std::string& fieldname);
}  // namespace

ValuePtr GetNodeValue(ASTNode* node) {
  if (node->IsConstNode()) {
    return ValuePtr(node->ToConstNode()->value());
  } else if (node->IsBinaryOpNode()) {
    return ValuePtr(node->ToBinaryOpNode()->value());
  } else if (node->IsUnaryOpNode()) {
    return ValuePtr(node->ToUnaryOpNode()->value());
  } else if (node->IsRefSymbolNode()) {
    RefSymbolNode* ref = node->ToRefSymbolNode();
    return ValuePtr(new Value(ref->GetDeclNode()->GetType()));
  } else if (node->IsFuncCallTypeInitNode()) {
    FuncCallTypeInitNode* typeinit = node->ToFuncCallTypeInitNode();
    DCHECK(typeinit->GetType().get() != NULL);
    return ValuePtr(new Value(typeinit->GetType()));
  } else if (node->IsFuncCallNode()) {
    FuncCallNode* call = node->ToFuncCallNode();
    DCHECK(call->GetFuncDefNode() && call->GetFuncDefNode()->IsFuncDefNode());
    DCHECK(call->GetFuncDefNode()->GetProtoNode());
    FuncProtoNode* proto = call->GetFuncDefNode()->GetProtoNode();
    return ValuePtr(new Value(proto->rettype()->GetType()));
  } else if (node->IsFieldNode()) {
    return ValuePtr(new Value(node->ToFieldNode()->GetType()));
  } else {
    NOTREACHED();
    return ValuePtr(new Value);
  }
}

TypedNode* GetTypedNode(ASTNode* node) {
  if (node->IsConstNode()) {
    TypedNode* typed = node->GetContext()->Create(ASTNode::kTypedNode,
                                                  node->loc())->ToTypedNode();
    typed->SetType(node->ToConstNode()->GetResultType());
    return typed;
  } else if (node->IsFuncCallNode()) {
    FuncCallNode* func = node->ToFuncCallNode();
    FuncDefNode* funcdef = func->GetFuncDefNode();
    DCHECK(funcdef != NULL);
    FuncProtoNode* proto = funcdef->GetProtoNode();
    DCHECK(proto != NULL);
    return proto->rettype();
  } else if (node->IsRefSymbolNode()) {
    SymbolNode* symbol = node->ToRefSymbolNode()->GetDeclNode();
    DCHECK(symbol != NULL);
    TypedNode* typed = symbol->GetTypedNode();
    DCHECK(typed != NULL);
    // perhaps struct field or swizzle expression
    if (typed->GetStructDecl() != NULL || typed->GetType()->IsVector()) {
      return typed;
    } else {
      return NULL;
    }
  } else if (node->IsBinaryOpNode()) {
    BinaryOpNode* binary = node->ToBinaryOpNode();
    if (binary->GetOperator() == kOpMember
        || binary->GetOperator() == kOpIndex) {
      TypedNode* typed = node->GetContext()->Create(ASTNode::kTypedNode,
                                                    node->loc())->ToTypedNode();
      typed->SetType(binary->GetResultType());
      return typed;
    } else {
      TypedNode* typed = node->GetContext()->Create(ASTNode::kTypedNode,
                                                    node->loc())->ToTypedNode();
      typed->SetType(binary->GetResultType());
      return typed;
    }
  } else {
    NOTREACHED();
    return NULL;
  }
}

TypePtr GetNodeType(ASTNode* node) {
  if (node->type() == ASTNode::kConstNode) {
    TypePtr typeptr(new Type(node->ToConstNode()->GetResultType()->type()));
    typeptr->SetStorageQualifier(kConst);
    return typeptr;
  } else if (node->IsRefSymbolNode()) {
    RefSymbolNode* ref = node->ToRefSymbolNode();
    return ref->GetDeclNode()->GetType();
  } else if (node->IsFuncCallTypeInitNode()) {
    FuncCallTypeInitNode* typeinit = node->ToFuncCallTypeInitNode();
    DCHECK(typeinit->GetType().get() != NULL);
    return typeinit->GetType();
  } else if (node->IsFuncCallNode()) {
    FuncCallNode* call = node->ToFuncCallNode();
    DCHECK(call->GetFuncDefNode() && call->GetFuncDefNode()->IsFuncDefNode());
    DCHECK(call->GetFuncDefNode()->GetProtoNode());
    FuncProtoNode* proto = call->GetFuncDefNode()->GetProtoNode();
    return proto->rettype()->GetType();
  } else if (node->IsUnaryOpNode()) {
    DCHECK(node->ToUnaryOpNode()->GetResultType().get() != NULL);
    return node->ToUnaryOpNode()->GetResultType();
  } else if (node->IsBinaryOpNode()) {
    DCHECK(node->ToBinaryOpNode()->GetResultType().get() != NULL);
    return node->ToBinaryOpNode()->GetResultType();
  } else if (node->IsFieldNode()) {
    return node->ToFieldNode()->GetType();
  } else {
    NOTREACHED();
    return NULL;
  }
}

bool ExpressionValidator::Valid(ASTNode* node) {
  if (HasError()) return false;
  if (node->IsUnaryOpNode()) {
    UnaryOpNode* unary = node->ToUnaryOpNode();
    CalcUnaryOperType(unary);
    DCHECK(unary->value().get() != NULL);
    if (!unary->value()->valid()) {
      std::stringstream ss;
      ss << "Invalid oper " << operator_str(unary->GetOperator());
      ReportError(ss.str(), node);
      return false;
    }
  } else if (node->IsBinaryOpNode()) {
    BinaryOpNode* binary = node->ToBinaryOpNode();
    CalcBinaryOperType(binary);
    DCHECK(binary->value().get() != NULL);
    if (!binary->value()->valid()) {
      std::stringstream ss;
      ss << "Invalid oper " << operator_str(binary->GetOperator());
      ReportError(ss.str(), node);
      return false;
    }
  } else if (node->IsSymbolNode()) {
    // check if the array dim expression is constant_expression
    if (!CheckArrayDecl(node->ToSymbolNode())) {
      return false;
    }
    DCHECK(node->first_child()->IsTypedNode());
    TypedNode* typed = node->first_child()->ToTypedNode();
    if (!ValidArrayDimSpecified(typed)) {
      return false;
    }
  } else if (node->IsTypedNode()) {
    if (!CalcArrayDim(node)) {
      return false;
    }
  } else if (node->IsInitializerNode()) {
    if (!CheckInitializer(node)) {
      return false;
    }
  } else if (node->IsFieldNode()) {
    // do nothing
    ASTNode* parent = node->parent();
    DCHECK(parent != NULL);
    if (parent->IsStructDeclNode()) {
      if (!InitFieldNodeType(node)) {
        return false;
      }
    }

    if (parent->IsFuncProtoNode() && parent->parent()
        && parent->parent()->IsFuncDefNode()) {
      if (!InitFieldNodeType(node)) {
        return false;
      }
    }
  }

  return !HasError();
}

int ExpressionValidator::CalcInitializerDim(ASTNode* node, ASTNodeVec* vec) {
  DCHECK(node->IsInitializerNode());
  int cnt = 0;
  ASTNode* cur = node->first_child();
  while (cur) {
    if (cur->IsInitializerNode()) {
      vec->push_back(cur);
    }
    cur = cur->next_sibling();
    cnt++;
  }
  return cnt;
}

int ExpressionValidator::CalcInitializerLevelDim(const ASTNodeVec& nodes,
                                                 ASTNodeVec* vec) {
  vec->clear();
  int prev_dim = -1;
  for (auto iter = nodes.begin(); iter != nodes.end(); ++iter) {
    int dim = CalcInitializerDim(*iter, vec);
    if (prev_dim != -1 && prev_dim != dim) {
      return -1;
    } else {
      prev_dim = dim;
    }
  }

  return prev_dim;
}

bool ExpressionValidator::InitFieldNodeType(ASTNode* node) {
  DCHECK(node->IsFieldNode());
  FieldNode* field_node = node->ToFieldNode();
  TypedNode* typed = field_node->GetTypedNode();
  DCHECK(NULL != typed);
  if (typed->GetDim().size() > 0) {
    TypePtr ptr = typed->GetType();
    for (int i = 0; i < (int)typed->GetDim().size(); ++i) {
      ASTNode* dimnode = typed->GetDim()[i];
      if (!CheckConstantExpression(dimnode)) {
        std::stringstream ss;
        ss << "array dim not consistent between initializer and constant";
        ReportError(ss.str(), typed, kArrayDimNotConsistent);
        return false;
      }

      ValuePtr vptr = GetNodeValue(dimnode);
      if (IsIntegerScalar(vptr->type())) {
        typed->GetType()->PushDim(GetInteger(vptr));
      } else {
        std::stringstream ss;
        ss << "array dim not consistent between initializer and constant";
        ReportError(ss.str(), typed, kArrayDimNotConsistent);
        return false;
      }
    }
  }
  return true;
}

bool ExpressionValidator::CheckInitializer(ASTNode* node) {
  DCHECK(node->IsInitializerNode());
  if (!node->parent()->IsSymbolNode()) {
    return true;
  }

  DCHECK(node->prev_sibling() && node->prev_sibling()->IsTypedNode());
  TypedNode* typed = node->prev_sibling()->ToTypedNode();
  TypePtr ptr = typed->GetType();
  if (!ptr->IsArray() && !ptr->IsVector() && !ptr->IsMatrix()) {
    return true;
  }

  std::vector<int> dims;
  ASTNodeVec init;
  ASTNodeVec level;
  init.push_back(node);
  while (!init.empty()) {
    int dim = CalcInitializerLevelDim(init, &level);
    if (dim == -1) {
      std::stringstream ss;
      ss << "array dim not same in initializer level";
      ReportError(ss.str(), typed, kArrayDimNotConsistent);
      return false;
    }

    dims.push_back(dim);
    init.swap(level);
  }

  if (dims.size() == 1 && dims[0] == 1 && ptr->GetDimVec().size() == 0) {
    return true;
  }

  if (dims.size() != ptr->GetDimVec().size()) {
    std::stringstream ss;
    ss << "array dim not consistent between initializer and constant";
    ReportError(ss.str(), typed, kArrayDimNotConsistent);
    return false;
  }

  for (size_t i = 0; i < dims.size(); ++i) {
    if (ptr->GetDim(i) == -1) {
      ptr->SetDim(i, dims[i]);
    } else if (ptr->GetDim(i) != dims[i]) {
      std::stringstream ss;
      ss << "array dim not consistent between initializer and constant";
      ReportError(ss.str(), typed, kArrayDimNotConsistent);
      return false;
    }
  }

  return true;
}

bool ExpressionValidator::ValidArrayDimSpecified(TypedNode* typed) {
  DCHECK(typed->IsTypedNode());
  TypePtr ptr = typed->GetType();
  if (ptr->IsArray()) {
    for (int index = 0; index < ptr->GetDimVec().size(); ++index) {
      if (ptr->GetDim(index) == -1) {
        std::stringstream ss;
        ss << "array dim not specified";
        ReportError(ss.str(), typed, kArrayDimNotSpecified);
        return false;
      }
    }
  }
  return true;
}

void ExpressionValidator::CalcBinaryOperType(BinaryOpNode* node) {
  DCHECK(node->value().get() == NULL);
  using ::base::StringPrintf;
  TypePtr typeptr;
  Operator op = node->GetOperator();
  ASTNode* oper1 = node->GetOper1();
  ASTNode* oper2 = node->GetOper2();
  ValuePtr ptr1 = GetNodeValue(oper1);
  ValuePtr ptr2 = GetNodeValue(oper2);
  Value& value1 = *(ptr1.get());
  Value& value2 = *(ptr2.get());
  Value v;
  if (!value1.valid() || !value2.valid()) {
    node->SetValue(v);
    return;
  }

  switch (op) {
    case kOpAdd: v = value1 + value2; break;
    case kOpSub: v = value1 - value2; break;
    case kOpMul: v = value1 * value2; break;
    case kOpDiv: v = value1 / value2; break;
    case kOpMod: v = value1 % value2; break;
    case kOpGe: v = value1 >= value2; break;
    case kOpGt: v = value1 > value2; break;
    case kOpLe: v = value1 <= value2; break;
    case kOpLt: v = value1 < value2; break;
    case kOpNe: v = value1 != value2; break;
    case kOpEq: v = value1 == value2; break;
    case kOpLogicalAnd: v = value1 && value2; break;
    case kOpLogicalOr: v = value1 || value2; break;
    case kOpLogicalXor: v = value1 ^ value2; break;
    case kOpAssign: value1 = value2; v = value1; break;
    case kOpAddAssign: value1 += value2; v = value1; break;
    case kOpSubAssign: value1 -= value2; v = value1; break;
    case kOpMulAssign: value1 *= value2; v = value1; break;
    case kOpDivAssign: value1 /= value2; v = value1; break;
    case kOpModAssign: value1 %= value2; v = value1; break;
    case kOpAndAssign: value1 &= value2; v = value1; break;
    case kOpOrAssign: value1 |= value2; v = value1; break;
    case kOpXorAssign: value1 ^= value2; v = value1; break;
    case kOpLShiftAssign: value1 <<= value2; v = value1; break;
    case kOpRShiftAssign: value1 >>= value2; v = value1; break;
    case kOpBitwiseAnd: v = value1 & value2; break;
    case kOpBitwiseOr: v = value1 | value2; break;
    case kOpBitwiseXor: v = value1 ^ value2; break;
    case kOpBitwiseLshift:v = value1 << value2; break;
    case kOpBitwiseRshift:v = value1 >> value2; break;
    case kOpIndex:  {
      TypePtr ptr = GetNodeType(node->GetOper1());
      if (ptr->IsArray()) {
        DCHECK_GT(ptr->GetDimVec().size(), 0);
        TypePtr newtype(new Type(ptr->type()));
        for (int i = 0; i < ptr->GetDimVec().size() - 1; ++i) {
          newtype->PushDim(ptr->GetDim(i));
        }
        v = Value(newtype);
      } else if (ptr->IsVector()) { 
        v = Value(kFloat);
      } else if (ptr->IsIntVec()) { 
        v = Value(kInt32);
      } else if (ptr->IsMatrix()) { 
        v  = Value(kFloat);
      } else {
        v = Value();
      }
    }
      break;
    case kOpMember: {
      DCHECK(oper2->IsFieldNode());
      FieldNode* field = oper2->ToFieldNode();
      if (oper1->IsFuncCallNode()) {
      } else if (oper1->IsBinaryOpNode()) {
        TypePtr ptr = oper1->ToBinaryOpNode()->GetResultType();
        DCHECK(ptr.get() != NULL);
        if (ptr->IsVector()) {
          if (ValidSwizzleExpression(ptr, field)) {
            v = Value(GetSwizzleType(field->fieldname()));
          } else {
            v = Value();
          }
        } else {
          v = Value();
        }
      } else if (oper1->IsRefSymbolNode()) {
        SymbolNode* symbol = oper1->ToRefSymbolNode()->GetDeclNode();
        TypedNode* symtype = symbol->GetTypedNode();
        DCHECK(symtype != NULL);
        DCHECK(symtype->GetType().get() != NULL);
        if (symtype->GetStructDecl()) {
          StructDeclNode* decl = symtype->GetStructDecl();
		  DCHECK(decl != NULL);
		  FieldNode* decl_field = decl->GetField(field->fieldname());
          if (decl_field == NULL) {
            std::stringstream ss;
            ss << "struct \"" << symtype->GetStructDecl()->struct_name() << "\""
               << " has no field named \"" << field->fieldname() << "\"";
            ReportError(ss.str(), node); 
          } else {
            v = Value(decl_field->GetType());
          }
        } else if (symtype->GetType()->IsVector()) {
          if (ValidSwizzleExpression(symtype->GetType(), field)) {
            v = Value(GetSwizzleType(field->fieldname()));
          } else {
            v = Value();
          }
        } else {
          ReportError(StringPrintf("Unsupport operator \"%s\"",operator_str(op)),
                      node);
          v = Value();
        }
      }
    }
    default:
      break;
  }

  node->SetValue(v);
}

void ExpressionValidator::CalcUnaryOperType(UnaryOpNode* node) {
  Value v;
  ValuePtr ptr = GetNodeValue(node->GetOper());
  Value& value = *(ptr.get());
  switch (node->GetOperator()) {
    case kOpNegative: v = -value; break;
    case kOpInc: v = ++value; break;
    case kOpDec: v = --value; break;
    case kOpLogicalNot: v = !value; break;
    case kOpBitwiseNot: v = ~value; break;
    default: NOTREACHED(); break;
  }
  node->SetValue(v);
}

TypePtr ExpressionValidator::GetFieldType(RefSymbolNode* ref, FieldNode* field) {
  DCHECK(ref->GetDeclNode() != NULL);
  if(ref->GetDeclNode()->GetType()->IsStructure()) {
    DCHECK(ref->GetDeclNode()->IsSymbolNode());
    SymbolNode* symbol = ref->GetDeclNode();
    TypedNode* typed = symbol->GetTypedNode();
    DCHECK(typed != NULL && typed->GetStructDecl());
    DCHECK(typed->GetStructDecl() != NULL);
    DCHECK(typed->GetStructDecl()->HasField(field->fieldname()));
    return field->GetType();
  } else if (ref->GetDeclNode()->GetType()->IsVector()) {
    switch (field->fieldname().length()) {
      case 1:return TypePtr(new Type(kFloat));
      case 2:return TypePtr(new Type(kVector2));
      case 3:return TypePtr(new Type(kVector3));
      case 4:return TypePtr(new Type(kVector4));
      default: NOTREACHED(); return NULL;
    }
  } else {
    NOTREACHED();
    return NULL;
  }
}

bool ExpressionValidator::ValidMulOperType(ASTNode* node1, ASTNode* node2) {
  TypePtr type1 = GetNodeType(node1);
  TypePtr type2 = GetNodeType(node2);
  if (type1->IsStructure() || type2->IsStructure()) { return false;
  } else if (type1->IsArray() || type2->IsArray()) { return false;
  } else if (type1->IsTexture() || type2->IsTexture()) { return false;
  } else if (type1->type() == type2->type()) { return true;
  } else if (IsReal(type1) || IsReal(type2)) { return true;
  } else if ((type1->type() == kVector2 && type2->type() == kMatrix2)
           || (type1->type() == kMatrix2 && type2->type() == kVector2)) {
    return true;
  } else if ((type1->type() == kVector3 && type2->type() == kMatrix3)
           || (type1->type() == kMatrix3 && type2->type() == kVector3)) {
    return true;
  } else if ((type1->type() == kVector4 && type2->type() == kMatrix4)
             || (type1->type() == kMatrix4 && type2->type() == kVector4)) {
    return true;
  }  else {
    return false;
  }
}

bool ExpressionValidator::CalcArrayDim(ASTNode* node) {
  DCHECK(node->IsTypedNode());
  TypedNode* typed = node->ToTypedNode();
  if (typed->IsArrayTyped()) {
    TypePtr ptr = typed->GetType();
    ASTNode* cur = typed->first_child();
    while (cur) {
      ValuePtr value = GetNodeValue(cur);
      if (!value->valid() || !value->assigned()
          || !IsIntegerScalar(value->type())) {
        return false;
      }

      ptr->PushDim(GetInteger(value));
      cur = cur->next_sibling();
    }

    while (ptr->GetDimVec().size() < typed->GetDim().size()) {
      ptr->PushDim(-1);
    }
    return true;
  } else {
    return true;
  }
}

bool ExpressionValidator::CheckArrayDecl(SymbolNode* symbol) {
  TypedNode* typed = symbol->GetTypedNode();
  DCHECK(typed && typed->GetType().get() != NULL);
  if (typed->IsArrayTyped()) {
    int cnt = 0;
    
    for (auto iter = typed->GetDim().begin(); iter != typed->GetDim().end();
         ++iter, ++cnt) {
      if (*iter != NULL) {
        if (!CheckConstantExpression(*iter)) {
          std::stringstream ss;
          ss << "array \"" << symbol->symbolname() << "\" dim expression not const";
          context_->ReportError((*iter)->loc(), ss.str(), kArrayDimNotConstExp);
          return false;
        }
      } else {
        // do nothing, CalcArrayDim will do it
      }
    }
  }
  return true;
}

bool ExpressionValidator::CheckConstantExpression(ASTNode* node) {
  if (node->type() == ASTNode::kConstNode) {
    return true;
  } else if (node->type() == ASTNode::kRefSymbolNode) {
    RefSymbolNode* ref = node->ToRefSymbolNode();
    DCHECK(ref->GetDeclNode());
    return ref->GetDeclNode()->GetType()->storage_qualifier() & kConst;
  } else if (node->type() == ASTNode::kUnaryOpNode) {
    UnaryOpNode* unary = node->ToUnaryOpNode();
    return CheckConstantExpression(unary->GetOper());
  } else if (node->type() == ASTNode::kBinaryOpNode) {
    BinaryOpNode* binary = node->ToBinaryOpNode();
    return CheckConstantExpression(binary->GetOper1())
        && CheckConstantExpression(binary->GetOper2());
  } else if (node->IsFuncCallNode()) {
    return false;
  } else {
    NOTREACHED();
    return false;
  }
}

bool ExpressionValidator::ValidSwizzleExpression(const TypePtr& type,
                                                 FieldNode* field) {
  DCHECK(type->IsVector());
  if (field->fieldname().length() > 4) {
    std::stringstream ss;
    ss << "swizzle expression is too long: \"" << field->fieldname() << "\"";
    ReportError(ss.str(), field);
    return false;
  }

  if (type->type() == kVector2) {
    if (ContainsOnlyChars(field->fieldname(), "xy")) {
      return true;
    } else {
      std::stringstream ss;
      ss << "swizzle expression on vec2 only support 'x', 'y'";
      ReportError(ss.str(), field);
      return false;
    }
  } else if (type->type() == kVector3) {
    if (ContainsOnlyChars(field->fieldname(), "xyz")
        || ContainsOnlyChars(field->fieldname(), "rgb")) {
      return true;
    } else {
      std::stringstream ss;
      ss << "swizzle expression on vec3 only support ('x', 'y', 'z')"
         << " or ('r', 'g', 'b')";
      ReportError(ss.str(), field);
      return false;
    }
  } else if (type->type() == kVector4) {
    if (ContainsOnlyChars(field->fieldname(), "xyzw")
        || ContainsOnlyChars(field->fieldname(), "rgba")) {
      return true;
    } else {
      std::stringstream ss;
      ss << "swizzle expression on vec4 only support ('x', 'y', 'z', 'w')"
         << " or ('r', 'g', 'b', 'a')";
      ReportError(ss.str(), field);
      return false;
    }
  } else {
    NOTREACHED();
    return false;
  }
}

void ExpressionValidator::ReportError(const std::string& stream, ASTNode* node,
                                      CompileErrno err) {
  DCHECK(context_ != NULL);
  failed_ = true;
  context_->ReportError(node->loc(), stream, err);
}

bool ConstExpressValueCalculator::OnVisitBegin(ASTNode* node) {
  return true;
}

void ConstExpressValueCalculator::OnVisitEnd(ASTNode* node) {
}

namespace {
bool IsReal(const TypePtr& t) {
  return t->type() >= kShort && t->type() <= kDouble && !t->IsArray();
}

BasicType GetSwizzleType(const std::string& fieldname) {
  switch (fieldname.length()) {
    case 1: return kFloat;
    case 2: return kVector2;
    case 3: return kVector3;
    case 4: return kVector4;
    default:
      NOTREACHED(); return kFloat;
  }
}
}
}  // namespace afx
}  // namespace azer
