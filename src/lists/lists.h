#ifndef DIP_LISTS_H
#define DIP_LISTS_H

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "../settings.h"
#include "../nodes/nodes.h"

namespace dip {

  // Node List
  
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

  // Source list
  
  class SourceList; // EnvSource needs a forward declaration
  
  struct EnvSource {
    std::string name;
    std::string path;
    std::string code;
    Source parent;
    NodeList nodes;
    std::shared_ptr<SourceList> sources;
  };
  
  class SourceList {
  private:
    std::map<std::string,EnvSource> sources;
  public:
    SourceList();
    void append(const std::string name, const std::string path, const std::string code, const Source parent);
    EnvSource& operator[](const std::string name);
  };

  // Hierarchy list
  
  struct Parent {
    int indent;
    std::string name;
  };
  
  class HierarchyList {
  public:
    std::vector<Parent> parents;
    void record(std::shared_ptr<BaseNode> node, const std::vector<Node::NodeDtype>& excluded);
  };

  // Branching list
  
  struct Case {
    std::string path;          // case path up to last @ sign
    bool value;                // final value of the case
    std::string code;          // code line with the case
    std::string expr;          // case expression
    std::string branch_id;     // branch ID
    std::string branch_part;   // branch part
    std::string case_id;       // case ID
    std::string case_type;     // one of the types: case/else/end
  };

  struct Branch {
    std::vector<std::string> cases;   // list of case IDs
    std::vector<std::string> types;   // list of case types
    std::map<std::string, int> nodes; // number of node definitions
  };
  
  class BranchingList {
  private:
    std::vector<std::string> state;         // list of openned branches
    std::map<std::string, Branch> branches; // all branches
    std::map<std::string, Case> cases;      // all cases
    int num_cases;                          
    int num_branches;
    std::string get_branch_id();
    std::string get_case_id();
    std::string open_branch(const std::string case_id);
    std::string switch_case(const std::string case_id, const std::string case_type);
    std::string close_branch();
  public:
    int register_case();
    bool false_case();
    void solve_case(std::shared_ptr<BaseNode> node);
    void prepare_node(std::shared_ptr<BaseNode> node);
  };
  
}

#endif // DIP_LISTS_H
