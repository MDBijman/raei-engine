#pragma once
#include <algorithm>
#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>


namespace JSON
{
	class JSON
	{
	private:
		enum types
		{
			object,
			string,
			number,
			array,
			boolean,
			null
		};

		union data_t
		{
			std::map<std::string, JSON>* object;
			std::vector<JSON>* array;
			std::string* string;
			int number;
			bool boolean;

			~data_t() {}
			data_t() = default;
			data_t(std::map<std::string, JSON>* obj) : object(obj) {}
			data_t(std::vector<JSON>* a) : array(a) {}
			data_t(std::string* str) : string(str) {}
			data_t(int numb) : number(numb) {}
			data_t(bool b) : boolean(b) {}
		};

		data_t data = {};
		types type;

		JSON& operator[](int index) const
		{
			return data.array->at(index);
		}

	public:
		/*
			Constructors
		*/

		JSON() : type(types::null) {}

		JSON(const std::string& value) : type(types::string), data(new std::string(value)) {}

		JSON(const char* value) : type(types::string), data(new std::string(value)) {}

		JSON(const std::map<std::string, JSON>& value) : type(types::object), data(new std::map<std::string, JSON>(value)) {}

		JSON(const std::vector<JSON>& value) : type(types::array), data(new std::vector<JSON>(value)) {}

		JSON(int value) : type(types::number), data(value) {}

		JSON(bool value) : type(types::boolean), data(value) {}

		JSON(const JSON& value) : type(value.type)
		{
			switch (type)
			{
			case types::array:
				data = value.data.array;
				break;
			case types::boolean:
				data = value.data.boolean;
				break;
			case types::number:
				data = value.data.number;
				break;
			case types::object:
				data = value.data.object;
				break;
			case types::string:
				data = value.data.string;
				break;
			}
		}

		JSON(std::initializer_list<JSON> list)
		{
			bool isObject = true;
			for (const auto& elem : list)
			{
				if (!elem.isArray() || !elem[0].isString() || elem.size() != 2)
				{
					isObject = false;
					break;
				}
			}

			if (isObject)
			{
				type = types::object;
				data.object = new std::map<std::string, JSON>();
				for (auto& elem : list)
				{
					data.object->emplace(*elem[0].data.string, elem[1]);
				}
			}
			else
			{
				type = types::array;
				data.array = new std::vector<JSON>(list);
			}
		}

		/*
			Size information
		*/

		size_t size() const
		{
			switch (type)
			{
			case types::null:
				return 0;
				break;
			case types::object:
				return data.object->size();
				break;
			case types::array:
				return data.array->size();
				break;
			default:
				return 1;
				break;
			}
		}

		/*
			Type information
		*/

		bool isObject() const { return type == types::object; }
		bool isArray() const { return type == types::array; }
		bool isString() const { return type == types::string; }
		bool isNumber() const { return type == types::number; }
		bool isBoolean() const { return type == types::boolean; }
		bool isNull() const { return type == types::null; }

		/*
			Implicit conversion operators
		*/

		operator std::map<std::string, JSON>&()
		{
			return *data.object;
		}

		operator std::vector<JSON>&()
		{
			return *data.array;
		}

		operator std::string&()
		{
			return *data.string;
		}

		operator int()
		{
			return data.number;
		}

		operator bool()
		{
			return data.boolean;
		}

		/*
			Access operators
		*/

		JSON& operator [](const std::string& name)
		{
			if (type == types::null)
			{
				type = types::object;
				data.object = new std::map<std::string, JSON>();
			}

			if (data.object->find(name) == data.object->end())
				data.object->insert({ name, JSON() });

			return data.object->at(name);
		}

		JSON& operator [](const char* name)
		{
			if (type == types::null)
			{
				type = types::object;
				data.object = new std::map<std::string, JSON>();
			}

			if (data.object->find(name) == data.object->end())
				data.object->insert({ name, JSON() });

			return data.object->at(name);
		}

		JSON& operator [](int index)
		{
			if (type == types::null)
			{
				type = types::array;
				data.array = new std::vector<JSON>();
			}
			else if (type != types::array)
			{
				throw std::domain_error("Wrong type");
			}
			int res = data.array->size() - 1;
			while (static_cast<int>(data.array->size()) - 1 < index)
			{
				data.array->push_back(JSON());
			}

			return data.array->at(index);
		}

		static JSON getArray()
		{
			return JSON(std::vector<JSON>());
		}
	};

	class Parser
	{
	public:
		static JSON parse(std::string content)
		{
			// Remove whitespace
			content.erase(std::remove_if(content.begin(), content.end(), ::isspace), content.end());
			return parseObject(content);
		}

		static JSON parseNumber(std::string content)
		{

			std::stringstream ss(content);
			int numb;
			ss >> numb;
			return JSON(numb);
		}

		static JSON parseBoolean(std::string content) {
			if (content.compare("true") == 0)
				return JSON(true);
			else if (content.compare("false") == 0)
				return JSON(false);
		}

		static JSON parseString(std::string content)
		{
			content.erase(0, 1);
			content.erase(content.length() - 1, 1);
			return JSON(content);
		}

		static JSON parseArray(std::string content)
		{
			JSON json = JSON::getArray();

			content.erase(0, 1);
			content.erase(content.length() - 1, 1);
			std::vector<uint32_t> seperators = findCommaSeperators(content);
			std::vector<std::string> components = splitAt(seperators, content);

			for (int i = 0; i < components.size(); i++)
			{
				 json[i] = parseAny(components[i]);
			}
			return json;
		}

		static JSON parseObject(std::string content)
		{
			JSON json;
			// Remove first and last character (brackets)
			content.erase(content.length() - 1, 1);
			content.erase(0, 1);

			std::vector<uint32_t> seperators = findCommaSeperators(content);
			std::vector<std::string> components = splitAt(seperators, content);

			for (std::string component : components)
			{
				// Erase first "
				component.erase(0, 1);
				size_t pos = component.find('"');
				std::string name = component.substr(0, pos);
				component.erase(0, pos + 2);

				json[name] = parseAny(component);
			}
			return json;
		}

		static JSON parseAny(std::string content)
		{
			switch (content.at(0))
			{
			case '{': // Nested Object
				return parseObject(content);
			case '[': // Array
				return parseArray(content);
			case '"': // String
				return parseString(content);
			case 't':
			case 'f':
				return parseBoolean(content);
			default: // Unsigned Number
				return parseNumber(content);
			}
		}

	private:
		static std::vector<std::string> splitAt(std::vector<uint32_t> seperators, std::string content)
		{
			std::vector<std::string> components;

			uint32_t prevLoc = -1;
			for (uint32_t loc : seperators)
			{
				components.push_back(content.substr(prevLoc + 1, loc - prevLoc - 1));
				prevLoc = loc;
			}
			components.push_back(content.substr(prevLoc + 1, content.length() - prevLoc - 1));
			return components;
		}

		static std::vector<uint32_t> findCommaSeperators(std::string content)
		{
			std::vector<uint32_t> locations;

			uint32_t bracketDepth = 0;
			uint32_t arrayDepth = 0;
			bool inString = false;

			const char* text = content.c_str();
			for (uint32_t location = 0; location < content.length(); location++)
			{
				if (inString && text[location] != '"')
					continue;

				switch (text[location])
				{
				case '"':
					inString = !inString;
					break;
				case ',':
					if (bracketDepth == 0 && arrayDepth == 0)
						locations.push_back(location);
					break;
				case '{':
					bracketDepth++;
					break;
				case '}':
					bracketDepth--;
					break;
				case '[':
					arrayDepth++;
					break;
				case ']':
					arrayDepth--;
					break;
				}
			}

			return locations;
		}
	};
}

