#include <iostream>
#include <memory>

// different platform have different realization
class MessageImp
{
public:
    virtual void drawGraph() = 0;
    virtual void playSound() = 0;
    virtual ~MessageImp() = default;
};


class AndroidMessageImp : public MessageImp
{
public:
    void drawGraph() override { std::cout<<"AndroidMessageImp::drawGraph()"<<std::endl; }
    void playSound() override { std::cout<<"AndroidMessageImp::playSound()"<<std::endl; }
    ~AndroidMessageImp() {}
};

// Message application
class Message
{
public:
    explicit Message(std::unique_ptr<MessageImp> _messageImp) : messageImp(std::move(_messageImp)) {}
    virtual void login() = 0;
    virtual void sendMessage(std::string) = 0;
    virtual void sendFile(std::string) = 0;
    virtual ~Message() = default;
protected:
    std::unique_ptr<MessageImp> messageImp;
};


// the evolution of Message transaction...
class MessageLite : public Message
{
public:
    explicit MessageLite(std::unique_ptr<MessageImp> _messageImp) : Message(std::move(_messageImp)) {}
    void login() override
    {
        messageImp->playSound();
        std::cout<<"login()..."<<std::endl;
    }
    void sendMessage(std::string picPath) override
    {
        messageImp->drawGraph();
        std::cout<<"PicturePath: "<<picPath<<std::endl;
    }
    void sendFile(std::string filePath) override
    {
        messageImp->playSound();
        std::cout<<"FilePath: "<<filePath<<std::endl;
    }
};

int main()
{
    std::unique_ptr<MessageImp> messageImp = std::make_unique<AndroidMessageImp>();
    std::unique_ptr<Message> message = std::make_unique<MessageLite>(std::move(messageImp));
    message->login();
    message->sendFile("D:\\test\\text.cpp");
    message->sendMessage("D:\\test\\text.jpg");
    return 0;
}