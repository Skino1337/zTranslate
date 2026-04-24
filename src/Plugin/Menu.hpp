namespace GOTHIC_NAMESPACE
{
    std::string MENU_LANGUAGE = "MENU_LANGUAGE";
    std::string MENUITEM_LANGUAGE = "MENUITEM_LANGUAGE";
    std::string MENUITEM_LANGUAGE_TEXT_CHOICE = "MENUITEM_LANGUAGE_TEXT_CHOICE";
    std::string MENUITEM_LANGUAGE_AUDIO_CHOICE = "MENUITEM_LANGUAGE_AUDIO_CHOICE";
    std::string main_menu_name = "";
    std::string current_menu_name = "";
    int menu_y_offset = 0;
    int main_menu_exit_y = -1;
    zCMenuItem* menu_item_language_text_choice = nullptr;
    zCMenuItem* menu_item_language_audio_choice = nullptr;
    bool is_setup_toggle = false;

    std::string get_menu_item_flags(const zCMenuItem* menu_item)
    {
        std::string menu_item_flags;

        if (menu_item->m_parItemFlags & IT_UNDEF)
            menu_item_flags += " | IT_UNDEF";

        if (menu_item->m_parItemFlags & IT_CHROMAKEYED)
            menu_item_flags += " | IT_CHROMAKEYED";

        if (menu_item->m_parItemFlags & IT_TRANSPARENT)
            menu_item_flags += " | IT_TRANSPARENT";

        if (menu_item->m_parItemFlags & IT_SELECTABLE)
            menu_item_flags += " | IT_SELECTABLE";

        if (menu_item->m_parItemFlags & IT_MOVEABLE)
            menu_item_flags += " | IT_MOVEABLE";

        if (menu_item->m_parItemFlags & IT_TXT_CENTER)
            menu_item_flags += " | IT_TXT_CENTER";

        if (menu_item->m_parItemFlags & IT_DISABLED)
            menu_item_flags += " | IT_DISABLED";

        if (menu_item->m_parItemFlags & IT_FADE)
            menu_item_flags += " | IT_FADE";

        if (menu_item->m_parItemFlags & IT_EFFECTS_NEXT)
            menu_item_flags += " | IT_EFFECTS_NEXT";

        if (menu_item->m_parItemFlags & IT_ONLY_OUT_GAME)
            menu_item_flags += " | IT_ONLY_OUT_GAME";

        if (menu_item->m_parItemFlags & IT_ONLY_IN_GAME)
            menu_item_flags += " | IT_ONLY_IN_GAME";

        if (menu_item->m_parItemFlags & IT_PERF_OPTION)
            menu_item_flags += " | IT_PERF_OPTION";

        if (menu_item->m_parItemFlags & IT_MULTILINE)
            menu_item_flags += " | IT_MULTILINE";

        if (menu_item->m_parItemFlags & IT_NEEDS_APPLY)
            menu_item_flags += " | IT_NEEDS_APPLY";

        if (menu_item->m_parItemFlags & IT_NEEDS_RESTART)
            menu_item_flags += " | IT_NEEDS_RESTART";

        if (menu_item->m_parItemFlags & IT_EXTENDED_MENU)
            menu_item_flags += " | IT_EXTENDED_MENU";

        if (menu_item_flags.length() >= 3)
            menu_item_flags.erase(0, 3);

        return menu_item_flags;
    }

    void menu_item_log(zCMenuItem* menu_item)
    {
        // if (menu_item == nullptr)
        // {
        //     log << "menu_item_log(), menu_item == nullptr" << std::endl;
        //     return;
        // }
        //
        // auto menu_item_name = menu_item->GetName();
        // log << "menu_item name: " << menu_item_name << std::endl;
        // log << "menu_item m_parFontName: " << menu_item->m_parFontName << std::endl;
        // log << "menu_item m_parText[0]: " << menu_item->m_parText[0] << std::endl;
        // log << "menu_item m_parText[1]: " << menu_item->m_parText[1] << std::endl;
        // // log << "menu_item m_parText[2]: " << menu_item->m_parText[2] << std::endl;
        // // log << "menu_item m_parText[3]: " << menu_item->m_parText[3] << std::endl;
        // // log << "menu_item m_parText[4]: " << menu_item->m_parText[4] << std::endl;
        // // log << "menu_item m_parText[5]: " << menu_item->m_parText[5] << std::endl;
        // // log << "menu_item m_parText[6]: " << menu_item->m_parText[6] << std::endl;
        // // log << "menu_item m_parText[7]: " << menu_item->m_parText[7] << std::endl;
        // // log << "menu_item m_parText[8]: " << menu_item->m_parText[8] << std::endl;
        // // log << "menu_item m_parText[9]: " << menu_item->m_parText[9] << std::endl;
        // log << "menu_item m_parBackPic: " << menu_item->m_parBackPic << std::endl;
        // log << "menu_item m_parAlphaMode: " << menu_item->m_parAlphaMode << std::endl;
        // log << "menu_item m_parAlpha: " << menu_item->m_parAlpha << std::endl;
        // log << "menu_item m_parType: " << menu_item->m_parType << std::endl;
        // log << "menu_item m_parOnSelAction[0]: " << menu_item->m_parOnSelAction[0] << std::endl;
        // log << "menu_item m_parOnSelAction[1]: " << menu_item->m_parOnSelAction[1] << std::endl;
        // log << "menu_item m_parOnSelAction[2]: " << menu_item->m_parOnSelAction[2] << std::endl;
        // log << "menu_item m_parOnSelAction[3]: " << menu_item->m_parOnSelAction[3] << std::endl;
        // log << "menu_item m_parOnSelAction[4]: " << menu_item->m_parOnSelAction[4] << std::endl;
        // log << "menu_item m_parOnSelAction_S[0]: " << menu_item->m_parOnSelAction_S[0] << std::endl;
        // log << "menu_item m_parOnSelAction_S[1]: " << menu_item->m_parOnSelAction_S[1] << std::endl;
        // log << "menu_item m_parOnSelAction_S[2]: " << menu_item->m_parOnSelAction_S[2] << std::endl;
        // log << "menu_item m_parOnSelAction_S[3]: " << menu_item->m_parOnSelAction_S[3] << std::endl;
        // log << "menu_item m_parOnSelAction_S[4]: " << menu_item->m_parOnSelAction_S[4] << std::endl;
        // log << "menu_item m_parOnChgSetOption: " << menu_item->m_parOnChgSetOption << std::endl;
        // log << "menu_item m_parOnChgSetOptionSection: " << menu_item->m_parOnChgSetOptionSection << std::endl;
        // log << "menu_item m_parOnEventAction[0]: " << menu_item->m_parOnEventAction[0] << std::endl;
        // log << "menu_item m_parOnEventAction[1]: " << menu_item->m_parOnEventAction[1] << std::endl;
        // log << "menu_item m_parOnEventAction[2]: " << menu_item->m_parOnEventAction[2] << std::endl;
        // // log << "menu_item m_parOnEventAction[3]: " << menu_item->m_parOnEventAction[3] << std::endl;
        // // log << "menu_item m_parOnEventAction[4]: " << menu_item->m_parOnEventAction[4] << std::endl;
        // // log << "menu_item m_parOnEventAction[5]: " << menu_item->m_parOnEventAction[5] << std::endl;
        // // log << "menu_item m_parOnEventAction[6]: " << menu_item->m_parOnEventAction[6] << std::endl;
        // // log << "menu_item m_parOnEventAction[7]: " << menu_item->m_parOnEventAction[7] << std::endl;
        // // log << "menu_item m_parOnEventAction[8]: " << menu_item->m_parOnEventAction[8] << std::endl;
        // // log << "menu_item m_parOnEventAction[9]: " << menu_item->m_parOnEventAction[9] << std::endl;
        // log << "menu_item m_parPosX: " << menu_item->m_parPosX << std::endl;
        // log << "menu_item m_parPosY: " << menu_item->m_parPosY << std::endl;
        // log << "menu_item m_parDimX: " << menu_item->m_parDimX << std::endl;
        // log << "menu_item m_parDimY: " << menu_item->m_parDimY << std::endl;
        // log << "menu_item m_parSizeStartScale: " << menu_item->m_parSizeStartScale << std::endl;
        // log << "menu_item m_parItemFlags: " << get_menu_item_flags(menu_item) << std::endl;
        // log << "menu_item m_parOpenDelayTime: " << menu_item->m_parOpenDelayTime << std::endl;
        // log << "menu_item m_parOpenDuration: " << menu_item->m_parOpenDuration << std::endl;
        // log << "menu_item m_parUserFloat[0]: " << menu_item->m_parUserFloat[0] << std::endl;
        // log << "menu_item m_parUserFloat[1]: " << menu_item->m_parUserFloat[1] << std::endl;
        // log << "menu_item m_parUserFloat[2]: " << menu_item->m_parUserFloat[2] << std::endl;
        // log << "menu_item m_parUserFloat[3]: " << menu_item->m_parUserFloat[3] << std::endl;
        // log << "menu_item m_parUserString[0]: " << menu_item->m_parUserString[0] << std::endl;
        // log << "menu_item m_parUserString[1]: " << menu_item->m_parUserString[1] << std::endl;
        // log << "menu_item m_parUserString[2]: " << menu_item->m_parUserString[2] << std::endl;
        // log << "menu_item m_parUserString[3]: " << menu_item->m_parUserString[3] << std::endl;
        // log << "menu_item m_parFrameSizeX: " << menu_item->m_parFrameSizeX << std::endl;
        // log << "menu_item m_parFrameSizeY: " << menu_item->m_parFrameSizeY << std::endl;
        //
        // log << "menu_item m_pInnerWindow: " << menu_item->m_pInnerWindow << std::endl;
        // log << "menu_item m_pFont: " << menu_item->m_pFont << std::endl;
        // log << "menu_item m_pFontHi: " << menu_item->m_pFontHi << std::endl;
        // log << "menu_item m_pFontSel: " << menu_item->m_pFontSel << std::endl;
        // log << "menu_item m_pFontDis: " << menu_item->m_pFontDis << std::endl;
        // log << "menu_item m_bViewInitialized: " << menu_item->m_bViewInitialized << std::endl;
        // log << "menu_item m_bLeaveItem: " << menu_item->m_bLeaveItem << std::endl;
        // log << "menu_item m_bVisible: " << menu_item->m_bVisible << std::endl;
        // // log << "menu_item m_listLines: " << menu_item->m_listLines << std::endl;
        // log << "menu_item id: " << menu_item->id << std::endl;
        // log << "menu_item inserted: " << menu_item->inserted << std::endl;
        // log << "menu_item changed: " << menu_item->changed << std::endl;
        // log << "menu_item active: " << menu_item->active << std::endl;
        // log << "menu_item open: " << menu_item->open << std::endl;
        // log << "menu_item close: " << menu_item->close << std::endl;
        // log << "menu_item opened: " << menu_item->opened << std::endl;
        // log << "menu_item closed: " << menu_item->closed << std::endl;
        // log << "menu_item disabled: " << menu_item->disabled << std::endl;
        // log << "menu_item orgWin: " << menu_item->orgWin << std::endl;
        // log << "menu_item fxTimer: " << menu_item->fxTimer << std::endl;
        // log << "menu_item openDelayTimer: " << menu_item->openDelayTimer << std::endl;
        // log << "menu_item activeTimer: " << menu_item->activeTimer << std::endl;
        // log << "menu_item registeredCPP: " << menu_item->registeredCPP << std::endl;
        // log << "menu_item firstTimeInserted: " << menu_item->firstTimeInserted << std::endl;
        // log << "menu_item m_pInnerWindow: " << menu_item->m_pInnerWindow << std::endl;
        //
        // log.flush();
    }

    void menu_log(zCMenu* menu)
    {
        // auto menu_name = menu->GetName();
        // log << "menu name: " << menu_name << std::endl;
        // log << "menu_item backPic: " << menu->backPic << std::endl;
        // log << "menu_item backWorld: " << menu->backWorld << std::endl;
        // log << "menu_item posx: " << menu->posx << std::endl;
        // log << "menu_item posy: " << menu->posy << std::endl;
        // log << "menu_item dimx: " << menu->dimx << std::endl;
        // log << "menu_item dimy: " << menu->dimy << std::endl;
        // log << "menu_item alpha: " << menu->alpha << std::endl;
        // log << "menu_item musicThemeName: " << menu->musicThemeName << std::endl;
        // log << "menu_item eventTimerUpdateMSec: " << menu->eventTimerUpdateMSec << std::endl;
        // log << "menu_item flags: " << menu->flags << std::endl;
        // log << "menu_item m_pWindow: " << menu->m_pWindow << std::endl;
        // log << "menu_item m_pInnerWindow: " << menu->m_pInnerWindow << std::endl;
        // log << "menu_item m_pViewInfo: " << menu->m_pViewInfo << std::endl;
        // log << "menu_item defaultOutGame: " << menu->defaultOutGame << std::endl;
        // log << "menu_item defaultInGame: " << menu->defaultInGame << std::endl;
        // log.flush();
        //
        // for (int i = 0; i < menu->m_listItems.GetNum(); ++i)
        // {
        //     log << "menu_item index: " << i <<std::endl;
        //     auto menu_item = menu->m_listItems[i];
        //     menu_item_log(menu_item);
        //     log << std::endl;
        // }
    }

    zCMenuItem* create_menu_item_from_other(const zCMenuItem* other)
    {
        zCMenuItem* created = zCMenuItem::Create("C_MENU_ITEM");
        if (created == nullptr)
            return nullptr;

        created->m_parFontName = other->m_parFontName;
        created->m_parBackPic = other->m_parBackPic;
        created->m_parAlphaMode = other->m_parAlphaMode;
        created->m_parAlpha = other->m_parAlpha;
        created->m_parType = other->m_parType;

        created->m_parPosX = other->m_parPosX;
        created->m_parPosY = other->m_parPosY;
        created->m_parDimX = other->m_parDimX;
        created->m_parDimY = other->m_parDimY;

        created->m_parSizeStartScale = other->m_parSizeStartScale;
        created->m_parItemFlags = other->m_parItemFlags;
        created->m_parOpenDelayTime = other->m_parOpenDelayTime;
        created->m_parOpenDuration = other->m_parOpenDuration;

        created->m_parUserFloat[0] = other->m_parUserFloat[0];
        created->m_parUserFloat[1] = other->m_parUserFloat[1];
        created->m_parUserFloat[2] = other->m_parUserFloat[2];
        created->m_parUserFloat[3] = other->m_parUserFloat[3];

        created->m_pFont = other->m_pFont;
        created->m_pFontHi = other->m_pFontHi;
        created->m_pFontSel = other->m_pFontSel;
        created->m_pFontDis = other->m_pFontDis;

        created->m_bVisible = other->m_bVisible;

        created->orgWin = other->orgWin;

        return created;
    }

    zCMenuItem* get_menu_item_in_menu_by_name(zCMenu* menu, std::string & menu_item_name)
    {
        for (int i = 0; i < menu->m_listItems.GetNum(); ++i)
        {
            std::string iterate_menu_item_name = (const char*)menu->m_listItems[i]->GetName();
            if (iterate_menu_item_name == menu_item_name)
                return menu->m_listItems[i];
        }

        return nullptr;
    }

    void move_menu_items_in_menu(zCMenu* menu)
    {
        auto menu_item = menu->m_listItems[menu->m_listItems.GetNum() - 1];
        int x = 0, y = 0;
        menu_item->GetPos(x, y);

        if (main_menu_exit_y < 0)
            main_menu_exit_y = y + menu_y_offset;

        if (y < main_menu_exit_y)
            menu_item->SetPos(x, main_menu_exit_y);
    }

    void create_and_insert_menu_item_language_to_menu(zCMenu* menu)
    {
        if (menu == nullptr)
            return;

        auto main_menu_items_len = menu->m_listItems.GetNum();
        if (main_menu_items_len < 3)
            return;

        auto menu_item_exit = menu->m_listItems[main_menu_items_len - 1];
        if (menu_item_exit == nullptr)
            return;

        auto menu_item_credits = menu->m_listItems[main_menu_items_len - 2];
        if (menu_item_credits == nullptr)
            return;

        auto menu_item_intro = menu->m_listItems[main_menu_items_len - 3];
        if (menu_item_intro == nullptr)
            return;

        // if (menu_y_offset == 0)
        // {
        int x = 0, menu_item_exit_y = 0, menu_item_credits_y = 0, menu_item_intro_y = 0;
        menu_item_exit->GetPos(x, menu_item_exit_y);
        menu_item_credits->GetPos(x, menu_item_credits_y);
        menu_item_intro->GetPos(x, menu_item_intro_y);

        menu_y_offset = menu_item_credits_y - menu_item_intro_y;
        // }

        move_menu_items_in_menu(menu);

        auto menu_item_language = create_menu_item_from_other(menu_item_exit);
        if (menu_item_language == nullptr)
            return;

        menu_item_language->SetName("MENUITEM_LANGUAGE"); // also change id
        menu_item_language->SetText(selected_language_data.menu_language.c_str(), 0, 0);
        menu_item_language->m_parText[1] = selected_language_data.menu_language_description.c_str();
        menu_item_language->m_parPosY = menu_item_credits_y + menu_y_offset;
        menu_item_language->m_parOnSelAction[0] = SEL_ACTION_STARTMENU;
        menu_item_language->m_parOnSelAction_S[0] = MENU_LANGUAGE.c_str();

        menu_item_language->m_parItemFlags |= IT_ONLY_OUT_GAME; // add flag

        menu_item_language->InsertInWin(menu_item_language->orgWin);

        // [] = <- do not expand array, only insert in uses slot, InsertAtPos - if need new slot
        menu->m_listItems[main_menu_items_len - 1] = menu_item_language;
        menu->m_listItems.InsertAtPos(menu_item_exit, main_menu_items_len);
    }
    //
    // void create_menu_language()
    // {
    //     zCMenu* menu_language = zCMenu::GetByName(MENU_LANGUAGE.c_str());
    //     if (menu_language == nullptr)
    //     {
    //         menu_language = new zCMenu();
    //         if (menu_language == nullptr)
    //             return;
    //     }
    //
    //     if (get_menu_item_in_menu_by_name(menu_language, MENUITEM_LANGUAGE_TEXT_CHOICE) != nullptr)
    //         return;
    //
    //     // return;
    //
    //     // log << "menu_language NOT FOUND" << std::endl;
    //     // log.flush();
    //
    //
    //
    //     menu_language->SetByScript("MENU_OPT_AUDIO");
    //     menu_language->SetName(MENU_LANGUAGE.c_str());
    //
    //     menu_language->Register();
    //     menu_language->Activate();
    //
    //     // menu_log(menu_language);
    //
    //     //     //->GetByName()
    //
    //     int min_x = 99999999, max_x = 0, min_y = 99999999, max_y = 0;
    //     std::vector<zCMenuItem*> menu_items_to_delete_vec;
    //     std::vector<zCMenuItem*> menu_items_to_not_delete_vec;
    //
    //     std::vector<zCMenuItem*> menu_items_label_vec;
    //     std::vector<int> menu_items_label_x_vec;
    //     std::vector<int> menu_items_label_y_vec;
    //     std::vector<zCMenuItem*> menu_items_choice_vec;
    //     std::vector<int> menu_items_choice_x_vec;
    //     std::vector<int> menu_items_choice_y_vec;
    //
    //     zCMenuItem* menu_item_headline = nullptr;
    //     zCMenuItem* menu_item_language_text_label = nullptr;
    //     zCMenuItem* menu_item_language_audio_label = nullptr;
    //     zCMenuItem* menu_item_back = nullptr;
    //     for (auto i = 0; i < menu_language->m_listItems.GetNum(); ++i)
    //     {
    //         std::string menu_item_name = (const char*)menu_language->m_listItems[i]->GetName();
    //         bool is_headline = menu_item_name.find("_HEAD") != std::string::npos;
    //         bool is_back = menu_item_name.find("_BACK") != std::string::npos;
    //         if (is_headline)
    //         {
    //             menu_item_headline = menu_language->m_listItems[i];
    //         }
    //         else if (is_back)
    //         {
    //             menu_item_back = menu_language->m_listItems[i];
    //         }
    //         else
    //         {
    //             int x = 0, y = 0;
    //             menu_language->m_listItems[i]->GetPos(x, y);
    //             min_x = min(x, min_x); max_x = max(x, max_x);
    //             min_y = min(y, min_y); max_y = max(y, max_y);
    //
    //             log << "name: " << menu_language->m_listItems[i]->GetName() << ", x: " << x << ", y: " << y << std::endl;
    //             log.flush();
    //
    //             // choice label
    //             if (menu_language->m_listItems[i]->m_parItemFlags & IT_EFFECTS_NEXT)
    //             {
    //                 menu_items_label_vec.push_back(menu_language->m_listItems[i]);
    //                 menu_items_label_x_vec.push_back(x);
    //                 menu_items_label_y_vec.push_back(y);
    //             }
    //             // choice
    //             else if (menu_language->m_listItems[i]->m_parType == MENU_ITEM_CHOICEBOX)
    //             {
    //                 menu_items_choice_vec.push_back(menu_language->m_listItems[i]);
    //                 menu_items_choice_x_vec.push_back(x);
    //                 menu_items_choice_y_vec.push_back(y);
    //             }
    //
    //             menu_items_to_delete_vec.push_back(menu_language->m_listItems[i]);
    //         }
    //     }
    //
    //     log << ", min_x: " << min_x << ", min_y: " << min_y << std::endl;
    //     log.flush();
    //
    //     // bool is_pair_found = false;
    //     // for (int i = 0; i < menu_items_label_vec.size(); ++i)
    //     // {
    //     //     if (is_pair_found)
    //     //         break;
    //     //
    //     //     for (int j = 0; j < menu_items_choice_vec.size(); ++j)
    //     //     {
    //     //         if (menu_items_label_y_vec[i] < menu_items_choice_y_vec[j] + 160
    //     //             && menu_items_label_y_vec[i] > menu_items_choice_y_vec[j] - 160)
    //     //         {
    //     //             menu_item_language_text_label = menu_items_label_vec[i];
    //     //             menu_item_language_text_choice = menu_items_choice_vec[j];
    //     //             is_pair_found = true;
    //     //             break;
    //     //         }
    //     //     }
    //     // }
    //
    //     for (int i = 0; i < menu_items_label_vec.size(); ++i)
    //     {
    //         if (menu_item_language_audio_label && menu_item_language_audio_choice)
    //             break;
    //
    //         for (int j = 0; j < menu_items_choice_vec.size(); ++j)
    //         {
    //             if (menu_items_label_y_vec[i] < menu_items_choice_y_vec[j] + 160
    //                 && menu_items_label_y_vec[i] > menu_items_choice_y_vec[j] - 160)
    //             {
    //                 if (!menu_item_language_text_label && !menu_item_language_text_choice)
    //                 {
    //                     menu_item_language_text_label = menu_items_label_vec[i];
    //                     menu_item_language_text_choice = menu_items_choice_vec[j];
    //                     menu_items_to_not_delete_vec.push_back(menu_item_language_text_label);
    //                     menu_items_to_not_delete_vec.push_back(menu_item_language_text_choice);
    //                     break;
    //                 }
    //                 if (!menu_item_language_audio_label && !menu_item_language_audio_choice)
    //                 {
    //                     menu_item_language_audio_label = menu_items_label_vec[i];
    //                     menu_item_language_audio_choice = menu_items_choice_vec[j];
    //                     menu_items_to_not_delete_vec.push_back(menu_item_language_audio_label);
    //                     menu_items_to_not_delete_vec.push_back(menu_item_language_audio_choice);
    //                     break;
    //                 }
    //             }
    //         }
    //     }
    //
    //     // menu_item_log(menu_item_language_text_label);
    //     // menu_item_log(menu_item_language_text_choice);
    //     // menu_item_log(menu_item_language_audio_label);
    //     // menu_item_log(menu_item_language_audio_choice);
    //
    //     menu_item_headline->SetName("MENUITEM_LANGUAGE_HEADLINE");
    //     menu_item_headline->SetText(selected_language_data.menu_language.c_str(), 0, 0);
    //
    //     menu_item_back->SetName("MENUITEM_LANGUAGE_BACK");
    //     menu_item_back->SetText(selected_language_data.menu_language_back.c_str(), 0, 0);
    //
    //     if (menu_item_language_text_label == nullptr || menu_item_language_text_choice == nullptr)
    //         return;
    //
    //     if (menu_item_language_audio_label == nullptr || menu_item_language_audio_choice == nullptr)
    //         return;
    //
    //     for (auto menu_items_to_delete : menu_items_to_delete_vec)
    //     {
    //         if (std::ranges::find(menu_items_to_not_delete_vec, menu_items_to_delete) == menu_items_to_not_delete_vec.end())
    //         {
    //             menu_items_to_delete->RemoveFromWin();
    //             menu_language->m_listItems.Remove(menu_items_to_delete);
    //         }
    //     }
    //
    //     // ---
    //
    //     menu_item_language_text_label->SetName("MENUITEM_LANGUAGE_TEXT");
    //     menu_item_language_text_label->SetText(selected_language_data.menu_language_select_text_language.c_str(), 0, 0);
    //     menu_item_language_text_label->m_parText[1] = selected_language_data.menu_language_select_text_language_description.c_str();
    //     menu_item_language_text_label->m_parPosY = min_y;
    //
    //     menu_item_language_text_choice->SetName(MENUITEM_LANGUAGE_TEXT_CHOICE.c_str());
    //     menu_item_language_text_choice->SetText(text_language_for_item.c_str(), 0, 0);
    //     menu_item_language_text_choice->m_parPosY = min_y + 120;
    //     menu_item_language_text_choice->m_parDimX = 2000;
    //     menu_item_language_text_choice->m_parOnChgSetOption = "";
    //     menu_item_language_text_choice->m_parOnChgSetOptionSection = "";
    //     for (auto i = 0; i < MAX_EVENTS; ++i)
    //         menu_item_language_text_choice->m_parOnEventAction[i] = 0;
    //
    //     if (menu_item_language_text_choice)
    //     {
    //         auto menu_item_choice_language_choice = dynamic_cast<zCMenuItemChoice*>(menu_item_language_text_choice);
    //         if (menu_item_choice_language_choice)
    //         {
    //             is_setup_toggle = true;
    //             for (auto i = menu_item_choice_language_choice->option; i < menu_item_language_choice_option_start; ++i)
    //             {
    //                 menu_item_choice_language_choice->ToggleValue(1, 0);
    //             }
    //             is_setup_toggle = false;
    //         }
    //     }
    //
    //     // ---
    //
    //     log << ", menu_y_offset: " << menu_y_offset << ", menu_y_offset: " << menu_y_offset << std::endl;
    //     log.flush();
    //
    //
    //
    //     menu_item_language_audio_label->SetName("MENUITEM_LANGUAGE_AUDIO");
    //     menu_item_language_audio_label->SetText(selected_language_data.menu_language_select_audio_language.c_str(), 0, 0);
    //     menu_item_language_audio_label->m_parText[1] = selected_language_data.menu_language_select_audio_language_description.c_str();
    //     menu_item_language_audio_label->m_parPosY = min_y + 550 * 1;
    //
    //     menu_item_language_audio_choice->SetName(MENUITEM_LANGUAGE_AUDIO_CHOICE.c_str());
    //     menu_item_language_audio_choice->SetText("", 0, 0);
    //     menu_item_language_audio_choice->m_parPosY = min_y + 120 + 550 * 1;
    //     menu_item_language_audio_choice->m_parDimX = 2000;
    //     menu_item_language_audio_choice->m_parOnChgSetOption = "";
    //     menu_item_language_audio_choice->m_parOnChgSetOptionSection = "";
    //     for (int i = 0; i < MAX_EVENTS; ++i)
    //         menu_item_language_audio_choice->m_parOnEventAction[i] = 0;
    //
    //     if (menu_item_language_audio_choice)
    //     {
    //         auto menu_item_choice_audio_language_choice = dynamic_cast<zCMenuItemChoice*>(menu_item_language_audio_choice);
    //         if (menu_item_choice_audio_language_choice)
    //         {
    //             if (menu_item_choice_audio_language_choice->GetNumOptions() < 2)
    //             {
    //                 menu_item_language_audio_label->m_parItemFlags |= IT_ONLY_IN_GAME;
    //                 menu_item_language_audio_label->m_parItemFlags |= IT_ONLY_OUT_GAME;
    //             }
    //             // is_setup_toggle = true;
    //             // for (auto i = menu_item_choice_audio_language_choice->option; i < menu_item_language_choice_option_start; ++i)
    //             // {
    //             //     menu_item_choice_audio_language_choice->ToggleValue(1, 0);
    //             // }
    //             // is_setup_toggle = false;
    //         }
    //     }
    //
    //     // ---
    //
    //     zCArray<zCMenuItem*> list_menu_items;
    //     list_menu_items.InsertEnd(menu_item_headline);
    //     list_menu_items.InsertEnd(menu_item_language_text_label);
    //     list_menu_items.InsertEnd(menu_item_language_text_choice);
    //     list_menu_items.InsertEnd(menu_item_language_audio_label);
    //     list_menu_items.InsertEnd(menu_item_language_audio_choice);
    //     list_menu_items.InsertEnd(menu_item_back);
    //     menu_language->m_listItems = list_menu_items;
    // }


    void create_menu_language()
    {
        zCMenu* menu_language = zCMenu::GetByName(MENU_LANGUAGE.c_str());
        if (menu_language != nullptr)
            return;

        menu_language = new zCMenu();
        if (menu_language == nullptr)
            return;

        menu_language->SetByScript("MENU_OPT_AUDIO");
        menu_language->SetName(MENU_LANGUAGE.c_str());

        int min_x = 99999999, max_x = 0, min_y = 99999999, max_y = 0;
        std::vector<zCMenuItem*> menu_items_to_delete_vec;
        std::vector<zCMenuItem*> menu_items_to_not_delete_vec;

        std::vector<zCMenuItem*> menu_items_label_vec;
        std::vector<int> menu_items_label_x_vec;
        std::vector<int> menu_items_label_y_vec;
        std::vector<zCMenuItem*> menu_items_choice_vec;
        std::vector<int> menu_items_choice_x_vec;
        std::vector<int> menu_items_choice_y_vec;

        zCMenuItem* menu_item_headline = nullptr;
        zCMenuItem* menu_item_language_text_label = nullptr;
        zCMenuItem* menu_item_language_audio_label = nullptr;
        zCMenuItem* menu_item_back = nullptr;
        for (auto i = 0; i < menu_language->m_listItems.GetNum(); ++i)
        {
            std::string menu_item_name = (const char*)menu_language->m_listItems[i]->GetName();
            bool is_headline = menu_item_name.find("_HEAD") != std::string::npos;
            bool is_back = menu_item_name.find("_BACK") != std::string::npos;
            if (is_headline)
            {
                menu_item_headline = menu_language->m_listItems[i];
            }
            else if (is_back)
            {
                menu_item_back = menu_language->m_listItems[i];
            }
            else
            {
                int x = 0, y = 0;
                menu_language->m_listItems[i]->GetPos(x, y);
                min_x = min(x, min_x); max_x = max(x, max_x);
                min_y = min(y, min_y); max_y = max(y, max_y);

                // log << "name: " << menu_language->m_listItems[i]->GetName() << ", x: " << x << ", y: " << y << std::endl;
                // log.flush();

                // choice label
                if (menu_language->m_listItems[i]->m_parItemFlags & IT_EFFECTS_NEXT)
                {
                    menu_items_label_vec.push_back(menu_language->m_listItems[i]);
                    menu_items_label_x_vec.push_back(x);
                    menu_items_label_y_vec.push_back(y);
                }
                // choice
                else if (menu_language->m_listItems[i]->m_parType == MENU_ITEM_CHOICEBOX)
                {
                    menu_items_choice_vec.push_back(menu_language->m_listItems[i]);
                    menu_items_choice_x_vec.push_back(x);
                    menu_items_choice_y_vec.push_back(y);
                }

                menu_items_to_delete_vec.push_back(menu_language->m_listItems[i]);
            }
        }

        bool is_text_pair_found = false; bool is_audio_pair_found = false;
        for (int i = 0; i < menu_items_label_vec.size(); ++i)
        {
            if (is_text_pair_found && is_audio_pair_found)
                break;

            for (int j = 0; j < menu_items_choice_vec.size(); ++j)
            {
                if (menu_items_label_y_vec[i] < menu_items_choice_y_vec[j] + 160
                    && menu_items_label_y_vec[i] > menu_items_choice_y_vec[j] - 160)
                {
                    if (!is_text_pair_found)
                    {
                        menu_item_language_text_label = menu_items_label_vec[i];
                        menu_item_language_text_choice = menu_items_choice_vec[j];
                        menu_items_to_not_delete_vec.push_back(menu_item_language_text_label);
                        menu_items_to_not_delete_vec.push_back(menu_item_language_text_choice);
                        is_text_pair_found = true;
                        break;
                    }
                    if (!is_audio_pair_found)
                    {
                        menu_item_language_audio_label = menu_items_label_vec[i];
                        menu_item_language_audio_choice = menu_items_choice_vec[j];
                        menu_items_to_not_delete_vec.push_back(menu_items_label_vec[i]);
                        menu_items_to_not_delete_vec.push_back(menu_items_choice_vec[j]);
                        is_audio_pair_found = true;
                        break;
                    }
                }
            }
        }

        for (auto menu_items_to_delete : menu_items_to_delete_vec)
        {
            if (std::ranges::find(menu_items_to_not_delete_vec, menu_items_to_delete) == menu_items_to_not_delete_vec.end())
            {
                menu_items_to_delete->RemoveFromWin();
                menu_language->m_listItems.Remove(menu_items_to_delete);
            }
        }

        // ---

        menu_item_headline->SetName("MENUITEM_LANGUAGE_HEADLINE");
        menu_item_headline->SetText(selected_language_data.menu_language.c_str(), 0, 0);

        menu_item_back->SetName("MENUITEM_LANGUAGE_BACK");
        menu_item_back->SetText(selected_language_data.menu_language_back.c_str(), 0, 0);

        // ---

        if (menu_item_language_text_label)
        {
            menu_item_language_text_label->SetName("MENUITEM_LANGUAGE_TEXT");
            menu_item_language_text_label->SetText(selected_language_data.menu_language_select_text_language.c_str(), 0, 0);
            menu_item_language_text_label->m_parText[1] = selected_language_data.menu_language_select_text_language_description.c_str();
            menu_item_language_text_label->m_parPosY = min_y;
        }

        if (menu_item_language_text_choice)
        {
            menu_item_language_text_choice->SetName(MENUITEM_LANGUAGE_TEXT_CHOICE.c_str());
            menu_item_language_text_choice->SetText(text_language_for_item.c_str(), 0, 0);
            menu_item_language_text_choice->m_parPosY = min_y + 120;
            menu_item_language_text_choice->m_parDimX = 2000;
            menu_item_language_text_choice->m_parOnChgSetOption = "";
            menu_item_language_text_choice->m_parOnChgSetOptionSection = "";
            for (auto i = 0; i < MAX_EVENTS; ++i)
                menu_item_language_text_choice->m_parOnEventAction[i] = 0;
        }

        if (menu_item_language_text_choice)
        {
            auto menu_item_choice_language_text_choice = dynamic_cast<zCMenuItemChoice*>(menu_item_language_text_choice);
            if (menu_item_choice_language_text_choice)
            {
                if (menu_item_language_text_label)
                {
                    if (menu_item_choice_language_text_choice->GetNumOptions() < 2)
                    {
                        menu_item_language_text_label->m_parItemFlags |= IT_ONLY_IN_GAME;
                        menu_item_language_text_label->m_parItemFlags |= IT_ONLY_OUT_GAME;
                    }
                }

                is_setup_toggle = true;
                for (auto i = menu_item_choice_language_text_choice->option; i < text_choice_option_start; ++i)
                {
                    menu_item_choice_language_text_choice->ToggleValue(1, 0);
                }
                is_setup_toggle = false;
            }
        }

        // ---

        if (menu_item_language_audio_label)
        {
            menu_item_language_audio_label->SetName("MENUITEM_LANGUAGE_AUDIO");
            menu_item_language_audio_label->SetText(selected_language_data.menu_language_select_audio_language.c_str(), 0, 0);
            menu_item_language_audio_label->m_parText[1] = selected_language_data.menu_language_select_audio_language_description.c_str();
            menu_item_language_audio_label->m_parPosY = min_y + 550 * 1;
        }

        if (menu_item_language_audio_choice)
        {
            menu_item_language_audio_choice->SetName(MENUITEM_LANGUAGE_AUDIO_CHOICE.c_str());
            menu_item_language_audio_choice->SetText(audio_language_for_item.c_str(), 0, 0);
            menu_item_language_audio_choice->m_parPosY = min_y + 120 + 550 * 1;
            menu_item_language_audio_choice->m_parDimX = 2000;
            menu_item_language_audio_choice->m_parOnChgSetOption = "";
            menu_item_language_audio_choice->m_parOnChgSetOptionSection = "";
            for (int i = 0; i < MAX_EVENTS; ++i)
                menu_item_language_audio_choice->m_parOnEventAction[i] = 0;

            if (menu_item_language_audio_choice)
            {
                auto menu_item_choice_audio_language_choice = dynamic_cast<zCMenuItemChoice*>(menu_item_language_audio_choice);
                if (menu_item_choice_audio_language_choice)
                {
                    if (menu_item_language_audio_label)
                    {
                        if (menu_item_choice_audio_language_choice->GetNumOptions() < 2)
                        {
                            menu_item_language_audio_label->m_parItemFlags |= IT_ONLY_IN_GAME;
                            menu_item_language_audio_label->m_parItemFlags |= IT_ONLY_OUT_GAME;
                        }
                    }

                    is_setup_toggle = true;
                    for (auto i = menu_item_choice_audio_language_choice->option; i < audio_choice_option_start; ++i)
                    {
                        menu_item_choice_audio_language_choice->ToggleValue(1, 0);
                    }
                    is_setup_toggle = false;
                }
            }
        }

        // ---

        zCArray<zCMenuItem*> list_menu_items;
        list_menu_items.InsertEnd(menu_item_headline);
        if (menu_item_language_text_label)
            list_menu_items.InsertEnd(menu_item_language_text_label);
        if (menu_item_language_text_choice)
            list_menu_items.InsertEnd(menu_item_language_text_choice);
        if (menu_item_language_audio_label)
            list_menu_items.InsertEnd(menu_item_language_audio_label);
        if (menu_item_language_audio_choice)
            list_menu_items.InsertEnd(menu_item_language_audio_choice);
        list_menu_items.InsertEnd(menu_item_back);
        menu_language->m_listItems = list_menu_items;
    }

    void menu_item_toggle_value(zCMenuItemChoice* menu_item_choice, int v1, int v2)
    {
        if (menu_item_choice == nullptr)
            return;

        if (is_setup_toggle)
            return;

        auto index = menu_item_choice->option;

        std::string toggled_menu_item_name = (const char*)menu_item_choice->GetName();
        if (toggled_menu_item_name == MENUITEM_LANGUAGE_TEXT_CHOICE)
        {
            change_language_from_options(index);
        }

        std::string option_name = (const char*)menu_item_choice->GetStringByOption();

        // log << "menu_item_toggle_value, toggled_menu_item_name: " << toggled_menu_item_name  << ", option: " << option_name << std::endl;
        // log << "menu_item_toggle_value, v1: " << v1  << ", v2: " << v2 << std::endl;
        // log << "menu_item_choice option: " << menu_item_choice->option << std::endl;
        // log << "menu_item_choice optionStart: " << menu_item_choice->optionStart << std::endl;
        // log << "menu_item_choice curStep: " << menu_item_choice->curStep << std::endl;
        // log.flush();
    }

    void menu_loop()
    {
        auto active_menu = zCMenu::GetActive();
        if (active_menu == nullptr)
            return;

        std::string active_menu_name = (const char*)active_menu->GetName();

        if (main_menu_name.empty())
        {
            main_menu_name = active_menu_name;
        }

        if (active_menu_name == main_menu_name)
        {
            auto menu_item_language = get_menu_item_in_menu_by_name(active_menu, MENUITEM_LANGUAGE);
            if (menu_item_language == nullptr)
            {
                // menu_log(main_menu);
                create_menu_language();
                create_and_insert_menu_item_language_to_menu(active_menu);
                // log << "main_menu_loaded: menu_item_language == nullptr" << std::endl;
                // log.flush();
            }
            else
            {
                create_menu_language();
                move_menu_items_in_menu(active_menu);
                // log << "main_menu_loaded: menu_item_language != nullptr" << std::endl;
                // log.flush();
            }
        }
    }

}
