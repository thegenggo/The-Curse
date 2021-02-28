#include <SFML/System.hpp>
#include <iostream>

void func()
{
    // this function is started when thread.launch() is called

    for (int i = 0; i < 10; ++i)
        std::cout << "I'm thread number one" << std::endl;
}

int main()
{
    void Print_color(int color);

    enum class Web_color { red = 0xFF0000, green = 0x00FF00, blue = 0x0000FF }; // no C26812
    enum class Product_info { red = 0, purple = 1, blue = 2 }; // no C26812

    Web_color webby = Web_color::blue;
    Print_color(webby);  // Error: cannot convert Web_color to int.
    Print_color(Product_info::Red);  // Error: cannot convert Product_info to int.

    return 0;
}