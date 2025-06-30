#ifndef INCLUDE_ANALYZER_OBSERVABLE_UNITS_H_
#define INCLUDE_ANALYZER_OBSERVABLE_UNITS_H_

#include <bitset>
#include <functional>
#include <mutex>
#include <ostream>
#include <vector>

namespace analyzer {

class ObservableUnits {
  using UnitsReadyCallback = std::function<void()>;

 public:
  ObservableUnits(const ObservableUnits &) = delete;
  ObservableUnits(ObservableUnits &&) = delete;
  ObservableUnits &operator=(const ObservableUnits &) = delete;
  ObservableUnits &operator=(ObservableUnits &&) = delete;

  static ObservableUnits &Instance();

  enum class UnitId : std::size_t {
    kAnalyzer = 0,
    kStreamForwarder,
    kOrderBookSnapshotProvider,
    kRealMarketEmulator,
    kOrderPlanManager,
    kOrderManager,
    kTransport,
    COUNT
  };
  template <UnitId unit>
  class Unit {
   public:
    virtual ~Unit() = default;

    virtual void SetBusy() { ObservableUnits::Instance().SetBusy(unit); }
    virtual void SetReady() { ObservableUnits::Instance().SetReady(unit); }
  };

  void SubscribeToAllUnitsReady(const UnitsReadyCallback &cb);
  void Unsubscribe();
  std::vector<UnitId> GetBusyUnits() const;

  friend std::ostream &operator<<(std::ostream &os, const std::vector<UnitId> &o);

 private:
  ObservableUnits();

  void SetReady(UnitId unit);
  void SetBusy(UnitId unit);

  mutable std::mutex mutex_;
  std::bitset<static_cast<std::size_t>(UnitId::COUNT)> units_busy_;
  UnitsReadyCallback units_ready_cb_;
};

std::ostream &operator<<(std::ostream &os, const std::vector<ObservableUnits::UnitId> &o);

using AnalyzerUnitState = ObservableUnits::Unit<ObservableUnits::UnitId::kAnalyzer>;
using SavedStreamForwarderUnitState =
    ObservableUnits::Unit<ObservableUnits::UnitId::kStreamForwarder>;
using OrderBookSnapshotProviderUnitState =
    ObservableUnits::Unit<ObservableUnits::UnitId::kOrderBookSnapshotProvider>;
using RealMarketEmulatorUnitState =
    ObservableUnits::Unit<ObservableUnits::UnitId::kRealMarketEmulator>;
using OrderPlanManagerUnitState =
    ObservableUnits::Unit<ObservableUnits::UnitId::kOrderPlanManager>;
using OrderManagerUnitState =
    ObservableUnits::Unit<ObservableUnits::UnitId::kOrderManager>;
using TransportUnitState = ObservableUnits::Unit<ObservableUnits::UnitId::kTransport>;

}  // namespace analyzer

#endif  // INCLUDE_ANALYZER_OBSERVABLE_UNITS_H_