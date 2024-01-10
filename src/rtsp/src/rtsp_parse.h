/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: rtsp parsing data packet class
 * Author: dingkang
 * Create: 2022-01-24
 */
#ifndef LIBCASTENGINE_RTSP_PARSE_H
#define LIBCASTENGINE_RTSP_PARSE_H

#include <unordered_map>
#include <string>

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
namespace CastSessionRtsp {
class RtspParse {
public:
    RtspParse() {}
    ~RtspParse() {}

    std::unordered_map<std::string, std::string> GetHeader()
    {
        return headers_;
    }

    std::string GetUnMatchedStr()
    {
        return unmatchedString_;
    }

    std::string GetFirstLine()
    {
        return firstLine_;
    }

    int GetStatusCode()
    {
        return statusCode_;
    }

    int GetSeq();

    static void ParseMsg(const std::string &str, RtspParse &msg);
    static int ParseIntSafe(const std::string &str);
    static uint32_t ParseUint32Safe(const std::string &str);
    static double ParseDoubleSafe(const std::string &str);
    static std::string GetTargetStr(const std::string &srcStr, const std::string &specificStr,
        const std::string &endStr);

private:
    std::string unmatchedString_;
    std::string firstLine_;
    std::unordered_map<std::string, std::string> headers_;
    int statusCode_{ 0 };
    int sequence_{ 0 };
};
} // namespace CastSessionRtsp
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS
#endif // LIBCASTENGINE_RTSP_PARSE_H