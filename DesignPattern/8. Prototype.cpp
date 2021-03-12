#include <iostream>
#include <memory>
#include <string>
#include <cassert>

class User
{
public:
    virtual void UserDoing() const = 0;
    virtual std::unique_ptr<User> clone() const = 0;
    virtual ~User() = default;
};



class GoldenUser : public User
{
public:
    explicit GoldenUser(std::string _name) : name(_name) {}
    GoldenUser(const GoldenUser& goldenUser) : name(goldenUser.name) {}
    GoldenUser& operator=(const GoldenUser& goldenUser)
    {
        this->name = goldenUser.name;
        return *this;
    }
    void UserDoing() const override { std::cout<<name<<std::endl; }
    void setName(std::string _name) { this->name = _name; }
    std::unique_ptr<User> clone() const override
    {
        return std::make_unique<GoldenUser>(*this);
    }
    ~GoldenUser() {}
private:
    std::string name;
};




class SilverUser : public User
{
public:
    explicit SilverUser(std::string _name) : name(_name) {}
    SilverUser(const SilverUser& silverUser) : name(silverUser.name) {}
    SilverUser& operator=(const SilverUser& silverUser)
    {
        this->name = silverUser.name;
        return *this;
    }
    void UserDoing() const override { std::cout<<name<<std::endl; }
    void setName(std::string _name) { this->name = _name; }
    std::unique_ptr<User> clone() const override
    {
        return std::make_unique<SilverUser>(*this);
    }
    ~SilverUser() {}
private:
    std::string name;
};



class Calculation
{
public:
    Calculation(std::unique_ptr<User> _prototype) : prototype(std::move(_prototype)) {}
    void UserPrototypeClone()
    {
        std::unique_ptr<User> newObject = prototype->clone();
        newObject->UserDoing();
    }
private:
    std::unique_ptr<User> prototype;
};



int main()
{
    auto goldenUser = std::make_unique<GoldenUser>("Golden User...");
    goldenUser->UserDoing();
    goldenUser->setName("down to Silver User...");
    auto newUser = goldenUser->clone();
    std::cout<<"goldenUser's UserDoing:\t";
    goldenUser->UserDoing();
    std::cout<<"newUser's UserDoing:\t";
    newUser->UserDoing();
    std::cout<<goldenUser.get()<<std::endl;
    std::cout<<newUser.get()<<std::endl;
    return 0;
}