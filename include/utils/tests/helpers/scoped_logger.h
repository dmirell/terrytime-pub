#ifndef INCLUDE_UTILS_TESTS_HELPERS_SCOPED_LOGGER_H_
#define INCLUDE_UTILS_TESTS_HELPERS_SCOPED_LOGGER_H_

#include <spdlog/sinks/ostream_sink.h>
#include <spdlog/spdlog.h>

#include <memory>
#include <sstream>
#include <string>

namespace utils {
class ScopedLogger {
 public:
  ScopedLogger() = delete;
  ScopedLogger(const ScopedLogger &) = delete;
  ScopedLogger(ScopedLogger &&) = delete;
  ScopedLogger &operator=(const ScopedLogger &) = delete;
  ScopedLogger &operator=(ScopedLogger &&) = delete;

  ScopedLogger(spdlog::level::level_enum level = spdlog::level::debug);

  ~ScopedLogger();

  std::string str() const;

 private:
  std::ostringstream log_stream_;
  std::shared_ptr<spdlog::logger> old_logger_;
  std::shared_ptr<spdlog::logger> ostream_logger_;
  std::shared_ptr<spdlog::sinks::sink> ostream_sink_;
};
}  // namespace utils

#endif  // INCLUDE_UTILS_TESTS_HELPERS_SCOPED_LOGGER_H_