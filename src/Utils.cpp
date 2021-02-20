#include "Utils.hpp"
#include <fstream>
#include <sys/types.h>
#include <dirent.h>

void Utils::my_exit(int exit_status, const std::string &error_msg)
{
    std::cerr << error_msg << std::endl;
    exit(exit_status);
}

Utils::CMD Utils::get_cmd_output(const std::string &command)
{
    char buffer[2048];
    CMD ret;
    ret.error = false;

    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        ret.error = true;
        ret.returnValue = -1;
        ret.output = "Pipe Failed !";
        return ret;
    }

    while (!feof(pipe)) {
        if (fgets(buffer, 2048, pipe) != NULL)
            ret.output += buffer;
    }
    ret.returnValue = pclose(pipe);
    if (ret.output[ret.output.size() - 1] == '\n')
        ret.output.pop_back();
    return ret;
}

std::string Utils::get_file_content(const std::string &in)
{
    DIR* dir = opendir(in.c_str());
    if (dir)
        my_exit(84, "Cannot open directory [" + in + "], exiting...");
    std::ifstream file;
    file.open(in, std::ifstream::in);

    if (file.is_open()) {
        std::string data;
        char c = file.get();

        while (file.good()) {
            data += c;
            c = file.get();
        }
        file.close();
        if (data[data.size() - 1] == '\n')
            data.pop_back();
        return data;
    }
    file.close();
    my_exit(84, "Cannot open file [" + in + "], exiting...");
    return "";
}

std::vector<std::string> Utils::string_to_vector(std::string str, char separator)
{
    std::vector<std::string> array;
    std::string temp;
    size_t len = str.size();

    for (size_t i = 0; i < len; i++) {
        if (str[i] == separator) {
            array.push_back(temp);
            temp.clear();
        }
        else
            temp.push_back(str[i]);
    }
    array.push_back(temp);
    return array;
}