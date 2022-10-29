#pragma once

#include "Element.h"
#include "Score.h"
#include "utils/Array.h"
#include "utils/reverse_bits.h"
#include <iosfwd>
#include <cassert>
#include <concepts>

class Group;
using GroupIndex = utils::ArrayIndex<Group>;

class Group
{
 private:
  elements_t m_elements{};

 public:
  Group() = default;
  Group(Element element) : m_elements(element) { }
  explicit Group(elements_t elements) : m_elements(elements) { }

  utils::bitset::const_iterator<elements_t::mask_type> begin() const { return m_elements.begin(); }
  utils::bitset::const_iterator<elements_t::mask_type> end() const { return m_elements.end(); }

  bool empty() const
  {
    return m_elements.none();
  }

  void clear()
  {
    m_elements.reset();
  }

  Group& add(Group elements)
  {
    // Don't add existing elements.
    assert(!m_elements.test(elements.m_elements));
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
    return m_elements.test(element);
  }

  bool is_single_bit() const
  {
    return m_elements.is_single_bit();
  }

  int element_count() const
  {
    return m_elements.count();
  }

  Score score() const;

  friend bool operator<(Group lhs, Group rhs)
  {
    return utils::reverse_bits(lhs.m_elements()) < utils::reverse_bits(rhs.m_elements());
  }

  void print_on(std::ostream& os) const;
};

template<typename T>
concept ConceptGroup = std::is_convertible_v<T, Group>;
