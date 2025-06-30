#include "events/event_hub.h"

#include <spdlog/spdlog.h>

#include <cassert>
#include <thread>

namespace events {

TransportUnitStateImpl::TransportUnitStateImpl(
    const std::shared_ptr<analyzer::TransportUnitState> &unit_state)
    : unit_state_(unit_state), total_event_count_(0) {}

void TransportUnitStateImpl::AddOneEvent() {
  std::lock_guard<std::mutex> lock(mutex_);
  if (0 == total_event_count_) {
    unit_state_->SetBusy();
  }
  total_event_count_++;
}

void TransportUnitStateImpl::RemoveOneEvent() {
  std::lock_guard<std::mutex> lock(mutex_);
  total_event_count_--;
  if (0 == total_event_count_) {
    unit_state_->SetReady();
  }
  if (total_event_count_ < 0) {
    spdlog::error("total_event_count_ < 0");
    exit(-1);
  }
}

int TransportUnitStateImpl::total_event_count() const { return total_event_count_; }

std::unique_ptr<TransportUnitStateImpl> g_transport_unit_state_impl;

void SetTransportUnitState(
    const std::shared_ptr<analyzer::TransportUnitState> &unit_state) {
  if (nullptr != g_transport_unit_state_impl) {
    spdlog::warn("SetTransportUnitState must be called once");
  }
  g_transport_unit_state_impl = std::make_unique<TransportUnitStateImpl>(unit_state);
}
}  // namespace events