#include "azer/resources/technique_manager.h"

#include "azer/render/render_system.h"
#include "azer/render/util/shader_array.h"
#include "azer/resources/resource_manager.h"
#include "azer/resources/json_util.h"
#include "base/json/json_reader.h"
#include "base/values.h"

namespace azer {
TechniquePtr TechniqueManager::GetTechnique(RenderSystem* rs,
                                            const StringType& path,
                                            ShaderType type) {
  TechniquePtr ptr = FindTechnique(path, rs->short_name(), type);
  if (ptr.get()) return ptr;

  if (!LoadTechnique(rs, path, type)) {
    LOG(ERROR) << "failed to load technique: " << path;
    return NULL;
  }

  return FindTechnique(path, rs->short_name(), type);
}

bool TechniqueManager::LoadTechnique(RenderSystem* rs, const StringType& path,
                                     ShaderType type) {
  DCHECK(res_mgr_ != NULL);
  FileContentPtr contentptr = res_mgr_->LoadFile(path);
  if (!contentptr.get()) return false;

  TechniqueResult result;

  if (!ParseTechnique(contentptr->data, contentptr->length, &result)) {
    return false;
  }

  ShaderArray shader_array;
  VertexDescPtr descptr = res_mgr_->GetVertexDesc(result.vertex_desc);
  if (!descptr.get()) return false;
  shader_array.SetVertexDesc(descptr);

  int index = 0;
  for (auto iter = result.program.begin(); iter != result.program.end(); ++iter,
           ++index) {
    RenderPipelineStage stage = (RenderPipelineStage)index;
    if (!iter->empty()) {
      StringType path = std::move(DictionaryKey(*iter, rs->short_name(), type));
      FileContentPtr ptr = res_mgr_->LoadFile(path);
      if (!ptr.get()) {
        LOG(ERROR) << "Failed to load program: " << path;
        return false;
      }
      shader_array.SetForStage(stage, std::string(ptr->data, ptr->length));
    }
  }
    
  TechniquePtr techptr = shader_array.CreateTechnique(rs);
  if (!techptr.get()) return false;

  StringType key = std::move(DictionaryKey(path, rs->short_name(), type));
  technique_dict_[key] = techptr;

  return true;
}

/**
 * {
 *   "vertex_stage": {
 *     "program": "technique/shader/base.ps",
 *     "vertex_desc": "vdesc/base.vdesc:base",
 *   }
 *   "geometry_stage": "",
 *   "pixel_stage": "",
 * }
 *
 */

bool TechniqueManager::ParseTechnique(const char* buffer, int len,
                                      TechniqueResult* result) {
  DCHECK(res_mgr_ != NULL);
  int json_error_code = 0;
  std::string json_error_desc;
  ::base::StringPiece piece(buffer, len);
  std::unique_ptr<::base::Value> root(
      ::base::JSONReader::ReadAndReturnError(piece,
                                             base::JSON_ALLOW_TRAILING_COMMAS,
                                             &json_error_code, &json_error_desc));
  if (!root.get()) {
    LOG(ERROR) << "Failed to Parse Technqiue JSON: " << json_error_desc;
    return false;
  }

  StringType str;
  const ::base::DictionaryValue* dict = NULL;
  const ::base::DictionaryValue* vertexdict = NULL;
  if (!root->GetAsDictionary(&dict)) return false;
    if (dict->GetString("pixel_stage", &str)) {
    result->program[kPixelStage] =  str;  
  } else {
    LOG(ERROR) << "Failed to get PixelStage";
    return false;
  }

  if (dict->GetDictionary("vertex_stage", &vertexdict)) {
    if (vertexdict->GetString("program", &str)) {
      result->program[kVertexStage] = str;
    } else {
      LOG(ERROR) << "No VertexStage Program";
      return false;
    }

    if (!vertexdict->GetString("vertex_desc", &result->vertex_desc)) {
      LOG(ERROR) << "No VertexDesc for VertexStage";
      return false;
    }
  } else {
    LOG(ERROR) << "Failed to get VertexStage";
    return false;
  }

  str.clear();
  dict->GetString("geometry_stage", &str);
  result->program[kGeometryStage] =  str;

  return true;
}

}  // namespace azer
