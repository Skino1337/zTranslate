namespace GOTHIC_NAMESPACE
{

    // // https://github.com/TheSuperHackers/miles-sdk-stub/blob/master/mss/mss.h
    // // https://www.radgametools.com/msshist.htm

    struct SoundData
    {
        std::string name;
        bool is_replaced = false;

        float original_duration = 0.0f;
        void* original_ptr = nullptr;
        int original_size = 0;

        float duration = 0.0f;
        void* ptr = nullptr;
        unsigned int size = 0;
    };

    std::map<std::string, SoundData> sound_data_map;

    zCWaveData* get_wave_data_from_resource(zCResource* resource)
    {
        auto cache_state = resource->cacheState & 3;
        if (cache_state != zRES_CACHED_IN)
            return nullptr;

        auto sound_mss = resource->CastTo<zCSndFX_MSS>();
        if (!sound_mss)
            return nullptr;

        auto v14 = *(DWORD_PTR*)(*((DWORD_PTR*)sound_mss + 30 /*0x1E*/) + 4 * *((DWORD_PTR*)sound_mss + 27 /*0x1B*/));
        if (!v14)
            return nullptr;

        auto v26 = *(DWORD_PTR*)(*(DWORD_PTR*)(v14 + 12 /*0x0C*/) + 4 * *((DWORD_PTR*)sound_mss + 28 /*0x1C*/));
        if (!v26)
            return nullptr;

        auto sound_frame = reinterpret_cast<zCSndFrame*>(v26);
        if (!sound_frame)
            return nullptr;

        auto wave_data = sound_frame->wav;
        if (!wave_data)
            return nullptr;

        return wave_data;
    }

    bool read_wav_file_to_sound_data(const std::string & file_path, SoundData & sound_data)
    {
        if (!std::filesystem::exists(file_path))
            return false;

        std::ifstream file(file_path, std::ios::binary);
        if (!file.is_open())
            return false;

        if (!file.good())
            return false;

        file.seekg (0, std::ios::end);
        const size_t size = file.tellg();
        file.seekg (0, std::ios::beg);

        if (size < 12) // min header
            return false;

        void* ptr = malloc(size);

        file.read(static_cast<char*>(ptr), size);
        file.close();

        if (file.gcount() != size)
            return false;

        sound_data.size = size;
        sound_data.ptr = ptr;
        sound_data.duration = (sound_data.size - 12) / 88.2f; // 88.2 = pytes per seconds

        return true;
    }

    void hook_pre_cache_out(zCResource* resource)
    {
        zCWaveData* wave_data = get_wave_data_from_resource(resource);
        if (!wave_data)
            return;

        std::string sound_file_name = wave_data->fileName.ToChar();
        std::transform(sound_file_name.begin(), sound_file_name.end(), sound_file_name.begin(), ::toupper);

        if (!sound_data_map.contains(sound_file_name))
            return;

        auto & sound_data = sound_data_map[sound_file_name];

        // log << std::endl;
        // log << "hook_pre_cache_out" << std::endl;
        // log << "sound_file_name: " << sound_file_name << std::endl;
        // log << "sound_data.original_ptr: " << sound_data.original_ptr << std::endl;
        // log << "sound_data.original_size: " << sound_data.original_size << std::endl;
        // log << "sound_data.original_duration: " << sound_data.original_duration << std::endl;
        // log << "sound_data.ptr: " << sound_data.ptr << std::endl;
        // log << "sound_data.size: " << sound_data.size << std::endl;
        // log << "sound_data.duration: " << sound_data.duration << std::endl;

        wave_data->memAddr = sound_data.original_ptr;
        wave_data->size = sound_data.original_size;
        wave_data->msLength = sound_data.original_duration;

        free(sound_data.ptr);
        sound_data.ptr = nullptr;
        sound_data.size = 0;
        sound_data.duration = 0.0f;
        sound_data.is_replaced = false;
    }

    void hook_post_cache_in(zCResource* resource)
    {
        auto wave_data = get_wave_data_from_resource(resource);
        if (!wave_data)
            return;

        std::string sound_file_name = wave_data->fileName.ToChar();
        if (sound_file_name.length() > 4)
            sound_file_name.erase(sound_file_name.length() - 4);
        std::transform(sound_file_name.begin(), sound_file_name.end(), sound_file_name.begin(), ::toupper);
        // auto replaced_sound_file_name_path = audio_folder_path / sound_file_name;
        // auto replaced_sound_file_name_path = plugin_folder_path / selected_language_data.language_name / audio_folder_path / sound_file_name;

        // log << "sound_file_name: " << sound_file_name << std::endl;
        // log.flush();

        if (!selected_audio_language_data->audio_data.file_path_map.contains(sound_file_name))
        {
            wave_data->msLength = wave_data->msLength * plugin_ini_data.section_other_dialog_duration_coef;
            return;
        }

        auto sound_data = SoundData();
        if (sound_data_map.contains(sound_file_name))
            sound_data = sound_data_map[sound_file_name];

        if (sound_data.is_replaced)
            return;

        // std::string file_path = (audio_folder_path / sound_file_name).string();
        // auto replaced_sound_file_name_path = (plugin_folder_path / selected_audio_language_data->audio_data.folder_name / audio_folder_path / sound_file_name).string();
        auto & replaced_sound_file_name_path = selected_audio_language_data->audio_data.file_path_map[sound_file_name];
        if (!read_wav_file_to_sound_data(replaced_sound_file_name_path, sound_data))
            return;

        sound_data.original_ptr = wave_data->memAddr;
        sound_data.original_size = wave_data->size;
        sound_data.original_duration = wave_data->msLength;

        wave_data->memAddr = sound_data.ptr;
        wave_data->size = sound_data.size;
        wave_data->msLength = sound_data.duration * plugin_ini_data.section_other_dialog_duration_coef;

        sound_data.is_replaced = true;

        sound_data_map[sound_file_name] = sound_data;

        // log << std::endl;
        // log << "hook_post_cache_in" << std::endl;
        // log << "sound_file_name: " << sound_file_name << std::endl;
        // log << "sound_data.original_ptr: " << sound_data.original_ptr << std::endl;
        // log << "sound_data.original_size: " << sound_data.original_size << std::endl;
        // log << "sound_data.original_duration: " << sound_data.original_duration << std::endl;
        // log << "sound_data.ptr: " << sound_data.ptr << std::endl;
        // log << "sound_data.size: " << sound_data.size << std::endl;
        // log << "sound_data.duration: " << sound_data.duration << std::endl;
        //
        // log.flush();
    }
}
