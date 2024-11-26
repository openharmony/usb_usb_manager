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
        cv_.notify_one(); // Wake up the thread if it's waiting
        workerThread_.join();
    }

    // 设置定时时间（单位：毫秒）
    void setInterval(unsigned int interval)
    {
        std::lock_guard<std::mutex> lock(cv_mutex_);
        interval_ = interval;
    }

    // 启动计时器
    void start()
    {
        std::lock_guard<std::mutex> lock(cv_mutex_);
        if (!running_) {
            running_ = true;
            last_start_time_ = std::chrono::steady_clock::now();
            cv_.notify_one(); // Wake up the thread if it's waiting
        }
    }

    // 停止计时器
    void stop()
    {
        std::lock_guard<std::mutex> lock(cv_mutex_);
        if (running_) {
            running_ = false;
        }
    }

    // 设置回调函数
    void setCallback(std::function<void()> callback)
    {
        std::lock_guard<std::mutex> lock(cv_mutex_);
        callback_ = callback;
    }

private:
    // 计时器等待和执行周期的函数
    void waitCycle()
    {
        while (!exit_) {
            std::unique_lock<std::mutex> lock(cv_mutex_);
            cv_.wait_for(lock, std::chrono::milliseconds(interval_),
            [this]{
                return !running_ || (std::chrono::steady_clock::now() - last_start_time_ >=
                    std::chrono::milliseconds(interval_));
            });
            if (running_) {
                running_ = false; // Reset running state for the next cycle
                lock.unlock(); // Unlock before calling the callback
                if (callback_) {
                    callback_(); // Execute the callback function
                }
            } else {
                // Reset last start time to ensure the timer is not skipped when started again
                last_start_time_ = std::chrono::steady_clock::now();
            }
        }
    }

    std::atomic<bool> running_;       // 控制定时器运行状态的原子变量
    std::atomic<bool> exit_;          // 控制是否退出循环的原子变量
    std::thread workerThread_;        // 用于执行定时任务的线程
    unsigned int interval_;          // 定时器间隔时间（毫秒）
    std::function<void()> callback_; // 定时器回调函数
    std::chrono::steady_clock::time_point last_start_time_; // 上次启动的时间戳
    std::condition_variable cv_;     // 条件变量，用于线程同步
    std::mutex cv_mutex_;           // 互斥锁，用于保护条件变量
};

#endif // USB_TIMER_H