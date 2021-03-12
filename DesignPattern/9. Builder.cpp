#include <iostream>
#include <vector>
#include <memory>
#include <string>

class House
{
public:
    virtual void Other() = 0;
    void Print()    // for debug
    {
        std::cout<<step1_para<<std::endl;
        std::cout<<step2_para<<std::endl;
        std::cout<<step3_para<<std::endl;
        std::cout<<step4_para<<std::endl;
        std::cout<<step5_para<<std::endl;
    }
    virtual ~House() = default;

    // mock a complex constructor
    std::string step1_para;
    std::string step2_para;
    std::string step3_para;
    std::string step4_para;
    std::string step5_para;
};

class StoneHouse : public House
{
public:
    void Other() override { std::cout<<"StoneHouse other......"<<std::endl; }
    ~StoneHouse() {}
};

class CrystalHouse : public House
{
public:
    void Other() override { std::cout<<"CrystalHouse other......"<<std::endl; }
    ~CrystalHouse() {}
};



class HouseBuilder
{
public:
    std::unique_ptr<House> getHouse() { return std::move(mHouse); }
    virtual ~HouseBuilder() = default;

    virtual void step1() const = 0;
    virtual void step2() const = 0;
    virtual void step3() const = 0;
    virtual void step4() const = 0;
    virtual void step5() const = 0;
protected:
    std::unique_ptr<House> mHouse;
};


class StoneHouseBuilder : public HouseBuilder
{
public:
    StoneHouseBuilder()
    {
        mHouse = std::make_unique<StoneHouse>();
    }
    ~StoneHouseBuilder() {}
protected:
    void step1() const override { mHouse->step1_para = "StoneHouse:111"; }
    void step2() const override { mHouse->step2_para = "StoneHouse:222"; }
    void step3() const override { mHouse->step3_para = "StoneHouse:333"; }
    void step4() const override { mHouse->step4_para = "StoneHouse:444"; }
    void step5() const override { mHouse->step5_para = "StoneHouse:555"; }
};


class CrystalHouseBuilder : public HouseBuilder
{
public:
    CrystalHouseBuilder()
    {
        mHouse = std::make_unique<CrystalHouse>();
    }
    ~CrystalHouseBuilder() {}
protected:
    void step1() const override { mHouse->step1_para = "CrystalHouse:111"; }
    void step2() const override { mHouse->step2_para = "CrystalHouse:222"; }
    void step3() const override { mHouse->step3_para = "CrystalHouse:333"; }
    void step4() const override { mHouse->step4_para = "CrystalHouse:444"; }
    void step5() const override { mHouse->step5_para = "CrystalHouse:555"; }
};


class HouseDirector
{
public:
    HouseDirector(std::unique_ptr<HouseBuilder> _houseBuilder) : houseBuilder(std::move(_houseBuilder)) {}
    std::unique_ptr<House> Construct()
    {
        houseBuilder->step1();
        houseBuilder->step2();
        houseBuilder->step3();
        houseBuilder->step4();
        houseBuilder->step5();
        return houseBuilder->getHouse();
    }
private:
    std::unique_ptr<HouseBuilder> houseBuilder;
};


int main()
{
    auto stoneHouse = std::make_unique<HouseDirector>(std::make_unique<StoneHouseBuilder>())->Construct();
    stoneHouse->Print();
    
    auto crystalHouse = std::make_unique<HouseDirector>(std::make_unique<CrystalHouseBuilder>())->Construct();
    crystalHouse->Print();

    return 0;
}

/*
#include <iostream>
#include <vector>
#include <memory>

class House
{
public:
    void init()
    {
        step1();
        for(int i = 0; i < 2; ++i)
            step2();
        step3();
        step4();
        for(int i = 0; i < 2; ++i)
            step5();
    }
    virtual ~House() = default;
protected:
    virtual void step1() = 0;
    virtual void step2() = 0;
    virtual void step3() = 0;
    virtual void step4() = 0;
    virtual void step5() = 0;
};

class StoneHouse : public House
{
public:
    StoneHouse() {}
    ~StoneHouse() {}
protected:
    void step1() override { std::cout<<"StoneHouse: step11111\n"; }
    void step2() override { std::cout<<"StoneHouse: step22222\n"; }
    void step3() override { std::cout<<"StoneHouse: step33333\n"; }
    void step4() override { std::cout<<"StoneHouse: step44444\n"; }
    void step5() override { std::cout<<"StoneHouse: step55555\n"; }
};

class Crystal : public House
{
public:
    Crystal() {}
    ~Crystal() {}
protected:
    void step1() override { std::cout<<"Crystal: step11111\n"; }
    void step2() override { std::cout<<"Crystal: step22222\n"; }
    void step3() override { std::cout<<"Crystal: step33333\n"; }
    void step4() override { std::cout<<"Crystal: step44444\n"; }
    void step5() override { std::cout<<"Crystal: step55555\n"; }
};

int main()
{
    std::vector<std::unique_ptr<House>>vec;
    vec.emplace_back(std::make_unique<StoneHouse>());
    vec.emplace_back(std::make_unique<Crystal>());
    for(auto& i : vec)
        i->init();
    return 0;
}
*/