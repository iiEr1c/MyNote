#include <iostream>
#include <string>
#include <memory>

// distinguish subject(Beverage) and extention(Mocha and Milk are decorated by BeverageDecorator)
class Beverage
{
public:
    virtual std::string getDescription() const = 0;
    virtual double cost() const = 0;
    virtual ~Beverage() = default;
};

class Espresso : public Beverage
{
public:
    std::string getDescription() const override { return "Espresso"; }
    double cost() const override { return 1.99; }
    ~Espresso() {}
};







class BeverageDecorator : public Beverage
{
public:
    explicit BeverageDecorator(std::unique_ptr<Beverage> _beverage) : beverage(std::move(_beverage)) {}
    virtual ~BeverageDecorator() = default;
protected:
    std::unique_ptr<Beverage> beverage;
};

class Mocha : public BeverageDecorator
{
public:
    explicit Mocha(std::unique_ptr<Beverage> _beverage) : BeverageDecorator(std::move(_beverage)) {}
    std::string getDescription() const { return beverage->getDescription() + ", Mocha"; }
    double cost() const override { return 0.50 + beverage->cost(); }
    ~Mocha() {}
};

class Milk : public BeverageDecorator
{
public:
    explicit Milk(std::unique_ptr<Beverage> _beverage) : BeverageDecorator(std::move(_beverage)) {}
    std::string getDescription() const { return beverage->getDescription() + ", Milk"; }
    double cost() const override { return 1.55 + beverage->cost(); }
    ~Milk() {}
};







int main()
{
    auto esp = std::make_unique<Espresso>();
    std::cout<<esp->getDescription()<<": $"<<esp->cost()<<std::endl;

    auto mocha = std::make_unique<Mocha>(std::move(esp));
    std::cout<<mocha->getDescription()<<": $"<<mocha->cost()<<std::endl;

    auto mocha_milk = std::make_unique<Milk>(std::move(mocha));
    std::cout<<mocha_milk->getDescription()<<": $"<<mocha_milk->cost()<<std::endl;
    return 0;
}