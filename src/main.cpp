#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "renderer.hpp"
#include "window.hpp"
#include "camera.hpp"
#include "shader.hpp"
#include "entity.hpp"
#include "light.hpp"

//***** Misc init
const unsigned int WIN_WIDTH = 1280;
const unsigned int WIN_HEIGHT = 720;

//***** Timing init
float dT = 0.0f;
float last_frame = 0.0f;

//***** Camera init
Camera camera(WIN_WIDTH, WIN_HEIGHT, glm::vec3(0.0f, 0.0f, 0.3f));

//***** Window 
Window window(WIN_WIDTH, WIN_HEIGHT, &camera);

int main() {
        //***** Init glew
        glewExperimental = GL_TRUE;
        glewInit();
        //***** Configure global opengl state
        glEnable(GL_DEPTH_TEST);
	//****** Init Renderer
	Renderer renderer(WIN_WIDTH, WIN_HEIGHT);
	renderer.camera = &camera;

//===================================================================
//TEMPORARY OBJECTS: IN FUTURE TO BE LOADED IN BY XML OR SIMILAR DATA
	//***** Build a cube mesh
	Mesh bottle_mesh = Mesh("../res/meshes/cube.obj");
	Mesh cube_mesh = Mesh("../res/meshes/cube.obj");
	//***** Build a texture
	Texture mars_tex = Texture("../res/textures/cloudy_earth.DDS");
	Texture white_tex = Texture("../res/textures/white.DDS");
	Texture orange_tex = Texture("../res/textures/io.DDS");
	Texture black_tex = Texture("../res/textures/brown.DDS");
	//***** Build a bottle
	Entity bottle_entity;
	bottle_entity.pvec = {0.0f, 1.0f, 0.0f};
        bottle_entity.scalevec = {1.5f, 0.5f, 0.5f};
	bottle_entity.angle = 0.1f;
	bottle_entity.build_matrices();
	bottle_entity.mesh_ptr = &bottle_mesh;
	bottle_entity.texture_ptr = &white_tex;
	bottle_entity.diffuse_ptr = &white_tex;
	//***** Build a cube
	Entity cube_entity;
	cube_entity.pvec = {0.0f, -0.80f, 0.0f};
	cube_entity.scalevec = {0.5f, 0.05f, 0.5f};
	cube_entity.build_matrices();
	cube_entity.mesh_ptr = &cube_mesh;
	cube_entity.texture_ptr = &white_tex;
	cube_entity.diffuse_ptr = &black_tex;
	//***** Build a spotlight
        Spotlight spotlight(glm::vec3(1.5f, 3.5f, 0.5f),
                        bottle_entity.pvec+glm::vec3(0.0f, 0.0f, 0.0f),
                        glm::vec3(1.0f, 1.0f, 1.0f),
                        5.0f, 0.01f, WIN_WIDTH, WIN_HEIGHT);

	std::vector<Spotlight> spotlights;
	spotlights.push_back(spotlight);
	std::vector<Spotlight>* spotlights_ptr = &spotlights;
	//***** Store the entities
	std::list<Entity> entity_list;
//	entity_list.push_back(cube_entity);
	entity_list.push_back(bottle_entity);
	std::list<Entity>* entity_list_ptr = &entity_list;
//===================================================================
//

	//***** Main Loop
	while (!window.should_close()) {
		//***** Timings
		float current_frame = glfwGetTime();
		dT = current_frame - last_frame;
		last_frame = current_frame;

		//***** Input
		window.process_input(dT);
	
		//***** Render
		renderer.render_scene(entity_list_ptr, spotlights_ptr);

		window.swap_buffers();
	}

	window.close();
	return 0;
}
