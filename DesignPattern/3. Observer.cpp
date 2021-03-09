#include <iostream>
#include <thread>
#include <chrono>
#include <memory>
#include <list>
#include <string>
#include <fstream>

class Progress
{
public:
    explicit Progress(std::string _hashString) : hashString(_hashString) {}
    virtual void doProgress(int) = 0;
    virtual ~Progress() = default;
    std::string hashString;
};


class myProgress : public Progress
{
public:
    explicit myProgress(std::string str) : Progress(str) {}
    void doProgress(int curProgress) override
    {
        // assume progress is [==========>], so it has 14 backspace characters('\b');
        std::cout<<"\b\b\b\b\b\b\b\b\b\b\b\b\b\b";
        std::cout<<'[';
        auto i = 1;
        while(i++ <= curProgress)
            std::cout<<'=';
        std::cout<<'>';
        while(i++ <= 10)
            std::cout<<' ';
        std::cout<<']';
    }
    ~myProgress() {}
};

class myProgress2 : public Progress
{
public:
    explicit myProgress2(std::string str) : Progress(str) {}
    void doProgress(int curProgress) override
    {
        std::ofstream outfile("./test.txt");
        outfile<<"myProgress2\n";
        outfile<<std::endl;
        outfile.close();
    }
    ~myProgress2() {}
};






class ALongTimeTask
{
public:
    ALongTimeTask() {}
    void addProgress(std::unique_ptr<Progress>m_progress)
    {
        m_progress_list.emplace_back(std::move(m_progress));
    }
    void removeProgress(std::string hashStr)
    {
        auto begin = m_progress_list.begin();
        auto end   = m_progress_list.end();
        while(begin != end)
        {
            if(begin->get()->hashString == hashStr)
                begin = m_progress_list.erase(begin);
            ++begin;
        }
    }
    void Doing()
    {
        using namespace std::chrono_literals;
        for(int i = 0; i < taskSum;)
        {
            auto curProgress = ++i;
            onProgress(curProgress);            // deal progress...
            if(i == taskSum) break;
            std::this_thread::sleep_for(3s);    // imitate some time-consume task....
        }
        std::cout<<std::endl;
    }
    ~ALongTimeTask() {}
protected:
    void onProgress(int val)
    {
        auto begin = m_progress_list.begin();
        auto end   = m_progress_list.end();
        for(; begin != end; ++begin)
            (*begin)->doProgress(val);
    }
private:
    int taskSum = 10;       // assume have 10 tasks
    std::list<std::unique_ptr<Progress>>m_progress_list;
};






class MainForm
{
public:
    void button()
    {
        std::unique_ptr<Progress>myProgressBar = std::make_unique<myProgress>("myProgressBar1");
        std::unique_ptr<Progress>myProgressBar2 = std::make_unique<myProgress2>("myProgressBar2");
        ALongTimeTask task;
        task.addProgress(std::move(myProgressBar));
        task.addProgress(std::move(myProgressBar2));
        task.removeProgress("myProgressBar1");
        task.Doing();
    }
    ~MainForm() {}
};

int main()
{
    MainForm app;
    app.button();
}



/*
 * 
 * ALongTimeTask depend on class ProgressBar's detail, when we try to change progress's style,
 * we have to modify updateProgress and compiler all again.
 * 
 */

/*
#include <iostream>
#include <thread>
#include <chrono>
#include <memory>

class ProgressBar
{
public:
    void doProgress(int curProgress)
    {
        // assume progress is [==========>], so it has 14 backspace characters('\b');
        std::cout<<"\b\b\b\b\b\b\b\b\b\b\b\b\b\b";
        std::cout<<'[';
        auto i = 1;
        while(i++ <= curProgress)
            std::cout<<'=';
        std::cout<<'>';
        while(i++ <= 10)
            std::cout<<' ';
        std::cout<<']';
    }
    ~ProgressBar() {}
};

class ALongTimeTask
{
public:
    explicit ALongTimeTask(std::unique_ptr<ProgressBar>_m_progressBar)
    {
        m_progressBar = std::move(_m_progressBar);
    }
    void Doing()
    {
        using namespace std::chrono_literals;
        std::cout<<"current progress:"<<std::endl;
        for(int i = 0; i < taskSum;)
        {
            auto curProgress = ++i;
            m_progressBar->doProgress(curProgress);
            if(i == 10) break;
            std::this_thread::sleep_for(3s);
        }
        std::cout<<std::endl;
    }
    ~ALongTimeTask() {}
private:
    int taskSum = 10;       // assume have 10 tasks
    std::unique_ptr<ProgressBar>m_progressBar;
};


int main()
{
    std::unique_ptr<ProgressBar>m_progressBar = std::make_unique<ProgressBar>();
    ALongTimeTask task(std::move(m_progressBar));
    task.Doing();
    return 0;
}
*/