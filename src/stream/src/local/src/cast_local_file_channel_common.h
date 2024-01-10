/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: Cast local file channel common
 * Author: liaoxin
 * Create: 2023-4-20
 */

#ifndef CAST_LOCAL_FILE_CHANNEL_COMMON_H
#define CAST_LOCAL_FILE_CHANNEL_COMMON_H

#include <string>
#include <map>

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
const std::string HTTP_REQ_METHOD = "method";
const std::string HTTP_REQ_URI = "uri";
const std::string HTTP_REQ_RANGE_START = "range_start";
const std::string HTTP_REQ_RANGE_END = "range_end";
const std::string HTTP_RSP_PROTOCOL = "protocol_ver";
const std::string HTTP_RSP_CODE = "statusCode";
const std::string HTTP_RSP_CONTENT_LENGTH = "content_length";
const std::string HTTP_RSP_CONTENT_RANGE_START = "range_start";
const std::string HTTP_RSP_CONTENT_RANGE_END = "range_end";
const std::string HTTP_RSP_CONTENT_RANGE_TOTAL = "range_total";
const std::string HTTP_RSP_CONTENT_DISPOSITION = "disposition";

const int64_t INVALID_END_POS = -1;

int ConvertFileId(const std::string &fileId);
bool IsLocalFile(const std::string &url);
bool IsLocalUrl(const std::string &url);
bool ParseHttpRequest(const uint8_t *buffer, int length, std::map<std::string, std::string> &request);
bool ParseStringToInt64(const std::string &str, int64_t &val);
bool ParseHttpResponse(const uint8_t *buffer, int length, std::map<std::string, std::string> &response,
    size_t &dataOffset);
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS

#endif // CAST_LOCAL_FILE_CHANNEL_COMMON_H
