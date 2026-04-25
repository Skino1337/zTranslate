#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <filesystem>
#include <source_location>

#include <sstream> // Для std::stringstream
#include <thread>
#include <chrono>

#include <unordered_set>
#include <set>
#include <unordered_map>
#include <map>
#include <any>
#include <algorithm>
#include <utility>

#include <TinyJSON.h>
#include <SHA256.h>

namespace GOTHIC_NAMESPACE
{
     std::unordered_map<std::uint32_t, std::string> stack_token_map = {
        {0,"OP_PLUS"},
        {1,"OP_MINUS"},
        {2,"OP_MUL"},
        {3,"OP_DIV"},
        {4,"OP_MOD"}, // "%" is the modulus (or modulo) operator
        {5,"OP_OR"},
        {6,"OP_AND"},
        {7,"OP_LOWER"},
        {8,"OP_HIGHER"},
        {9,"OP_MOVINT"},
        {11,"OP_LOG_OR"},
        {12,"OP_LOG_AND"},
        {13,"OP_SHIFTL"},
        {14,"OP_SHIFTR"},
        {15,"OP_LOWER_EQ"},
        {16,"OP_EQUAL"},
        {17,"OP_NOTEQUAL"},
        {18,"OP_HIGHER_EQ"},
        {19,"OP_ADD_MOVE_INT"}, // +=
        {20,"OP_SUB_MOVE_INT"}, // -+
        {21,"OP_MUL_MOVE_INT"}, // /=
        {22,"OP_DIV_MOVE_INT"}, // *=
        // {30,"OP_UNARY"}, // +BELTBONUS_STR01);
        {30,"OP_UN_PLUS"}, // +BELTBONUS_STR01);
        {31,"OP_UN_MINUS"}, // change 1 to -1 // -BELTBONUS_STR01);
        {32,"OP_UN_NOT"}, // !NPC_ISDEAD(SLF)
        {33,"OP_UN_NEG"}, // if (!(LIGHT.LIGHTCOLOR & ~ZCOLOR_ALPHA)) {
        // {33,"OP_MAX"}, // = OP_UN_NEG
        {40,"TOK_BRACKETON"},
        {41,"TOK_BRACKETOFF"},
        {42,"TOK_SEMIKOLON"},
        {43,"TOK_KOMMA"},
        {44,"TOK_SCHWEIF"},
        {45,"TOK_NONE"},
        {51,"TOK_FLOAT"},
        {52,"TOK_VAR"},
        {53,"TOK_OPERATOR"},
        {60,"TOK_RET"},
        {61,"TOK_CALL"},
        {62,"TOK_CALLEXTERN"},
        {63,"TOK_POPINT"},
        {64,"TOK_PUSHINT"},
        {65,"TOK_PUSHVAR"},
        {66,"TOK_PUSHSTR"},
        {67,"TOK_PUSHINST"},
        {68,"TOK_PUSHINDEX"},
        {69,"TOK_POPVAR"},
        {70,"TOK_ASSIGNSTR"},
        {71,"TOK_ASSIGNSTRP"},
        {72,"TOK_ASSIGNFUNC"},
        {73,"TOK_ASSIGNFLOAT"},
        {74,"TOK_ASSIGNINST"},
        {75,"TOK_JUMP"}, // unconditional
        {76,"TOK_JUMP_IF_NOT_TRUE"}, // JNE / JNZ	Jump if Not Equal / Jump if Not Zero	ZF = 0
        {80,"TOK_SETINSTANCE"},
        {90,"TOK_SKIP"},
        {91,"TOK_LABEL"},
        {92,"TOK_FUNC"},
        {93,"TOK_FUNCEND"},
        {94,"TOK_CLASS"},
        {95,"TOK_CLASSEND"},
        {96,"TOK_INSTANCE"},
        {97,"TOK_INSTANCEEND"},
        {98,"TOK_NEWSTRING"},
        {128,"TOK_VARARRAY"},
        {245,"TOK_PUSHVAR_ARRAY"},
    };

    std::vector<std::string> split_string(const std::string & string_data, std::string delimiter = " ")
    {
        const uint32_t size = string_data.length() + 1;
        char* buffer = new char[size];
        strcpy_s(buffer, size, string_data.c_str());
        char* context = nullptr;
        char* token = strtok_s(buffer, delimiter.c_str(), &context);

        std::vector<std::string> tokens;
        while (token != nullptr)
        {
            tokens.emplace_back(token);
            token = strtok_s(nullptr, delimiter.c_str(), &context);
        }

        return tokens;
    }

    bool string_have_substring_case_insensitive(std::string search_string, std::string search_substring)
    {
        for (char & c : search_string)
            c = std::tolower((unsigned char)c);

        for (char & c : search_substring)
            c = std::tolower((unsigned char)c);

        return search_string.find(search_substring) != std::string::npos;
    }

    bool string_compare_case_insensitive(const std::string & a, const std::string & b)
    {
        const bool is_length_equals = a.length() == b.length();
        const bool is_chars_equals = std::equal(a.begin(), a.end(), b.begin(),
            [] (const unsigned char char_a, const unsigned char char_b) {
                return std::tolower(char_a) == std::tolower(char_b);
            });

        return is_length_equals && is_chars_equals;
    }

    std::string const_char_to_string_bytes(const char* data)
    {
        size_t length = 0;
        while (data[length] != '\0')
            length++;

        std::ostringstream result;
        for (size_t i = 0; i < length; ++i)
        {
            result << "0x" << std::hex << std::uppercase << std::setw(2) << std::setfill('0')
            << static_cast<unsigned int>(static_cast<unsigned char>(data[i])) << " ";
        }

        return result.str();
    }

    std::string byte_to_hex_string(unsigned char byte)
    {
        char buffer[3]; // 2 chars for hex, 1 for null terminator
        sprintf_s(buffer, "%02X", byte); // %02X formats as uppercase hex, min width 2, zero-padded
        return std::string(buffer);
    }

    int32_t get_token_next_bytes(byte* bytecode, uint32_t offset)
    {
        int32_t symbol_index = 0;
        // Cast to uint32_t before shifting to avoid sign extension issues
        symbol_index |= static_cast<int32_t>(bytecode[offset + 4]) << 24;
        symbol_index |= static_cast<int32_t>(bytecode[offset + 3]) << 16;
        symbol_index |= static_cast<int32_t>(bytecode[offset + 2]) << 8;
        symbol_index |= static_cast<int32_t>(bytecode[offset + 1]) << 0;

        return symbol_index;
    }

    int32_t get_reversed_token_next_bytes(byte* bytecode, uint32_t offset)
    {
        int32_t symbol_index = 0;
        // Cast to uint32_t before shifting to avoid sign extension issues
        symbol_index |= static_cast<int32_t>(bytecode[offset + 4]) << 0;
        symbol_index |= static_cast<int32_t>(bytecode[offset + 3]) << 8;
        symbol_index |= static_cast<int32_t>(bytecode[offset + 2]) << 16;
        symbol_index |= static_cast<int32_t>(bytecode[offset + 1]) << 24;

        return symbol_index;
    }

    std::string bytes_to_stack_data(byte* bytecode, const uint32_t offset, const uint32_t length, std::string& comment)
    {
        std::stringstream address_string_stream;
        address_string_stream << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << offset;

        std::stringstream address_string_stream_f;
        address_string_stream_f << std::left << std::setw(32) << address_string_stream.str();

        std::stringstream bytes_string_stream;
        bytes_string_stream << std::hex << std::uppercase << std::setfill('0');
        for (uint32_t i = 0; i < length; ++i)
        {
            bytes_string_stream << std::setw(2) << static_cast<uint32_t>(bytecode[i + offset]);
            if (i != length - 1)
            {
                bytes_string_stream << " ";
            }
        }

        std::stringstream bytes_string_stream_f;
        bytes_string_stream_f << std::left << std::setw(32) << bytes_string_stream.str();

        std::stringstream token_string_stream;
        auto token = static_cast<uint32_t>(bytecode[offset]);
        token_string_stream << stack_token_map[token];

        if (length == 5)
        {
            int32_t symbol_index = get_token_next_bytes(bytecode, offset);
            if (stack_token_map[token].find("JUMP") != std::string::npos)
            {
                token_string_stream << " " << std::hex << std::uppercase
                << std::setfill('0') << std::setw(8) << symbol_index;
            }
            else
            {
                token_string_stream << " " << symbol_index;
            }

        }

        std::stringstream token_string_stream_f;
        token_string_stream_f << std::left << std::setw(32) << token_string_stream.str();

        std::stringstream stack_data_string_stream;
        stack_data_string_stream << address_string_stream_f.str();
        stack_data_string_stream << bytes_string_stream_f.str();
        stack_data_string_stream << token_string_stream_f.str();
        stack_data_string_stream << comment;
        stack_data_string_stream << std::endl << std::endl;

        return stack_data_string_stream.str();
    }

    std::string cp_to_utf8(const std::string& cp_str, uint32_t cp)
    {
        // 1. Convert CPXXXX to UTF-16
        int required_size_wide = MultiByteToWideChar(cp, 0, cp_str.c_str(), -1, nullptr, 0);
        if (required_size_wide == 0) return "";

        std::vector<wchar_t> wbuf(required_size_wide);
        MultiByteToWideChar(cp, 0, cp_str.c_str(), -1, wbuf.data(), required_size_wide);

        // 2. Convert UTF-16 to UTF-8
        int utf8_len = WideCharToMultiByte(CP_UTF8, 0, wbuf.data(), -1, nullptr, 0, nullptr, nullptr);
        if (utf8_len == 0) return "";

        std::vector<char> utf8_buf(utf8_len);
        WideCharToMultiByte(CP_UTF8, 0, wbuf.data(), -1, utf8_buf.data(), utf8_len, nullptr, nullptr);

        return std::string(utf8_buf.data());
    }

    std::string utf8_to_cp(const std::string& utf8_str, uint32_t cp)
    {
        // Convert UTF-8 to UTF-16 (wide characters)
        int len = MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, NULL, 0);
        if (len == 0) return ""; // Error

        std::wstring wstr(len, 0);
        MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, &wstr[0], len);

        // Convert UTF-16 to CP1251
        // The length includes the null terminator, subtract for string size
        int len2 = WideCharToMultiByte(cp, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
        if (len2 == 0) return ""; // Error

        std::string cp_str(len2, 0);
        WideCharToMultiByte(cp, 0, wstr.c_str(), -1, &cp_str[0], len2, NULL, NULL);

        // Remove the null terminator from the result string for standard C++ string usage
        cp_str.resize(len2 - 1);

        return cp_str;
    }

    void remove_broken_symbols(std::string& str)
    {
        // Use the erase-remove idiom
        str.erase(std::remove_if(str.begin(), str.end(),
            [](unsigned char c) {
                // Define your condition for removal here.
                return !std::isprint(c);
            }),
            str.end());
    }

    // steeled from https://github.com/Gratt-5r2/zFont
    int32_t guess_game_encoding()
    {
        int32_t arrow_id = parser->GetIndex("ItAmArrow");
        if (arrow_id == -1) arrow_id = parser->GetIndex("ItRw_Arrow");
        if (arrow_id == -1) arrow_id = parser->GetIndex("ItAm_Arrow");

        if (arrow_id != -1)
        {
            oCItem* item = new oCItem();
            item->InitByScript(arrow_id, 0);
            std::string item_name = (const char*)item->name;
            item->Release();

            if (item_name == utf8_to_cp(std::string("Šíp"),1250))           return 1250; // cz
            if (item_name == utf8_to_cp(std::string("Nyílvessző"),1250))    return 1250; // hu
            if (item_name == utf8_to_cp(std::string("Strzała"),1250))       return 1250; // pl
            if (item_name == utf8_to_cp(std::string("Стрела"),1251))        return 1251; // ru
            if (item_name == utf8_to_cp(std::string("Стріла"),1251))        return 1251; // uk
            if (item_name == utf8_to_cp(std::string("Pfeil"),1252))         return 1252; // de
            if (item_name == utf8_to_cp(std::string("Arrow"),1252))         return 1252; // en
            if (item_name == utf8_to_cp(std::string("Flèche"),1252))        return 1252; // fr
            if (item_name == utf8_to_cp(std::string("Freccia"),1252))       return 1252; // it
            if (item_name == utf8_to_cp(std::string("Flecha"),1252))        return 1252; // es
            if (item_name == utf8_to_cp(std::string("Ok"),1254))            return 1254; // tr
        }

        return -1;
    }
}