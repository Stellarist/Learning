#pragma once

#include <vector>
#include <string_view>

#include <glm/glm.hpp>

using vertex_t = glm::vec3;
using edge_t = glm::uvec2;
using index_t = unsigned int;

template <>
struct std::hash<vertex_t> {
	std::size_t operator()(const vertex_t& v) const noexcept
	{
		return std::hash<float>()(v.x) ^ (std::hash<float>()(v.y) << 1) ^ (std::hash<float>()(v.z) << 2);
	}
};

template <>
struct std::hash<edge_t> {
	std::size_t operator()(const edge_t& e) const noexcept
	{
		return std::hash<unsigned int>()(e.x) ^ (std::hash<unsigned int>()(e.y) << 1);
	}
};

class Mesh {
public:
	static void loadObj(std::string_view file_path, std::vector<vertex_t>& vertices, std::vector<index_t>& indices);
};
