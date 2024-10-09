#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <zip.h>

#if defined(_WIN32) || defined(__CYGWIN__)
#include <windows.h>
#include <commdlg.h>
#include <direct.h>
char* OpenFileDialog(){
    static char filePath[256];
    char currentDir[256];
    _getcwd(currentDir, sizeof(currentDir));
    OPENFILENAMEA ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = nullptr;
    ofn.lpstrFile = filePath;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(filePath);
    ofn.lpstrFilter = "HDR Files\0*.hdr\0""All Image Files\0*.hdr;*.png;*.jpg;*.jpeg;*.bmp\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    if(GetOpenFileNameA(&ofn)) {
        _chdir(currentDir);
        return filePath;
    }
    else {
        _chdir(currentDir);
        return nullptr;
    }
}
char* OpenFileDialogTex(){
    static char filePath[256];
    char currentDir[256];
    _getcwd(currentDir, sizeof(currentDir));
    OPENFILENAMEA ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = nullptr;
    ofn.lpstrFile = filePath;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(filePath);
    ofn.lpstrFilter = "All Image Files\0*.png;*.jpg;*.jpeg;*.bmp\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    if(GetOpenFileNameA(&ofn)) {
        _chdir(currentDir);
        return filePath;
    }
    else {
        _chdir(currentDir);
        return nullptr;
    }
}
char* OpenFileDialogZip(){
    static char filePath[256];
    char currentDir[256];
    _getcwd(currentDir, sizeof(currentDir));
    OPENFILENAMEA ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = nullptr;
    ofn.lpstrFile = filePath;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(filePath);
    ofn.lpstrFilter = "Zip Files (*.zip)\0*.zip\0All Files (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    if(GetOpenFileNameA(&ofn)) {
        _chdir(currentDir);
        return filePath;
    }
    else {
        _chdir(currentDir);
        return nullptr;
    }
}
#else
#include <QFileDialog>
#include <QString>
#include <QApplication>
char* OpenFileDialog(){
    static char filePath[256];
    int argc = 0;
    char* argv[] = {nullptr};
    QApplication app(argc, argv);
    QApplication::setApplicationName("Material Viewer");
    QString filename = QFileDialog::getOpenFileName(nullptr, "Open File", "", "HDR Files (*.hdr);;All Image Files (*.hdr *.png *.jpg *.jpeg *.bmp)");
    if(!filename.isEmpty()){
        snprintf(filePath, sizeof(filePath), "%s", filename.toStdString().c_str());
        return filePath;
    }
    else return nullptr;
}
char* OpenFileDialogTex(){
    static char filePath[256];
    int argc = 0;
    char* argv[] = {nullptr};
    QApplication app(argc, argv);
    QApplication::setApplicationName("Material Viewer");
    QString filename = QFileDialog::getOpenFileName(nullptr, "Open File", "", "All Image Files (*.png *.jpg *.jpeg *.bmp)");
    if(!filename.isEmpty()){
        snprintf(filePath, sizeof(filePath), "%s", filename.toStdString().c_str());
        return filePath;
    }
    else return nullptr;
}
char* OpenFileDialogZip(){
    static char filePath[256];
    int argc = 0;
    char* argv[] = {nullptr};
    QApplication app(argc, argv);
    QApplication::setApplicationName("Material Viewer");
    QString filename = QFileDialog::getOpenFileName(nullptr, "Open Zip File", "", "Zip Files (*.zip);;All Files (*.*)");
    if(!filename.isEmpty()){
        snprintf(filePath, sizeof(filePath), "%s", filename.toStdString().c_str());
        return filePath;
    }
    else return nullptr;
}
#endif
unsigned int SCR_WIDTH=800;
unsigned int SCR_HEIGHT=600;
const float pi = 3.14159265359;
float radius = 5.0f;
float yaw = pi/8;
float pitch = pi/12;
glm::vec3 camPos = glm::vec3(sin(yaw)*radius, sin(pitch)*radius, cos(yaw)*radius);
float lastX = SCR_WIDTH / 2.0;
float lastY = SCR_HEIGHT / 2.0;
float fov = 45.0f;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
bool firstMouse = true;
bool pressMouse = false;
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow *window);
char* getShaders(char file[]){
    FILE* shaderFile = fopen(file, "r");
    if(!shaderFile) {
        std::cout<<"Error opening shader file at "<<file<<"\n";
        return nullptr;
    }
    int fileSize = 0;
    fseek(shaderFile, 0, SEEK_END);
    fileSize = ftell(shaderFile);
    rewind(shaderFile);
    char* shader = (char*)malloc(sizeof(char) * (fileSize+1));
    fread(shader, sizeof(char), fileSize, shaderFile);
    shader[fileSize] = '\0';
    fclose(shaderFile);
    return shader;
}
unsigned int loadEnv(char file[]){
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrComponents;
    float* data = stbi_loadf(file, &width, &height, &nrComponents, 0);
    unsigned int hdrTexture;
    if(data){
        glGenTextures(1, &hdrTexture);
        glBindTexture(GL_TEXTURE_2D, hdrTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, data);
        stbi_image_free(data);
    }
    else{
        std::cout<<"HDR image failed to load at path "<<file<<std::endl;
        stbi_image_free(data);
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    return hdrTexture;
}
unsigned int loadTexture(char file[]){
    unsigned int textureID;
    glGenTextures(2, &textureID);
    int width, height, nrComponents;
    unsigned char* data = stbi_load(file, &width, &height, &nrComponents, 0);
    if(data){
        GLenum format;
        if(nrComponents == 1) format = GL_RED;
        else if(nrComponents == 3) format = GL_RGB;
        else if(nrComponents == 4) format = GL_RGBA;
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
    }
    else{
        std::cout<<"Texture image filed to load at path "<<file<<std::endl;
        stbi_image_free(data);
    }
    return textureID;
}
unsigned int* OpenZipFile(char* path){
    char* filenames[] = {
        (char*)"albedo.png",
        (char*)"metalness.png",
        (char*)"normal.png",
        (char*)"roughness.png",
        (char*)"ao.png",
    };
    const int numTextures = 5;
    unsigned int* textures = new unsigned int[numTextures];
    for(unsigned int i=0; i<numTextures; i++) textures[i] = -1;
    int err = 0;
    zip* archive = zip_open(path, 0, &err);
    if(!archive) {
        return textures;
    }
    for(unsigned int i=0; i<numTextures; i++){
        zip_file* zfile = zip_fopen(archive, filenames[i], 0);
        if(!zfile) {
            continue;
        }
        FILE* output = fopen(filenames[i], "wb");
        if(!output){
            zip_fclose(zfile);
            continue;
        }
        char buffer[4096];
        int bytesRead;
        while((bytesRead = zip_fread(zfile, buffer, sizeof(buffer))) > 0)
            fwrite(buffer, 1, bytesRead, output);
        fclose(output);
        zip_fclose(zfile);
        textures[i] = loadTexture(filenames[i]);
        remove(filenames[i]);
    }
    zip_close(archive);
    return textures;
}
unsigned int createShader(const char* vertSource, const char* fragSource){
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertSource, nullptr);
    glCompileShader(vertexShader);
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cout<<"A vertex shader compilation failed.\n"<<infoLog<<std::endl;
    }
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragSource, nullptr);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cout<<"A fragment shader compilation failed.\n"<<infoLog<<std::endl;
    }
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success){
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cout << "Linking shader program failed. \n"<<infoLog<<std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return shaderProgram;
}
std::pair<std::pair<unsigned int, unsigned int>, std::pair<unsigned int, unsigned int>> HDRItoCubemap(char environmentLoc[], unsigned int skyProgram, unsigned int irradianceProgram, unsigned int prefilterProgram, unsigned int brdfProgram, unsigned int VAO, unsigned int quadVAO){
    unsigned int captureFBO;
    unsigned int captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);
    unsigned int hdrTexture = loadEnv(environmentLoc);
    unsigned int envCubemap;
    glGenTextures(1, &envCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    for(unsigned int i=0; i<6; i++){
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB32F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glm::mat4 captureProj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] = {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
    };
    glUseProgram(skyProgram);
    glUniform1i(glGetUniformLocation(skyProgram, "skybox"), 0); 
    glUniformMatrix4fv(glGetUniformLocation(skyProgram, "projection"), 1, GL_FALSE, &captureProj[0][0]);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);
    glViewport(0, 0, 512, 512);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for(unsigned int i=0; i<6; i++){
        glUniformMatrix4fv(glGetUniformLocation(skyProgram, "view"), 1, GL_FALSE, &captureViews[i][0][0]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    unsigned int irradianceMap;
    glGenTextures(1, &irradianceMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
    for(unsigned int i=0; i<6; i++){
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB32F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);
    glUseProgram(irradianceProgram);
    glUniform1i(glGetUniformLocation(irradianceProgram, "skybox"), 0); 
    glUniformMatrix4fv(glGetUniformLocation(irradianceProgram, "projection"), 1, GL_FALSE, &captureProj[0][0]);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    glViewport(0, 0, 32, 32);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for(unsigned int i=0; i<6; i++){
        glUniformMatrix4fv(glGetUniformLocation(irradianceProgram, "view"), 1, GL_FALSE, &captureViews[i][0][0]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    unsigned int prefilterMap;
    glGenTextures(1, &prefilterMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
    for(unsigned int i=0; i<6; i++){
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB32F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    glUseProgram(prefilterProgram);
    glUniform1i(glGetUniformLocation(prefilterProgram, "skybox"), 0); 
    glUniformMatrix4fv(glGetUniformLocation(prefilterProgram, "projection"), 1, GL_FALSE, &captureProj[0][0]);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    unsigned int maxMipLevels = 5;
    for(unsigned int mip = 0; mip<maxMipLevels; mip++){
        unsigned int mipWidth = static_cast<unsigned int>(128 * std::pow(0.5, mip));
        unsigned int mipHeight = static_cast<unsigned int>(128 * std::pow(0.5, mip));
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);
        float roughness = (float)mip / (float)(maxMipLevels - 1);
        glUniform1f(glGetUniformLocation(prefilterProgram, "roughness"), roughness); 
        for(unsigned int i=0; i<6; i++){
            glUniformMatrix4fv(glGetUniformLocation(prefilterProgram, "view"), 1, GL_FALSE, &captureViews[i][0][0]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(0);
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    unsigned int brdfLUTTexture;
    glGenTextures(1, &brdfLUTTexture);
    glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);
    glViewport(0, 0, 512, 512);
    glUseProgram(brdfProgram);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    return {{envCubemap, irradianceMap}, {prefilterMap, brdfLUTTexture}};
}

char vertexLoc[] = "./src/shaders/main.vert";
char fragmentLoc[] = "./src/shaders/main.frag";
char skyFragmentLoc[] = "./src/shaders/sky.frag";
char cubemapVertexLoc[] = "./src/shaders/cubemap.vert";
char cubemapFragmentLoc[] = "./src/shaders/cubemap.frag";
char irradianceFragmentLoc[] = "./src/shaders/irradiance.frag";
char prefilterFragmentLoc[] = "./src/shaders/prefilter.frag";
char brdfFragmentLoc[] = "./src/shaders/brdf.frag";
char brdfVertexLoc[] = "./src/shaders/brdf.vert";
char uiVertexLoc[] = "./src/shaders/ui.vert";
char uiFragmentLoc[] = "./src/shaders/ui.frag";
const char* vertexShaderSource = getShaders(vertexLoc);
const char* fragmentShaderSource = getShaders(fragmentLoc);
const char* skyFragmentShaderSource = getShaders(skyFragmentLoc);
const char* cubemapVertexShaderSource = getShaders(cubemapVertexLoc);
const char* cubemapFragmentShaderSource = getShaders(cubemapFragmentLoc);
const char* irradianceFragmentShaderSource = getShaders(irradianceFragmentLoc);
const char* prefilterFragmentShaderSource = getShaders(prefilterFragmentLoc);
const char* brdfFragmentShaderSource = getShaders(brdfFragmentLoc);
const char* brdfVertexShaderSource = getShaders(brdfVertexLoc);
const char* uiVertexShaderSource = getShaders(uiVertexLoc);
const char* uiFragmentShaderSource = getShaders(uiFragmentLoc);
char albedoLoc[] = "./src/material/albedo.png";
char aoLoc[] = "./src/material/ao.png";
char metallicLoc[] = "./src/material/metallic.png";
char normalLoc[] = "./src/material/normal.png";
char roughnessLoc[] = "./src/material/roughness.png";
char environmentLocs[][70] = {
    "./src/environments/industrial_sunset_puresky/environment.hdr",
    "./src/environments/kloppenheim_02_puresky/environment.hdr",
    "./src/environments/snowy_forest/environment.hdr",
    "./src/environments/syferfontein_1d_clear_puresky/environment.hdr"
};
int currentElement = 0;
char uiElementLocs[][30] = {
    "./src/ui/hdri_ui1.png",
    "./src/ui/hdri_ui2.png",
    "./src/ui/hdri_ui3.png",
    "./src/ui/hdri_ui4.png",
    "./src/ui/hdri_ui5.png",
    "./src/ui/hdri_ui6.png",
    "./src/ui/hdri_ui7.png",
    "./src/ui/hdri_ui8.png",
    "./src/ui/hdri_ui9.png",
    "./src/ui/img_ui10.png",
    "./src/ui/img_ui11.png",
    "./src/ui/img_ui12.png",
    "./src/ui/img_ui13.png",
    "./src/ui/img_ui14.png",
    "./src/ui/img_ui15.png"
};
glm::vec3 extraColors[sizeof(uiElementLocs)/30];
bool highlightingUI = false;
bool selectingEnv = false;
bool selectingShape = false;
bool uploadingEnv = false;
char* uploadedEnv = nullptr;
bool showMaterialUI = false;
unsigned int albedo;
unsigned int metallic;
unsigned int normal;
unsigned int roughness;
unsigned int ao;

int main()
{   
    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window;
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Material Viewer (Alpha)", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to open GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouseCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    float vertices[] = {
        // vertex position,  texture coordinate,  normal vector
        -0.75f, -0.75f, -0.75f,  0.0f, 0.0f,  0.0f, 0.0f, -1.0f,
         0.75f, -0.75f, -0.75f,  1.0f, 0.0f,  0.0f, 0.0f, -1.0f,
         0.75f,  0.75f, -0.75f,  1.0f, 1.0f,  0.0f, 0.0f, -1.0f,
         0.75f,  0.75f, -0.75f,  1.0f, 1.0f,  0.0f, 0.0f, -1.0f,
        -0.75f,  0.75f, -0.75f,  0.0f, 1.0f,  0.0f, 0.0f, -1.0f,
        -0.75f, -0.75f, -0.75f,  0.0f, 0.0f,  0.0f, 0.0f, -1.0f,

        -0.75f, -0.75f,  0.75f,  0.0f, 0.0f,  0.0f, 0.0f, 1.0f,
         0.75f, -0.75f,  0.75f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,
         0.75f,  0.75f,  0.75f,  1.0f, 1.0f,  0.0f, 0.0f, 1.0f,
         0.75f,  0.75f,  0.75f,  1.0f, 1.0f,  0.0f, 0.0f, 1.0f,
        -0.75f,  0.75f,  0.75f,  0.0f, 1.0f,  0.0f, 0.0f, 1.0f,
        -0.75f, -0.75f,  0.75f,  0.0f, 0.0f,  0.0f, 0.0f, 1.0f,

        -0.75f,  0.75f,  0.75f,  1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
        -0.75f,  0.75f, -0.75f,  1.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
        -0.75f, -0.75f, -0.75f,  0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
        -0.75f, -0.75f, -0.75f,  0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
        -0.75f, -0.75f,  0.75f,  0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
        -0.75f,  0.75f,  0.75f,  1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,

         0.75f,  0.75f,  0.75f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f,
         0.75f,  0.75f, -0.75f,  1.0f, 1.0f,  1.0f, 0.0f, 0.0f,
         0.75f, -0.75f, -0.75f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f,
         0.75f, -0.75f, -0.75f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f,
         0.75f, -0.75f,  0.75f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
         0.75f,  0.75f,  0.75f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f,

        -0.75f, -0.75f, -0.75f,  0.0f, 1.0f,  0.0f, -1.0f, 0.0f,
         0.75f, -0.75f, -0.75f,  1.0f, 1.0f,  0.0f, -1.0f, 0.0f,
         0.75f, -0.75f,  0.75f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
         0.75f, -0.75f,  0.75f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
        -0.75f, -0.75f,  0.75f,  0.0f, 0.0f,  0.0f, -1.0f, 0.0f,
        -0.75f, -0.75f, -0.75f,  0.0f, 1.0f,  0.0f, -1.0f, 0.0f,

        -0.75f,  0.75f, -0.75f,  0.0f, 1.0f,  0.0f, 1.0f, 0.0f,
         0.75f,  0.75f, -0.75f,  1.0f, 1.0f,  0.0f, 1.0f, 0.0f,
         0.75f,  0.75f,  0.75f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f,
         0.75f,  0.75f,  0.75f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f,
        -0.75f,  0.75f,  0.75f,  0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
        -0.75f,  0.75f, -0.75f,  0.0f, 1.0f,  0.0f, 1.0f, 0.0f
    };
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));

    unsigned int indexCount;
    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> uv;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;
    const unsigned int X_SEGMENTS = 64;
    const unsigned int Y_SEGMENTS = 64;
    for(unsigned int x=0; x<=X_SEGMENTS; x++){
        for(unsigned int y=0; y<=Y_SEGMENTS; y++){
            float xSegment = (float)x / (float)X_SEGMENTS;
            float ySegment = (float)y / (float)Y_SEGMENTS;
            float xPos = std::cos(xSegment * 2.0 * pi) * std::sin(ySegment * pi);
            float yPos = std::cos(ySegment * pi);
            float zPos = std::sin(xSegment * 2.0f * pi) * std::sin(ySegment * pi);
            positions.push_back(glm::vec3(xPos, yPos, zPos));
            uv.push_back(glm::vec2(xSegment, ySegment));
            normals.push_back(glm::vec3(xPos, yPos, zPos));
        }
    }
    bool oddRow = false;
    for(unsigned int y=0; y<Y_SEGMENTS; y++){
        if(!oddRow){
            for(unsigned int x=0; x<=X_SEGMENTS; x++){
                indices.push_back(y * (X_SEGMENTS + 1) + x);
                indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
            }
        }
        else{
            for(int x=X_SEGMENTS; x>=0; x--){
                indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                indices.push_back(y * (X_SEGMENTS + 1) + x);
            }
        }
        oddRow = !oddRow;
    }
    indexCount = static_cast<unsigned int>(indices.size());
    std::vector<float> sphereVertices;
    for(unsigned int i=0; i<positions.size(); i++){
        sphereVertices.push_back(positions[i].x);
        sphereVertices.push_back(positions[i].y);
        sphereVertices.push_back(positions[i].z);
        if(uv.size() > 0){
            sphereVertices.push_back(uv[i].x);
            sphereVertices.push_back(uv[i].y);
        }
        if(normals.size() > 0){
            sphereVertices.push_back(normals[i].x);
            sphereVertices.push_back(normals[i].y);
            sphereVertices.push_back(normals[i].z);
        }
    }
    unsigned int sphereVAO, sphereVBO, sphereEBO;
    glGenVertexArrays(1, &sphereVAO);
    glGenBuffers(1, &sphereVBO);
    glGenBuffers(1, &sphereEBO);
    glBindVertexArray(sphereVAO);
    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(float), &sphereVertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(float), &indices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));

    unsigned int shaderProgram = createShader(vertexShaderSource, fragmentShaderSource);

    float skyVertices[] = {       
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f
    };
    unsigned int skyVBO, skyVAO;
    glGenVertexArrays(1, &skyVAO);
    glGenBuffers(1, &skyVBO);
    glBindVertexArray(skyVAO);

    glBindBuffer(GL_ARRAY_BUFFER, skyVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyVertices), &skyVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    
    unsigned int skyShaderProgram = createShader(cubemapVertexShaderSource, skyFragmentShaderSource);

    unsigned int cubemapShaderProgram = createShader(cubemapVertexShaderSource, cubemapFragmentShaderSource);
    
    unsigned int irradianceShaderProgram = createShader(cubemapVertexShaderSource, irradianceFragmentShaderSource);

    unsigned int prefilterShaderProgram = createShader(cubemapVertexShaderSource, prefilterFragmentShaderSource);

    unsigned int brdfShaderProgram = createShader(brdfVertexShaderSource, brdfFragmentShaderSource);

    unsigned int quadVAO;
    unsigned int quadVBO;
    float quadVertices[] = {
        -1.0f, 1.0f, 0.0f,  0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
        1.0f, 1.0f, 0.0f,  1.0f, 1.0f,
        1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
    };
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    std::pair<std::pair<unsigned int, unsigned int>, std::pair<unsigned int, unsigned int>> envMaps = HDRItoCubemap(environmentLocs[currentElement], cubemapShaderProgram, irradianceShaderProgram, prefilterShaderProgram, brdfShaderProgram, skyVAO, quadVAO);
    unsigned int envCubemap = envMaps.first.first;
    unsigned int irradianceMap = envMaps.first.second;
    unsigned int prefilterMap = envMaps.second.first;
    unsigned int brdfMap = envMaps.second.second;

    albedo = loadTexture(albedoLoc);
    metallic = loadTexture(metallicLoc);
    normal = loadTexture(normalLoc);
    roughness = loadTexture(roughnessLoc);
    ao = loadTexture(aoLoc);
    for(unsigned int i=0; i<sizeof(extraColors) / 12; i++){
        extraColors[i] = glm::vec3(1.0f);
    }
    unsigned int uiElements[sizeof(uiElementLocs)/30] = {};
    for(unsigned int i=0; i<sizeof(uiElementLocs)/30; i++){
        uiElements[i] = loadTexture(uiElementLocs[i]);
    }

    unsigned int spriteVBO, spriteVAO;
    float spriteVertices[] = {
        //vertex position,  texture coordinate
        0.0f, 1.0f,  0.0f, 1.0f,
        1.0f, 0.0f,  1.0f, 0.0f,
        0.0f, 0.0f,  0.0f, 0.0f, 
        0.0f, 1.0f,  0.0f, 1.0f,
        1.0f, 1.0f,  1.0f, 1.0f,
        1.0f, 0.0f,  1.0f, 0.0f
    };
    glGenVertexArrays(1, &spriteVAO);
    glGenBuffers(1, &spriteVBO);
    glBindBuffer(GL_ARRAY_BUFFER, spriteVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(spriteVertices), spriteVertices, GL_STATIC_DRAW);
    glBindVertexArray(spriteVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    unsigned int spriteProgram = createShader(uiVertexShaderSource, uiFragmentShaderSource);

    bool isCube = false;

    while(!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        if(selectingEnv){
            selectingEnv = false;
            envMaps = HDRItoCubemap(environmentLocs[currentElement], cubemapShaderProgram, irradianceShaderProgram, prefilterShaderProgram, brdfShaderProgram, skyVAO, quadVAO);
            envCubemap = envMaps.first.first;
            irradianceMap = envMaps.first.second;
            prefilterMap = envMaps.second.first;
        }
        else if(selectingShape){
            selectingShape = false;
            isCube = currentElement == 4;
        }
        else if(uploadingEnv){
            uploadingEnv = false;
            envMaps = HDRItoCubemap(uploadedEnv, cubemapShaderProgram, irradianceShaderProgram, prefilterShaderProgram, brdfShaderProgram, skyVAO, quadVAO);
            envCubemap = envMaps.first.first;
            irradianceMap = envMaps.first.second;
            prefilterMap = envMaps.second.first;
        }
        
        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        glUseProgram(shaderProgram);

        glm::mat4 projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);

        glm::mat4 view = glm::lookAt(camPos, glm::vec3(0.0,0.0,0.0), glm::vec3(0.0,1.0,0.0));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, &view[0][0]);

        glUniform3fv(glGetUniformLocation(shaderProgram, "camPos"), 1, &camPos[0]);

        if(isCube)
            glBindVertexArray(VAO);
        else
            glBindVertexArray(sphereVAO);

        glm::mat4 model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, brdfMap);
        glUniform1i(glGetUniformLocation(shaderProgram, "irradianceMap"), 0);
        glUniform1i(glGetUniformLocation(shaderProgram, "envCubemap"), 1);
        glUniform1i(glGetUniformLocation(shaderProgram, "prefilterMap"), 2);
        glUniform1i(glGetUniformLocation(shaderProgram, "brdfMap"), 3);

        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, albedo);
        glUniform1i(glGetUniformLocation(shaderProgram, "albedoMap"), 4);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, metallic);
        glUniform1i(glGetUniformLocation(shaderProgram, "metallicMap"), 5);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, normal);
        glUniform1i(glGetUniformLocation(shaderProgram, "normalMap"), 6);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, roughness);
        glUniform1i(glGetUniformLocation(shaderProgram, "roughnessMap"), 7);
        glActiveTexture(GL_TEXTURE8);
        glBindTexture(GL_TEXTURE_2D, ao);
        glUniform1i(glGetUniformLocation(shaderProgram, "aoMap"), 8);

        if(isCube)
            glDrawArrays(GL_TRIANGLES, 0, 36);
        else
            glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glDepthFunc(GL_LEQUAL); 
        glDepthRange(1.0f, 1.0f);
        glUseProgram(skyShaderProgram);
        view = glm::mat4(glm::mat3(view));
        glUniformMatrix4fv(glGetUniformLocation(skyShaderProgram, "view"), 1, GL_FALSE, &view[0][0]); 
        glUniformMatrix4fv(glGetUniformLocation(skyShaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);
        glBindVertexArray(skyVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthRange(0.0f, 1.0f);  
        glDepthFunc(GL_LESS);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glm::mat4 orthoProj = glm::ortho(0.0f, (float)SCR_WIDTH, (float)SCR_HEIGHT, 0.0f, -1.0f, 1.0f);
        glm::mat4 spriteModel = glm::mat4(1.0f);
        glUseProgram(spriteProgram);
        spriteModel = glm::translate(spriteModel, glm::vec3(10.0f, (float)SCR_HEIGHT - 60.0f, 0.0f));
        spriteModel = glm::scale(spriteModel, glm::vec3(50.0f, 50.0f, 1.0f));
        glUniformMatrix4fv(glGetUniformLocation(spriteProgram, "model"), 1, GL_FALSE, &spriteModel[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(spriteProgram, "projection"), 1, GL_FALSE, &orthoProj[0][0]);
        glUniform3fv(glGetUniformLocation(spriteProgram, "extraColor"), 1, &extraColors[0][0]);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, uiElements[0]);
        glBindVertexArray(spriteVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        spriteModel = glm::translate(spriteModel, glm::vec3(1.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(spriteProgram, "model"), 1, GL_FALSE, &spriteModel[0][0]);
        glUniform3fv(glGetUniformLocation(spriteProgram, "extraColor"), 1, &extraColors[1][0]);
        glBindTexture(GL_TEXTURE_2D, uiElements[1]);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        spriteModel = glm::translate(spriteModel, glm::vec3(1.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(spriteProgram, "model"), 1, GL_FALSE, &spriteModel[0][0]);
        glUniform3fv(glGetUniformLocation(spriteProgram, "extraColor"), 1, &extraColors[2][0]);
        glBindTexture(GL_TEXTURE_2D, uiElements[2]);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        spriteModel = glm::translate(spriteModel, glm::vec3(1.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(spriteProgram, "model"), 1, GL_FALSE, &spriteModel[0][0]);
        glUniform3fv(glGetUniformLocation(spriteProgram, "extraColor"), 1, &extraColors[3][0]);
        glBindTexture(GL_TEXTURE_2D, uiElements[3]);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        spriteModel = glm::translate(spriteModel, glm::vec3(1.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(spriteProgram, "model"), 1, GL_FALSE, &spriteModel[0][0]);
        glUniform3fv(glGetUniformLocation(spriteProgram, "extraColor"), 1, &extraColors[6][0]);
        glBindTexture(GL_TEXTURE_2D, uiElements[6]);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        spriteModel = glm::mat4(1.0f);
        spriteModel = glm::translate(spriteModel, glm::vec3((float)SCR_WIDTH - 60.0f, (float)SCR_HEIGHT - 60.0f, 0.0f));
        spriteModel = glm::scale(spriteModel, glm::vec3(50.0f, 50.0f, 1.0f));
        glUniformMatrix4fv(glGetUniformLocation(spriteProgram, "model"), 1, GL_FALSE, &spriteModel[0][0]);
        glUniform3fv(glGetUniformLocation(spriteProgram, "extraColor"), 1, &extraColors[4][0]);
        glBindTexture(GL_TEXTURE_2D, uiElements[4]);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        spriteModel = glm::translate(spriteModel, glm::vec3(-1.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(spriteProgram, "model"), 1, GL_FALSE, &spriteModel[0][0]);
        glUniform3fv(glGetUniformLocation(spriteProgram, "extraColor"), 1, &extraColors[5][0]);
        glBindTexture(GL_TEXTURE_2D, uiElements[5]);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        spriteModel = glm::translate(spriteModel, glm::vec3(-1.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(spriteProgram, "model"), 1, GL_FALSE, &spriteModel[0][0]);
        glUniform3fv(glGetUniformLocation(spriteProgram, "extraColor"), 1, &extraColors[7][0]);
        glBindTexture(GL_TEXTURE_2D, uiElements[7]);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        if(showMaterialUI){
            spriteModel = glm::mat4(1.0f);
            spriteModel = glm::translate(spriteModel, glm::vec3((float)SCR_WIDTH / 4.0f + 20.0f, (float)SCR_HEIGHT / 4.0f + 20.0f, 0.0f));
            spriteModel = glm::scale(spriteModel, glm::vec3(50.0f, 50.0f, 1.0f));
            glUniformMatrix4fv(glGetUniformLocation(spriteProgram, "model"), 1, GL_FALSE, &spriteModel[0][0]);
            glUniform3fv(glGetUniformLocation(spriteProgram, "extraColor"), 1, &extraColors[9][0]);
            glBindTexture(GL_TEXTURE_2D, albedo);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            spriteModel = glm::translate(spriteModel, glm::vec3(0.0f, 1.2f, 0.0f));
            glUniformMatrix4fv(glGetUniformLocation(spriteProgram, "model"), 1, GL_FALSE, &spriteModel[0][0]);
            glUniform3fv(glGetUniformLocation(spriteProgram, "extraColor"), 1, &extraColors[10][0]);
            glBindTexture(GL_TEXTURE_2D, metallic);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            spriteModel = glm::translate(spriteModel, glm::vec3(0.0f, 1.2f, 0.0f));
            glUniformMatrix4fv(glGetUniformLocation(spriteProgram, "model"), 1, GL_FALSE, &spriteModel[0][0]);
            glUniform3fv(glGetUniformLocation(spriteProgram, "extraColor"), 1, &extraColors[11][0]);
            glBindTexture(GL_TEXTURE_2D, normal);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            spriteModel = glm::translate(spriteModel, glm::vec3(0.0f, 1.2f, 0.0f));
            glUniformMatrix4fv(glGetUniformLocation(spriteProgram, "model"), 1, GL_FALSE, &spriteModel[0][0]);
            glUniform3fv(glGetUniformLocation(spriteProgram, "extraColor"), 1, &extraColors[12][0]);
            glBindTexture(GL_TEXTURE_2D, roughness);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            spriteModel = glm::translate(spriteModel, glm::vec3(0.0f, 1.2f, 0.0f));
            glUniformMatrix4fv(glGetUniformLocation(spriteProgram, "model"), 1, GL_FALSE, &spriteModel[0][0]);
            glUniform3fv(glGetUniformLocation(spriteProgram, "extraColor"), 1, &extraColors[13][0]);
            glBindTexture(GL_TEXTURE_2D, ao);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            spriteModel = glm::mat4(1.0f);
            spriteModel = glm::translate(spriteModel, glm::vec3((float)SCR_WIDTH / 4.0f + 80.0f, (float)SCR_HEIGHT / 4.0f + 70.0f, 0.0f));
            spriteModel = glm::scale(spriteModel, glm::vec3(160.0f, -50.0f, 1.0f));
            glUniformMatrix4fv(glGetUniformLocation(spriteProgram, "model"), 1, GL_FALSE, &spriteModel[0][0]);
            glUniform3fv(glGetUniformLocation(spriteProgram, "extraColor"), 1, &extraColors[9][0]);
            glBindTexture(GL_TEXTURE_2D, uiElements[9]);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            spriteModel = glm::translate(spriteModel, glm::vec3(0.0f, -1.2f, 0.0f));
            glUniformMatrix4fv(glGetUniformLocation(spriteProgram, "model"), 1, GL_FALSE, &spriteModel[0][0]);
            glUniform3fv(glGetUniformLocation(spriteProgram, "extraColor"), 1, &extraColors[10][0]);
            glBindTexture(GL_TEXTURE_2D, uiElements[10]);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            spriteModel = glm::translate(spriteModel, glm::vec3(0.0f, -1.2f, 0.0f));
            glUniformMatrix4fv(glGetUniformLocation(spriteProgram, "model"), 1, GL_FALSE, &spriteModel[0][0]);
            glUniform3fv(glGetUniformLocation(spriteProgram, "extraColor"), 1, &extraColors[11][0]);
            glBindTexture(GL_TEXTURE_2D, uiElements[11]);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            spriteModel = glm::translate(spriteModel, glm::vec3(0.0f, -1.2f, 0.0f));
            glUniformMatrix4fv(glGetUniformLocation(spriteProgram, "model"), 1, GL_FALSE, &spriteModel[0][0]);
            glUniform3fv(glGetUniformLocation(spriteProgram, "extraColor"), 1, &extraColors[12][0]);
            glBindTexture(GL_TEXTURE_2D, uiElements[12]);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            spriteModel = glm::translate(spriteModel, glm::vec3(0.0f, -1.2f, 0.0f));
            glUniformMatrix4fv(glGetUniformLocation(spriteProgram, "model"), 1, GL_FALSE, &spriteModel[0][0]);
            glUniform3fv(glGetUniformLocation(spriteProgram, "extraColor"), 1, &extraColors[13][0]);
            glBindTexture(GL_TEXTURE_2D, uiElements[13]);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            spriteModel = glm::mat4(1.0f);
            spriteModel = glm::translate(spriteModel, glm::vec3((float)SCR_WIDTH * 3.0f / 4.0f - 30.0f, (float)SCR_HEIGHT / 4.0f + 20.0f, 0.0f));
            spriteModel = glm::scale(spriteModel, glm::vec3(20.0f, 20.0f, 1.0f));
            glUniformMatrix4fv(glGetUniformLocation(spriteProgram, "model"), 1, GL_FALSE, &spriteModel[0][0]);
            glUniform3fv(glGetUniformLocation(spriteProgram, "extraColor"), 1, &extraColors[8][0]);
            glBindTexture(GL_TEXTURE_2D, uiElements[8]);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            spriteModel = glm::scale(spriteModel, glm::vec3(2.5f, -2.5f, 1.0f));
            spriteModel = glm::translate(spriteModel, glm::vec3(-0.6f, -1.5f, 0.0f));
            glUniformMatrix4fv(glGetUniformLocation(spriteProgram, "model"), 1, GL_FALSE, &spriteModel[0][0]);
            glUniform3fv(glGetUniformLocation(spriteProgram, "extraColor"), 1, &extraColors[14][0]);
            glBindTexture(GL_TEXTURE_2D, uiElements[14]);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            spriteModel = glm::mat4(1.0f);
            spriteModel = glm::translate(spriteModel, glm::vec3((float)SCR_WIDTH / 4.0f, (float)SCR_HEIGHT / 4.0f, 0.0f));
            spriteModel = glm::scale(spriteModel, glm::vec3((float)SCR_WIDTH / 2.0f, (float)SCR_HEIGHT / 1.8f, 1.0f));
            glUniformMatrix4fv(glGetUniformLocation(spriteProgram, "model"), 1, GL_FALSE, &spriteModel[0][0]);
            glUniform3fv(glGetUniformLocation(spriteProgram, "extraColor"), 1, &glm::vec3(1.0f, 1.0f, 1.0f)[0]);
            glBindTexture(GL_TEXTURE_2D, 0);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        glBindVertexArray(0);


        glfwSwapBuffers(window);
        glfwPollEvents();    
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &skyVAO);
    glDeleteBuffers(1, &skyVBO);
    
    glfwTerminate();
    return 0;
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
}
void uploadHDRI(){
    char* newEnvironment = OpenFileDialog();
    if(newEnvironment) {
        uploadedEnv = newEnvironment;
        uploadingEnv = true;
    }
}
void uploadTexture(int tex){
    char* newTexture = OpenFileDialogTex();
    if(!newTexture) return;
    if(tex==0) albedo = loadTexture(newTexture);
    else if(tex==1) metallic = loadTexture(newTexture);
    else if(tex==2) normal = loadTexture(newTexture);
    else if(tex==3) roughness = loadTexture(newTexture);
    else if(tex==4) ao = loadTexture(newTexture);
}
void uploadZip(){
    char* newZip = OpenFileDialogZip();
    if(newZip){
        unsigned int* textures = OpenZipFile(newZip);
        if(textures[0] != -1)
            albedo = textures[0];
        if(textures[1] != -1)
            metallic = textures[1];
        if(textures[2] != -1)
            normal = textures[2];
        if(textures[3] != -1)
            roughness = textures[3];
        if(textures[4] != -1)
            ao = textures[4];
    }
    else return;
}
void processInput(GLFWwindow *window){
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    else if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && highlightingUI){
        if(currentElement < 4)
            selectingEnv = true;
        else if(currentElement < 6)
            selectingShape = true;
        else if(currentElement == 6)
            uploadHDRI();
        else if(currentElement == 7)
            showMaterialUI = true;
        else if(currentElement == 8)
            showMaterialUI = false;
        else if(currentElement < 14)
            uploadTexture(currentElement - 9);
        else if(currentElement == 14)
            uploadZip();
    }
}
void hoverElement(int elementNum){
    for(unsigned int i=0; i<sizeof(extraColors)/12; i++){
        extraColors[i] = glm::vec3(1.0f);
    }
    if(elementNum != -1){
        extraColors[elementNum] = glm::vec3(1.3f, 1.3f, 1.6f);
        currentElement = elementNum;
    }
    return;
}
void mouseCallback(GLFWwindow* window, double xposIn, double yposIn){
    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
        lastX = xposIn;
        lastY = yposIn;
        firstMouse = true;
        if(showMaterialUI){
            if(yposIn > SCR_HEIGHT / 4.0f + 20.0f && yposIn < SCR_HEIGHT / 4.0f + 40.0f && xposIn > SCR_WIDTH * 3.0f / 4.0f - 30.0f && xposIn < SCR_WIDTH * 3.0f / 4.0f - 10.0f){
                hoverElement(8);
                highlightingUI = true;
            }
            else if(yposIn > SCR_HEIGHT * 29.0f / 36.0f || yposIn < SCR_HEIGHT / 4.0f || xposIn < SCR_WIDTH / 4.0f || xposIn > SCR_WIDTH * 3.0f / 4.0f){
                currentElement = 8;
                highlightingUI = true;
            }
            else if(yposIn < SCR_HEIGHT / 4.0f + 95.0f && yposIn > SCR_HEIGHT / 4.0f + 45.0f && xposIn > SCR_WIDTH * 3.0f / 4.0f - 60.0f && xposIn < SCR_WIDTH * 3.0f / 4.0f - 10.0f){
                hoverElement(14); highlightingUI = true;
            }
            else if(xposIn > SCR_WIDTH / 4.0f + 20.0f && xposIn < SCR_WIDTH / 4.0f + 70.0f){
                if(yposIn > SCR_HEIGHT / 4.0f + 20.0f && yposIn < SCR_HEIGHT / 4.0f + 70.0f) {
                    hoverElement(9); highlightingUI = true;
                }
                else if(yposIn > SCR_HEIGHT / 4.0f + 80.0f && yposIn < SCR_HEIGHT / 4.0f + 130.0f) {
                    hoverElement(10); highlightingUI = true;
                }
                else if(yposIn > SCR_HEIGHT / 4.0f + 140.0f && yposIn < SCR_HEIGHT / 4.0f + 190.0f) {
                    hoverElement(11); highlightingUI = true;
                }
                else if(yposIn > SCR_HEIGHT / 4.0f + 200.0f && yposIn < SCR_HEIGHT / 4.0f + 250.0f) {
                    hoverElement(12); highlightingUI = true;
                }
                else if(yposIn > SCR_HEIGHT / 4.0f + 260.0f && yposIn < SCR_HEIGHT / 4.0f + 310.0f) {
                    hoverElement(13); highlightingUI = true;
                }
                else if(highlightingUI) {hoverElement(-1); highlightingUI = false;}
            }
            else if(highlightingUI) {hoverElement(-1); highlightingUI = false;}
        }
        else if(yposIn > SCR_HEIGHT - 60.0f && yposIn < SCR_HEIGHT - 10.0f){
            if(xposIn > 10.0f && xposIn < 60.0f) {hoverElement(0); highlightingUI = true;}
            else if(xposIn > 60.0f && xposIn < 110.0f) {hoverElement(1); highlightingUI = true;}
            else if(xposIn > 110.0f && xposIn < 160.0f) {hoverElement(2); highlightingUI = true;}
            else if(xposIn > 160.0f && xposIn < 210.0f) {hoverElement(3); highlightingUI = true;}
            else if(xposIn > 210.0f && xposIn < 260.0f) {hoverElement(6); highlightingUI = true;}
            else if(xposIn > SCR_WIDTH - 60.0f && xposIn < SCR_WIDTH - 10.0f) {hoverElement(4); highlightingUI = true;}
            else if(xposIn > SCR_WIDTH - 110.0f && xposIn < SCR_WIDTH - 60.0f) {hoverElement(5); highlightingUI = true;}
            else if(xposIn > SCR_WIDTH - 160.0f && xposIn < SCR_WIDTH - 110.0f) {hoverElement(7); highlightingUI = true;}
            else if(highlightingUI) {hoverElement(-1); highlightingUI = false;}
        }
        else if(highlightingUI) {hoverElement(-1); highlightingUI = false;}
        return;
    }
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    if (firstMouse){
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    float xOffset = xpos - lastX;
    float yOffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;
    float sensitivity = 0.007f;
    xOffset *= sensitivity;
    yOffset *= sensitivity;
    yaw += xOffset;
    pitch += yOffset;
    if(pitch > 1.57)
        pitch =  1.57f;
    else if(pitch < -1.57f)
        pitch = -1.57f;
    float camX = sin(yaw) * radius;
    float camY = sin(pitch) * radius;
    float camZ = cos(yaw) * radius;
    camPos = glm::vec3(camX, camY, camZ);
}
