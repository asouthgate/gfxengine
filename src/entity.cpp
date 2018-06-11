#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <stdlib.h>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/projection.hpp>
#include <glm/gtc/random.hpp>
#include "texture.hpp"
#include "mesh.hpp"
#include "entity.hpp"

float PI = 3.14;

Entity::Entity() { 
}

void Entity::build_matrices() {
	scaleM = glm::scale(glm::mat4(1.0f), scalevec);
	transM = glm::translate(glm::mat4(1.0f), pvec);
	rotM = glm::rotate(glm::mat4(1.0f), angle, rotational_axis);
	tsM = transM*scaleM;
	modM = tsM*rotM;
	inverse_modM = glm::transpose(glm::inverse(modM));
	inverse_transM = glm::inverse(transM);
	direction = glm::normalize(glm::vec3(original_direction.x*cos(angle) - original_direction.y*sin(angle), original_direction.x*sin(angle)+original_direction.y*cos(angle), 0.0f));
	normal_direction = glm::normalize(glm::vec3(original_direction.x*cos(angle+PI/2) - original_direction.y*sin(angle+PI/2), original_direction.x*sin(angle+PI/2)+original_direction.y*cos(angle+PI/2),0.0f));
}
