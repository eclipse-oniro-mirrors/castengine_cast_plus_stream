/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Description: supply cast session listener implement proxy class.
 * Author: zhangge
 * Create: 2022-6-15
 */

#include "cast_session_listener_impl_proxy.h"

#include "cast_engine_common_helper.h"
#include "cast_engine_log.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
DEFINE_CAST_ENGINE_LABEL("Cast-Session-Listener");

void CastSessionListenerImplProxy::OnDeviceState(const DeviceStateInfo &stateInfo)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        CLOGE("Failed to write the interface token");
        return;
    }
    if (!WriteDeviceStateInfo(data, stateInfo)) {
        CLOGE("Failed to write the state info");
        return;
    }
    if (Remote()->SendRequest(ON_DEVICE_STATE, data, reply, option) != ERR_NONE) {
        CLOGE("Failed to send ipc request when reporting cast session event");
    }
}

void CastSessionListenerImplProxy::OnEvent(const EventId &eventId, const std::string &jsonParam)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        CLOGE("Failed to write the interface token");
        return;
    }
    if (!WriteEvent(data, eventId, jsonParam)) {
        CLOGE("Failed to write the event");
        return;
    }
    if (Remote()->SendRequest(ON_EVENT, data, reply, option) != ERR_NONE) {
        CLOGE("Failed to send ipc request when reporting cast session event");
    }
}
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS
