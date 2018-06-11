#ifndef LIGHT_HPP
#define LIGHT_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Spotlight {
	public:	
		Spotlight(glm::vec3 pos, glm::vec3 target, glm::vec3 col, float str, float rad, int win_width_, int win_height_);
		Spotlight() {}
		glm::mat4 get_PV();
		void update_PV(glm::vec3 pvec, glm::vec3 target);
		glm::vec3 colour = {1.0f, 1.0f, 1.0f};
		glm::vec3 direction = {1.0f, 1.0f, 1.0f};
		glm::vec3 target = {1.0f, 1.0f, 1.0f};
		float radius = 1.0f;
		float strength = 0.0f;
		float r_1 = 3.0f;
		bool ON = false;
		glm::vec3 pvec;
			
	private:
		glm::mat4 PV;
		glm::mat4 projM;
		glm::mat4 viewM;
		int WIN_WIDTH;
		int WIN_HEIGHT;

};

class Pointlight {
	public:
		Pointlight(glm::vec3 pos, glm::vec3 col, float str) : pvec(pos), strength(str), colour(col) {}
		glm::vec3 pvec;
		glm::vec3 colour;
		float strength;
};

#endif
