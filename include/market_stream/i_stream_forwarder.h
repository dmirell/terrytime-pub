#ifndef INCLUDE_MARKET_STREAM_I_STREAM_FORWARDER_H_
#define INCLUDE_MARKET_STREAM_I_STREAM_FORWARDER_H_

#include <future>

namespace market_stream {

class IStreamForwarder {
 public:
  IStreamForwarder() {}

  virtual std::future<void> StartAsync() = 0;
  virtual ~IStreamForwarder() = default;

 protected:
  std::promise<void> stream_started_promise_;
};

}  // namespace market_stream

#endif  // INCLUDE_MARKET_STREAM_I_STREAM_FORWARDER_H_