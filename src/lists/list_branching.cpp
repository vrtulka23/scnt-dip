#include <regex>

#include "lists.h"

namespace dip {

  // Get ID of a current branch
  std::string BranchingList::get_branch_id() {
    if (state.empty())
      throw std::runtime_error("Branch list is empty");
    else
      return state.back();
  }

  // Get ID of a current case
  std::string BranchingList::get_case_id() {
    std::string branch_id = get_branch_id();
    Branch branch = branches.at(branch_id);
    if (branch.cases.empty())
      throw std::runtime_error("Branch with an ID '"+branch_id+"' does not have any cases");
    else
      return branch.cases.back();
  }

  // Start a new branch
  int BranchingList::open_branch(const std::string case_id) {
    num_branches++;
    std::string branch_id = std::to_string(SIGN_CONDITION)+std::to_string(num_branches);
    state.push_back(branch_id);
    branches[branch_id] = Branch({case_id}, {std::string(KEYWORD_CASE)});
    return 0;
  }

  // Go to a new case within a branch
  int BranchingList::switch_case(const std::string case_id, const std::string case_type) {
    std::string branch_id = get_branch_id();
    Branch branch = branches.at(branch_id);
    branch.cases.push_back(case_id);
    branch.types.push_back(case_type);
    return branch.cases.size()-1;
  }

  // Close current branch
  void BranchingList::close_branch() {
    state.pop_back();
  }

  // Add a new case
  int BranchingList::register_case() {
    return ++num_cases;
  }

  // Checks if case value is false
  bool BranchingList::false_case() {
    if (state.empty())
      return false;
    std::string branch_id = get_branch_id();
    Branch branch = branches.at(branch_id);
    int num_true = 0;
    for (std::string case_id: branch.cases) {
      Case cs = cases.at(case_id);
      if (cs.value==true)
	num_true++;
    }
    if (num_true!=1)
      return true;
    std::string case_id = get_case_id();
    Case cs = cases.at(case_id);
    if (!cs.value)
      return true;
    return false;
  }

  // Manage condition nodes
  void BranchingList::solve_case(std::shared_ptr<BaseNode> node) {
    std::ostringstream oss;
    oss << "^([a-zA-Z0-9_.-]*[" << SIGN_CONDITION << "])([0-9]+)";
    std::regex pattern(oss.str());
    std::smatch matchResult;
    if (std::regex_search(node->name, matchResult, pattern)) {
      std::shared_ptr<CaseNode> cnode = std::dynamic_pointer_cast<CaseNode>(node);
      if (cnode==nullptr)
	throw std::runtime_error("Given node must be a case node:  "+node->line.code);
      std::string path_new = matchResult[1].str();
      std::string path_old;
      if (!state.empty()) {
	std::string case_id = get_case_id();
	Case cs = cases.at(case_id);
	path_old = cs.path;
      }
      // validate correct condition type and process end of the case
      if (cnode->case_type==KEYWORD_CASE) {
	// continue
      } else if (cnode->case_type==KEYWORD_ELSE and !cases.empty()) {
	// continue
      } else if (cnode->case_type==KEYWORD_END and !cases.empty() and path_old==path_new) {
	close_branch();
	return;
      } else {
	throw std::runtime_error("Invalid condition:  "+node->line.code);
      }
      // determine branch part and ID
      int branch_part;
      std::string case_id = std::string(1,SIGN_CONDITION) + matchResult[2].str();
      if (path_new==path_old) {
	branch_part = switch_case(case_id, cnode->case_type);
      } else if (path_new.size()<path_old.size()) {
	// close openned branches until the same branch is reached
	while (path_new!=path_old) {
	  close_branch();
	  if (!state.empty()) {
	    std::string case_id_old = get_case_id();
	    Case cs = cases.at(case_id_old);
	    path_old = cs.path;
	  } else {
	    path_old = "";
	  }
	}
	branch_part = switch_case(case_id, cnode->case_type);
      } else {
	branch_part = open_branch(case_id);
      }
      std::string branch_id = get_branch_id();
      // register new case
      cases[case_id] = Case(path_new, cnode->line.code, cnode->value_expr, cnode->value,
			    branch_id, branch_part, case_id, cnode->case_type);
    } else {
      throw std::runtime_error("Invalid condition: "+node->line.code);
    }
  }

  /// Manage parameter nodes in a condition
  void BranchingList::prepare_node(std::shared_ptr<BaseNode> node) {
    if (state.empty())
      return;
    std::string case_id = get_case_id();
    Case cs = cases.at(case_id);
    // ending case at lower indent
    if (!(node->name.rfind(cs.path, 0) == 0)) {
      close_branch();
    }
    // register node to a branch
    if (!state.empty()) {
      std::string branch_id = get_branch_id();
      node->branch_id = branch_id;
      node->case_id = get_case_id();
      std::string node_name = node->clean_name();
      Branch branch = branches.at(branch_id);
      auto it = branch.nodes.find(node_name);
      if (it == branch.nodes.end()) {
	branch.nodes[node_name] = 1;
      } else {
	branch.nodes[node_name]++;
      }
    }
  }

}
