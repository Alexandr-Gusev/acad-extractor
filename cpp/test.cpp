#include <iostream>

#include "core.h"
#include "utils.h"

int main(void)
{
    SetConsoleOutputCP(65001);
    std::cout << "Select AutoCAD blocks on screen" << std::endl;
    std::vector<BSTR> names;
    std::vector<std::vector<std::pair<BSTR, BSTR>>> attrs_list;
    std::vector<std::vector<std::pair<BSTR, VARIANT>>> props_list;
    try
    {
        DWORD dt = select_on_screen(&names, &attrs_list, &props_list);
        std::cout << names.size() << " at " << dt << " ms" << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cout << "exception: " << e.what() << std::endl;
        return 1;
    }
    for (size_t i = 0; i < names.size(); i++)
    {
        std::cout << std::string(70, '-') << std::endl;
        std::cout << utf8(names[i]) << std::endl;
        for (size_t j = 0; j < attrs_list[i].size(); j++)
        {
            std::cout << "attr " << utf8(attrs_list[i][j].first) << " = " << utf8(attrs_list[i][j].second) << std::endl;
        }
        for (size_t j = 0; j < props_list[i].size(); j++)
        {
            std::cout << "prop " << utf8(props_list[i][j].first) << " = ";
            if (V_VT(&props_list[i][j].second) == VT_BSTR)
            {
                std::cout << utf8(V_BSTR(&props_list[i][j].second));
            }
            else
            {
                std::cout << V_R8(&props_list[i][j].second);
            }
            std::cout << std::endl;
        }
    }
    return 0;
}
