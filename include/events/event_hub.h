#ifndef INCLUDE_EVENTS_EVENT_HUB_H_
#define INCLUDE_EVENTS_EVENT_HUB_H_

#include <spdlog/spdlog.h>

#include <atomic>
#include <condition_variable>
#include <functional>
#include <memory>
#include <queue>
#include <thread>
#include <vector>

#include "analyzer/observable_units.h"
#include "events.h"

#define SUBSCRIBE_TO_EVENT(event_handler_ptr, method_ptr)                             \
  do {                                                                                \
    auto event_handler_locked = event_handler_ptr.lock();                             \
    if (event_handler_locked) {                                                       \
      event_handler_locked->Subscribe(                                                \
          std::bind(method_ptr, this, std::placeholders::_1, std::placeholders::_2)); \
    }                                                                                 \
  } while (0)

namespace events {

class TransportUnitStateImpl;
extern std::unique_ptr<TransportUnitStateImpl> g_transport_unit_state_impl;

template <class MessageQueue>
class EventHub {
  using Event = typename MessageQueue::Event;

  template <Event e>
  using EventType = typename MessageQueue::template EventTypeFromId<e>::type;

  using EventsHandleCallback = std::function<void(Event event, const void *data)>;

 public:
  EventHub(const EventHub &) = delete;
  EventHub(EventHub &&) = delete;
  EventHub &operator=(const EventHub &) = delete;
  EventHub &operator=(EventHub &&) = delete;

  EventHub() = default;
  ~EventHub() { Shutdown(); }

  class Handler {
   public:
    Handler(EventHub &event_hub) : event_hub_(event_hub) {}

    void Subscribe(const EventsHandleCallback &cb) {
      spdlog::debug("Create new MessageQueueThread for MQ: {}", MessageQueue::kClassName);
      auto message_queue = std::make_shared<MessageQueueThread>(cb);
      event_hub_.message_queues_.push_back(message_queue);
    }

   private:
    EventHub &event_hub_;
  };

  class Dispatcher {
   public:
    Dispatcher(EventHub &event_hub) : event_hub_(event_hub) {}

    template <Event e>
    void DispatchEvent(const EventType<e> &event_data) {
      if (!event_hub_.event_hub_stopped_) {
        for (auto &it : event_hub_.message_queues_) {
          it->template PushEvent<e>(event_data);
        }
      }
    }

   private:
    EventHub &event_hub_;
  };

  std::weak_ptr<Dispatcher> dispatcher() {
    if (nullptr == dispatcher_) {
      dispatcher_ = std::make_shared<Dispatcher>(*this);
    }
    return dispatcher_;
  }

  std::weak_ptr<Handler> CreateHandler() {
    handlers_.emplace_back(std::make_shared<Handler>(*this));
    return handlers_.back();
  }

  void Shutdown() {
    spdlog::info("Stopping MessageQueueThreads...");
    event_hub_stopped_ = true;
    for (auto &it : message_queues_) {
      it->Stop();
    }
    spdlog::info("Stopping MessageQueueThreads finished");
  }

  void WaitForAllEventsProcessed() {
    bool is_all_events_processed = true;
    do {
      is_all_events_processed = true;
      for (const auto &it : message_queues_) {
        if (it->IsWorking()) {
          is_all_events_processed = false;
          break;
        }
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
    } while (!is_all_events_processed);
  }

 private:
  class MessageQueueThread {
   public:
    MessageQueueThread(const EventsHandleCallback &cb)
        : cb_(cb), thread_(std::bind(&MessageQueueThread::ThreadLoop, this)) {
      spdlog::debug("MessageQueueThread initialized");
    }

    template <Event e>
    void PushEvent(const EventType<e> &event_data) {
      std::lock_guard<std::mutex> lock(mutex_);

      if (g_transport_unit_state_impl) {
        g_transport_unit_state_impl->AddOneEvent();
      }

      events_queue_.push([this, event_data]() {
        auto *event_data_ptr = &event_data;
        cb_(e, static_cast<const void *>(event_data_ptr));
      });
      cv_.notify_one();
    }

    void Stop() {
      spdlog::debug("Stopping MessageQueueThread for MQ: {}", MessageQueue::kClassName);
      is_stopped_ = true;
      cv_.notify_one();
      if (thread_.joinable()) {
        thread_.join();
      }
      spdlog::debug("MessageQueueThread finished");
    }

    bool IsWorking() const {
      std::lock_guard<std::mutex> lock(mutex_);
      return !events_queue_.empty() || is_processing_event_;
    }

   private:
    using EventFunc = std::function<void()>;

    void ThreadLoop() {
      while (true) {
        std::unique_lock<std::mutex> lock(mutex_);
        is_processing_event_ = false;
        cv_.wait(lock, [this] { return !events_queue_.empty() || is_stopped_; });

        if (is_stopped_ && events_queue_.empty()) {
          return;
        }

        is_processing_event_ = true;

        auto event = events_queue_.front();
        events_queue_.pop();

        lock.unlock();

        event();

        if (g_transport_unit_state_impl) {
          g_transport_unit_state_impl->RemoveOneEvent();
        }
      }
    }

    EventsHandleCallback cb_;
    std::thread thread_;
    std::atomic<bool> is_stopped_{false};
    bool is_processing_event_{false};
    std::condition_variable cv_;
    mutable std::mutex mutex_;
    std::queue<EventFunc> events_queue_;
  };

  std::shared_ptr<Dispatcher> dispatcher_;
  std::vector<std::shared_ptr<Handler>> handlers_;
  std::atomic<bool> event_hub_stopped_{false};
  std::vector<std::shared_ptr<MessageQueueThread>> message_queues_;
};

void SetTransportUnitState(
    const std::shared_ptr<analyzer::TransportUnitState> &unit_state);

class TransportUnitStateImpl {
 public:
  TransportUnitStateImpl(const std::shared_ptr<analyzer::TransportUnitState> &unit_state);

  void AddOneEvent();
  void RemoveOneEvent();

  int total_event_count() const;

 private:
  std::mutex mutex_;
  std::shared_ptr<analyzer::TransportUnitState> unit_state_;
  int total_event_count_;
};

}  // namespace events

#endif  // INCLUDE_EVENTS_EVENT_HUB_H_
