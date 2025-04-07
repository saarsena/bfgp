#pragma once

#include "../events/GameEvents.h"
#include "../scheduler/Scheduler.h"
#include "../scheduler/SchedulerUtils.h"
#include "../scheduler/TimedEventScheduler.h"
#include <entt/entt.hpp>
#include <iostream>

class SchedulerExample {
public:
  // Run the EntityScheduler example
  void runSchedulerExample();

  // Run the TimedEventScheduler example
  void runTimedEventExample();

  // Run the Event Integration example
  void runEventIntegrationExample();

  // Run all examples
  void run();
};