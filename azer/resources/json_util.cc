#include "azer/resources/json_util.h"
#include "azer/base/string.h"
#include "base/json/json_reader.h"
#include "azer/resources/resource_manager.h"

namespace azer {
namespace detail {

bool ParseVertexDescItem(const ::base::Value* value, VertexDesc::Desc* desc) {
  if (value == NULL) {
    return false;
  }

  const ::base::DictionaryValue* dict = NULL;
  int semantic_index;
  std::string semantic_name, data_type;
  value->GetAsDictionary(&dict);
  if (dict == NULL) {
    return false;
  }
  
  if (!dict->GetString("semantic_name", &semantic_name)) {
    LOG(ERROR) << "no semantic_name in VertexDesc";
    return false;
  }

  if (!dict->GetInteger("semantic_index", &semantic_index)) {
    LOG(ERROR) << "no semantic_index in VertexDesc";
    return false;
  }

  if (!dict->GetString("data_type", &data_type)) {
    LOG(ERROR) << "no data_type in VertexDesc";
    return false;
  }

  if (semantic_name.length() > sizeof(desc->name) - 1) {
    LOG(ERROR) << "Semantic's name is too long";
    return false;
  }

  strcpy(desc->name, semantic_name.c_str());
  desc->semantic_index = 0;
  if (data_type == "float") {
    desc->type = azer::kFloat;
  } else if (data_type == "vec2") {
    desc->type = azer::kVec2;
  } else if (data_type == "vec3") {
    desc->type = azer::kVec3;
  } else if (data_type == "vec4") {
    desc->type = azer::kVec4;
  } else if (data_type == "int") {
    desc->type = azer::kInt;
  } else if (data_type == "int2") {
    desc->type = azer::kIntVec2;
  } else if (data_type == "int3") {
    desc->type = azer::kIntVec3;
  } else if (data_type == "int4") {
    desc->type = azer::kIntVec4;
  } else {
    NOTREACHED();
  }
  return true;
}

bool ParseVertexDesc(const char* buffer, int len, VertexDescVec* vdesc_vec) {
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
  if (!root->GetAsList(&root_list)) {
    LOG(ERROR) << "No VertexDesc in list in file";
    return false;
  }

  StringType name;
  for (int i = 0; i < root_list->GetSize(); ++i) {
    const ::base::DictionaryValue* dict = NULL;
    const ::base::Value* value = NULL;
    const ::base::ListValue* desc_list = NULL;
    if (!root_list->Get(i, &value)) {return false;}
    if (!value->GetAsDictionary(&dict)) { return false;}
    if (!dict->GetString("name", &name)) {
      LOG(ERROR) << "No name in VertexDesc item";
      return false;
    }

    if (!dict->GetList("desc", &desc_list)) {
      LOG(ERROR) << "No desc in VertexDesc item";
      return false;
    }

    const int desc_num = desc_list->GetSize();
    std::unique_ptr<VertexDesc::Desc[]> desc(new VertexDesc::Desc[desc_num]);
    for (int i = 0; i < desc_num; ++i) {
      const ::base::Value* desc_value = NULL;
      desc_list->Get(i, &desc_value);
      if (!ParseVertexDescItem(desc_value, desc.get() + i)) {
        return false;
      }
    }

    VertexDescPtr desc_ptr(new VertexDesc(desc.get(), desc_num));
    vdesc_vec->push_back(std::make_pair(name, desc_ptr));
  }

  
  return true;
}

}  // namespace detail
}  // namespace azer
