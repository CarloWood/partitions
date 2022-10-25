include(AICxxProject)

add_executable(partitions partitions.cxx Element.cxx ElementPair.cxx Group.cxx Partition.cxx PartitionIteratorBase.cxx PartitionIteratorBruteForce.cxx PartitionIterator.cxx Score.cxx)
target_link_libraries(partitions PRIVATE ${AICXX_OBJECTS_LIST})
