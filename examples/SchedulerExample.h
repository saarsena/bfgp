#pragma once

#include "../include/scheduler/Scheduler.h"
#include "../include/scheduler/SchedulerUtils.h"
#include "../include/scheduler/TimedEventScheduler.h"
#include <entt/entt.hpp>
#include <iostream>

class SchedulerExample {
public:
  // Run the EntityScheduler example
  void runSchedulerExample();

  // Run the TimedEventScheduler example
  void runTimedEventExample();

  // Run both examples
  void run();
};