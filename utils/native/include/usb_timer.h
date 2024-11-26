/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef USB_TIMER_H
#define USB_TIMER_H

#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <functional>
#include <mutex>
#include <condition_variable>

class Timer {
public:
    Timer() : running_(false), exit_(false), workerThread_(&Timer::waitCycle, this) {}
    ~Timer()
    {
        stop();
        exit_ = true;
        cv_.notify_one();
        workerThread_.join();
    }

    void setInterval(unsigned int interval)
    {
        std::lock_guard<std::mutex> lock(cv_mutex_);
        interval_ = interval;
    }

    void start()
    {
        std::lock_guard<std::mutex> lock(cv_mutex_);
        if (!running_) {
            running_ = true;
            last_start_time_ = std::chrono::steady_clock::now();
            cv_.notify_one();
        }
    }

    void stop()
    {
        std::lock_guard<std::mutex> lock(cv_mutex_);
        if (running_) {
            running_ = false;
        }
    }

    void setCallback(std::function<void()> callback)
    {
        std::lock_guard<std::mutex> lock(cv_mutex_);
        callback_ = callback;
    }

private:
    void waitCycle()
    {
        while (!exit_) {
            std::unique_lock<std::mutex> lock(cv_mutex_);
            cv_.wait_for(lock, std::chrono::milliseconds(interval_), [this] {
                return !running_ || (std::chrono::steady_clock::now() - last_start_time_ >=
                    std::chrono::milliseconds(interval_));
            });
            if (running_) {
                running_ = false;
                lock.unlock();
                if (callback_) {
                    callback_();
                }
            } else {
                // Reset last start time to ensure the timer is not skipped when started again
                last_start_time_ = std::chrono::steady_clock::now();
            }
        }
    }

    std::atomic<bool> running_;
    std::atomic<bool> exit_;
    std::thread workerThread_;
    unsigned int interval_;
    std::function<void()> callback_;
    std::chrono::steady_clock::time_point last_start_time_;
    std::condition_variable cv_;
    std::mutex cv_mutex_;
};

#endif