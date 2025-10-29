#define TINYOBJLOADER_IMPLEMENTATION

#include "Mesh.hpp"

#include <unordered_map>

#include <tiny_obj_loader.h>

void Mesh::loadObj(std::string_view file_path, std::vector<vertex_t>& vertices, std::vector<index_t>& indices)
{
	tinyobj::ObjReaderConfig config;
	tinyobj::ObjReader       reader;

	if (!reader.ParseFromFile(file_path.data(), config)) {
		if (!reader.Error().empty())
			throw std::runtime_error(reader.Error());
		throw std::runtime_error("Failed to load OBJ file: " + std::string(file_path));
	}

	const auto& attrib = reader.GetAttrib();
	const auto& shapes = reader.GetShapes();

	std::unordered_map<vertex_t, unsigned int, std::hash<vertex_t>> vertex_map;

	for (const auto& shape : shapes) {
		size_t index_offset = 0;

		for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
			int fv = shape.mesh.num_face_vertices[f];

			for (size_t v = 0; v < fv; v++) {
				tinyobj::index_t idx = shape.mesh.indices[index_offset + v];

				vertex_t vertex(
				    attrib.vertices[3 * idx.vertex_index + 0],
				    attrib.vertices[3 * idx.vertex_index + 1],
				    attrib.vertices[3 * idx.vertex_index + 2]);

				if (vertex_map.count(vertex) > 0)
					indices.push_back(vertex_map[vertex]);
				else {
					vertex_map[vertex] = vertices.size();
					vertices.emplace_back(std::move(vertex));
					indices.push_back(vertex_map[vertex]);
				}
			}

			index_offset += fv;
		}
	}
}
