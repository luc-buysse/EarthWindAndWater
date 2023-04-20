#pragma once

#include <map>
#include <string>
#include "cgp/display/opengl/opengl.hpp"
#include "cgp/shape/mesh/mesh.hpp"
#include "cgp/display/drawable/shading_parameters/shading_parameters.hpp"

namespace cgp
{
	enum SHADER_TYPE {
		SHADER_BASIC = 0b0000,
		SHADER_TESSELATION = 0b0001,
		SHADER_INSTANCES = 0b0010,
		SHADER_BLUR = 0b0100
	};

	struct custom_drawable
	{
		custom_drawable();
		custom_drawable& initialize(mesh const& data_to_send, std::string const& object_name = "unset_name", GLuint shader = default_shader, GLuint texture = default_texture);
		custom_drawable& clear();

		custom_drawable& update_position(buffer<vec3> const& new_position);
		custom_drawable& update_normal(buffer<vec3> const& new_normal);
		custom_drawable& update_color(buffer<vec3> const& new_color);
		custom_drawable& update_uv(buffer<vec2> const& new_uv);
		custom_drawable& update_tangent(buffer<vec2> const& new_tangent);
		custom_drawable& update_bitangent(buffer<vec2> const& new_bitangent);

		// Stores VBO ID in GPU_elements_id
		std::map<std::string, GLuint> vbo;
		GLuint vao;

		GLuint number_triangles;
		GLuint shader;
		GLuint texture;

		// Mirror
		GLuint displacement_texture;

		// Tesselation
		GLuint normal_texture;
		GLuint roughness_texture;
		GLuint ambient_occlusion_texture;
		GLuint height_texture;

		int shader_type;

		// Shading
		shading_parameters_phong shading;
		bool translate;

		// Basic
		affine_rts transform;
		vec3 anisotropic_scale;
		mat4 model_matrix;

		// Instances
		buffer<mat4> instance_models;
		const int instance_count() const;
		GLuint instance_models_buffer;

		std::string name;

		static GLuint default_shader;
		static GLuint default_texture;
	};

	template <typename SCENE_ENVIRONMENT>
	void draw(custom_drawable const& drawable, SCENE_ENVIRONMENT const& environment);

	template <typename SCENE_ENVIRONMENT>
	void draw_wireframe(custom_drawable const& drawable, SCENE_ENVIRONMENT const& environment, vec3 const& color = { 0,0,1 });
}


namespace cgp
{
	template <typename SCENE_ENVIRONMENT>
	void draw(custom_drawable const& drawable, SCENE_ENVIRONMENT const& environment)
	{
		if (drawable.number_triangles == 0) return;

		// Setup shader
		assert_cgp(drawable.shader != 0, "Try to draw custom_drawable without shader [name:" + drawable.name + "]");
		assert_cgp(drawable.texture != 0, "Try to draw custom_drawable without texture [name:" + drawable.name + "]");
		glUseProgram(drawable.shader); opengl_check;

		// Send uniforms for this shader
		if (drawable.shader_type != SHADER_BLUR) {
			opengl_uniform(drawable.shader, environment);
			opengl_uniform(drawable.shader, drawable.shading);
			opengl_uniform(drawable.shader, "translate", drawable.translate, false);
		}

		if(drawable.shader_type ^ SHADER_INSTANCES) {
			opengl_uniform(drawable.shader, "model", drawable.model_matrix);
		}

		// Set texture

		if (drawable.shader_type == SHADER_BLUR || drawable.shader_type == SHADER_BASIC || drawable.shader_type & SHADER_INSTANCES) {
			glActiveTexture(GL_TEXTURE0); opengl_check;
			glBindTexture(GL_TEXTURE_2D, drawable.texture); opengl_check;
			opengl_uniform(drawable.shader, "image_texture", 0);  opengl_check;
		}

		if (drawable.shader_type == SHADER_BLUR) {
			assert_cgp(drawable.displacement_texture != 0, "Try to draw custom_drawable without shader [name:" + drawable.name + "]");

			glActiveTexture(GL_TEXTURE1); opengl_check;
			glBindTexture(GL_TEXTURE_2D, drawable.displacement_texture); opengl_check;
			opengl_uniform(drawable.shader, "displacement_texture", 1);  opengl_check;
		}

		// Initialize all the textures needed by the tesselation shader
		if (drawable.shader_type & SHADER_TESSELATION) {

			// Set texture
			glActiveTexture(GL_TEXTURE0); opengl_check;
			glBindTexture(GL_TEXTURE_2D, drawable.texture); opengl_check;
			opengl_uniform(drawable.shader, "image_texture", 0);  opengl_check;

			// Set normal texture
			if (drawable.normal_texture != NULL) {
				glActiveTexture(GL_TEXTURE1); opengl_check;
				glBindTexture(GL_TEXTURE_2D, drawable.normal_texture); opengl_check;
				opengl_uniform(drawable.shader, "normal_texture", 1);  opengl_check;
			}

			// Set roughness texture
			if (drawable.roughness_texture != NULL) {
				glActiveTexture(GL_TEXTURE2); opengl_check;
				glBindTexture(GL_TEXTURE_2D, drawable.roughness_texture); opengl_check;
				opengl_uniform(drawable.shader, "roughness_texture", 2);  opengl_check;
			}

			// Set ambient occlusion texture
			if (drawable.ambient_occlusion_texture != NULL) {
				glActiveTexture(GL_TEXTURE3); opengl_check;
				glBindTexture(GL_TEXTURE_2D, drawable.ambient_occlusion_texture); opengl_check;
				opengl_uniform(drawable.shader, "ambient_occlusion_texture", 3);  opengl_check;
			}

			// Set height texture
			if (drawable.height_texture != NULL) {
				glActiveTexture(GL_TEXTURE4); opengl_check;
				glBindTexture(GL_TEXTURE_2D, drawable.height_texture); opengl_check;
				opengl_uniform(drawable.shader, "height_texture", 4);  opengl_check;
			}
		}

		// Call draw function
		assert_cgp(drawable.number_triangles > 0, "Try to draw custom_drawable with 0 triangles [name:" + drawable.name + "]"); opengl_check;
		glBindVertexArray(drawable.vao);   opengl_check;
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawable.vbo.at("index")); opengl_check;

		if (drawable.shader_type == SHADER_BLUR) {
			glDrawElements(GL_TRIANGLES, GLsizei(drawable.number_triangles * 3), GL_UNSIGNED_INT, nullptr);
		}
		else if (drawable.shader_type == SHADER_INSTANCES) {
			glDrawElementsInstanced(GL_TRIANGLES, GLsizei(drawable.number_triangles * 3), GL_UNSIGNED_INT, nullptr, drawable.instance_count()); opengl_check;
		}
		else if(drawable.shader_type == SHADER_TESSELATION) {
			glPatchParameteri(GL_PATCH_VERTICES, 3); opengl_check;
			glDrawElements(GL_PATCHES, GLsizei(drawable.number_triangles * 3), GL_UNSIGNED_INT, nullptr); opengl_check;
		}
		else if (drawable.shader_type & SHADER_TESSELATION & SHADER_INSTANCES) {
			glPatchParameteri(GL_PATCH_VERTICES, 3); opengl_check;
			glDrawElementsInstanced(GL_PATCHES, GLsizei(drawable.number_triangles * 3), GL_UNSIGNED_INT, nullptr, drawable.instance_count()); opengl_check;
		}
		else {
			glDrawElements(GL_TRIANGLES, GLsizei(drawable.number_triangles * 3), GL_UNSIGNED_INT, nullptr);
		}

		// Clean buffers
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}


	template <typename SCENE_ENVIRONMENT>
	void draw_wireframe(custom_drawable const& drawable, SCENE_ENVIRONMENT const& scene, vec3 const& color)
	{
		custom_drawable wireframe = drawable;
		wireframe.shading.phong = { 1.0f,0.0f,0.0f,64.0f };
		wireframe.shading.color = color;
		wireframe.shading.use_texture = false;
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glEnable(GL_POLYGON_OFFSET_LINE);
		glPolygonOffset(-1.0, 1.0);
		draw(wireframe, scene);
		glDisable(GL_POLYGON_OFFSET_LINE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}