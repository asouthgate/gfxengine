#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <string>
#include <list>
#include <vector>
#include <glm/glm.hpp>
#include <sstream>
#include "mesh.hpp"
#include "light.hpp"
#include "texture.hpp"

class Entity {
        public:
		//***** Functions
		Entity();
		void build_matrices();

		//***** Graphics data: model
		Mesh* mesh_ptr;
		Texture* texture_ptr;
		Texture* diffuse_ptr;

		//***** Graphical quantities
		glm::mat4 transM;
		glm::mat4 inverse_transM;
		glm::mat4 rotM;
		glm::mat4 scaleM;
		glm::mat4 modM;
		glm::mat4 tsM;
		glm::mat4 inverse_modM;
		glm::vec3 scalevec = {0.5f, 0.5f, 0.5f};
		glm::vec3 pvec = {0.0f, 0.0f, -1.0f};
		glm::vec3 model_pvec = {0.0f, 0.0f, 0.0f};
		glm::vec3 rotational_axis = {0.0f, 1.0f, 0.0f};
		float angle = 0;
		glm::vec3 direction = {1.0f, 0.0f, 0.0f};
		glm::vec3 normal_direction = {0.0f, 1.0f, 0.0f};
		glm::vec3 original_direction = {1.0f, 0.0f, 0.0f};

		//***** Flags and labels
		bool VISIBLE = false;
		std::string name = "default name";

		//***** Light sources
		std::vector<Spotlight> spotlights;
		std::vector<Pointlight> pointlights;
};

#endif
