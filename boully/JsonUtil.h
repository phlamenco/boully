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

#include <type_traits>
#include <string>
#include <vector>
#include "rapidjson/document.h"
#include "string_split.h"

namespace boully {
// for int
template <typename T>
typename std::enable_if<std::is_same<T, int>::value, void>::type
    _set_value(rapidjson::Value* node, T value, rapidjson::Document& d)
{
    node->SetInt(value);
}

template <typename T>
typename std::enable_if<std::is_same<T, int>::value, void>::type
    _get_value(rapidjson::Value* node, T& value)
{
    value = node->GetInt();
}
 
// for string
template <typename T>
typename std::enable_if<std::is_same<T, std::string>::value, void>::type
    _set_value(rapidjson::Value* node, T value, rapidjson::Document& d)
{
    node->SetString(value.c_str(), value.length(), d.GetAllocator());
}

template <typename T>
typename std::enable_if<std::is_same<T, std::string>::value, void>::type
    _get_value(rapidjson::Value* node, T& value)
{
    value = std::string(node->GetString());
}
 
// for char *
template <typename T>
typename std::enable_if<std::is_same<T, const char *>::value, void>::type
    _set_value(rapidjson::Value* node, T value, rapidjson::Document& d)
{
    node->SetString(value, d.GetAllocator());
}

template <typename T>
typename std::enable_if<std::is_same<T, char *>::value, void>::type
    _get_value(rapidjson::Value* node, T& value)
{
    value = node->GetString();
}

template <typename T>
int get_value(rapidjson::Document& d, const std::string& key, T& value)
{
    using namespace std;
    using namespace rapidjson;

    vector<string> node_vec;
    Value *node = static_cast<Value *>(&d);

    node_vec = __inner_split(key, '.');
    for (int i = 0; i < node_vec.size(); i++) {
        auto itr = node->FindMember(node_vec[i].c_str());
        if (i == node_vec.size() - 1) {
            if (itr != 0) {
                Value* temp = &(itr->value);
                if (temp->IsObject())
                    return -1;
                boully::_get_value<T>(temp, value);
                return 0;
            } else {
                return -1;
            }
        } else {
            if (itr != 0) {
                node = &(itr->value);
                if (!node->IsObject()) {
                    return -1;
                }
            }
            else {
                return -1;
            }
        }
    }
    return -1;
}

template <typename T>
int set_value(rapidjson::Document& d, const std::string& key, T value)
{
    using namespace rapidjson;
    using namespace std;
    
    Value* node = &d;
    vector<string> node_vec;

    node_vec = __inner_split(key, '.');
    for (int i = 0; i < node_vec.size(); i++) {
        auto itr = node->FindMember(node_vec[i].c_str());
        if (i == node_vec.size() - 1) {
            if (itr != 0) {
                boully::_set_value<T>(&(itr->value), value, d);
            } else {
                if (!node->IsObject()) {
                    node->SetObject();
                }
                Value a;
                node->AddMember(node_vec[i].c_str(), d.GetAllocator(), a, d.GetAllocator());
                boully::_set_value<T>(&((*node)[node_vec[i].c_str()]), value, d);
            }

        } else {
            if (itr != 0) {
                if (itr->value.IsObject()) {
                    node = &(itr->value);
                } else {
                    return -1;
                }
            } else {
                Value key_wrapper(node_vec[i].c_str(), d.GetAllocator());
                Value null_value;
                node->AddMember(key_wrapper, null_value, d.GetAllocator());
                node = &((*node)[node_vec[i].c_str()]);
                node->SetObject();
            }
        }
    }
    return 0;
}

int set_value_by_array(rapidjson::Document& d, const std::string& key, const std::string value);
int remove(rapidjson::Document&, const std::string&);
 
}
