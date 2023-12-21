/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Description: supply cast stream player manager.
 * Author: zhangjingnan
 * Create: 2023-1-11
 */

#ifndef CAST_STREAM_PLAYER_MANAGER_H
#define CAST_STREAM_PLAYER_MANAGER_H

#include "cast_stream_player.h"
#include "i_stream_player_impl.h"
#include "i_stream_player_listener_impl.h"
#include "i_cast_stream_manager_server.h"
#include "cast_local_file_channel_client.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
class CastStreamPlayerManager : public IStreamPlayerImpl,
    public std::enable_shared_from_this<CastStreamPlayerManager> {
public:
    CastStreamPlayerManager(std::shared_ptr<ICastStreamManagerServer> callback,
        std::shared_ptr<CastLocalFileChannelClient> fileChannel);
    ~CastStreamPlayerManager() override;

    void SetSessionCallbackForRelease(const std::function<void(void)>& callback);
    int32_t RegisterListener(sptr<IStreamPlayerListenerImpl> listener) override;
    int32_t UnregisterListener() override;
    int32_t SetSurface(sptr<IBufferProducer> producer) override;
    int32_t Load(const MediaInfo &mediaInfo) override;
    int32_t Play(const MediaInfo &mediaInfo) override;
    int32_t InnerPlay(const MediaInfo &mediaInfo);
    int32_t Play(int index) override;
    int32_t Play() override;
    int32_t Pause() override;
    int32_t Stop() override;
    int32_t Next() override;
    int32_t Previous() override;
    int32_t Seek(int position) override;
    int32_t FastForward(int delta) override;
    int32_t FastRewind(int delta) override;
    int32_t SetVolume(int volume) override;
    int32_t SetLoopMode(const LoopMode mode) override;
    int32_t SetSpeed(const PlaybackSpeed speed) override;
    int32_t GetPlayerStatus(PlayerStates &playerStates) override;
    int32_t GetPosition(int &position) override;
    int32_t GetDuration(int &duration) override;
    int32_t GetVolume(int &volume, int &maxVolume) override;
    int32_t GetLoopMode(LoopMode &loopMode) override;
    int32_t GetPlaySpeed(PlaybackSpeed &playbackSpeed) override;
    int32_t GetMediaInfoHolder(MediaInfoHolder &mediaInfoHolder) override;
    int32_t Release() override;

private:
    bool InnerPlayLocked(bool isLoading);
    bool StopLocked();
    PlaybackSpeed ConvertMediaSpeedToPlaybackSpeed(Media::PlaybackRateMode speedMode);
    std::function<void(void)> sessionCallback_;

    std::mutex mutex_;
    std::mutex sessionCallbackMutex_;
    std::shared_ptr<CastStreamPlayer> player_;
    std::shared_ptr<CastStreamPlayerCallback> callback_;
    MediaInfo mediaInfo_{};
    std::atomic<bool> isReceiveLoadCommand_{ false };
    std::atomic<bool> isReceivePlayCommand_{ false };
    std::atomic<bool> isReady_{ false };
    sptr<Surface> surface_;
};
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS
#endif // CAST_STREAM_PLAYER_MANAGER_H