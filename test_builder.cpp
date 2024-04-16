// Parse include.txt and generate test programs

#include <fstream>
#include <string>
#include <algorithm>
#include <format>
#include <filesystem>

int main() 
{
    std::ifstream file("include.csv");
    std::string str; 
    while (std::getline(file, str))
    {
        // line is delimited by ;
        // Test #;name;include text
        // Replace ',' in include text with '\n'
        
        auto firstDelim = str.find(';', 0);
        auto secondDelim = str.find(';', firstDelim + 1);
        
        auto testNum = str.substr(0, firstDelim);
        auto name = str.substr(firstDelim + 1, secondDelim - firstDelim - 1);
        auto includeText = str.substr(secondDelim + 1);
        
        std::replace(includeText.begin(), includeText.end(), ',', '\n');
        
        //printf("%s | %s | %s\n", testNum.c_str(), name.c_str(), includeText.c_str());
        
        std::string test_data_raw = "{}\nint main() {{ return 0; }}\n";
        std::string test_data = std::vformat(test_data_raw, std::make_format_args(includeText));
        std::string test_path = std::format("temp/{}_{}", testNum, name);
        std::string test_file = std::format("temp/{}_{}/test.cpp", testNum, name);
        
        std::filesystem::create_directories(test_path);
        
        std::ofstream f(test_file);
        f << test_data;
        f.close();
    }
}
