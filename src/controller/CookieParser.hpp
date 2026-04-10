#include <string>
#include <unordered_map>
#include <sstream>
#include <algorithm>

class CookieParser
{
public:
    inline static std::unordered_map<std::string, std::string> parse(const std::string &cookieHeader)
    {
        std::unordered_map<std::string, std::string> cookies;
        std::istringstream stream(cookieHeader);
        std::string token;

        while (std::getline(stream, token, ';'))
        {
            auto pos = token.find('=');
            if (pos != std::string::npos)
            {
                std::string key = trim(token.substr(0, pos));
                std::string value = trim(token.substr(pos + 1));
                cookies[key] = value;
            }
        }
        return cookies;
    }

private:
    inline static std::string trim(const std::string &str)
    {
        size_t start = str.find_first_not_of(" \t");
        size_t end = str.find_last_not_of(" \t");
        return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
    }
};
