/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Description: common method for cast session
 * Author: lijianzhao
 * Create: 2022-01-19
 */

#include "utils.h"
#include <cctype>
#include <glib.h>

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {

bool Utils::Base64Encode(const std::string &source, std::string &encoded)
{
    gchar *out = g_base64_encode(reinterpret_cast<const guchar *>(source.c_str()), source.size());
    if (!out) {
        return false;
    }
    gsize out_len = strlen(out);
    encoded = std::string(out, out_len);
    g_free(out);
    return true;
}

bool Utils::Base64Decode(const std::string &encoded, std::string &decoded)
{
    gsize out_len = 0;
    guchar *decodeData = g_base64_decode(encoded.c_str(), &out_len);
    if (!decodeData) {
        return false;
    }
    decoded = std::string(reinterpret_cast<const char *>(decodeData), out_len);
    g_free(decodeData);
    return true;
}


void Utils::SplitString(const std::string &src, std::vector<std::string> &dest, const std::string &seperator)
{
    std::string::size_type beginPos = 0;
    std::string::size_type endPos = src.find(seperator);
    while (endPos != std::string::npos) {
        dest.push_back(src.substr(beginPos, endPos - beginPos));
        beginPos = endPos + seperator.size();
        endPos = src.find(seperator, beginPos);
    }
    if (beginPos != src.length()) {
        dest.push_back(src.substr(beginPos));
    }
}

std::string &Utils::Trim(std::string &str)
{
    if (str.empty()) {
        return str;
    }
    str.erase(0, str.find_first_not_of(" "));
    str.erase(str.find_last_not_of(" ") + 1);
    return str;
}

std::string &Utils::ToLower(std::string &str)
{
    std::locale loc;
    for (std::string::size_type i = 0; i < str.length(); ++i) {
        str[i] = std::tolower(str[i], loc);
    }
    return str;
}

bool Utils::StartWith(const std::string &mainStr, const std::string &subStr)
{
    return mainStr.find(subStr) == 0;
}

int Utils::IntToByteArray(int num, int length, uint8_t *output)
{
    for (int i = 0; i < length; i++) {
        output[length - 1 - i] = static_cast<uint8_t>(
            (static_cast<unsigned int>(num) >> static_cast<unsigned int>(BYTE_TO_BIT_OFFSET * i))) &
            INT_TO_BYTE;
    }
    return length;
}

uint32_t Utils::ByteArrayToInt(const uint8_t *input, unsigned int length)
{
    uint32_t output = 0;
    if (length > BYTE_TO_BIT_OFFSET) {
        return output;
    }
    for (unsigned int i = 0; i < length; i++) {
        output |= (input[i] << (BYTE_TO_BIT_OFFSET * (length - 1 - i)));
    }
    return output;
}

int32_t Utils::StringToInt(const std::string &str)
{
    if (str.size() == 0) {
        return 0;
    }
    char *nextPtr = nullptr;
    long result = strtol(str.c_str(), &nextPtr, DECIMALISM);
    if (errno == ERANGE || *nextPtr != '\0') {
        return 0;
    }
    return static_cast<int32_t>(result);
}

} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS
