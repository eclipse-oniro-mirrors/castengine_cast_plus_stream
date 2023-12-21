/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Description: cast stream manager interface.
 * Author: huangchanggui
 * Create: 2023-02-01
 */

#ifndef I_CAST_STREAM_MANAGER_H
#define I_CAST_STREAM_MANAGER_H

#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include "json.hpp"
#include "channel.h"
#include "channel_listener.h"
#include "cast_stream_common.h"
#include "i_cast_local_file_channel.h"
#include "i_cast_stream_listener.h"
#include "i_stream_player_ipc.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
using nlohmann::json;

class ICastStreamManager {
public:
    ICastStreamManager();
    virtual ~ICastStreamManager();

    virtual sptr<IStreamPlayerIpc> CreateStreamPlayer(const std::function<void(void)> &releaseCallback) = 0;
    void ProcessActionsEvent(int event, const std::string &param);
    std::shared_ptr<IChannelListener> GetChannelListener();
    void AddChannel(std::shared_ptr<Channel> channel);
    void RemoveChannel(std::shared_ptr<Channel> channel);
    std::string GetStreamPlayerCapability();
    std::string HandleCustomNegotiationParams(const std::string &playerParams);

    static constexpr int MODULE_EVENT_ID_CONTROL_EVENT = 100;
    static constexpr int MODULE_EVENT_ID_CALLBACK_EVENT = 101;
    static constexpr int MODULE_EVENT_ID_CHANNEL_CREATE = 102;
    static constexpr int MODULE_EVENT_ID_CHANNEL_DESTROY = 103;
    static constexpr int MODULE_EVENT_ID_STREAM_CHANNEL = 104;

protected:
    const std::string KEY_ACTION = "ACTION";
    const std::string KEY_CALLBACK_ACTION = "CALLBACK_ACTION";
    const std::string KEY_DATA = "DATA";
    const std::string KEY_CURRENT_INDEX = "CURRENT_INDEX";
    const std::string KEY_PROGRESS_INTERVAL = "PROGRESS_INTERVAL";
    const std::string KEY_LIST = "LIST";
    const std::string KEY_MEDIA_ID = "MEDIA_ID";
    const std::string KEY_MEDIA_NAME = "MEDIA_NAME";
    const std::string KEY_MEDIA_URL = "MEDIA_URL";
    const std::string KEY_MEDIA_TYPE = "MEDIA_TYPE";
    const std::string KEY_MEDIA_SIZE = "MEDIA_SIZE";
    const std::string KEY_START_POSITION = "START_POSITION";
    const std::string KEY_DURATION = "DURATION";
    const std::string KEY_CLOSING_CREDITS_POSITION = "CLOSING_CREDITS_POSITION";
    const std::string KEY_ALBUM_COVER_URL = "ALBUM_COVER_URL";
    const std::string KEY_ALBUM_TITLE = "ALBUM_TITLE";
    const std::string KEY_MEDIA_ARTIST = "MEDIA_ARTIST";
    const std::string KEY_LRC_URL = "LRC_URL";
    const std::string KEY_LRC_CONTENT = "LRC_CONTENT";
    const std::string KEY_APP_ICON_URL = "APP_ICON_URL";
    const std::string KEY_APP_NAME = "APP_NAME";
    const std::string KEY_VOLUME = "VOLUME";
    const std::string KEY_MUTE = "MUTE";
    const std::string KEY_MAX_VOLUME = "MAX_VOLUME";
    const std::string KEY_MODE = "MODE";
    const std::string KEY_DELTA = "DELTA";
    const std::string KEY_SPEED = "SPEED";
    const std::string KEY_POSITION = "POSITION";
    const std::string KEY_BUFFER_POSITION = "BUFFER_POSITION";
    const std::string KEY_PLAY_INFO = "PLAY_INFO";
    const std::string KEY_ERROR_CODE = "ERROR_CODE";
    const std::string KEY_ERROR_MSG = "ERROR_MSG";
    const std::string KEY_PLAY_BACK_STATE = "PLAYBACK_STATE";
    const std::string KEY_IS_PLAY_WHEN_READY = "IS_PLAY_WHEN_READY";
    const std::string KEY_IS_LOOPING = "IS_LOOPING";
    const std::string KEY_PARAMS_STREAM_VOLUME = "MEDIA_VOLUME";
    const std::string KEY_PARAMS_PLAYER_VERSION_CODE = "PLAYER_VERSION_CODE";
    const std::string KEY_CAPABILITY_SUPPORT_4K = "SUPPORT_4K";
    const std::string KEY_CAPABILITY_SUPPORT_DRM = "DRM_CAPABILITY";
    const std::string KEY_CAPABILITY_DRM_PROPERTIES = "DRM_PROPERTIES_CAPABILITY";

    const std::string ACTION_PLAY = "play";
    const std::string ACTION_LOAD = "load";
    const std::string ACTION_PAUSE = "pause";
    const std::string ACTION_RESUME = "resume";
    const std::string ACTION_STOP = "stop";
    const std::string ACTION_NEXT = "next";
    const std::string ACTION_PREVIOUS = "previous";
    const std::string ACTION_SEEK = "seek";
    const std::string ACTION_FAST_FORWARD = "fastForward";
    const std::string ACTION_FAST_REWIND = "fastRewind";
    const std::string ACTION_SET_VOLUME = "setVolume";
    const std::string ACTION_SET_REPEAT_MODE = "setRepeatMode";
    const std::string ACTION_SET_SPEED = "setSpeed";
    const std::string ACTION_PLAYER_STATUS_CHANGED = "onPlayerStatusChanged";
    const std::string ACTION_POSITION_CHANGED = "onPositionChanged";
    const std::string ACTION_MEDIA_ITEM_CHANGED = "onMediaItemChanged";
    const std::string ACTION_VOLUME_CHANGED = "onVolumeChanged";
    const std::string ACTION_REPEAT_MODE_CHANGED = "onRepeatModeChanged";
    const std::string ACTION_SPEED_CHANGED = "onPlaySpeedChanged";
    const std::string ACTION_PLAYER_ERROR = "onPlayerError";
    const std::string ACTION_NEXT_REQUEST = "onNextRequest";
    const std::string ACTION_PREVIOUS_REQUEST = "onPreviousRequest";
    const std::string ACTION_SEEK_DONE = "onSeekDone";
    const std::string ACTION_END_OF_STREAM = "onEndOfStream";
    const std::string ACTION_PLAY_REQUEST = "onPlayRequest";

    void Handle();
    bool SendControlAction(const std::string &action, const json &dataBody = {});
    bool SendCallbackAction(const std::string &action, const json &dataBody = {});
    bool ParseMediaInfo(const json &data, MediaInfo &MediaInfo);
    void EncapMediaInfo(const MediaInfo &mediaInfo, json &data);

    using StreamActionProcessor = std::function<bool(const json &data)>;
    std::map<std::string, StreamActionProcessor> streamActionProcessor_ {};
    std::queue<std::pair<json, StreamActionProcessor>> workQueue_;
    std::thread handleThread_;
    std::mutex queueMutex_;
    std::condition_variable condition_;
    std::atomic<bool> isRunning_{ false };

    std::shared_ptr<ICastLocalFileChannel> localFileChannel_;
    std::shared_ptr<ICastStreamListener> streamListener_;
    sptr<IStreamPlayerListenerImpl> playerListener_;
    sptr<IStreamPlayerIpc> playerIpc_;

    std::mutex dataMutex_;
    std::mutex listenerMutex_;
    int currentVolume_{ CAST_STREAM_INT_INVALID };
    int maxVolume_{ CAST_STREAM_INT_INVALID };
};
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS
#endif // CAST_STREAM_PLAYER_H