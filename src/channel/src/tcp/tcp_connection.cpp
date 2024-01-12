/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * Description: tcp connection.
 * Author: renshuang
 * Create: 2023-09-25
 */

#include "tcp_connection.h"

#include "cast_engine_log.h"
#include "securec.h"
#include "transport.h"
#include "utils.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
DEFINE_CAST_ENGINE_LABEL("CastEngine-TcpConnection");

TcpConnection::~TcpConnection()
{
    CLOGD("Enter.");
}

int TcpConnection::StartConnection(const ChannelRequest &request, std::shared_ptr<IChannelListener> channelListener)
{
    CLOGD("Tcp Start Connection Enter.");
    ConfigSocket();
    StashRequest(request);
    SetRequest(request);
    SetListener(channelListener);
    
    std::thread(&TcpConnection::Connect, shared_from_this()).detach();
    return RET_OK;
}

void TcpConnection::Connect()
{
    CLOGD("Tcp Connect Enter.");
    if (channelRequest_.remoteDeviceInfo.ipAddress.empty() || channelRequest_.remotePort == INVALID_PORT) {
        return;
    }
    std::shared_ptr<ConnectionListener> listener = listener_;
    if (!listener) {
        CLOGE("listener_ is nullptr.");
        return;
    }
    int port = socket_.Bind(channelRequest_.localDeviceInfo.ipAddress, channelRequest_.localPort);
    CLOGD("Start server socket, localIp:%s, bindPort:%{public}d", channelRequest_.localDeviceInfo.ipAddress.c_str(),
        port);
    bool ret = socket_.Connect(channelRequest_.remoteDeviceInfo.ipAddress, channelRequest_.remotePort);
    if (!ret) {
        CLOGE("Tcp Connect Failed.");
        listener->OnConnectionConnectFailed(channelRequest_, ret);
        return;
    }
    listener->OnConnectionOpened(shared_from_this());
    if (channelRequest_.isReceiver) {
        Receive(remoteSocket_);
    }
}

int TcpConnection::StartListen(const ChannelRequest &request, std::shared_ptr<IChannelListener> channelListener)
{
    CLOGD("Tcp Start Listen Enter.");
    ConfigSocket();
    StashRequest(request);
    SetRequest(request);
    SetListener(channelListener);
    
    int port = socket_.Bind(request.localDeviceInfo.ipAddress, request.localPort);
    CLOGD("Start server socket, localIp:%s, bindPort:%{public}d", request.localDeviceInfo.ipAddress.c_str(), port);
    socket_.Listen(SOMAXCONN);
    if (request.moduleType == ModuleType::VIDEO && request.remoteDeviceInfo.deviceType != DeviceType::DEVICE_HICAR) {
        std::thread(&TcpConnection::AcceptVideoAndAudio, shared_from_this()).detach();
    } else {
        std::thread(&TcpConnection::Accept, shared_from_this()).detach();
    }
    return port;
}

void TcpConnection::ConfigSocket()
{
    socket_.SetSendBufferSize(SOCKET_SEND_BUFFER_SIZE);
    socket_.SetRecvBufferSize(SOCKET_RECV_BUFFER_SIZE);
    socket_.SetKeepAlive();
    socket_.SetReuseAddr();
}

/*
 * Only used in media module for tcp server to accept client twice, include both video and audio.
 */
void TcpConnection::AcceptVideoAndAudio()
{
    CLOGD("Tcp AcceptVideoAndAudio Enter.");
    Accept();
    Accept();
}

void TcpConnection::Accept()
{
    CLOGD("Tcp Accept Enter.");
    std::shared_ptr<ConnectionListener> listener = listener_;
    if (!listener) {
        CLOGE("listener_ is nullptr.");
        return;
    }
    int sockfd = socket_.Accept();
    if (sockfd == INVALID_SOCKET) {
        CLOGE("Open Session Failed, sessionId = %{public}d, moduleType = %{public}d",
            channelRequest_.remoteDeviceInfo.sessionId, channelRequest_.moduleType);
        listener->OnConnectionConnectFailed(channelRequest_, sockfd);
        return;
    }
    int remotePort = socket_.GetPeerPort(sockfd);
    if (remotePort == INVALID_PORT) {
        CLOGE("Open Session Failed, sessionId = %{public}d, moduleType = %{public}d",
            channelRequest_.remoteDeviceInfo.sessionId, channelRequest_.moduleType);
        listener->OnConnectionConnectFailed(channelRequest_, remotePort);
        return;
    }
    if (channelRequest_.moduleType == ModuleType::VIDEO && remotePort != channelRequest_.remotePort &&
            channelRequest_.remoteDeviceInfo.deviceType != DeviceType::DEVICE_HICAR) {
        // audio
        std::lock_guard<std::mutex> lg(connectionMtx_);
        SetAudioConnection(sockfd);
        CLOGD("Open Session Succ, sessionId = %{public}d, moduleType = %{public}d, client = %{public}d",
            tcpAudioConn_->channelRequest_.remoteDeviceInfo.sessionId, tcpAudioConn_->channelRequest_.moduleType,
            sockfd);
        listener->OnConnectionOpened(tcpAudioConn_);
    } else {
        remoteSocket_ = sockfd;
        // rtsp, remotectrl, video
        CLOGD("Open Session Succ, sessionId = %{public}d, moduleType = %{public}d, client = %{public}d",
            channelRequest_.remoteDeviceInfo.sessionId, channelRequest_.moduleType, sockfd);
        listener->OnConnectionOpened(shared_from_this());
    }

    if (!channelRequest_.isReceiver) {
        CLOGE("Only Send Media data for Source end.");
        return;
    }
    Receive(sockfd);
    CLOGI("Tcp Accept out.");
}

void TcpConnection::SetAudioConnection(int socket)
{
    CLOGD("Tcp SetAudioConnection Enter.");
    if (!tcpAudioConn_) {
        tcpAudioConn_ = std::make_shared<TcpConnection>();
    }
    tcpAudioConn_->remoteSocket_ = socket;
    ChannelRequest audioChannelrequest = channelRequest_;
    audioChannelrequest.moduleType = ModuleType::AUDIO;
    tcpAudioConn_->StashRequest(audioChannelrequest);
    tcpAudioConn_->SetRequest(audioChannelrequest);
    tcpAudioConn_->SetConnectionListener(listener_);
    tcpAudioConn_->SetListener(GetListener());
}

void TcpConnection::Receive(int socket)
{
    CLOGD("Tcp Receive Client Enter.");
    std::thread(&TcpConnection::ReadLooper, shared_from_this(), socket).detach();
}

void TcpConnection::ReadLooper(int socket)
{
    CLOGD("Tcp Read Client Looper Enter.");
    isReceiving_ = true;
    HandleReceivedData(socket);
}

void TcpConnection::HandleReceivedData(int socket)
{
    std::shared_ptr<ConnectionListener> listener = listener_;
    if (!listener) {
        CLOGE("listener_ is nullptr.");
        return;
    }
    while (isReceiving_) {
        CLOGD("TCP Recv Data start.");
        int sockfd = socket == INVALID_SOCKET ? socket_.GetSocketFd() : socket;
        uint8_t header[PACKET_HEADER_LEN] = {};
        ssize_t length = socket_.Recv(sockfd, header, sizeof(header));
        if (length == STOP_RECEIVE) {
            break;
        }
        if (length != PACKET_HEADER_LEN) {
            CLOGE("Receive header data error.");
            listener->OnConnectionError(shared_from_this(), length);
            return;
        }
        uint32_t dataLength = GetReceivedDataLength(header);
        if (dataLength > ILLEGAL_LENGTH) {
            CLOGE("Receive payload data length is illegal.");
            listener->OnConnectionError(shared_from_this(), length);
            return;
        }
        uint8_t buf[dataLength];
        if (memset_s(buf, dataLength, 0, dataLength) != RET_OK) {
            CLOGE("memset_s failed");
            continue;
        }
        length = socket_.Recv(sockfd, buf, sizeof(buf));
        if (length == STOP_RECEIVE) {
            break;
        }
        if (length != dataLength) {
            CLOGE("Receive payload data length is illegal.");
            listener->OnConnectionError(shared_from_this(), length);
            return;
        }
        if (channelRequest_.moduleType == ModuleType::REMOTE_CONTROL) {
            HandleRemoteControlReceivedData(dataLength, header, buf);
            continue;
        }
        CLOGD("TCP recvFrameLen done, dataLength = %{public}d", dataLength);
        if (GetListener()) {
            GetListener()->OnDataReceived(buf, dataLength, 0);
        }
    }
    CLOGI("HandleReceivedData Out.");
}
 
uint32_t TcpConnection::GetReceivedDataLength(uint8_t *header)
{
    uint32_t dataLength = Utils::ByteArrayToInt(header, PACKET_HEADER_LEN);
    if (channelRequest_.moduleType == ModuleType::REMOTE_CONTROL) {
        dataLength &= CONTROL_LENGTH_MASK;
        dataLength -= PACKET_HEADER_LEN;
    }
    return dataLength;
}

void TcpConnection::HandleRemoteControlReceivedData(uint32_t dataLength, uint8_t *header, uint8_t *buf)
{
    uint8_t controlBuf[PACKET_HEADER_LEN + dataLength];
    if (memcpy_s(controlBuf, PACKET_HEADER_LEN, header, PACKET_HEADER_LEN) != RET_OK) {
        CLOGE("Copy data failed");
        return;
    }
    if (memcpy_s(controlBuf + PACKET_HEADER_LEN, dataLength, buf, dataLength) != RET_OK) {
        CLOGE("Copy data failed");
        return;
    }
    CLOGD("TCP recv remote control done, dataLength = %{public}d", PACKET_HEADER_LEN + dataLength);
    if (GetListener()) {
        GetListener()->OnDataReceived(controlBuf, PACKET_HEADER_LEN + dataLength, 0);
    }
}

void TcpConnection::CloseConnection()
{
    CLOGI("Tcp Close Enter.");
    std::lock_guard<std::mutex> lg(connectionMtx_);
    isReceiving_.store(false);
    if (tcpAudioConn_) {
        CLOGD("Close Tcp Audio Connection.");
        tcpAudioConn_->CloseConnection();
    }
    if (remoteSocket_ != INVALID_SOCKET) {
        socket_.Shutdown(remoteSocket_);
        remoteSocket_ = INVALID_SOCKET;
    }
    socket_.Close();
    if (listener_) {
        listener_->OnConnectionClosed(shared_from_this());
    }
    CLOGI("Tcp Close Out.");
}

bool TcpConnection::Send(const uint8_t *buf, int bufLen)
{
    CLOGD("Tcp Send Enter, len = %{public}d", bufLen);
    if (buf == nullptr || bufLen <= 0) {
        CLOGE("Data or length is illegal.");
        return false;
    }
    uint8_t sendBuf[bufLen + PACKET_HEADER_LEN];
    Utils::IntToByteArray(bufLen, PACKET_HEADER_LEN, sendBuf);
    errno_t cpyRet = memcpy_s(sendBuf + PACKET_HEADER_LEN, bufLen, buf, bufLen);
    if (cpyRet != RET_OK) {
        return false;
    }
    CLOGD("Tcp Send, socket = %{public}d, moduleType = %{public}d", remoteSocket_, channelRequest_.moduleType);
    int sockfd = remoteSocket_ == INVALID_SOCKET ? socket_.GetSocketFd() : remoteSocket_;
    return socket_.Send(sockfd, sendBuf, bufLen + PACKET_HEADER_LEN) > RET_OK ? true : false;
}
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS