#pragma once

#include "Group.h"
#include "utils/Vector.h"

class Tree
{
 private:
  Element m_element;    // The element we're storing information about.
  GroupIndex m_group;   // The group that the current element is in.
  int m_count;          // Number of consecutive times the value m_group occurs.

  utils::Vector<Tree, GroupIndex> m_children; // The trees for subsequent elements.

 public:
  // Create an undefined dummy tree.
  Tree() : m_element('A') { }
  Tree(Element element, GroupIndex group) : m_element(element), m_group(group), m_count{0} { }

  Tree* update(Element e, GroupIndex g)
  {
    Tree* nt = this;
    if (g == m_children.iend())
      m_children.emplace_back(e, g);
    nt = &m_children[g];
    nt->m_count++;
    return nt;
  }

  void print_on(std::ostream& os) const;
};
