#include "common/include.hpp"
#include "include/controls.hpp"


extern int width;
extern int height;

extern float horizontalAngleF5;
extern float verticalAngleF5;

extern float speed; 
extern float speedAngle;
extern float mouseSpeed; 
extern float FoV;

bool isPressF3 = false;
bool isPressF5 = false;

/*
La map est normalement de ce type :
     ^ (z)
     |
(x)  |
<----
Or ca pose pas mal de probleme et l'angle 0 du player correspond à pi/2
Donc j'ai mis un angle initial du joueur de pi/2 (et donc 0 selon z) et à partir de ça le plan est initialement ça :
     ^ (x)
     |
     |      (z)
      ------>
Ainsi se souvenir que vu de haut, absisses (z) et ordonnées (x)
*/

glm::mat4 controlsView(GLFWwindow* window, float &deltaTime, Player &player){
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    glfwSetCursorPos(window, width/2, height/2);

    float decX = mouseSpeed * deltaTime * float(width/2 - xpos);
    float decY = mouseSpeed * deltaTime * float(height/2 - ypos);

    horizontalAngleF5 += decX;
    verticalAngleF5 -= ( -1.57f <= verticalAngleF5 - decY && verticalAngleF5 - decY <= 1.57f) ? decY : 0;

    glm::vec3 up = glm::vec3(0,1,0);
    
    glm::vec3 direction = glm::vec3(cos(-verticalAngleF5)*sin(horizontalAngleF5), sin(-verticalAngleF5), cos(-verticalAngleF5)*cos(horizontalAngleF5));

    movePlayer(window, deltaTime, player, direction);
    glm::mat4 View = glm::lookAt(player.pos, player.pos+direction, up);

    return View ; 
}

void movePlayer(GLFWwindow* window, float &deltaTime, Player &player, glm::vec3 &direction){
    glm::vec3 right = glm::vec3(sin(horizontalAngleF5 - 3.14f/2.0f), 0, cos(horizontalAngleF5 - 3.14f/2.0f));

    if (glfwGetKey( window, GLFW_KEY_UP ) == GLFW_PRESS){
        player.pos += direction * deltaTime * speed;
    }

    if (glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS){
        player.pos -= direction * deltaTime * speed;
    }
    glm::vec3 rds = right * deltaTime * speed;
    if (glfwGetKey( window, GLFW_KEY_RIGHT ) == GLFW_PRESS){
        player.pos += rds;
    }
    if (glfwGetKey( window, GLFW_KEY_LEFT ) == GLFW_PRESS){
        player.pos -= rds;
    }
}
