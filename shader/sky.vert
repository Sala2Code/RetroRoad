#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 UVCoords;

uniform mat4 ProjView;

void main(){
    vec4 pos = ProjView * vec4(aPos, 1.0f);

    gl_Position = vec4(pos.x, pos.y, pos.w, pos.w);  // z equal w, result depth of 1.0f
    UVCoords = vec3(aPos.x, aPos.y, -aPos.z);    // flip the z axis due to the different coordinate systems (left hand vs right hand)
}   