#pragma once

#include "base/basictypes.h"
#include "base/time/time.h"

namespace azer {
class FrameCounter {
 public:
  FrameCounter(double timespan = 0.3f)
      : kClearTimespan(timespan) {
    frame_cnt_ = 0;
    frame_cnt_since_prev_clear_ = 0;
    prev_average_ = 0.0f;
    start_ = 0.0f;
    prev_clear_ = 0.0f;
  }

  void Update(double time, float delta) {
    frame_cnt_++;
    frame_cnt_since_prev_clear_++;
    if (time - prev_clear_ > kClearTimespan) {
      prev_average_ = CalcFramePerSeconds(time);
      prev_clear_ = time;
      frame_cnt_since_prev_clear_ = 0.0;
    }
  }

  float GetAverageFPS(double now, float delta) {
    if (now - frame_cnt_since_prev_clear_ > kClearTimespan / 2.0f) {
      return CalcFramePerSeconds(now);
    } else {
      return prev_average_;
    }
  }

  float GetCurrentFPS(double now, float delta) {
    return 1.0f / delta;
  }

  uint64 Frames() const { return frame_cnt_;}
 private:
  float CalcFramePerSeconds(double now) {
    return (double)frame_cnt_since_prev_clear_ / (now - prev_clear_);
  }

  uint64 frame_cnt_;
  uint32 frame_cnt_since_prev_clear_;
  double prev_clear_;
  double start_;
  float prev_average_;
  const double kClearTimespan;
  DISALLOW_COPY_AND_ASSIGN(FrameCounter);
};
}  // namespace azer
