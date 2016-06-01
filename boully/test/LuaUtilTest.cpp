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

#include "LuaUtil.h"
#include "gtest/gtest.h"

using namespace boully;
using namespace std;

class LuaUtilTest : public ::testing::Test {
public:
    lua_State *L_;
    
    void SetUp() {
        L_ = luaL_newstate();
        luaL_openlibs(L_);

        // DON'T USE luaL_loadfile
        //int error = luaL_loadfile(L_, "test_lua.lua");
        // using loadfile seems we can't get function from lua_getglobal
        int error = luaL_dofile(L_, "test_lua.lua");
        if (error) {
            fprintf(stderr, "%s\n", lua_tostring(L_, -1));
            lua_pop(L_, 1);  /* pop error message from the stack */
        }
    }
    void TearDown() {
        lua_close(L_);

    }
};

TEST_F(LuaUtilTest, CallLuaFunc) {
    string value;
    int res;
    EXPECT_EQ(0, CallLuaFunc(L_, value, "test_str", "test_str"));
    EXPECT_EQ(string("test_str"), value);
    EXPECT_EQ(0, CallLuaFunc(L_, res, "test_int", 10));
    EXPECT_EQ(20, res);
    EXPECT_EQ(0, CallLuaFunc(L_, res, "test_int_non_args"));
    EXPECT_EQ(10, res);
}
