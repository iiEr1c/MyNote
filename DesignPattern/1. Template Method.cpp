#include <iostream>
#include <vector>

class Library
{
public:
    /*
     * prerequisite:
     * 1. 'run' method is stable, also mean the Application's behavior skeleton is fixed.
     * 2. only step2 & step4 is variable, it must be custom.
     */
    void run()
    {
        step1();
        if(step2())
            step3();
        for(int i = 0; i < 3; ++i)
            step4();
        step5();
    }
    virtual ~Library() = default;
protected:
    virtual bool step2() = 0;
    virtual void step4() = 0;
private:
    void step1() { std::cout<<"function step11111()...\n"; }
    void step3() { std::cout<<"function step33333()...\n"; }
    void step5() { std::cout<<"function step55555()...\n"; }
};

class Application : public Library
{
public:
    bool step2() override
    {
        std::cout<<"step22222 is override, and return true...\n";
        return true;
    }

    void step4() override
    {
        std::cout<<"step44444 is override\n";
    }
    ~Application(){}
};

int main()
{
    Application app;
    app.run();
    return 0;
}
/* a example: unused Template Method pattern's code
#include <iostream>
class Library
{
public:
    void step1() { std::cout<<"function step11111()...\n"; }
    void step3() { std::cout<<"function step33333()...\n"; }
    void step5() { std::cout<<"function step55555()...\n"; }
    ~Library() {}
};

class Application
{
public:
    bool step2() override
    {
        std::cout<<"step22222 is override, and return true...\n";
        return true;
    }

    void step4() override
    {
        std::cout<<"step44444 is override\n";
    }
    ~Application() {}
};

int main()
{
    Library lib();
    Application app();
    lib.step1();
    if(app.step2())
        lib.step3();
    for(int i = 0; i < 3; ++i)
        app.step4();
    lib.step5();
    return 0;
}
*/