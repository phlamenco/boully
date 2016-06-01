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

#include "lua.hpp"
#include <type_traits>
#include <utility>
#include <string>

namespace boully {
    // for int
    template <typename T>
        typename std::enable_if<std::is_same<T, int>::value, void>::type
        lua_push(lua_State *L, T value)
    {
        lua_pushinteger(L, value);
    }
    template <typename T>
        typename std::enable_if<std::is_same<T, int>::value, int>::type
        lua_get(lua_State *L, T& value)
    {
        int isnum = 0;
        value = lua_tointegerx(L, -1, &isnum);
        return isnum != 1 ? -1 : 0;
    }

    // for double
    template <typename T>
        typename std::enable_if<std::is_same<T, double>::value, void>::type
        lua_push(lua_State *L, T value)
    {
        lua_pushnumber(L, value);
    }
    template <typename T>
        typename std::enable_if<std::is_same<T, double>::value, int>::type
        lua_get(lua_State *L, T& value)
    {
        int isnum = 0;
        value = lua_tonumberx(L, -1, &isnum);
        return isnum != 1 ? -1 : 0;
    }

    // for char * and string
    template <typename T>
        typename std::enable_if<std::is_same<T, const char*>::value, void>::type
        lua_push(lua_State *L, T value)
    {
        lua_pushstring(L, value);
    }
    template <typename T>
        typename std::enable_if<std::is_same<T, std::string>::value, void>::type
        lua_push(lua_State *L, T value)
    {
        lua_pushstring(L, value.c_str());
    }
    template <typename T>
        typename std::enable_if<std::is_same<T, std::string>::value, int>::type
        lua_get(lua_State *L, T& value)
    {
        size_t len = 0;
        const char *res = lua_tolstring(L, -1, &len);
        if (res == NULL) {
            return -1;
        } else {
            if (len == 0)
                value = std::string();
            else
                value = std::string(res, len);
            return 0;
        }
    }

    // recursively push value
    // base function

    void lua_push_recursive(lua_State *L);

    template <typename T>
        void lua_push_recursive(lua_State *L, T value)
    {
        lua_push<T>(L, value);
    }
    
    template <typename T, typename... Args>
        void lua_push_recursive(lua_State *L, T value, Args... args)
    {
        lua_push<T>(L, value);
        lua_push_recursive(L, args...);
    }

    template <typename T, typename... Args>
        int CallLuaFunc(lua_State *L, T& res, const char* func, Args... args)
    {
        lua_getglobal(L, func);  /* push function */
        const int args_num = sizeof...(args);
        if (args_num > 0) {
            lua_push_recursive(L, std::forward<Args>(args)...);
        }        
        if (lua_pcall(L, args_num, 1, 0) != 0) {
            fprintf(stderr, "%s with args_num: %d\n", lua_tostring(L, -1), args_num);
            lua_pop(L, 1);  /* pop error message from the stack */
            return -1;
        }
        if (lua_get<T>(L, res) != 0) {
            lua_pop(L, 1);
            return -1;
        }
        lua_pop(L, 1);
        return 0;
    }
}

