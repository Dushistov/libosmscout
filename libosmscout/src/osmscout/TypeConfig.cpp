/*
  This source is part of the libosmscout library
  Copyright (C) 2009  Tim Teulings

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
*/

#include <osmscout/TypeConfig.h>

#include <cassert>

namespace osmscout {

  Condition::~Condition()
  {
    // no code
  }

  TagEquals::TagEquals(TagId tag,
                       const std::string& tagValue)
  : tag(tag),
    tagValue(tagValue)
  {
    // no code
  }

  Condition* TagEquals::Copy() const
  {
    return new TagEquals(tag,tagValue);
  }

  bool TagEquals::Evaluate(const std::map<TagId,std::string>& tagMap)
  {
    std::map<TagId,std::string>::const_iterator t;

    t=tagMap.find(tag);

    if (t==tagMap.end()) {
      return false;
    }

    return t->second==tagValue;
  }

  TagInfo::TagInfo()
   : id(0)
  {
  }

  TagInfo::TagInfo(const std::string& name,
                   bool internalOnly)
   : id(0),
     name(name),
     internalOnly(internalOnly)
  {
    // no code
  }

  TagInfo& TagInfo::SetId(TagId id)
  {
    this->id=id;

    return *this;
  }

  TypeInfo::TypeInfo()
   : id(0),
     condition(NULL),
     canBeNode(false),
     canBeWay(false),
     canBeArea(false),
     canBeRelation(false),
     canBeOverview(false),
     canBeRoute(false),
     canBeIndexed(false)
  {
    // no code
  }

  TypeInfo::TypeInfo(const TypeInfo& other)
  {
    this->id=other.id;
    this->name=other.name;

    if (other.condition!=NULL) {
      this->condition=other.condition->Copy();
    }
    else {
      this->condition=NULL;
    }

    this->canBeNode=other.canBeNode;
    this->canBeWay=other.canBeWay;
    this->canBeArea=other.canBeArea;
    this->canBeRelation=other.canBeRelation;
    this->canBeOverview=other.canBeOverview;
    this->canBeRoute=other.canBeRoute;
    this->canBeIndexed=other.canBeIndexed;
  }

  TypeInfo::~TypeInfo()
  {
    delete condition;
  }

  void TypeInfo::operator=(const TypeInfo& other)
  {
    // Check for self assignment!
    if (this==&other) {
      return;
    }

    delete this->condition;

    this->id=other.id;
    this->name=other.name;

    if (other.condition!=NULL) {
      this->condition=other.condition->Copy();
    }
    else {
      this->condition=NULL;
    }

    this->canBeNode=other.canBeNode;
    this->canBeWay=other.canBeWay;
    this->canBeArea=other.canBeArea;
    this->canBeRelation=other.canBeRelation;
    this->canBeRoute=other.canBeRoute;
    this->canBeOverview=other.canBeOverview;
    this->canBeIndexed=other.canBeIndexed;
  }

  TypeInfo& TypeInfo::SetId(TypeId id)
  {
    this->id=id;

    return *this;
  }

  TypeInfo& TypeInfo::SetType(const std::string& name,
                              Condition* condition)
  {
    this->name=name;

    delete this->condition;
    this->condition=condition;

    return *this;
  }

  TypeConfig::TypeConfig()
   : nextTagId(0),
     nextTypeId(0)
  {
    // Make sure, that this is always registered first.
    // It assures that id 0 is always reserved for tagIgnore
    RegisterTagForInternalUse("");

    RegisterTagForExternalUse("admin_level");
    RegisterTagForExternalUse("boundary");
    RegisterTagForExternalUse("building");
    RegisterTagForExternalUse("bridge");
    RegisterTagForExternalUse("highway");
    RegisterTagForExternalUse("layer");
    RegisterTagForExternalUse("name");
    RegisterTagForExternalUse("natural");
    RegisterTagForExternalUse("oneway");
    RegisterTagForExternalUse("place");
    RegisterTagForExternalUse("place_name");
    RegisterTagForExternalUse("ref");
    RegisterTagForExternalUse("restriction");
    RegisterTagForExternalUse("tunnel");
    RegisterTagForExternalUse("type");
    RegisterTagForExternalUse("width");

    TypeInfo ignore;
    TypeInfo route;

    // Make sure, that this is always registered first.
    // It assures that id 0 is always reserved for typeIgnore
    ignore.SetType("",NULL);

    AddTypeInfo(ignore);

    route.SetType("_route",NULL)
         .CanBeWay(true);

    AddTypeInfo(route);

    tagAdminLevel=GetTagId("admin_level");
    tagBoundary=GetTagId("boundary");
    tagBuilding=GetTagId("building");
    tagBridge=GetTagId("bridge");
    tagLayer=GetTagId("layer");
    tagName=GetTagId("name");
    tagOneway=GetTagId("oneway");
    tagPlace=GetTagId("place");
    tagPlaceName=GetTagId("place_name");
    tagRef=GetTagId("ref");
    tagTunnel=GetTagId("tunnel");
    tagType=GetTagId("type");
    tagWidth=GetTagId("width");

    assert(tagAdminLevel!=tagIgnore);
    assert(tagBoundary!=tagIgnore);
    assert(tagBuilding!=tagIgnore);
    assert(tagBridge!=tagIgnore);
    assert(tagLayer!=tagIgnore);
    assert(tagName!=tagIgnore);
    assert(tagOneway!=tagIgnore);
    assert(tagPlace!=tagIgnore);
    assert(tagPlaceName!=tagIgnore);
    assert(tagRef!=tagIgnore);
    assert(tagTunnel!=tagIgnore);
    assert(tagType!=tagIgnore);
    assert(tagWidth!=tagIgnore);
  }

  TypeConfig::~TypeConfig()
  {
    // no code
  }

  const std::vector<TagInfo>& TypeConfig::GetTags() const
  {
    return tags;
  }

  const std::vector<TypeInfo>& TypeConfig::GetTypes() const
  {
    return types;
  }

  TagId TypeConfig::RegisterTagForInternalUse(const std::string& tagName)
  {
    std::map<std::string,TagId>::const_iterator mapping=stringToTagMap.find(tagName);

    if (mapping!=stringToTagMap.end()) {
      return mapping->second;
    }

    TagInfo tagInfo(tagName,true);

    if (tagInfo.GetId()==0) {
      tagInfo.SetId(nextTagId);

      nextTagId++;
    }
    else {
      nextTagId=std::max(nextTagId,(TagId)(tagInfo.GetId()+1));
    }

    tags.push_back(tagInfo);
    stringToTagMap[tagInfo.GetName()]=tagInfo.GetId();

    return tagInfo.GetId();
  }

  TagId TypeConfig::RegisterTagForExternalUse(const std::string& tagName)
  {
    std::map<std::string,TagId>::const_iterator mapping=stringToTagMap.find(tagName);

    if (mapping!=stringToTagMap.end()) {
      // TODO: Set to externalUse!
      return mapping->second;
    }

    TagInfo tagInfo(tagName,false);

    if (tagInfo.GetId()==0) {
      tagInfo.SetId(nextTagId);

      nextTagId++;
    }
    else {
      nextTagId=std::max(nextTagId,(TagId)(tagInfo.GetId()+1));
    }

    tags.push_back(tagInfo);
    stringToTagMap[tagInfo.GetName()]=tagInfo.GetId();

    return tagInfo.GetId();
  }

  void TypeConfig::RestoreTagInfo(const TagInfo& tagInfo)
  {
    // We have same tags, that are already and always
    // registered in the constructor, we skip them here...
    if (stringToTagMap.find(tagInfo.GetName())!=stringToTagMap.end()) {
      return;
    }

    assert(stringToTagMap.find(tagInfo.GetName())==stringToTagMap.end());
    assert(tagInfo.GetId()!=0 ||
           (tagInfo.GetId()==0 && tagInfo.GetName().empty()));

    nextTagId=std::max(nextTagId,(TagId)(tagInfo.GetId()+1));

    if (tags.size()>=tagInfo.GetId()) {
      tags.resize(tagInfo.GetId()+1);
    }

    tags[tagInfo.GetId()]=tagInfo;
    stringToTagMap[tagInfo.GetName()]=tagInfo.GetId();
  }

  TypeConfig& TypeConfig::AddTypeInfo(TypeInfo& typeInfo)
  {
    if (typeInfo.GetId()==0) {
      typeInfo.SetId(nextTypeId);

      nextTypeId++;
    }
    else {
      nextTypeId=std::max(nextTypeId,(TypeId)(typeInfo.GetId()+1));
    }

    if (nameToTypeMap.find(typeInfo.GetName())==nameToTypeMap.end()) {
      types.push_back(typeInfo);
      nameToTypeMap[typeInfo.GetName()]=typeInfo;
    }

    if (idToTypeMap.find(typeInfo.GetId())==idToTypeMap.end()) {
      idToTypeMap[typeInfo.GetId()]=typeInfo;
    }

    return *this;
  }

  TypeId TypeConfig::GetMaxTypeId() const
  {
    if (nextTypeId==0) {
      return 0;
    }
    else {
      return nextTypeId-1;
    }
  }

  TagId TypeConfig::GetTagId(const char* name) const
  {
    std::map<std::string,TagId>::const_iterator iter=stringToTagMap.find(name);

    if (iter!=stringToTagMap.end()) {
      return iter->second;
    }
    else {
      return tagIgnore;
    }
  }

  const TagInfo& TypeConfig::GetTagInfo(TagId id) const
  {
    assert(id<tags.size());

    return tags[id];
  }

  const TypeInfo& TypeConfig::GetTypeInfo(TypeId id) const
  {
    assert(id<types.size());

    return types[id];
  }

  void TypeConfig::ResolveTags(const std::map<TagId,std::string>& map,
                               std::vector<Tag>& tags) const
  {
    tags.clear();

    for (std::map<TagId,std::string>::const_iterator t=map.begin();
         t!=map.end();
         ++t) {
      if (GetTagInfo(t->first).IsInternalOnly()) {
        continue;
      }

      Tag tag;

      tag.key=t->first;
      tag.value=t->second;

      tags.push_back(tag);
    }
  }

  bool TypeConfig::GetNodeTypeId(const std::map<TagId,std::string>& tagMap,
                                 TypeId &typeId) const
  {
    typeId=typeIgnore;

    if (tagMap.size()==0) {
      return false;
    }

    for (size_t i=0; i<types.size(); i++) {
      if (types[i].GetCondition()==NULL ||
          !types[i].CanBeNode()) {
        continue;
      }

      if (types[i].GetCondition()->Evaluate(tagMap)) {
        typeId=types[i].GetId();
        return true;
      }
    }

    return false;
  }

  bool TypeConfig::GetWayAreaTypeId(const std::map<TagId,std::string>& tagMap,
                                    TypeId &wayType,
                                    TypeId &areaType) const
  {
    wayType=typeIgnore;
    areaType=typeIgnore;

    if (tagMap.size()==0) {
      return false;
    }

    for (size_t i=0; i<types.size(); i++) {
      if ((types[i].CanBeWay() || types[i].CanBeArea()) &&
          types[i].GetCondition()!=NULL &&
          types[i].GetCondition()->Evaluate(tagMap)) {
        if (types[i].CanBeWay()) {
          wayType=types[i].GetId();
        }

        if (types[i].CanBeArea()) {
          areaType=types[i].GetId();
        }

        if (wayType!=typeIgnore &&
            areaType!=typeIgnore) {
          return true;
        }
      }
    }

    return wayType!=typeIgnore || areaType!=typeIgnore;
  }

  bool TypeConfig::GetRelationTypeId(const std::map<TagId,std::string>& tagMap,
                                     TypeId &typeId) const
  {
    typeId=typeIgnore;

    if (tagMap.size()==0) {
      return false;
    }

    for (size_t i=0; i<types.size(); i++) {
      if (types[i].GetCondition()==NULL ||
          !types[i].CanBeRelation()) {
        continue;
      }

      if (types[i].GetCondition()->Evaluate(tagMap)) {
        typeId=types[i].GetId();
        return true;
      }
    }

    return false;
  }

  TypeId TypeConfig::GetNodeTypeId(const std::string& name) const
  {
    std::map<std::string,TypeInfo>::const_iterator iter=nameToTypeMap.find(name);

    if (iter!=nameToTypeMap.end() &&
        iter->second.CanBeNode()) {
      return iter->second.GetId();
    }

    return typeIgnore;
  }

  TypeId TypeConfig::GetWayTypeId(const std::string& name) const
  {
    std::map<std::string,TypeInfo>::const_iterator iter=nameToTypeMap.find(name);

    if (iter!=nameToTypeMap.end() &&
        iter->second.CanBeWay()) {
      return iter->second.GetId();
    }

    return typeIgnore;
  }

  TypeId TypeConfig::GetAreaTypeId(const std::string& name) const
  {
    std::map<std::string,TypeInfo>::const_iterator iter=nameToTypeMap.find(name);

    if (iter!=nameToTypeMap.end() &&
        iter->second.CanBeArea()) {
      return iter->second.GetId();
    }

    return typeIgnore;
  }

  TypeId TypeConfig::GetRelationTypeId(const std::string& name) const
  {
    std::map<std::string,TypeInfo>::const_iterator iter=nameToTypeMap.find(name);

    if (iter!=nameToTypeMap.end() &&
        iter->second.CanBeRelation()) {
      return iter->second.GetId();
    }

    return typeIgnore;
  }

  void TypeConfig::GetRoutables(std::set<TypeId>& types) const
  {
    types.clear();

    for (std::vector<TypeInfo>::const_iterator type=this->types.begin();
         type!=this->types.end();
         ++type) {
      if (type->CanBeRoute()) {
        types.insert(type->GetId());
      }
    }
  }

  void TypeConfig::GetIndexables(std::set<TypeId>& types) const
  {
    types.clear();

    for (std::vector<TypeInfo>::const_iterator type=this->types.begin();
         type!=this->types.end();
         ++type) {
      if (type->CanBeIndexed()) {
        types.insert(type->GetId());
      }
    }
  }
}
