include(AICxxProject)

add_executable(partitions partitions.cxx)
target_link_libraries(partitions PRIVATE ${AICXX_OBJECTS_LIST})
