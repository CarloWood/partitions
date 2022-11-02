include(AICxxProject)

add_executable(partitions partitions.cxx Element.cxx ElementPair.cxx Set.cxx Partition.cxx PartitionIteratorBase.cxx PartitionIteratorSingleElement.cxx PartitionIteratorWholeSet.cxx PartitionIteratorBruteForce.cxx PartitionIteratorExplode.cxx PairTripletIteratorExplode.cxx PartitionIterator.cxx Score.cxx PartitionTask.cxx)
target_link_libraries(partitions PRIVATE ${AICXX_OBJECTS_LIST})
