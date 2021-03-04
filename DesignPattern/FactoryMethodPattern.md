# 工厂模式
通过"对象创建"模式绕开```new```,来避免对象创建```new```过程中所导致的紧耦合(以来具体类),从而支持对象创建的稳定.它是接口抽象之后的第一步工作

## why?
1. 在软件系统中, 经常面临着创建对象的工作; 由于需求的变化, 需要创建的对象的```具体类型```经常发生```变化```
2. 如何应对这种变化? 如何绕过常规的对象创建方法(```new```),提供一种"封装机制"来避免客户程序和这种具体对象创建工作的的紧耦合?

## 先看一个例子
在以下例子中, 倘若新增需求, 例如新增二进制、文本、图片、视频分割器等需求, 则需要在```MainForm```中的第```33```行则需要```明确指定```生成的对象, 如这里的```BinarySplitter```对象, 因此```MainForm```类编译时会对```BinarySplitter```类产生依赖. 在这种情况下, ```MainForm```违背了依赖倒置原则, ```MainForm```应当依赖于抽象, 而不是依赖于实现的细节(如这里的```BinarySplitter```). 那么如何解决这个问题呢? 简单地将```BinarySplitter```替换为```ISplitter```是不可行的. 

```cpp
// ISplitter.cpp
class ISplitter{
public:
    virtual void split() = 0;
    virtual ~ISplitter(){}
};

class BinarySplitter : public ISplitter{};
class TxtSplitter: public ISplitter{};
class PictureSplitter: public ISplitter{};
class VideoSplitter: public ISplitter{};

// MainForm.cpp
class MainForm : public Form
{
private:
	TextBox* txtFilePath;
	TextBox* txtFileNumber;
	ProgressBar* progressBar;
public:
    void Button1_Click()
    {
        ISplitter* splitter =
            new BinarySplitter();//依赖具体类
            
        splitter->split();
	}
};
```

我们不能```显示地```使用```new```来创建对象, 那么是否可以使用另外一个方法来创建对象呢?
```cpp
class ISplitter{
public:
    virtual void split() = 0;
    virtual ~ISplitter(){}
};

class BinarySplitter : public ISplitter{};
class TxtSplitter: public ISplitter{};
class PictureSplitter: public ISplitter{};
class VideoSplitter: public ISplitter{};

class SplitterFactory{
public:
    ISPlitter* CreateSplitter()
    {
        return new BinarySplitter();
        // return new TxtSplitter();
        // return new PictureSplitter();
    }
};

// MainForm.cpp
class MainForm : public Form
{
private:
	TextBox* txtFilePath;
	TextBox* txtFileNumber;
	ProgressBar* progressBar;
public:
    void Button1_Click()
    {
        SplitterFactory factory;
        ISplitter* splitter = factory.CreateSplitter();
	}
};
```

但是仍然没有解决问题, 因此需要使用```虚函数```来解决这个问题

```cpp

class ISplitter{
public:
    virtual void split() = 0;
    virtual ~ISplitter(){}
};
// 具体类
class BinarySplitter : public ISplitter{};
class TxtSplitter: public ISplitter{};
class PictureSplitter: public ISplitter{};
class VideoSplitter: public ISplitter{};
// 抽象工厂
class SplitterFactory{
public:
    virtual ISPlitter* CreateSplitter() = 0;
    virtual ~SplitterFactory(){}
};
// 具体工厂
class BinarySplitterFactory : public SplitterFactory{
public:
    ISplitter* CreateSplitter() override { return new BinarySplitter(); }
};
class TxtSplitterFactory : public SplitterFactory{
public:
    ISplitter* CreateSplitter() override { return new TxtSplitter(); }
};
class PictureSplitterFactory : public SplitterFactory{
public:
    ISplitter* CreateSplitter() override { return new PictureSplitter(); }
};
class VideoSplitterFactory : public SplitterFactory{
public:
    ISplitter* CreateSplitter() override { return new VideoSplitter(); }
};

// MainForm.cpp
class MainForm : public Form
{
private:
    SplitterFactory* factory;       // 工厂
public:
    MainForm(SplitterFactory* factory) : factory(factory) {}
    void Button1_Click()
    {
        ISplitter* splitter = factory->CreateSplitter();
    }
};
```

通过在```MainForm```里添加```SplitterFactory```指针, 利用```虚函数```的运行时延迟绑定特性, 消除了```MainForm```对之前诸如```BinaryFactor```的依赖, 但是对于每一个新增的```Product```, 仍然要继承```SplitterFactory```抽象工厂, 做出具体的实现. 本应在```MainForm```中的实现, 被移动到了```SplitterFactory```的子类中去实现, 从而实现了对```MainForm```的复用.(amend -> extend)

