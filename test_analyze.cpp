
#include <filesystem>
#include <iostream>
#include <fstream>
#include <format>
#include <vector>

struct d1_report_stats
{
    int header_count = 0;
    int header_time = 0;
};

int get_line_count_no_whitespace(const std::filesystem::path& path)
{
    int count = 0;
    
    std::ifstream file(path.string());   
    std::string str;
    while (std::getline(file, str))
    {
        const bool is_not_whitespace = (str.find_first_not_of(" \t\n\v\f\r") != std::string::npos);
        count += (is_not_whitespace ? 1 : 0);
    }
    
    return count;
}

d1_report_stats get_d1_report_stats(const std::filesystem::path& path)
{
    d1_report_stats stats;
    
    std::ifstream file(path.string());   
    std::string str;
    bool found_include_start = false;
    bool stats_found = false;
    while (std::getline(file, str))
    {
        if(!found_include_start && (str == "Include Headers:")) {
            found_include_start = true;
            continue;
        }
        
        if(str.contains("\tCount: ")) {
            std::string count_str = str.substr(8);
            stats.header_count = std::stoi(count_str);
            continue;
        }
        
        if(str.contains("\tTotal: ")) {
            std::string total_str = str.substr(8);
            total_str = total_str.substr(0, total_str.size() - 1); // Remove s from end
            stats.header_time = static_cast<int>(std::stod(total_str) * 1000);
            stats_found = true;
            break;
        }
    }
    
    if(!stats_found) {
        std::cout << path.string().c_str() << ": Stats were not found\n";
    }
    
    return stats;
}

struct result
{
    std::string name;
    d1_report_stats d1_cpp14{};
    d1_report_stats d1_cpp17{};
    d1_report_stats d1_cpp20{};
    d1_report_stats d1_cpplatest{};
    int line_count_comments_cpp14 = 0;
    int line_count_comments_cpp17 = 0;
    int line_count_comments_cpp20 = 0;
    int line_count_comments_cpplatest = 0;
    int line_count_no_comments_cpp14 = 0;
    int line_count_no_comments_cpp17 = 0;
    int line_count_no_comments_cpp20 = 0;
    int line_count_no_comments_cpplatest = 0;
};

int main()
{
    std::vector<result> results;
    
    for (auto const& temp_entry : std::filesystem::directory_iterator{std::filesystem::path("temp")})
    {
        std::cout << temp_entry.path().filename().string().c_str() << "\n";
        if(temp_entry.is_directory())
        {
            result r{};
            r.name = temp_entry.path().filename().string();
            
            for (auto const& test_entry : std::filesystem::directory_iterator{temp_entry.path()})
            {
                // d1report stats
                if(test_entry.path().filename() == "d1report_cpp14.txt") { 
                    r.d1_cpp14 = get_d1_report_stats(test_entry.path());
                }
                if(test_entry.path().filename() == "d1report_cpp17.txt") { 
                    r.d1_cpp17 = get_d1_report_stats(test_entry.path());
                }
                if(test_entry.path().filename() == "d1report_cpp20.txt") { 
                    r.d1_cpp20 = get_d1_report_stats(test_entry.path());
                }
                if(test_entry.path().filename() == "d1report_cpplatest.txt") { 
                    r.d1_cpplatest = get_d1_report_stats(test_entry.path());
                }
                
                // Line counts with comments
                if(test_entry.path().filename() == "expanded_comments_cpp14.cpp") {
                    r.line_count_comments_cpp14 = get_line_count_no_whitespace(test_entry.path());
                }
                if(test_entry.path().filename() == "expanded_comments_cpp17.cpp") {
                    r.line_count_comments_cpp17 = get_line_count_no_whitespace(test_entry.path());
                }
                if(test_entry.path().filename() == "expanded_comments_cpp20.cpp") {
                    r.line_count_comments_cpp20 = get_line_count_no_whitespace(test_entry.path());
                }
                if(test_entry.path().filename() == "expanded_comments_cpplatest.cpp") {
                    r.line_count_comments_cpplatest = get_line_count_no_whitespace(test_entry.path());
                }
                
                // Line counts without comments
                if(test_entry.path().filename() == "expanded_no_comments_cpp14.cpp") {
                    r.line_count_no_comments_cpp14 = get_line_count_no_whitespace(test_entry.path());
                }
                if(test_entry.path().filename() == "expanded_no_comments_cpp17.cpp") {
                    r.line_count_no_comments_cpp17 = get_line_count_no_whitespace(test_entry.path());
                }
                if(test_entry.path().filename() == "expanded_no_comments_cpp20.cpp") {
                    r.line_count_no_comments_cpp20 = get_line_count_no_whitespace(test_entry.path());
                }
                if(test_entry.path().filename() == "expanded_no_comments_cpplatest.cpp") {
                    r.line_count_no_comments_cpplatest = get_line_count_no_whitespace(test_entry.path());
                }
            }
            
            results.push_back(r);
            
        }
    }
    
    std::string output;
    
    for(auto& r : results)
    {
        output += std::vformat(
            "{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{}\n",
            std::make_format_args(
                r.name,
                r.d1_cpp14.header_count,
                r.d1_cpp17.header_count,
                r.d1_cpp20.header_count,
                r.d1_cpplatest.header_count,
                r.d1_cpp14.header_time,
                r.d1_cpp17.header_time,
                r.d1_cpp20.header_time,
                r.d1_cpplatest.header_time,
                r.line_count_comments_cpp14,
                r.line_count_comments_cpp17,
                r.line_count_comments_cpp20,
                r.line_count_comments_cpplatest,
                r.line_count_no_comments_cpp14,
                r.line_count_no_comments_cpp17,
                r.line_count_no_comments_cpp20,
                r.line_count_no_comments_cpplatest
            )
        );
    }
    
    std::ofstream f("results/results.csv");
    f << output;
    f.close();
    
}
