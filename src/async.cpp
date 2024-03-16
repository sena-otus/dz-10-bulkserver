#include "async.h"
#include "bulkasync.h"

class BulkAsyncS
{

public:
  static BulkAsync *getBulkAsync()
  {
    static BulkAsync s;
    return &s;
  }
};


namespace async {

  handle_t connect(std::size_t N) {
    return BulkAsyncS::getBulkAsync()->connect(N);
  }

  void receive(handle_t handle, const char *data, std::size_t size) {
    BulkAsyncS::getBulkAsync()->receive(handle, data, size);
  }

  void disconnect(handle_t handle) {
    BulkAsyncS::getBulkAsync()->disconnect(handle);
  }

}
