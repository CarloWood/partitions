include(AICxxProject)

add_executable(partitions partitions.cxx Element.cxx ElementPair.cxx Group.cxx Partition.cxx PartitionIteratorBase.cxx PartitionIteratorBruteForce.cxx PartitionIterator.cxx Score.cxx Tree.cxx)
target_link_libraries(partitions PRIVATE ${AICXX_OBJECTS_LIST})

add_executable(Table Table.cxx Partition.cxx Group.cxx Element.cxx Score.cxx PartitionIteratorBase.cxx PartitionIteratorBruteForce.cxx PartitionIterator.cxx)
target_link_libraries(Table PRIVATE ${AICXX_OBJECTS_LIST})
