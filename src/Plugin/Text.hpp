
// #include "Plugin/Helper.hpp"
// #include "tinyjson/TinyJSON.h"

// https://github.com/System-Glitch/SHA256
// https://github.com/FFMG/tinyjson

namespace GOTHIC_NAMESPACE
{
	// std::ofstream log;
	// std::ofstream stack_log;

	std::vector<std::string> class_with_translated_fields = {
		"C_INFO.DESCRIPTION",
		"C_NPC.NAME",
		"C_ITEM.NAME", "C_ITEM.DESCRIPTION", "C_ITEM.TEXT",
		"C_MENU_ITEM.TEXT",
	};
    std::vector<std::string> assign_string_vector = {};

	// function name, string arguments count, translatable argument position (from 1)
	std::vector<std::string> functions_with_translated_argument = {
		"INFO_ADDCHOICE,     1, 1",
		"LOG_ADDENTRY,       2, 1, 2",
		"INTRODUCECHAPTER,   4, 1, 2",
		"DOC_PRINTLINE,      1, 1",
		"DOC_PRINTLINES,     1, 1",
		"PRINTSCREEN,        2, 1",
		"AI_PRINTSCREEN,     2, 1",
	};
	std::vector<std::vector<std::any>> call_function_with_string_argument_vector_vector = {};

	std::unordered_map<std::string, std::vector<bool>> call_function_with_translated_string_argument_map = {
		{"LOG_ADDENTRY", {true, true}}
	};

	// std::filesystem::path translate_file_path = "Translate/translate.json";
	std::unordered_map<std::string, std::string> json_settings = {
		{"virtual_machine_hash", "-1"},
		{"translate_name", ""},
		{"script_encoding", "-1"},
		{"min_translate_value", "100"},
	};

	// сюда мы записываем строки из json файла с ВМ
	std::vector<std::string> translate_string_data_string_insert_order;
	std::unordered_map<std::string, std::vector<std::string>> translate_string_data_vmh_insert_order;
	std::unordered_map<std::string, std::unordered_map<std::string, std::unordered_map<std::string, std::string>>> translate_string_data;

	// сюда мы записываем текущий скрипт без ВМ
	std::vector<std::string> script_string_data_insert_order;
	std::unordered_map<std::string, std::unordered_map<std::string, std::string>> script_string_data;

	// Тут разделения нет
	std::vector<std::string> ou_string_data_insert_order;
	std::unordered_map<std::string, std::unordered_map<std::string, std::string>> ou_string_data;

	// Тут разделения нет
	std::vector<std::string> menu_string_data_insert_order;
	std::unordered_map<std::string, std::unordered_map<std::string, std::string>> menu_string_data;

	std::unordered_map<uint64_t, std::string> menu_virtual_address_name_map = {{0, std::string("")}};

	std::unordered_map<uint64_t, std::string> virtual_address_name_map = {{0, std::string("")}};
	std::unordered_map<uint64_t, int64_t> virtual_address_index_map = {{0, 0}};

	// std::deque<int64_t> pushed_stack = {};
	// std::deque<bool> pushed_stack_string_watch = {};
	std::unordered_map<std::string, std::vector<bool>> edit_string_functions = {
		{"CONCATSTRINGS", {true, true}},
	};

	std::unordered_set<std::string> function_with_internal_push_stack_set = {
		"LOCALS.READINT",
		"LOCALS.ALLOC",
	};

	std::string encoding_detection_string;
	// uint64_t address_skip_min_value = 0;
	uint64_t ikarus_min_address = 0xFFFFFFFF;
	uint64_t ikarus_max_address = 0;
	uint64_t lego_min_address = 0xFFFFFFFF;
	uint64_t lego_max_address = 0;
	bool ikarus_found = false;
	bool lego_found = false;


	struct StackVariable
	{
		std::string name;
		std::string appear_section_name;
		int64_t value = -1;
		uint64_t array_index = 0;
		bool is_array = false;

		StackVariable() = default;
		StackVariable(std::string s) : appear_section_name(std::move(s)) {};
		StackVariable(const char * s) : appear_section_name(s) {};
		// StackVariable(const char * s, int64_t v) : appear_section_name(s), value(v) {};
		// StackVariable(std::string s, int64_t v) : appear_section_name(s), value(v) {};

		std::string to_string() const
		{
			return "name: " + name
				// + ", appear_section_name: " + appear_section_name
				+ ", value = "+ std::to_string(value);
		}
	};

	struct StackVariableTrace
	{
		StackVariable stack_variable = StackVariable();
		std::vector<StackVariable> extra_stack_variable_vector = {};
		std::vector<StackVariable> returned_inner_stack_variable_vector = {};

		StackVariableTrace() = default;
		// StackVariableTrace(const std::vector<StackVariable> & svv) : stack_variable_vector(svv) {};
		// StackVariableTrace(const StackVariable & sv) : stack_variable_vector(std::vector{sv}) {};

		std::string to_string() const
		{
			std::stringstream ss;
			ss << "SV: " << stack_variable.to_string();

			ss << ", ESV: ";
			for (uint64_t i = 0; i < extra_stack_variable_vector.size(); i++)
			{
				if (i > 0)
					ss << ", ";

				ss << extra_stack_variable_vector[i].to_string();
			}

			ss << ", RISV: ";
			for (uint64_t i = 0; i < returned_inner_stack_variable_vector.size(); i++)
			{
				if (i > 0)
					ss << ", ";

				ss << returned_inner_stack_variable_vector[i].to_string();
			}

			return ss.str();
		}
	};



	struct TranslatedField
	{
		std::string name;
		bool is_array = false;

		TranslatedField() = default;
		TranslatedField(std::string name, const bool is_array) : name(std::move(name)), is_array(is_array) {}
	};

	struct ClassWithTranslatedField
	{
		std::string name = "UNNAMED_CLASS";
		std::vector<TranslatedField> translated_field_vector = {};

		ClassWithTranslatedField() = default;
		ClassWithTranslatedField(std::string name, std::vector<TranslatedField> tfv)
			: name(std::move(name)), translated_field_vector(std::move(tfv)) {};
	};

	std::map<std::string, ClassWithTranslatedField> class_with_translated_field_map {
			{"C_INFO", ClassWithTranslatedField("C_INFO", {
				TranslatedField("DESCRIPTION",	false),
			})},
			{"C_NPC", ClassWithTranslatedField("C_NPC", {
				TranslatedField("NAME",			false),
			})},
			{"C_ITEM", ClassWithTranslatedField("C_ITEM", {
				TranslatedField("NAME",			false),
				TranslatedField("DESCRIPTION",	false),
				TranslatedField("TEXT",			true),
			})},
			{"C_MENU_ITEM", ClassWithTranslatedField("C_MENU_ITEM", {
				TranslatedField("TEXT",			true),
			})},
		};

	struct TranslatedArgument
	{
		std::string name = "ARG_?";
		uint64_t number = 0;
		bool is_translated = false;
		std::set<std::string> used_in = {};

		TranslatedArgument() = default;
		TranslatedArgument(std::string name, const uint64_t number, const bool is_translated)
			: name(std::move(name)), number(number), is_translated(is_translated) {};

		std::string to_string() const
		{
			std::string is_tr = is_translated ? "true" : "false";
			std::string us_in;
			for (auto & used_in_string: used_in)
			{
				us_in += " " + used_in_string;
			}
			return "[TranslatedArgument] name: " + name
				+ ", number: " + std::to_string(number)
				+ ", is_translated: " + is_tr
				+ ", used_in:" + us_in;
		}
	};

	struct FunctionWithTranslatedArgument
	{
		std::string name = "UNNAMED_FUNCTION";
		uint64_t argument_count = 0;
		std::vector<TranslatedArgument> translated_argument_vector = {};

		FunctionWithTranslatedArgument() = default;
		FunctionWithTranslatedArgument(std::string name, const uint64_t argument_count, std::vector<TranslatedArgument> svv)
			: name(std::move(name)), argument_count(argument_count), translated_argument_vector(std::move(svv)) {};

		std::string to_string() const
		{
			std::string translated_argument_vector_ret;
			for (auto & translated_argument: translated_argument_vector)
			{
				translated_argument_vector_ret += " " + translated_argument.to_string();
			}
			return "[FunctionWithTranslatedArgument] name: " + name
				+ ", argument_count: " + std::to_string(argument_count)
				+ ", translated_argument_vector:" + translated_argument_vector_ret;
		}
	};

	std::map<std::string, FunctionWithTranslatedArgument> function_with_translated_argument_map {
		{"LOG_ADDENTRY", FunctionWithTranslatedArgument("LOG_ADDENTRY", 2, {
			TranslatedArgument("TOPIC_NAME",	1, true),
			TranslatedArgument("ENTRY",			2, true),
		})},
		{"PRINT", FunctionWithTranslatedArgument("PRINT", 1, {
			TranslatedArgument("TEXT",			1, true),
		})},
		{"PRINTDIALOG", FunctionWithTranslatedArgument("PRINTDIALOG", 6, {
			TranslatedArgument("WINDOW",		1, false),
			TranslatedArgument("TEXT",			2, true),
			TranslatedArgument("POS_X",			3, false),
			TranslatedArgument("POS_Y",			4, false),
			TranslatedArgument("FONT",			5, false),
			TranslatedArgument("TIME_SEC",		6, false),
		})},
		{"PRINTMULTI", FunctionWithTranslatedArgument("PRINTMULTI", 5, {
			TranslatedArgument("TEXT1",			1, true),
			TranslatedArgument("TEXT2",			2, true),
			TranslatedArgument("TEXT3",			3, true),
			TranslatedArgument("TEXT4",			4, true),
			TranslatedArgument("TEXT5",			5, true),
		})},
		{"PRINTSCREEN", FunctionWithTranslatedArgument("PRINTSCREEN", 5, {
			TranslatedArgument("TEXT",			1, true),
			TranslatedArgument("POS_X",			2, false),
			TranslatedArgument("POS_Y",			3, false),
			TranslatedArgument("FONT",			4, false),
			TranslatedArgument("TIME_SEC",		5, false),
		})},
		{"AI_PRINTSCREEN", FunctionWithTranslatedArgument("AI_PRINTSCREEN", 5, {
			TranslatedArgument("TEXT",			1, true),
			TranslatedArgument("POS_X",			2, false),
			TranslatedArgument("POS_Y",			3, false),
			TranslatedArgument("FONT",			4, false),
			TranslatedArgument("TIME_SEC",		5, false),
		})},
		{"INTRODUCECHAPTER", FunctionWithTranslatedArgument("INTRODUCECHAPTER", 5, {
			TranslatedArgument("CHAPTER",		1, true),
			TranslatedArgument("TEXT",			2, true),
			TranslatedArgument("TEXTURE",		3, false),
			TranslatedArgument("SOUND",			4, false),
			TranslatedArgument("TIME_SEC",		5, false),
		})},
		{"INFO_ADDCHOICE", FunctionWithTranslatedArgument("INFO_ADDCHOICE", 3, {
			TranslatedArgument("DIALOG",		1, false),
			TranslatedArgument("TEXT",			2, true),
			TranslatedArgument("FNC",			3, false),
		})},
		{"DOC_PRINTLINE", FunctionWithTranslatedArgument("DOC_PRINTLINE", 3, {
			TranslatedArgument("DOC_ID",		1, false),
			TranslatedArgument("PAGE",			2, false),
			TranslatedArgument("TEXT",			3, true),
		})},
		{"DOC_PRINTLINES", FunctionWithTranslatedArgument("DOC_PRINTLINES", 3, {
			TranslatedArgument("DOC_ID",		1, false),
			TranslatedArgument("PAGE",			2, false),
			TranslatedArgument("TEXT",			3, true),
		})},
	};

	// https://auronen.cokoliv.eu/gmc/zengin/scripts/extenders/lego/tools/interface/
	std::map<std::string, FunctionWithTranslatedArgument> ikarus_function_with_translated_argument_map {
		{"PRINT_EXT", FunctionWithTranslatedArgument("PRINT_EXT", 6, {
			TranslatedArgument("X",				1, false),
			TranslatedArgument("Y",				2, false),
			TranslatedArgument("TEXT",			3, true),
			TranslatedArgument("FONT",			4, false),
			TranslatedArgument("COLOR",			5, false),
			TranslatedArgument("TIME",			6, false),
		})},
		{"PRINTS_EXT", FunctionWithTranslatedArgument("PRINTS_EXT", 2, {
			TranslatedArgument("TEXT",			1, true),
			TranslatedArgument("COLOR",			2, false),
		})},
		{"PRINTS", FunctionWithTranslatedArgument("PRINTS", 1, {
			TranslatedArgument("TEXT",			1, true),
		})},
	};

	struct EditStringArgument
	{
		std::string name = "ARG_?";
		uint64_t number = 0;
		bool is_edit = false;

		EditStringArgument() = default;
		EditStringArgument(std::string name, const uint64_t number, const bool is_edit)
			: name(std::move(name)), number(number), is_edit(is_edit) {};

		std::string to_string() const
		{
			std::string is_ed = is_edit ? "true" : "false";
			return "[EditStringArgument] name: " + name
				+ ", number: " + std::to_string(number)
				+ ", is_edit: " + is_ed;
		}
	};

	struct FunctionWithEditStringArgument
	{
		std::string name = "UNNAMED_FUNCTION";
		uint64_t argument_count = 0;
		std::vector<EditStringArgument> edit_string_argument_vector = {};
		std::vector<StackVariableTrace> inner_stack_variable_vector = {};

		FunctionWithEditStringArgument() = default;
		FunctionWithEditStringArgument(std::string name, const uint64_t argument_count, std::vector<EditStringArgument> esav)
			: name(std::move(name)), argument_count(argument_count), edit_string_argument_vector(std::move(esav)) {};
		FunctionWithEditStringArgument(std::string name, const uint64_t argument_count, std::vector<EditStringArgument> esav, std::vector<StackVariableTrace> svtv)
			: name(std::move(name)), argument_count(argument_count), edit_string_argument_vector(std::move(esav)), inner_stack_variable_vector(std::move(svtv)) {};

		std::string to_string() const
		{
			std::string edit_string_argument_vector_ret;
			for (auto & edit_string_argument: edit_string_argument_vector)
			{
				edit_string_argument_vector_ret += " " + edit_string_argument.to_string();
			}

			std::string inner_stack_variable_vector_ret;
			for (auto & inner_stack_variable: inner_stack_variable_vector)
			{
				inner_stack_variable_vector_ret += " " + inner_stack_variable.to_string();
			}

			return "[FunctionWithEditStringArgument] name: " + name
				+ ", argument_count: " + std::to_string(argument_count)
				+ ", edit_string_argument_vector:" + edit_string_argument_vector_ret
				+ ", inner_stack_variable_vector_ret:" + inner_stack_variable_vector_ret;
		}
	};

	std::map<std::string, FunctionWithEditStringArgument> function_with_edit_string_argument_map {
		{"CONCATSTRINGS", FunctionWithEditStringArgument("CONCATSTRINGS", 2, {
			EditStringArgument("STRING1",		1, true),
			EditStringArgument("STRING2",		2, true),
		})},
	};

	// fix via return from func

	// USEMELEEWEAPONRECIPE

	// func string CS2C(var string S1, var string S2, var int COUNT) {
	// var result_string = CONCATSTRINGS(S1, S2);
	// for (count)
	// 		result_string = CONCATSTRINGS(resulkt_string, S2);
	//  return result_string
	// };

	// func string CS2(var string S1, var string S2) {
	// 	return CONCATSTRINGS(S1, S2);
	// };

	// func string CS3(var string S1, var string S2, var string S3) {
	// 	return CS2(CS2(S1, S2), S3);
	// };

	// чекать по "50", "75"

	struct SourceData
	{
		std::string section_name;
		std::string function_name;
		uint64_t function_count = 0;
		std::string argument_name;
		uint64_t argument_number = 0;
		uint64_t argument_array_index = 0;
		bool is_array = false;

		SourceData() = default;
	};

	struct StackData
	{
		std::string name;
		int64_t index = -1;
		int64_t address_begin = 0;
		int64_t address_end = 0;
		int64_t size = 0;
		bool is_use_ai_function = false;

		StackData() = default;
	};

	std::map<uint64_t, StackData> script_address_stack_data_map = {};
	std::map<uint64_t, StackData> script_index_stack_data_map = {};
	std::map<std::string, StackData> script_name_stack_data_map = {};

	std::map<uint64_t, StackData> menu_address_stack_data_map = {};
	std::map<uint64_t, StackData> menu_index_stack_data_map = {};
	std::map<std::string, StackData> menu_name_stack_data_map = {};

	std::deque<StackVariableTrace> pushed_stack_variable_trace_vector = {};
	std::unordered_map<int64_t, StackVariableTrace> moved_variable_value_trace_map;

	std::vector<std::string> function_trace_tree_vector = {};

	// ...

	bool assign_symbol_check(zCPar_Symbol* symbol)
	{
		std::string symbol_name = (const char*)symbol->name;

		for (auto & assign_string : assign_string_vector)
		{
			if (assign_string == symbol_name)
				return true;
		}

		return false;
	};

	std::vector<std::vector<uint64_t>> call_symbol_check(zCPar_Symbol* function_symbol,
		const std::deque<int64_t>& pushed_string_variable_stack, uint64_t latest_call_address,
		const std::deque<uint64_t>& pushed_address_array_variable_stack)
	{
		std::vector<std::vector<uint64_t>> return_empty_value = {};

		std::string symbol_name = (const char*)function_symbol->name;
		// log << "\n";
		// log << "call_symbol_check" << "\n";
		// log << "call_symbol_check symbol_name: " << symbol_name << "\n";
		// log << "call_symbol_check latest_call_address: " << latest_call_address << "\n";

		for (auto& call_function_with_string_argument_vector : call_function_with_string_argument_vector_vector)
		{
			// if vector contain only function name - check only for function name
			if (call_function_with_string_argument_vector.size() < 2)
				continue;

			// log << "call_function_with_string_argument_vector.size(): " << call_function_with_string_argument_vector.size() << "\n";

			std::string function_name = std::any_cast<std::string>(call_function_with_string_argument_vector[0]);
			// log << "function_name: " << function_name << "\n";
			if (function_name != symbol_name)
				continue;

			// total arguments count, where argument is string
			uint64_t string_argument_count = std::any_cast<uint64_t>(call_function_with_string_argument_vector[1]);

			// anti crush check
			if (pushed_string_variable_stack.size() - string_argument_count < 0)
				return return_empty_value;

			// check for functions in arguments, address trick =/
			auto farthest_argument_address = pushed_address_array_variable_stack[pushed_address_array_variable_stack.size() - string_argument_count];
			// log << "farthest_argument_address.: " << farthest_argument_address << "\n";
			if (latest_call_address > farthest_argument_address)
				return return_empty_value;

			// log << "pushed_string_variable_stack.size(): " << pushed_string_variable_stack.size() << "\n";
			// log << "string_argument_count: " << string_argument_count << "\n";

			std::vector<uint64_t> translate_pushed_string_variable_stack_index_vector = {};
			std::vector<uint64_t> do_not_translate_pushed_string_variable_stack_index_vector = {};

			uint64_t function_string_argument_position = 1;
			for (uint64_t i = pushed_string_variable_stack.size() - string_argument_count; // 10 - 4 = 6, 6/9, 7/9, 8/9, 9/9
				i < pushed_string_variable_stack.size(); ++i) // i < 10
			{
				// log << "i: " << i << "\n";
				bool is_translate_flag = false;
				for (uint64_t j = 2; j < call_function_with_string_argument_vector.size(); ++j) // ind2/3 - ind3/3
				{
					uint64_t call_function_with_string_argument = std::any_cast<uint64_t>(call_function_with_string_argument_vector[j]); // 1, 2
					if (function_string_argument_position == call_function_with_string_argument)
					{
						translate_pushed_string_variable_stack_index_vector.push_back(i);
						is_translate_flag = true;
						break;
					}
				}

				if (is_translate_flag == false)
					do_not_translate_pushed_string_variable_stack_index_vector.push_back(i);

				function_string_argument_position++;
			}

			// log << "translate_pushed_string_variable_stack_index_vector: " << "\n";
			for (auto index : translate_pushed_string_variable_stack_index_vector)
			{
				// log << "index: " << index << ", sym id: " << pushed_string_variable_stack[index] << "\n";
			}

			// log << "do_not_translate_pushed_string_variable_stack_index_vector: " << "\n";
			for (auto index : do_not_translate_pushed_string_variable_stack_index_vector)
			{
				// log << "index: " << index << ", sym id: " << pushed_string_variable_stack[index] << "\n";
			}

			return {translate_pushed_string_variable_stack_index_vector, do_not_translate_pushed_string_variable_stack_index_vector};
		}

		return {};
	};

	bool check_symbol_for_constant_string(zCPar_Symbol* symbol)
	{
		if (symbol->type != zPAR_TYPE_STRING)
			return false;

		if (!symbol->HasFlag(zPAR_FLAG_CONST))
			return false;

		if (symbol->ele != 1)
			return false;

		return true;
	};

	bool check_symbol_for_argument_or_local_variable(zCPar_Symbol* symbol)
	{
		if (symbol->type != zPAR_TYPE_STRING)
			return false;

		if (symbol->flags != 0)
			return false;

		if (symbol->ele != 1)
			return false;

		return true;
	};

	std::string encode_string(const std::string& string, const std::string& enc_from, const std::string& enc_to)
	{
		std::string encoded_string;
		uint64_t cp = 0;

		if (enc_from == "utf8")
		{
			try {
				cp = std::stoi(enc_to);
			} catch (...) {}
			if (cp >= 1250 && cp <= 1254)
				encoded_string = utf8_to_cp(string, cp);
		}
		else if (enc_to == "utf8")
		{
			try {
				cp = std::stoi(enc_from);
			} catch (...) {}
			if (cp >= 1250 && cp <= 1254)
				encoded_string = cp_to_utf8(string, cp);
		}

		return encoded_string;
	};

	uint64_t get_stack_section_address(std::unordered_map<uint64_t, std::string>& map,
		uint64_t virtual_address, bool is_next = false)
	{
		uint64_t nearest_virtual_address = 0;
		uint64_t nearest_next_virtual_address = 0xFFFFFFFF;
		for (const auto& [virtual_adr, name] : map)
		{
			if (virtual_adr > virtual_address)
			{
				nearest_next_virtual_address = min(nearest_next_virtual_address, virtual_adr);
			}
			else
			{
				nearest_virtual_address = max(nearest_virtual_address, virtual_adr);
			}
		}
		return is_next ? nearest_next_virtual_address : nearest_virtual_address;
	}

	std::string get_stack_section_name(std::unordered_map<uint64_t, std::string>& map, uint64_t virtual_address)
	{
		auto nearest_virtual_address = get_stack_section_address(map, virtual_address);

		std::string stack_section_name = map[nearest_virtual_address];

		return stack_section_name;
	}

	std::string create_source(const std::string& section_name, const std::string& symbol_name, int64_t function_count,
		const std::string& variable_name, uint64_t index = 0, uint64_t argument_number = -1)
	{
		std::string variable_name_temp = variable_name.substr(1);
		int64_t raw_name = 0;
		try {
			raw_name = std::stoi(variable_name_temp);
		} catch (...) {}

		std::string source = "[";
		if (raw_name >= 10000)
		{
			source += "RAW_STRING";
		}
		else
		{
			source += variable_name;
		}
		source += "] " + section_name + " -> ";

		if (function_count > 0)
		{
			source += "#" + std::to_string(function_count) + " ";
		}

		source += symbol_name;

		if (index > 0)
		{
			source += " [" + std::to_string(index) + "]";
		}

		if (argument_number > 0)
		{
			source += " -> ARGUMENT " + std::to_string(argument_number);
		}

		return source;
	}

	std::string create_source_v2(const std::string & symbol_name, const SourceData & source_data)
	{
		std::string variable_name_temp = symbol_name.substr(1);
		int64_t raw_name = 0;
		try {
			raw_name = std::stoi(variable_name_temp);
		} catch (...) {}

		std::string source = "[";
		if (raw_name >= 10000)
		{
			source += "RAW_STRING";
		}
		else
		{
			auto variable_name_spliped_vector = split_string(symbol_name, ".");
			if (variable_name_spliped_vector.size() == 2)
			{
				source += variable_name_spliped_vector[variable_name_spliped_vector.size() - 1];
			}
			else
			{
				source += symbol_name;
			}
		}
		source += "] " + source_data.section_name;

		if (!source_data.function_name.empty())
			source += " -> " + source_data.function_name;

		if (source_data.function_count >= 2)
			source += " #" + std::to_string(source_data.function_count) + " ";

		auto argument_name_splited_vector = split_string(source_data.argument_name, ".");
		std::string & argument_name = argument_name_splited_vector[argument_name_splited_vector.size() - 1];

		source += " -> " + argument_name;

		if (source_data.argument_array_index >= 0 && source_data.is_array)
		{
			source += " [" + std::to_string(source_data.argument_array_index) + "]";
		}

		return source;
	}

	bool is_need_to_translate(const std::string &string_data)
	{
		if (string_data.empty())
			return false;

		if (string_data == " " || string_data == "  " || string_data == "   ")
			return false;

		if (string_data == "\n" || string_data == " \n" || string_data == "  \n")
			return false;

		if (string_data.find('$') == 0)
			return false;

		if (string_data.find('_') != std::string::npos)
			return false;

		if (string_data.size() >= 4)
		{
			bool dot_check = string_data.substr(string_data.size() - 4, 1) == ".";
			bool space1_check = string_data.substr(string_data.size() - 3, 1) == " ";
			bool space2_check = string_data.substr(string_data.size() - 2, 1) == " ";
			bool space3_check = string_data.substr(string_data.size() - 1, 1) == " ";
			bool dot_next_check = string_data.substr(string_data.size() - 3, 1) == ".";
			if (dot_check && !dot_next_check && !space1_check && !space2_check && !space3_check)
				return false;
		}

		std::size_t pos = -1;
		bool is_only_numbers = true;
		try {
			int64_t number_result = std::stoi(string_data, &pos);
		} catch (...) {is_only_numbers = false;}

		if (is_only_numbers && pos == string_data.size() - 1) //work mb...
			return false;

		return true;
	}

	uint64_t calculate_translate_value(const std::string& string_name, const std::string& string_data)
	{
		if (string_name == "TXT_TALENTS")
			return 100;

		if (string_name == "TXT_SPELLS")
			return 100;

		if (string_name == "TXT_TALENTS_SKILLS")
			return 100;

		if (string_name == "TXT_GUILDS")
			return 100;

		if (string_name.find("PRINT") != std::string::npos)
			return 75;

		if (string_name.substr(0, 8) == "MOBNAME_")
			return 75;

		if (string_data.find('_') != std::string::npos)
			return 0;

		return 50;
	}

	bool script_string_data_add(const std::string& text, std::string& source, const std::string& translate_value,
		const std::string& symbol_table_id, const std::string& array_id, const bool is_menu = false)
	{
		if (!is_need_to_translate(text))
			return false;

		// for example in G2A from ou getting umlauts...
		remove_broken_symbols(source);

		auto & string_data_insert_order =
			is_menu ? menu_string_data_insert_order : script_string_data_insert_order;
		auto & string_data = is_menu ? menu_string_data : script_string_data;

		if (string_data.count(text) <= 0)
		{
			string_data_insert_order.push_back(text);
			string_data[text]["translate_source"] = source;
			string_data[text]["translate_value"] = translate_value;
			string_data[text]["symbol_table_id"] = symbol_table_id;
			string_data[text]["array_id"] = array_id;
		}
		else
		{
			auto& string_entry = string_data[text];
			for (uint64_t i = 1; i < 9999; ++i)
			{
				std::string translate_source_with_num = "translate_source";
				if (i > 1)
					translate_source_with_num += std::to_string(i);

				std::string translate_value_with_num = "translate_value";
				if (i > 1)
					translate_value_with_num += std::to_string(i);

				std::string symbol_table_id_with_num = "symbol_table_id";
				if (i > 1)
					symbol_table_id_with_num += std::to_string(i);

				std::string array_id_with_num = "array_id";
				if (i > 1)
					array_id_with_num += std::to_string(i);

				// just rewrite, raw string with extended source
				if (string_entry[symbol_table_id_with_num] == symbol_table_id
					&& string_entry[array_id_with_num] == array_id)
				{
					std::string & source_old = string_entry[translate_source_with_num];
					if (source_old.size() > 7 && source_old.substr(source_old.size() - 7) == "-> NAME")
					{
						// don't change object name, name is priority
						break;
					}

					string_entry[translate_value_with_num] = translate_value;
					string_entry[translate_source_with_num] = source;
					break;
				}

				if (i > 1 && string_entry.count(translate_source_with_num) <= 0)
				{
					string_entry[translate_value_with_num] = translate_value;
					string_entry[translate_source_with_num] = source;
					string_entry[symbol_table_id_with_num] = symbol_table_id;
					string_entry[array_id_with_num] = array_id;
					break;
				}
			}
		}

		return true;
	}

	bool menu_string_data_add(const std::string& text, std::string& source)
	{
		if (!is_need_to_translate(text))
			return false;

		// if (std::count(menu_string_data_insert_order.begin(), menu_string_data_insert_order.end(), text))
		// 	return false;

		remove_broken_symbols(source);
		menu_string_data_insert_order.push_back(text);
		menu_string_data[text]["translate_source"] = source; // if same name, other source?
		menu_string_data[text]["translated_string"] = "";

		return true;
	}

	bool parse_and_add_function_with_translated_argument(const std::string& string_data)
	{
		uint64_t size = string_data.length() + 1;
		char* buffer = new char[size];
		strcpy_s(buffer, size, string_data.c_str());
		char* context = nullptr;
		char* token = strtok_s(buffer, ",", &context);

		std::vector<std::string> tokens;
		while (token != nullptr)
		{
			tokens.emplace_back(token);
			token = strtok_s(nullptr, ",", &context);
		}

		std::vector<std::any> call_function_with_string_argument_vector = {};
		for (uint64_t i = 0; i < tokens.size(); ++i)
		{
			tokens[i].erase(tokens[i].begin(), std::find_if(tokens[i].begin(), tokens[i].end(), [](unsigned char ch)
			{
				return !std::isspace(ch);
			}));
			tokens[i].erase(std::find_if(tokens[i].rbegin(), tokens[i].rend(), [](unsigned char ch)
			{
				return !std::isspace(ch);
			}).base(), tokens[i].end());

			if (i == 0)
			{
				std::transform(tokens[i].begin(), tokens[i].end(), tokens[i].begin(), [](unsigned char c)
				{
					return std::toupper(c);
				});
				call_function_with_string_argument_vector.emplace_back(tokens[i]);
			}
			else if (i > 0)
			{
				uint64_t arg_num = 0;
				try {
					arg_num = std::stoi(tokens[i]);
				} catch (...) {}
				if (arg_num > 0)
				{
					call_function_with_string_argument_vector.emplace_back(static_cast<uint64_t>(arg_num));
				}
				else
				{
					return false;
				}
			}
		}

		if (call_function_with_string_argument_vector.size() > 1)
		{
			bool already_added = false;
			for (auto& call_function_with_string_argument_vector_ : call_function_with_string_argument_vector_vector)
			{
				if (std::any_cast<std::string>(call_function_with_string_argument_vector_[0])
					== std::any_cast<std::string>(call_function_with_string_argument_vector[0]))
				{
					already_added = true;
					break;
				}
			}
			if (!already_added)
				call_function_with_string_argument_vector_vector.emplace_back(call_function_with_string_argument_vector);
		}

		return true;
	}

	std::string read_json_object_to_map(TinyJSON::TJValueObject* json_object, std::unordered_map<std::string, std::string>& map)
	{
		auto json_string = json_object->try_get_value("original_string");
		if (!json_string || !json_string->is_string())
			return "";

		for (uint64_t i = 1; i <= 9999; ++i) {
			std::string translate_source_with_num = "translate_source";
			if (i > 1)
				translate_source_with_num += std::to_string(i);

			auto json_source = json_object->try_get_value(translate_source_with_num.c_str());
			if (json_source && json_source->is_string())
			{
				map[translate_source_with_num] = json_source->get_string();
				// log << "translate_source: " << map[translate_source_with_num] << std::endl;
			}
			else
				break;

			// ...

			std::string translate_value_with_num = "translate_value";
			if (i > 1)
			 	translate_value_with_num += std::to_string(i);

			auto json_translate_value = json_object->try_get_value(translate_value_with_num.c_str());
			if (json_translate_value && json_translate_value->is_string())
				map[translate_value_with_num] = json_translate_value->get_string();

			// ...

			std::string translated_string_with_num = "translated_string";
			if (i > 1)
			 	translated_string_with_num += std::to_string(i);

			auto json_translated_string = json_object->try_get_value(translated_string_with_num.c_str());
			if (json_translated_string && json_translated_string->is_string())
				map[translated_string_with_num] = json_translated_string->get_string();
		}

		// log << "original_string: " << json_string->get_string() << std::endl;
		return json_string->get_string();
	}

	bool read_json()
	{
		TinyJSON::parse_options options = {};
		options.throw_exception = false;

		auto json = TinyJSON::TJ::parse_file(selected_language_data.full_path_to_translate_file.c_str(), options);
		if (!json || !json->is_object())
		{
			// log << "cannot open JSON file, or json file is not valid, path: " << selected_language_data.full_path_to_translate_file.c_str() << std::endl;
			return false;
		}

		// Clear old data
		translate_string_data_vmh_insert_order.clear();
		translate_string_data.clear();

		ou_string_data_insert_order.clear();
		ou_string_data.clear();

		menu_string_data_insert_order.clear();
		menu_string_data.clear();

		auto json_object = dynamic_cast<TinyJSON::TJValueObject*>(json);

		auto json_virtual_machine_hash = json_object->try_get_value("virtual_machine_hash", false);
		if (json_virtual_machine_hash && json_virtual_machine_hash->is_string())
			json_settings["virtual_machine_hash"] = json_virtual_machine_hash->get_string();

		// log << "read_json translate_file_path: " << translate_file_path.string() << " \n";
		// log << "read_json json_virtual_machine_hash: " << json_virtual_machine_hash->get_string() << " \n";
		// log << "read_json json_virtual_machine_hash: " << json_settings["virtual_machine_hash"] << " \n";
		// log.flush();

		auto json_translate_name = json_object->try_get_value("translate_name", false);
		if (json_translate_name && json_translate_name->is_string())
			json_settings["translate_name"] = json_translate_name->get_string();

		auto json_script_encoding = json_object->try_get_value("script_encoding", false);
		if (json_script_encoding && json_script_encoding->is_string())
			json_settings["script_encoding"] = json_script_encoding->get_string();

		auto json_min_translate_value = json_object->try_get_value("min_translate_value", false);
		if (json_min_translate_value && json_min_translate_value->is_string())
			json_settings["min_translate_value"] = json_min_translate_value->get_string();

		// std::vector<std::string> class_with_translated_fields = {
		// 	"C_INFO.DESCRIPTION",
		// 	"C_NPC.NAME",
		// 	"C_ITEM.NAME", "C_ITEM.DESCRIPTION", "C_ITEM.TEXT",
		// 	"C_MENU_ITEM.TEXT",
		// };
		// std::vector<std::string> assign_string_vector = {};

		// auto json_class_with_translated_fields = new TinyJSON::TJValueArray();
		// for (auto& class_with_translated_field : class_with_translated_fields)
		// 	json_class_with_translated_fields->add_string(class_with_translated_field.c_str());
		// json->set("class_with_translated_fields", json_class_with_translated_fields);
		//
		// auto json_functions_with_translated_argument = new TinyJSON::TJValueArray();
		// for (auto& function_with_translated_argument : functions_with_translated_argument)
		// 	json_functions_with_translated_argument->add_string(function_with_translated_argument.c_str());
		// json->set("functions_with_translated_argument", json_functions_with_translated_argument);


		auto json_string_array_ = json_object->try_get_value("script", false);
		if (!json_string_array_ || !json_string_array_->is_array())
		{
			return false;
		}

		auto json_ou_array_ = json_object->try_get_value("ou", false);
		if (!json_ou_array_ || !json_ou_array_->is_array())
		{
			return false;
		}

		auto json_menu_array_ = json_object->try_get_value("menu", false);
		if (!json_menu_array_ || !json_menu_array_->is_array())
		{
			return false;
		}

		auto json_string_array = dynamic_cast<const TinyJSON::TJValueArray*>(json_string_array_);
		for (int64_t i = 0; i < json_string_array->get_number_of_items(); ++i)
		{
			auto json_string_element = json_string_array->at(i);
			if (!json_string_element->is_object())
				continue;

			auto json_object_string_element = dynamic_cast<TinyJSON::TJValueObject*>(json_string_element);

			//...

			std::string vmh = std::string(json_settings["virtual_machine_hash"]); // need a copy
			auto json_vmh = json_object_string_element->try_get_value("virtual_machine_hash");
			if (json_vmh && json_vmh->is_string())
				vmh = json_vmh->get_string();
			// MyFile << "vmh " << vmh << "\n";

			//...

			auto json_string = json_object_string_element->try_get_value("original_string");
			if (!json_string || !json_string->is_string())
				continue;

			auto string = json_string->get_string();
			// MyFile << "string " << i << " " << string << "\n";

			// log << "json string: " << string << ", bytes: " << const_char_to_string_bytes(string) << "\n";

			// ...

			 for (uint64_t j = 1; j <= 9999; ++j)
			 {
			 	std::string translate_source_with_num = "translate_source";
			 	if (j > 1)
			 		translate_source_with_num += std::to_string(j);

			 	auto json_source = json_object_string_element->try_get_value(translate_source_with_num.c_str());
			 	if (!json_source || !json_source->is_string())
			 		break;

			 	auto source = json_source->get_string();
			 	// MyFile << "source " << source << "\n";

			 	// ...

			 	std::string translate_value_with_num = "translate_value";
			 	if (j > 1)
			 		translate_value_with_num += std::to_string(j);

			 	auto json_translate_value = json_object_string_element->try_get_value(translate_value_with_num.c_str());
			 	if (!json_translate_value || !json_translate_value->is_string())
			 		break;

			 	auto translate_value = json_translate_value->get_string();
			 	// MyFile << "translate_value " << translate_value << "\n";

			 	//...

			 	std::string translated_string_with_num = "translated_string";
			 	if (j > 1)
			 		translated_string_with_num += std::to_string(j);

			 	auto json_translated_string = json_object_string_element->try_get_value(translated_string_with_num.c_str());
			 	if (!json_translated_string || !json_translated_string->is_string())
			 		break;

			 	auto translated_string = json_translated_string->get_string();

			 	std::string encoded_string = string;
			 	// 1251 = russian
			 	// 1252 = german

			 	uint64_t cp = std::stoi(json_settings["script_encoding"]);
			 	if (cp >= 1250 && cp <= 1254)
			 		encoded_string = utf8_to_cp(string, cp);

			 	// log << "json string enc: " << encoded_string << ", bytes: " << const_char_to_string_bytes(encoded_string.c_str()) << "\n";

			 	// for ordering
			 	if (std::count(translate_string_data_string_insert_order.begin(), translate_string_data_string_insert_order.end(), encoded_string) <= 0)
			 		translate_string_data_string_insert_order.push_back(encoded_string);

			 	if (std::count(translate_string_data_vmh_insert_order[encoded_string].begin(),
			 		translate_string_data_vmh_insert_order[encoded_string].end(), vmh) <= 0)
			 		translate_string_data_vmh_insert_order[encoded_string].push_back(vmh);

			 	translate_string_data[encoded_string][vmh][translate_source_with_num] = source;
			 	translate_string_data[encoded_string][vmh][translate_value_with_num] = translate_value;
			 	translate_string_data[encoded_string][vmh][translated_string_with_num] = translated_string;
			 }
		}

		// ...

		auto json_ou_array = dynamic_cast<const TinyJSON::TJValueArray*>(json_ou_array_);
		for (int64_t i = 0; i < json_ou_array->get_number_of_items(); ++i)
		{
			auto json_ou_element = json_ou_array->at(i);
			if (!json_ou_element->is_object())
				continue;

			auto json_object_ou_element = dynamic_cast<TinyJSON::TJValueObject*>(json_ou_element);

			std::unordered_map<std::string, std::string> map;
			auto string = read_json_object_to_map(json_object_ou_element, map);
			if (string.empty())
				continue;

			std::string encoded_string = string;
			uint64_t cp = std::stoi(json_settings["script_encoding"]);
			if (cp >= 1250 && cp <= 1254)
				encoded_string = utf8_to_cp(string, cp);

			ou_string_data_insert_order.emplace_back(encoded_string);
			ou_string_data[encoded_string] = map;
		}

		// ...

		auto json_menu_array = dynamic_cast<const TinyJSON::TJValueArray*>(json_menu_array_);
		for (int64_t i = 0; i < json_menu_array->get_number_of_items(); ++i)
		{
			auto json_menu_element = json_menu_array->at(i);
			if (!json_menu_element->is_object())
				continue;

			auto json_object_menu_element = dynamic_cast<TinyJSON::TJValueObject*>(json_menu_element);

			std::unordered_map<std::string, std::string> map;
			auto string = read_json_object_to_map(json_object_menu_element, map);
			if (string.empty())
				continue;

			std::string encoded_string = string;
			uint64_t cp = std::stoi(json_settings["script_encoding"]);
			if (cp >= 1250 && cp <= 1254)
				encoded_string = utf8_to_cp(string, cp);

			menu_string_data_insert_order.emplace_back(encoded_string);
			menu_string_data[encoded_string] = map;
		}

		return true;
	}

	void write_map_to_json_object(TinyJSON::TJValueObject* json_object, const std::string& vmh,
		std::unordered_map<std::string, std::string>& map, const std::string& original_string,
		const std::string& script_encoding)
	{
		std::string string_utf8;
		uint64_t cp = std::stoi(script_encoding);
		if (cp >= 1250 && cp <= 1254)
			string_utf8 = cp_to_utf8(original_string, cp);

		json_object->set_string("original_string", string_utf8.c_str());

		if (!vmh.empty())
		{
			json_object->set_string("virtual_machine_hash", vmh.c_str());
		}

		for (uint64_t j = 1; j <= 9999; ++j)
		{
			std::string translate_source_with_num = "translate_source";
			if (j > 1)
				translate_source_with_num += std::to_string(j);

			bool is_source = map.count(translate_source_with_num) > 0;
			if (is_source)
			{
				json_object->set_string(translate_source_with_num.c_str(), map[translate_source_with_num].c_str());
			}

			if (!is_source)
				break;

			std::string translate_value_with_num = "translate_value";
			if (j > 1)
				translate_value_with_num += std::to_string(j);

			bool is_value = map.count(translate_value_with_num) > 0;
			if (is_value)
			{
				json_object->set_string(translate_value_with_num.c_str(), map[translate_value_with_num].c_str());
			}

			std::string translated_string_with_num = "translated_string";
			if (j > 1)
				translated_string_with_num += std::to_string(j);

			json_object->set_string(translated_string_with_num.c_str(), map[translated_string_with_num].c_str());


			// std::string symbol_table_id_with_num = "symbol_table_id";
			// if (j > 1)
			// 	symbol_table_id_with_num += std::to_string(j);
			// json_string->set_string(symbol_table_id_with_num.c_str(), string_entry[symbol_table_id_with_num].c_str());
			//
			// std::string array_id_with_num = "array_id";
			// if (j > 1)
			// 	array_id_with_num += std::to_string(j);
			// json_string->set_string(array_id_with_num.c_str(), string_entry[array_id_with_num].c_str());
		}
	}

	void write_ou(TinyJSON::TJValueArray* json_array, const std::string& script_encoding)
	{
		for (int i = 0; i < ou_string_data_insert_order.size(); ++i)
		{
			auto& string_key = ou_string_data_insert_order[i];
			auto& string_entry = ou_string_data[string_key];

			auto json_string  = new TinyJSON::TJValueObject();

			write_map_to_json_object(json_string, "", string_entry, string_key, script_encoding);

			json_array->add(json_string);
		}
	}

	void write_menu(TinyJSON::TJValueArray* json_array, const std::string& script_encoding)
	{
		for (int i = 0; i < menu_string_data_insert_order.size(); ++i)
		{
			auto& string_key = menu_string_data_insert_order[i];
			auto& string_entry = menu_string_data[string_key];

			auto json_string  = new TinyJSON::TJValueObject();

			write_map_to_json_object(json_string, "", string_entry, string_key, script_encoding);

			json_array->add(json_string);
		}
	}

	void write_json(bool is_json_read, const std::string& new_virtual_machine_hash)
	{
		TinyJSON::write_options options = {};
		options.throw_exception = false;
		options.write_formating = TinyJSON::formating::indented;
		options.byte_order_mark = TinyJSON::write_options::utf8;

		auto json = new TinyJSON::TJValueObject();

		json->set_string("virtual_machine_hash", new_virtual_machine_hash.c_str());
		json->set_string("translate_name", json_settings["translate_name"].c_str());
		json->set_string("script_encoding", json_settings["script_encoding"].c_str());
		json->set_string("min_translate_value", json_settings["min_translate_value"].c_str());

		// log << "write virtual_machine_hash: " << new_virtual_machine_hash << " \n";
		// log.flush();

		// auto json_class_with_translated_fields = new TinyJSON::TJValueArray();
		// for (auto& class_with_translated_field : class_with_translated_fields)
		// 	json_class_with_translated_fields->add_string(class_with_translated_field.c_str());
		// json->set("class_with_translated_fields", json_class_with_translated_fields);
		//
		// auto json_functions_with_translated_argument = new TinyJSON::TJValueArray();
		// for (auto& function_with_translated_argument : functions_with_translated_argument)
		// 	json_functions_with_translated_argument->add_string(function_with_translated_argument.c_str());
		// json->set("functions_with_translated_argument", json_functions_with_translated_argument);

		// if (is_settings_only)
		// {
		// 	// write json only with settings for hand encoding fix
		// 	TinyJSON::TJ::write_file(translate_file_path.c_str(), *json, options);
		// 	return;
		// }

		auto json_string_array = new TinyJSON::TJValueArray();
		auto json_ou_array = new TinyJSON::TJValueArray();
		auto json_menu_array = new TinyJSON::TJValueArray();

		if (!is_json_read)
		{
			for (int i = 0; i < script_string_data_insert_order.size(); ++i)
			{
				auto& string_key = script_string_data_insert_order[i];
				auto& string_entry = script_string_data[string_key];

				auto json_string  = new TinyJSON::TJValueObject();

				write_map_to_json_object(json_string, "", string_entry, string_key, json_settings["script_encoding"]);

				json_string_array->add(json_string);
			}
			json->set("script", json_string_array);

			write_ou(json_ou_array, json_settings["script_encoding"]);
			json->set("ou", json_ou_array);

			write_menu(json_menu_array, json_settings["script_encoding"]);
			json->set("menu", json_menu_array);

			TinyJSON::TJ::write_file(selected_language_data.full_path_to_translate_file.c_str(), *json, options);

			return;
		}

		// json file is already read

		// 3 round of circle
		// сравниваем со скриптом - и если находим разницу, добавляем новый перевод, везде ставим хэш с ордером
		// проходим и записываем те вариаеты - где только 1 хэш
		// проходим и записываем те вариаеты - где много хешей, в порядке хешей

		// log << "write_json, is_json_read: " << is_json_read << "\n";

		// Проверяем есть ли новые строчки в скрипте
		for (auto& [script_string_key, script_string_variables] : script_string_data)
		{
			// Стринга нет, в скрипте появился новый стринг
			if (translate_string_data.count(script_string_key) <= 0)
			{
				// Копируем? со всем ненужным, всё равно не пишем лишнего в файл
				// Но уже в новый хэш ???

				// log << "find new string in script: " << script_string_key << "\n";

				// for ordering
				if (std::count(translate_string_data_string_insert_order.begin(), translate_string_data_string_insert_order.end(), script_string_key) <= 0)
					translate_string_data_string_insert_order.push_back(script_string_key);

				if (std::count(translate_string_data_vmh_insert_order[script_string_key].begin(),
					translate_string_data_vmh_insert_order[script_string_key].end(), new_virtual_machine_hash) <= 0)
					translate_string_data_vmh_insert_order[script_string_key].push_back(new_virtual_machine_hash);

				translate_string_data[script_string_key][new_virtual_machine_hash] = script_string_variables;
			}
			// Стринг уже есть, сравниваем...
			else
			{
				auto& vmh_from_file = json_settings["virtual_machine_hash"];
				auto& translate_string_variables = translate_string_data[script_string_key][vmh_from_file];

				bool is_same_sources = true;
				for (uint64_t i = 1; i < 9999; ++i)
				{
					std::string translate_source_with_num = "translate_source";
					if (i > 1)
						translate_source_with_num += std::to_string(i);

					bool is_in_script = script_string_variables.count(translate_source_with_num);
					bool is_in_translate = translate_string_variables.count(translate_source_with_num);

					if (!is_in_script && !is_in_translate)
						break;

					// где то есть - где то нет
					if ((!is_in_script && is_in_translate) || (is_in_script && !is_in_translate))
					{
						is_same_sources = false;
						break;
					}

					// разные истоники на одной позиции
					if (script_string_variables[translate_source_with_num] != translate_string_variables[translate_source_with_num])
					{
						is_same_sources = false;
						break;
					}
				}

				if (!is_same_sources)
				{
					// log << "find diff string in script: " << script_string_key << "\n";

					// for ordering
					if (std::count(translate_string_data_string_insert_order.begin(), translate_string_data_string_insert_order.end(), script_string_key) <= 0)
						translate_string_data_string_insert_order.push_back(script_string_key);

					if (std::count(translate_string_data_vmh_insert_order[script_string_key].begin(),
						translate_string_data_vmh_insert_order[script_string_key].end(), new_virtual_machine_hash) <= 0)
						translate_string_data_vmh_insert_order[script_string_key].push_back(new_virtual_machine_hash);

					// Копируем? со всем ненужным, всё равно не пишем лишнего в файл
					// Но уже в новый хэш ???
					translate_string_data[script_string_key][new_virtual_machine_hash] = script_string_variables;
				}
			}
		}

		// Проверяем есть ли в актуальном переводе строки которых нет в скрипте
		// for (auto& [translate_string_key, translate_string_entry] : translate_string_data[])
		// {
		// }

		// сначала пишет только старые строки, где нет новых ВМ
		for (int i = 0; i < translate_string_data_string_insert_order.size(); ++i)
		{
			auto& translate_string_key = translate_string_data_string_insert_order[i];
			// auto& translate_string_entry = translate_string_data[translate_string_key];

			uint64_t vmh_count = translate_string_data_vmh_insert_order[translate_string_key].size();
			if (vmh_count <= 0)
				continue;

			if (vmh_count > 1)
				continue;

			// if vmh_count == 1, execute next code

			auto& translate_vmh_key = translate_string_data_vmh_insert_order[translate_string_key][0];

			// write only "old" script string
			if (translate_vmh_key == new_virtual_machine_hash)
				continue;

			auto& vmh_entry = translate_string_data[translate_string_key][translate_vmh_key];

			auto json_string  = new TinyJSON::TJValueObject();

			write_map_to_json_object(json_string, "", vmh_entry, translate_string_key,
				json_settings["script_encoding"]);

			json_string_array->add(json_string);
		}

		// потом пишем только новые строки, где есть новый VMH
		for (int i = 0; i < translate_string_data_string_insert_order.size(); ++i)
		{
			auto& translate_string_key = translate_string_data_string_insert_order[i];
			// auto& translate_string_entry = translate_string_data[translate_string_key];

			uint64_t vmh_count = translate_string_data_vmh_insert_order[translate_string_key].size();
			if (vmh_count <= 0)
				continue;

			if (vmh_count == 1)
			{
				// строки где только старый VMH - скипаем
				if (translate_string_data_vmh_insert_order[translate_string_key][0] != new_virtual_machine_hash)
					continue;
			}

			for (int j = 0; j < translate_string_data_vmh_insert_order[translate_string_key].size(); ++j)
			{
				auto& translate_vmh_key = translate_string_data_vmh_insert_order[translate_string_key][j];

				auto& vmh_entry = translate_string_data[translate_string_key][translate_vmh_key];

				auto json_string  = new TinyJSON::TJValueObject();

				write_map_to_json_object(json_string, translate_vmh_key, vmh_entry, translate_string_key,
					json_settings["script_encoding"]);

				json_string_array->add(json_string);
			}
		}

		json->set("script", json_string_array);

		write_ou(json_ou_array, json_settings["script_encoding"]);
		json->set("ou", json_ou_array);

		write_menu(json_menu_array, json_settings["script_encoding"]);
		json->set("menu", json_menu_array);

		TinyJSON::TJ::write_file(selected_language_data.full_path_to_translate_file.c_str(), *json, options);
	}

	void parse_stack_for_string_in_arguments(byte* bytecode, uint64_t address_begin, uint64_t argument_count)
	{
		// log << "parse_stack_for_macros" << "\n";
		// log << "parse_stack_for_macros address_begin: " << address_begin << "\n";
		// log << "parse_stack_for_macros func name: " << virtual_address_name_map[address_begin] << "\n";
		// log << "parse_stack_for_macros size: " << argument_count * 64 << "\n";
		// log << "parse_stack_for_macros argument_count: " << argument_count << "\n";

		// std::string("LOG_ADDENTRY"), static_cast<uint64_t>(2), static_cast<uint64_t>(1), static_cast<uint64_t>(2)

		// auto stack_section_name = get_stack_section_name(virtual_address_name_map, address_begin);
		std::vector<std::any> call_function_with_string_argument_vector = {
			virtual_address_name_map[address_begin], static_cast<uint64_t>(0)
		};

		// log << "parse_stack_for_macros stack_section_name: " << stack_section_name << "\n";

		std::deque<int64_t> pushed_string_variable_stack = {};

		// req ids can be local variable
		uint64_t pushed_argument_counter = 0;

		for (uint64_t i = address_begin; i < address_begin + (argument_count * 64); ++i)
		{
			int64_t token = bytecode[i];

			if (pushed_argument_counter >= argument_count)
			{
				if (std::any_cast<uint64_t>(call_function_with_string_argument_vector[1]) > 0)
				{
					// call_function_with_string_argument_vector_vector.emplace_back(call_function_with_string_argument_vector);
					// log << "parse_stack_for_macros ADD: " << virtual_address_name_map[address_begin] <<
					// 	" str arg "<< std::any_cast<uint64_t>(call_function_with_string_argument_vector[1]) << "\n";
				}

				return;
			}

			if (token == zPAR_TOK_RET)
				break;
			else if (token == zPAR_TOK_PUSHVAR + zPAR_TOK_FLAGARRAY)
				i += 5;
			else if (token == zPAR_TOK_CALLEXTERN
				||token == zPAR_TOK_PUSHINT
				|| token == zPAR_TOK_CALL
				|| token == zPAR_TOK_JUMP
				|| token == zPAR_TOK_JUMPF
				|| token == zPAR_TOK_SETINSTANCE)
				i += 4;
			else if (token == zPAR_TOK_PUSHINST)
			{
				// log << "parse_stack_for_macros TOK_PUSHINST pushed_argument_counter += 1" << "\n";

				pushed_argument_counter += 1;
				i += 4;
			}
			else if (token == zPAR_TOK_PUSHVAR)
			{
				int64_t symbol_index = get_token_next_bytes(bytecode, i);
				zCPar_Symbol* symbol = parser->symtab.table[symbol_index];
				std::string symbol_name = (const char*)symbol->name;

				// pushed_string_variable_stack.push_back(symbol_index);

				if (symbol->type == zPAR_TYPE_STRING)
				{
					call_function_with_string_argument_vector[1] = std::any_cast<uint64_t>(call_function_with_string_argument_vector[1]) + 1;
				}


				// log << "parse_stack_for_macros TOK_PUSHVAR pushed_argument_counter += 1, var id " << symbol_index << ", name " << symbol_name <<"\n";

				pushed_argument_counter += 1;
				i += 4;
			}
			// else if (token == zPAR_TOK_ASSIGNSTR) // 0x46, MOVS
			// {
			// 	// if (pushed_argument_counter > argument_count)
			// 	// 	continue;
			//
			// 	// int64_t to_symbol_index = pushed_string_variable_stack[pushed_string_variable_stack.size() - 1];
			// 	// zCPar_Symbol* to_symbol = parser->symtab.table[to_symbol_index];
			// 	//
			// 	// std::string symbol_name = (const char*)to_symbol->name;
			//
			// 	log << "parse_stack_for_macros TOK_ASSIGNSTR"  << "\n";
			//
			// 	call_function_with_string_argument_vector[1] = std::any_cast<uint64_t>(call_function_with_string_argument_vector[1]) + 1;
			// 	// log << "parse_stack_for_macros TOK_ASSIGNSTR symbol_index: " << to_symbol_index << ", symbol_name: " << symbol_name << "\n";
			// 	// log << "parse_stack_for_macros TOK_ASSIGNSTR string_arg_count: " << std::any_cast<uint64_t>(call_function_with_string_argument_vector[1]) << "\n";
			// }
		}
	}

	void parse_stack(byte* bytecode, uint64_t max, zCParser* loc_parser, bool is_menu = false)
	{
		std::deque<int64_t> pushed_string_index_variable_stack = {};
		std::deque<uint64_t> pushed_string_array_index_variable_stack = {};
		std::deque<uint64_t> pushed_string_address_array_variable_stack = {};

		uint64_t latest_call_address = 0;

		std::string section_name;
		std::unordered_map<std::string, std::unordered_map<std::string, int64_t>> function_counter;

		for (uint64_t i = 0; i < max; ++i)
		{
			int64_t token = bytecode[i];

			if (is_menu)
			{
				if (menu_virtual_address_name_map.count(i))
					section_name = menu_virtual_address_name_map[i];
			}
			else
			{
				if (virtual_address_name_map.count(i))
					section_name = virtual_address_name_map[i];
			}


			if (token == zPAR_TOK_PUSHVAR + zPAR_TOK_FLAGARRAY)
			{
				// VARIABLE !!!

				int64_t symbol_index = get_token_next_bytes(bytecode, i);
				std::string symbol_name = (const char*)loc_parser->symtab.table[symbol_index]->name;
				uint64_t array_index = bytecode[i + 5];

				if (loc_parser->symtab.table[symbol_index]->type != zPAR_TYPE_STRING)
				{
					i += 5;
					continue;
				}

				if (pushed_string_index_variable_stack.size() >= 100)
					pushed_string_index_variable_stack.pop_front();
				pushed_string_index_variable_stack.push_back(symbol_index);

				if (pushed_string_array_index_variable_stack.size() >= 100)
					pushed_string_array_index_variable_stack.pop_front();
				pushed_string_array_index_variable_stack.push_back(array_index);

				if (pushed_string_address_array_variable_stack.size() >= 100)
					pushed_string_address_array_variable_stack.pop_front();
				pushed_string_address_array_variable_stack.push_back(i);

				i += 5;
			}
			else if (token == zPAR_TOK_CALLEXTERN || token == zPAR_TOK_CALL) // 0x3E || 0x3D
			{
				if (is_menu)
				{
					i += 4;
					continue;
				}

				int64_t function_symbol_index = 0;
				if (token == zPAR_TOK_CALLEXTERN)
				{
					function_symbol_index = get_token_next_bytes(bytecode, i);
				}
				else
				{
					function_symbol_index = get_token_next_bytes(bytecode, i);
					function_symbol_index = virtual_address_index_map[function_symbol_index];
				}



				auto function_symbol = loc_parser->symtab.table[function_symbol_index];
				std::string function_name = (const char*)function_symbol->name;

				function_counter[section_name][function_name] += 1;
				int64_t function_count = function_counter[section_name][function_name];

				// log << "function_count section_name: " << section_name << "\n";
				// log << "function_count function_name: " << function_name << "\n";
				// log << "function_count function_count: " << function_count << "\n";
				// log << "\n";

				// log << "TOK_CALL function_name: " << function_name << "\n";
				// log << "TOK_CALL latest_call_address: " << latest_call_address << "\n";


				// auto address_begin = get_stack_section_address(virtual_address_name_map, i);
				// auto arg_count = function_symbol->ele;

				if (!function_symbol->HasFlag(zPAR_FLAG_EXTERNAL))
				{
					auto address_begin = get_token_next_bytes(bytecode, i);
					parse_stack_for_string_in_arguments(bytecode, address_begin, function_symbol->ele);
				}


				std::vector<std::vector<uint64_t>> pushed_string_variable_stack_index_vector_vector =
					call_symbol_check(function_symbol, pushed_string_index_variable_stack,
						latest_call_address, pushed_string_address_array_variable_stack);

				// latest_call_address = i;
				// i += 4;
				// continue;

				if (pushed_string_variable_stack_index_vector_vector.size() == 2)
				{
					for (auto& pushed_string_variable_stack_index_vector : pushed_string_variable_stack_index_vector_vector[0])
					{
						int64_t argument_symbol_index = pushed_string_index_variable_stack[pushed_string_variable_stack_index_vector];
						uint64_t argument_array_index = pushed_string_array_index_variable_stack[pushed_string_variable_stack_index_vector];
						zCPar_Symbol* argument_symbol = loc_parser->symtab.table[argument_symbol_index];

						// log << "TOK_CALL latest_call_address: " << latest_call_address << "\n";
						// log << "TOK_CALL argument_symbol name: " << argument_symbol->name << "\n";
						// log << "TOK_CALL argument_symbol type: " << argument_symbol->type << "\n";
						// log << "TOK_CALL argument_symbol flags: " << argument_symbol->flags << "\n";
						// log << "TOK_CALL argument_symbol array_size: " << argument_symbol->ele << "\n";

						if (check_symbol_for_constant_string(argument_symbol))
						{
							std::string variable_name = (const char*)argument_symbol->name;
							std::string variable_text = (const char*)*argument_symbol->stringdata;
							std::string source = create_source(section_name, function_name, function_count, variable_name);

							// log << "TOK_CALL ADD TO TRANSLATE 100: " << variable_text << "\n";
							script_string_data_add(variable_text, source, "100",
								std::to_string(argument_symbol_index), std::to_string(argument_array_index));
							// used_symbol_string_index.push_back(from_symbol_index);
						}
					}

					for (auto pushed_string_variable_stack_index_vector : pushed_string_variable_stack_index_vector_vector[1])
					{
						int64_t from_symbol_index = pushed_string_index_variable_stack[pushed_string_variable_stack_index_vector];
						uint64_t array_index = pushed_string_array_index_variable_stack[pushed_string_variable_stack_index_vector];
						zCPar_Symbol* from_symbol = loc_parser->symtab.table[from_symbol_index];

						std::string variable_name = (const char*)from_symbol->name;
						std::string variable_text = (const char*)*from_symbol->stringdata;
						std::string source = create_source(section_name, function_name, function_count, variable_name);

						// log << "TOK_CALL ADD TO TRANSLATE 0: " << variable_text << "\n";
						script_string_data_add(variable_text, source, "0",
							std::to_string(from_symbol_index), std::to_string(array_index));
						// used_symbol_string_index.push_back(from_symbol_index);
					}
				}
				else
				{
					// log << "return {}" << "\n";
				}

				latest_call_address = i;
				i += 4;
			}
			else if (token == zPAR_TOK_PUSHINT
				|| token == zPAR_TOK_PUSHINST
				|| token == zPAR_TOK_JUMP
				|| token == zPAR_TOK_JUMPF
				|| token == zPAR_TOK_SETINSTANCE)
			{
				i += 4;
			}
			else if (token == zPAR_TOK_PUSHVAR)
			{
				// VARIABLE !!!

				int64_t symbol_index = get_token_next_bytes(bytecode, i);
				std::string symbol_name = (const char*)loc_parser->symtab.table[symbol_index]->name;

				if (loc_parser->symtab.table[symbol_index]->type != zPAR_TYPE_STRING)
				{
					i += 4;
					continue;
				}

				if (pushed_string_index_variable_stack.size() >= 100)
					pushed_string_index_variable_stack.pop_front();
				pushed_string_index_variable_stack.push_back(symbol_index);

				if (pushed_string_array_index_variable_stack.size() >= 100)
					pushed_string_array_index_variable_stack.pop_front();
				pushed_string_array_index_variable_stack.push_back(0);

				if (pushed_string_address_array_variable_stack.size() >= 100)
					pushed_string_address_array_variable_stack.pop_front();
				pushed_string_address_array_variable_stack.push_back(i);

				i += 4;
			}
			else if (token == zPAR_TOK_ASSIGNSTR) // 0x46, MOVS
			{
				continue;

				int64_t from_symbol_index = pushed_string_index_variable_stack[pushed_string_index_variable_stack.size() - 2];
				uint64_t from_array_index = pushed_string_array_index_variable_stack[pushed_string_array_index_variable_stack.size() - 2];
				zCPar_Symbol* from_symbol = loc_parser->symtab.table[from_symbol_index];

				if (!check_symbol_for_constant_string(from_symbol))
				{
					continue;
				}

				int64_t to_symbol_index = pushed_string_index_variable_stack[pushed_string_index_variable_stack.size() - 1];
				uint64_t to_array_index = pushed_string_array_index_variable_stack[pushed_string_array_index_variable_stack.size() - 1];
				zCPar_Symbol* to_symbol = loc_parser->symtab.table[to_symbol_index];

				std::string symbol_name = (const char*)to_symbol->name;
				std::string variable_name = (const char*)from_symbol->name;
				std::string variable_text = (const char*)*from_symbol->stringdata;
				std::string source = create_source(section_name, symbol_name, -1, variable_name, to_array_index, -1);

				if (assign_symbol_check(to_symbol))
				{
					if (is_menu)
					{
						if (std::count(menu_string_data_insert_order.begin(), menu_string_data_insert_order.end(), variable_text) <= 0)
						{
							remove_broken_symbols(source);
							menu_string_data_insert_order.push_back(variable_text);
							menu_string_data[variable_text]["translate_source"] = source; // if same name, other source?
							menu_string_data[variable_text]["translated_string"] = "";
						}
					}
					else
					{
						script_string_data_add(variable_text, source, "100",
							std::to_string(from_symbol_index), std::to_string(from_array_index));
						// used_symbol_string_index.push_back(from_symbol_index);
					}
				}
				else
				{
					script_string_data_add(variable_text, source, "0",
						std::to_string(from_symbol_index), std::to_string(from_array_index));
					// used_symbol_string_index.push_back(from_symbol_index);
				}
			}
			else
			{
				// other token size is 1
			}
		}
	}

	void check_add_stack_variable(zCParser* loc_parser, StackVariable & stack_variable, SourceData & source_data, bool is_menu = false)
	{
		if (stack_variable.value < 0 || stack_variable.value >= loc_parser->symtab.table.GetNum())
			return;

		zCPar_Symbol* argument_symbol = loc_parser->symtab.table[stack_variable.value];

		if (argument_symbol->type != zPAR_TYPE_STRING || !argument_symbol->HasFlag(zPAR_FLAG_CONST))
			return;

		std::string variable_symbol_name = (const char*)argument_symbol->name;
		std::string variable_symbol_text = (const char*)argument_symbol->stringdata[stack_variable.array_index];

		// @TODO: добавить что если это возов функции где стройка аргумент, и функции нет в перечне, то добалять источник к этой строке-аргумету

		std::string function_trace_tree_string;
		// for (auto & function_trace_tree : function_trace_tree_vector)
		// {
		// 	if (!function_trace_tree_string.empty())
		// 	{
		// 		function_trace_tree_string += " -> " + function_trace_tree;
		// 	}
		//
		// 	auto function_trace_tree_splited_vector = split_string(function_trace_tree);
		// 	if (function_trace_tree_splited_vector[function_trace_tree_splited_vector.size() - 1]
		// 		== stack_variable.appear_section_name)
		// 	{
		// 		function_trace_tree_string = function_trace_tree;
		// 	}
		// }

		// std::string source = create_source(source_data.argument_name,
		// 	source_data.function_name, source_data.function_count, argument_symbol_name,
		// 	stack_variable.array_index, source_data.argument_number);

		std::string source = create_source_v2(variable_symbol_name, source_data);

		// log << "[TRANSLATE] argument_symbol_name: \"" << variable_symbol_name << "\"" << std::endl;
		// log << "[TRANSLATE] argument_symbol_text: \"" << variable_symbol_text << "\"" << std::endl;
		// log << "[TRANSLATE] source: " << source << std::endl;
		// log << std::endl;

		script_string_data_add(variable_symbol_text, source, "100",
			std::to_string(stack_variable.value), std::to_string(stack_variable.array_index), is_menu);
	}

	void function_with_edit_string_add_section(zCParser* loc_parser, FunctionWithTranslatedArgument & section_fwta,
		std::vector<StackVariableTrace> & edit_string_stack_variable_trace_vector,
		StackVariableTrace & section_return)
	{
		// need a copy!!!
		auto return_stack_variable_vector = section_return.extra_stack_variable_vector;
		return_stack_variable_vector.push_back(section_return.stack_variable);

		bool is_string_in_return = false;
		for (auto & return_stack_variable : return_stack_variable_vector)
		{
			if (return_stack_variable.value >= 0 && return_stack_variable.value < loc_parser->symtab.table.GetNum())
			{
				auto symbol = loc_parser->symtab.table[return_stack_variable.value];

				if (symbol->type == zPAR_TYPE_STRING)
				{
					is_string_in_return = true;
					break;
				}
			}
		}

		if (!is_string_in_return)
			return;

		// log << "[LOG] function_with_edit_string_add_section: " << section_fwta.to_string() << std::endl;
		// log << "[LOG] section_return: " << section_return.to_string() << std::endl;

		auto function_with_edit_wtring_argument = FunctionWithEditStringArgument();
		for (auto & translated_argument : section_fwta.translated_argument_vector)
		{
			auto edit_string_argument = EditStringArgument();
			edit_string_argument.name = translated_argument.name;
			edit_string_argument.is_edit = false;
			edit_string_argument.number = translated_argument.number;

			function_with_edit_wtring_argument.edit_string_argument_vector.push_back(edit_string_argument);
		}

		bool is_edit_argument_found = false;
		bool is_inner_found = false;
		for (auto & edit_string_stack_variable_trace : edit_string_stack_variable_trace_vector)
		{
			// need a copy!!!
			auto edit_stack_variable_vector = edit_string_stack_variable_trace.extra_stack_variable_vector;
			edit_stack_variable_vector.push_back(section_return.stack_variable);

			for (auto & edit_stack_variable : edit_stack_variable_vector)
			{
				for (auto & return_stack_variable : return_stack_variable_vector)
				{
					// log << "[LOG] return_stack_variable.name == edit_stack_variable.name: " << return_stack_variable.name << std::endl;

					if (return_stack_variable.name == edit_stack_variable.name)
					{
						bool is_edit_in_argument = false;
						for (auto & translated_argument : section_fwta.translated_argument_vector)
						{
							// log << "[LOG] edit_stack_variable.name == translated_argument.name: " << translated_argument.name << std::endl;

							if (edit_stack_variable.name == translated_argument.name)
							{
								// log << "[LOG] edit_stack_variable.name == return_stack_variable.name: " << edit_stack_variable.name << std::endl;

								function_with_edit_wtring_argument.edit_string_argument_vector[translated_argument.number - 1].is_edit = true;
								is_edit_argument_found = true;
								is_edit_in_argument = true;
							}
						}

						if (!is_edit_in_argument && edit_stack_variable.value >= 0)
						{
							auto stack_variable_trace  = StackVariableTrace();
							stack_variable_trace.stack_variable = edit_stack_variable;

							function_with_edit_wtring_argument.inner_stack_variable_vector.push_back(stack_variable_trace);
							is_inner_found = true;
						}
					}
				}
			}
		}

		if (is_edit_argument_found || is_inner_found)
		{
			function_with_edit_wtring_argument.name = section_fwta.name;
			function_with_edit_wtring_argument.argument_count = function_with_edit_wtring_argument.edit_string_argument_vector.size();

			// log << "[TRACE] is_edit_found: " << function_with_wdit_wtring_argument.name << std::endl;
			// log << "[TRACE] is_edit_found: " << function_with_wdit_wtring_argument.argument_count << std::endl;

			function_with_edit_string_argument_map[function_with_edit_wtring_argument.name] = function_with_edit_wtring_argument;
		}
	}

	void function_with_translated_argument_add_section_v2(zCParser* loc_parser,
		FunctionWithTranslatedArgument & section_fwta,
		SourceData & source_data,
		std::vector<StackVariableTrace> & prepared_argument_vector)
	{
		// 1 вариант сразу вне очереди трасировать функцию, ищем еще варианты...
		// 2 вариант коллектить всю инфу, а потом по ним проходится...
		// 3 проходить 2 раза...

		// можно сдлеать 1 вариант но делать пометку что есть АИ_

		// thx archolos...
		bool is_ai_function = source_data.function_name.substr(0, 11) == "AI_FUNCTION";
		if (is_ai_function && prepared_argument_vector.size() > 2 && !script_index_stack_data_map.empty())
		{
			// log << "[TRACE] function_name: " << source_data.function_name << ", args: " << prepared_argument_vector.size() << std::endl;
			auto & stack_data = script_index_stack_data_map.begin()->second;
			bool found_function = false;

			auto stack_variable_vector = prepared_argument_vector[1].extra_stack_variable_vector;
			stack_variable_vector.push_back(prepared_argument_vector[1].stack_variable);
			for (auto & stack_variable : stack_variable_vector)
			{
				// log << "[TRACE] stack_variable: " << stack_variable.to_string() << std::endl;

				if (script_index_stack_data_map.count(stack_variable.value))
				{
					stack_data = script_index_stack_data_map[stack_variable.value];
					// log << "[TRACE] stack_data.name: " << stack_data.name << std::endl;

					found_function = true;
					break;
				}
			}

			if (found_function)
			{
				// fix name
				source_data.function_name = stack_data.name;

				// delete 2 first element of vector (var C_NPC slf, var func function)
				prepared_argument_vector.erase(prepared_argument_vector.begin());
				prepared_argument_vector.erase(prepared_argument_vector.begin());
			}

			// log << std::endl;
		}

		// log << "[LOG] function_with_translated_argument_add_section_v2: " << source_data.function_name << std::endl;

		if (function_with_translated_argument_map.count(source_data.function_name))
		{
			// log << "[LOG] function_with_translated_argument_map.count(source_data.function_name): " << source_data.function_name << std::endl;

			auto & called_fwta = function_with_translated_argument_map[source_data.function_name];

			if (prepared_argument_vector.size() == called_fwta.argument_count)
			{
				// log << "[LOG] prepared_argument_vector.size() == called_fwta.argument_count: " << called_fwta.argument_count << std::endl;
				// i = argument index
				for (uint64_t i = 0; i < prepared_argument_vector.size(); ++i)
				{
					// log << "[LOG] arg# " << i << ", is_translated: " << called_fwta.translated_argument_vector[i].is_translated << std::endl;

					if (called_fwta.translated_argument_vector[i].is_translated == false)
						continue;

					// need a copy!!!
					auto stack_variable_vector = prepared_argument_vector[i].extra_stack_variable_vector;
					stack_variable_vector.push_back(prepared_argument_vector[i].stack_variable);

					for (auto & stack_variable : stack_variable_vector)
					{
						for (auto & section_translated_argument : section_fwta.translated_argument_vector)
						{
							if (stack_variable.name == section_translated_argument.name)
							{
								// log << "[LOG] stack_variable.name == section_translated_argument.name: " << stack_variable.name << std::endl;
								// log << "[LOG] section_fwta.name: " << section_fwta.name << ", set section_translated_argument.is_translated = true " << section_translated_argument.name << std::endl;

								section_translated_argument.is_translated = true;
								section_translated_argument.used_in.insert(called_fwta.name);
							}
						}

						source_data.section_name = section_fwta.name;
						source_data.argument_name = called_fwta.translated_argument_vector[i].name;
						source_data.argument_number = i + 1;
						check_add_stack_variable(loc_parser, stack_variable, source_data);
					}
				}
			}
			else
			{
				// diff arg count, big problem...
			}
		}
		else
		{
			auto called_fwta = FunctionWithTranslatedArgument();
			called_fwta.argument_count = prepared_argument_vector.size();
			called_fwta.name = source_data.function_name;

			for (uint64_t i = 0; i < prepared_argument_vector.size(); ++i)
			{
				auto ta = TranslatedArgument("ARG " + std::to_string(i + 1), i + 1, false);
				called_fwta.translated_argument_vector.push_back(ta);
			}

			function_with_translated_argument_map[called_fwta.name] = called_fwta;
		}
	}

    void trace_stack(byte* bytecode, uint64_t address_begin, uint64_t size, zCParser* loc_parser, bool is_menu = false)
	{
		// stack_log.close();
		// std::remove("stack_log.txt");
		// stack_log.open("stack_log.txt", std::ios::app);
		// while (!stack_log.is_open())
		// 	std::this_thread::sleep_for(std::chrono::milliseconds(10));

		uint64_t old_i = address_begin;

		int64_t token_value = 0;
		zCPar_Symbol* token_symbol = nullptr;
		std::unordered_map<std::string, std::unordered_map<std::string, int64_t>> function_counter;
		std::string last_function_name;

		auto & section_stack_data = is_menu ? menu_address_stack_data_map[address_begin] : script_address_stack_data_map[address_begin];

		// auto section_name = virtual_address_name_map[address_begin];
		// auto section_symbol_index = virtual_address_index_map[address_begin];
		auto section_symbol = loc_parser->symtab.table[section_stack_data.index];

		bool is_edit_string_func_found_in_section = false;
		bool is_goto_found = false;
		bool is_last_return = false;

		// auto address_end = get_stack_section_address(virtual_address_name_map, address_begin, true);
		// auto section_size = address_end - address_begin;
		// stack_log << "[TRACE] [" << section_stack_data.name << "] ADDRESS: " << address_begin
		// << ", SIZE: " << section_stack_data.size << std::endl;
		// stack_log.flush();

		uint64_t section_argument_count = section_symbol->ele;
		uint64_t section_return_count = section_symbol->HasFlag(zPAR_FLAG_RETURN);
		uint64_t section_initial_stack_size = pushed_stack_variable_trace_vector.size();
		uint64_t section_disired_stack_size = (section_initial_stack_size - section_argument_count) + section_return_count;

		// stack_log << "[TRACE] [" << section_stack_data.name << "] TYPE: " << section_symbol->type
		// << ", ARGUMENT COUNT: " << section_symbol->ele
		// << ", RETURN: " << section_symbol->HasFlag(zPAR_FLAG_RETURN) << std::endl;
		// stack_log.flush();

		bool is_section_instance =  section_symbol->type == zPAR_TYPE_INSTANCE;
		bool is_section_prototype =  section_symbol->type == zPAR_TYPE_PROTOTYPE;
		bool is_section_function =  section_symbol->type == zPAR_TYPE_FUNC;

		// std::stringstream bytes_string_stream;
		// bytes_string_stream << std::hex << std::uppercase << std::setfill('0');
		// for (uint64_t i = 0; i < size; ++i)
		// {
		// 	bytes_string_stream << std::setw(2) << static_cast<uint64_t>(bytecode[address_begin + i + 0]);
		// 	if (i != size - 1)
		// 	{
		// 		bytes_string_stream << " ";
		// 	}
		// }
		//
		// stack_log << bytes_string_stream.str() << std::endl;

		// FFFUCK!!!
		// https://auronen.cokoliv.eu/gmc/zengin/scripts/extenders/lego/tools/string_builder/
		std::unordered_map<std::string, std::vector<StackVariableTrace>> string_builder_argument_map = {};
		std::string used_sb = "DEFAULT_SB";

		std::vector<std::string> section_argument_name_vector = {};
		std::vector<std::string> prev_section_argument_name_vector = {};
		std::vector<StackVariableTrace> edit_string_stack_variable_trace_vector = {};

		for (uint64_t i = section_argument_count; i > 0; --i)
		{
			uint64_t argument_index = pushed_stack_variable_trace_vector.size() - i;
			auto & stack_variable = pushed_stack_variable_trace_vector[argument_index].stack_variable;
			// stack_log << "[TRACE] ARGUMENT " << std::to_string(i) << " DATA: " << stack_variable.to_string() << std::endl; // why -i
			prev_section_argument_name_vector.push_back(stack_variable.name);
		}

		// TranslatedArgument added after all local variables added
		auto section_fwta = FunctionWithTranslatedArgument();
		section_fwta.argument_count = section_argument_count;
		section_fwta.name = section_stack_data.name;

		for (uint64_t i = address_begin; i <= address_begin + size; ++i)
		{
			if (old_i != i)
			{
				std::string comment;
				if (token_symbol)
					comment = token_symbol->name;
				auto stack_data_string = bytes_to_stack_data(bytecode, old_i, i - old_i, comment);

				// stack_log << stack_data_string;
				// stack_log.flush();
			}

			if (is_last_return || i >= address_begin + size)
				break;

			// stack_log << "[TRACE] [" << section_stack_data.name << "] stack size: " << pushed_stack_variable_trace_vector.size() << std::endl;
			// stack_log.flush();

			old_i = i;

			int64_t token = bytecode[i];

			if (token == zPAR_TOK_PUSHVAR) // 0x41
			{
				token_value = get_token_next_bytes(bytecode, i);
				token_symbol = loc_parser->symtab.table[token_value];

				// ...

				auto stack_variable = StackVariable();
				stack_variable.name = (const char *)token_symbol->name;
				stack_variable.appear_section_name = section_stack_data.name;
				stack_variable.value = token_value;

				auto stack_variable_trace  = StackVariableTrace();
				stack_variable_trace.stack_variable = stack_variable;

				pushed_stack_variable_trace_vector.push_back(stack_variable_trace);

				// ...

				if (section_argument_name_vector.size() < section_argument_count)
				{
					section_argument_name_vector.insert(section_argument_name_vector.begin(), stack_variable.name);

					uint64_t argument_number = (section_argument_count - section_argument_name_vector.size()) + 1;

					auto ta = TranslatedArgument(stack_variable.name, argument_number, false);
					section_fwta.translated_argument_vector.insert(section_fwta.translated_argument_vector.begin(), ta);
				}

				// stack_log << "[TRACE] zPAR_TOK_PUSHVAR OK: " << stack_variable_trace.to_string() << std::endl;
				// stack_log.flush();

				// ...

				i += 4;
			}
			else if (token == zPAR_TOK_PUSHVAR + zPAR_TOK_FLAGARRAY) // 0xF5
			{
				token_value = get_token_next_bytes(bytecode, i);
				token_symbol = loc_parser->symtab.table[token_value];
				int64_t array_index = bytecode[i + 5];

				auto stack_variable = StackVariable();
				stack_variable.name = (const char *)token_symbol->name;
				stack_variable.appear_section_name = section_stack_data.name;
				stack_variable.value = token_value;
				stack_variable.array_index = array_index;
				stack_variable.is_array = true;

				auto stack_variable_trace  = StackVariableTrace();
				stack_variable_trace.stack_variable = stack_variable;

				pushed_stack_variable_trace_vector.push_back(stack_variable_trace);

				// ...

				if (section_argument_name_vector.size() < section_argument_count)
				{
					section_argument_name_vector.insert(section_argument_name_vector.begin(), stack_variable.name);

					uint64_t argument_number = (section_argument_count - section_argument_name_vector.size()) + 1;

					auto ta = TranslatedArgument(stack_variable.name, argument_number, false);
					section_fwta.translated_argument_vector.insert(section_fwta.translated_argument_vector.begin(), ta);
				}

				// ...

				i += 5;
			}
			else if (token == zPAR_TOK_PUSHINST)
			{
				token_value = get_token_next_bytes(bytecode, i);
				token_symbol = loc_parser->symtab.table[token_value];

				auto stack_variable = StackVariable();
				stack_variable.name = (const char *)token_symbol->name;
				stack_variable.appear_section_name = section_stack_data.name;
				stack_variable.value = token_value;

				auto stack_variable_trace  = StackVariableTrace();
				stack_variable_trace.stack_variable = stack_variable;

				pushed_stack_variable_trace_vector.push_back(stack_variable_trace);

				// ...

				if (section_argument_name_vector.size() < section_argument_count)
				{
					section_argument_name_vector.insert(section_argument_name_vector.begin(), stack_variable.name);

					uint64_t argument_number = (section_argument_count - section_argument_name_vector.size()) + 1;

					auto ta = TranslatedArgument(stack_variable.name, argument_number, false);
					section_fwta.translated_argument_vector.insert(section_fwta.translated_argument_vector.begin(), ta);
				}

				// ...

				i += 4;
			}
			else if (token == zPAR_TOK_PUSHINT) // 0x40
			{
				token_value = get_token_next_bytes(bytecode, i);
				token_symbol = nullptr;

				auto stack_variable = StackVariable();
				stack_variable.name = "INTEGER";
				stack_variable.appear_section_name = section_stack_data.name;
				stack_variable.value = token_value;

				auto stack_variable_trace  = StackVariableTrace();
				stack_variable_trace.stack_variable = stack_variable;

				pushed_stack_variable_trace_vector.push_back(stack_variable_trace);

				// ...

				if (section_argument_name_vector.size() < section_argument_count)
				{
					section_argument_name_vector.insert(section_argument_name_vector.begin(), stack_variable.name);

					uint64_t argument_number = (section_argument_count - section_argument_name_vector.size()) + 1;

					auto ta = TranslatedArgument(stack_variable.name, argument_number, false);
					section_fwta.translated_argument_vector.insert(section_fwta.translated_argument_vector.begin(), ta);
				}

				// ...

				i += 4;
			}
			else if (token == zPAR_TOK_CALL) // 0x3D , in script code
			{
				// stack_log << "[TRACE] zPAR_TOK_CALL 1: " << std::endl;
				// stack_log.flush();

				auto token_address = get_token_next_bytes(bytecode, i);
				auto & token_stack_data = is_menu ? menu_address_stack_data_map[token_address] : script_address_stack_data_map[token_address];

				// stack_log << "[TRACE] zPAR_TOK_CALL 2, token_address: " << token_address << std::endl;
				// stack_log << "[TRACE] zPAR_TOK_CALL 2, token_stack_data.index: " << token_stack_data.index << std::endl;
				// stack_log << "[TRACE] zPAR_TOK_CALL 2, token_stack_data.address_begin: " << token_stack_data.address_begin << std::endl;
				// stack_log << "[TRACE] zPAR_TOK_CALL 2, token_stack_data.address_end: " << token_stack_data.address_end << std::endl;
				// stack_log << "[TRACE] zPAR_TOK_CALL 2, token_stack_data.name: " << token_stack_data.name << std::endl;
				// stack_log << "[TRACE] zPAR_TOK_CALL 2, token_stack_data.size: " << token_stack_data.size << std::endl;
				// stack_log.flush();

				token_symbol = loc_parser->symtab.table[token_stack_data.index];
				std::string function_name = (const char*)token_symbol->name;
				uint64_t argument_count = token_symbol->ele;
				last_function_name = function_name;

				// Если это сразу AI_FUNCTION, или код в котором сипользуется AI_FUNCTION - то и этот помечаем для повторного прохода.
				bool is_ai_function = function_name.substr(0, 11) == "AI_FUNCTION";
				if (is_ai_function && argument_count > 2)
				{
					std::string suffix = function_name.substr(function_name.length() - 4);
					bool is_ai_function_use_string = suffix.find('S') != std::string::npos;
					if (is_ai_function_use_string)
					{
						section_stack_data.is_use_ai_function = true;
						// log << "[LOG] section_stack_data.is_use_ai_function = true: " << section_stack_data.name << std::endl;
					}
				}
				if (token_stack_data.is_use_ai_function)
				{
					section_stack_data.is_use_ai_function = true;
					// log << "[LOG] section_stack_data.is_use_ai_function = true: " << section_stack_data.name << std::endl;
				}


				// String Builder Section
				if (lego_found)
				{
					if (function_name == "SB_NEW")
					{
						// check raw SB_NEW without var set ( s = SB_NEW() -> SB_NEW() ) ???
					}
					else if (function_name == "SB_USE")
					{
						used_sb = pushed_stack_variable_trace_vector.back().stack_variable.name; // mb ok
						// log << "[LOG] CALL SB_USE: " << used_sb << std::endl;
					}
					else if (function_name == "SB")
					{
						auto & string_argument = pushed_stack_variable_trace_vector.back();
						string_builder_argument_map[used_sb].push_back(string_argument);
						// log << "[LOG] CALL SB, used_sb : " << used_sb << std::endl;
						// log << "[LOG] CALL SB, string_argument: " << string_argument.to_string() << std::endl;
						// log << std::endl;
					}
				}


				// log << "[LOG] CALL function_name: " << function_name << std::endl;

				// stack_log << "[TRACE] zPAR_TOK_CALL 3, function_name: " << function_name << ", argument_count: " << argument_count << std::endl;

				function_counter[section_stack_data.name][function_name] += 1;
				uint64_t function_count = function_counter[section_stack_data.name][function_name];

				bool is_return_value = token_symbol->HasFlag(zPAR_FLAG_RETURN);
				if (function_with_internal_push_stack_set.count(function_name))
					is_return_value = true;

				std::vector<StackVariableTrace> prepared_argument_vector;
				int64_t k = 0;
				for (uint64_t j = argument_count; j > 0; --j)
				{
					uint64_t stack_variable_trace_argment_index = pushed_stack_variable_trace_vector.size() - j;
					auto & stack_variable_trace_argment = pushed_stack_variable_trace_vector[stack_variable_trace_argment_index];
					// stack_log << "[TRACE] arg# " << k + 1 << " " << stack_variable_trace_argment.to_string() << std::endl;
					if (moved_variable_value_trace_map.count(stack_variable_trace_argment.stack_variable.value))
					{
						stack_variable_trace_argment = moved_variable_value_trace_map[stack_variable_trace_argment.stack_variable.value];
						// stack_log << "[TRACE] found move, new arg#" << k + 1 << " " << stack_variable_trace_argment.to_string() << std::endl;
					}

					prepared_argument_vector.push_back(stack_variable_trace_argment);

					k++;
				}

				// ...

				// don't call recursion and ikarus & lego code
				// bool do_trace = address_begin != token_address && token_address > address_skip_min_value;

				// if (do_trace)
				function_trace_tree_vector.push_back("#" + std::to_string(function_count) + " " + function_name);

				auto source_data = SourceData();
				source_data.function_name = function_name;
				source_data.function_count = function_count;



				function_with_translated_argument_add_section_v2(loc_parser, section_fwta, source_data, prepared_argument_vector);




				auto function_with_edit_string_argument = FunctionWithEditStringArgument();
				if (function_with_edit_string_argument_map.count(function_name)
					&& argument_count == function_with_edit_string_argument_map[function_name].argument_count)
				{
					function_with_edit_string_argument = function_with_edit_string_argument_map[function_name];
					is_edit_string_func_found_in_section = true;

					// log << "[TRACE] found function_with_edit_string_argument: " << section_fwta.name << std::endl;
					// log << "[TRACE] found function_with_edit_string_argument: " << function_with_edit_string_argument.to_string() << std::endl;
				}
				// else
				// {
				// 	function_with_edit_string_argument = FunctionWithEditStringArgument();
				// 	is_edit_string_func_found_in_section = false;
				// }

				// --------------------------------------------------------------------------------------------------- func string BUILDRECIPEDIALOGDESCRIPTION(var int RECIPEINST) {

				// // manage !!!
				// if (function_with_translated_argument_map.count(function_name))
				// {
				// 	do_trace = false;
				// 	auto & function_with_translated_argument = function_with_translated_argument_map[function_name];
				// 	function_with_translated_argument_add_section(loc_parser, section_stack_data,
				// 		section_argument_name_vector, section_fwta,
				// 		prepared_argument_vector);
				// }
				// else
				// {
				// 	trace_stack(bytecode, token_address, token_stack_data.size, loc_parser);
				// }

				// if (do_trace)
				// {
				for (uint64_t j = 0; j < token_symbol->ele; ++j)
				{
					pushed_stack_variable_trace_vector.pop_back();
				}

				if (is_return_value)
				{
					auto return_stack_variable = StackVariable();
					return_stack_variable.name = "RETURN VALUE FROM " + function_name;
					return_stack_variable.appear_section_name = section_stack_data.name;

					auto return_stack_variable_trace  = StackVariableTrace();
					return_stack_variable_trace.stack_variable = return_stack_variable;

					// Edit String (CS...)
					for (uint64_t j = 0; j < function_with_edit_string_argument.edit_string_argument_vector.size(); ++j)
					{
						if (!function_with_edit_string_argument.edit_string_argument_vector[j].is_edit)
							continue;

						return_stack_variable_trace.extra_stack_variable_vector.push_back(prepared_argument_vector[j].stack_variable);
						return_stack_variable_trace.extra_stack_variable_vector.insert(
							return_stack_variable_trace.extra_stack_variable_vector.end(),
							prepared_argument_vector[j].extra_stack_variable_vector.begin(),
							prepared_argument_vector[j].extra_stack_variable_vector.end());
					}

					// log << "[LOG] BEFORE, function_with_edit_string_argument.to_string(): " << return_stack_variable_trace.to_string() << std::endl;
					//

					//
					// log << "[LOG] AFTER, function_with_edit_string_argument.to_string(): " << return_stack_variable_trace.to_string() << std::endl;
					// log << std::endl;

					if (is_edit_string_func_found_in_section)
					{
						edit_string_stack_variable_trace_vector.push_back(return_stack_variable_trace);
					}

					// String Builder Section
					if (lego_found && function_name == "SB_TOSTRING")
					{
						for (auto & string_builder_argument : string_builder_argument_map[used_sb])
						{
							return_stack_variable_trace.extra_stack_variable_vector.push_back(string_builder_argument.stack_variable);
							return_stack_variable_trace.extra_stack_variable_vector.insert(
								return_stack_variable_trace.extra_stack_variable_vector.end(),
								string_builder_argument.extra_stack_variable_vector.begin(),
								string_builder_argument.extra_stack_variable_vector.end());
						}

						// Edit String (like CS but with SB)
						is_edit_string_func_found_in_section = true;
						edit_string_stack_variable_trace_vector.push_back(return_stack_variable_trace);
					}

					// if (function_name == "BUILDRECIPEDIALOGDESCRIPTION")
					// {
					// 	log << "[LOG] BUILDRECIPEDIALOGDESCRIPTION RETURN, function_with_edit_string_argument.to_string(): " << function_with_edit_string_argument.to_string() << std::endl;
					// 	log << "[LOG] BUILDRECIPEDIALOGDESCRIPTION RETURN, return_stack_variable_trace.to_string(): " << return_stack_variable_trace.to_string() << std::endl;
					// 	log << std::endl;
					// }

					if (is_edit_string_func_found_in_section)
					{
						// Edit String (CS...)
						for (auto & inner_stack_variable_trace : function_with_edit_string_argument.inner_stack_variable_vector)
						{
							return_stack_variable_trace.extra_stack_variable_vector.push_back(inner_stack_variable_trace.stack_variable);

							return_stack_variable_trace.extra_stack_variable_vector.insert(
								return_stack_variable_trace.extra_stack_variable_vector.end(),
								inner_stack_variable_trace.extra_stack_variable_vector.begin(),
								inner_stack_variable_trace.extra_stack_variable_vector.end());
						}
					}

					pushed_stack_variable_trace_vector.push_back(return_stack_variable_trace);
				}

				// stack_log << "[TRACE] DONT TRACE RECURSION and ikarus & lego code" << std::endl;
				// }

				i += 4;
			}
			else if (token == zPAR_TOK_CALLEXTERN) // 0x3E , in game code (not script)
			{
				token_value = get_token_next_bytes(bytecode, i);
				token_symbol = loc_parser->symtab.table[token_value];
				std::string function_name = (const char *)token_symbol->name;
				uint64_t argument_count = token_symbol->ele;
				last_function_name = function_name;

				function_counter[section_stack_data.name][function_name] += 1;
				int64_t function_count = function_counter[section_stack_data.name][function_name];

				std::vector<StackVariableTrace> prepared_argument_vector;
				int64_t k = 0;
				for (uint64_t j = argument_count; j > 0; --j)
				{
					uint64_t stack_variable_trace_argment_index = pushed_stack_variable_trace_vector.size() - j;
					auto & stack_variable_trace_argment = pushed_stack_variable_trace_vector[stack_variable_trace_argment_index];
					// stack_log << "[TRACE] arg#" << k + 1 << " " << stack_variable_trace_argment.to_string() << std::endl;
					if (moved_variable_value_trace_map.count(stack_variable_trace_argment.stack_variable.value))
					{
						stack_variable_trace_argment = moved_variable_value_trace_map[stack_variable_trace_argment.stack_variable.value];
						// stack_log << "[TRACE] found move, new arg#" << k + 1 << " " << stack_variable_trace_argment.to_string() << std::endl;
					}

					prepared_argument_vector.push_back(stack_variable_trace_argment);

					k++;
				}

				// // manage !!!
				// if (function_with_translated_argument_map.count(function_name))
				// {
				// 	// auto & function_with_translated_argument = function_with_translated_argument_map[function_name];
				// 	// function_with_translated_argument_add_section(loc_parser, section_stack_data,
				// 	// 	section_argument_name_vector, function_with_translated_argument,
				// 	// 	prepared_argument_vector);
				//
				// 	void
				// }

				auto source_data = SourceData();
				source_data.function_name = function_name;
				source_data.function_count = function_count;
				function_with_translated_argument_add_section_v2(loc_parser, section_fwta, source_data, prepared_argument_vector);

				auto function_with_edit_string_argument = FunctionWithEditStringArgument();
				if (function_with_edit_string_argument_map.count(function_name)
					&& argument_count == function_with_edit_string_argument_map[function_name].argument_count)
				{
					function_with_edit_string_argument = function_with_edit_string_argument_map[function_name];
					is_edit_string_func_found_in_section = true;
				}

				// // получаем {"CONCATSTRINGS", {true, true}}
				// std::vector<bool> trace_argument = {};
				// if (edit_string_functions.count(function_name) && argument_count == edit_string_functions[function_name].size())
				// {
				// 	trace_argument = edit_string_functions[function_name];
				// }
				// else
				// {
				// 	for (uint64_t j = 0; j < argument_count; ++j)
				// 	{
				// 		trace_argument.push_back(false);
				// 	}
				// }

				auto return_stack_variable = StackVariable();
				return_stack_variable.name = "RETURN VALUE FROM " + function_name;
				return_stack_variable.appear_section_name = section_stack_data.name;

				auto return_stack_variable_trace  = StackVariableTrace();
				return_stack_variable_trace.stack_variable = return_stack_variable;

				for (uint64_t j = 0; j < function_with_edit_string_argument.edit_string_argument_vector.size(); ++j)
				{
					if (!function_with_edit_string_argument.edit_string_argument_vector[j].is_edit)
						continue;

					return_stack_variable_trace.extra_stack_variable_vector.push_back(prepared_argument_vector[j].stack_variable);
					return_stack_variable_trace.extra_stack_variable_vector.insert(
						return_stack_variable_trace.extra_stack_variable_vector.end(),
						prepared_argument_vector[j].extra_stack_variable_vector.begin(),
						prepared_argument_vector[j].extra_stack_variable_vector.end());
				}

				if (is_edit_string_func_found_in_section)
				{
					edit_string_stack_variable_trace_vector.push_back(return_stack_variable_trace);
				}

				// for (uint64_t j = 0; j < prepared_argument_vector.size(); ++j)
				// {
				// 	if (trace_argument[j])
				// 	{
				// 		// if (prepared_argument_vector[j].extra_stack_variable_vector.size() > 0)
				// 		// {
				// 		// 	return_stack_variable_trace.extra_stack_variable_vector.insert(
				// 		// 		return_stack_variable_trace.extra_stack_variable_vector.end(),
				// 		// 		prepared_argument_vector[j].extra_stack_variable_vector.begin(),
				// 		// 		prepared_argument_vector[j].extra_stack_variable_vector.end());
				// 		// }
				// 		// else
				// 		// {
				// 		// 	return_stack_variable_trace.extra_stack_variable_vector.push_back(prepared_argument_vector[j].stack_variable);
				// 		// }
				// 		return_stack_variable_trace.extra_stack_variable_vector.push_back(prepared_argument_vector[j].stack_variable);
				// 		return_stack_variable_trace.extra_stack_variable_vector.insert(
				// 			return_stack_variable_trace.extra_stack_variable_vector.end(),
				// 			prepared_argument_vector[j].extra_stack_variable_vector.begin(),
				// 			prepared_argument_vector[j].extra_stack_variable_vector.end());
				// 	}
				// }

				for (uint64_t j = 0; j < argument_count; ++j)
				{
					pushed_stack_variable_trace_vector.pop_back();
				}

				bool is_return_value = token_symbol->HasFlag(zPAR_FLAG_RETURN);
				if (is_return_value)
				{
					pushed_stack_variable_trace_vector.push_back(return_stack_variable_trace);
					// stack_log << "[TRACE] return value (last stack): " << pushed_stack_variable_trace_vector[pushed_stack_variable_trace_vector.size() - 1].to_string() << std::endl;
				}

				// stack_log << "[TRACE] argument_count: " << argument_count << ", is_return_value: " << is_return_value << std::endl;

				i += 4;
			}
			else if (token == zPAR_TOK_SETINSTANCE)
			{
				token_value = 0;
				token_symbol = nullptr;

				i += 4;
			}
			else if (token == zPAR_TOK_JUMP) // unconditional jump, dont compare stack
			{
				is_goto_found = true;

				token_value = 0;
				token_symbol = nullptr;

				i += 4;
			}
			else if (token == zPAR_TOK_JUMPF)
			{
				is_goto_found = true;

				// Archolos::REMOVEMILITIAARMOR_QUESTS_APPLY::0x00378ACC
				if (!pushed_stack_variable_trace_vector.empty())
					pushed_stack_variable_trace_vector.pop_back();

				token_value = 0;
				token_symbol = nullptr;

				i += 4;
			}
			else if (token == zPAR_OP_EQUAL || token == zPAR_OP_NOTEQUAL
				|| token == zPAR_OP_LOWER_EQ || token == zPAR_OP_HIGHER_EQ
				|| token == zPAR_OP_LOWER || token == zPAR_OP_HIGHER
				|| token == zPAR_OP_OR || token == zPAR_OP_AND
				|| token == zPAR_OP_LOG_OR || token == zPAR_OP_LOG_AND
				|| token == zPAR_OP_SHIFTL || token == zPAR_OP_SHIFTR)
			{
				if (!pushed_stack_variable_trace_vector.empty())
					pushed_stack_variable_trace_vector.pop_back();

				if (!pushed_stack_variable_trace_vector.empty())
					pushed_stack_variable_trace_vector.pop_back();

				auto result_stack_variable = StackVariable();
				result_stack_variable.name = "RESULT OF OPERATION " + stack_token_map[token];
				result_stack_variable.appear_section_name = section_stack_data.name;

				auto result_stack_variable_trace  = StackVariableTrace();
				result_stack_variable_trace.stack_variable = result_stack_variable;

				pushed_stack_variable_trace_vector.push_back(result_stack_variable_trace);

				token_value = 0;
				token_symbol = nullptr;
			}
			else if (token == zPAR_OP_ISPLUS || token == zPAR_OP_ISMINUS
				|| token == zPAR_OP_ISMUL || token == zPAR_OP_ISDIV) // -=, +=, /=, *=
			{
				pushed_stack_variable_trace_vector.pop_back();
				pushed_stack_variable_trace_vector.pop_back();

				token_value = 0;
				token_symbol = nullptr;
			}
			else if (token == zPAR_TOK_ASSIGNINST || token == zPAR_OP_IS) // 0x4A, MOVINST || 0x09, MOVINT
			{
				// String Bulder Section
				if (lego_found && token == zPAR_OP_IS && last_function_name == "SB_NEW")
				{
					used_sb = pushed_stack_variable_trace_vector.back().stack_variable.name;
					last_function_name = "";
				}

				pushed_stack_variable_trace_vector.pop_back();
				pushed_stack_variable_trace_vector.pop_back();

				token_value = 0;
				token_symbol = nullptr;
			}
			else if (token == zPAR_TOK_ASSIGNSTR) // 0x46, MOVS, MOVSTRING
			{
				auto & svt_f = pushed_stack_variable_trace_vector[pushed_stack_variable_trace_vector.size() - 2];
				auto & svt_t = pushed_stack_variable_trace_vector[pushed_stack_variable_trace_vector.size() - 1];

				svt_f.extra_stack_variable_vector.push_back(svt_t.stack_variable);

				if (moved_variable_value_trace_map.count(svt_f.stack_variable.value))
				{
					svt_f = moved_variable_value_trace_map[svt_f.stack_variable.value];
					// stack_log << "[TRACE] found move: " << svt_f.to_string() << std::endl;
				}

				int64_t value_f = svt_f.stack_variable.value;
				int64_t value_t = svt_t.stack_variable.value;

				moved_variable_value_trace_map[value_t] = svt_f;

				// stack_log << "[TRACE] value_f : " << value_f << std::endl;
				// stack_log << "[TRACE] value_t : " << value_t << std::endl;

				// stack_log << "[TRACE] move string from : " << svt_f.to_string() << std::endl;
				// stack_log << "[TRACE] move string to : " << svt_t.to_string() << std::endl;
				// stack_log << "[TRACE] moved_variable_value_trace_map now: " << moved_variable_value_trace_map[value_t].to_string() << std::endl;

				// class fields can be change dynamicly in function
				if (is_section_instance || is_section_prototype || is_section_function)
				{
					auto splited_field_name_vector = split_string(svt_t.stack_variable.name, ".");

					if (splited_field_name_vector.size() == 2)
					{
						if (class_with_translated_field_map.count(splited_field_name_vector[0]))
						{
							auto & class_with_translated_field = class_with_translated_field_map[splited_field_name_vector[0]];

							for (auto & translated_field : class_with_translated_field.translated_field_vector)
							{
								if (translated_field.name != splited_field_name_vector[1])
									continue;

								auto source_data = SourceData();
								source_data.section_name = section_fwta.name;
								source_data.function_name = class_with_translated_field.name;
								source_data.argument_name = translated_field.name;
								source_data.argument_array_index = svt_t.stack_variable.array_index;
								source_data.is_array = translated_field.is_array;

								// log << "[LOG] section_name: " << source_data.section_name << std::endl;
								// log << "[LOG] function_name: " << source_data.function_name << std::endl;
								// log << "[LOG] argument_array_index: " << source_data.argument_array_index << std::endl;
								// log << "[LOG] is_array: " << source_data.is_array << std::endl;

								// need a copy!!!
								auto stack_variable_vector = svt_f.extra_stack_variable_vector;
								stack_variable_vector.push_back(svt_f.stack_variable);

								for (auto & stack_variable : stack_variable_vector)
								{
									check_add_stack_variable(loc_parser, stack_variable, source_data, is_menu);
								}
							}
						}
					}

					// for (auto & cwtf : class_with_translated_fields)
					// {
					// 	if (cwtf == svt_t.stack_variable.name)
					// 	{
					// 		auto source_data = SourceData();
					// 		source_data.section_name = section_fwta.name;
					//
					// 		source_data.function_name = splited_name[0];
					// 		source_data.argument_name = svt_t.stack_variable.name;
					// 		source_data.argument_array_index = svt_t.stack_variable.array_index;
					// 		source_data.is_array = true;
					// 		auto source = create_source_v2(svt_f.stack_variable.name, source_data);
					// 		log << "class_with_translated_fields: " << source << std::endl;
					//
					// 		// token_symbol = loc_parser->symtab.table[svt_f.stack_variable.value];
					// 		// std::string variable_text = svt_t.stack_variable.value
					// 		// script_string_data_add(variable_text, source, "100",
					// 		// 	std::to_string(argument_symbol_index), std::to_string(argument_array_index));
					// 	}
					// }
				}

				pushed_stack_variable_trace_vector.pop_back();
				pushed_stack_variable_trace_vector.pop_back();

				token_value = 0;
				token_symbol = nullptr;

				// stack_log << "[TRACE] MOVS svt_f OK: " << svt_f.to_string() << std::endl;
				// stack_log << "[TRACE] MOVS svt_t OK: " << svt_t.to_string() << std::endl;
				// stack_log.flush();
			}
			else if (token == zPAR_OP_PLUS || token == zPAR_OP_MINUS
				|| token == zPAR_OP_MUL || token == zPAR_OP_DIV || token == zPAR_OP_MOD)
			{
				pushed_stack_variable_trace_vector.pop_back();
				pushed_stack_variable_trace_vector.pop_back();

				auto result_stack_variable = StackVariable();
				result_stack_variable.name = "RESULT OF OPERATION " + stack_token_map[token];
				result_stack_variable.appear_section_name = section_stack_data.name;

				auto result_stack_variable_trace  = StackVariableTrace();
				result_stack_variable_trace.stack_variable = result_stack_variable;

				pushed_stack_variable_trace_vector.push_back(result_stack_variable_trace);

				token_value = 0;
				token_symbol = nullptr;
			}
			else if (token == zPAR_TOK_RET)
			{
				// stack_log << "[TRACE] RETURN FROM SECTION: " << section_stack_data.name << std::endl;
				// stack_log << "[TRACE] TYPE: " << section_symbol->type
				// << ", ARGUMENT COUNT: " << section_symbol->ele
				// << ", RETURN: " << section_symbol->HasFlag(zPAR_FLAG_RETURN) << std::endl;

				// for multiple return in functions
				if (is_goto_found && i + 2 < address_begin + size)
				{
					// stack_log << "[TRACE] NOT LAST" << std::endl;
					if (section_symbol->HasFlag(zPAR_FLAG_RETURN))
					{
						// WTF? Try to fix this shit...
						if (!pushed_stack_variable_trace_vector.empty())
						{
							pushed_stack_variable_trace_vector.pop_back();
						}
					}
				}
				else
				{
					// stack_log << "[TRACE] LAST" << std::endl;
					is_last_return = true;
				}

				if (is_edit_string_func_found_in_section && is_last_return && !pushed_stack_variable_trace_vector.empty() && is_section_function)
				{
					auto & return_variable_trace = pushed_stack_variable_trace_vector.back();
					if (moved_variable_value_trace_map.count(return_variable_trace.stack_variable.value))
					{
						return_variable_trace = moved_variable_value_trace_map[return_variable_trace.stack_variable.value];
					}

					function_with_edit_string_add_section(loc_parser, section_fwta,
						edit_string_stack_variable_trace_vector, return_variable_trace);
				}
			}
			else
			{
				token_value = 0;
				token_symbol = nullptr;
			}
		}

		if (section_symbol->HasFlag(zPAR_FLAG_RETURN))
		{
			if (!pushed_stack_variable_trace_vector.empty())
			{
				uint64_t stack_variable_trace_argment_index = pushed_stack_variable_trace_vector.size() - 1;
				auto & stack_variable_trace_argment = pushed_stack_variable_trace_vector[stack_variable_trace_argment_index];
				if (moved_variable_value_trace_map.count(stack_variable_trace_argment.stack_variable.value))
				{
					stack_variable_trace_argment = moved_variable_value_trace_map[stack_variable_trace_argment.stack_variable.value];
					pushed_stack_variable_trace_vector[pushed_stack_variable_trace_vector.size() - 1] = stack_variable_trace_argment;
				}
			}

			if (pushed_stack_variable_trace_vector.size() < section_disired_stack_size)
			{
				auto return_stack_variable = StackVariable();
				return_stack_variable.name = "FAKE RETURN VALUE";
				return_stack_variable.appear_section_name = section_stack_data.name;

				auto return_stack_variable_trace  = StackVariableTrace();
				return_stack_variable_trace.stack_variable = return_stack_variable;

				pushed_stack_variable_trace_vector.push_back(return_stack_variable_trace);
			}

			// stack_log << "[TRACE] return value (last stack): " << pushed_stack_variable_trace_vector[pushed_stack_variable_trace_vector.size() - 1].to_string() << std::endl;
		}

		if (!function_trace_tree_vector.empty())
			function_trace_tree_vector.pop_back();

		function_with_translated_argument_map[section_fwta.name] = section_fwta;

		// log << std::endl;
		// log << std::endl;
    }

	void translate_parser(const zCParser* local_parser, const std::string& virtual_machine_hash)
	{
		int64_t min_translate_value = std::stoi(json_settings["min_translate_value"]);
		std::unordered_map<std::string, uint64_t> translate_count_map;

		bool is_script = local_parser == parser; // mb this is problem
		bool is_menu = local_parser == parserMenu; // mb this is problem
		std::unordered_map<std::string, std::string> empty_string_entry;

		for (int64_t i = 0; i < local_parser->symtab.table.GetNum(); ++i)
		{
			auto symbol = local_parser->symtab.table[i];

			if (symbol->type != zPAR_TYPE_STRING)
				continue;

			if (!symbol->HasFlag(zPAR_FLAG_CONST))
				continue;

			for (int j = 0; j < symbol->ele; ++j)
			{
				std::string symbol_name = (const char*)symbol->name;
				std::string symbol_string_data = (const char*)symbol->stringdata[j];

				auto& string_entry = empty_string_entry;
				if (is_menu)
				{
					if (menu_string_data.count(symbol_string_data) <= 0)
						continue;

					string_entry = menu_string_data[symbol_string_data];
				}
				else if (is_script)
				{
					if (translate_string_data.count(symbol_string_data) <= 0)
						continue;

					if (translate_string_data[symbol_string_data].count(virtual_machine_hash) <= 0)
						continue;

					string_entry = translate_string_data[symbol_string_data][virtual_machine_hash];
				}

				if (string_entry.empty())
					continue;

				uint64_t translate_count = 1;
				if (translate_count_map.count(symbol_string_data) > 0)
					translate_count = translate_count_map[symbol_string_data];

				std::string translate_value_with_num = "translate_value";
				if (translate_count > 1)
					translate_value_with_num += std::to_string(translate_count);

				int64_t translate_value = std::stoi(string_entry[translate_value_with_num]);
				if (translate_value < min_translate_value)
				{
					translate_count_map[symbol_string_data] = translate_count + 1;
					continue;
				}

				std::string translated_string_with_num = "translated_string";
				if (translate_count > 1)
					translated_string_with_num += std::to_string(translate_count);

				std::string translated_string = string_entry[translated_string_with_num];
				if (translated_string.empty())
				{
					auto encoded_translated_string = encode_string(symbol_string_data, json_settings["script_encoding"], "utf8");
					zSTRING zstr = encoded_translated_string.c_str();
					symbol->SetValue(zstr, j);

					translate_count_map[symbol_string_data] = translate_count + 1;
					continue;
				}

				zSTRING zstr = translated_string.c_str();
				symbol->SetValue(zstr, j);

				translate_count_map[symbol_string_data] = translate_count + 1;
			}
		}
	}

	void collect_string_for_encoding()
	{
		for (int64_t i = parser->symtab.table.GetNum() - 1; i > 0; --i)
		{
			auto symbol = parser->symtab.table[i];

			if (symbol->type != zPAR_TYPE_STRING)
				continue;

			if (!symbol->HasFlag(zPAR_FLAG_CONST))
				continue;

			std::string symbol_string_data = (const char*)symbol->stringdata[0];

			if (!is_need_to_translate(symbol_string_data))
				continue;

			if (symbol_string_data.substr(symbol_string_data.size() - 1, 1) != ".")
				continue;

			if (symbol_string_data.length() > 32)
				encoding_detection_string += symbol_string_data + " ";

			if (encoding_detection_string.length() > 32 * 8)
				return;
		}
	}

	void parse_script()
	{
		// 1. parse all strings with order
		for (int64_t i = 0; i < parser->symtab.table.GetNum(); i++)
		{
			auto symbol = parser->symtab.table[i];
			std::string symbol_name = (const char*)symbol->name;

			if (!ikarus_found)
			{
				if (symbol_name == "IKARUS_VERSION")
				{
					ikarus_found = true;
				}
			}

			if (!lego_found)
			{
				if (symbol_name == "LEGO_VERSION")
				{
					lego_found = true;
				}
			}

			if (symbol->type != zPAR_TYPE_STRING)
				continue;

			if (!symbol->HasFlag(zPAR_FLAG_CONST))
				continue;

			for (int64_t j = 0; j < symbol->ele; ++j)
			{
				std::string symbol_string_data = (const char*)symbol->stringdata[j];
				std::string source = "[" + symbol_name + "]";
				if (symbol->ele > 1)
					source += "[" + std::to_string(j) + "]";
				if (symbol->ele == 1)
				{
					std::string symbol_name_temp = std::string(symbol_name); // need a copy
					if (symbol_name_temp.length() > 1)
					{
						symbol_name_temp = symbol_name.substr(1);
					}
					int64_t raw_string_index = 0;
					try {
						raw_string_index = std::stoi(symbol_name_temp);
					} catch (...) {}
					if (raw_string_index >= 10000)
					{
						source = "[RAW_STRING]";
					}
				}

				// log << "[LOG] find string, name: " << symbol_name << ", source: " << source << ", data: " << symbol_string_data << std::endl;

				uint64_t translate_value = calculate_translate_value(symbol_name, symbol_string_data);
				script_string_data_add(symbol_string_data, source, std::to_string(translate_value),
					std::to_string(i), std::to_string(j));
			}
		}

		// // 2. fill virtual_address_map and virtual_address_index_map, for get a string source
		// for (int i = 0; i < parser->symtab.table.GetNum(); ++i)
		// {
		// 	auto symbol = parser->symtab.table[i];
		// 	auto virtual_address = reinterpret_cast<uint64_t>(symbol->adr);
		//
		// 	if (virtual_address > 0)
		// 	{
		// 		std::string symbol_name = (const char*)symbol->name;
		// 		virtual_address_name_map[virtual_address] = symbol_name;
		// 		virtual_address_index_map[virtual_address] = i;
		// 	}
		// }


		if (ikarus_found)
		{
			function_with_translated_argument_map.insert(
				ikarus_function_with_translated_argument_map.begin(),
				ikarus_function_with_translated_argument_map.end());
		}

		auto stack_virtual_address = reinterpret_cast<int64_t>(parser->stack.stack);
		int64_t stack_size = parser->stack.GetDynSize();

		// log << "stack_virtual_address: " << stack_virtual_address << std::endl;
		// log << "stack_virtual_address end: " << stack_virtual_address + stack_size << std::endl;
		// log << "stack_size: " << stack_size << std::endl;
		// log << "ikarus_found: " << ikarus_found << std::endl;
		// log << "lego_found: " << lego_found << std::endl;

		// 2. fill virtual_address_map and virtual_address_index_map, for get a string source
		for (int64_t i = 0; i < parser->symtab.table.GetNum(); ++i)
		{
			auto symbol = parser->symtab.table[i];
			auto virtual_address = reinterpret_cast<int64_t>(symbol->adr);

			// only virtual
			if (virtual_address > 0 && virtual_address < stack_size)
			{
				auto stack_data = StackData();
				stack_data.name = (const char*)symbol->name;
				stack_data.index = i;
				stack_data.address_begin = virtual_address;

				script_address_stack_data_map[virtual_address] = stack_data;

				if (symbol->type == zPAR_TYPE_FUNC)
				{
					if (ikarus_found)
					{
						if (stack_data.name == "MEM_CHECKVERSION") // first ikarus function
						{
							// log << "MEM_CHECKVERSION: " << virtual_address << std::endl;
							ikarus_min_address = min(ikarus_min_address, virtual_address);
						}

						if (stack_data.name == "PRINTF") // last ikarus function
						{
							// log << "PRINTF: " << virtual_address << std::endl;
							ikarus_max_address = max(ikarus_max_address, virtual_address);
						}
					}

					if (lego_found)
					{
						if (stack_data.name == "ATAN2F") // first lego function
						{
							// log << "ATAN2F: " << virtual_address << std::endl;
							lego_min_address = min(lego_min_address, virtual_address);
						}

						if (stack_data.name == "LEGO_INIT") // last lego function
						{
							// log << "LEGO_INIT: " << virtual_address << std::endl;
							lego_max_address = max(lego_max_address, virtual_address);
						}
					}
				}
			}
		}

		// log << "ikarus_min_address: " << ikarus_min_address << std::endl;
		// log << "ikarus_max_address: " << ikarus_max_address << std::endl;
		// log << "lego_min_address: " << lego_min_address << std::endl;
		// log << "lego_max_address: " << lego_max_address << std::endl;

		/// WAT WE DO WITH LAST???????????? stack_size THIS!!!
		int64_t virtual_address_prev = -1;
		uint64_t virtual_address_last = script_address_stack_data_map.rbegin()->first;
		for (auto & [virtual_address, stack_data] : script_address_stack_data_map)
		{
			if (virtual_address_prev > 0)
			{
				auto & stack_data_prev = script_address_stack_data_map[virtual_address_prev];
				stack_data_prev.address_end = stack_data.address_begin;
				stack_data_prev.size = stack_data_prev.address_end - stack_data_prev.address_begin;

				// with full data
				script_index_stack_data_map[stack_data_prev.index] = stack_data_prev;
				script_name_stack_data_map[stack_data_prev.name] = stack_data_prev;
			}

			if (virtual_address == virtual_address_last)
			{
				stack_data.address_end = stack_size;
				stack_data.size = stack_data.address_end - stack_data.address_begin;

				// with full data
				menu_index_stack_data_map[stack_data.index] = stack_data;
				menu_name_stack_data_map[stack_data.name] = stack_data;
			}

			// stack_data_prev = stack_data;
			virtual_address_prev = virtual_address;
		}


		// auto stack_variable_t_1 = StackVariable("arg1");
		// auto stack_variable_t_2 = StackVariable("arg2");
		// auto stack_variable_t_3 = StackVariable("arg2");
		// auto stack_variable_trace_t_1 = StackVariableTrace();
		// auto stack_variable_trace_t_2 = StackVariableTrace();
		// auto stack_variable_trace_t_3 = StackVariableTrace();
		// stack_variable_trace_t_1.stack_variable = stack_variable_t_1;
		// stack_variable_trace_t_2.stack_variable = stack_variable_t_2;
		// stack_variable_trace_t_3.stack_variable = stack_variable_t_3;
		// auto stack_data = StackData();








		// for (auto & [virtual_address, stack_data] : address_stack_data_map)
		// {
		// 	log << "name: " << stack_data.name;
		// 	log << ", index: " << stack_data.index;
		// 	log << ", address_begin: " << stack_data.address_begin;
		// 	log << ", address_end: " << stack_data.address_end;
		// 	log << ", size: " << stack_data.size << std::endl;
		// }


		// if (address_stack_data_map.count(virtual_address_old))
		// {
		//
		// }
		//
		// virtual_address_old = virtual_address;
		// uint64_t virtual_address_old = 0xFFFFFFFF;

		// 3. add strings from vm with source
		// parse_stack(parser->stack.stack, parser->stack.GetDynSize(), parser); <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

		// DIA_ADDON_CAVALORN_TEACH_CHOICES, B_BUILDLEARNSTRING




		// std::remove("stack_log.txt");




		// std::unordered_map<std::string, bool> exception_func;
		// exception_func["B_REMOVEGHOST"] = true; // fake return
		// exception_func["B_MM_WISPDETECT"] = true; // fake return, 2 stack instead 0...
		// exception_func["B_MM_DESYNCHRONIZE"] = true; // fake return
		// exception_func["ZS_GREENTENTACLE"] = true; // fake return

		uint64_t counter = 0;
		for (auto & [virtual_address, stack_data] : script_address_stack_data_map)
		{
			// if (counter > 1000)
			// 	break;

			// std::remove("stack_log.txt");
			// log << "trace_stack, stack_data.name: " << stack_data.name << ", virtual_address: " << virtual_address << std::endl;

			auto symbol = parser->symtab.table[stack_data.index];

			moved_variable_value_trace_map = {};

			bool func_or_inst = symbol->type == zPAR_TYPE_FUNC || symbol->type == zPAR_TYPE_INSTANCE || symbol->type == zPAR_TYPE_PROTOTYPE;
			bool is_ikarus_section = virtual_address >= ikarus_min_address && virtual_address <= ikarus_max_address;
			bool is_lego_section = virtual_address >= lego_min_address && virtual_address <= lego_max_address;
			bool adress_check = virtual_address > 0 && !is_ikarus_section && !is_lego_section;

			// log << "trace_stack, func_or_inst: " << func_or_inst << std::endl;
			// log << "trace_stack, is_ikarus_section: " << is_ikarus_section << std::endl;
			// log << "trace_stack, is_lego_section: " << is_lego_section << std::endl;
			// log << "trace_stack, !symbol->HasFlag(zPAR_FLAG_EXTERNAL): " << !symbol->HasFlag(zPAR_FLAG_EXTERNAL) << std::endl;

			if (func_or_inst && !symbol->HasFlag(zPAR_FLAG_EXTERNAL) && adress_check)
			{
				// if (stack_data.name != "PRINTS_EXT")
				// 	continue;

				pushed_stack_variable_trace_vector = {};
				for (int j = 0; j < symbol->ele; ++j)
				{
					auto stack_variable_t = StackVariable();
					stack_variable_t.name = "ARGUMENT_" + std::to_string(j);
					auto stack_variable_trace_t = StackVariableTrace();
					stack_variable_trace_t.stack_variable = stack_variable_t;
					pushed_stack_variable_trace_vector.push_back(stack_variable_trace_t);
				}

				// function_trace_tree_vector.emplace_back(stack_data.n);

				// function_trace_tree_vector = {};
				// moved_variable_value_trace_map = {};

				// log << "trace_stack, stack_data.name: " << stack_data.name << std::endl;
				trace_stack(parser->stack.stack, virtual_address, stack_data.size, parser);

				// if (pushed_stack_variable_trace_vector.size() == 0 || pushed_stack_variable_trace_vector.size() > 1)
				// {
				// 	// if (symbol->HasFlag(zPAR_FLAG_RETURN) && exception_func.count(symbol_name) == 0)
				// 	// 	break;
				// }

				counter += 1;
			}
		}


		// for (auto & function_with_translated_argument : function_with_translated_argument_map)
		// {
		// 	log << "function_with_translated_argument_map: " << function_with_translated_argument.second.to_string() << std::endl;
		// }

		// for (auto & function_with_edit_string_argument : function_with_edit_string_argument_map)
		// {
		// 	log << "function_with_edit_string_argument: " << function_with_edit_string_argument.second.to_string() << std::endl;
		// }


		// fix AI
		for (auto & [virtual_address, stack_data] : script_address_stack_data_map)
		{
			auto symbol = parser->symtab.table[stack_data.index];

			moved_variable_value_trace_map = {};

			if (stack_data.is_use_ai_function)
			{
				pushed_stack_variable_trace_vector = {};
				for (int j = 0; j < symbol->ele; ++j)
				{
					auto stack_variable_t = StackVariable();
					stack_variable_t.name = "ARGUMENT_" + std::to_string(j);
					auto stack_variable_trace_t = StackVariableTrace();
					stack_variable_trace_t.stack_variable = stack_variable_t;
					pushed_stack_variable_trace_vector.push_back(stack_variable_trace_t);
				}

				// log << "stack_data.is_use_ai_function: " << stack_data.name << std::endl;
				trace_stack(parser->stack.stack, virtual_address, stack_data.size, parser);
			}
		}
	}

	void parse_ou(bool is_translate = false)
	{
		// steeled from https://github.com/Gratt-5r2/zFont/blob/master/zFont/CharsetDetector.cpp
		auto ou_list = ogame->csMan->library->ouList;
		for (int64_t i = 0; i < ou_list.GetNum(); ++i)
		{
			auto ou = ou_list[i];
			auto& blocks = ou->blocks;

			for (int64_t j = 0; j < blocks.GetNum(); ++j)
			{
				auto& block = blocks[j];
				auto atomic_block = block.block->CastTo<zCCSAtomicBlock>();
				if (atomic_block)
				{
					auto message_conversion = atomic_block->command->CastTo<oCMsgConversation>();
					if (message_conversion)
					{
						std::string string = (const char*)message_conversion->text;

						if (is_translate)
						{
							std::string encoded_translated_string = string;
							if (ou_string_data.count(string) > 0)
							{
								std::string translated_string = ou_string_data[string]["translated_string"];
								if (translated_string.empty())
								{
									encoded_translated_string = encode_string(encoded_translated_string, json_settings["script_encoding"], "utf8");
								}
								else
								{
									encoded_translated_string = translated_string;
								}
							}
							else
							{
								encoded_translated_string = encode_string(encoded_translated_string, json_settings["script_encoding"], "utf8");
							}

							zSTRING zstr = encoded_translated_string.c_str();
							message_conversion->text = zstr;
						}
						else
						{
							if (std::count(ou_string_data_insert_order.begin(), ou_string_data_insert_order.end(), string) <= 0)
							{
								std::string source = (const char*)ou->roleName;
								remove_broken_symbols(source);
								source = "[" + source + "]";

								ou_string_data_insert_order.push_back(string);
								ou_string_data[string]["translate_source"] = source; // if same name, other source?
								ou_string_data[string]["translated_string"] = "";
							}
						}
					}
				}
			}
		}
	}

	void parse_menu()
	{
		// 1. parse all strings with order
		for (int64_t i = 0; i < parserMenu->symtab.table.GetNum(); i++)
		{
			auto symbol = parserMenu->symtab.table[i];
			std::string symbol_name = (const char*)symbol->name;

			if (symbol->type != zPAR_TYPE_STRING)
				continue;

			if (!symbol->HasFlag(zPAR_FLAG_CONST))
				continue;

			for (int64_t j = 0; j < symbol->ele; ++j)
			{
				std::string symbol_string_data = (const char*)symbol->stringdata[j];
				std::string source = "[" + symbol_name + "]";
				if (symbol->ele > 1)
					source += "[" + std::to_string(j) + "]";
				if (symbol->ele == 1)
				{
					std::string symbol_name_temp = std::string(symbol_name); // need a copy
					if (symbol_name_temp.length() > 1)
					{
						symbol_name_temp = symbol_name.substr(1);
					}
					int64_t raw_string_index = 0;
					try {
						raw_string_index = std::stoi(symbol_name_temp);
					} catch (...) {}
					if (raw_string_index >= 10000)
					{
						source = "[RAW_STRING]";
					}
				}

				// log << "[LOG] find string, name: " << symbol_name << ", source: " << source << ", data: " << symbol_string_data << std::endl;

				script_string_data_add(symbol_string_data, source, "0",
					std::to_string(i), std::to_string(j), true);
			}
		}

		auto stack_virtual_address = reinterpret_cast<int64_t>(parserMenu->stack.stack);
		int64_t stack_size = parserMenu->stack.GetDynSize();

		// 2. fill virtual_address_map and virtual_address_index_map, for get a string source
		for (int64_t i = 0; i < parserMenu->symtab.table.GetNum(); ++i)
		{
			auto symbol = parserMenu->symtab.table[i];
			auto virtual_address = reinterpret_cast<int32_t>(symbol->adr);
			int64_t offset = symbol->offset;

			bool is_function = symbol->type == zPAR_TYPE_FUNC;
			bool is_class = false; // symbol->type == zPAR_TYPE_CLASS;
			bool is_instance = symbol->type == zPAR_TYPE_INSTANCE;
			bool is_prototype = symbol->type == zPAR_TYPE_PROTOTYPE;
			bool is_offset = offset == 0 || (offset != 0 && is_function);

			// rly don't know what is offset... related to inheritance?
			// bool is_valid = offset == 0 && (is_function || is_instance || is_prototype || is_class);

			bool is_valid = is_offset && (is_function || is_instance || is_prototype || is_class);

			// log << "[LOG] symbol, name: " << (const char*)symbol->name << std::endl;
			// log << "[LOG] symbol, virtual_address: " << virtual_address << std::endl;
			// log << "[LOG] symbol, offset: " << offset << std::endl;
			// log << std::endl;
			// log.flush();

			// only virtual
			if (is_valid && virtual_address >= 0 && virtual_address < stack_size)
			{
				auto stack_data = StackData();
				stack_data.name = (const char*)symbol->name;
				stack_data.index = i;
				stack_data.address_begin = virtual_address;

				// log << "[LOG] stack_data.index: " << stack_data.index << std::endl;
				// log << "[LOG] stack_data.address_begin: " << stack_data.address_begin << std::endl;
				// log << "[LOG] stack_data.address_end: " << stack_data.address_end << std::endl;
				// log << "[LOG] stack_data.name: " << stack_data.name << std::endl;
				// log << "[LOG] stack_data.size: " << stack_data.size << std::endl;
				// log << "[LOG] symbol->type: " << symbol->type << std::endl;
				// log << std::endl;
				// log.flush();

				menu_address_stack_data_map[virtual_address] = stack_data;
			}
		}

		int64_t virtual_address_prev = -1;
		uint64_t virtual_address_last = menu_address_stack_data_map.rbegin()->first;
		for (auto & [virtual_address, stack_data] : menu_address_stack_data_map)
		{
			if (virtual_address_prev > 0)
			{
				auto & stack_data_prev = menu_address_stack_data_map[virtual_address_prev];
				stack_data_prev.address_end = stack_data.address_begin;
				stack_data_prev.size = stack_data_prev.address_end - stack_data_prev.address_begin;

				// with full data
				menu_index_stack_data_map[stack_data_prev.index] = stack_data_prev;
				menu_name_stack_data_map[stack_data_prev.name] = stack_data_prev;
			}

			if (virtual_address == virtual_address_last)
			{
				stack_data.address_end = stack_size;
				stack_data.size = stack_data.address_end - stack_data.address_begin;

				// with full data
				menu_index_stack_data_map[stack_data.index] = stack_data;
				menu_name_stack_data_map[stack_data.name] = stack_data;
			}

			// stack_data_prev = stack_data;
			virtual_address_prev = virtual_address;
		}

		// for (auto & [virtual_address, stack_data] : menu_address_stack_data_map)
		// {
		// 	log << "[LOG] virtual_address.index: " << virtual_address << std::endl;
		// 	log << "[LOG] stack_data.name: " << stack_data.name << std::endl;
		// 	log << "[LOG] stack_data.index: " << stack_data.index << std::endl;
		// 	log << "[LOG] stack_data.address_begin: " << stack_data.address_begin << std::endl;
		// 	log << "[LOG] stack_data.address_end: " << stack_data.address_end << std::endl;
		// 	log << "[LOG] stack_data.size: " << stack_data.size << std::endl;
		// 	log << std::endl;
		// 	log.flush();
		// }

		// 3. add strings from vm with source
		uint64_t counter = 0;
		for (auto & [virtual_address, stack_data] : menu_address_stack_data_map)
		{
			// if (counter > 1000)
			// 	break;

			// std::remove("stack_log.txt");

			auto symbol = parserMenu->symtab.table[stack_data.index];

			moved_variable_value_trace_map = {};

			bool func_or_inst = symbol->type == zPAR_TYPE_FUNC || symbol->type == zPAR_TYPE_INSTANCE;
			if (func_or_inst && !symbol->HasFlag(zPAR_FLAG_EXTERNAL) && virtual_address >= 0)
			{
				pushed_stack_variable_trace_vector = {};
				for (int64_t j = 0; j < symbol->ele; ++j)
				{
					auto stack_variable_t = StackVariable();
					stack_variable_t.name = "ARGUMENT_" + std::to_string(j);
					auto stack_variable_trace_t = StackVariableTrace();
					stack_variable_trace_t.stack_variable = stack_variable_t;
					pushed_stack_variable_trace_vector.push_back(stack_variable_trace_t);
				}

				trace_stack(parserMenu->stack.stack, virtual_address, stack_data.size, parserMenu, true);

				counter += 1;
			}
		}
		// // 2. add strings from vm with source
		// parse_stack(parserMenu->stack.stack, parserMenu->stack.GetDynSize(), parserMenu, true);
	}

	int64_t get_ikarus_last_address()
	{
		auto l_ikarus_found = false;
		for (int64_t i = 0; i < parser->symtab.table.GetNum(); ++i)
		{
			auto symbol = parser->symtab.table[i];
			std::string symbol_name = (const char*)symbol->name;

			if (symbol_name == "IKARUS_VERSION")
			{
				l_ikarus_found = true;
				break;
			}
		}

		if (!l_ikarus_found)
			return 0;

		for (int64_t i = 0; i < parser->symtab.table.GetNum(); ++i)
		{
			auto symbol = parser->symtab.table[i];
			auto virtual_address = reinterpret_cast<int64_t>(symbol->adr);

			// only virtual
			if (virtual_address > 0 && virtual_address < parser->stack.GetDynSize())
			{
				if (symbol->type == zPAR_TYPE_FUNC)
				{
					std::string symbol_name = (const char*)symbol->name;
					if (symbol_name == "PRINTF") // last ikarus function
					{
						return virtual_address + 18;
					}
				}
			}
		}

		return 0;
	}

	// script parser and menu parser filled, ou filled
	void hook_after_game_init_and_before_menu_appear()
	{
		// return;

		// std::this_thread::sleep_for(std::chrono::milliseconds(1000));

		// std::remove(translate_file_path.c_str());

		// std::remove("log.txt");
		// std::remove("stack_log.txt");
		// log.open("log.txt", std::ios::app);
		// stack_log.open("stack_log.txt", std::ios::app);
		// while (!log.is_open())
		// 	std::this_thread::sleep_for(std::chrono::seconds(1));
		// while (!stack_log.is_open())
		// 	std::this_thread::sleep_for(std::chrono::seconds(1));

		// std::filesystem::path current_folder_path = std::filesystem::current_path();
		// translate_file_path = current_folder_path / translate_file_path;
		// std::filesystem::create_directories(translate_file_path.parent_path());

		// при изменении хеша нужно еще оюшки все перезаписывать - но перевод не стирать



		// ikarus or something (patches?) change stack every time differently

		// auto ikarus_last_address = get_ikarus_last_address();
		//
		// // 1. we need a virtual machine hash
		// SHA256 sha;
		// sha.update(parser->stack.stack + ikarus_last_address, parser->stack.GetDynSize() - ikarus_last_address);
		// std::string virtual_machine_hash = SHA256::toString(sha.digest());
		//
		// std::stringstream bytes_string_stream;
		// bytes_string_stream << std::hex << std::uppercase << std::setfill('0');
		// for (uint32_t i = 0; i < parser->stack.GetDynSize(); ++i)
		// {
		// 	bytes_string_stream << std::setw(2) << static_cast<uint32_t>(parser->stack.stack[i]);
		// 	if (i > 0 && i % 16 == 0)
		// 	{
		// 		bytes_string_stream << std::endl;
		// 	}
		// }

		std::string virtual_machine_hash = std::to_string(parser->stack.GetDynSize());


		json_settings["script_encoding"] = std::to_string(guess_game_encoding());

		// collect_string_for_encoding();
		// auto lng = gge(encoding_detection_string);
		// log << "lng: " << encoding_detection_string << " \n";
		// log << "lng: " << lng << " \n";
		// // ExitProcess(1);
		// return;

		// parse_script();
		// // parse_ou();
		// // parse_menu();
		// write_json(false, virtual_machine_hash);
		// ExitProcess(1);
		// return;


		// 2. read a translation json file
		auto is_json_read = read_json();
		bool is_same_vm = false;
		if (is_json_read)
			is_same_vm = virtual_machine_hash == json_settings["virtual_machine_hash"];


		// log << "is_json_read: " << is_json_read << " \n";
		// log << "virtual_machine_hash: " << virtual_machine_hash << " \n";
		// log << "virtual_machine_LEN: " << parser->stack.GetDynSize() << " \n";
		// // log << "ikarus_last_address: " << ikarus_last_address << " \n";
		// log << "json_virtual_machine_hash: " << json_settings["virtual_machine_hash"] << " \n";
		// // log << "bytes_string_stream: " << bytes_string_stream.str() << " \n";
		// log.flush();

		if (is_json_read == false)
		{
			parse_script();
			parse_ou();
			parse_menu();

			// json_settings["virtual_machine_hash"] = virtual_machine_hash;
			if (json_settings["script_encoding"] == "-1")
				json_settings["script_encoding"] = std::to_string(guess_game_encoding());

			write_json(false, virtual_machine_hash);

			ExitProcess(1);

			return;
		}

		if (is_same_vm == false)
		{
			parse_script();
			parse_ou();
			parse_menu();

			// json_settings["virtual_machine_hash"] = virtual_machine_hash;
			if (json_settings["script_encoding"] == "-1")
				json_settings["script_encoding"] = std::to_string(guess_game_encoding());

			write_json(true, virtual_machine_hash);

			ExitProcess(1);

			// log << "LOG_END\n";

			return;
		}

		// if (is_json_read == false || is_same_vm == false)
		// {
		// 	parse_script();
		// 	parse_ou();
		// 	parse_menu();
		//
		// 	json_settings["virtual_machine_hash"] = virtual_machine_hash;
		// 	json_settings["script_encoding"] = std::to_string(guess_game_encoding());
		//
		// 	write_json(is_json_read);
		//
		// 	ExitProcess(1);
		//
		// 	return;
		// }

		if (is_json_read)
		{
			// translate needed script strings
			translate_parser(parser, virtual_machine_hash);

			// translate ou
			parse_ou(true);

			// translate menu
			// translate_menu();
			translate_parser(parserMenu, "");
		}


		// log << "LOG_END\n";
	}
}