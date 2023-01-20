#include <iostream>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <array>
#include <sstream>
#include <fstream>

struct LanguageHasher
{
    std::size_t operator()(const std::array<std::string, 2> & a) const
    {
        return std::hash<std::string>{}(a[0]+"-"+a[1]);
    }   
};

using Dico = std::unordered_map<std::array<std::string, 2>,
                                std::unordered_map<std::string, std::string>, LanguageHasher>;


std::vector<std::string>
split_string(const std::string & s)
{
    std::vector<std::string> vec{};
    std::string word;
    std::istringstream ss(s);
    while(ss >> word)
    {
        vec.push_back(word);
    }
    return vec;

}
std::vector<std::string>
fetch_input()
{
    std::string line{};
    std::getline(std::cin,line);
    return split_string(line);
}



void
run_command(const std::vector<std::string> & input,
            Dico & dico,
            std::vector<std::vector<std::string>> & history,
            std::array<std::string, 2> & languages)
{
    if(input[0] == "add" && input.size() == 3)
    {
        history.push_back(input);
        dico[languages].insert({input[1], input[2]});
        std::cout << input[1] << " => "  << input[2] << std::endl;
    }
    else if (input[0] == "translate" && input.size() >= 2)
    {
        for (const std::string & word: std::vector<std::string>{input.begin()+1, input.end()})
        {
            const auto found = dico[languages].find(word);
            if (found == dico[languages].end())
            {
                std::cout << "??? ";
            }
            else
            {
                std::cout << found->second << " ";
            }
        }
        std::cout << std::endl;
    }
    else if (input[0] == "print" and input.size() == 1)
    {
        for (const auto & [wd,ts]: dico[languages])
        {
            std::cout << "  " << wd << " => " << ts << std::endl;
        }
    }
    else if(input[0] == "save" && input.size() == 2)
    {
        std::ofstream myfile;
        myfile.open (input[1]);
        for (const auto & line: history)
        {
            for (const std::string & wd: line)
            {
                myfile << wd << " ";
            }
            myfile << std::endl;
        }
        myfile.close();   
    }
    else if(input[0] == "load" && input.size() == 2)
    {
        std::string line;
        std::ifstream myfile (input[1]);
        if (myfile.is_open())
        {
            while ( getline (myfile,line) )
            {
                run_command(split_string(line), dico, history, languages);
            }
            myfile.close();
        }
        else
        {
            std::cout << "could not open file\n";
        }
    }
    else if (input[0] == "clear")
    {
        dico[languages].clear();
    }
    else if (input[0] == "remove" && input.size() == 2)
    {
        dico[languages].erase(input[1]);
        for (auto & [key, val]: dico[languages])
        {
            if (val==input[1])
            {
                dico[languages].erase(key);
                break;
            }
        }
    }
    else if (input[0] == "from" && input.size() == 2)
    {
        languages[0] = input[1];
    }
    else if (input[0] == "to" && input.size() == 2)
    {
        languages[1] = input[1];
    }
    else
    {
        std::cout << "Malformed input\n";
    }

}

int
main(const int32_t argc,
     const char ** argv)
{
    std::string language_source = "fr";
    std::string language_target = "en";
    if (argc == 3)
    {
        language_source = argv[1];
        language_target = argv[2];
    }

    std::array<std::string, 2> languages{language_source, language_target};
    const std::unordered_set<std::string>
        exit_keywords{ "e", "exit", "q", "quit" };

    Dico dico{{languages, {}}};
    std::vector<std::vector<std::string>> history;
    while (true)
    {
        std::cout << "type something: ";
        const std::vector<std::string> input = fetch_input();

        if (exit_keywords.count(input[0])) break;
        run_command(input, dico, history, languages);
    }

    return 0;
}
