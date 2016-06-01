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

#include "LuaWrapper.h"
#include "gtest/gtest.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

using namespace boully;

class LuaWrapperTest : public ::testing::Test {
public:
    LuaWrapper *lw;
    rapidjson::Document d;
    
    void SetUp() {
        const char* json_str = "{\"project\": {\"name\": \"rapidjson\"},\"stars\":10}";
        d.Parse<0>(json_str);
        lw = new LuaWrapper();
        lw->init("luawrapper.lua", &d);        
    }

    void TearDown() {
        delete lw;
    }
};

TEST_F(LuaWrapperTest, GetService) {
    std::string provider;
    EXPECT_EQ(0, lw->GetService(provider, "dispatcher1"));
    EXPECT_EQ("bn-as1", provider);
    EXPECT_EQ(0, lw->GetService(provider, "dispatcher2", 1111));
    EXPECT_EQ("bn-as-keywords", provider);
}
