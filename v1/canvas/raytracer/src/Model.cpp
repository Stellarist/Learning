#define STB_IMAGE_IMPLEMENTATION
#define TINYOBJLOADER_IMPLEMENTATION

#include "Model.hpp"

#include <iostream>

Texture::Texture(const std::string& file_path, TextureType type)
{
	this->file_path = file_path;
	this->type = type;
	auto image = stbi_load(file_path.c_str(), &this->width, &this->height, &this->nrChannels, 0);
	stbi_set_flip_vertically_on_load(true);

	if (image == nullptr) {
		std::cerr << "Failed to load texture " << file_path << std::endl;
		exit(1);
	}

	data.resize(this->width * this->height);
	for (int i = 0; i < this->width * this->height; i++)
		data[i] = vec3f_t(
		    image[i * nrChannels + 0] / 255.0f,
		    image[i * nrChannels + 1] / 255.0f,
		    image[i * nrChannels + 2] / 255.0f);

	stbi_image_free(image);
}

vec3f_t Texture::sample(float u, float v) const
{
	int u_img = u * width;
	int v_img = v * height;
	int index = u_img * width + v_img;
	if (index < 0)
		index = 0;

	return data[index % data.size()];
}

Model::Model(const std::string& filepath, Material* mat)
{
	// get file directory and name
	size_t      file_pos = filepath.find_last_of('/');
	std::string file_dir = filepath.substr(0, file_pos + 1);
	std::string file_name = filepath.substr(file_pos + 1);
	default_material = mat;

	// load obj file
	tinyobj::ObjReader       reader;
	tinyobj::ObjReaderConfig reader_config;
	reader_config.triangulate = true;
	reader_config.mtl_search_path = file_dir;

	if (!reader.ParseFromFile(file_dir + file_name, reader_config)) {
		if (!reader.Error().empty()) {
			std::cerr << "TinyObjReader1: " << reader.Error() << std::endl;
		}
		exit(1);
	}
	if (!reader.Warning().empty()) {
		std::cerr << "TinyObjReader2: " << reader.Error() << std::endl;
	}

	// read properties
	const auto& attrib = reader.GetAttrib();
	const auto& shapes = reader.GetShapes();

	// convert materials
	for (const auto& material : reader.GetMaterials()) {
		materials.push_back(Material(
		    vec3f_t(material.diffuse[0], material.diffuse[1], material.diffuse[2]),
		    vec3f_t(material.specular[0], material.specular[1], material.specular[2]),
		    material.ior,
		    vec3f_t(material.emission[0], material.emission[1], material.emission[2]),
		    material.shininess > 0 ? material.shininess : 10.f));
	}

	// read textures
	for (auto& material : reader.GetMaterials()) {
		if (!material.diffuse_texname.empty())
			textures.emplace(material.diffuse_texname, Texture(file_dir + material.diffuse_texname, TextureType::DIFFUSE));

		if (!material.specular_texname.empty())
			textures.emplace(material.specular_texname, Texture(file_dir + material.specular_texname, TextureType::SPECULAR));

		if (!material.bump_texname.empty())
			textures.emplace(material.bump_texname, Texture(file_dir + material.bump_texname, TextureType::BUMP));
	}

	size_t total_triangles = 0;
	for (const auto& shape : shapes)
		total_triangles += shape.mesh.num_face_vertices.size();

	// read vertices and normals
	std::vector<Primitive*> primitives;
	primitives.reserve(total_triangles);
	for (const auto& shape : shapes) {
		const auto& mesh = shape.mesh;

		for (size_t f = 0; f < mesh.num_face_vertices.size(); f++) {
			Triangle triangle;

			size_t idx0 = mesh.indices[3 * f + 0].vertex_index;
			size_t idx1 = mesh.indices[3 * f + 1].vertex_index;
			size_t idx2 = mesh.indices[3 * f + 2].vertex_index;

			triangle.v0 = vec3f_t(attrib.vertices[3 * idx0 + 0], attrib.vertices[3 * idx0 + 1], attrib.vertices[3 * idx0 + 2]);
			triangle.v1 = vec3f_t(attrib.vertices[3 * idx1 + 0], attrib.vertices[3 * idx1 + 1], attrib.vertices[3 * idx1 + 2]);
			triangle.v2 = vec3f_t(attrib.vertices[3 * idx2 + 0], attrib.vertices[3 * idx2 + 1], attrib.vertices[3 * idx2 + 2]);

			if (!attrib.normals.empty()) {
				size_t nidx0 = mesh.indices[3 * f + 0].normal_index;
				size_t nidx1 = mesh.indices[3 * f + 1].normal_index;
				size_t nidx2 = mesh.indices[3 * f + 2].normal_index;

				if (nidx0 < attrib.normals.size() / 3) {
					triangle.n0 = vec3f_t(attrib.normals[3 * nidx0 + 0], attrib.normals[3 * nidx0 + 1], attrib.normals[3 * nidx0 + 2]);
					triangle.n1 = vec3f_t(attrib.normals[3 * nidx1 + 0], attrib.normals[3 * nidx1 + 1], attrib.normals[3 * nidx1 + 2]);
					triangle.n2 = vec3f_t(attrib.normals[3 * nidx2 + 0], attrib.normals[3 * nidx2 + 1], attrib.normals[3 * nidx2 + 2]);
				} else {
					vec3f_t face_normal = (triangle.v1 - triangle.v0).cross(triangle.v2 - triangle.v0).normalized();
					triangle.n0 = triangle.n1 = triangle.n2 = face_normal;
				}
			} else {
				vec3f_t face_normal = (triangle.v1 - triangle.v0).cross(triangle.v2 - triangle.v0).normalized();
				triangle.n0 = triangle.n1 = triangle.n2 = face_normal;
			}

			if (!attrib.texcoords.empty()) {
				size_t tidx0 = mesh.indices[3 * f + 0].texcoord_index;
				size_t tidx1 = mesh.indices[3 * f + 1].texcoord_index;
				size_t tidx2 = mesh.indices[3 * f + 2].texcoord_index;

				if (tidx0 < attrib.texcoords.size() / 2 && tidx0 != SIZE_MAX) {
					triangle.t0 = vec2f_t(attrib.texcoords[2 * tidx0 + 0], attrib.texcoords[2 * tidx0 + 1]);
					triangle.t1 = vec2f_t(attrib.texcoords[2 * tidx1 + 0], attrib.texcoords[2 * tidx1 + 1]);
					triangle.t2 = vec2f_t(attrib.texcoords[2 * tidx2 + 0], attrib.texcoords[2 * tidx2 + 1]);
				} else {
					triangle.t0 = vec2f_t(0.0f, 0.0f);
					triangle.t1 = vec2f_t(1.0f, 0.0f);
					triangle.t2 = vec2f_t(0.0f, 1.0f);
				}
			} else {
				triangle.t0 = vec2f_t(0.0f, 0.0f);
				triangle.t1 = vec2f_t(1.0f, 0.0f);
				triangle.t2 = vec2f_t(0.0f, 1.0f);
			}

			int mat_id = -1;
			if (f < mesh.material_ids.size() && mesh.material_ids[f] >= 0 && mesh.material_ids[f] < materials.size())
				mat_id = mesh.material_ids[f];

			if (mat_id >= 0 && mat_id < materials.size()) {
				triangle.material = &materials[mat_id];
				has_emission |= triangle.material->hasEmission();
			} else if (default_material) {
				triangle.material = default_material;
				has_emission |= default_material->hasEmission();
			}

			triangle.sarea = triangle.area();
			total_area += triangle.sarea;

			triangles.push_back(triangle);
		}
	}

	for (auto& triangle : triangles)
		primitives.push_back(&triangle);

	// compute bounding box
	bounding_box = triangles[0].bound();
	for (const auto& triangle : triangles)
		bounding_box = Bound::merge(bounding_box, triangle.bound());

	// build BVH
	bvh = new BVHAccel(primitives);
}

Model::~Model()
{
	delete bvh;
	bvh = nullptr;
}

Bound Model::bound() const
{
	return bounding_box;
}

float Model::area() const
{
	return total_area;
}

void Model::sample(Intersection& pos, float& pdf)
{
	if (bvh) {
		bvh->sample(pos, pdf);
		pos.emit = pos.material->emission;
	}
}

bool Model::intersect(const Ray& ray) const
{
	return true;
}

bool Model::intersect(const Ray& ray, float& tnear, uint32_t& index) const
{
	bool intersected = false;

	for (auto i = 0; i < triangles.size(); i++) {
		const auto& triangle = triangles[i];
		float       t, u, v;
		if (Triangle::intersect(triangle.v0, triangle.v1, triangle.v2, ray.origin, ray.direction, t, u, v)) {
			tnear = t;
			index = i;
			intersected |= true;
		}
	}

	return intersected;
}

Intersection Model::getIntersection(const Ray& ray)
{
	Intersection intersection;
	if (bvh)
		intersection = bvh->intersect(ray);

	return intersection;
}

bool Model::hasEmission() const
{
	return has_emission;
}

vec3f_t Model::evalDiffuse(const vec2f_t& tx) const
{
	float scale = 5;
	float pattern = (std::fmodf(tx.x() * scale, 1) > 0.5) ^ (std::fmodf(tx.y() * scale, 1) > 0.5);
	return Geometry::lerp(vec3f_t(0.815, 0.235, 0.031), vec3f_t(0.937, 0.937, 0.231), pattern);
}

void Model::getSurfaceProps(const vec3f_t& point, const vec3f_t& direction, uint32_t index, const vec2f_t& uv, vec3f_t& normal, vec2f_t& texcoords) const
{
	if (index < triangles.size())
		triangles[index].getSurfaceProps(point, direction, index, uv, normal, texcoords);
	else {
		normal = vec3f_t(0, 0, 1);
		texcoords = vec2f_t(0, 0);
	}
}
