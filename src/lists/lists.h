#ifndef DIP_LISTS_H
#define DIP_LISTS_H

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "../settings.h"
#include "../nodes/nodes.h"

namespace dip {

  class NodeList {
  private:
    BaseNode::NodeListType nodes;
  public:
    size_t size() const;
    void push_front(std::shared_ptr<BaseNode> node);
    void push_back(std::shared_ptr<BaseNode> node);
    std::shared_ptr<BaseNode> pop_front();
    std::shared_ptr<BaseNode> pop_back();
    std::shared_ptr<BaseNode> operator[](const size_t index);
  };

  class SourceList; // EnvSource needs a forward declaration
  
  struct EnvSource {
    std::string name;
    std::string path;
    std::string code;
    Source parent;
    NodeList nodes;
    SourceList* sources = NULL;
  };
  
  class SourceList {
  private:
    std::map<std::string,EnvSource> sources;
  public:
    SourceList();
    void append(const std::string name, const std::string path, const std::string code, const Source parent);
    EnvSource& operator[](const std::string name);
  };

  struct Parent {
    int indent;
    std::string name;
  };
  
  class HierarchyList {
  public:
    std::vector<Parent> parents;
    void record(std::shared_ptr<BaseNode> node, const std::vector<Node::NodeKeyword>& excluded);
  };

}

#endif // DIP_LISTS_H
