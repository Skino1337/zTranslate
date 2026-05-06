# Introduction

Plugin for translate gothic games.\
Text: support\
Audio: support\
Texture/Image: Not support yet\
Subtitles (for video): Not support yet\
Video: Not support yet\
Fonts: Not support yet

# Requirements

Union 1.0m+\
Some plugin for change fonts with TTF.\
like this: https://github.com/SaiyansKing/Gothic-TTF \
or like this: https://github.com/Gratt-5r2/zFont

# Plugin installation

Just placing it in `[GAME_FOLDER]/System/autorun` directory.

# Configuration

In TTF plugin, need change encoding for utf-8.

In path `[GAME_FOLDER]/System/zTranslate.ini` located plugin config file.\
In path `[GAME_FOLDER]/zTranslate` located plugin translates folder.

After first start, if needed, plugin automatically create all files and folders.

`_` - language folder for native language.

## Config file
```ini
[TEXT]
lang =

[AUDIO]
lang =

[OTHER]
dialog_duration_coefficient = 1.00
```

## JSON language file
```json
{
  "language_name": "_",
  "language_name_native": "Native",
  "menu_language": "Language",
  "menu_language_description": "Change game language",
  "menu_language_select_text_language": "Text",
  "menu_language_select_text_language_description": "Select game text language",
  "menu_language_select_audio_language": "Audio",
  "menu_language_select_audio_language_description": "Select game audio language",
  "menu_language_back": "Back"
}
```

## JSON text translate file
### Header
```json
{
    "virtual_machine_hash": "11242701",
    "translate_name": "",
    "script_encoding": "1252",
    "min_translate_value": "100"
}
```

### Data
```json
{
    "original_string": "Raven´s Tagebuch, gefunden in der Minentalburg.",
    "translate_source": "[RAW_STRING] RAVENSTAGEBUCH_01 -> C_ITEM -> DESCRIPTION",
    "translate_value": "100",
    "translated_string": "Дневник Ворона, найден в замке Долины Рудников."
}
```


# Add new translate

In plugin config file, in `[TEXT]` section change parameter `lang = ` to `lang = YOU_LANGUAGE`.\
For example `text=pl` or `text=de`\
If plugin do not find this folder, it creates language template, with translate file automatically.

## Text translate
Go to `[GAME_FOLDER]/zTranslate/[YOU_LANGUAGE]/Text` and just edit `translate.json` file

## Audio translate
Just put in some audio files in `[GAME_FOLDER]/zTranslate/[YOU_LANGUAGE]/Audio` folder.\
Audio files need have same name. Like `DIA_331_WALDTRAUT_SCHMUCK_15_01.WAV`

Audio format must be:
```
Format                         : PCM
Format settings                : Little / Signed
Bit rate mode                  : Constant
Channel(s)                     : 1 channel
Sampling rate                  : 44.1 kHz
Bit depth                      : 16 bits
```