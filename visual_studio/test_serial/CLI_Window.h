#pragma once



class CLI_Window
{
public:
    CLI_Window();
    ~CLI_Window();

    CLI_Window(const CLI_Window&) = delete;
    CLI_Window(CLI_Window&&) = delete;
    CLI_Window& operator=(const CLI_Window&) = delete;
    CLI_Window& operator=(CLI_Window&&) = delete;

    void Instance();
protected:

private:

};
