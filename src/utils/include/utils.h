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

#ifndef UTILS_H
#define UTILS_H

#include <locale>
#include <string>
#include <vector>

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
#if !defined(DISALLOW_COPY_AND_ASSIGN)
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName &);            \
    void operator = (const TypeName &)
#endif

#if !defined(DISALLOW_EVIL_CONSTRUCTORS)
#define DISALLOW_EVIL_CONSTRUCTORS(TypeName) DISALLOW_COPY_AND_ASSIGN(TypeName)
#endif

#if !defined(DISALLOW_IMPLICIT_CONSTRUCTORS)
#define DISALLOW_IMPLICIT_CONSTRUCTORS(TypeName) \
    TypeName();                                  \
    DISALLOW_COPY_AND_ASSIGN(TypeName)
#endif

struct PacketData {
    uint8_t *data;
    int length;
};

struct ConstPacketData {
    const uint8_t *data;
    int length;
};

class Utils {
public:
    Utils() {}
    ~Utils() {}
    static void SplitString(const std::string &src, std::vector<std::string> &dest, const std::string &seperator);
    static std::string &Trim(std::string &str);
    static std::string &ToLower(std::string &str);
    static bool StartWith(const std::string &mainStr, const std::string &subStr);
    static int IntToByteArray(int num, int length, uint8_t *output);
    static uint32_t ByteArrayToInt(const uint8_t *input, unsigned int length);
    static int32_t StringToInt(const std::string &str);

    static bool Base64Encode(const std::string &source, std::string &encoded);
    static bool Base64Decode(const std::string &encoded, std::string &decoded);

private:
    static const int BYTE_TO_BIT_OFFSET = 8;
    static const uint8_t INT_TO_BYTE = 0xff;
    static const std::string BASE64_CHARS;
    static inline bool IsBase64(unsigned char c);
    static const int DECIMALISM = 10;
};

inline constexpr char SANDBOX_PATH[] = "/data/service/el1/public/cast_engine_service";
inline constexpr char PARAM_MEDIA_DUMP[] = "debug.cast.media.dump";
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS

#endif
