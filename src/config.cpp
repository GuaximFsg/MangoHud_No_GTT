#include <fstream>
#include <sstream>
#include <iostream>
#include <string.h>
#include "config.h"
#include "overlay_params.h"
#include "file_utils.h"

std::unordered_map<std::string,std::string> options;

void parseConfigLine(std::string line){
    if(line.find("#")!=std::string::npos)
        {
            line = line.erase(line.find("#"),std::string::npos);
        }
    size_t space = line.find(" ");
    while(space!=std::string::npos)
        {
            line = line.erase(space,1);
            space = line.find(" ");
        }
    space = line.find("\t");
    while(space!=std::string::npos)
        {
            line = line.erase(space,1);
            space = line.find("\t");
        }
    size_t equal = line.find("=");
    if(equal==std::string::npos)
        {
            if (!line.empty())
                options.insert({line, "1"});
            return;
        }
    
    options.insert({line.substr(0,equal), line.substr(equal+1)});
}

void parseConfigFile() {
    std::vector<std::string> paths;
    std::string home;
    static const char *config_dir = "/.config/MangoHud/";

    const char *env_home = std::getenv("HOME");
    if (env_home)
        home = env_home;
    if (!home.empty()) {
        paths.push_back(home + "/.local/share/MangoHud/MangoHud.conf");
        paths.push_back(home + config_dir + "MangoHud.conf");
    }

    std::string exe_path = get_exe_path();
    auto n = exe_path.find_last_of('/');
    if (!exe_path.empty() && n != std::string::npos && n < exe_path.size() - 1) {
        // as executable's name
        std::string basename = exe_path.substr(n + 1);
        if (!home.empty())
            paths.push_back(home + config_dir + basename + ".conf");

        // in executable's folder though not much sense in /usr/bin/
        paths.push_back(exe_path.substr(0, n) + "/MangoHud.conf");

        // find executable's path when run in Wine
        if (!home.empty() && (basename == "wine-preloader" || basename == "wine64-preloader")) {
            std::string line;
            std::ifstream stream("/proc/self/cmdline");
            while (std::getline(stream, line, '\0'))
            {
                if (!line.empty()
                    && (n = line.find_last_of('\\')) != std::string::npos
                    && n < line.size() - 1) // have at least one character
                {
                    auto dot = line.find_last_of('.');
                    if (dot < n)
                        dot = line.size();
                    paths.push_back(home + config_dir + "wine-" + line.substr(n + 1, dot - n - 1) + ".conf");
                    break;
                }
            }

        }
    }

    std::string line;
    for (auto& p : paths) {
        std::ifstream stream(p);
        if (!stream.good()) {
            // printing just so user has an idea of possible configs
            std::cerr << "skipping config: " << p << std::endl;
            continue;
        }

        std::cerr << "parsing config: " << p;
        while (std::getline(stream, line))
        {
            parseConfigLine(line);
        }
        std::cerr << " [ ok ]" << std::endl;

    }
}