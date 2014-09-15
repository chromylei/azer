#pragma once

#include <string>
#include <vector>

#include "azer/afx/compiler/type.h"

namespace azer {
namespace afx {

class ASTNode;
class TypedNode;

/**
 * 从 sampler 函数的角度计算 texture 的名称
 */
std::string GenReferredTextureVarName(ASTNode* node);

bool IsTextureArray(ASTNode* node);

/**
 * 判断 texture 是否属于 Material
 */
bool IsMaterialTex(ASTNode* node);

/**
 * 获得 texture 的声明 node
 * 如果是结构体的成员，则返回结构体的 symbol 节点
 * 如果仅仅是一个变量，则返回变量的声明节点
 */
ASTNode* GetTexDeclNode(ASTNode* node);

/**
 * 获取最终的 Texture 节点的类型节点
 * 例如 mtrl.tex.diffuse[0]， 最终节点是 diffuse 这个 FieldNode
 */
TypedNode* GetTexNode(ASTNode* node);

/**
 * 获取的 Texture 节点的 Type
 * 注： Texture 有可能是 Texture1D, Texture2D 等，还有可能是数组
 */
TypePtr GetTextureType(ASTNode* node);

/**
 * 为 node 产生一个完整的 texture 名称(包括数组后缀) 
 */
std::string HLSLRefferedTextureFullName(ASTNode* node);

/**
 * 此函数与 HLSLRefferedTextureFullName 不同的是
 * 数组部分的是声明时的维度
 */
std::string HLSLDeclaredTextureFullName(ASTNode* node);

/**
 * 获取 sampler 变量的名称
 * D3D11 在执行 sampler 时，需要指定一个 sampler, afx 为每一个 uniform 变量生成一个
 * 默认 sampler, 以下两个函数将根据 texture 节点的名称生成对应 sampler 对象的名称
 * 一种 DeclFullName，在声明时使用，除了名称之外，它还带有 array specifier
 * 另外一个函数访问时使用
 */
std::string HLSLUniformTextureSamplerDeclFullName(ASTNode* node);
std::string HLSLUniformTextureSamplerFullName(ASTNode* node);


/**
 * 设置 uniform texture 的函数名
 */
std::string HLSLTextureUniformFuncName(ASTNode* node);

/**
 * texture 后面的 array specified
 */
std::string HLSLTextureArraySpecifier(ASTNode* node);

std::string HLSLTextureTypeName(ASTNode* node);

/**
 * 获取访问路径
 * a.b.tex 那么他将被分成一个 std::vector<std::string> = {a, b, tex}
 */
void GetTexRefferPath(ASTNode* node, std::vector<std::string>* path);

bool IsUniformTexture(ASTNode* node);

/**
 * 判断函数的参数是否是 Texture(最终结果是否是 texture 类型)
 */
bool IsNodeTypeTexture(ASTNode* node);

bool HasTextureField(ASTNode* field);
bool HasOnlyTextureField(ASTNode* node);
}  // namespace afx
}  // namespace azer
