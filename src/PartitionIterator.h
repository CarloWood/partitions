#ifndef PARTITION_ITERATOR_H
#define PARTITION_ITERATOR_H

#include <cassert>
#include <memory>

class PartitionIteratorBase;
class Partition;

class PartitionIterator
{
 private:
  std::unique_ptr<PartitionIteratorBase> m_base;

 public:
  // Create an 'end' iterator.
  PartitionIterator();
  // Create an iterator that runs over the neighboring partitions.
  PartitionIterator(Partition const& orig, std::unique_ptr<PartitionIteratorBase>&& base);
  ~PartitionIterator();

  Partition operator*() const;
  PartitionIterator& operator++();
  friend bool operator!=(PartitionIterator const& lhs, PartitionIterator const& rhs);

  void print_on(std::ostream& os) const;
};

#endif // PARTITION_ITERATOR_H
