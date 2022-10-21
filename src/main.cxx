#include "sys.h"
#include "utils/Array.h"
#include "debug.h"
#include <iostream>
#include <cstdint>
#include <cassert>
#include <array>
#include <concepts>
#include <compare>

using elements_t = uint16_t;
constexpr int N = 5;

template<typename Category, std::integral INT>
class Index
{
 private:
  INT m_index;

 public:
  constexpr explicit Index(INT index) : m_index(index) { }
  INT get_value() const { return m_index; }
  Index& operator++()    { ++m_index; return *this; }
  Index& operator--()    { --m_index; return *this; }
  Index  operator++(int) { Index prev(m_index); ++m_index; return prev; }
  Index  operator--(int) { Index prev(m_index); --m_index; return prev; }
  auto operator<=>(Index rhs) { return m_index <=> rhs.m_index; }
  bool operator==(Index const&) const = default;  
};

class Element;
using ElementIndex = Index<Element, int>;

class Element
{
 private:
  char m_name;
  elements_t m_bit;
  ElementIndex m_index;

  static constexpr ElementIndex s_ibegin{0};
  static constexpr ElementIndex s_iend{N};

 public:
  Element(ElementIndex index) : m_name(index_to_name(index)), m_bit(index_to_bit(index)), m_index(index) { }
  explicit Element(char name) : m_name(name), m_bit(index_to_bit(name_to_index(name))), m_index(name_to_index(name)) { }

  static ElementIndex ibegin() { return s_ibegin; }
  static ElementIndex iend() { return s_iend; }

  operator elements_t() const { return m_bit; }

 private:
  static char index_to_name(ElementIndex index)
  {
    return 'A' + index.get_value();
  }

  static elements_t index_to_bit(ElementIndex index)
  {
    assert(index < Element::iend());
    return elements_t{1} << index.get_value();
  }

  static ElementIndex name_to_index(char name)
  {
    return ElementIndex{name - 'A'};
  }

 public:
  void print_on(std::ostream& os) const
  {
    os << m_name;
  }

  friend std::ostream& operator<<(std::ostream& os, Element const& element)
  {
    element.print_on(os);
    return os;
  }
};

class Group;
using GroupIndex = Index<Group, int>;

class Group
{
 private:
  elements_t m_elements{};

 public:
  Group() = default;
  Group(Element element) : m_elements(element) { }
  explicit Group(elements_t elements) : m_elements(elements) { }

  bool empty() const
  {
    return m_elements == 0;
  }

  Group& add(Group elements)
  {
    // Don't add existing elements.
    assert((m_elements & elements.m_elements) == 0);
    m_elements |= elements.m_elements;
    return *this;
  }

  Group& remove(Group group)
  {
    // Don't try to remove non-existing elements.
    assert((m_elements & group.m_elements) == group.m_elements);
    m_elements &= ~group.m_elements;
    return *this;
  }

  Group& toggle(Element element)
  {
    m_elements ^= element;
    return *this;
  }

  bool test(Element element) const
  {
    return (m_elements & element);
  }

  bool is_single_bit() const
  {
    return !((m_elements - 1) & m_elements);
  }

  void print_on(std::ostream& os) const
  {
    os << '{';
    for (ElementIndex i = Element::ibegin(); i != Element::iend(); ++i)
    {
      if ((m_elements & Element{i}))
        os << Element{i};
    }
    os << '}';
  }

  friend std::ostream& operator<<(std::ostream& os, Group const& group)
  {
    group.print_on(os);
    return os;
  }
};

class PartitionIterator;

class Partition
{
 private:
  friend class PartitionIterator;
  utils::Array<Group, N, GroupIndex> m_groups;

 public:
  Partition() : m_groups{Group{static_cast<elements_t>((1 << N) - 1)}, /*...*/} { }

  void print_on(std::ostream& os) const
  {
    os << '<';
    char const* prefix = "";
    for (GroupIndex g{0}; g < m_groups.size(); ++g)
    {
      if (m_groups[g].empty())
        break;
      os << prefix << m_groups[g];
      prefix = ", ";
    }
    os << '>';
  }

  PartitionIterator begin() const;
  PartitionIterator end() const;

  GroupIndex number_of_groups() const
  {
    GroupIndex g{0}
    while (g < m_groups.size())
    {
      if (m_groups[g].empty())
        break;
      ++g;
    }
    return g;
  }

  bool is_alone(Element element) const
  {
    for (GroupIndex g = 0; g < m_groups.size(); ++g)
      if (m_groups[g].test(element))
        return m_groups[g].is_single_bit();
    // Never reached.
    assert(false);
    return false;
  }

  GroupIndex group_of(Element element) const
  {
    for (GroupIndex g{0}; g < m_groups.size(); ++g)
      if (m_groups[g].test(element))
        return g;
    // Never reached.
    assert(false);
    return 0;
  }

  friend std::ostream& operator<<(std::ostream& os, Partition const& partition)
  {
    partition.print_on(os);
    return os;
  }
};

class PartitionIterator
{
 private:
  // The partition that we iterator over.
  Partition const m_original;
  GroupIndex const m_number_of_groups_original;
  // Loop variables.
  ElementIndex m_current_element_index{0};      // 0...N-1
  GroupIndex m_new_group{0};                    // 0...N-1
  // Cached information about the current element.
  GroupIndex m_current_element_group{};         // The group that the current element is in in the original.
  bool m_current_element_is_alone{};            // Whether or not it is the only elment in that group (in the original).

 public:
  // Create an 'end' iterator.
  PartitionIterator() : m_current_element_index(0), m_new_group{-1} { }
  // Create an iterator that runs over the neighboring partitions.
  PartitionIterator(Partition orig) : m_original(orig), m_number_of_groups_original(orig.number_of_groups()), m_current_element_group(orig.group_of(m_current_element_index)), m_current_element_is_alone(orig.is_alone(m_current_element_index))
  {
    if (m_current_element_group == m_new_group)
      ++*this;
  }

  Partition operator*() const
  {
    Partition p = m_original;
    p.m_groups[m_current_element_group].remove(Element{m_current_element_index});
    p.m_groups[m_new_group].add(Element{m_current_element_index});
    return p;
  }

  PartitionIterator& operator++()
  {
    //    0       1    2  3
    // <{C,D}, {A,B}, 0, 0>  --> 3  2+1
    // <{A,C,D}, {B}, 0, 0>
    // <{C,D}, {B}, {A}, 0>
    //
    // <{B,C,D}, {A}, 0, 0>  --> 1  2-1
    // <{A,B,C,D}, 0, 0, 0>
    //
    do
    {
      if (++m_new_group == (m_current_element_is_alone ? m_number_of_groups_original - 1 : m_number_of_groups_original + 1))
      {
        m_new_group = 0;
        if (++m_current_element_index == Element::iend())
        {
          m_current_element_index = ElementIndex{0};
          m_new_group = -1;
          break;
        }
        m_current_element_is_alone = m_original.is_alone(m_current_element_index);
        m_current_element_group = m_original.group_of(m_current_element_index);
      }
    }
    while (m_new_group == m_current_element_group);
    return *this;
  }

  friend bool operator!=(PartitionIterator const& lhs, PartitionIterator const& rhs)
  {
    return lhs.m_current_element_index != rhs.m_current_element_index || lhs.m_new_group != rhs.m_new_group;
  }
};

PartitionIterator Partition::begin() const
{
  return PartitionIterator(*this);
}

PartitionIterator Partition::end() const
{
  return {};
}

Element const A{'A'};
Element const B{'B'};
Element const C{'C'};
Element const D{'D'};
Element const E{'E'};
#if 0
Element const F{'F'};
Element const G{'G'};
Element const H{'H'};
Element const I{'I'};
Element const J{'J'};
Element const K{'K'};
Element const L{'L'};
Element const M{'M'};
Element const N{'N'};
Element const O{'O'};
Element const P{'P'};
#endif

int main()
{
  Partition p;

  std::cout << "Original: " << p << '\n';
  for (auto pi = p.begin(); pi != p.end(); ++pi)
  {
    Partition p2 = *pi;
    std::cout << "Original: " << p2 << '\n';
    for (auto pi2 = p2.begin(); pi2 != p2.end(); ++pi2)
      std::cout << *pi2 << '\n';
  }
}
