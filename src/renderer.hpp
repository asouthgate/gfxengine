#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <list>
#include "shader.hpp"
#include "light.hpp"
#include "entity.hpp"
#include "camera.hpp"
#include "mesh.hpp"
#include "texture.hpp"

class Renderer {
	public:
		Renderer(int WIN_WIDTH_, int WIN_HEIGHT_);
		void render_scene(std::list<Entity>* entity_list_tr, std::vector<Spotlight>* spotlight_list_pr);
		void draw_preprocessor(std::list<Entity>* entity_list_ptr);
		void geometry_pass(std::list<Entity>* entity_list_ptr);
		void draw_spotlight_map(std::list<Entity>* entity_list_ptr, std::vector<Spotlight>* spotlight_list_pr);
		void draw_fxaa();

		//------TEMPORARY POINTER
		Camera* camera;			
		
	private:
		void bind_vertices(Mesh* mesh_ptr);
		void bind_UVs(Mesh* mesh_ptr);
		void bind_normals(Mesh* mesh_ptr);
		void bind_elements(Mesh* mesh_ptr);
		
		//-----SHADER OBJECTS
		Shader depth_shad;
		Shader shacc_shad;
		Shader preprocessor_shad;
		Shader fxaa_shad;
		Shader g_shad;

		//-----FBOs
		GLuint FBO;
		GLuint depth_FBO;

		//-----FOR PING PONGING
		GLuint depth_texture, ping_FBO_texture;
		GLuint ping_color_attachment = GL_COLOR_ATTACHMENT4;

		//-----FOR DEPTH
		GLuint depth_color_attachment = GL_COLOR_ATTACHMENT3;

		//------FOR PREPROCESSING BLOOM 
		GLuint preprocessor_textures[2];
		GLenum preprocessor_color_attachments[2] = {GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7};

		//------FOR FXAA
		GLuint postprocessor_texture;
		GLuint postprocessor_color_attachment = GL_COLOR_ATTACHMENT5;
		GLuint fxaa_texture;
	
		//------FOR THE G BUFFER
		GLuint g_RBO;
		GLuint g_buffer_textures[3];
		GLenum g_color_attachments[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
		
		//------BACKDROP FOR ON-SCREEN QUAD
		Mesh backdrop = Mesh("../res/meshes/plane.obj");
		Texture backdrop_texture = Texture("../res/textures/black.DDS");
		Texture backdrop_diffuse = Texture("../res/textures/jshine.DDS");	
		//-----??
		int WIN_WIDTH;
		int WIN_HEIGHT;
		GLuint VAid;
};
	

#endif
