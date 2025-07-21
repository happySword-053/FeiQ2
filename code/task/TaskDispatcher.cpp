#include "TaskDispatcher.h"

TaskDispatcher::TaskDispatcher(QObject *parent)
:QObject(parent)
{}

std::list<MessageBubble *> TaskDispatcher::getFriendHistory(const QString &friendMac)
{
    std::list<MessageBubble *> messageBubbles;
    try{
        auto it = this->chatMessageDAO.getChatMessage(friendMac.toUtf8().data());
        for(auto &chatMessageDO : it){
            MessageType type = chatMessageDO.getIsSelf() ? MessageType::Outgoing : MessageType::Incoming;
            QString time = QString::fromStdString(chatMessageDO.getTimestamp());
            QString content = QString::fromStdString(chatMessageDO.getContent());
            QString sender = QString::fromStdString(chatMessageDO.getMac());
            MessageBubble *messageBubble = new MessageBubble(type, sender, time, content);
            messageBubbles.push_back(messageBubble);
            
        }
    }catch(const std::runtime_error& e){
        LOG(std::string("查询聊天消息失败,错误输出") + e.what() + ",好友mac为:" + friendMac.toUtf8().data(), ERROR);
        throw std::runtime_error("查询聊天消息失败"); 
    }
    return messageBubbles;
}

FriendInfo TaskDispatcher::getFriendInfo(const QString &friendMac)
{
    return this->friends.getFriendInfoByMac(friendMac.toUtf8().data());
}

void TaskDispatcher::broadcastGetFriendInfo()
{
}
