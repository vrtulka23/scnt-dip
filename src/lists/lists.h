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
    NodeList() {};
    NodeList(const BaseNode::NodeListType& nl): nodes(nl) {};
    size_t size() const;
    void push_front(BaseNode::PointerType node);
    void push_back(BaseNode::PointerType node);
    BaseNode::PointerType pop_front();
    BaseNode::PointerType pop_back();
    BaseNode::PointerType at(const size_t index);
    BaseNode::PointerType at(const size_t index) const;
  };

  // Source list
  
  class SourceList; // EnvSource needs a forward declaration
  
  struct EnvSource {
    std::string name;   // source key
    std::string path;   // source path
    std::string code;   // source code
    Source parent;      // parent source
    NodeList nodes;     // parsed nodes
    //std::shared_ptr<SourceList> sources;
  };
  
  class SourceList {
  private:
    std::map<std::string,EnvSource> sources;
  public:
    SourceList();
    void append(const std::string& name, const std::string& path, const std::string& code, const Source& parent);
    void append(const std::string& name, const EnvSource& src);
    EnvSource& operator[](const std::string& name);
    const EnvSource& operator[](const std::string& name) const;
  };

  // Hierarchy list
  
  struct Parent {
    int indent;
    std::string name;
  };
  
  class HierarchyList {
  private:
    std::vector<Parent> parents;
  public:
    void record(BaseNode::PointerType node, const std::vector<BaseNode::NodeDtype>& excluded);
  };

  // Branching list
  
  struct Case {
    std::string path;          // case path up to last @ sign
    std::string code;          // code line with the case
    std::string expr;          // case logical expression
    bool value;                // final value of the case
    std::string branch_id;     // branch ID
    int branch_part;           // pat on the branch
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
    std::string get_case_id(std::string branch_id="");
    int open_branch(const std::string case_id);
    int switch_case(const std::string case_id, const std::string case_type);
    void close_branch();
  public:
    BranchingList(): num_cases(0), num_branches(0) {};
    int register_case();
    bool false_case();
    void solve_case(BaseNode::PointerType node);
    void prepare_node(BaseNode::PointerType node);
    std::string clean_name(const std::string& node);
  };

  class FunctionList {
  public:
    typedef BaseValue::PointerType (*ValueFunctionType)(const Environment& env);
    typedef BaseNode::NodeListType (*TableFunctionType)(const Environment& env);
  private:
    std::map<std::string, ValueFunctionType> value_functions;
    std::map<std::string, TableFunctionType> table_functions;
  public:
    void append_value(const std::string& name, ValueFunctionType func);
    void append_table(const std::string& name, TableFunctionType func);
    ValueFunctionType get_value(const std::string& name) const;
    TableFunctionType get_table(const std::string& name) const;
  };
  
}

#endif // DIP_LISTS_H
