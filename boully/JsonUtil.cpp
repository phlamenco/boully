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

#include <vector>
#include <string>
#include "JsonUtil.h"

namespace boully {

// key is the format of "key1.key2.key3"
int set_value_by_array(rapidjson::Document& d, const string& key, const string value) {
    using namespace rapidjson;
    
    const int max_key_len = 256;
    const char sep_key_char = '.';
    const char * end = key.c_str();
    const char * start = key.c_str();
    char temp_key[max_key_len] = {0};
    Value* node = 0;
    int flag = 1;
    int key_len = 0;
    Value val(value.c_str(), d.GetAllocator());

    while (1) {
        if ((*end == sep_key_char) || (*end == '\0')) {
            key_len = end - start;
            if ((key_len == 0) || (key_len >= (int)sizeof(temp_key))) {
                return -1;
            }
            memcpy(temp_key, start, key_len);
            temp_key[key_len] = '\0';
            start = end + 1;
            // the last key
            if (*end == '\0') {
                if (flag == 1) {
                    // if this is the key1
                    flag = 0;
                    if (d.HasMember(temp_key)) {
                        node = &d[temp_key];
                        node->PushBack(val, d.GetAllocator());
                    } else {
                        Value temp_value(kArrayType);
                        temp_value.PushBack(val, d.GetAllocator());
                        Value key_wrapper(temp_key, d.GetAllocator());
                        d.AddMember(key_wrapper, temp_value, d.GetAllocator());
                    }
                } else {
                    if (node->HasMember(temp_key)) {
                        node = &((*node)[temp_key]);
                        node->PushBack(val, d.GetAllocator());
                    } else {
                        Value temp_value(kArrayType);
                        temp_value.PushBack(val, d.GetAllocator());
                        Value key_wrapper(temp_key, d.GetAllocator());
                        node->AddMember(key_wrapper, temp_value, d.GetAllocator());
                    }
                }
                return 0;
            } else {            // not the last key
                if (flag == 1) {
                    // if this is the key1
                    flag = 0;
                    if (d.HasMember(temp_key)) {
                        node = &d[temp_key];
                    } else {
                        Value key_wrapper(temp_key, d.GetAllocator());
                        Value null_value;
                        d.AddMember(key_wrapper, null_value, d.GetAllocator());
                        node = &d[temp_key];
                        node->SetObject();
                    }
                } else {
                    if (node->HasMember(temp_key)) {
                        node = &((*node)[temp_key]);
                    } else {
                        Value key_wrapper(temp_key, d.GetAllocator());
                        Value null_value;
                        node->AddMember(key_wrapper, null_value, d.GetAllocator());
                        node = &((*node)[temp_key]);
                        node->SetObject();
                    }
                }
            }
        }
        ++end;
    }
    return -1;
}

    /*
     * remove object or value from a json struct
     * @return -2: wrong key, -1: remove error, 0 normal
     */
int remove(rapidjson::Document&d, const string&key) {
    using namespace rapidjson;
    using namespace boost;
    using namespace std;
    
    Value* node = &d;
    vector<string> node_vec;
    split(node_vec, key, is_any_of("."));

    for (int i = 0; i < node_vec.size(); i++) {
        auto itr = node->FindMember(node_vec[i].c_str());
        if (i == node_vec.size() - 1) {
            if (itr != 0) {
                return node->RemoveMember(node_vec[i].c_str()) ? 0: -1;
            } else {
                return -2;
            }
        } else {
            if (itr != 0) {
                if (itr->value.IsObject()) {
                    node = &(itr->value);
                } 
            } else {
                return -2;
            }
        }
    }
    return 0;
}

} // namespace boully
