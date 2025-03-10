#include <sstream>
#include <string>
#include <iostream>

#include "dip.h"
#include "settings.h"

namespace dip {

  int DIP::num_instances = 0;
  
  DIP::DIP(const Environment* e) {
    // Initial settings
    name = DIP::num_instances++;
    source.name = "DIP"+std::to_string(name)+"_"+std::string(ROOT_SOURCE);
    source.line_number = 0;
    env.sources.append(source.name,"","",{"",0});
    
    // Populate node lists
    nodes_nohierarchy.insert(nodes_nohierarchy.end(), nodes_special.begin(), nodes_special.end());
    nodes_nohierarchy.insert(nodes_nohierarchy.end(), nodes_properties.begin(), nodes_properties.end());
    nodes_notypes.insert(nodes_notypes.end(), nodes_special.begin(), nodes_special.end());
    nodes_notypes.insert(nodes_notypes.end(), nodes_properties.begin(), nodes_properties.end());
    nodes_notypes.insert(nodes_notypes.end(), nodes_hierarchy.begin(), nodes_hierarchy.end());
  }
  
  void DIP::add_string(const std::string code) {
    std::istringstream ss(code);
    std::string token;
    num_strings += 1;
    std::string source_name = "DIP"+std::to_string(name)+"_"+std::string(STRING_SOURCE)+std::to_string(num_strings);
    int line_number = 0;
    while (getline(ss, token, SEPARATOR_NEWLINE)) {
      lines.push({token,{source_name,line_number}});
      line_number += 1;
    }
    int lineno = source.line_number;
    std::string path = env.sources[source.name].path;
    env.sources.append(source_name,path,code,{source.name, lineno});
  }
  
  std::string DIP::to_string() {
    return "DIP";
  }

  NodeList DIP::_get_queue() {
    NodeList queue;
    while (lines.size()>0) {
      Line line = lines.front();
      lines.pop();
      // group block strings
      size_t pos = 0;
      if ((pos = line.code.find(SIGN_BLOCK)) != std::string::npos) {      // opening """
	pos += SIGN_BLOCK.length();
	std::ostringstream oss;
	oss << line.code;
	if (line.code.find(SIGN_BLOCK, pos) == std::string::npos) {       // closing """ on the same line
	  while (lines.size()>0) {
	    Line block_line = lines.front();
	    lines.pop();
	    oss << SIGN_NEWLINE << block_line.code;
	    if (block_line.code.find(SIGN_BLOCK) != std::string::npos) {  // closing """ on a subsequent line
	      break;
	    }
	  }
	}
	line.code = oss.str();
      }
      // determine node type
      std::shared_ptr<BaseNode> node = _determine_node(line);
      queue.push_back(node);
    }
    return queue;
  }

  std::shared_ptr<BaseNode> DIP::_determine_node(Line& line) {
    // add replacement mark for escape symbols
    Parser::encode_escape_symbols(line.code);
    
    // determine node type
    Parser parser(line);
    std::shared_ptr<BaseNode> node(nullptr);
    node = EmptyNode::is_node(parser);
    if (node==nullptr) parser.part_indent();
    // if (node==nullptr) node = ImportNode::is_node(parser);
    // if (node==nullptr) node = UnitNode::is_node(parser);
    // if (node==nullptr) node = SourceNode::is_node(parser);
    if (node==nullptr) node = CaseNode::is_node(parser);
    if (node==nullptr) node = OptionsNode::is_node(parser);
    if (node==nullptr) node = ConstantNode::is_node(parser);
    if (node==nullptr) node = FormatNode::is_node(parser);
    if (node==nullptr) node = TagsNode::is_node(parser);
    if (node==nullptr) node = DescriptionNode::is_node(parser);
    // if (node==nullptr) node = ConditionNode::is_node(parser);
    if (node==nullptr) parser.part_name();
    if (node==nullptr) node = GroupNode::is_node(parser);
    // if (node==nullptr) node = ImportNode::is_node(parser);
    if (node==nullptr) node = ModificationNode::is_node(parser);
    if (node==nullptr) parser.part_type();
    if (node==nullptr) node = BooleanNode::is_node(parser);
    if (node==nullptr) node = IntegerNode::is_node(parser);
    if (node==nullptr) node = FloatNode::is_node(parser);
    if (node==nullptr) node = StringNode::is_node(parser);
    // if (node==nullptr) node = TableNode::is_node(parser);

    // make sure that everything was parsed
    if (node==nullptr)
      throw std::runtime_error("Node could not be determined from : "+line.code);
    if (parser.do_continue())
      throw std::runtime_error("Could not parse all text on the line: "+line.code);
    
    // convert escape symbols to original characterss
    if (!node->value_fn.empty())
      Parser::decode_escape_symbols(node->value_fn);
    if (!node->value_expr.empty())
      Parser::decode_escape_symbols(node->value_expr);
    for (size_t i=0; i<node->value_raw.size(); i++)
      Parser::decode_escape_symbols(node->value_raw[i]);
    Parser::decode_escape_symbols(node->line.code);
    
    return node;
  }
  
  Environment DIP::parse() {
    NodeList queue = _get_queue();
    Environment target = env;
    while (queue.size()>0) {
      std::shared_ptr<BaseNode> node = queue.pop_front();
      if (!target.branching.false_case() or node->has_dtype(Node::NODE_CASE)) {
	// TODO: value injecting
	// Perform specific node parsing only outside of case or inside of valid case
	BaseNode::NodeListType parsed = node->parse(target);
	if (parsed.size()>0) {
	  while (parsed.size()>0) {
	    queue.push_front(parsed.back());
	    parsed.pop_back();
	  }
	  continue;
	}
      }
      // Create hierarchical names
      target.hierarchy.record(node, nodes_nohierarchy);
      // Add nodes to the node list
      if (std::find(nodes_notypes.begin(), nodes_notypes.end(), node->dtype) != nodes_notypes.end()) {
	continue;
      } else if (node->dtype==Node::NODE_CASE) {
	target.branching.solve_case(node);
      }	else if (target.branching.false_case()) {
	continue;
      } else {
	target.branching.prepare_node(node);
	// Clean node name from cases
	node->name = target.branching.clean_name(node->name);
	// Set the node value
	// TODO: maybe this can be done after modifications?!
	std::shared_ptr<ValueNode> vnode = std::dynamic_pointer_cast<ValueNode>(node);
	if (vnode) {
	  vnode->set_value();
	}
	// If node was previously defined, modify its value
	bool new_node = true;
	for (size_t i=0; i<target.nodes.size(); i++) {
	  if (target.nodes[i]->name==node->name) {
	    std::shared_ptr<ValueNode> pnode = std::dynamic_pointer_cast<ValueNode>(target.nodes[i]);
	    pnode->validate_constant();
	    pnode->modify_value(node, target);
	    new_node = false;
	  }
	}
	if (new_node) {
	  if (node->dtype==Node::NODE_MODIFICATION) {
	    std::string prefix = "DIP"+std::to_string(name)+"_"+std::string(STRING_SOURCE);
	    if (node->line.source.name.compare(0, prefix.size(), prefix) == 0)
	      throw std::runtime_error("Modifying undefined node: "+node->line.code);
	  }
	  target.nodes.push_back(node);
	}
      }      
    }
    // Validate nodes
    for (ssize_t i=0; i<target.nodes.size(); i++) {
      std::shared_ptr<ValueNode> vnode = std::dynamic_pointer_cast<ValueNode>(target.nodes[i]);
      if (vnode) {
	vnode->validate_definition();
	vnode->validate_options();
	// TODO Check conditions
	vnode->validate_format();
      } else {
	throw std::runtime_error("Detected non-value node in the node list: "+target.nodes[i]->line.code);
      }
    }
    return target;
  }

}
