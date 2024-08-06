#pragma once

template<class Derived, class T>
class ResourceManager
{
public:
	ResourceManager(const std::string pathsfile) :
		m_filepath(pathsfile)
	{

	}
private:
	loadPaths(const std::string& pathsfile);
};
