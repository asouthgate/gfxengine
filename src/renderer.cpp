#include <map>
#include <algorithm>
#include <vector>
#include <iostream>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>
#include "renderer.hpp"
#include "entity.hpp"
#include "light.hpp"
#include "mesh.hpp"
#include "texture.hpp"
#include "shader.hpp"

//===================================================================
//***** Binding functions for sending data
void Renderer::bind_vertices(Mesh* mesh_ptr) {
        glBindBuffer(GL_ARRAY_BUFFER, mesh_ptr->get_VB());
        glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE, 0, (void*)0);
}
void Renderer::bind_UVs(Mesh* mesh_ptr) {
        glBindBuffer(GL_ARRAY_BUFFER, mesh_ptr->get_UVB());
        glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,0,(void*)0);
}
void Renderer::bind_normals(Mesh* mesh_ptr) {
        glBindBuffer(GL_ARRAY_BUFFER, mesh_ptr->get_NB());
        glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,0,(void*)0);
}
void Renderer::bind_elements(Mesh* mesh_ptr) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_ptr->get_EB());
}
//===================================================================

//===================================================================
//***** Individual pipeline drawing functions
void Renderer::draw_spotlight_map(std::list<Entity>* entity_list_ptr, std::vector<Spotlight>* spotlights_ptr) {
	//***** Draw spotlights
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	depth_shad.clear_uniforms();
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	for (auto& e : *entity_list_ptr) {
		if (camera->on_screen()) {
			for (auto& spotlight : *spotlights_ptr) {
                                //***** Draw to depth texture
                                glCullFace(GL_FRONT);
				glm::mat4 light_PV = spotlight.get_PV();
				glDrawBuffer(depth_color_attachment);
				glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				depth_shad.activate();
				depth_shad.load_uniforms();
				depth_shad.clear_uniforms();
				for (auto& e2 : *entity_list_ptr) {
						depth_shad.mat4_parameters[depth_shad.lPVM] = light_PV*e2.modM;
						depth_shad.load_uniforms();
						bind_vertices(e2.mesh_ptr);
						bind_elements(e2.mesh_ptr);
						glDrawElements(GL_TRIANGLES, e2.mesh_ptr->get_n_indices(), GL_UNSIGNED_SHORT, (void*)0);
				}

                                //***** Accumulate depth textures with shadow accumulator
                                glCullFace(GL_BACK);
				glDrawBuffer(ping_color_attachment);
				glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//				glClear(GL_COLOR_BUFFER_BIT);
				shacc_shad.activate();
				shacc_shad.clear_uniforms();	
				shacc_shad.vec3_parameters[shacc_shad.light_position] = spotlight.direction;
				shacc_shad.vec3_parameters[shacc_shad.camera_position] = camera->position;
				shacc_shad.vec3_parameters[shacc_shad.light_colour_position] = spotlight.colour;
				shacc_shad.texture_parameters[shacc_shad.g_position_map] = g_buffer_textures[0];
				shacc_shad.texture_parameters[shacc_shad.g_normal_map] = g_buffer_textures[1];
				shacc_shad.texture_parameters[shacc_shad.g_albedo_map] = g_buffer_textures[2];
				shacc_shad.texture_parameters[shacc_shad.depth_map_position] = depth_texture;
				shacc_shad.float_parameters[shacc_shad.window_width] = WIN_WIDTH;
				shacc_shad.float_parameters[shacc_shad.window_height] = WIN_HEIGHT;
				shacc_shad.load_uniforms();
				shacc_shad.clear_uniforms();
				for (auto& e2 : *entity_list_ptr) {
					if (camera->on_screen()) {
						shacc_shad.mat4_parameters[shacc_shad.lPVM] = light_PV*e2.modM;
						shacc_shad.mat4_parameters[shacc_shad.PVM] = camera->get_PV_matrix()*e2.modM;
						shacc_shad.load_uniforms();
						bind_vertices(e2.mesh_ptr);
						bind_elements(e2.mesh_ptr);
						glDrawElements(GL_TRIANGLES, e2.mesh_ptr->get_n_indices(), GL_UNSIGNED_SHORT, (void*)0);
					}
				}	
				glDisable(GL_BLEND);
			}
		}
	}
}	

void Renderer::geometry_pass(std::list<Entity>* entity_list_ptr) {
        //***** Geometry pass for drawing fundamental object data to the G buffer
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glDrawBuffers(3, g_color_attachments);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	g_shad.activate();
	for (auto& e2 : *entity_list_ptr) {
		if (camera->on_screen()) {
			g_shad.mat4_parameters[g_shad.modMID] = e2.modM;
			g_shad.mat4_parameters[g_shad.PVM] = camera->get_PV_matrix()*e2.modM;
			g_shad.mat4_parameters[g_shad.inverse_modM_position] = e2.inverse_modM;
			g_shad.texture_parameters[g_shad.material_diffuse_position] = e2.diffuse_ptr->textureID;
			g_shad.texture_parameters[g_shad.material_texture_position] = e2.texture_ptr->textureID;
			g_shad.load_uniforms();
			bind_vertices(e2.mesh_ptr);
			bind_UVs(e2.mesh_ptr);
			bind_normals(e2.mesh_ptr);
			bind_elements(e2.mesh_ptr);
			glDrawElements(GL_TRIANGLES, e2.mesh_ptr->get_n_indices(), GL_UNSIGNED_SHORT, (void*)0);	
		}
	}
}

void Renderer::draw_preprocessor(std::list<Entity>* entity_list_ptr) {
        //***** Preprocessor step; required for bloom (for now removed), but also used for pointlights.
        //***** In future, this function may be removed entirely if bloom is never necessary
	glDrawBuffers(2, preprocessor_color_attachments);
	std::vector<glm::vec3> pointlight_positions;
	std::vector<glm::vec4> pointlight_colours;
	int c = 0;
	for (auto & e : *entity_list_ptr) {
		if (c < 6 && e.pointlights.size() > 0) {
			for (Pointlight p : e.pointlights) {
				pointlight_positions.push_back(e.pvec);
				pointlight_colours.push_back(glm::vec4(p.colour, p.strength));
			}
		}
	}
	preprocessor_shad.float_parameters[preprocessor_shad.n_pointlights] = pointlight_positions.size();
	preprocessor_shad.vec3_array_parameters[preprocessor_shad.pointlight_positions] = pointlight_positions;
	preprocessor_shad.vec4_array_parameters[preprocessor_shad.pointlight_colours] = pointlight_colours;
	preprocessor_shad.texture_parameters[preprocessor_shad.spotlight_map] = ping_FBO_texture;
	preprocessor_shad.texture_parameters[preprocessor_shad.g_position_map] = g_buffer_textures[0];
	preprocessor_shad.texture_parameters[preprocessor_shad.g_normal_map] = g_buffer_textures[1];
	preprocessor_shad.texture_parameters[preprocessor_shad.g_albedo_map] = g_buffer_textures[2];
	preprocessor_shad.vec3_parameters[preprocessor_shad.camera_position] = camera->position;
	preprocessor_shad.texture_parameters[preprocessor_shad.backdrop] = backdrop_texture.textureID;
	preprocessor_shad.activate();
	preprocessor_shad.load_uniforms();
	preprocessor_shad.clear_uniforms();
	bind_vertices(&backdrop);
	bind_UVs(&backdrop);
	bind_elements(&backdrop);
	glDrawElements(GL_TRIANGLES, backdrop.get_n_indices(), GL_UNSIGNED_SHORT, (void*)0);
}

void Renderer::draw_fxaa() {
        //**** Use fast approximate anti aliasing
	fxaa_shad.texture_parameters[fxaa_shad.image] = ping_FBO_texture;
//	fxaa_shad.texture_parameters[fxaa_shad.image] = depth_texture;

	fxaa_shad.int_parameters[fxaa_shad.window_width] = WIN_WIDTH;
	fxaa_shad.int_parameters[fxaa_shad.window_height] = WIN_HEIGHT;
	fxaa_shad.activate();
	fxaa_shad.load_uniforms();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	bind_vertices(&backdrop);
	bind_UVs(&backdrop);
	bind_elements(&backdrop);
	glDrawElements(GL_TRIANGLES, backdrop.get_n_indices(), GL_UNSIGNED_SHORT, (void*)0);
}
//===================================================================

void Renderer::render_scene(std::list<Entity>* entity_list_ptr, std::vector<Spotlight>* spotlights_ptr) {
        //***** Render scene call: carries out graphics pipeline
	for (int i = 0; i < 1; i++) {
	        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_BLEND);	
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_DEPTH_BUFFER_BIT);
		geometry_pass(entity_list_ptr);
		glClear(GL_DEPTH_BUFFER_BIT);
		draw_spotlight_map(entity_list_ptr, spotlights_ptr);
//		draw_preprocessor(entity_list_ptr);
		glDepthMask(GL_TRUE);
		draw_fxaa();
	}
}

Renderer::Renderer(int WIN_WIDTH_, int WIN_HEIGHT_) : WIN_WIDTH(WIN_WIDTH_), WIN_HEIGHT(WIN_HEIGHT_) {
	//***** Renderer initialization
	glEnable(GL_MULTISAMPLE);  
	glewExperimental = GL_TRUE;
	glewInit();
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glGenVertexArrays(1, &VAid);
        glBindVertexArray(VAid);

	//***** Fetch shaders
	fxaa_shad = Shader("../res/shad/fxaa.vshad", "../res/shad/fxaa.fshad");
	depth_shad = Shader("../res/shad/depth.vshad", "../res/shad/depth.fshad");
	shacc_shad = Shader("../res/shad/shadow_accumulator.vshad", "../res/shad/shadow_accumulator.fshad");
	preprocessor_shad = Shader("../res/shad/preprocessor.vshad", "../res/shad/preprocessor.fshad");
	g_shad = Shader("../res/shad/g.vshad", "../res/shad/g.fshad");

	//***** Enable Client State
	glEnableClientState(GL_TEXTURE_COORD_ARRAY_EXT); 

	//***** Initialize the G Buffer
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glGenTextures(3, g_buffer_textures);
		  
	//***** G buffer position component initialization
	glBindTexture(GL_TEXTURE_2D, g_buffer_textures[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WIN_WIDTH, WIN_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, g_color_attachments[0], GL_TEXTURE_2D, g_buffer_textures[0], 0);
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "GL G POS STATUS ERROR: " << std::endl;
		std::cout << "\t" << glGetError() << std::endl;
	}
	  
	//***** G buffer normal component initialization
	glBindTexture(GL_TEXTURE_2D, g_buffer_textures[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WIN_WIDTH, WIN_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, g_color_attachments[1], GL_TEXTURE_2D, g_buffer_textures[1], 0);
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "GL G NORM STATUS ERROR: " << std::endl;
		std::cout << "\t" << glGetError() << std::endl;
	}
	  
	//***** G buffer position component initialization
	glBindTexture(GL_TEXTURE_2D, g_buffer_textures[2]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, WIN_WIDTH, WIN_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, g_color_attachments[2], GL_TEXTURE_2D, g_buffer_textures[2], 0);
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "GL G COL STATUS ERROR: " << std::endl;
		std::cout << "\t" << glGetError() << std::endl;
	}

	//***** Depth buffer initialization
        glGenTextures(1, &depth_texture);
        glBindTexture(GL_TEXTURE_2D, depth_texture);
//        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, WIN_WIDTH, WIN_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, WIN_WIDTH, WIN_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);

        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        GLfloat borderColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
//        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_texture, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, depth_color_attachment, GL_TEXTURE_2D, depth_texture, 0);
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                std::cout << "GL DEPTH STATUS ERROR: " << std::endl;
                std::cout << "\t" << glGetError() << std::endl;
        }

	//***** G-RBO init
	glGenRenderbuffers(1, &g_RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, g_RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WIN_WIDTH, WIN_HEIGHT);	
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, g_RBO);		  
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "GL G RBO STATUS ERROR: " << std::endl;
		std::cout << "\t" << glGetError() << std::endl;
	}

	//***** Ping init for multi depth
	glGenTextures(1, &ping_FBO_texture);
	glBindTexture(GL_TEXTURE_2D, ping_FBO_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WIN_WIDTH, WIN_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); 
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);  
	glFramebufferTexture2D(GL_FRAMEBUFFER, ping_color_attachment, GL_TEXTURE_2D, ping_FBO_texture, 0);	
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "GL PING STATUS ERROR: " << std::endl;
		std::cout << "\t" << glGetError() << std::endl;
	}

	//***** Pre-processor FBO Init
	glGenTextures(2, preprocessor_textures);
	for (GLuint i = 0; i < 2; i++){
		glBindTexture(GL_TEXTURE_2D, preprocessor_textures[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WIN_WIDTH, WIN_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	    	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	    	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6+i, GL_TEXTURE_2D, preprocessor_textures[i], 0);
	}
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "GL PREPROCESSOR STATUS ERROR: " << std::endl;
		std::cout << "\t" << glGetError() << std::endl;
	}

	//***** Enable vertex attrib arrays
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
}
