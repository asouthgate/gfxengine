#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "light.hpp"

Spotlight::Spotlight(glm::vec3 pos, glm::vec3 target_, glm::vec3 col, float str, float rad, int win_width_, int win_height_) : pvec(pos), target(target_), colour(col), strength(str), radius(rad), WIN_WIDTH(win_width_), WIN_HEIGHT(win_height_) {
	projM = glm::ortho<float>(pos.x-4.,pos.x+4.,pos.y-4.,pos.y+4.,pos.z-0.9,pos.z+5.5);
	direction = pos-target;
	viewM = lookAt(pos, target, glm::vec3(0.0f, 1.0f, 0.0f));
	PV = projM*viewM;
}


void Spotlight::update_PV(glm::vec3 pos, glm::vec3 target) {
	viewM = lookAt(pos, target, glm::vec3(0.0f, 1.0f, 0.0f));
	PV = projM*viewM;
}

glm::mat4 Spotlight::get_PV() {
        return PV;
}

