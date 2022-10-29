#pragma once

#include "X_ASSERT.h"
#include "utils/BitSet.h"
#include "utils/has_print_on.h"
#include <iosfwd>

using utils::has_print_on::operator<<;

using elements_t = utils::BitSet<uint64_t>;
constexpr int number_of_elements = 24;

class Element;
using ElementIndex = elements_t::Index;
using ElementIndexPOD = utils::bitset::IndexPOD;

std::ostream& operator<<(std::ostream& os, ElementIndex index);

class Element
{
 private:
  elements_t m_bit;
  ElementIndex m_index;
  char m_name;

 public:
  static constexpr ElementIndexPOD s_ibegin = utils::bitset::index_begin;
  static constexpr ElementIndexPOD s_iend{number_of_elements};

  constexpr Element(ElementIndex index) : m_bit(index_to_bit(index)), m_index(index), m_name(index_to_name(index)) { }
  constexpr Element(elements_t bit) : m_bit(bit), m_index(bit.lssbi()), m_name(index_to_name(m_index)) { }
  explicit constexpr Element(char name) : m_bit(index_to_bit(name_to_index(name))), m_index(name_to_index(name)), m_name(name) { }

  static constexpr ElementIndex ibegin() { return s_ibegin; }
  static constexpr ElementIndex iend() { return s_iend; }

  constexpr operator elements_t() const { return m_bit; }

  constexpr Element operator|(Element const& rhs) const { return m_bit | rhs; }
  constexpr Element operator&(Element const& rhs) const { return m_bit & rhs; }
  constexpr Element operator^(Element const& rhs) const { return m_bit ^ rhs; }
  constexpr bool operator!=(Element const& rhs) const { return m_bit != rhs; }
  constexpr auto operator<=>(Element const& rhs) const { return m_bit <=> rhs.m_bit; }

 private:
  static constexpr char index_to_name(ElementIndex index)
  {
    return 'A' + index();
  }

  static constexpr elements_t index_to_bit(ElementIndex index)
  {
    X_ASSERT(index < Element::iend());
    return elements_t{1} << index();
  }

  static constexpr ElementIndex name_to_index(char name)
  {
    return ElementIndex{ElementIndexPOD{static_cast<int8_t>(name - 'A')}};
  }

 public:
  void print_on(std::ostream& os) const;
  friend std::ostream& operator<<(std::ostream& os, Element const& element);
};
