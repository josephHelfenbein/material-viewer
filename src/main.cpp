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
#include <fstream>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <OBJ_Loader.h>

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
char* SaveMatFileDialog(){
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
    ofn.lpstrFilter = "Material Files (*.mat)\0*.mat\0All Files (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = nullptr;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = nullptr;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;
    snprintf(filePath, sizeof(filePath), "newMat.mat");
    if(GetSaveFileNameA(&ofn)){
        std::string fileStr(filePath);
        if(fileStr.find(".mat") == std::string::npos)
            fileStr += ".mat";
        _chdir(currentDir);
        return _strdup(fileStr.c_str());
    }
    else {
        _chdir(currentDir);
        return nullptr;
    }
}
char* OpenFileDialogMaterial(){
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
    ofn.lpstrFilter = "Material Files (*.mat)\0*.mat\0All Files (*.*)\0*.*\0";
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
char* SaveMatFileDialog(){
    static char filePath[256];
    int argc = 0;
    char* argv[] = {nullptr};
    QApplication app(argc, argv);
    QApplication::setApplicationName("Material Viewer");
    QString defaultFileName = "newMat.mat";
    QString filename = QFileDialog::getSaveFileName(nullptr, "Save Material File", defaultFileName, "Material Files (*.mat);;All Files (*.*)");
    if(!filename.isEmpty()){
        std::string fileStr = filename.toStdString();
        if(fileStr.find(".mat") == std::string::npos)
            fileStr += ".mat";
        snprintf(filePath, sizeof(filePath), "%s", filename.toStdString().c_str());
        return filePath;
    }
    else return nullptr;
}
char* OpenFileDialogMaterial(){
    static char filePath[256];
    int argc = 0;
    char* argv[] = {nullptr};
    QApplication app(argc, argv);
    QApplication::setApplicationName("Material Viewer");
    QString filename = QFileDialog::getOpenFileName(nullptr, "Open Material File", "", "Material Files (*.mat);;All Files (*.*)");
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
std::string error = "";
float errorTime = 0.0f;
std::string tooltip = "";
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow *window);
char* getShaders(char file[]){
    FILE* shaderFile = fopen(file, "r");
    if(!shaderFile) {
        std::cerr<<"Error opening shader file at "<<file<<"\n";
        error = "Error opening shader file.";
        errorTime = 0.0f;
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
        std::cerr<<"HDR image failed to load at path "<<file<<std::endl;
        error = "HDR image failed to load.";
        errorTime = 0.0f;
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
        std::cerr<<"Texture image failed to load at path "<<file<<std::endl;
        error = "Texture image failed to load.";
        errorTime = 0.0f;
        stbi_image_free(data);
    }
    return textureID;
}
struct ImageData{
    unsigned char* data;
    int width;
    int height;
    int channels;
    size_t dataSize;
};
unsigned int loadTexture(ImageData* imageData){
    unsigned int textureID;
    glGenTextures(2, &textureID);
    GLenum format;
    int nrComponents = imageData->channels;
    if(nrComponents == 1) format = GL_RED;
    else if(nrComponents == 3) format = GL_RGB;
    else if(nrComponents == 4) format = GL_RGBA;
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, imageData->width, imageData->height, 0, format, GL_UNSIGNED_BYTE, imageData->data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
            error = "Could not find one or more textures.";
            errorTime = 0.0f;
            continue;
        }
        FILE* output = fopen(filenames[i], "wb");
        if(!output){
            error = "Could not load one or more textures";
            errorTime = 0.0f;
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
        std::cerr<<"A vertex shader compilation failed.\n"<<infoLog<<std::endl;
        error = "A vertex shader compilation failed.";
        errorTime = 0.0f;
    }
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragSource, nullptr);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr<<"A fragment shader compilation failed.\n"<<infoLog<<std::endl;
        error = "A fragment shader compilation failed.";
        errorTime = 0.0f;
    }
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success){
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "Linking shader program failed. \n"<<infoLog<<std::endl;
        error = "Linking shader program failed.";
        errorTime = 0.0f;
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
struct TextureMetadata{
    int width;
    int height;
    int channels;
    unsigned int dataSize;
};
ImageData* loadTextureData(unsigned int textureID){
    ImageData* imgData = new ImageData();
    glBindTexture(GL_TEXTURE_2D, textureID);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &imgData->width);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &imgData->height);
    GLint format;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &format);
    switch(format){
        case GL_RGB:
            imgData->channels = 3;
            break;
        case GL_RGBA:
            imgData->channels = 4;
            break;
        case GL_RED:
            imgData->channels = 1;
            break;
        default:
            std::cerr<<"Unsupported texture format."<<std::endl;
            error = "Unsupported texture format.";
            errorTime = 0.0f;
            delete imgData;
            return nullptr;
    }
    imgData->data = new unsigned char[imgData->width * imgData->height * imgData->channels];
    glGetTexImage(GL_TEXTURE_2D, 0, format, GL_UNSIGNED_BYTE, imgData->data);
    glBindTexture(GL_TEXTURE_2D, 0);
    return imgData;
}
bool writeCustomTextureFile(const char* outputPath, unsigned int albedo, unsigned int roughness, unsigned int normal, unsigned int metalness, unsigned int ao){
    ImageData* albedoData = loadTextureData(albedo);
    ImageData* roughnessData = loadTextureData(roughness);
    ImageData* normalData = loadTextureData(normal);
    ImageData* metalnessData = loadTextureData(metalness);
    ImageData* aoData = loadTextureData(ao);
    if (!albedoData || !roughnessData || !normalData || !metalnessData || !aoData) {
        std::cerr << "Failed to load one or more texture data." << std::endl;
        error = "Failed to load one or more texture data.";
        errorTime = 0.0f;
        return false;
    }
    TextureMetadata albedoMeta = { albedoData->width, albedoData->height, albedoData->channels, static_cast<unsigned int>(albedoData->width * albedoData->height * albedoData->channels) };
    TextureMetadata roughnessMeta = { roughnessData->width, roughnessData->height, roughnessData->channels, static_cast<unsigned int>(roughnessData->width * roughnessData->height * roughnessData->channels) };
    TextureMetadata normalMeta = { normalData->width, normalData->height, normalData->channels, static_cast<unsigned int>(normalData->width * normalData->height * normalData->channels) };
    TextureMetadata metalnessMeta = { metalnessData->width, metalnessData->height, metalnessData->channels, static_cast<unsigned int>(metalnessData->width * metalnessData->height * metalnessData->channels) };
    TextureMetadata aoMeta = { aoData->width, aoData->height, aoData->channels, static_cast<unsigned int>(aoData->width * aoData->height * aoData->channels) };
    std::ofstream outputFile(outputPath, std::ios::binary);
    if (!outputFile.is_open()) {
        std::cerr << "Failed to open output file " << outputPath << std::endl;
        error = "Failed to open output file.";
        errorTime = 0.0f;
        return false;
    }
    long long int magicNumber = 0x4D4154455249414C;
    outputFile.write(reinterpret_cast<const char*>(&magicNumber), sizeof(magicNumber));
    outputFile.write(reinterpret_cast<const char*>(&albedoMeta), sizeof(albedoMeta));
    outputFile.write(reinterpret_cast<const char*>(&roughnessMeta), sizeof(roughnessMeta));
    outputFile.write(reinterpret_cast<const char*>(&normalMeta), sizeof(normalMeta));
    outputFile.write(reinterpret_cast<const char*>(&metalnessMeta), sizeof(metalnessMeta));
    outputFile.write(reinterpret_cast<const char*>(&aoMeta), sizeof(aoMeta));
    outputFile.write(reinterpret_cast<const char*>(albedoData->data), albedoMeta.dataSize);
    outputFile.write(reinterpret_cast<const char*>(roughnessData->data), roughnessMeta.dataSize);
    outputFile.write(reinterpret_cast<const char*>(normalData->data), normalMeta.dataSize);
    outputFile.write(reinterpret_cast<const char*>(metalnessData->data), metalnessMeta.dataSize);
    outputFile.write(reinterpret_cast<const char*>(aoData->data), aoMeta.dataSize);
    delete[] albedoData->data;
    delete albedoData;
    delete[] roughnessData->data;
    delete roughnessData;
    delete[] normalData->data;
    delete normalData;
    delete[] metalnessData->data;
    delete metalnessData;
    delete[] aoData->data;
    delete aoData;
    outputFile.close();
    std::cout << "Material file written to " << outputPath << std::endl;
    return true;
}
std::pair<std::pair<std::pair<unsigned int, unsigned int>,std::pair<unsigned int, unsigned int>>, unsigned int> readCustomTextureFile(const char* inputPath){
    unsigned int albedoID;
    unsigned int roughnessID;
    unsigned int normalID;
    unsigned int metallicID;
    unsigned int aoID;
    std::ifstream inputFile(inputPath, std::ios::binary);
    if(!inputFile.is_open()){
        std::cerr << "Failed to load material file" << std::endl;
        error = "Failed to load material file.";
        errorTime = 0.0f;
        return {{{albedoID, roughnessID}, {normalID, metallicID}}, aoID};
    }
    long long int magicNumber;
    inputFile.read(reinterpret_cast<char*>(&magicNumber), sizeof(magicNumber));
    if(magicNumber != 0x4D4154455249414C) {
        std::cerr << "Invalid file format" << std::endl;
        error = "Invalid file format.";
        errorTime = 0.0f;
        return {{{albedoID, roughnessID}, {normalID, metallicID}}, aoID};
    }
    TextureMetadata albedoMeta, roughnessMeta, normalMeta, metalnessMeta, aoMeta;
    inputFile.read(reinterpret_cast<char*>(&albedoMeta), sizeof(albedoMeta));
    inputFile.read(reinterpret_cast<char*>(&roughnessMeta), sizeof(roughnessMeta));
    inputFile.read(reinterpret_cast<char*>(&normalMeta), sizeof(normalMeta));
    inputFile.read(reinterpret_cast<char*>(&metalnessMeta), sizeof(metalnessMeta));
    inputFile.read(reinterpret_cast<char*>(&aoMeta), sizeof(aoMeta));
    auto loadTextureFromStream = [&inputFile](const TextureMetadata& meta) -> ImageData* {
        ImageData* imageData = new ImageData();
        imageData->width = meta.width;
        imageData->height = meta.height;
        imageData->channels = meta.channels;
        imageData->data = new unsigned char[meta.dataSize];
        inputFile.read(reinterpret_cast<char*>(imageData->data), meta.dataSize);
        return imageData;
    };
    ImageData* albedoData = loadTextureFromStream(albedoMeta);
    ImageData* roughnessData = loadTextureFromStream(roughnessMeta);
    ImageData* normalData = loadTextureFromStream(normalMeta);
    ImageData* metalnessData = loadTextureFromStream(metalnessMeta);
    ImageData* aoData = loadTextureFromStream(aoMeta);
    albedoID = loadTexture(albedoData);
    roughnessID = loadTexture(roughnessData);
    normalID = loadTexture(normalData);
    metallicID = loadTexture(metalnessData);
    aoID = loadTexture(aoData);
    delete[] albedoData->data;
    delete albedoData;
    delete[] roughnessData->data;
    delete roughnessData;
    delete[] normalData->data;
    delete normalData;
    delete[] metalnessData->data;
    delete metalnessData;
    delete[] aoData->data;
    delete aoData;
    inputFile.close();
    return {{{albedoID, roughnessID}, {normalID, metallicID}}, aoID};
}
struct Character{
    unsigned int textureID;
    glm::ivec2 size;
    glm::ivec2 bearing;
    unsigned int advance;
};
std::map<char, Character> Characters;
void prepareCharacters(){
    FT_Library ft;
    if(FT_Init_FreeType(&ft)) {
        std::cerr<<"Could not initialize FreeType"<<std::endl;
        error = "Could not initialize FreeType.";
        errorTime = 0.0f;
        FT_Done_FreeType(ft);
        return;
    }
    FT_Face face;
    if(FT_New_Face(ft, "./src/resources/Roboto-Regular.ttf", 0, &face)){
        std::cerr<<"Could not load font"<<std::endl;
        error = "Could not load font.";
        errorTime = 0.0f;
        FT_Done_Face(face);
        FT_Done_FreeType(ft);
        return;
    }
    FT_Set_Pixel_Sizes(face, 0, 48);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    for(unsigned char c = 0; c < 128; c++){
        if(FT_Load_Char(face, c, FT_LOAD_RENDER)){
            std::cerr<<"Failed to load gylph: "<<c<<std::endl;
            continue;
        }
        unsigned int textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        Character character = {textureID, glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows), glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top), static_cast<unsigned int>(face->glyph->advance.x)};
        Characters.insert(std::pair<char, Character>(c, character));
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}
void RenderText(unsigned int shader, unsigned int VAO, unsigned int VBO, std::string text, float x, float y, float scale, glm::vec3 color){
    glUseProgram(shader);
    glm::mat4 textProj = glm::ortho(0.0f, (float)SCR_WIDTH, 0.0f, (float)SCR_HEIGHT);
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, &textProj[0][0]);
    glUniform3f(glGetUniformLocation(shader, "textColor"), color.x, color.y, color.z);
    glm::mat4 spriteModel = glm::mat4(1.0f);
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, &spriteModel[0][0]);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);
    std::string::const_iterator c;
    for(c = text.begin(); c != text.end(); c++){
        Character ch = Characters[*c];
        float xPos = x + ch.bearing.x * scale;
        float yPos = y - (ch.size.y - ch.bearing.y) * scale;
        float w = ch.size.x * scale;
        float h = ch.size.y * scale;
        float vertices[6][4] = {
            {xPos, yPos + h, 0.0f, 0.0f},            
            {xPos, yPos, 0.0f, 1.0f},
            {xPos + w, yPos, 1.0f, 1.0f},
            {xPos, yPos + h, 0.0f, 0.0f},
            {xPos + w, yPos, 1.0f, 1.0f},
            {xPos + w, yPos + h, 1.0f, 0.0f}    
        };
        glBindTexture(GL_TEXTURE_2D, ch.textureID);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        x += (ch.advance >> 6) * scale;
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
void loadModel(char filePath[], unsigned int &VAO, unsigned int &VBO, unsigned int &EBO, unsigned int &indexCount){
    objl::Loader loader;
    if(!loader.LoadFile(filePath)){
        std::cerr<<"Failed to load OBJ file"<<std::endl;
        error = "Failed to load OBJ file";
        return;
    }
    objl::Mesh mesh = loader.LoadedMeshes[0];
    glm::vec3 minBound(FLT_MAX, FLT_MAX, FLT_MAX);
    glm::vec3 maxBound(-FLT_MAX, -FLT_MAX, -FLT_MAX);
    for(int i=0; i<mesh.Vertices.size(); i++){
        const auto& vertex = mesh.Vertices[i];
        minBound = glm::min(minBound, glm::vec3(vertex.Position.X, vertex.Position.Y, vertex.Position.Z));
        maxBound = glm::max(maxBound, glm::vec3(vertex.Position.X, vertex.Position.Y, vertex.Position.Z));
    }
    glm::vec3 center = (minBound + maxBound) * 0.5f;
    float maxDistance = 0.0f;
    for (const auto& vertex : mesh.Vertices) {
        glm::vec3 vertexPos(vertex.Position.X, vertex.Position.Y, vertex.Position.Z);
        float distance = glm::length(vertexPos - center);
        maxDistance = std::max(maxDistance, distance);
    }
    glm::vec3 size = maxBound - minBound;
    float aspectRatio = size.x / size.y;
    float scaleFactor;
    if(aspectRatio < 0.1f || aspectRatio > 10.0f) scaleFactor = 1.8f / maxDistance;
    else scaleFactor = 1.8f / (maxDistance * 1.5f);
    std::vector<float> vertices;
    std::vector<unsigned int> indices = mesh.Indices;
    for(int i=0; i<mesh.Vertices.size(); i++){
        glm::vec3 scaledPosition(
            (mesh.Vertices[i].Position.X - center.x) * scaleFactor,
            (mesh.Vertices[i].Position.Y - center.y) * scaleFactor,
            (mesh.Vertices[i].Position.Z - center.z) * scaleFactor
        );
        vertices.push_back(scaledPosition.x);
        vertices.push_back(scaledPosition.y);
        vertices.push_back(scaledPosition.z);
        vertices.push_back(mesh.Vertices[i].TextureCoordinate.X);
        vertices.push_back(mesh.Vertices[i].TextureCoordinate.Y);
        vertices.push_back(mesh.Vertices[i].Normal.X);
        vertices.push_back(mesh.Vertices[i].Normal.Y);
        vertices.push_back(mesh.Vertices[i].Normal.Z);
    }
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    indexCount = indices.size();
    return;
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
char textFragmentLoc[] = "./src/shaders/text.frag";
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
const char* textFragmentShaderSource = getShaders(textFragmentLoc);
char defaultMatLoc[] = "./src/material/stainless_steel.mat";
char albedoLoc[] = "./src/material/albedoNot.png";
char aoLoc[] = "./src/material/aoNot.png";
char metallicLoc[] = "./src/material/metallicNot.png";
char normalLoc[] = "./src/material/normalNot.png";
char roughnessLoc[] = "./src/material/roughnessNot.png";
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
    "./src/ui/img_ui15.png",
    "./src/ui/img_ui16.png",
    "./src/ui/img_ui17.png",
    "./src/ui/img_ui18.png"
};
glm::vec3 extraColors[sizeof(uiElementLocs)/30];
char cubeLoc[] = "./src/models/cube.obj";
char sphereLoc[] = "./src/models/sphere.obj";
char teapotLoc[] = "./src/models/teapot.obj";
char backgroundLoc[] = "./src/resources/background.png";
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
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window;
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Material Viewer (Alpha)", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Failed to open GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouseCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    unsigned int cubeVAO, cubeVBO, cubeEBO, cubeIndexCount;
    loadModel(cubeLoc, cubeVAO, cubeVBO, cubeEBO, cubeIndexCount);

    unsigned int sphereVAO, sphereVBO, sphereEBO, sphereIndexCount;
    loadModel(sphereLoc, sphereVAO, sphereVBO, sphereEBO, sphereIndexCount);

    unsigned int teapotVAO, teapotVBO, teapotEBO, teapotIndexCount;
    loadModel(teapotLoc, teapotVAO, teapotVBO, teapotEBO, teapotIndexCount);
    
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

    std::pair<std::pair<std::pair<unsigned int, unsigned int>,std::pair<unsigned int, unsigned int>>, unsigned int> defaultMat = readCustomTextureFile(defaultMatLoc);
    albedo = defaultMat.first.first.first;
    roughness = defaultMat.first.first.second;
    normal = defaultMat.first.second.first;
    metallic = defaultMat.first.second.second;
    ao = defaultMat.second;

    for(unsigned int i=0; i<sizeof(extraColors) / 12; i++){
        extraColors[i] = glm::vec3(1.0f);
    }
    unsigned int uiElements[sizeof(uiElementLocs)/30] = {};
    for(unsigned int i=0; i<sizeof(uiElementLocs)/30; i++){
        uiElements[i] = loadTexture(uiElementLocs[i]);
    }
    unsigned int uiBackground = loadTexture(backgroundLoc);

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
    unsigned int textProgram = createShader(uiVertexShaderSource, textFragmentShaderSource);
    unsigned int textVAO, textVBO;
    glGenVertexArrays(1, &textVAO);
    glGenBuffers(1, &textVBO);
    glBindVertexArray(textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    prepareCharacters();

    int shapeNum = 0;
    float tooltipTime = 0.0f;

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
            uploadedEnv = nullptr;
        }
        else if(selectingShape){
            selectingShape = false;
            if(currentElement == 4) shapeNum = 1;
            else if(currentElement == 5) shapeNum = 0;
            else if(currentElement == 17) shapeNum = 2;
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

        if(shapeNum == 1)
            glBindVertexArray(cubeVAO);
        else if(shapeNum == 0)
            glBindVertexArray(sphereVAO);
        else if(shapeNum == 2)
            glBindVertexArray(teapotVAO);

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

        if(shapeNum == 1)
            glDrawElements(GL_TRIANGLES, cubeIndexCount, GL_UNSIGNED_INT, 0);
        else if(shapeNum == 0)
            glDrawElements(GL_TRIANGLES, sphereIndexCount, GL_UNSIGNED_INT, 0);
        else if(shapeNum == 2)
            glDrawElements(GL_TRIANGLES, teapotIndexCount, GL_UNSIGNED_INT, 0);
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
        
        if(error != ""){
            RenderText(textProgram, textVAO, textVBO, error, 10.0f, (float)SCR_HEIGHT - 20.0f, 0.35f, glm::vec3(0.8f, 0.2f, 0.2f));
            errorTime += deltaTime;
            if(errorTime >= 7.0f) error = "";
        }
        else errorTime = 0.0f;
        if(tooltip != ""){
            tooltipTime += deltaTime;
            if(tooltipTime >= 0.5f){
                RenderText(textProgram, textVAO, textVBO, tooltip, lastX, (float)SCR_HEIGHT - lastY, 0.35f, glm::vec3(0.8f, 0.8f, 0.8f));
                glUseProgram(spriteProgram);
                glm::mat4 spriteModel = glm::mat4(1.0f);
                spriteModel = glm::translate(spriteModel, glm::vec3(lastX - 2.5f, (float)SCR_HEIGHT - lastY - 2.5f, 0.0f));
                spriteModel = glm::scale(spriteModel, glm::vec3(8.5f * tooltip.size(), 20.0f, 1.0f));
                glm::mat4 textProj = glm::ortho(0.0f, (float)SCR_WIDTH, 0.0f, (float)SCR_HEIGHT);
                glUniformMatrix4fv(glGetUniformLocation(spriteProgram, "model"), 1, GL_FALSE, &spriteModel[0][0]);
                glUniformMatrix4fv(glGetUniformLocation(spriteProgram, "projection"), 1, GL_FALSE, &textProj[0][0]);
                glUniform3fv(glGetUniformLocation(spriteProgram, "extraColor"), 1, &glm::vec3(0.5f, 0.5f, 0.5f)[0]);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, 0);
                glBindVertexArray(spriteVAO);
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
        }
        else tooltipTime = 0.0f;
        glm::mat4 orthoProj = glm::ortho(0.0f, (float)SCR_WIDTH, (float)SCR_HEIGHT, 0.0f, -1.0f, 1.0f);
        glm::mat4 spriteModel = glm::mat4(1.0f);
        glUseProgram(spriteProgram);
        spriteModel = glm::translate(spriteModel, glm::vec3(10.0f, (float)SCR_HEIGHT - 10.0f, 0.0f));
        spriteModel = glm::scale(spriteModel, glm::vec3(50.0f, -50.0f, 1.0f));
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
        spriteModel = glm::translate(spriteModel, glm::vec3((float)SCR_WIDTH - 60.0f, (float)SCR_HEIGHT - 10.0f, 0.0f));
        spriteModel = glm::scale(spriteModel, glm::vec3(50.0f, -50.0f, 1.0f));
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
        glUniform3fv(glGetUniformLocation(spriteProgram, "extraColor"), 1, &extraColors[17][0]);
        glBindTexture(GL_TEXTURE_2D, uiElements[17]);
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
            RenderText(textProgram, textVAO, textVBO, "Base Color", (float)SCR_WIDTH / 4.0f + 80.0f, (float)SCR_HEIGHT * 3.0f / 4.0f - 50.0f, 0.4f, glm::vec3(0.8f, 0.8f, 0.8f) * extraColors[9]);
            RenderText(textProgram, textVAO, textVBO, "Metalness", (float)SCR_WIDTH / 4.0f + 80.0f, (float)SCR_HEIGHT * 3.0f / 4.0f - 110.0f, 0.4f, glm::vec3(0.8f, 0.8f, 0.8f) * extraColors[10]);
            RenderText(textProgram, textVAO, textVBO, "Normal Map", (float)SCR_WIDTH / 4.0f + 80.0f, (float)SCR_HEIGHT * 3.0f / 4.0f - 170.0f, 0.4f, glm::vec3(0.8f, 0.8f, 0.8f) * extraColors[11]);
            RenderText(textProgram, textVAO, textVBO, "Roughness", (float)SCR_WIDTH / 4.0f + 80.0f, (float)SCR_HEIGHT * 3.0f / 4.0f - 230.0f, 0.4f, glm::vec3(0.8f, 0.8f, 0.8f) * extraColors[12]);
            RenderText(textProgram, textVAO, textVBO, "Ambient Occlusion", (float)SCR_WIDTH / 4.0f + 80.0f, (float)SCR_HEIGHT * 3.0f / 4.0f - 290.0f, 0.4f, glm::vec3(0.8f, 0.8f, 0.8f) * extraColors[13]);
            glUseProgram(spriteProgram);
            glBindVertexArray(spriteVAO);
            glUniformMatrix4fv(glGetUniformLocation(spriteProgram, "projection"), 1, GL_FALSE, &orthoProj[0][0]);
            glActiveTexture(GL_TEXTURE0);
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
            spriteModel = glm::translate(spriteModel, glm::vec3(0.0f, -1.1f, 0.0f));
            glUniformMatrix4fv(glGetUniformLocation(spriteProgram, "model"), 1, GL_FALSE, &spriteModel[0][0]);
            glUniform3fv(glGetUniformLocation(spriteProgram, "extraColor"), 1, &extraColors[15][0]);
            glBindTexture(GL_TEXTURE_2D, uiElements[15]);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            spriteModel = glm::translate(spriteModel, glm::vec3(0.0f, -1.1f, 0.0f));
            glUniformMatrix4fv(glGetUniformLocation(spriteProgram, "model"), 1, GL_FALSE, &spriteModel[0][0]);
            glUniform3fv(glGetUniformLocation(spriteProgram, "extraColor"), 1, &extraColors[16][0]);
            glBindTexture(GL_TEXTURE_2D, uiElements[16]);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            spriteModel = glm::mat4(1.0f);
            spriteModel = glm::translate(spriteModel, glm::vec3((float)SCR_WIDTH * 0.085f, (float)SCR_HEIGHT * 0.95f, 0.0f));
            spriteModel = glm::scale(spriteModel, glm::vec3((float)SCR_WIDTH / 1.2f, -(float)SCR_HEIGHT / 1.2f, 1.0f));
            glUniformMatrix4fv(glGetUniformLocation(spriteProgram, "model"), 1, GL_FALSE, &spriteModel[0][0]);
            glUniform3fv(glGetUniformLocation(spriteProgram, "extraColor"), 1, &glm::vec3(1.0f, 1.0f, 1.0f)[0]);
            glBindTexture(GL_TEXTURE_2D, uiBackground);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        glBindVertexArray(0);


        glfwSwapBuffers(window);
        glfwPollEvents();    
    }

    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &cubeEBO);
    glDeleteVertexArrays(1, &sphereVAO);
    glDeleteBuffers(1, &sphereVBO);
    glDeleteBuffers(1, &sphereEBO);
    glDeleteVertexArrays(1, &teapotVAO);
    glDeleteBuffers(1, &teapotVBO);
    glDeleteBuffers(1, &teapotEBO);
    glDeleteVertexArrays(1, &skyVAO);
    glDeleteBuffers(1, &skyVBO);
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
    glDeleteVertexArrays(1, &spriteVAO);
    glDeleteBuffers(1, &spriteVBO);
    glDeleteVertexArrays(1, &textVAO);
    glDeleteBuffers(1, &textVBO);
    
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
        else albedo = loadTexture(albedoLoc);
        if(textures[1] != -1)
            metallic = textures[1];
        else metallic = loadTexture(metallicLoc);
        if(textures[2] != -1)
            normal = textures[2];
        else normal = loadTexture(normalLoc);
        if(textures[3] != -1)
            roughness = textures[3];
        else roughness = loadTexture(roughnessLoc);
        if(textures[4] != -1)
            ao = textures[4];
        else ao = loadTexture(aoLoc);
    }
}
void saveToFile(){
    char* matPath = SaveMatFileDialog();
    if(matPath) writeCustomTextureFile(matPath, albedo, roughness, normal, metallic, ao);
}
void uploadMat(){
    char* matPath = OpenFileDialogMaterial();
    if(matPath){
        std::pair<std::pair<std::pair<unsigned int, unsigned int>,std::pair<unsigned int, unsigned int>>, unsigned int> result = readCustomTextureFile(matPath);
        if(result.first.first.first != 0){
            albedo = result.first.first.first;
            roughness = result.first.first.second;
            normal = result.first.second.first;
            metallic = result.first.second.second;
            ao = result.second;
        }
    }
}
void processInput(GLFWwindow *window){
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    else if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && highlightingUI){
        if(currentElement < 4)
            selectingEnv = true;
        else if(currentElement < 6 || currentElement == 17)
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
        else if(currentElement == 15)
            saveToFile();
        else if(currentElement == 16)
            uploadMat();
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
    else tooltip = "";
    return;
}
void mouseCallback(GLFWwindow* window, double xposIn, double yposIn){
    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
        lastX = xposIn;
        lastY = yposIn;
        firstMouse = true;
        if(showMaterialUI){
            if(yposIn > SCR_HEIGHT / 4.0f + 20.0f && yposIn < SCR_HEIGHT / 4.0f + 40.0f && xposIn > SCR_WIDTH * 3.0f / 4.0f - 30.0f && xposIn < SCR_WIDTH * 3.0f / 4.0f - 10.0f){
                hoverElement(8); highlightingUI = true;
            }
            else if(yposIn > SCR_HEIGHT * 29.0f / 36.0f || yposIn < SCR_HEIGHT / 4.0f || xposIn < SCR_WIDTH / 4.0f || xposIn > SCR_WIDTH * 3.0f / 4.0f){
                hoverElement(-1); currentElement = 8; highlightingUI = true;
            }
            else if(yposIn < SCR_HEIGHT / 4.0f + 95.0f && yposIn > SCR_HEIGHT / 4.0f + 45.0f && xposIn > SCR_WIDTH * 3.0f / 4.0f - 60.0f && xposIn < SCR_WIDTH * 3.0f / 4.0f - 10.0f){
                hoverElement(14); highlightingUI = true; tooltip = "Upload .zip";
            }
            else if(yposIn < SCR_HEIGHT / 4.0f + 150.0f && yposIn > SCR_HEIGHT / 4.0f + 100.0f && xposIn > SCR_WIDTH * 3.0f / 4.0f - 60.0f && xposIn < SCR_WIDTH * 3.0f / 4.0f - 10.0f){
                hoverElement(15); highlightingUI = true; tooltip = "Save .mat";
            }
            else if(yposIn < SCR_HEIGHT / 4.0f + 205.0f && yposIn > SCR_HEIGHT / 4.0f + 155.0f && xposIn > SCR_WIDTH * 3.0f / 4.0f - 60.0f && xposIn < SCR_WIDTH * 3.0f / 4.0f - 10.0f){
                hoverElement(16); highlightingUI = true; tooltip = "Upload .mat";
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
            else if(xposIn > 210.0f && xposIn < 260.0f) {hoverElement(6); highlightingUI = true; tooltip = "Upload HDRI environment";}
            else if(xposIn > SCR_WIDTH - 60.0f && xposIn < SCR_WIDTH - 10.0f) {hoverElement(4); highlightingUI = true;}
            else if(xposIn > SCR_WIDTH - 110.0f && xposIn < SCR_WIDTH - 60.0f) {hoverElement(5); highlightingUI = true;}
            else if(xposIn > SCR_WIDTH - 160.0f && xposIn < SCR_WIDTH - 110.0f) {hoverElement(17); highlightingUI = true;}
            else if(xposIn > SCR_WIDTH - 210.0f && xposIn < SCR_WIDTH - 160.0f) {hoverElement(7); highlightingUI = true; tooltip = "Change material";}
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
