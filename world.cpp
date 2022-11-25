#include "include/world.hpp"

const unsigned int perm[] =
    {151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,
    142,8,99,37,240,21,10,23,190,6,148,247,120,234,75,0,26,197,62,94,252,219,
    203,117,35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168,68,175,
    74,165,71,134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,
    105,92,41,55,46,245,40,244,102,143,54,65,25,63,161,1,216,80,73,209,76,132,
    187,208,89,18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,186,3,
    64,52,217,226,250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,
    47,16,58,17,182,189,28,42,223,183,170,213,119,248,152,2,44,154,163,70,221,
    153,101,155,167,43,172,9,129,22,39,253,19,98,108,110,79,113,224,232,178,185,
    112,104,218,246,97,228,251,34,242,193,238,210,144,12,191,179,162,241,81,51,145,
    235,249,14,239,107,49,192,214,31,181,199,106,157,184,84,204,176,115,121,50,45,
    127,4,150,254,138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,
    156,180};


float Get2DPerlinNoiseValue(float x, float y, float res){
    float tmp,Cx,Cy,Li1,Li2;

    x /= res;
    y /= res;

    int x0 = (int)(x);
    int y0 = (int)(y);

    unsigned short ii = x0 & 255;
    unsigned short jj = y0 & 255;

    //Pour récupérer les vecteurs
    int gi0 = perm[ii + perm[jj]] % 8;
    int gi1 = perm[ii + 1 + perm[jj]] % 8;
    int gi2 = perm[ii + perm[jj + 1]] % 8;
    int gi3 = perm[ii + 1 + perm[jj + 1]] % 8;

    float s = v_gradient2(gi0, x-x0, y-y0);
    float t = v_gradient2(gi1, x-x0-1, y-y0);
    float u = v_gradient2(gi2, x-x0, y-y0-1);
    float v = v_gradient2(gi3, x-x0-1, y-y0-1);

    //Lissage
    tmp = x-x0;
    Cx = 3 * tmp * tmp - 2 * tmp * tmp * tmp;
    Li1 = s + Cx*(t-s);
    Li2 = u + Cx*(v-u);
    tmp = y - y0;
    Cy = 3 * tmp * tmp - 2 * tmp * tmp * tmp;

    return Li1 + Cy*(Li2-Li1);
}

float v_gradient2(int &gi, float valX, float valY){
    float unit = 1.0f/sqrt(2);
    float gradient2[][2] = {{unit,unit},{-unit,unit},{unit,-unit},{-unit,-unit},{1,0},{-1,0},{0,1},{0,-1}};

    return gradient2[gi][0]*valX + gradient2[gi][1]*valY;
}

bool displayWorld(int &height, int z){
    if(height>limSol && ( (z<widthMap/2 - widthRoad && z<widthMap/2) || (z>widthMap/2 + widthRoad && z>widthMap/2) ) ){
        return true;
    }
   return false;

}