namespace GOTHIC_NAMESPACE
{
    struct PluginIniData
    {
        std::string text;
        std::string audio;

        // std::string translate_file;

        PluginIniData() = default;

        bool is_valid() const
        {
            return !text.empty(); // && !translate_file.empty();
        }
    };

    struct LanguageData
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

        bool is_translate_audio = false;

        LanguageData() = default;
    };

    // std::ofstream log;

    std::filesystem::path current_folder_path;
    std::filesystem::path plugin_ini_file_path = "System/zTranslate.ini";
    std::filesystem::path translate_file_path = "translate.json";
    std::filesystem::path plugin_folder_path = "zTranslate";
    PluginIniData plugin_ini_data;
    LanguageData selected_language_data;
    std::map<int, LanguageData> language_data_map;
    std::string text_language_for_item;
    std::string audio_language_for_item;
    int text_choice_option_start = 0;
    int audio_choice_option_start = 0;

    void init_log()
    {
        // std::remove("log.txt");
        // log.open("log.txt", std::ios::app);
        // while (!log.is_open())
        //     std::this_thread::sleep_for(std::chrono::seconds(1));
        //
        // log << std::unitbuf; // Enable auto-flushing
    }

    void save_ini_file(const std::string & ini_file_path, std::vector<std::pair<std::string, std::string>> & pair_vec)
    {
        std::ofstream file;
        file.open(ini_file_path, std::ios::out);
        if (!file.is_open())
        {
            // log << "cant open (to save) ini file: " << ini_file_path << std::endl;
            // log.flush();
            return;
        }

        for (auto const& [variable, value] : pair_vec)
        {
            file << variable << "=" << value << std::endl;
        }

        file.flush();
        file.close();
    }

    void read_ini_file(const std::string & ini_file_path, std::vector<std::pair<std::string, std::string>> & pair_vec)
    {
        std::ifstream file(ini_file_path);
        if (!file.is_open())
        {
            // log << "cant open ini file: " << ini_file_path << std::endl;
            return;
        }

        std::string line;
        while (std::getline(file, line))
        {
            if (line.empty())
                continue;

            if (line[0] == ';')
                continue;

            auto string_splited_vector = split_string(line, "=");
            if (string_splited_vector.size() != 2)
                continue;

            std::string chars_to_delete = "!@#$%^&*()_+"; // ???

            std::string variable = string_splited_vector[0];
            std::erase_if(variable,
              [](unsigned char c) {
                  return std::isspace(c) || !std::isprint(c);
              });

            std::string value = string_splited_vector[1];
            std::erase_if(value,
              [](unsigned char c) {
                  return std::isspace(c) || !std::isprint(c);
              });

            pair_vec.emplace_back(variable, value);

            // log << "variable: " << variable << ", value: " << value << std::endl;
        }

        file.close();
    }

    void save_plugin_ini_file()
    {
        std::ofstream file;
        file.open(plugin_ini_file_path.string(), std::ios::out);
        if (!file.is_open())
        {
            // log << "cant open (to save) ini file: " << plugin_ini_file_path << std::endl;
            return;
        }

        const auto text = plugin_ini_data.text == "_" ? "" : plugin_ini_data.text;
        file << "text=" << text << std::endl;
        file << "audio=" << plugin_ini_data.audio << std::endl;
        // file << "translate_file=" << plugin_ini_data.translate_file << std::endl;

        file.flush();
        file.close();
    }

    void read_plugin_ini_file()
    {
        std::vector<std::pair<std::string, std::string>> pair_vec;
        read_ini_file(plugin_ini_file_path.string(), pair_vec);

        for (auto const& [variable, value] : pair_vec)
        {
            if (variable == "text")
            {
                plugin_ini_data.text = value;
            }
            if (variable == "audio")
            {
                plugin_ini_data.audio = value;
            }
            else if (variable == "translate_file")
            {
                // plugin_ini_data.translate_file = value;
            }
        }

        if (plugin_ini_data.text.empty())
            plugin_ini_data.text = "_";
    }

    void change_language_from_options(int index)
    {
        if (!language_data_map.contains(index))
            return;

        plugin_ini_data.text = language_data_map[index].folder_name;

        save_plugin_ini_file();
    }

    void save_empty_language_json_file(const std::string & file_path, const std::string & language_folder_name)
    {
        TinyJSON::write_options options = {};
        options.throw_exception = false;
        options.write_formating = TinyJSON::formating::indented;
        options.byte_order_mark = TinyJSON::write_options::utf8;

        auto json = new TinyJSON::TJValueObject();

        LanguageData language_data;
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
            language_data.language_name = json_value->get_string();

        json_value = json_object->try_get_value("language_name_native", false);
        if (json_value && json_value->is_string())
            language_data.language_name_native = json_value->get_string();

        json_value = json_object->try_get_value("menu_language", false);
        if (json_value && json_value->is_string())
            language_data.menu_language = json_value->get_string();

        json_value = json_object->try_get_value("menu_language_description", false);
        if (json_value && json_value->is_string())
            language_data.menu_language_description = json_value->get_string();

        json_value = json_object->try_get_value("menu_language_select_text_language", false);
        if (json_value && json_value->is_string())
            language_data.menu_language_select_text_language = json_value->get_string();

        json_value = json_object->try_get_value("menu_language_select_text_language_description", false);
        if (json_value && json_value->is_string())
            language_data.menu_language_select_text_language_description = json_value->get_string();

        json_value = json_object->try_get_value("menu_language_select_audio_language", false);
        if (json_value && json_value->is_string())
            language_data.menu_language_select_audio_language = json_value->get_string();

        json_value = json_object->try_get_value("menu_language_select_audio_language_description", false);
        if (json_value && json_value->is_string())
            language_data.menu_language_select_audio_language_description = json_value->get_string();

        json_value = json_object->try_get_value("menu_language_back", false);
        if (json_value && json_value->is_string())
            language_data.menu_language_back = json_value->get_string();

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
                if (string_compare_case_insensitive(filename, "text"))
                {
                    language_data.full_path_to_translate_file =
                        (plugin_folder_path / language_data.folder_name / filename / translate_file_path).string();
                    //  = lang_translate_file_path.string();
                    if (std::filesystem::exists(language_data.full_path_to_translate_file))
                    {
                        language_data.is_translate_text = true;
                        // log << "found translate file: " << language_data.full_path_to_translate_file << std::endl;
                        // log.flush();
                    }
                    else
                    {
                        // log << "NOT found translate file: " << language_data.full_path_to_translate_file << std::endl;
                        // log.flush();
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

        const auto audio_path = language_folder_path / "Audio";
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
        if (std::ranges::find(folder_name_sorted_vec, plugin_ini_data.text) == folder_name_sorted_vec.end())
        {
            create_language_template_if_need(plugin_ini_data.text);
            folder_name_sorted_vec.push_back(plugin_ini_data.text);
        }

        std::ranges::sort(folder_name_sorted_vec);

        for (auto & folder_name : folder_name_sorted_vec)
        {
            auto folder_path = plugin_folder_path / folder_name;

            auto language_data = LanguageData();
            language_data.folder_name = folder_name;

            // log << "scan_translate_folder, found subfolder, folder_path: " << folder_path << std::endl;
            // log << "scan_translate_folder, found subfolder, folder_name: " << folder_name << std::endl;
            // log.flush();

            scan_translate_subfolder(folder_path.string(), language_data);

            language_data_map[language_data_map.size()] = language_data;
        }
    }

    void hook_game_init()
    {
        init_log();

        // log << "hook_game_init" << std::endl;

        // logv2("hi" << "");

        current_folder_path = std::filesystem::current_path();
        plugin_ini_file_path = current_folder_path / plugin_ini_file_path;
        plugin_folder_path = current_folder_path / plugin_folder_path;

        // language_data_map[0] = LanguageData();

        // create zTranslate folder
        if (!std::filesystem::exists(plugin_folder_path))
            std::filesystem::create_directories(plugin_folder_path);

        // create zTranslate.ini file
        if (!std::filesystem::exists(plugin_ini_file_path))
            save_plugin_ini_file();

        read_plugin_ini_file();
        scan_translate_folder();

        for (const auto & [id, language_data] : language_data_map)
        {
            if (language_data.folder_name == plugin_ini_data.text)
            {
                text_choice_option_start = id;
                selected_language_data = language_data;
                break;
            }
        }

        language_data_map[0].language_name = selected_language_data.language_name_native;

        for (const auto & [id, language_data] : language_data_map)
        {
            text_language_for_item += text_language_for_item.empty() ? "" : "|";
            text_language_for_item += language_data.language_name;
        }
    }
}