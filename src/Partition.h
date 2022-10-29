#ifndef PARTITION_H
#define PARTITION_H

#include "Element.h"
#include "Group.h"
#include "utils/Array.h"
#include "utils/RandomNumber.h"
#include <iosfwd>
#include <algorithm>

class PartitionIterator;
class PartitionIteratorBruteForce;
class PartitionIteratorScatter;

class Partition
{
 private:
  friend class PartitionIterator;
  utils::Array<Group, number_of_elements, GroupIndex> m_groups;

 public:
  Partition();

  template<ConceptGroup... GROUP>
  Partition(GROUP... groups) : m_groups{groups...}
  {
    for (GroupIndex gi{sizeof...(groups)}; gi != m_groups.iend(); ++gi)
      m_groups[gi] = Group{elements_t{elements_t::mask_type{0}}};
    sort();
  }

  Partition(utils::Array<Group, number_of_elements, GroupIndex> const& groups) : m_groups(groups) { sort(); }

  void print_groups() const;
  void print_on(std::ostream& os) const;

  Group group(GroupIndex group_index) const
  {
    return m_groups[group_index];
  }

  auto gbegin() const { return m_groups.ibegin(); }
  auto gend() const { return m_groups.iend(); }

  template<typename Algorithm>
  PartitionIterator begin() const;

  PartitionIterator end() const;

  static PartitionIteratorBruteForce bbegin();
  static PartitionIteratorBruteForce bend();

  PartitionIteratorScatter sbegin();
  PartitionIteratorScatter send();

  static utils::RandomNumber s_random_number;
  static Partition random();

  GroupIndex number_of_groups() const;
  bool is_alone(Element element) const;
  GroupIndex group_of(Element element) const;
  GroupIndex first_empty_group() const;
  Score score() const;

  void sort()
  {
    std::sort(m_groups.rbegin(), m_groups.rend());
  }

  void add_to(GroupIndex group_index, Group group)
  {
    m_groups[group_index].add(group);
  }

  void remove_from(GroupIndex group_index, Group group)
  {
    m_groups[group_index].remove(group);
  }

  Score find_local_maximum();

  friend bool operator<(Partition const& lhs, Partition const& rhs)
  {
    return lhs.m_groups < rhs.m_groups;
  }

  friend std::ostream& operator<<(std::ostream& os, Partition const& partition);
};

#endif // PARTITION_H

#ifndef PARTITION_ITERATOR_H
#include "PartitionIterator.h"
#endif

#ifndef PARTITION_H_definitions
#define PARTITION_H_definitions

template<typename Algorithm>
PartitionIterator Partition::begin() const
{
  return {*this, std::make_unique<Algorithm>(*this)};
}

#endif // PARTITION_H_definitions
