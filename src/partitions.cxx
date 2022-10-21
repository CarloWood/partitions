#include "sys.h"
#include "utils/Array.h"
#include "utils/Index.h"
#include "utils/BitSet.h"
#include "utils/has_print_on.h"
#include "debug.h"
#include <iostream>
#include <cstdint>
#include <cassert>
#include <array>
#include <compare>
#include <random>

#if defined NDEBUG
# define X_ASSERT(CHECK) void(0)
#else
# define X_ASSERT(CHECK) \
    ( (CHECK) ? void(0) : []{assert(!#CHECK);}() )
#endif

using utils::has_print_on::operator<<;

using elements_t = utils::BitSet<uint16_t>;
constexpr int N = 16;

class Element;
using ElementIndex = elements_t::Index;
using ElementIndexPOD = utils::bitset::IndexPOD;

std::ostream& operator<<(std::ostream& os, ElementIndex index)
{
  os << '#' << (int)index();
  return os;
}

class Element
{
 private:
  elements_t m_bit;
  ElementIndex m_index;
  char m_name;

  static constexpr ElementIndexPOD s_ibegin = utils::bitset::index_begin;
  static constexpr ElementIndexPOD s_iend{N};

 public:
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
    return ElementIndex{static_cast<ElementIndexPOD>(name - 'A')};
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
using GroupIndex = utils::ArrayIndex<Group>;

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
    return m_elements.none();
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

  void print_on(std::ostream& os) const
  {
    os << '{';
    for (ElementIndex i = Element::ibegin(); i != Element::iend(); ++i)
    {
      if (m_elements.test(Element{i}))
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
    for (GroupIndex g = m_groups.ibegin(); g != m_groups.iend(); ++g)
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
    GroupIndex g = m_groups.ibegin();
    while (g != m_groups.iend())
    {
      if (m_groups[g].empty())
        break;
      ++g;
    }
    return g;
  }

  bool is_alone(Element element) const
  {
    for (GroupIndex g = m_groups.ibegin(); g != m_groups.iend(); ++g)
      if (m_groups[g].test(element))
        return m_groups[g].is_single_bit();
    // Never reached.
    assert(false);
    return false;
  }

  GroupIndex group_of(Element element) const
  {
    for (GroupIndex g = m_groups.ibegin(); g != m_groups.iend(); ++g)
      if (m_groups[g].test(element))
        return g;
    // Never reached.
    assert(false);
    return {};
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
  ElementIndex m_current_element_index{Element::ibegin()};      // 0...N-1
  GroupIndex m_new_group{0};                                    // 0...N-1
  // Cached information about the current element.
  GroupIndex m_current_element_group{};         // The group that the current element is in in the original.
  bool m_current_element_is_alone{};            // Whether or not it is the only elment in that group (in the original).

 public:
  // Create an 'end' iterator.
  PartitionIterator() : m_current_element_index(Element::ibegin()), m_new_group{} { }
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
        m_new_group.set_to_zero();
        if (++m_current_element_index == Element::iend())
        {
          m_current_element_index = Element::ibegin();
          m_new_group.set_to_undefined();
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

class ElementPair
{
 private:
  elements_t m_element_pair;    // Has the two bits set that make up the pair; or none when this pair is undefined.

 public:
  // Construct an undefined element pair.
  ElementPair() = default;
  constexpr ElementPair(Element element1, Element element2) : m_element_pair(element1 | element2)
  {
    X_ASSERT(element1 != element2);
  }

  constexpr elements_t get_pair() const { return m_element_pair; }

  void print_on(std::ostream& os) const
  {
    for (Element element : m_element_pair)
      os << element;
  }
};

enum Infinite
{
  negative_inf,
  normal,
  positive_inf
};

class Score
{
 private:
  Infinite m_infinite;
  double m_value;       // Only valid when m_infinity == normal.

 public:
  Score(double value = 0.0) : m_infinite(normal), m_value(value) { }
  Score(Infinite inf) : m_infinite(inf), m_value(0) { }

  bool is_zero() const
  {
    return m_infinite == normal && m_value == 0.0;
  }

  bool is_infinite() const
  {
    return m_infinite != normal;
  }

  bool is_positive_inf() const
  {
    return m_infinite == positive_inf;
  }

  double value() const
  {
    // Don't call this when is_infinite returns true.
    assert(m_infinite == normal);
    return m_value;
  }

  void print_on(std::ostream& os) const
  {
    if (m_infinite == negative_inf)
      os << "-inf";
    else if (m_infinite == normal)
      os << m_value;
    else
      os << "+inf";
  }
};

class ScoreSum
{
 private:
  int m_number_of_positive_inf;
  double m_normal_sum;
  int m_number_of_negative_inf;

 public:
  ScoreSum() : m_number_of_positive_inf(0), m_normal_sum(0.0), m_number_of_negative_inf(0) { }

  void clear()
  {
    m_number_of_positive_inf = 0;
    m_normal_sum = 0.0;
    m_number_of_negative_inf = 0;
  }

  ScoreSum& operator+=(Score score)
  {
    if (score.is_infinite())
    {
      if (score.is_positive_inf())
        ++m_number_of_positive_inf;
      else
        ++m_number_of_negative_inf;
    }
    else
      m_normal_sum += score.value();
    return *this;
  }

  ScoreSum& operator+=(ScoreSum const& score)
  {

    return *this;
  }


  ScoreSum& operator=(Score score)
  {
    clear();
    return this->operator+=(score);
  }

  friend bool operator<(ScoreSum const& lhs, ScoreSum const& rhs)
  {
    if (lhs.m_number_of_negative_inf != rhs.m_number_of_negative_inf)
      return lhs.m_number_of_negative_inf > rhs.m_number_of_negative_inf;

    if (lhs.m_number_of_positive_inf != rhs.m_number_of_positive_inf)
      return lhs.m_number_of_positive_inf < rhs.m_number_of_positive_inf;

    return lhs.m_normal_sum < rhs.m_normal_sum;
  }

  void print_on(std::ostream& os) const
  {
    if (m_number_of_negative_inf != 0)
      os << "<NEGATIVE INFINITY!>";
    else
    {
      os << m_normal_sum;
      if (m_number_of_positive_inf)
        os << " + inf * " << m_number_of_positive_inf;
    }
  }
};

Element const A{'A'};
Element const B{'B'};
Element const C{'C'};
Element const D{'D'};
Element const E{'E'};
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

constexpr int number_of_pairs = N * (N - 1) / 2;
constexpr ElementIndexPOD last_element_index{N - 1};
constexpr ElementIndexPOD second_last_element_index{N - 2};
constexpr ElementIndex last_element{last_element_index};
constexpr ElementIndex second_last_element{second_last_element_index};
constexpr elements_t::mask_type max_score_index = ElementPair{second_last_element, last_element}.get_pair().to_ulong();
std::array<Score, max_score_index + 1> g_scores;
std::array<Score, 8> g_possible_scores = {
  negative_inf,
  -100,
  -5,
  -1,
  1,
  5,
  100,
  positive_inf
};

Score const& score(ElementPair ep)
{
  elements_t::mask_type score_index = ep.get_pair().to_ulong();
  return g_scores[score_index];
}

int main()
{
  std::mt19937::result_type seed{1};
  std::mt19937 gen(seed);
  std::uniform_int_distribution<int> distrib(0, g_possible_scores.size() - 1);

  Partition p;

  std::cout << "Original: " << p << '\n';
  for (auto pi = p.begin(); pi != p.end(); ++pi)
    std::cout << *pi << '\n';

  for (ElementIndex i1 = Element::ibegin(); i1 != Element::iend(); ++i1)
  {
    for (ElementIndex i2 = i1 + 1; i2 != Element::iend(); ++i2)
    {
      ElementPair ep(i1, i2);
      elements_t::mask_type score_index = ep.get_pair().to_ulong();
      int rv = distrib(gen);
      g_scores[score_index] = g_possible_scores[rv];

      std::cout << ep << " : " << score(ep) << '\n';
    }
  }
}
