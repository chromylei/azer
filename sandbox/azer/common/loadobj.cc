#include "sandbox/azer/common/loadobj.h"

#include <iostream>

#include "azer/render/align.h"

#include "base/files/file_path.h"
#include "base/file_util.h"
#include "base/strings/string_split.h"
#include "base/strings/string_util.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/utf_string_conversions.h"

using base::UTF8ToWide;

const azer::VertexDesc::Desc ObjMeshLoader::kVertexDescs[] = {
  {"POSITION", azer::VertexDesc::kPosition, azer::kVec4},
  {"TEXCOORD", azer::VertexDesc::kTexCoord, azer::kVec2},
  {"NORMAL",   azer::VertexDesc::kNormal,   azer::kVec3},
  {"TANGENT",  azer::VertexDesc::kTangent,  azer::kVec3},
};

const int ObjMeshLoader::kDescNum = (int32)arraysize(ObjMeshLoader::kVertexDescs);

ObjMeshLoader::ObjMeshLoader() {
  curr_ = NULL;
}

void ObjMeshLoader::dump() {
  std::cout << "\tv: " << std::endl;
  for (auto iter = v_.begin(); iter != v_.end(); ++iter) {
    std::cout << "\t" << *iter << std::endl;
  }

  std::cout << "vn: " << std::endl;
  for (auto iter = vn_.begin(); iter != vn_.end(); ++iter) {
    std::cout <<"\t" <<  *iter << std::endl;
  }

  std::cout << "vt: " << std::endl;
  for (auto iter = vt_.begin(); iter != vt_.end(); ++iter) {
    std::cout << "\t" << *iter << std::endl;
  }

  
  for (auto iter = entries_.begin(); iter != entries_.end(); ++iter) {
    DumpGroup(**iter);
  }
}

void ObjMeshLoader::DumpGroup(const Group& g) {
  std::cout << "group: " << g.name << std::endl;

  std::cout << "faces: " << std::endl;
  for (auto iter = g.f.begin(); iter != g.f.end(); ++iter) {
    std::cout << "\t" << *iter << std::endl;
  }

  std::cout << std::endl << std::endl << std::endl;
}

void ObjMeshLoader::TransGroup(Group* sub, azer::Mesh::SubMesh* asub) {
  azer::RenderSystem* rs = azer::RenderSystem::Current();
  azer::VertexDescPtr vbdesc(new azer::VertexDesc(kVertexDescs, ARRAYSIZE(kVertexDescs)));
  azer::VertexDataPtr vbd(new azer::VertexData(vbdesc, sub->f.size()));
  
  Vertex* ptr_begin = (Vertex*)vbd->pointer();
  Vertex* ptr = ptr_begin;
  std::unique_ptr<uint32[]> idata(new uint32[sub->f.size()]);
  uint32* iptr_begin = idata.get();
  uint32* iptr = iptr_begin;
  int cnt = 0;
  for (auto iter = sub->f.begin(); iter != sub->f.end(); ++iter, ++cnt, ++ptr, ++iptr) {
    int vidx = iter->v - 1;
    int nidx = iter->vn - 1;
    ptr->position = azer::vec4(v_[vidx], 1.0);
    if (iter->vt >= 0) {
      int tidx = iter->vt - 1;
      ptr->tex = azer::vec2(vt_[tidx].x, vt_[tidx].y);
    }
    ptr->normal = vn_[nidx];
    *iptr = cnt;

    if (cnt > 0 && cnt % 3 == 0) {
      azer::Vector3 tang = azer::CalcTangent((ptr-3)->position, (ptr-3)->tex,
                                             (ptr-2)->position, (ptr-2)->tex,
                                             (ptr-1)->position, (ptr-1)->tex);
      (ptr-3)->tangent = tang;
      (ptr-2)->tangent = tang;
      (ptr-1)->tangent = tang;
    }

    /*
     * 此处可以看到，没有三角形会被复用，因此不用计算平均，直接对每个三角形计算即可
     */
  }

  asub->vb_ptr.reset(rs->CreateVertexBuffer(azer::VertexBuffer::Options(), vbd));

  if (sub->f.size() > 0) {
    azer::IndicesDataPtr ibd(new azer::IndicesData(sub->f.size(),
                                                   azer::IndicesData::kUint32,
                                                   azer::IndicesData::kMainMemory));
    memcpy(ibd->pointer(), (uint8*)idata.get(), sizeof(uint32) * sub->f.size());
    asub->ib_ptr.reset(rs->CreateIndicesBuffer(azer::IndicesBuffer::Options(), ibd));
  }

  azer::Mesh::Group group;
  group.first = 0;
  group.num = sub->f.size();
  group.mtrl_ptr = sub->mtrl;
  asub->groups.push_back(group);
  
}

bool ObjMeshLoader::Load(const base::FilePath& path, azer::Mesh *mesh) {
  path_ = path;
  std::string content;
  if (!base::ReadFileToString(path, &content)) {
    return false;
  }
  std::vector<std::string> vec;
  base::SplitString(content, '\n', &vec);
  for (auto iter = vec.begin(); iter != vec.end(); ++iter) {
    std::string trim;
    TrimWhitespace(*iter, TRIM_ALL, &trim);
    HandleLine(trim);
  }

  for (auto iter = entries_.begin(); iter != entries_.end(); ++iter) {
    azer::SubMeshPtr ptr(new azer::Mesh::SubMesh);
    TransGroup(*iter, ptr.get());
    mesh->Add(ptr);
  }

  return true;
}

void ObjMeshLoader::HandleMaterialLine(const std::string& linestr) {
  azer::RenderSystem* rs = azer::RenderSystem::Current();
  base::StringPiece line(linestr);
  if (line.starts_with("newmtl ")) {
    curr_mtrl_.reset(new azer::Material);
    std::string name = line.substr(strlen("newmtl ")).as_string();
    materials_.insert(std::make_pair(name, curr_mtrl_));
  } else if (line.starts_with("map_Ka ")) {
    std::wstring path = UTF8ToWide(line.substr(strlen("map_Ka ")).as_string());
    std::wstring str = path_.DirName().value() + TEXT("/") + path;
    azer::TexturePtr tex(rs->CreateTextureFromFile(
        azer::Texture::k2D, base::FilePath(str)));
    curr_mtrl_->textures()->push_back(tex);
  } else if (line.starts_with("map_bump ")) {
    std::wstring path = UTF8ToWide(line.substr(strlen("map_bump ")).as_string());
    std::wstring str = path_.DirName().value() + TEXT("/") + path;
    azer::TexturePtr tex(rs->CreateTextureFromFile(
        azer::Texture::k2D, base::FilePath(str)));
    curr_mtrl_->textures()->push_back(tex);
  } else if (line.starts_with("Ka ")) {  // ambient
    double x, y, z;
    std::vector<std::string> vec;
    std::string val;
    TrimWhitespace(line.substr(strlen("vt ")).as_string(), TRIM_ALL, &val);
    base::SplitString(val, ' ', &vec);
    CHECK_EQ(vec.size(), 3u);
    CHECK(base::StringToDouble(vec[0], &x));
    CHECK(base::StringToDouble(vec[1], &y));
    CHECK(base::StringToDouble(vec[2], &z));
    curr_mtrl_->attributes()->ambient = azer::vec3(x, y, z);
  } else if (line.starts_with("Kd ")) {  // diffuse
    double x, y, z;
    std::vector<std::string> vec;
    std::string val;
    TrimWhitespace(line.substr(strlen("vt ")).as_string(), TRIM_ALL, &val);
    base::SplitString(val, ' ', &vec);
    CHECK_EQ(vec.size(), 3u);
    CHECK(base::StringToDouble(vec[0], &x));
    CHECK(base::StringToDouble(vec[1], &y));
    CHECK(base::StringToDouble(vec[2], &z));
    curr_mtrl_->attributes()->diffuse = azer::vec3(x, y, z);
  }
}

void ObjMeshLoader::ParseMaterial(const std::string& path) {
  std::string content;
  std::wstring str = path_.DirName().value() + TEXT("/") + UTF8ToWide(path);
  if (!base::ReadFileToString(base::FilePath(str), &content)) {
    LOG(ERROR) << "cannot load material file: " << str;
    return;
  }

  std::vector<std::string> vec;
  base::SplitString(content, '\n', &vec);
  for (auto iter = vec.begin(); iter != vec.end(); ++iter) {
    ObjMeshLoader::HandleMaterialLine(*iter);
  }
}

void ObjMeshLoader::HandleLine(const std::string& linestr) {
  base::StringPiece line(linestr);
  std::vector<std::string> vec;
  double x, y, z;
  if (linestr.c_str()[0] == '#') {return;
  } else if (line.starts_with("v ")) {
    vec.clear();
    std::string val;
    TrimWhitespace(line.substr(strlen("v ")).as_string(), TRIM_ALL, &val);
    base::SplitString(val, ' ', &vec);
    CHECK(vec.size() ==  3u || vec.size() == 2u);
    CHECK(base::StringToDouble(vec[0], &x)) << vec[0];
    CHECK(base::StringToDouble(vec[1], &y)) << vec[1];
    if (vec.size() == 3) CHECK(base::StringToDouble(vec[2], &z));
    else z = 0.0;
    v_.push_back(azer::vec3((float)x, (float)y, (float)z));
  } else if (line.starts_with("vn ")) {
    vec.clear();
    std::string val;
    TrimWhitespace(line.substr(strlen("vn ")).as_string(), TRIM_ALL, &val);
    base::SplitString(val, ' ', &vec);
    CHECK_EQ(vec.size(), 3u);
    CHECK(base::StringToDouble(vec[0], &x));
    CHECK(base::StringToDouble(vec[1], &y));
    CHECK(base::StringToDouble(vec[2], &z));
    vn_.push_back(azer::vec3((float)x, (float)y, (float)z));
  } else if (line.starts_with("vt ")) {
    vec.clear();
    std::string val;
    TrimWhitespace(line.substr(strlen("vt ")).as_string(), TRIM_ALL, &val);
    base::SplitString(val, ' ', &vec);
    CHECK_EQ(vec.size(), 3u);
    CHECK(base::StringToDouble(vec[0], &x));
    CHECK(base::StringToDouble(vec[1], &y));
    CHECK(base::StringToDouble(vec[2], &z));
    vt_.push_back(azer::vec3((float)x, (float)y, (float)z));
  } else if (line.starts_with("g ")) {
    curr_ = new Group;
    entries_.push_back(curr_);
    curr_->name = line.substr(2).as_string();
  } else if (line.starts_with("f ")) {
    vec.clear();
    std::string val;
    TrimWhitespace(line.substr(strlen("f ")).as_string(), TRIM_ALL, &val);
    ParseFace(val);
  } else if (line.starts_with("o ")) {
    CHECK(false);
  } else if (line.starts_with("usemtl ")) {
    std::string name = line.substr(strlen("usemtl ")).as_string();
    curr_->mtrl = materials_[name];
  } else if (line.starts_with("mtllib ")) {
    ParseMaterial(line.substr(strlen("mtllib ")).as_string());
  }
}

void ObjMeshLoader::ParseFace(const std::string& val) {
  std::vector<std::string> vec;
  base::SplitString(val, ' ', &vec);
  std::vector<std::string> faces;
  ObjFace facearray[3];
  int cnt = 0;
  for (auto iter = vec.begin(); iter != vec.end(); ++iter) {
    ObjFace f;
    faces.clear();
    base::SplitString(*iter, '/', &faces);
    CHECK_LE(faces.size(), 3u);
    CHECK(::base::StringToInt(faces[0], &f.v));
    if (faces.size() > 1 && faces[1].size() > 0u) {
      CHECK(::base::StringToInt(faces[1], &f.vt)) << faces[1];
    } else {
      f.vt = -1;
    }
      
    if (faces.size() > 2 && faces[2].size() > 0u) {
      CHECK(::base::StringToInt(faces[2], &f.vn)) << faces[2];
    } else {
      f.vn = -1;
    }
    facearray[cnt++] = f;
  }

  if (facearray[0].v != facearray[1].v
      && facearray[1].v != facearray[2].v
      && facearray[2].v != facearray[0].v) {
    curr_->f.push_back(facearray[0]);
    curr_->f.push_back(facearray[1]);
    curr_->f.push_back(facearray[2]);
  }
}
