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

#include <cassert>
#include "JsonUtil.h"
#include "LuaUtil.h"

namespace boully {
    class LuaWrapper;
    typedef int (LuaWrapper::*mem_func)(lua_State * L);
    template <mem_func func>
    int adapter(lua_State * L) {
        LuaWrapper * ptr = *static_cast<LuaWrapper**>(lua_getextraspace(L));
        return ((*ptr).*func)(L);
    }

    class LuaWrapper {
    public:
        LuaWrapper(): doc(nullptr) {
            L_ = luaL_newstate();
            luaL_openlibs(L_);
            *static_cast<LuaWrapper**>(lua_getextraspace(L_)) = this;
        }
        
        ~LuaWrapper() {
            lua_close(L_);
        }

        // a rapidjson document point is passed in, and this class is not
        // responsible for deleting this point
        int init(const char* lua_file, rapidjson::Document *d) {
            doc = d;
            int error = luaL_dofile(L_, lua_file);
            if (error) {
                fprintf(stderr, "%s\n", lua_tostring(L_, -1));
                lua_pop(L_, 1);  /* pop error message from the stack */
                exit(-1);
            }
            
            const luaL_Reg regs[] = {
                { "get_value_str", &adapter<&LuaWrapper::_lua_get_value_str> },
                { "get_value_int", &adapter<&LuaWrapper::_lua_get_value_int> },
                { NULL, NULL }
            };
            //lua_newtable(L_); since the lua is quite simple, so remove table definition
            lua_pushglobaltable(L_); 
            luaL_setfuncs(L_, regs, 0);
            return 0;
        }

        template <typename T, typename... Args>
        int GetService(T& res, const char* func, Args... args) {
            assert(doc != nullptr);
            // the lua file must have a dispatcher function
            return boully::CallLuaFunc(L_,
                                         res,
                                         func,
                                         std::forward<Args>(args)...);
        }
    private:
        int _lua_get_value_str(lua_State * L) {
            const char *key = luaL_checkstring(L, 1);
            std::string str;
            int err = boully::get_value(*doc, key, str);
            if (err != 0) {
                lua_pushnil(L);
            } else {
                boully::lua_push(L, str.c_str());
            }
            return 1;
        }
        int _lua_get_value_int(lua_State * L) {
            const char *key = luaL_checkstring(L, 1);
            int val;
            int err = boully::get_value(*doc, key, val);
            if (err != 0) {
                lua_pushnil(L);
            } else {
                boully::lua_push(L, val);
            }
            return 1;
        }
        rapidjson::Document *doc;
        lua_State *L_;
    };
}

