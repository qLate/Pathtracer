// ReSharper disable CppInconsistentNaming
#pragma once

#include "Color.h"
#include "Debug.h"
#include "json.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/string_cast.hpp"

class JsonUtility
{
	template <typename Class, typename T>
	struct PropertyImpl
	{
		using Type = T;

		T Class::* member;
		const char* name;

		constexpr PropertyImpl(T Class::* member_, const char* name_);
	};

	template <typename T, T... S, typename F>
	static constexpr void for_sequence(std::integer_sequence<T, S...>, F&& f);

public:
	template <typename Class, typename T>
	static constexpr auto property(T Class::* member, const char* name);

	template <typename T>
	static T fromJson(const nlohmann::json& data);

	template <typename T>
	static nlohmann::json toJson(const T& object, bool includeClassType = true);

private:
	template <typename T>
	static nlohmann::json valueToJson(const T& value);
	template <>
	nlohmann::json valueToJson(const glm::vec2& value);
	template <>
	nlohmann::json valueToJson(const glm::vec3& value);
	template <>
	nlohmann::json valueToJson(const glm::vec4& value);
	template <>
	nlohmann::json valueToJson(const Color& value);
	template <>
	nlohmann::json valueToJson(const glm::ivec2& value);
	template <>
	nlohmann::json valueToJson(const glm::ivec3& value);
	template <>
	nlohmann::json valueToJson(const glm::quat& value);

	template <typename T>
	static T valueFromString(const nlohmann::json& data);
	template <>
	glm::vec2 valueFromString(const nlohmann::json& data);
	template <>
	glm::vec3 valueFromString(const nlohmann::json& data);
	template <>
	glm::vec4 valueFromString(const nlohmann::json& data);
	template <>
	Color valueFromString(const nlohmann::json& data);
	template <>
	glm::ivec2 valueFromString(const nlohmann::json& data);
	template <>
	glm::ivec3 valueFromString(const nlohmann::json& data);
	template <>
	glm::quat valueFromString(const nlohmann::json& data);
};

template <typename T>
concept HasProperties = requires
{
	{ T::properties() };
};

template <typename Class, typename T>
constexpr JsonUtility::PropertyImpl<Class, T>::PropertyImpl(T Class::* member_, const char* name_): member {member_}, name {name_} {}

template <typename Class, typename T>
constexpr auto JsonUtility::property(T Class::* member, const char* name)
{
	return PropertyImpl<Class, T> {member, name};
}

template <typename T>
T JsonUtility::fromJson(const nlohmann::json& data)
{
	if (data.is_null()) return {};
	T object {};

	constexpr auto nbProperties = std::tuple_size_v<decltype(T::properties())>;
	for_sequence(std::make_index_sequence<nbProperties> {}, [&](auto i)
	{
		constexpr auto property = std::get<i>(T::properties());
		if (!data.contains(property.name)) return;

		using Type = typename decltype(property)::Type;
		using CleanType = std::remove_pointer_t<Type>;

		if constexpr (std::is_pointer_v<Type>)
		{
			if (data[property.name] != nullptr)
				object.*property.member = new CleanType(valueFromString<CleanType>(data[property.name]));
			else
				object.*property.member = nullptr;
		}
		else
			object.*property.member = valueFromString<Type>(data[property.name]);
	});

	return object;
}
template <typename T>
nlohmann::json JsonUtility::toJson(const T& object, bool includeClassType)
{
	nlohmann::json data;

	if (includeClassType)
	{
		std::string str(typeid(object).name());
		data["classType"] = str.substr(str.find_last_of(' ') + 1);
	}

	constexpr auto nbProperties = std::tuple_size_v<decltype(T::properties())>;
	for_sequence(std::make_index_sequence<nbProperties> {}, [&](auto i)
	{
		constexpr auto property = std::get<i>(T::properties());

		using Type = typename decltype(property)::Type;

		if constexpr (std::is_pointer_v<Type>)
		{
			if (object.*property.member)
				data[property.name] = valueToJson(*(object.*property.member));
			else
				data[property.name] = nullptr;
		}
		else
			data[property.name] = valueToJson(object.*property.member);
	});

	return data;
}
template <typename T, T... S, typename F>
constexpr void JsonUtility::for_sequence(std::integer_sequence<T, S...>, F&& f)
{
	using unpack_t = int[];
	(void)unpack_t {
		(static_cast<void>(f(std::integral_constant<T, S> {})), 0)..., 0
	};
}

template <typename T>
nlohmann::json JsonUtility::valueToJson(const T& value)
{
	if constexpr (HasProperties<T>)
		return toJson(value);
	else if constexpr (std::is_same_v<T, bool> || std::is_same_v<T, float> || std::is_same_v<T, int> || std::is_same_v<T, std::string>)
		return value;
	else
		return "unsupported_type";
}
template <>
inline nlohmann::json JsonUtility::valueToJson<glm::vec<2, float>>(const glm::vec2& value)
{
	return {value.x, value.y};
}
template <>
inline nlohmann::json JsonUtility::valueToJson<glm::vec<3, float>>(const glm::vec3& value)
{
	return {value.x, value.y, value.z};
}
template <>
inline nlohmann::json JsonUtility::valueToJson<glm::vec<4, float>>(const glm::vec4& value)
{
	return {value.x, value.y, value.z, value.w};
}
template <>
inline nlohmann::json JsonUtility::valueToJson<Color>(const Color& value)
{
	return {value.x, value.y, value.z, value.w};
}
template <>
inline nlohmann::json JsonUtility::valueToJson<glm::vec<2, int>>(const glm::ivec2& value)
{
	return {value.x, value.y};
}
template <>
inline nlohmann::json JsonUtility::valueToJson<glm::vec<3, int>>(const glm::ivec3& value)
{
	return {value.x, value.y};
}
template <>
inline nlohmann::json JsonUtility::valueToJson<glm::qua<float>>(const glm::quat& value)
{
	return {value.w, value.x, value.y, value.z};
}

template <typename T>
T JsonUtility::valueFromString(const nlohmann::json& data)
{
	if constexpr (HasProperties<T>)
		return fromJson<T>(data);
	else
		return data.get<T>();
}
template <>
inline bool JsonUtility::valueFromString(const nlohmann::json& data)
{
	return data.get<bool>();
}
template <>
inline float JsonUtility::valueFromString(const nlohmann::json& data)
{
	return data.get<float>();
}
template <>
inline int JsonUtility::valueFromString(const nlohmann::json& data)
{
	return data.get<int>();
}
template <>
inline std::string JsonUtility::valueFromString(const nlohmann::json& data)
{
	return data;
}
template <>
inline glm::vec2 JsonUtility::valueFromString<glm::vec<2, float>>(const nlohmann::json& data)
{
	return {data[0].get<float>(), data[1].get<float>()};
}
template <>
inline glm::vec3 JsonUtility::valueFromString<glm::vec<3, float>>(const nlohmann::json& data)
{
	return {data[0].get<float>(), data[1].get<float>(), data[2].get<float>()};
}
template <>
inline glm::vec4 JsonUtility::valueFromString<glm::vec<4, float>>(const nlohmann::json& data)
{
	return {data[0].get<float>(), data[1].get<float>(), data[2].get<float>(), data[3].get<float>()};
}
template <>
inline Color JsonUtility::valueFromString<Color>(const nlohmann::json& data)
{
	return {data[0].get<float>(), data[1].get<float>(), data[2].get<float>(), data[3].get<float>()};
}
template <>
inline glm::ivec2 JsonUtility::valueFromString<glm::vec<2, int>>(const nlohmann::json& data)
{
	return {data[0].get<int>(), data[1].get<int>()};
}
template <>
inline glm::ivec3 JsonUtility::valueFromString<glm::vec<3, int>>(const nlohmann::json& data)
{
	return {data[0].get<int>(), data[1].get<int>(), data[2].get<int>()};
}
template <>
inline glm::quat JsonUtility::valueFromString<glm::qua<float>>(const nlohmann::json& data)
{
	return {data[0].get<float>(), data[1].get<float>(), data[2].get<float>(), data[3].get<float>()};
}
