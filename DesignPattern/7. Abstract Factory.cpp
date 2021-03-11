#include <iostream>
#include <string>
#include <memory>

// UserType and UserRuler is interdependent...
class User
{
public:
    virtual void UserDoing() const = 0;
    virtual ~User() = default;
};

class GoldenUser : public User
{
public:
    void UserDoing() const override { std::cout<<"Golden UserDoing..."<<std::endl; }
    ~GoldenUser() {}
};

class SilverUser : public User
{
public:
    void UserDoing() const override { std::cout<<"Silver UserDoing..."<<std::endl; }
    ~SilverUser() {}
};




class UserRuler
{
public:
    virtual void discount() const = 0;
    virtual void priority() const = 0;
    virtual ~UserRuler() = default;
};


class GoldenUserRuler : public UserRuler
{
public:
    void discount() const override { std::cout<<"GoldenUser discount..."<<std::endl; }
    void priority() const override { std::cout<<"GoldenUser priority..."<<std::endl; }
    ~GoldenUserRuler() {}
};


class SilverUserRuler : public UserRuler
{
public:
    void discount() const override { std::cout<<"SilverUser discount..."<<std::endl; }
    void priority() const override { std::cout<<"SilverUser priority..."<<std::endl; }
    ~SilverUserRuler() {}
};


class UserAndRulerFactory
{
public:
    virtual std::unique_ptr<User> CreateUser() const = 0;
    virtual std::unique_ptr<UserRuler> CreateUserRuler() const = 0;
    virtual ~UserAndRulerFactory() = default;
};


class GoldenUserAndRulerFactory : public UserAndRulerFactory
{
public:
    std::unique_ptr<User> CreateUser() const override
    {
        return std::make_unique<GoldenUser>();
    }

    std::unique_ptr<UserRuler> CreateUserRuler() const override
    {
        return std::make_unique<GoldenUserRuler>();
    }
    ~GoldenUserAndRulerFactory() {}
};



class SilverUserAndRulerFactory : public UserAndRulerFactory
{
public:
    std::unique_ptr<User> CreateUser() const override
    {
        return std::make_unique<SilverUser>();
    }

    std::unique_ptr<UserRuler> CreateUserRuler() const override
    {
        return std::make_unique<SilverUserRuler>();
    }
};


class Calculation
{
public:
    explicit Calculation(std::unique_ptr<UserAndRulerFactory> _factory) : factory(std::move(_factory)) {}
    ~Calculation() {}
    void calculation() const
    {
        std::unique_ptr<User> user = factory->CreateUser();
        std::unique_ptr<UserRuler> userRuler = factory->CreateUserRuler();
        user->UserDoing();
        userRuler->discount();
        userRuler->priority();
    }
private:
    std::unique_ptr<UserAndRulerFactory> factory;
};





int main()
{
    std::unique_ptr<UserAndRulerFactory> goldenFactory = std::make_unique<GoldenUserAndRulerFactory>();
    std::unique_ptr<UserAndRulerFactory> silverFactory = std::make_unique<SilverUserAndRulerFactory>();

    Calculation goldenCal(std::move(goldenFactory));
    Calculation silverCal(std::move(silverFactory));

    goldenCal.calculation();
    std::cout<<"------------------------------------------------\n";
    silverCal.calculation();
    return 0;
}