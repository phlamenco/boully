/*
 * Copyright 2016 Baidu, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <atomic>
#include <chrono>
#include <thread>

//https://en.wikipedia.org/wiki/Token_bucket

namespace boully {

class TokenBucket {
 public:
    // constructor is not thead-safe
    explicit TokenBucket(int capacity, int qps) :
        capacity_(capacity),
        tokens_((long)capacity),
        qps_(qps),
        stop(0) {
        interval = 1000000 / qps;
    }
    // non-copyable and non-assignable
    TokenBucket(const TokenBucket&) = delete;
    TokenBucket& operator=(const TokenBucket&) = delete;

    ~TokenBucket() {
        stop++;
        t.join();
    }
    // thread-safe,
    int Fetch() {
        // possibly underflow
        return (tokens_-- <= 0) ? -1 : 0;
    }
    void Run() {
        t = std::thread([this]() {
                while (stop.load() != 1) {
                    int currNum = tokens_.load();
                    if (currNum <= 0)
                        tokens_.store(1);
                    else {
                        if ((currNum + 1) < capacity_)
                            tokens_++;
                        else
                            tokens_.store(capacity_);
                    }
                    std::this_thread::sleep_for(std::chrono::microseconds(interval));
                }
            });
    }
    
 private:
    int qps_;
    int capacity_;
    int interval;
    std::atomic<long> tokens_;
    std::atomic<int> stop;
    std::thread t;
};

}
