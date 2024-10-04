#include "Engine/Core/NamedStrings.hpp"

void NamedStrings::PopulateFromXmlElementAttributes(XmlElement const& element)
{
	const XmlAttribute* attr = element.FirstAttribute();

	while (attr != nullptr)
	{
		m_keyValuePairs[attr->Name()] = attr->Value();

		attr = attr->Next();
	}
}

void NamedStrings::SetValue(std::string const& keyName, std::string const& newValue)
{
	m_keyValuePairs[keyName] = newValue;
}

std::string NamedStrings::GetValue(std::string const& keyName, std::string const& defaultValue) const
{
	auto found = m_keyValuePairs.find(keyName);
	if (found == m_keyValuePairs.end())
	{
		return defaultValue;
	}
	return found->second;
}

bool NamedStrings::GetValue(std::string const& keyName, bool defaultValue) const
{
	auto found = m_keyValuePairs.find(keyName);
	if (found == m_keyValuePairs.end())
	{
		return defaultValue;
	}
	if (found->second == "true")
	{
		return true;
	}
	return false;
}

int NamedStrings::GetValue(std::string const& keyName, int defaultValue) const
{
	auto found = m_keyValuePairs.find(keyName);
	if (found == m_keyValuePairs.end())
	{
		return defaultValue;
	}
	return atoi(found->second.c_str());
}

float NamedStrings::GetValue(std::string const& keyName, float defaultValue) const
{
	auto found = m_keyValuePairs.find(keyName);
	if (found == m_keyValuePairs.end())
	{
		return defaultValue;
	}
	return static_cast<float>(atof(found->second.c_str()));
}

std::string NamedStrings::GetValue(std::string const& keyName, char const* defaultValue) const
{
	auto found = m_keyValuePairs.find(keyName);
	if (found == m_keyValuePairs.end())
	{
		return defaultValue;
	}
	return found->second.c_str();
}

Rgba8 NamedStrings::GetValue(std::string const& keyName, Rgba8 const& defaultValue) const
{
	auto found = m_keyValuePairs.find(keyName);

	if (found == m_keyValuePairs.end())
	{
		return defaultValue;
	}

	Rgba8 result;
	result.SetFromText(found->second.c_str());
	return result;
}

Vec2 NamedStrings::GetValue(std::string const& keyName, Vec2 const& defaultValue) const
{
	auto found = m_keyValuePairs.find(keyName);

	if (found == m_keyValuePairs.end())
	{
		return defaultValue;
	}

	Vec2 result;
	result.SetFromText(found->second.c_str());
	return result;
}

IntVec2 NamedStrings::GetValue(std::string const& keyName, IntVec2 const& defaultValue) const
{
	auto found = m_keyValuePairs.find(keyName);

	if (found == m_keyValuePairs.end())
	{
		return defaultValue;
	}

	IntVec2 result;
	result.SetFromText(found->second.c_str());
	return result;
}

bool NamedStrings::IsKeyNameValid(std::string const& keyName)
{
	auto found = m_keyValuePairs.find(keyName);

	if (found == m_keyValuePairs.end())
	{
		return false;
	}

	return true;
}
