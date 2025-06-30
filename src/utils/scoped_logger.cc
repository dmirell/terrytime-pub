#include "utils/tests/helpers/scoped_logger.h"

namespace utils {

ScopedLogger::ScopedLogger(spdlog::level::level_enum level) {
  // Store the current default logger
  old_logger_ = spdlog::default_logger();

  // Create a string sink logger
  ostream_sink_ = std::make_shared<spdlog::sinks::ostream_sink_mt>(log_stream_);
  ostream_logger_ = std::make_shared<spdlog::logger>("gtest_logger", ostream_sink_);
  ostream_logger_->set_pattern(">%v<");
  ostream_logger_->set_level(level);

  // Set the ostream_logger
  spdlog::set_default_logger(ostream_logger_);
}

ScopedLogger::~ScopedLogger() {
  // Restore the old logger globally
  spdlog::set_default_logger(old_logger_);
}

std::string ScopedLogger::str() const { return log_stream_.str(); }

}  // namespace utils
