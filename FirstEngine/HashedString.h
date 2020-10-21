#pragma once

#include <string>

class HashedString {
public:
	explicit HashedString(char const* const pIdentString) : m_ident(hash_name(pIdentString)), m_identStr(pIdentString) {}

	unsigned long getHashValue() const;
	const std::string& getStr() const;

	static void* hash_name(char const* pIdentStr);

	bool operator<(HashedString const& o) const;
	bool operator==(HashedString const& o) const;

private:

	// note: m_ident is stored as a void* not an int, so that in
	// the debugger it will show up as hex-values instead of
	// integer values. This is a bit more representative of what
	// we're doing here and makes it easy to allow external code
	// to assign event types as desired.
	void* m_ident;
	std::string	m_identStr;
};