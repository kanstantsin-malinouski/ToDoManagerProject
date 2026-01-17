#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cctype>


namespace utils {

    inline std::string trim(std::string s) {
        auto notSpace = [](unsigned char ch) { return !std::isspace(ch); };
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), notSpace));
        s.erase(std::find_if(s.rbegin(), s.rend(), notSpace).base(), s.end());
        return s;
    }

    inline std::string toLower(std::string s) {
        std::transform(s.begin(), s.end(), s.begin(),
            [](unsigned char c) { return (char)std::tolower(c); });
        return s;
    }

    inline std::vector<std::string> split(const std::string& s, char delim) {
        std::vector<std::string> out;
        std::stringstream ss(s);
        std::string item;
        while (std::getline(ss, item, delim)) out.push_back(item);
        return out;
    }

    // Simple "escape" for our delimiter '|'
    inline std::string escapePipes(const std::string& s) {
        std::string out;
        out.reserve(s.size());
        for (char c : s) {
            if (c == '|') out += "\\|";
            else if (c == '\\') out += "\\\\";
            else out += c;
        }
        return out;
    }

    inline std::string unescapePipes(const std::string& s) {
        std::string out;
        out.reserve(s.size());
        for (size_t i = 0; i < s.size(); i++) {
            if (s[i] == '\\' && i + 1 < s.size()) {
                char n = s[i + 1];
                if (n == '|' || n == '\\') { out += n; i++; continue; }
            }
            out += s[i];
        }
        return out;
    }

    // Input helpers
    inline int readInt(const std::string& prompt, int minV, int maxV) {
        while (true) {
            std::cout << prompt;
            std::string line;
            std::getline(std::cin, line);
            line = trim(line);
            try {
                int v = std::stoi(line);
                if (v < minV || v > maxV) throw std::runtime_error("range");
                return v;
            }
            catch (...) {
                std::cout << "Invalid number. Try again.\n";
            }
        }
    }

    inline std::string readLine(const std::string& prompt, bool allowEmpty = false) {
        while (true) {
            std::cout << prompt;
            std::string line;
            std::getline(std::cin, line);
            line = trim(line);
            if (allowEmpty || !line.empty()) return line;
            std::cout << "Input cannot be empty. Try again.\n";
        }
    }

    inline bool isValidDateYYYYMMDD(const std::string& s) {
        // Very simple validation: YYYY-MM-DD
        if (s.size() != 10) return false;
        if (s[4] != '-' || s[7] != '-') return false;
        for (size_t i = 0; i < s.size(); i++) {
            if (i == 4 || i == 7) continue;
            if (!std::isdigit((unsigned char)s[i])) return false;
        }
        int y = std::stoi(s.substr(0, 4));
        int m = std::stoi(s.substr(5, 2));
        int d = std::stoi(s.substr(8, 2));
        if (y < 1900 || y > 3000) return false;
        if (m < 1 || m > 12) return false;
        if (d < 1 || d > 31) return false;
        return true;
    }

} // namespace utils
