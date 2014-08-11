#pragma once

namespace azer {
namespace afx {
enum CompileErrno {
    kNoError = 0,
    kUnknownErr = -1,
    kSyntaxError = 1,
    kCannotFindSymbol,
    kInvalidAttr,
    kFuncDefined,
    kSymbolDeclared,
    kStructDeclared,
    kInvalidStageMainParam,
    kArrayDimNotConstExp,
    kArrayDimNotSpecified,
    kArrayDimNotConsistent,
    kMultiUniformDeclInStatement,
    kTextureMustbeUniformed,
    kOperUnacceptableOper,
  };


}  // namespace afx
}  // namespace azer
