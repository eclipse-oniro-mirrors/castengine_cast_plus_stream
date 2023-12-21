/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Description: link type of channel and the calling module type
 * Author: sunhong
 * Create: 2022-01-19
 */

#ifndef CHANNEL_INFO_H
#define CHANNEL_INFO_H

#include <array>

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
enum class ChannelLinkType {
    SOFT_BUS,
    TCP,
    VTP
};

enum class ModuleType {
    AUTH,
    RTSP,
    RTCP,
    VIDEO,
    AUDIO,
    REMOTE_CONTROL,
    STREAM,
    MODULE_TYPE_MAX
};

const std::array<std::string, static_cast<size_t>(ModuleType::MODULE_TYPE_MAX)> MODULE_TYPE_STRING = {
    "AUTH", "RTSP", "RTCP", "VIDEO", "AUDIO", "REMOTE_CONTROL", "STREAM",
};
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS

#endif // CHANNEL_INFO_H