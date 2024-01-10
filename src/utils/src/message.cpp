/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: message definition
 * Author: lijianzhao
 * Create: 2022-01-19
 */

#include "message.h"
#include <chrono>

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
Message::Message() : Message(0, 0, 0, 0) {}

Message::Message(int what) : Message(what, 0, 0, 0) {}

Message::Message(int what, std::string strArg) : Message(what, 0, 0, 0, strArg) {}

Message::Message(int what, int arg1) : Message(what, arg1, 0, 0) {}

Message::Message(int what, int arg1, int arg2) : Message(what, arg1, arg2, 0) {}

Message::Message(int what, int arg1, std::string strArg) : Message(what, arg1, 0, 0, strArg) {}

Message::Message(int what, int arg1, int arg2, long uptimeMillis) : Message(what, arg1, arg2, uptimeMillis, "") {}

Message::Message(int what, int arg1, int arg2, long uptimeMillis, std::string strArg)
    : what_(what), arg1_(arg1), arg2_(arg2), strArg_(strArg)
{
    when_ = std::chrono::system_clock::now() + std::chrono::milliseconds(uptimeMillis);
    task_ = nullptr;
    ptrArg_ = -1;
}

void Message::SetWhen(long uptimeMillis)
{
    when_ = std::chrono::system_clock::now() + std::chrono::milliseconds(uptimeMillis);
}

void Message::SetFunction(Function func)
{
    this->task_ = func;
}

void Message::SetPtrArg(intptr_t arg)
{
    ptrArg_ = arg;
}

void Message::SetStrArg(std::string strArg)
{
    this->strArg_ = strArg;
}

Message &Message::operator=(const Message &msg)
{
    this->arg1_ = msg.arg1_;
    this->arg2_ = msg.arg2_;
    this->what_ = msg.what_;
    this->when_ = msg.when_;
    this->task_ = msg.task_;
    this->ptrArg_ = msg.ptrArg_;
    this->strArg_ = msg.strArg_;
    this->eventCode_ = msg.eventCode_;
    return *this;
}

Message::Message(const Message &msg)
{
    this->arg1_ = msg.arg1_;
    this->arg2_ = msg.arg2_;
    this->what_ = msg.what_;
    this->when_ = msg.when_;
    this->task_ = msg.task_;
    this->ptrArg_ = msg.ptrArg_;
    this->strArg_ = msg.strArg_;
    this->eventCode_ = msg.eventCode_;
}
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS