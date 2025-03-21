#include <regex>

#include "lists.h"

namespace dip {

  // Get ID of a current branch
  std::string BranchingList::get_branch_id() {
    if (state.size()>0) {
      return state.back();
    } else
      throw std::runtime_error("Cannot get a branch ID because branch list is empty");
  }

  // Get ID of a current case
  std::string BranchingList::get_case_id(std::string branch_id) {
    if (branch_id.empty())
      branch_id = get_branch_id();
    Branch& branch = branches.at(branch_id);
    if (branch.cases.empty())
      throw std::runtime_error("Branch with an ID '"+branch_id+"' does not have any cases");
    else
      return branch.cases.back();
  }

  // Start a new branch
  int BranchingList::open_branch(const std::string case_id) {
    num_branches++;
    std::string branch_id = std::string(1, SIGN_CONDITION)+"B"+std::to_string(num_branches);
    state.push_back(branch_id);
    branches[branch_id] = Branch({case_id}, {std::string(KEYWORD_CASE)});
    return 0;
  }

  // Go to a new case within a branch
  int BranchingList::switch_case(const std::string case_id, const std::string case_type) {
    std::string branch_id = get_branch_id();
    Branch& branch = branches.at(branch_id);
    branch.cases.push_back(case_id);
    branch.types.push_back(case_type);
    return branch.cases.size()-1;
  }

  // Close current branch
  void BranchingList::close_branch() {
    if (state.empty())
      throw std::runtime_error("No more branches to be closed");
    else
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
    Branch& branch = branches.at(branch_id);
    int num_true = 0;
    // count number of cases with a true value in the current branch
    for (std::string case_id: branch.cases) {
      Case& cs = cases.at(case_id);
      if (cs.value==true)
	num_true++;
    }
    // get the value of the current case
    std::string case_id = get_case_id();
    Case& cs = cases.at(case_id);
    // the current case is false if this is the first true case in a branch
    return (num_true!=1 or !cs.value);
  }

  // Manage condition nodes
  void BranchingList::solve_case(BaseNode::PointerType node) {
    std::ostringstream oss;
    oss << "(.*[" << SIGN_CONDITION << "])C([0-9]+)";
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
	Case& cs = cases.at(case_id);
	path_old = cs.path;
      }
      // validate correct condition type and process end of the case
      if (cnode->case_type==KEYWORD_CASE) {
	// continue
      } else if (cnode->case_type==KEYWORD_ELSE and !cases.empty()) {
	// continue
      } else if (cnode->case_type==KEYWORD_END and !cases.empty() and path_old.size()>=path_new.size()) {
	close_branch();
	return;
      } else {
	std::cout << cases.size() << " " << path_old << " " << path_new << std::endl;
	throw std::runtime_error("Invalid condition type:  "+node->line.code);
      }
      // determine branch part and ID
      int branch_part;
      std::string case_id = std::string(1,SIGN_CONDITION) + "C" + matchResult[2].str();
      if (path_new==path_old) {
	branch_part = switch_case(case_id, cnode->case_type);
      } else if (path_new.size()<path_old.size()) {
	// close openned branches until the same branch is reached
	while (path_new!=path_old and path_old.size()>0) {
	  close_branch();
	  if (state.size()>0) {
	    std::string case_id_old = get_case_id();
	    Case& cs = cases.at(case_id_old);
	    path_old = cs.path;
	  } else {
	    path_old = "";
	  }
	}
	if (state.empty())
	  branch_part = open_branch(case_id);
	else
	  branch_part = switch_case(case_id, cnode->case_type);
      } else {
	branch_part = open_branch(case_id);
      }
      // get current branch id
      std::string branch_id = get_branch_id();
      // take into account values of the parent nodes
      // std::cout << "case " << branch_id << " " << branch_part << " " << case_id << " " << cnode->case_type << " " << cnode->value;
      bool case_value = cnode->value;
      if (state.size()>1) {
	std::string parent_branch_id = state[state.size()-2];
	std::string parent_case_id = get_case_id(parent_branch_id);
	Case& cs = cases.at(parent_case_id);
	case_value &= cs.value;
	// std::cout << " " << case_value << " " << parent_branch_id << parent_case_id;
      }
      // std::cout << std::endl;
      // register new case
      std::string expr = (cnode->value_raw.empty()) ? "" : cnode->value_raw.at(0);
      cases[case_id] = Case(path_new, cnode->line.code, expr, case_value,
			    branch_id, branch_part, case_id, cnode->case_type);
    } else {
      throw std::runtime_error("Invalid condition format: "+node->line.code);
    }
  }

  // Manage parameter nodes in a condition
  void BranchingList::prepare_node(BaseNode::PointerType node) {
    if (state.empty())
      return;
    std::string case_id = get_case_id();
    Case& cs = cases.at(case_id);
    // ending case at lower indent
    if (!(node->name.rfind(cs.path, 0) == 0)) {
      close_branch();
    }
    // register node to a branch
    if (!state.empty()) {
      std::string branch_id = get_branch_id();
      node->branch_id = branch_id;
      node->case_id = get_case_id();
      std::string node_name = clean_name(node->name);
      Branch& branch = branches.at(branch_id);
      auto it = branch.nodes.find(node_name);
      if (it == branch.nodes.end()) {
	branch.nodes[node_name] = 1;
      } else {
	branch.nodes[node_name]++;
      }
    }
  }

  std::string BranchingList::clean_name(const std::string& name) {
    std::ostringstream oss;
    oss << "(" << SIGN_CONDITION << "C[0-9]+" << SIGN_SEPARATOR << ")";
    std::regex pattern(oss.str());
    return std::regex_replace(name, pattern, "");
  }
  
}
