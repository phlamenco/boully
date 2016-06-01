/*
 * Copyright (c) 2016 Baidu.com, Inc. All Rights Reserved
 * @author: liaosiwei
 * @date: 2016-5-25
 */

#include "gtest/gtest.h"
#include "TokenBucket.h"
#include <ctime>

using namespace boully;

class TestTokenBucket : public ::testing::Test {
public:
    TokenBucket *tb;
    void SetUp() {
        tb = new TokenBucket(20, 200);
        tb->Run();
    }
    void TearDown() {
        delete tb;
    }
};

using namespace std;

TEST_F(TestTokenBucket, qpsControl) {
    int count = 0;
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    
    for (int i = 0; i < 1000; i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
        int flag = tb->Fetch();
        if (flag == -1)
            ;
        else {
            count++;
        }
    }
    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    
    std::cout << elapsed_seconds.count();
    cout << '\t' << count << endl;
    cout << "qps: " << count / elapsed_seconds.count() << endl;
}

TEST_F(TestTokenBucket, qpsMultiThreadControl) {
    atomic<int> count(0);
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    vector<thread> thread_vec;
    for (int i = 0; i < 8; i++)
        thread_vec.push_back(
                         thread([&count, this](){
                                 for (int i = 0; i < 1000; i++) {
                                     std::this_thread::sleep_for(std::chrono::milliseconds(2));
                                     int flag = tb->Fetch();
                                     if (flag == -1)
                                         ;
                                     else {
                                         count++;
                                     }
                                 }
                             })
                             );
    for (auto& t: thread_vec)
        t.join();
    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    
    std::cout << elapsed_seconds.count();
    cout << '\t' << count.load() << endl;
    cout << "qps: " << count.load() / elapsed_seconds.count() << endl;
}

