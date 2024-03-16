add_library(async
  block.cpp
  parser.cpp
  stdoutwriter.cpp
  filewriter.cpp
  outq.cpp
  async.cpp
  bulkasync.cpp
)

target_link_libraries(async PRIVATE Boost::boost)

set_target_properties(async PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

install(TARGETS async LIBRARY DESTINATION lib)
