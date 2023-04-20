#include "custom_drawable.h"

#include "cgp/base/base.hpp"

namespace cgp
{
	GLuint custom_drawable::default_shader = 0;
	GLuint custom_drawable::default_texture = 0;


	custom_drawable::custom_drawable()
		:vbo(), vao(0), number_triangles(0), shader(0), texture(0), transform(), anisotropic_scale({ 1,1,1 }), shading(), name("uninitialized"), normal_texture(0), shader_type(SHADER_BASIC), model_matrix(mat4::identity()), translate(true)
	{}

	static void warning_initialize(std::string const previous_name, std::string const current_name)
	{
		std::string warning = "\n";
		warning += "  > The original custom_drawable is called [" + previous_name + "]\n";
		warning += "  > The current mesh_drawble is called [" + current_name + "]\n";
		warning += "In normal condition, you should avoid initializing mesh_drawble without clearing the previous one - the allocated memory on the GPU is going to be lost.\n";
		warning += " - If you want to clear the memory, please call custom_drawable.clear() before calling a new initialization\n";
		warning += " - If the vbo is shared between several instances of custom_drawable and you don't want to clear this memory, you can create a new default custom_drawable() before applying the initialization.\n";

		warning_cgp("Calling custom_drawable.initialize() on a structure custom_drawable with non zero VBOs", warning);
	}

	custom_drawable& custom_drawable::initialize(mesh const& data_to_send, std::string const& object_name, GLuint shader_arg, GLuint texture_arg)
	{
		// Error detection before sending the data to avoid unexpected behavior
		// *********************************************************************** //

		opengl_check;

		// Check if this custom_drawable is already initialized
		if (vbo.size() != 0 || vao != 0)
			warning_initialize(name, object_name);

		if (data_to_send.position.size() == 0) {
			warning_cgp("Warning try to generate custom_drawable [" + name + "] with 0 vertex", "");
			return *this;
		}

		// Sanity check before sending mesh data to GPU
		assert_cgp(mesh_check(data_to_send), "Cannot send this mesh data to GPU in initializing custom_drawable with name [" + name + "]");


		// Variable initialization
		// *********************************************************************** //

		name = object_name;
		shader = shader_arg;
		texture = texture_arg;
		transform = affine_rts();
		shading = shading_parameters_phong();

		// Send the data to the GPU
		// ******************************************** //

		// Fill VBOs
		if (shader_type != SHADER_BLUR) {
			opengl_create_gl_buffer_data(GL_ARRAY_BUFFER, vbo["position"], data_to_send.position, GL_DYNAMIC_DRAW);
			opengl_create_gl_buffer_data(GL_ARRAY_BUFFER, vbo["normal"], data_to_send.normal, GL_DYNAMIC_DRAW);
			opengl_create_gl_buffer_data(GL_ARRAY_BUFFER, vbo["color"], data_to_send.color, GL_DYNAMIC_DRAW);
			opengl_create_gl_buffer_data(GL_ARRAY_BUFFER, vbo["uv"], data_to_send.uv, GL_DYNAMIC_DRAW);
			opengl_create_gl_buffer_data(GL_ELEMENT_ARRAY_BUFFER, vbo["index"], data_to_send.connectivity, GL_DYNAMIC_DRAW);
		}
		else {

			opengl_create_gl_buffer_data(GL_ARRAY_BUFFER, vbo["position"], data_to_send.position, GL_DYNAMIC_DRAW);
			opengl_create_gl_buffer_data(GL_ARRAY_BUFFER, vbo["uv"], data_to_send.uv, GL_DYNAMIC_DRAW);
			opengl_create_gl_buffer_data(GL_ELEMENT_ARRAY_BUFFER, vbo["index"], data_to_send.connectivity, GL_DYNAMIC_DRAW);
		}

		// Instance buffers
		if (shader_type & SHADER_INSTANCES) {
			std::cout << sizeof(instance_models) / sizeof(float) << std::endl;
			opengl_create_gl_buffer_data(GL_ARRAY_BUFFER, instance_models_buffer, instance_models, GL_DYNAMIC_DRAW);
		}

		if (shader_type & SHADER_TESSELATION) {

			// Compute tangents and bitangents
			// ******************************************** //

			buffer<vec3> tangents(data_to_send.position.size());
			buffer<vec3> bitangents(data_to_send.position.size());

			for (int i = 0; i < data_to_send.connectivity.size(); i++) {
				vec3 p1 = data_to_send.position[data_to_send.connectivity[i][0]];
				vec3 p2 = data_to_send.position[data_to_send.connectivity[i][1]];
				vec3 p3 = data_to_send.position[data_to_send.connectivity[i][2]];

				vec2 uv1 = data_to_send.uv[data_to_send.connectivity[i][0]];
				vec2 uv2 = data_to_send.uv[data_to_send.connectivity[i][1]];
				vec2 uv3 = data_to_send.uv[data_to_send.connectivity[i][2]];

				vec3 tangent, bitangent;

				vec3 deltaPos1 = p1 - p2;
				vec3 deltaPos2 = p2 - p3;

				vec2 deltaUV1 = uv1 - uv2;
				vec2 deltaUV2 = uv2 - uv3;

				tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
				bitangent = (deltaPos1 * deltaUV2.x - deltaPos2 * deltaUV1.x) / (deltaUV1.y * deltaUV2.x - deltaUV2.y * deltaUV1.x);

				vec3 n1 = data_to_send.normal[data_to_send.connectivity[i][0]];
				vec3 n2 = data_to_send.normal[data_to_send.connectivity[i][1]];
				vec3 n3 = data_to_send.normal[data_to_send.connectivity[i][2]];

				vec3 t1 = normalize(tangent - dot(tangent, n1) * n1);
				vec3 t2 = normalize(tangent - dot(tangent, n2) * n2);
				vec3 t3 = normalize(tangent - dot(tangent, n3) * n3);

				vec3 b1 = normalize(bitangent - dot(tangent, n1) * n1);
				vec3 b2 = normalize(bitangent - dot(tangent, n2) * n2);
				vec3 b3 = normalize(bitangent - dot(tangent, n3) * n3);

				tangents[data_to_send.connectivity[i][0]] = t1;
				tangents[data_to_send.connectivity[i][1]] = t2;
				tangents[data_to_send.connectivity[i][2]] = t3;

				bitangents[data_to_send.connectivity[i][0]] = b1;
				bitangents[data_to_send.connectivity[i][1]] = b2;
				bitangents[data_to_send.connectivity[i][2]] = b3;
			}

			opengl_create_gl_buffer_data(GL_ARRAY_BUFFER, vbo["tangent"], tangents, GL_DYNAMIC_DRAW);
			opengl_create_gl_buffer_data(GL_ARRAY_BUFFER, vbo["bitangent"], bitangents, GL_DYNAMIC_DRAW);
		}

		// Store number of triangles
		number_triangles = static_cast<GLuint>(data_to_send.connectivity.size());

		// Generate VAO
		glGenVertexArrays(1, &vao); opengl_check;
		glBindVertexArray(vao);    opengl_check;

		if (shader_type != SHADER_BLUR) {
			opengl_set_vertex_attribute(vbo["position"], 0, 3, GL_FLOAT);
			opengl_set_vertex_attribute(vbo["normal"], 1, 3, GL_FLOAT);
			opengl_set_vertex_attribute(vbo["color"], 2, 3, GL_FLOAT);
			opengl_set_vertex_attribute(vbo["uv"], 3, 2, GL_FLOAT);
		}
		else {
			opengl_set_vertex_attribute(vbo["position"], 0, 3, GL_FLOAT);
			opengl_set_vertex_attribute(vbo["uv"], 1, 2, GL_FLOAT);
		}
		
		if (shader_type & SHADER_INSTANCES) {
			GLuint pos = 6;
			GLuint pos1 = pos + 0;
			GLuint pos2 = pos + 1;
			GLuint pos3 = pos + 2;
			GLuint pos4 = pos + 3;

			glEnableVertexAttribArray(pos1);
			glEnableVertexAttribArray(pos2);
			glEnableVertexAttribArray(pos3);
			glEnableVertexAttribArray(pos4);

			glBindBuffer(GL_ARRAY_BUFFER, instance_models_buffer);

			glVertexAttribPointer(pos1, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(0));
			glVertexAttribPointer(pos2, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 4));
			glVertexAttribPointer(pos3, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 8));
			glVertexAttribPointer(pos4, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 12));

			glVertexAttribDivisor(pos1, 1);
			glVertexAttribDivisor(pos2, 1);
			glVertexAttribDivisor(pos3, 1);
			glVertexAttribDivisor(pos4, 1);
		}

		if (shader_type == SHADER_TESSELATION) {
			opengl_set_vertex_attribute(vbo["tangent"], 4, 3, GL_FLOAT);
			opengl_set_vertex_attribute(vbo["bitangent"], 5, 3, GL_FLOAT);
		}
		
		glBindVertexArray(0);      opengl_check;

		return *this;
	}


	custom_drawable& custom_drawable::update_position(buffer<vec3> const& new_position)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbo["position"]); opengl_check;
		glBufferSubData(GL_ARRAY_BUFFER, 0, size_in_memory(new_position), ptr(new_position));  opengl_check;
		return *this;
	}
	custom_drawable& custom_drawable::update_normal(buffer<vec3> const& new_normals)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbo["normal"]); opengl_check;
		glBufferSubData(GL_ARRAY_BUFFER, 0, size_in_memory(new_normals), ptr(new_normals));  opengl_check;
		return *this;
	}
	custom_drawable& custom_drawable::update_color(buffer<vec3> const& new_color)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbo["color"]); opengl_check;
		glBufferSubData(GL_ARRAY_BUFFER, 0, size_in_memory(new_color), ptr(new_color));  opengl_check;
		return *this;
	}
	custom_drawable& custom_drawable::update_uv(buffer<vec2> const& new_uv)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbo["uv"]); opengl_check;
		glBufferSubData(GL_ARRAY_BUFFER, 0, size_in_memory(new_uv), ptr(new_uv));  opengl_check;
		return *this;
	}
	custom_drawable& custom_drawable::update_tangent(buffer<vec2> const& new_tangent)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbo["tangent"]); opengl_check;
		glBufferSubData(GL_ARRAY_BUFFER, 0, size_in_memory(new_tangent), ptr(new_tangent));  opengl_check;
		return *this;
	}
	custom_drawable& custom_drawable::update_bitangent(buffer<vec2> const& new_bitangent)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbo["uv"]); opengl_check;
		glBufferSubData(GL_ARRAY_BUFFER, 0, size_in_memory(new_bitangent), ptr(new_bitangent));  opengl_check;
		return *this;
	}

	custom_drawable& custom_drawable::clear()
	{
		for (auto& buffer : vbo)
			glDeleteBuffers(1, &(buffer.second));
		vbo.clear();

		glDeleteVertexArrays(1, &vao);
		vao = 0;
		opengl_check;

		number_triangles = 0;
		shader = 0;
		texture = 0;
		normal_texture = 0;
		transform = affine_rts();
		anisotropic_scale = { 1,1,1 };
		shading = shading_parameters_phong();
		name = "uninitialized";

		return *this;
	}

	const int custom_drawable::instance_count() const {
		return instance_models.size();
	}
}