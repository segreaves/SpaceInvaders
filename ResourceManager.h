#pragma once
#include "Utils.h"
#include <unordered_map>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

/// <summary>
/// Base class for the texture and sound managers.
/// </summary>
/// <typeparam name="Derived"></typeparam>
/// <typeparam name="T"></typeparam>
template<class Derived, class T>
class ResourceManager
{
public:
	ResourceManager(const std::string pathsFile)
	{
		loadPaths(pathsFile);
	}

	virtual ~ResourceManager()
	{
		purgeResources();
	}

	T* getResource(const std::string& id)
	{
		auto r = retrieve(id);
		return (r ? r->first : nullptr);
	}

	std::string getPath(const std::string& id)
	{
		auto p = m_paths.find(id);
		return (p != m_paths.end() ? p->second : "");
	}

	bool requireResource(const std::string& id)
	{
		auto r = retrieve(id);
		if (r)
		{
			r->second++;
			return true;
		}

		auto path = m_paths.find(id);
		if (path == m_paths.end()) return false;
		T* resource = load(path->second);
		if (!resource) return false;
		m_resources.emplace(id, std::make_pair(resource, 1));
		return true;
	}

	bool releaseResource(const std::string& id)
	{
		auto r = retrieve(id);
		if (!r) return false;
		--r->second;
		if (!r->second)
			unload(id);
		return true;
	}

	bool forceReleaseResource(const std::string& id)
	{
		auto r = retrieve(id);
		if (!r) return false;
		unload(id);
		return true;
	}

	void purgeResources()
	{
		while (m_resources.begin() != m_resources.end())
		{
			delete m_resources.begin()->second.first;
			m_resources.erase(m_resources.begin());
		}
	}
protected:
	T* load(const std::string& id)
	{
		return static_cast<Derived*>(this)->load(id);
	}
private:
	bool unload(const std::string& id)
	{
		auto it = m_resources.find(id);
		if (it == m_resources.end()) return false;
		delete it->second.first;
		m_resources.erase(it);
		std::cout << "ResourceManager::unload() deleted resource: " << id << std::endl;
		return true;
	}

	std::pair<T*, unsigned int>* retrieve(const std::string& id)
	{
		auto it = m_resources.find(id);
		return (it != m_resources.end() ? &it->second : nullptr);
	}

	void loadPaths(const std::string& pathFile)
	{
		std::ifstream paths;
		std::string fullPath = Utils::getAssetsDirectory() + pathFile;
		paths.open(fullPath);
		if (!paths.is_open())
		{
			std::cerr << "Resource manager failed to open: " << fullPath << std::endl;
			return;
		}
		std::string line;
		while (std::getline(paths, line))
		{
			std::stringstream ss(line);
			std::string pathName;
			std::string path;
			ss >> pathName >> path;
			m_paths.emplace(pathName, path);
			std::cout << "Loaded path: " << pathName << " (" << path << ")" << std::endl;
		}
		paths.close();
		return;
	}
private:
	std::unordered_map<std::string, std::pair<T*, unsigned int>> m_resources;
	std::unordered_map<std::string, std::string> m_paths;
};
