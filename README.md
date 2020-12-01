# WinLamb

A lightweight, header-only C++17 library to build native Windows GUIs, using pure [Win32 API](https://en.wikipedia.org/wiki/Windows_API) and [lambda closures](https://www.cprogramming.com/c++11/c++11-lambda-closures.html) to handle window messages. It's a thin layer over the Win32 API.

**Note:** If you're looking for the old C++11, version, [see this branch](https://github.com/rodrigocfd/winlamb/tree/cpp11).

## Usage

WinLamb is a header-only library. The easiest way to use it is simply download the files, and `#include` them.

For detailed information, [check out the documentation](https://rodrigocfd.github.io/winlamb).

## Example

The [example](https://rodrigocfd.github.io/winlamb/ex02.html) below is a complete Win32 program. Each window has a class, and messages are handled with lambda closures. There's no need to write a message loop or window registering.

* Declaration file: `My_Window.h`

````cpp
#include "winlamb/button.h"
#include "winlamb/window_main.h"

class My_Window : public wl::window_main {
private:
    wl::button btnHey{this};

public:
    My_Window();
};
````

* Implementation file: `My_Window.cpp`

````cpp
#include "My_Window.h"

RUN(My_Window)

My_Window::My_Window()
{
    setup().title = L"Click the window...";
    setup().style |= WS_MINIMIZEBOX;
    setup().size = {350, 250};

    on().wm_create([this]()
    {
        // WM_CREATE is sent to our window once, right after its creation.
        // Here we create all child controls, and perform any initial tasks.

        btnHey.create(L"Click me", {20, 20}, 80, BS_DEFPUSHBUTTON);
    });

    btnHey.on().bn_clicked([this]()
    {
        // BN_CLICKED notification is sent every time the button is clicked.

        set_title(L"Hey, button clicked");
    });
}
````

Note that Win32 uses [Unicode strings](https://docs.microsoft.com/en-us/windows/win32/learnwin32/working-with-strings) natively, that means to use `wchar_t`, `std::wstring` and `std::wstring_view`.

## License

Licensed under [MIT license](https://opensource.org/licenses/MIT), see [LICENSE.md](LICENSE.md) for details.