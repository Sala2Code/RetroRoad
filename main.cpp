#include "common/include.hpp"
#include "include/constants.hpp"
#include <functional>

#include "common/loadShader.hpp"

#include "include/display.hpp"
#include "include/controls.hpp"
#include "include/world.hpp"
#include "include/function.hpp"

int main(){
	glfwInit();
    glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width, height, "Retro World", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window); 
	gladLoadGL();
	glfwSwapInterval(1); // 60 fps
	glViewport(0, 0, width, height);

	// * Shaders
	GLuint shaderProgramBasic = LoadShaders("shader/basic.vert", "shader/basic.frag");
	GLuint shaderProgramBlur = LoadShaders("shader/Blur.vert", "shader/Blur.frag");
	GLuint shaderProgramBloom = LoadShaders("shader/bloom.vert", "shader/bloom.frag");
	GLuint shaderProgramSky = LoadShaders("shader/sky.vert", "shader/sky.frag");

	glUseProgram(shaderProgramBasic);
    glUniform1i( glGetUniformLocation(shaderProgramBasic, "diffuseTexture"),0);
	GLuint MatrixID = glGetUniformLocation(shaderProgramBasic, "MVP");

    glUseProgram(shaderProgramBlur);
    glUniform1i( glGetUniformLocation(shaderProgramBlur, "screenTexture"),0);

    glUseProgram(shaderProgramBloom);
    glUniform1i( glGetUniformLocation(shaderProgramBloom, "screenTexture"),0);
    glUniform1i( glGetUniformLocation(shaderProgramBloom, "bloomTexture"),1);

	glUseProgram(shaderProgramSky);
    glUniform1i( glGetUniformLocation(shaderProgramSky, "skybox"), 0);
	GLuint ProjViewMatrixID = glGetUniformLocation(shaderProgramSky, "ProjView");

	// * Perlin Noise
	int chunkHeightMap[widthMap*widthMap];
	for(int x=0;x<widthMap;x++){
		for(int z=0;z<widthMap;z++){
			chunkHeightMap[x*widthMap+z] = (int) (((Get2DPerlinNoiseValue(z,  x,  res))+1)*0.5*150);
		}
	}
	Player player(glm::vec3(widthMap/2,limSol+2, widthMap/2));

	static const GLfloat VertexData[3*3*widthMap*widthMap*2] = {0};
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexData), VertexData, GL_STATIC_DRAW);

	// * FrameBuffers : Bloom effect
	float quadVertices[] = { 
        -1.0f,  1.0f,
        -1.0f, -1.0f,
         1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f, -1.0f,
         1.0f,  1.0f,
    };
	float quadUVs[] = {
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 0.0f,
        1.0f, 1.0f
    };

	GLuint vertexbufferQuad;
	glGenBuffers(1, &vertexbufferQuad);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbufferQuad);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	GLuint uvsbufferQuad;
	glGenBuffers(1, &uvsbufferQuad);
	glBindBuffer(GL_ARRAY_BUFFER, uvsbufferQuad);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadUVs), quadUVs, GL_STATIC_DRAW);

	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

    unsigned int postProcessingFBO;
	glGenFramebuffers(1, &postProcessingFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, postProcessingFBO);

    GLuint depthbuffer;
    glGenRenderbuffers(1, &depthbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthbuffer);

	unsigned int postProcessingTexture;
	glGenTextures(1, &postProcessingTexture);
	glBindTexture(GL_TEXTURE_2D, postProcessingTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcessingTexture, 0);

	unsigned int bloomTexture;
	glGenTextures(1, &bloomTexture);
	glBindTexture(GL_TEXTURE_2D, bloomTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, bloomTexture, 0);

	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);

	auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Post-Processing Framebuffer error: " << fboStatus << std::endl;

	unsigned int pingpongFBO[2];
	unsigned int pingpongBuffer[2];
	glGenFramebuffers(2, pingpongFBO);
	glGenTextures(2, pingpongBuffer);
	for (unsigned int i = 0; i < 2; i++){
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, pingpongBuffer[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongBuffer[i], 0);

		fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (fboStatus != GL_FRAMEBUFFER_COMPLETE){
			std::cout << "Ping-Pong Framebuffer error: " << fboStatus << std::endl;
		}
	}

	// * CubeMap
	float cubeMapVertices[] = {
        -1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		 1.0f, 1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		 1.0f,-1.0f,-1.0f,
		 1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		 1.0f,-1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,
		 1.0f,-1.0f,-1.0f,
		 1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f,-1.0f,
		 1.0f, 1.0f, 1.0f,
		 1.0f,-1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,
		 1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		 1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		 1.0f,-1.0f, 1.0f
    };
	GLuint vertexbufferCubeMap;
	glGenBuffers(1, &vertexbufferCubeMap);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbufferCubeMap);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeMapVertices), cubeMapVertices, GL_STATIC_DRAW);


	std::string Cubemap[6] = {
	    "img/pz.jpg", // pz
	    "img/nz.jpg", // nz
	    "img/py.jpg", // py
	    "img/ny.jpg", // ny
	    "img/nx.jpg", // nx
	    "img/px.jpg" // px
	};

	unsigned int cubemapTexture;
	glGenTextures(1, &cubemapTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	for (unsigned int i = 0; i < 6; i++){
		int width, height, nrChannels;
		unsigned char* data = stbi_load(Cubemap[i].c_str(), &width, &height, &nrChannels, 0);
		if (data){
			stbi_set_flip_vertically_on_load(false);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else{
			std::cout << "Failed to load texture: " << Cubemap[i] << std::endl;
			stbi_image_free(data);
		}
	}

    glfwSetCursorPos(window, width/2, height/2);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int aPosX = widthMap/2; // To set the player (camera) to center
	

	while (!glfwWindowShouldClose(window))
	{
		fps(window, lastTime, lastTimeFPS, n_frame, deltaTime); // Update deltaTime and display fps

		if(player.pos.x-aPosX > tolView){ // Shift left (to update last lines)
			shiftLeft(chunkHeightMap, widthMap*widthMap, tolView*widthMap);
			aPosX = player.pos.x;
			for(int z=0;z<widthMap;z++){
				chunkHeightMap[(widthMap-1)*widthMap+z] = (int) (((Get2DPerlinNoiseValue(z,  player.pos.x+widthMap/2,  res))+1)*0.5*150); // if tolView = 1
			}
		}
		else if(aPosX-player.pos.x > tolView){ // Shift right (to update first lines)
			if(aPosX>widthMap){
				shiftRight(chunkHeightMap, widthMap*widthMap, tolView*widthMap*2);
				aPosX = player.pos.x;
				for(int x=0;x<2;x++){
					for(int z=0;z<widthMap;z++){
						chunkHeightMap[x*widthMap+z] = (int) (((Get2DPerlinNoiseValue(z,  player.pos.x-widthMap/2+x,  res))+1)*0.5*150); // if tolView = 1
					}
				}	
			}
		}
		
		glBindFramebuffer(GL_FRAMEBUFFER, postProcessingFBO);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDisable(GL_CULL_FACE);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0, 0, 0, 1);

		glBindVertexArray(VAO);
		View = controlsView(window, deltaTime, player);
		player.pos.y = limSol+1;
		glm::mat4 ProjView = Projection * View ;
		Model = glm::mat4(1.0);
		mvp =  ProjView * Model ;	
		
		glUseProgram(shaderProgramBasic);
		GLfloat newTriangle[9*2*widthMap*widthMap];

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);

		// * Create the world dynamically
		for(int x=0; x<widthMap-1; x++){
			int xco = x + aPosX - widthMap/2;
			for(int z=0;z<widthMap;z++){
				GLfloat coordTri[] = { 
					(GLfloat) xco, (displayWorld(chunkHeightMap[x*widthMap+z], z)) ? (GLfloat) chunkHeightMap[x*widthMap+z] : GLlimSol, (GLfloat) z,
					(GLfloat) xco+1, (displayWorld(chunkHeightMap[(x+1)*widthMap+z], z)) ? (GLfloat) chunkHeightMap[(x+1)*widthMap+z] : GLlimSol, (GLfloat) z,
					(GLfloat) xco+1, (displayWorld(chunkHeightMap[(x+1)*widthMap+z+1], z+1)) ? (GLfloat) chunkHeightMap[(x+1)*widthMap+z+1] : GLlimSol, (GLfloat) z+1,

					(GLfloat) xco, (displayWorld(chunkHeightMap[x*widthMap+z], z)) ? (GLfloat) chunkHeightMap[x*widthMap+z] : GLlimSol, (GLfloat) z,
					(GLfloat) xco, (displayWorld(chunkHeightMap[x*widthMap+z+1], z+1)) ? (GLfloat) chunkHeightMap[x*widthMap+z+1] : GLlimSol, (GLfloat) z+1,
					(GLfloat) xco+1, (displayWorld(chunkHeightMap[(x+1)*widthMap+z+1], z+1)) ? (GLfloat) chunkHeightMap[(x+1)*widthMap+z+1] : GLlimSol, (GLfloat) z+1,
				};
				for(unsigned short i=0;i<sizeof(coordTri);i++){
					newTriangle[9*2*(x*widthMap+z)+i] = coordTri[i];
				}
			}
		}
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(VertexData), &newTriangle);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawArrays(GL_TRIANGLES, 0, 3*widthMap*widthMap*2); 
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		unbindBuffer(2);

		// * CubeMap
        glDepthFunc(GL_LEQUAL);
        glUseProgram(shaderProgramSky);
        glm::mat4 ProjCube = Projection * glm::mat4(glm::mat3(View));
        glUniformMatrix4fv(ProjViewMatrixID, 1, GL_FALSE, &ProjCube[0][0]);

        glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbufferCubeMap);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthFunc(GL_LESS);
		unbindBuffer(1);

		// * Blur
		bool horizontal = true, first_iteration = true;
		int amount = 2; // Amount of time to bounce the blur
		glUseProgram(shaderProgramBlur);
		for (unsigned int i = 0; i < amount; i++){
			glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
			glUniform1i(glGetUniformLocation(shaderProgramBlur, "horizontal"), horizontal);

			// Get the data from the bloomTexture
			if (first_iteration){
				glBindTexture(GL_TEXTURE_2D, bloomTexture);
				first_iteration = false;
			}
			// Move the data between the pingPong textures
			else{
				glBindTexture(GL_TEXTURE_2D, pingpongBuffer[!horizontal]);
			}

			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, vertexbufferQuad);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
			
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, uvsbufferQuad);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
			glDisable(GL_DEPTH_TEST);
			glDrawArrays(GL_TRIANGLES, 0, 6);

			horizontal = !horizontal;
		}

		// * Draw the whole scene
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(shaderProgramBloom);

		glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbufferQuad);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
        
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, uvsbufferQuad);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glDisable(GL_DEPTH_TEST);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, postProcessingTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, pingpongBuffer[!horizontal]);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		unbindBuffer(2);


		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	
	glDeleteProgram(shaderProgramBasic);
	glDeleteProgram(shaderProgramBlur);
	glDeleteProgram(shaderProgramBloom);

	glDeleteBuffers(1, &vertexbuffer);

	glDeleteBuffers(1, &vertexbufferQuad);
	glDeleteBuffers(1, &uvsbufferQuad);

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}