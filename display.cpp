#include "common/include.hpp"
#include "include/display.hpp"

void generateBuffer(
            GLuint &EBO, GLuint &vertexbuffer, GLuint &uvbuffer, GLuint &normalbuffer,
            std::vector<unsigned short> &indices, std::vector<glm::vec3> &indexed_vertices, 
            std::vector<glm::vec2> &indexed_uvs, std::vector<glm::vec3> &indexed_normals)
{
    
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);
	
    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);

}

void fps(GLFWwindow *window, double &lastTime, double &lastTimeFPS, unsigned int &n_frame, float &deltaTime){
	    double currentTime = glfwGetTime();
		n_frame++;
		if(currentTime - lastTimeFPS >=1){
			lastTimeFPS+=1;
			std::string title = "Retro World " + std::to_string(n_frame);
			glfwSetWindowTitle(window, title.c_str());
			n_frame = 0;
		}
        deltaTime = float(currentTime - lastTime);
        lastTime = currentTime;
}

void bindBuffer(GLuint &EBO, GLuint &vertexbuffer, GLuint &uvbuffer, GLuint &normalbuffer){
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glVertexAttribPointer(1, 2,GL_FLOAT,GL_FALSE, 0,(void*)0);
		        
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
		glVertexAttribPointer(2, 3,GL_FLOAT,GL_FALSE, 0,(void*)0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
}

void unbindBuffer(unsigned short n){
	for(unsigned short i=0;i<n;i++){
		glDisableVertexAttribArray(i);
	}
}

void bindTexture(GLuint &Texture, GLuint &TextureID){
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture);
	glUniform1i(TextureID, 0);
}