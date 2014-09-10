#include "azer/resources/material_manager.h"

#include "azer/resources/resource_manager.h"
#include "azer/resources/json_util.h"
#include "azer/base/image.h"
#include "azer/render/util/image.h"
#include "base/json/json_reader.h"

namespace azer {

namespace {
typedef std::vector<std::pair<StringType, MaterialPtr> > MaterialVec;
bool ParseMaterial(const char* buffer, int len, MaterialManager* mgr,
                   RenderSystem* rs, MaterialVec* mtrlvec);
inline void GetTexturePath(const base::DictionaryValue* dict,
                           const std::string& path,
                           std::vector<StringType>* tex_path) {
  CHECK_GT(path.size(), 0u);
  const ::base::ListValue* list = NULL;
  if (dict->GetList(path, &list)) {
    StringType str;
    for (int i = 0; i < list->GetSize(); ++i) {
      CHECK(list->GetString(i, &str));
      tex_path->push_back(str);
    }
  }
}
}  // namespace

TexturePtr MaterialManager::LoadTexture(const ResFilePath& path, RenderSystem* rs) {
  // TODO(yanglei)
  // use content of texture to create Texture, but not to use CreateTextureFromFile
  // because of we cannto to call the function when filesystem is packed
  CHECK(rs != NULL);
  ::base::FilePath abso_path;
  if (res_mgr_->GetAbsoluteFilePath(path, &abso_path)) {
    azer::Texture::Options texopt;
    texopt.target = azer::Texture::kShaderResource;
    azer::ImagePtr imgptr(azer::LoadImageFromFile(abso_path));
    return TexturePtr(rs->CreateTexture(texopt, imgptr.get()));
  }
  LOG(ERROR) << "No such filepath " << path.value() << " for texture";
  return NULL;
}

MaterialPtr MaterialManager::GetMaterial(const StringType& name, RenderSystem* rs) {
  DCHECK(NULL != res_mgr_);
  MaterialPtr ptr(FindMaterial(name));
  if (ptr.get()) return ptr;

  StringType path, package;
  if (!SplitPackage(name, &path, &package)) return NULL;
  FileContentPtr content_ptr(res_mgr_->LoadFile(ResFilePath(path)));
  if (content_ptr.get() == NULL) return NULL;
  if (!LoadMaterial(ResFilePath(path), rs)) {
    return NULL;
  }

  return FindMaterial(name);
}

bool MaterialManager::LoadMaterial(const ResFilePath& path, RenderSystem* rs) {
  DCHECK(NULL != res_mgr_);
  FileContentPtr ptr = res_mgr_->LoadFile(path);
  MaterialVec vec;
  if (!ParseMaterial((const char*)(ptr->data), ptr->length, this, rs, &vec)) {
    LOG(ERROR) << "Failed to LoadMaterial from path: " << path.value();
    return false;
  }

  for (auto iter = vec.begin(); iter != vec.end(); ++iter) {
    StringType fullpath = path.value();
    fullpath.push_back(FILE_PATH_LITERAL(':'));
    fullpath.append(iter->first);
    material_dict_[fullpath] = iter->second;
  }

  return true;
}

namespace {
bool ParseMaterial(const char* buffer, int len, MaterialManager* mgr,
                   RenderSystem* rs, MaterialVec* mtrlvec) {
  int json_error_code = 0;
  std::string json_error_desc;
  ::base::StringPiece piece(buffer, len);
  std::unique_ptr<::base::Value> root(
      ::base::JSONReader::ReadAndReturnError(piece,
                                             base::JSON_ALLOW_TRAILING_COMMAS,
                                             &json_error_code, &json_error_desc));
  ::base::ListValue* root_list = NULL;
  if (!root.get()) {
    LOG(ERROR) << "Failed to Parse Material JSON: " << json_error_desc;
    return false;
  }
  if (!root->GetAsList(&root_list)) { return false; }

  StringType name;
  std::vector<base::FilePath::StringType> texvec;
  for (int i = 0; i < root_list->GetSize(); ++i) {
    MaterialPtr mtrl(new Material);
    const Value* value = NULL;
    const ::base::DictionaryValue* dict = NULL;
    if (!root_list->Get(i, &value)) return false;
    if (!value->GetAsDictionary(&dict)) return false;

    dict->GetString("name", &name);
    detail::GetFloat(dict, "transparency", &mtrl->attributes().transparency, 1.0f);
    detail::GetFloat(dict, "specular_power", &mtrl->attributes().specular_power);
    detail::GetFloat(dict, "optical_density", &mtrl->attributes().optical_density);
    detail::GetInt(dict, "illumination", &mtrl->attributes().illumination, 1);
    detail::GetVector4(dict, "ambient", &mtrl->attributes().ambient);
    detail::GetVector4(dict, "diffuse", &mtrl->attributes().diffuse);

    texvec.clear();
    GetTexturePath(dict, "diffuse_tex", &texvec);
    for (auto iter = texvec.begin(); iter != texvec.end(); ++iter) {
      mtrl->AddDiffuseTex(mgr->GetTexture(*iter, rs));
    }

    texvec.clear();
    GetTexturePath(dict, "normal_tex", &texvec);
    for (auto iter = texvec.begin(); iter != texvec.end(); ++iter) {
      mtrl->AddNormalTex(mgr->GetTexture(*iter, rs));
    }

    texvec.clear();
    GetTexturePath(dict, "specular_tex", &texvec);
    for (auto iter = texvec.begin(); iter != texvec.end(); ++iter) {
      mtrl->AddSpecularTex(mgr->GetTexture(*iter, rs));
    }

    texvec.clear();
    GetTexturePath(dict, "alpha_tex", &texvec);
    for (auto iter = texvec.begin(); iter != texvec.end(); ++iter) {
      mtrl->AddAlphaTex(mgr->GetTexture(*iter, rs));
    }

    mtrlvec->push_back(make_pair(name, mtrl));
  }

  return true;
}
}  // namespace
}  // namespace azer
