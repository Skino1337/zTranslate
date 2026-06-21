namespace GOTHIC_NAMESPACE
{
    struct PluginIniData
    {
        std::string     section_text_section_name           = "TEXT";
        bool            section_text_enable                 = true;
        std::string     section_text_lang;


        std::string     section_audio_section_name          = "AUDIO";
        std::string     section_audio_lang;

        std::string     section_other_section_name          = "OTHER";
        float           section_other_dialog_duration_coef   = 1.0f;

        PluginIniData() = default;
    };

    struct TextData
    {
        std::string folder_name;
        std::string language_name = "Native";
        std::string language_name_native = "Native";
        std::string menu_language = "Language";
        std::string menu_language_description = "Change game language";
        std::string menu_language_select_text_language = "Text";
        std::string menu_language_select_text_language_description = "Select game text language";
        std::string menu_language_select_audio_language = "Audio";
        std::string menu_language_select_audio_language_description = "Select game audio language";
        std::string menu_language_back = "Back";

        bool is_translate_text = false;
        std::string full_path_to_translate_file;

        TextData() = default;
    };

    struct AudioData
    {
        std::string folder_name;
        std::map<std::string, std::string> file_path_map;

        bool is_translate_audio = false;


        AudioData() = default;
    };

    struct LanguageData
    {
        TextData text_data = TextData();
        AudioData audio_data = AudioData();

        LanguageData() = default;
    };

    std::map<int, LanguageData> language_data_map;

    int text_choice_index = 0;
    std::vector<std::string> text_language_for_item_vec;

    int audio_choice_index = 0;
    std::vector<std::string> audio_language_for_item_vec;

    LanguageData* selected_text_language_data;
    LanguageData* selected_audio_language_data;


    std::filesystem::path current_folder_path;
    std::filesystem::path plugin_ini_file_path = "System/zTranslate.ini";
    std::filesystem::path translate_file_path = "translate.json";
    std::filesystem::path plugin_folder_path = "zTranslate";
    std::filesystem::path text_folder_path = "Text";
    std::filesystem::path audio_folder_path = "Audio";

    PluginIniData plugin_ini_data;


    // std::ofstream log;
    // std::string log_file_path = "log.txt";
    // bool is_log_init = false;

    void init_log()
    {
        // if (!is_log_init)
        //     std::remove(log_file_path.c_str());
        //
        // if (!log.is_open())
        //     log.open(log_file_path, std::ios::app);
        //
        // if (log.is_open())
        // {
        //     log << std::unitbuf;
        //     is_log_init = true;
        // }
        // else
        // {
        //     is_log_init = false;
        // }
    }

    void save_plugin_ini_file()
    {
        std::string temp_string;
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2);

        ss << "[" << plugin_ini_data.section_text_section_name << "]" << std::endl;
        temp_string = plugin_ini_data.section_text_enable ? "true" : "false";
        ss << "enable" << " = " << temp_string << std::endl;
        temp_string = plugin_ini_data.section_text_lang == "_" ? "" : plugin_ini_data.section_text_lang;
        ss << "lang" << " = " << temp_string << std::endl;

        ss << std::endl;

        ss << "[" << plugin_ini_data.section_audio_section_name << "]" << std::endl;
        temp_string = plugin_ini_data.section_audio_lang == "_" ? " " : plugin_ini_data.section_audio_lang;
        ss << "lang" << " = " << temp_string << std::endl;

        ss << std::endl;

        ss << "[" << plugin_ini_data.section_other_section_name << "]" << std::endl;
        ss << "dialog_duration_coefficient" << " = " << plugin_ini_data.section_other_dialog_duration_coef << std::endl;

        std::ofstream file;
        file.open(plugin_ini_file_path, std::ios::out);
        if (file.is_open())
        {
            file << ss.str();
            file.flush();
            file.close();
        }
    }

    void read_plugin_ini_file()
    {
        std::ifstream file(plugin_ini_file_path);
        if (!file.is_open())
        {
            // log << "cant open ini file: " << ini_file_path << std::endl;
            return;
        }

        std::string section;
        std::string line;
        while (std::getline(file, line))
        {
            if (line.empty())
                continue;

            if (line[0] == ';')
                continue;

            if (line[0] == '[')
            {
                section = line.substr(1, line.size() - 2);
                continue;
            }

            auto string_splited_vector = split_string(line, "=");
            if (string_splited_vector.size() == 1 && line[line.size() - 1] == '=')
                string_splited_vector.emplace_back(" ");

            if (string_splited_vector.size() != 2)
                continue;

            std::string chars_to_delete = "!@#$%^&*()_+"; // ???

            std::string variable = string_splited_vector[0];
            std::erase_if(variable,
              [](unsigned char c) {
                  return std::isspace(c) || !std::isprint(c);
              });

            std::ranges::transform(variable, variable.begin(), [] (unsigned char c)
            {
                return std::tolower(c);
            });

            std::string value = string_splited_vector[1];
            std::erase_if(value,
              [](unsigned char c) {
                  return std::isspace(c) || !std::isprint(c);
              });

            std::ranges::transform(value, value.begin(), [] (unsigned char c)
            {
                return std::tolower(c);
            });

            if (section == plugin_ini_data.section_text_section_name)
            {
                if (variable == "lang")
                {
                    if (value.empty())
                    {
                        value = "_";
                    }
                    plugin_ini_data.section_text_lang = value;
                }
                else if (variable == "enable")
                {
                    if (value == "true")
                    {
                        plugin_ini_data.section_text_enable = true;
                    }
                    if (value == "false")
                    {
                        plugin_ini_data.section_text_enable = false;
                    }
                }
            }
            else if (section == plugin_ini_data.section_audio_section_name)
            {
                if (variable == "lang")
                {
                    if (value.empty())
                        value = "_";
                    plugin_ini_data.section_audio_lang = value;
                }
            }
            else if (section == plugin_ini_data.section_other_section_name)
            {
                float value_float = 0.0f;
                if (variable == "dialog_duration_coefficient")
                {
                    value_float = 1.0f;
                    try {
                        value_float = std::stof(value);
                    } catch (...) {}
                    value_float = std::clamp(value_float, 0.5f, 10.0f);
                    plugin_ini_data.section_other_dialog_duration_coef = value_float;
                }
            }

            // log << "variable: " << variable << ", value: " << value << std::endl;
        }

        file.close();
    }

    bool change_language_text_from_options(int index)
    {
        if (!language_data_map.contains(index))
            return false;

        text_choice_index = index;
        plugin_ini_data.section_text_lang = language_data_map[index].text_data.folder_name;

        save_plugin_ini_file();

        return true;
    }

    bool change_language_audio_from_options(int index)
    {
        if (!language_data_map.contains(index))
            return false;

        audio_choice_index = index;
        plugin_ini_data.section_audio_lang = language_data_map[index].audio_data.folder_name;

        save_plugin_ini_file();

        return true;
    }

    void save_empty_language_json_file(const std::string & file_path, const std::string & language_folder_name)
    {
        TinyJSON::write_options options = {};
        options.throw_exception = false;
        options.write_formating = TinyJSON::formating::indented;
        options.byte_order_mark = TinyJSON::write_options::utf8;

        auto json = new TinyJSON::TJValueObject();

        TextData language_data;
        json->set_string("language_name", language_folder_name.c_str());
        json->set_string("language_name_native", language_data.language_name_native.c_str());
        json->set_string("menu_language", language_data.menu_language.c_str());
        json->set_string("menu_language_description", language_data.menu_language_description.c_str());
        json->set_string("menu_language_select_text_language", language_data.menu_language_select_text_language.c_str());
        json->set_string("menu_language_select_text_language_description", language_data.menu_language_select_text_language_description.c_str());
        json->set_string("menu_language_select_audio_language", language_data.menu_language_select_audio_language.c_str());
        json->set_string("menu_language_select_audio_language_description", language_data.menu_language_select_audio_language_description.c_str());
        json->set_string("menu_language_back", language_data.menu_language_back.c_str());

        TinyJSON::TJ::write_file(file_path.c_str(), *json, options);
    }

    bool read_language_json_file(const std::string & file_path, LanguageData & language_data)
    {
        TinyJSON::parse_options options = {};
        options.throw_exception = false;

        auto json = TinyJSON::TJ::parse_file(file_path.c_str(), options);
        if (!json || !json->is_object())
            return false;

        auto json_object = dynamic_cast<TinyJSON::TJValueObject*>(json);

        auto json_value = json_object->try_get_value("language_name", false);
        if (json_value && json_value->is_string())
            language_data.text_data.language_name = json_value->get_string();

        json_value = json_object->try_get_value("language_name_native", false);
        if (json_value && json_value->is_string())
            language_data.text_data.language_name_native = json_value->get_string();

        json_value = json_object->try_get_value("menu_language", false);
        if (json_value && json_value->is_string())
            language_data.text_data.menu_language = json_value->get_string();

        json_value = json_object->try_get_value("menu_language_description", false);
        if (json_value && json_value->is_string())
            language_data.text_data.menu_language_description = json_value->get_string();

        json_value = json_object->try_get_value("menu_language_select_text_language", false);
        if (json_value && json_value->is_string())
            language_data.text_data.menu_language_select_text_language = json_value->get_string();

        json_value = json_object->try_get_value("menu_language_select_text_language_description", false);
        if (json_value && json_value->is_string())
            language_data.text_data.menu_language_select_text_language_description = json_value->get_string();

        json_value = json_object->try_get_value("menu_language_select_audio_language", false);
        if (json_value && json_value->is_string())
            language_data.text_data.menu_language_select_audio_language = json_value->get_string();

        json_value = json_object->try_get_value("menu_language_select_audio_language_description", false);
        if (json_value && json_value->is_string())
            language_data.text_data.menu_language_select_audio_language_description = json_value->get_string();

        json_value = json_object->try_get_value("menu_language_back", false);
        if (json_value && json_value->is_string())
            language_data.text_data.menu_language_back = json_value->get_string();

        return true;
    }

    void scan_translate_subfolder(const std::string & folder_path, LanguageData & language_data)
    {
        for (const auto & entry : std::filesystem::directory_iterator(folder_path))
        {
            auto path = entry.path().string();
            auto filename = entry.path().filename().string();

            if (entry.is_directory())
            {
                if (string_compare_case_insensitive(filename, text_folder_path.string()))
                {
                    language_data.text_data.full_path_to_translate_file =
                        (plugin_folder_path / language_data.text_data.folder_name / filename / translate_file_path).string();
                    //  = lang_translate_file_path.string();
                    if (std::filesystem::exists(language_data.text_data.full_path_to_translate_file))
                    {
                        language_data.text_data.is_translate_text = true;
                        // log << "found translate file: " << language_data.full_path_to_translate_file << std::endl;
                        // log.flush();
                    }
                    else
                    {
                        // log << "NOT found translate file: " << language_data.full_path_to_translate_file << std::endl;
                        // log.flush();
                    }
                }
                else if (string_compare_case_insensitive(filename, audio_folder_path.string()))
                {
                    // log << "filename: " << filename << std::endl;
                    // log << "path: " << path << std::endl;
                    // log.flush();

                    for (const auto & audio_entry : std::filesystem::recursive_directory_iterator(path))
                    {
                        if (!audio_entry.is_regular_file())
                            continue;

                        // DIA_ADDON_SATURAS_HALLO_WEIßTDU_14_00
                        auto audio_file_path_utf8 = audio_entry.path().u8string();
                        auto audio_file_path = std::string(audio_file_path_utf8.begin(), audio_file_path_utf8.end());

                        auto audio_file_name_utf8 = audio_entry.path().filename().u8string();
                        auto audio_file_name = std::string(audio_file_name_utf8.begin(), audio_file_name_utf8.end());

                        auto audio_file_extension_utf8 = audio_entry.path().extension().u8string();
                        auto audio_file_extension = std::string(audio_file_extension_utf8.begin(), audio_file_extension_utf8.end());

                        if (string_compare_case_insensitive(audio_file_extension, ".wav"))
                        {
                            audio_file_name.erase(audio_file_name.length() - 4);
                            language_data.audio_data.file_path_map[audio_file_name] = audio_file_path;
                            language_data.audio_data.is_translate_audio = true;

                            // log << "audio_file_path: " << audio_file_path << std::endl;
                            // log << "audio_file_name: " << audio_file_name << std::endl;
                            // log << "audio_file_extension: " << audio_file_extension << std::endl;
                            // log.flush();
                        }
                    }
                }
            }
            else if (entry.is_regular_file())
            {
                if (string_compare_case_insensitive(filename, "language.json"))
                {
                    read_language_json_file(path, language_data);
                }

                // log << "scan_translate_subfolder, found language file: " << path << std::endl;
                // log.flush();
            }
        }
    }

    void create_language_template_if_need(const std::string & lang_folder_name)
    {
        const auto language_folder_path = plugin_folder_path / lang_folder_name;

        const auto text_path = language_folder_path / "Text";
        if (!std::filesystem::exists(text_path))
            std::filesystem::create_directories(text_path);

        const auto audio_path = language_folder_path / audio_folder_path;
        if (!std::filesystem::exists(audio_path))
            std::filesystem::create_directories(audio_path);

        const auto video_path = language_folder_path / "Video";
        if (!std::filesystem::exists(video_path))
            std::filesystem::create_directories(video_path);

        const auto texture_path = language_folder_path / "Texture";
        if (!std::filesystem::exists(texture_path))
            std::filesystem::create_directories(texture_path);

        const auto subtitles_path = language_folder_path / "Subtitles";
        if (!std::filesystem::exists(subtitles_path))
            std::filesystem::create_directories(subtitles_path);

        const auto language_json_path = language_folder_path / "language.json";
        if (!std::filesystem::exists(language_json_path))
            save_empty_language_json_file(language_json_path.string(), lang_folder_name);
    }

    void scan_translate_folder()
    {
        create_language_template_if_need("_");

        std::vector<std::string> folder_name_sorted_vec;
        for (const auto & entry : std::filesystem::directory_iterator(plugin_folder_path))
        {
            if (!entry.is_directory())
                continue;

            auto folder_name = entry.path().filename().string();
            folder_name_sorted_vec.push_back(folder_name);
        }

        // no folder for lang
        if (std::ranges::find(folder_name_sorted_vec, plugin_ini_data.section_text_lang) == folder_name_sorted_vec.end())
        {
            create_language_template_if_need(plugin_ini_data.section_text_lang);
            folder_name_sorted_vec.push_back(plugin_ini_data.section_text_lang);
        }

        std::ranges::sort(folder_name_sorted_vec);

        for (auto & folder_name : folder_name_sorted_vec)
        {
            auto folder_path = plugin_folder_path / folder_name;

            auto language_data = LanguageData();
            language_data.text_data.folder_name = folder_name;
            language_data.audio_data.folder_name = folder_name;

            // log << "scan_translate_folder, found subfolder, folder_path: " << folder_path << std::endl;
            // log << "scan_translate_folder, found subfolder, folder_name: " << folder_name << std::endl;
            // log.flush();

            scan_translate_subfolder(folder_path.string(), language_data);

            language_data_map[language_data_map.size()] = language_data;
        }
    }

    void plugin_init()
    {
        init_log();

        current_folder_path = std::filesystem::current_path();
        plugin_ini_file_path = current_folder_path / plugin_ini_file_path;
        plugin_folder_path = current_folder_path / plugin_folder_path;

        // create zTranslate folder
        if (!std::filesystem::exists(plugin_folder_path))
            std::filesystem::create_directories(plugin_folder_path);

        // create zTranslate.ini file
        if (!std::filesystem::exists(plugin_ini_file_path))
            save_plugin_ini_file();

        read_plugin_ini_file();
        scan_translate_folder();

        selected_text_language_data = & language_data_map[0];
        selected_audio_language_data = & language_data_map[0];

        for (auto & [id, language_data] : language_data_map)
        {
            if (language_data.text_data.folder_name == plugin_ini_data.section_text_lang)
            {
                text_choice_index = id;
                selected_text_language_data = & language_data;
            }
            if (language_data.audio_data.folder_name == plugin_ini_data.section_audio_lang)
            {
                audio_choice_index = id;
                selected_audio_language_data = & language_data;
            }
        }

        // --- Text

        // set name for _ lang (Native)
        language_data_map[0].text_data.language_name = selected_text_language_data->text_data.language_name_native;
        for (const auto & [id, language_data] : language_data_map)
        {
            text_language_for_item_vec.push_back(language_data.text_data.language_name);
        }


        // --- Text

        // --- Audio

        language_data_map[0].audio_data.is_translate_audio = true;
        for (const auto & [id, language_data] : language_data_map)
        {
            if (!language_data.audio_data.is_translate_audio)
                continue;

            audio_language_for_item_vec.push_back(language_data.text_data.language_name);
        }

        // --- Audio


        // if (std::filesystem::exists(selected_text_language_data->text_data.full_path_to_translate_file))
        // {
        //     std::remove(selected_text_language_data->text_data.full_path_to_translate_file.c_str());
        // }

        // log << "hook_game_init" << std::endl;
        // log << "plugin_ini_data.section_text_lang: " << plugin_ini_data.section_text_lang << std::endl;
        // log << "plugin_ini_data.section_audio_lang: " << plugin_ini_data.section_audio_lang << std::endl;
        // log << "plugin_ini_data.section_other_dialog_duration_coef: " << plugin_ini_data.section_other_dialog_duration_coef << std::endl;
    }
}