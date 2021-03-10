#include <iostream>
#include <string>
#include <memory>
#include <vector>

class Editor
{
public:
    virtual void edit() const = 0;
    virtual ~Editor() = default;
};

class TextEditor : public Editor
{
public:
    TextEditor() {}
    void edit() const override { std::cout<<"TextEditor..."<<std::endl; }
    ~TextEditor() {}
};

class BinaryEditor : public Editor
{
public:
    BinaryEditor() {}
    void edit() const override { std::cout<<"BinaryEditor..."<<std::endl; }
    ~BinaryEditor() {}
};

class PictureEditor : public Editor
{
public:
    PictureEditor() {}
    void edit() const override { std::cout<<"PictureEditor..."<<std::endl; }
    ~PictureEditor() {}
};

class EditorFactory
{
public:
    virtual std::unique_ptr<Editor> createEditor() = 0;
    virtual ~EditorFactory() = default;
};

class TextEditorFactory : public EditorFactory
{
public:
    std::unique_ptr<Editor> createEditor() override
    {
        return std::make_unique<TextEditor>();
    }
    ~TextEditorFactory() {}
};


class BinaryEditorFactory : public EditorFactory
{
public:
    std::unique_ptr<Editor> createEditor() override
    {
        return std::make_unique<BinaryEditor>();
    }
    ~BinaryEditorFactory() {}
};


class PictureEditorFactory : public EditorFactory
{
public:
    std::unique_ptr<Editor> createEditor() override
    {
        return std::make_unique<PictureEditor>();
    }
    ~PictureEditorFactory() {}
};


class Click
{
public:
    virtual void click() = 0;
    virtual ~Click() = default;
};


class ButtonClick : public Click
{
public:
    ButtonClick(std::unique_ptr<EditorFactory> _factory) : factory(std::move(_factory)) {}
    ButtonClick(const ButtonClick&) = delete;
    ButtonClick& operator=(const ButtonClick&) = delete;
    ButtonClick(ButtonClick&& button) noexcept : factory(std::move(button.factory)) {}
    void click() override
    {
        std::unique_ptr<Editor> text = factory->createEditor();
        text->edit();
    }
    ~ButtonClick() {}
private:
    std::unique_ptr<EditorFactory> factory;
};


void mockApp()
{
    std::vector<std::unique_ptr<EditorFactory>>vec;
    vec.emplace_back(std::make_unique<TextEditorFactory>());
    vec.emplace_back(std::make_unique<BinaryEditorFactory>());
    vec.emplace_back(std::make_unique<PictureEditorFactory>());

    std::vector<ButtonClick>buttons;
    auto begin = vec.begin();
    auto end   = vec.end();
    for(; begin != end; ++begin)
        buttons.emplace_back(std::move(*begin));
    for(auto& i : buttons)
        i.click();
}




void dependConcreteClass()
{
    // how to return a class object rather than explicit declaration.
    std::unique_ptr<Editor> text = std::make_unique<TextEditor>();
    text->edit();

    std::unique_ptr<Editor> binary = std::make_unique<BinaryEditor>();
    binary->edit();

    std::unique_ptr<Editor> picture = std::make_unique<PictureEditor>();
    picture->edit();
}



int main()
{
    mockApp();
    return 0;
}