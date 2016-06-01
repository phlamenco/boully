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

#include "gtest/gtest.h"
#include "JsonUtil.h"
#include "prettywriter.h"
#include <iostream>

using namespace rapidjson;
using namespace boully;
using namespace std;

class JsonUtilTest : public ::testing::Test {
public:
    Document d;
    
    void SetUp() {
        const char* json_str = "{\"project\": {\"name\": \"rapidjson\"},\"stars\":\"10\"}";
        d.Parse<0>(json_str);
        output();
    }
    void TearDown() {
        output();
    }
    void output() {
        StringBuffer sb;
        PrettyWriter<StringBuffer> writer(sb);
        d.Accept(writer);    // Accept() traverses the DOM and generates Handler events.
        puts(sb.GetString());
    }
};

TEST_F(JsonUtilTest, SetValueByArray) {
    set_value_by_array(d, "item.remove", "a remove node test");
    set_value_by_array(d, "item.stage.hold", "level 3 test");
    set_value_by_array(d, "item.stage.hold", "level 3.1 test");
    set_value_by_array(d, "item.stage.remove", "level 3.2 test");
    set_value_by_array(d, "item.test.chinese", "‘⁄’‚¿Ô");
    set_value_by_array(d, "_10000", "VIP_000000239_23934238023");
    set_value_by_array(d, "item.test.empty.str", "");
    set_value_by_array(d, "item.hold", "stage");
}

TEST_F(JsonUtilTest, SetValue) {
    string a("string_value");
    EXPECT_EQ(0, set_value(d, "key1", a));
    EXPECT_EQ(0, set_value(d, "key3", "value3"));
    EXPECT_EQ(0, set_value(d, "key2", 10));
    EXPECT_EQ(0, set_value(d, "key1", "value_changed_key1"));
}

TEST_F(JsonUtilTest, GetValue) {
    char* value;
    string value1;
    EXPECT_EQ(0, get_value(d, string("project.name"), value));
    EXPECT_EQ(string("rapidjson"), string(value));
    EXPECT_EQ(0, get_value(d, "stars", value1));
    EXPECT_EQ(string("10"), value1);
    EXPECT_EQ(-1, get_value(d, "project.name.none", value1));
    EXPECT_EQ(-1, get_value(d, "project", value1));
    set_value(d, "int_test", 10);
    int res;
    EXPECT_EQ(0, get_value(d, "int_test", res));
    EXPECT_EQ(10, res);
}

TEST_F(JsonUtilTest, Remove) {
    EXPECT_EQ(0, remove(d, "stars"));
    EXPECT_EQ(-2, remove(d, "stars"));
    EXPECT_EQ(-2, remove(d, "project.name.none"));
    EXPECT_EQ(0, remove(d, "project.name"));
}

