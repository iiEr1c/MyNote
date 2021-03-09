#include <iostream>
#include <memory>
#include <vector>

/*
 * when calculation logic is stable, but algorithm often changes(such as parameter in here)
 */

class TaxStrategy
{
public:
    virtual void calculate() = 0;
    virtual ~TaxStrategy() = default;
};

class CNTax : public TaxStrategy
{
public:
    void calculate() override { std::cout<<"tax == CN_Tax\n"; }
    ~CNTax() {}
};

class USTax : public TaxStrategy
{
public:
    void calculate() override { std::cout<<"tax == US_Tax\n"; }
    ~USTax() {}
};

class DETax : public TaxStrategy
{
public:
    void calculate() override { std::cout<<"tax == DE_Tax\n"; }
    ~DETax() {}
};

// also can extend in other files()
class JPTax : public TaxStrategy
{
public:
    void calculate() override { std::cout<<"tax == JP_Tax\n"; }
    ~JPTax() {}
};

class SalesOrder
{
public:
    // explicit just for easy to test...
    explicit SalesOrder(std::unique_ptr<TaxStrategy> taxstrategy)
    {
        strategy = std::move(taxstrategy);
    }
    double CalculateTax()
    {
        strategy->calculate();
        return 0.0;
    }
    ~SalesOrder() {}
private:
    std::unique_ptr<TaxStrategy>strategy;
};

int main()
{
    std::unique_ptr<TaxStrategy>cnTaxStrategy = std::make_unique<CNTax>();
    SalesOrder cnSalesOrder(std::move(cnTaxStrategy));
    cnSalesOrder.CalculateTax();

    std::unique_ptr<TaxStrategy>cnTaxStrategy2 = std::make_unique<USTax>();
    SalesOrder cnSalesOrder2(std::move(cnTaxStrategy2));
    cnSalesOrder2.CalculateTax();
}


/*
 * a contrasted example
 * there are some questions.
 * if demand is add extra country's tax, such as JP_Tax, KR_Tax and so on
 * we have to modify TaxBase(add JP_Tax, KR_Tax) and CalculateTax(add other Tax's judgement)
 * so we have to compiler 'again' and test all(if it's a complex program), of course not there.
#include <iostream>

enum TaxBase
{
    CN_Tax,
    US_Tax,
    DE_Tax
};

class SalesOrder
{
public:
    explicit SalesOrder(TaxBase _tax) : tax(_tax) {}
    // imitate various algorithm calculation
    void CalculateTax()
    {
        if(tax == CN_Tax) { std::cout<<"tax == CN_Tax\n"; }
        else if(tax == US_Tax) { std::cout<<"tax == US_Tax\n"; }
        else if(tax == DE_Tax) { std::cout<<"tax == DE_Tax\n"; }
    }
private:
    TaxBase tax;
};

int main()
{
    SalesOrder s1(CN_Tax);
    s1.CalculateTax();
    SalesOrder s2(US_Tax);
    s2.CalculateTax();
}
*/