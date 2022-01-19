#include "Terrain.h"
#include <stb/stb_image.h>

Terrain::Terrain(unsigned resolution, float scale, string texPath) {
	this->resolution = resolution;
	this->scale = scale;
    this->texPath = texPath;
}

Mesh Terrain::generateTerrain(float maxHeight, float smoothness, unsigned seed) {
    float step = scale / (float)resolution;
    this->maxHeight = maxHeight;
    this->smoothness = smoothness;

    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    vector<Texture>      textures;

    if (perlin != NULL)
        delete perlin;
    perlin = new PerlinNoise(seed);

    cout << maxHeight << endl;

    float noiseMaxHeight = 0;
    float noiseMinHeight = maxHeight;
    for (int i = 0; i < resolution; i++) {
        for (int j = 0; j < resolution; j++) {
            float height = getPerlinHeight(j, i);
            if (height > noiseMaxHeight)
                noiseMaxHeight = height;
            if (height < noiseMinHeight)
                noiseMinHeight = height;
        }
    }
    yOffset = noiseMinHeight;
    yFactor = maxHeight / (noiseMaxHeight - noiseMinHeight);

    cout << yOffset << endl;
    cout << noiseMaxHeight << endl;
    cout << yFactor << endl;

    int repeats = 2;

    for (int i = 0; i < resolution; i++) {
        for (int j = 0; j < resolution; j++) {
            Vertex newVertex;
            newVertex.Position.x = j * step;
            newVertex.Position.y = getPerlinHeight(j, i);
            newVertex.Position.z = i * step;
            newVertex.TexCoords.x = repeats * ((float)j) / (float)resolution;
            newVertex.TexCoords.y = repeats * (1.0 - ((float)i) / (float)resolution);
            vertices.push_back(newVertex);
        }

        if (i > 0) {
            int t1, t2, t3, t4;
            for (int j = 0; j < resolution - 1; j++) {
                t1 = ((i - 1) * resolution + j);
                t2 = ((i - 1) * resolution + (j + 1));
                t3 = (i * resolution + j);
                t4 = (i * resolution + (j + 1));

                //cout << "[" << t1 << ", " << t2 << ", " << t3 << "], ";
                //cout << "[" << t2 << ", " << t3 << ", " << t4 << "]" << endl;

                indices.push_back(t1);
                indices.push_back(t2);
                indices.push_back(t3);
                indices.push_back(t2);
                indices.push_back(t3);
                indices.push_back(t4);
            }
        }
    }
    unsigned texId;
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char* data = stbi_load(texPath.c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    
    Texture blankTexture;
    blankTexture.id = texId;
    blankTexture.type = "NO_TYPE";
    //"scenery/Scenery/resources/tex_grass.png";
    //blankTexture.path = "NO_PATH";
    blankTexture.path = texPath.c_str();
    textures.push_back(blankTexture);

    Mesh terrain(vertices, indices, textures);
    return terrain;
}

float Terrain::getHeight(float x, float z) {
    double xPos = (double)x / (double)(scale * smoothness);
    double yPos = (double)z / (double)(scale * smoothness);
    return calculateHeight(xPos, yPos);
}

float Terrain::getPerlinHeight(int j, int i) {
    double xPos = (double)j / (double)(resolution * smoothness);
    double yPos = (double)i / (double)(resolution * smoothness);
    return calculateHeight(xPos, yPos);
}

float Terrain::calculateHeight(float x, float y) {
    float strength = maxHeight;// / 255.0f;
    return (float)((perlin->getNoise(x * scale, y * scale, 0.5) * strength) - yOffset) * yFactor;
}