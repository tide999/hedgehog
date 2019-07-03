//
// Created by Bardakoff, Alexandre (IntlAssoc) on 2019-04-03.
//

#ifndef HEDGEHOG_DEFAULT_SCHEDULER_H
#define HEDGEHOG_DEFAULT_SCHEDULER_H

#include <thread>
#include <vector>
#include <algorithm>
#include "../../core/scheduler/abstract_scheduler.h"

class DefaultScheduler : public AbstractScheduler {
 private:
  std::unique_ptr<std::vector<std::thread>> threads_ = nullptr;
  std::unique_ptr<std::vector<std::shared_ptr<CoreNode>>> innerGraphs_ = nullptr;

 public:
  std::unique_ptr<AbstractScheduler> create() const override {
    return std::make_unique<DefaultScheduler>();
  }

  DefaultScheduler() {
    this->threads_ = std::make_unique<std::vector<std::thread>>();
    this->innerGraphs_ = std::make_unique<std::vector<std::shared_ptr<CoreNode>>>();
  }
  ~DefaultScheduler() override = default;

  void spawnThreads(std::vector<std::shared_ptr<CoreNode>> &insideCores) override {
    for (auto &core : insideCores) {
      if (core->type() != NodeType::Graph) {
        try {
          threads_->emplace_back(&CoreNode::run, core);
        } catch (const std::system_error &e) {
          std::cout << "Caught system_error with code " << e.code() << ": " << e.what() << '\n';
          exit(42);
        }
      }
      else { innerGraphs_->push_back(core); }
    }
  }

  void joinAll() override {
    std::for_each(threads_->begin(), threads_->end(), [](std::thread &t) { t.join(); });
    for (std::shared_ptr<CoreNode> &innerGraph : *(this->innerGraphs_)) { innerGraph->joinThreads(); }
  }

};

#endif //HEDGEHOG_DEFAULT_SCHEDULER_H
