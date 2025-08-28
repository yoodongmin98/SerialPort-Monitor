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

    void Instance(const float _X, const float _Y);
protected:

private:
	float WindowSizeX = 0;
	float WindowSizeY = 0;
};
