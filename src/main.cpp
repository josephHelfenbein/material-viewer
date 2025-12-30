#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <memory>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>
#include <zip.h>
#include <fstream>
#include <sstream>
#include <freetype/include/ft2build.h>
#include FT_FREETYPE_H
#include <OBJ-Loader/Source/OBJ_Loader.h>
#include <map>
#include <mutex>
#include <thread>
#include <zstd/lib/zstd.h>
#include <unordered_map>
#include <unordered_set>
#include <regex>
#include <algorithm>
#include <cstdlib>
#include "embedded_resources.h"

#if defined(__APPLE__)
#include <mach-o/dyld.h>
#include <limits.h>
#endif

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
char* SaveZipFileDialog(){
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
    ofn.lpstrFileTitle = nullptr;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = nullptr;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;
    snprintf(filePath, sizeof(filePath), "textures.zip");
    if(GetSaveFileNameA(&ofn)){
        std::string fileStr(filePath);
        if(fileStr.find(".zip") == std::string::npos)
            fileStr += ".zip";
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
char* SaveZipFileDialog(){
    static char filePath[256];
    int argc = 0;
    char* argv[] = {nullptr};
    QApplication app(argc, argv);
    QApplication::setApplicationName("Material Viewer");
    QString defaultFileName = "textures.zip";
    QString filename = QFileDialog::getSaveFileName(nullptr, "Save Zip File", defaultFileName, "Zip Files (*.zip);;All Files (*.*)");
    if(!filename.isEmpty()){
        std::string fileStr = filename.toStdString();
        if(fileStr.find(".zip") == std::string::npos)
            fileStr += ".zip";
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
float contentScale = 1.0f;

inline float ui(float value) { return value * contentScale; }

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
bool isMetallic = true;
float errorTime = 0.0f;
std::string tooltip = "";
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow *window);

std::string toResourcePath(const std::string& path) {
    std::string result = path;
    for (char& c : result) {
        if (c == '\\') c = '/';
    }
    size_t srcPos = result.find("/src/");
    if (srcPos != std::string::npos) {
        result = result.substr(srcPos + 5);
    } else {
        srcPos = result.find("src/");
        if (srcPos == 0) {
            result = result.substr(4);
        }
    }
    if (!result.empty() && result[0] == '/') {
        result = result.substr(1);
    }
    return result;
}

char* getShaders(std::string file){
    std::string resourcePath = toResourcePath(file);
    const auto* resource = EmbeddedResources::getResource(resourcePath);
    if (resource) {
        char* shader = (char*)malloc(*resource->size + 1);
        memcpy(shader, resource->data, *resource->size);
        shader[*resource->size] = '\0';
        return shader;
    }
    FILE* shaderFile = fopen(file.c_str(), "r");
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
unsigned int loadEnv(std::string file){
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrComponents;
    float* data = nullptr;
    std::string resourcePath = toResourcePath(file);
    const auto* resource = EmbeddedResources::getResource(resourcePath);
    if (resource) {
        data = stbi_loadf_from_memory(resource->data, static_cast<int>(*resource->size), &width, &height, &nrComponents, 0);
    } else {
        data = stbi_loadf(file.c_str(), &width, &height, &nrComponents, 0);
    }
    
    unsigned int hdrTexture;
    if(data){
        glGenTextures(1, &hdrTexture);
        glBindTexture(GL_TEXTURE_2D, hdrTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
    }
    else{
        std::cerr<<"HDR image failed to load at path "<<file<<std::endl;
        error = "HDR image failed to load.";
        errorTime = 0.0f;
    }
    return hdrTexture;
}
unsigned int loadTexture(std::string file){
    unsigned int textureID;
    glGenTextures(1, &textureID);
    int width, height, nrComponents;
    unsigned char* data = nullptr;
    
    std::string resourcePath = toResourcePath(file);
    const auto* resource = EmbeddedResources::getResource(resourcePath);
    if (resource) {
        data = stbi_load_from_memory(resource->data, static_cast<int>(*resource->size), &width, &height, &nrComponents, 0);
    } else {
        data = stbi_load(file.c_str(), &width, &height, &nrComponents, 0);
    }
    
    if(data){
        GLenum format = GL_RGB;
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
    }
    else{
        std::cerr<<"Texture image failed to load at path "<<file<<std::endl;
        error = "Texture image failed to load.";
        errorTime = 0.0f;
    }
    stbi_image_free(data);
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
    glGenTextures(1, &textureID);
    GLenum format = GL_RGB;
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
std::string replaceSlashes(const std::string& path) {
    std::regex slashRegex("/");
    return std::regex_replace(path, slashRegex, "\\");
}
std::string getAppPath(const char* relativePath){
    std::string pathBuffer;
    std::string relativePathStr(relativePath); 
#if defined(_WIN32) || defined(__CYGWIN__)
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);
    std::string exeDir(path);
    exeDir = exeDir.substr(0, exeDir.find_last_of("\\/"));
    std::string modifiedRelativePath = replaceSlashes(relativePathStr);
    std::string parentDir = exeDir.substr(0, exeDir.find_last_of("\\/"));
    std::string srcInParent = parentDir + "\\src";
    std::string srcInExeDir = exeDir + "\\src";
    if (GetFileAttributesA(srcInExeDir.c_str()) != INVALID_FILE_ATTRIBUTES) {
        pathBuffer = srcInExeDir + replaceSlashes(relativePathStr);
    }
    else if (GetFileAttributesA(srcInParent.c_str()) != INVALID_FILE_ATTRIBUTES) {
        pathBuffer = srcInParent + replaceSlashes(relativePathStr);
    } 
    else pathBuffer = exeDir + "\\src" + replaceSlashes(relativePathStr);
#elif defined(__APPLE__)
    char execPath[PATH_MAX];
    uint32_t size = sizeof(execPath);
    if (_NSGetExecutablePath(execPath, &size) == 0) {
        char* realPath = realpath(execPath, nullptr);
        if (realPath) {
            std::string exeDir(realPath);
            free(realPath);
            size_t macosPos = exeDir.rfind("/Contents/MacOS/");
            if (macosPos != std::string::npos) {
                pathBuffer = exeDir.substr(0, macosPos) + "/Contents/Resources" + relativePathStr;
            } else {
                size_t buildPos = exeDir.rfind("/build/");
                if (buildPos != std::string::npos) {
                    pathBuffer = exeDir.substr(0, buildPos) + "/src" + relativePathStr;
                } else {
                    pathBuffer = std::string("./src") + relativePathStr;
                }
            }
        } else {
            pathBuffer = std::string("./src") + relativePathStr;
        }
    } else {
        pathBuffer = std::string("./src") + relativePathStr;
    }
#else
    const char* appdir = std::getenv("APPDIR");
    if (appdir) pathBuffer = std::string(appdir) + std::string("/src") + relativePathStr;
    else pathBuffer = std::string("./src") + relativePathStr;
#endif
    return pathBuffer;
}
std::string normalizeString(const std::string &str) {
    std::string normalized;
    for (char c : str) {
        if (std::isalnum(static_cast<unsigned char>(c))) normalized += std::tolower(static_cast<unsigned char>(c));
        else normalized += ' ';
    }
    static const std::unordered_set<std::string> irrelevant = {"3k", "4k", "2k", "/"};
    std::istringstream iss(normalized);
    std::string token, result;
    while (iss >> token) {
        if (irrelevant.count(token) == 0) result += token + " ";
    }
    if(!result.empty()) result.pop_back();
    return result;
}
std::vector<std::string> generateNgrams(const std::string &str, int n){
    std::vector<std::string> ngrams;
    if(str.size()<n) {
        ngrams.push_back(str);
        return ngrams;
    }
    for(size_t i=0; i<=str.size()-n; i++){
        ngrams.push_back(str.substr(i,n));
    }
    return ngrams;
}
double computeCosineSimilarity(const std::vector<std::string> &vec1, const std::vector<std::string> &vec2){
    std::unordered_map<std::string, int> freq1, freq2;
    for(const auto &gram : vec1) freq1[gram]++;
    for(const auto &gram : vec2) freq2[gram]++;
    double dot = 0.0, mag1 = 0.0, mag2 = 0.0;
    for(const auto &[gram, count1] : freq1){
        if(freq2.count(gram)) dot += count1 * freq2[gram];
        mag1 += count1 * count1;
    }
    for(const auto &[gram, count2] : freq2){
        mag2 += count2 * count2;
    }
    if (mag1 == 0 || mag2 == 0) return 0.0;
    return dot / (std::sqrt(mag1) * std::sqrt(mag2));
}
double computeMatchScore(const std::string &filename, const std::string &keyword){
    std::string normalizedFilename = normalizeString(filename);
    std::string normalizedKeyword = normalizeString(keyword);
    std::istringstream iss(normalizedFilename);
    std::string word;
    while(iss >> word){
        if(word == normalizedKeyword) return 1.0;
    }
    if(normalizedKeyword.length() >= 3 && normalizedFilename.find(normalizedKeyword) != std::string::npos){
        return 0.9;
    }
    auto filenameNgrams = generateNgrams(normalizedFilename, 3);
    auto keywordNgrams = generateNgrams(normalizedKeyword, 3);
    return computeCosineSimilarity(filenameNgrams, keywordNgrams) * 0.8;
}
void matchTextures(const std::vector<std::string> &filenames, const std::unordered_map<std::string, int> &textureMap, zip* archive, int numTextures, std::vector<int> &textures){
    int numFiles = filenames.size();
    std::vector<std::vector<double>> probabilities(numFiles, std::vector<double>(numTextures, 0.0));
    for (int i = 0; i < numFiles; i++) {
        std::string format = filenames[i].substr(filenames[i].size()-3, 3);
        if(format != "png" && format != "jpg" && format != "peg" && format != "gif" && format != "ebp" && format != "bmp" && format != "eif") continue;
        for (const auto &[keyword, slot] : textureMap) {
            probabilities[i][slot] = std::max(probabilities[i][slot], computeMatchScore(filenames[i], keyword));
        }
    }
    std::vector<bool> assignedSlots(numTextures, false);
    std::vector<bool> assignedFiles(numFiles, false);

    while (true) {
        double maxProbability = 0.0;
        int bestFile = -1;
        int bestSlot = -1;

        for (int i = 0; i < numFiles; i++) {
            if (assignedFiles[i]) continue;
            for (int j = 0; j < numTextures; j++) {
                if (assignedSlots[j]) continue;
                if (probabilities[i][j] > maxProbability) {
                    maxProbability = probabilities[i][j];
                    bestFile = i;
                    bestSlot = j;
                }
            }
        }
        if (bestFile == -1 || bestSlot == -1 || maxProbability < 0.1) break; 
        zip_file* zfile = zip_fopen(archive, filenames[bestFile].c_str(), ZIP_FL_UNCHANGED);
        if(!zfile){
            std::cerr << "Failed to open file in zip: " << filenames[bestFile] << std::endl;
            assignedFiles[bestFile] = true;
            continue;
        }
        std::string tempFilename = filenames[bestFile];
        std::replace(tempFilename.begin(), tempFilename.end(), '/', '_');
        #if defined(_WIN32) || defined(__CYGWIN__)
        #else
        const char* tempDir = std::getenv("TMPDIR") ? std::getenv("TMPDIR") : "/tmp/";
        tempFilename = std::string(tempDir) + "/" + tempFilename;
        #endif
        std::ofstream outFile(tempFilename.c_str(), std::ios::binary);
        if (!outFile) {
            std::cerr << "Failed to create temporary file: " << tempFilename << std::endl;
            zip_fclose(zfile);
            assignedFiles[bestFile] = true;
            continue;
        }
        char buffer[4096];
        int bytesRead;
        while ((bytesRead = zip_fread(zfile, buffer, sizeof(buffer))) > 0) {
            outFile.write(buffer, bytesRead);
        }
        zip_fclose(zfile);
        outFile.close();
        textures[bestSlot] = loadTexture(tempFilename);
        remove(tempFilename.c_str());
        assignedFiles[bestFile] = true;
        assignedSlots[bestSlot] = true;
        for (int i = 0; i < numFiles; i++) {
            if (assignedFiles[i]) continue;
            double sum = 0.0;
            for (int j = 0; j < numTextures; j++) {
                if (assignedSlots[j]) continue;
                sum += probabilities[i][j];
            }
            if (sum > 0) {
                for (int j = 0; j < numTextures; j++) {
                    if (!assignedSlots[j]) probabilities[i][j] /= sum;
                }
            }
        }
    }
}
std::pair<unsigned int*, bool> OpenZipFile(const char* path){
    const int numTextures = 5;
    unsigned int* textures = new unsigned int[numTextures];
    for(int i=0; i<numTextures; i++) textures[i] = -1;
    int err = 0;
    zip* archive = zip_open(path, 0, &err);
    bool metallic = true;
    if(!archive) {
        std::cerr<<"Failed to open archive."<<std::endl;
        error = "Failed to open archive";
        errorTime = 0.0f;
        return {textures, metallic};
    }
    else{
        std::unordered_map<std::string, int> textureMap = {
            {"albedo", 0}, {"diffuse", 0}, {"color", 0}, {"col", 0}, {"base", 0},
            {"metallic", 1}, {"metalness", 1}, {"metal", 1},
            {"normal", 2}, {"nrm", 2}, {"nor", 2},
            {"roughness", 3}, {"rough", 3},
            {"ao", 4}, {"ambient", 4}, {"occlusion", 4},
            {"refl", 5}, {"reflection", 5}, {"specular", 5}, {"spec", 5},
            {"gloss", 6}, {"glossiness", 6}
        };
        zip_int64_t numFiles = zip_get_num_entries(archive, 0);
        std::vector<std::string> filenames;
        for(zip_int64_t i=0; i<numFiles; i++){
            const char* filename = zip_get_name(archive, i, 0);
            if(!filename){
                std::cerr<<"Error opening file in zip"<<std::endl;
                error = "Error opening file in zip";
                errorTime = 0.0f;
                continue;
            }
            filenames.emplace_back(filename);
        }
        std::vector<int> matchedTextures(numTextures+2, -1);

        matchTextures(filenames, textureMap, archive, numTextures+2, matchedTextures);
        textures[0] = matchedTextures[0];
        textures[2] = matchedTextures[2];
        textures[4] = matchedTextures[4];
        if(matchedTextures[5]!=-1&&matchedTextures[6]!=-1){
            textures[1] = matchedTextures[5];
            textures[3] = matchedTextures[6];
            metallic = false;
        }
        else{
            textures[1] = matchedTextures[1];
            textures[3] = matchedTextures[3];
            metallic = true;
        }
    }
    zip_close(archive);
    return {textures, metallic};
}
unsigned int createShader(std::string &vertSource, std::string &fragSource){
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* vertSourceCStr = vertSource.c_str();
    glShaderSource(vertexShader, 1, &vertSourceCStr, nullptr);
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
    const char* fragShaderCStr = fragSource.c_str();
    glShaderSource(fragmentShader, 1, &fragShaderCStr, nullptr);
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
void HDRItoCubemap(std::string environmentLoc, unsigned int skyProgram, unsigned int irradianceProgram, unsigned int prefilterProgram, unsigned int VAO, unsigned int &envCubemapSet, unsigned int &irradianceMapSet, unsigned int &prefilterMapSet){
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
    for(unsigned int i=0; i<6; i++){
        glUniformMatrix4fv(glGetUniformLocation(skyProgram, "view"), 1, GL_FALSE, &captureViews[i][0][0]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
    }
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
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);
    glUseProgram(irradianceProgram);
    glUniform1i(glGetUniformLocation(irradianceProgram, "skybox"), 0); 
    glUniformMatrix4fv(glGetUniformLocation(irradianceProgram, "projection"), 1, GL_FALSE, &captureProj[0][0]);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    glViewport(0, 0, 32, 32);
    for(unsigned int i=0; i<6; i++){
        glUniformMatrix4fv(glGetUniformLocation(irradianceProgram, "view"), 1, GL_FALSE, &captureViews[i][0][0]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
    }
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
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    envCubemapSet = envCubemap;
    irradianceMapSet = irradianceMap;
    prefilterMapSet = prefilterMap;
    return;
}
void GetBRDFLUTTexture(unsigned int brdfProgram, unsigned int &brdfMapSet){
    unsigned int captureFBO;
    unsigned int captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);
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
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    brdfMapSet = brdfLUTTexture;
    return;
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
    GLint internalFormat;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &internalFormat);
    GLenum format;
    switch(internalFormat){
        case GL_RGB:
        case GL_RGB8:
        case GL_SRGB:
        case GL_SRGB8:
            imgData->channels = 3;
            format = GL_RGB;
            break;
        case GL_RGBA:
        case GL_RGBA8:
        case GL_SRGB_ALPHA:
        case GL_SRGB8_ALPHA8:
            imgData->channels = 4;
            format = GL_RGBA;
            break;
        case GL_RED:
        case GL_R8:
            imgData->channels = 1;
            format = GL_RED;
            break;
        default:
            std::cerr<<"Unsupported texture format: "<<internalFormat<<std::endl;
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
std::mutex zipMutex;
void saveTexturesToZip(const char* path, unsigned int albedo, unsigned int roughness, unsigned int normal, unsigned int metallic, unsigned int ao){
    int errorTemp;
    zip_t* zip = zip_open(path, ZIP_CREATE | ZIP_TRUNCATE, &errorTemp);
    if(!zip){
        std::cerr<<"Couldn't save zip file"<<std::endl;
        error = "Couldn't save zip file";
        errorTime = 0.0f;
        return;
    }
    ImageData* albedoData = loadTextureData(albedo);
    ImageData* roughnessData = loadTextureData(roughness);
    ImageData* normalData = loadTextureData(normal);
    ImageData* metalnessData = loadTextureData(metallic);
    ImageData* aoData = loadTextureData(ao);
    if (!albedoData || !roughnessData || !normalData || !metalnessData || !aoData) {
        std::cerr << "Failed to load one or more texture data." << std::endl;
        error = "Failed to load one or more texture data.";
        errorTime = 0.0f;
        zip_discard(zip);
        return;
    }
    std::vector<std::pair<std::string, std::vector<unsigned char>>> dataBuffer;
    auto processTexture = [&](ImageData* texture, const std::string& filename) {
        int pngSize;
        unsigned char* pngData = stbi_write_png_to_mem(texture->data, texture->width * texture->channels, texture->width, texture->height, texture->channels, &pngSize);
        if (!pngData) {
            std::cerr<<"Couldn't save png to memory"<<std::endl;
            error = "Couldn't save png to memory";
            errorTime = 0.0f;
            return;
        }
        std::lock_guard<std::mutex> lock(zipMutex);
        dataBuffer.emplace_back(filename, std::vector<unsigned char>(pngData, pngData + pngSize));
        stbi_image_free(pngData);
    };
    std::thread t1(processTexture, albedoData, "albedo.png");
    std::thread t2(processTexture, roughnessData, "roughness.png");
    std::thread t3(processTexture, normalData, "normal.png");
    std::thread t4(processTexture, metalnessData, "metallic.png");
    std::thread t5(processTexture, aoData, "ao.png");
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    for(const auto& [filename, pngData] : dataBuffer) {
        int pngSize = sizeof(pngData);
        zip_source_t* source = zip_source_buffer(zip, pngData.data(), pngData.size(), 0);
        if (!source || zip_file_add(zip, filename.c_str(), source, ZIP_FL_OVERWRITE) < 0) {
            zip_source_free(source);
            zip_discard(zip);
            std::cerr << "Couldn't add PNG to zip file"<<std::endl;
            error = "Couldn't add PNG to zip file";
            errorTime = 0.0f;
            return;
        }
    }
    zip_close(zip);
    return;
}
void writeCustomTextureFile(const char* outputPath, unsigned int albedo, unsigned int roughness, unsigned int normal, unsigned int metalness, unsigned int ao){
    ImageData* albedoData = loadTextureData(albedo);
    ImageData* roughnessData = loadTextureData(roughness);
    ImageData* normalData = loadTextureData(normal);
    ImageData* metalnessData = loadTextureData(metalness);
    ImageData* aoData = loadTextureData(ao);
    if (!albedoData || !roughnessData || !normalData || !metalnessData || !aoData) {
        std::cerr << "Failed to load one or more texture data." << std::endl;
        error = "Failed to load one or more texture data.";
        errorTime = 0.0f;
        return;
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
        return;
    }
    long long int magicNumber;
    if(isMetallic) magicNumber = 0x4D4154455249414C;
    else magicNumber = 0x53504543554C4152;
    outputFile.write(reinterpret_cast<const char*>(&magicNumber), sizeof(magicNumber));
    outputFile.write(reinterpret_cast<const char*>(&albedoMeta), sizeof(albedoMeta));
    outputFile.write(reinterpret_cast<const char*>(&roughnessMeta), sizeof(roughnessMeta));
    outputFile.write(reinterpret_cast<const char*>(&normalMeta), sizeof(normalMeta));
    outputFile.write(reinterpret_cast<const char*>(&metalnessMeta), sizeof(metalnessMeta));
    outputFile.write(reinterpret_cast<const char*>(&aoMeta), sizeof(aoMeta));
    auto compressWriteTexture = [&outputFile](ImageData* imageData){
        size_t originalSize = imageData->width * imageData->height * imageData->channels;
        size_t compressedBound = ZSTD_compressBound(originalSize);
        std::vector<unsigned char> compressedData(compressedBound);
        size_t compressedSize = ZSTD_compress(compressedData.data(), compressedBound, imageData->data, originalSize, 1);
        if(ZSTD_isError(compressedSize)){
            std::cerr<<"Compression error: "<<ZSTD_getErrorName(compressedSize)<<std::endl;
            error = "Error compressing textures";
            errorTime = 0.0f;
            return;
        }
        outputFile.write(reinterpret_cast<const char*>(&compressedSize), sizeof(compressedSize));
        outputFile.write(reinterpret_cast<const char*>(compressedData.data()), compressedSize);
    };
    compressWriteTexture(albedoData);
    compressWriteTexture(roughnessData);
    compressWriteTexture(normalData);
    compressWriteTexture(metalnessData);
    compressWriteTexture(aoData);
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
    return;
}

class MemoryBuffer : public std::streambuf {
public:
    MemoryBuffer(const unsigned char* data, size_t size) {
        char* p = const_cast<char*>(reinterpret_cast<const char*>(data));
        setg(p, p, p + size);
    }
};

void readCustomTextureFile(std::string inputPath, unsigned int &albedo, unsigned int &roughness, unsigned int &normal, unsigned int &metallic, unsigned int &ao){
    unsigned int albedoID;
    unsigned int roughnessID;
    unsigned int normalID;
    unsigned int metallicID;
    unsigned int aoID;
    std::string resourcePath = toResourcePath(inputPath);
    const auto* resource = EmbeddedResources::getResource(resourcePath);
    
    std::unique_ptr<MemoryBuffer> memBuf;
    std::unique_ptr<std::istream> inputStream;
    std::ifstream inputFile;
    
    if (resource) {
        memBuf = std::make_unique<MemoryBuffer>(resource->data, *resource->size);
        inputStream = std::make_unique<std::istream>(memBuf.get());
    } else {
        inputFile.open(inputPath, std::ios::binary);
        if(!inputFile.is_open()){
            std::cerr << "Failed to load material file" << std::endl;
            error = "Failed to load material file.";
            errorTime = 0.0f;
            return;
        }
        inputStream = std::make_unique<std::istream>(inputFile.rdbuf());
    }
    
    std::istream& input = *inputStream;
    
    long long int magicNumber;
    input.read(reinterpret_cast<char*>(&magicNumber), sizeof(magicNumber));
    if(magicNumber == 0x4D4154455249414C) isMetallic = true;
    else if(magicNumber == 0x53504543554C4152) isMetallic = false;
    else{
        std::cerr << "Invalid file format" << std::endl;
        error = "Invalid file format.";
        errorTime = 0.0f;
        return;
    }
    TextureMetadata albedoMeta, roughnessMeta, normalMeta, metalnessMeta, aoMeta;
    input.read(reinterpret_cast<char*>(&albedoMeta), sizeof(albedoMeta));
    input.read(reinterpret_cast<char*>(&roughnessMeta), sizeof(roughnessMeta));
    input.read(reinterpret_cast<char*>(&normalMeta), sizeof(normalMeta));
    input.read(reinterpret_cast<char*>(&metalnessMeta), sizeof(metalnessMeta));
    input.read(reinterpret_cast<char*>(&aoMeta), sizeof(aoMeta));
    auto loadTextureFromStream = [&input](const TextureMetadata& meta) -> ImageData* {
        size_t compressedSize;
        input.read(reinterpret_cast<char*>(&compressedSize), sizeof(compressedSize));
        std::vector<unsigned char> compressedData(compressedSize);
        input.read(reinterpret_cast<char*>(compressedData.data()), compressedSize);
        size_t decompressedSize = meta.width * meta.height * meta.channels;
        ImageData* imageData = new ImageData();
        imageData->width = meta.width;
        imageData->height = meta.height;
        imageData->channels = meta.channels;
        imageData->data = new unsigned char[decompressedSize];
        size_t actualDecompressedSize = ZSTD_decompress(imageData->data, decompressedSize, compressedData.data(), compressedSize);
        if (ZSTD_isError(actualDecompressedSize)) {
            std::cerr << "Decompression error: " << ZSTD_getErrorName(actualDecompressedSize) << std::endl;
            error = "Error decompressing textures";
            errorTime = 0.0f;
            delete[] imageData->data;
            delete imageData;
            return nullptr;
        }
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
    if (inputFile.is_open()) inputFile.close();
    albedo = albedoID;
    roughness = roughnessID;
    normal = normalID;
    metallic = metallicID;
    ao=aoID;
    return;
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
    const auto* resource = EmbeddedResources::getResource("resources/Roboto-Regular.ttf");
    FT_Error ftError;
    if (resource) {
        ftError = FT_New_Memory_Face(ft, resource->data, static_cast<FT_Long>(*resource->size), 0, &face);
    } else {
        ftError = FT_New_Face(ft, getAppPath("/resources/Roboto-Regular.ttf").c_str(), 0, &face);
    }
    
    if(ftError){
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
void loadModel(std::string filePath, unsigned int &VAO, unsigned int &VBO, unsigned int &EBO, unsigned int &indexCount){
    objl::Loader loader;
    bool loaded = false;
    std::string tempFilePath;
    
    std::string resourcePath = toResourcePath(filePath);
    const auto* resource = EmbeddedResources::getResource(resourcePath);
    if (resource) {
#if defined(_WIN32) || defined(__CYGWIN__)
        char tempPath[MAX_PATH];
        GetTempPathA(MAX_PATH, tempPath);
        tempFilePath = std::string(tempPath) + "materialviewer_temp.obj";
#else
        tempFilePath = "/tmp/materialviewer_temp.obj";
#endif
        std::ofstream tempFile(tempFilePath, std::ios::binary);
        if (tempFile) {
            tempFile.write(reinterpret_cast<const char*>(resource->data), *resource->size);
            tempFile.close();
            loaded = loader.LoadFile(tempFilePath);
            std::remove(tempFilePath.c_str());
        }
    } else {
        loaded = loader.LoadFile(filePath.c_str());
    }
    
    if(!loaded){
        std::cerr<<"Failed to load OBJ file"<<std::endl;
        error = "Failed to load OBJ file";
        errorTime = 0.0f;
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
    if(aspectRatio > 1.0f) scaleFactor = 1.8f / maxDistance;
    else scaleFactor = 1.25f / (maxDistance);
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

std::string vertexLoc = getAppPath("/shaders/main.vert");
std::string fragmentLoc = getAppPath("/shaders/main.frag");
std::string skyFragmentLoc = getAppPath("/shaders/sky.frag");
std::string cubemapVertexLoc = getAppPath("/shaders/cubemap.vert");
std::string cubemapFragmentLoc = getAppPath("/shaders/cubemap.frag");
std::string irradianceFragmentLoc = getAppPath("/shaders/irradiance.frag");
std::string prefilterFragmentLoc = getAppPath("/shaders/prefilter.frag");
std::string brdfFragmentLoc = getAppPath("/shaders/brdf.frag");
std::string brdfVertexLoc = getAppPath("/shaders/brdf.vert");
std::string uiVertexLoc = getAppPath("/shaders/ui.vert");
std::string uiFragmentLoc = getAppPath("/shaders/ui.frag");
std::string textFragmentLoc = getAppPath("/shaders/text.frag");
std::string vertexShaderSource = getShaders(vertexLoc);
std::string fragmentShaderSource = getShaders(fragmentLoc);
std::string skyFragmentShaderSource = getShaders(skyFragmentLoc);
std::string cubemapVertexShaderSource = getShaders(cubemapVertexLoc);
std::string cubemapFragmentShaderSource = getShaders(cubemapFragmentLoc);
std::string irradianceFragmentShaderSource = getShaders(irradianceFragmentLoc);
std::string prefilterFragmentShaderSource = getShaders(prefilterFragmentLoc);
std::string brdfFragmentShaderSource = getShaders(brdfFragmentLoc);
std::string brdfVertexShaderSource = getShaders(brdfVertexLoc);
std::string uiVertexShaderSource = getShaders(uiVertexLoc);
std::string uiFragmentShaderSource = getShaders(uiFragmentLoc);
std::string textFragmentShaderSource = getShaders(textFragmentLoc);
std::string defaultMatLoc = getAppPath("/material/stainless_steel.mat");
std::string albedoLoc = getAppPath("/material/albedoNot.png");
std::string aoLoc = getAppPath("/material/aoNot.png");
std::string metallicLoc = getAppPath("/material/metallicNot.png");
std::string normalLoc = getAppPath("/material/normalNot.png");
std::string roughnessLoc = getAppPath("/material/roughnessNot.png");
std::string environmentLocs[] = {
    getAppPath("/environments/industrial_sunset_puresky/environment.hdr"),
    getAppPath("/environments/kloppenheim_02_puresky/environment.hdr"),
    getAppPath("/environments/snowy_forest/environment.hdr"),
    getAppPath("/environments/syferfontein_1d_clear_puresky/environment.hdr")
};
int currentElement = 0;
std::string uiElementLocs[] = {
    getAppPath("/ui/hdri_ui1.png"),
    getAppPath("/ui/hdri_ui2.png"),
    getAppPath("/ui/hdri_ui3.png"),
    getAppPath("/ui/hdri_ui4.png"),
    getAppPath("/ui/hdri_ui5.png"),
    getAppPath("/ui/hdri_ui6.png"),
    getAppPath("/ui/hdri_ui7.png"),
    getAppPath("/ui/hdri_ui8.png"),
    getAppPath("/ui/hdri_ui9.png"),
    getAppPath("/ui/img_ui10.png"),
    getAppPath("/ui/img_ui11.png"),
    getAppPath("/ui/img_ui12.png"),
    getAppPath("/ui/img_ui13.png"),
    getAppPath("/ui/img_ui14.png"),
    getAppPath("/ui/img_ui15.png"),
    getAppPath("/ui/img_ui16.png"),
    getAppPath("/ui/img_ui17.png"),
    getAppPath("/ui/img_ui18.png"),
    getAppPath("/ui/img_ui19.png"),
    getAppPath("/ui/img_ui20.png"),
    getAppPath("/ui/img_ui21.png"),
};
constexpr size_t UI_ELEMENT_COUNT = 21;
glm::vec3 extraColors[UI_ELEMENT_COUNT];
std::string cubeLoc = getAppPath("/models/cube.obj");
std::string sphereLoc = getAppPath("/models/sphere.obj");
std::string teapotLoc = getAppPath("/models/teapot.obj");
std::string backgroundLoc = getAppPath("/resources/background.png");
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

int main(int argc, char* argv[]) {   
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window;
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Material Viewer", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Failed to open GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouseCallback);
    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    SCR_WIDTH = fbWidth;
    SCR_HEIGHT = fbHeight;
    int winWidth, winHeight;
    glfwGetWindowSize(window, &winWidth, &winHeight);
    contentScale = (float)fbWidth / (float)winWidth; // for Retina or high-DPI displays

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

    unsigned int envCubemap;
    unsigned int irradianceMap;
    unsigned int prefilterMap;
    unsigned int brdfMap;
    HDRItoCubemap(environmentLocs[currentElement], cubemapShaderProgram, irradianceShaderProgram, prefilterShaderProgram, skyVAO, envCubemap, irradianceMap, prefilterMap);
    GetBRDFLUTTexture(brdfShaderProgram, brdfMap);

    readCustomTextureFile(defaultMatLoc, albedo, roughness, normal, metallic, ao);
    if (argc > 1) {
        std::cout << "Opening file: " << argv[1] << std::endl;
        readCustomTextureFile(argv[1], albedo, roughness, normal, metallic, ao);
    } 
    else std::cout << "No file provided." << std::endl;

    for(unsigned int i=0; i<UI_ELEMENT_COUNT; i++){
        extraColors[i] = glm::vec3(1.0f);
    }
    unsigned int uiElements[UI_ELEMENT_COUNT] = {};
    for(unsigned int i=0; i<UI_ELEMENT_COUNT; i++){
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
            HDRItoCubemap(environmentLocs[currentElement], cubemapShaderProgram, irradianceShaderProgram, prefilterShaderProgram, skyVAO, envCubemap, irradianceMap, prefilterMap);
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
            HDRItoCubemap(uploadedEnv, cubemapShaderProgram, irradianceShaderProgram, prefilterShaderProgram, skyVAO, envCubemap, irradianceMap, prefilterMap);
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

        if(isMetallic) glUniform1i(glGetUniformLocation(shaderProgram, "isMetallic"), 1);
        else glUniform1i(glGetUniformLocation(shaderProgram, "isMetallic"), 0);

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
            RenderText(textProgram, textVAO, textVBO, error, ui(10.0f), (float)SCR_HEIGHT - ui(20.0f), 0.35f * contentScale, glm::vec3(0.8f, 0.2f, 0.2f));
            errorTime += deltaTime;
            if(errorTime >= 7.0f) error = "";
        }
        else errorTime = 0.0f;
        if(tooltip != ""){
            tooltipTime += deltaTime;
            if(tooltipTime >= 0.5f){
                RenderText(textProgram, textVAO, textVBO, tooltip, lastX, (float)SCR_HEIGHT - lastY, 0.35f * contentScale, glm::vec3(0.8f, 0.8f, 0.8f));
                glUseProgram(spriteProgram);
                glm::mat4 spriteModel = glm::mat4(1.0f);
                spriteModel = glm::translate(spriteModel, glm::vec3(lastX - ui(2.5f), (float)SCR_HEIGHT - lastY - ui(2.5f), 0.0f));
                spriteModel = glm::scale(spriteModel, glm::vec3(ui(8.5f) * tooltip.size(), ui(20.0f), 1.0f));
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
        spriteModel = glm::translate(spriteModel, glm::vec3(ui(10.0f), (float)SCR_HEIGHT - ui(10.0f), 0.0f));
        spriteModel = glm::scale(spriteModel, glm::vec3(ui(50.0f), ui(-50.0f), 1.0f));
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
        spriteModel = glm::translate(spriteModel, glm::vec3((float)SCR_WIDTH - ui(60.0f), (float)SCR_HEIGHT - ui(10.0f), 0.0f));
        spriteModel = glm::scale(spriteModel, glm::vec3(ui(50.0f), ui(-50.0f), 1.0f));
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
            spriteModel = glm::translate(spriteModel, glm::vec3((float)SCR_WIDTH / 4.0f + ui(20.0f), (float)SCR_HEIGHT / 4.0f + ui(20.0f), 0.0f));
            spriteModel = glm::scale(spriteModel, glm::vec3(ui(50.0f), ui(50.0f), 1.0f));
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
            RenderText(textProgram, textVAO, textVBO, "Base Color", (float)SCR_WIDTH / 4.0f + ui(80.0f), (float)SCR_HEIGHT * 3.0f / 4.0f - ui(50.0f), 0.4f * contentScale, glm::vec3(0.8f, 0.8f, 0.8f) * extraColors[9]);
            if(isMetallic){
                RenderText(textProgram, textVAO, textVBO, "Metalness", (float)SCR_WIDTH / 4.0f + ui(80.0f), (float)SCR_HEIGHT * 3.0f / 4.0f - ui(110.0f), 0.4f * contentScale, glm::vec3(0.8f, 0.8f, 0.8f) * extraColors[10]);
                RenderText(textProgram, textVAO, textVBO, "Roughness", (float)SCR_WIDTH / 4.0f + ui(80.0f), (float)SCR_HEIGHT * 3.0f / 4.0f - ui(230.0f), 0.4f * contentScale, glm::vec3(0.8f, 0.8f, 0.8f) * extraColors[12]);
            }
            else{
                RenderText(textProgram, textVAO, textVBO, "Specular", (float)SCR_WIDTH / 4.0f + ui(80.0f), (float)SCR_HEIGHT * 3.0f / 4.0f - ui(110.0f), 0.4f * contentScale, glm::vec3(0.8f, 0.8f, 0.8f) * extraColors[10]);
                RenderText(textProgram, textVAO, textVBO, "Glossiness", (float)SCR_WIDTH / 4.0f + ui(80.0f), (float)SCR_HEIGHT * 3.0f / 4.0f - ui(230.0f), 0.4f * contentScale, glm::vec3(0.8f, 0.8f, 0.8f) * extraColors[12]);
            }
            RenderText(textProgram, textVAO, textVBO, "Normal Map", (float)SCR_WIDTH / 4.0f + ui(80.0f), (float)SCR_HEIGHT * 3.0f / 4.0f - ui(170.0f), 0.4f * contentScale, glm::vec3(0.8f, 0.8f, 0.8f) * extraColors[11]);
            RenderText(textProgram, textVAO, textVBO, "Ambient Occlusion", (float)SCR_WIDTH / 4.0f + ui(80.0f), (float)SCR_HEIGHT * 3.0f / 4.0f - ui(290.0f), 0.4f * contentScale, glm::vec3(0.8f, 0.8f, 0.8f) * extraColors[13]);
            RenderText(textProgram, textVAO, textVBO, "Metallic", (float)SCR_WIDTH * 2.25f / 4.0f - ui(25.0f), (float)SCR_HEIGHT * 0.5f / 4.0f + ui(45.0f), 0.3f * contentScale, glm::vec3(0.8f, 0.8f, 0.8f) * extraColors[19]);
            RenderText(textProgram, textVAO, textVBO, "Specular", (float)SCR_WIDTH * 2.25f / 4.0f + ui(75.0f), (float)SCR_HEIGHT * 0.5f / 4.0f + ui(45.0f), 0.3f * contentScale, glm::vec3(0.8f, 0.8f, 0.8f) * extraColors[20]);
            glUseProgram(spriteProgram);
            glBindVertexArray(spriteVAO);
            glUniformMatrix4fv(glGetUniformLocation(spriteProgram, "projection"), 1, GL_FALSE, &orthoProj[0][0]);
            glActiveTexture(GL_TEXTURE0);
            spriteModel = glm::mat4(1.0f);
            spriteModel = glm::translate(spriteModel, glm::vec3((float)SCR_WIDTH * 3.0f / 4.0f - ui(30.0f), (float)SCR_HEIGHT / 4.0f + ui(20.0f), 0.0f));
            spriteModel = glm::scale(spriteModel, glm::vec3(ui(20.0f), ui(20.0f), 1.0f));
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
            spriteModel = glm::translate(spriteModel, glm::vec3(0.0f, -1.1f, 0.0f));
            glUniformMatrix4fv(glGetUniformLocation(spriteProgram, "model"), 1, GL_FALSE, &spriteModel[0][0]);
            glUniform3fv(glGetUniformLocation(spriteProgram, "extraColor"), 1, &extraColors[18][0]);
            glBindTexture(GL_TEXTURE_2D, uiElements[18]);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            spriteModel = glm::mat4(1.0f);
            spriteModel = glm::translate(spriteModel, glm::vec3((float)SCR_WIDTH * 2.25f / 4.0f - ui(50.0f), (float)SCR_HEIGHT * 3.5f / 4.0f - ui(30.0f), 0.0f));
            spriteModel = glm::scale(spriteModel, glm::vec3(ui(100.0f), ui(-40.0f), 1.0f));
            glUniformMatrix4fv(glGetUniformLocation(spriteProgram, "model"), 1, GL_FALSE, &spriteModel[0][0]);
            glUniform3fv(glGetUniformLocation(spriteProgram, "extraColor"), 1, &extraColors[19][0]);
            if(isMetallic) glBindTexture(GL_TEXTURE_2D, uiElements[20]);
            else glBindTexture(GL_TEXTURE_2D, uiElements[19]);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            spriteModel = glm::translate(spriteModel, glm::vec3(1.0f, 0.0f, 0.0f));
            glUniformMatrix4fv(glGetUniformLocation(spriteProgram, "model"), 1, GL_FALSE, &spriteModel[0][0]);
            glUniform3fv(glGetUniformLocation(spriteProgram, "extraColor"), 1, &extraColors[20][0]);
            if(!isMetallic) glBindTexture(GL_TEXTURE_2D, uiElements[20]);
            else glBindTexture(GL_TEXTURE_2D, uiElements[19]);
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
    const char* newTexture = OpenFileDialogTex();
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
        std::pair<unsigned int*, bool> import = OpenZipFile(newZip);
        unsigned int* textures = import.first;
        isMetallic = import.second;
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
    if(matPath) readCustomTextureFile(matPath, albedo, roughness, normal, metallic, ao);
}
void downloadTextures(){
    char* zipPath = SaveZipFileDialog();
    if(zipPath){
        saveTexturesToZip(zipPath, albedo, roughness, normal, metallic, ao);
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
        else if(currentElement == 18)
            downloadTextures();
        else if(currentElement == 19)
            isMetallic = true;
        else if(currentElement == 20)
            isMetallic = false;
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
    double xpos = xposIn * contentScale;
    double ypos = yposIn * contentScale;
    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = true;
        if(showMaterialUI){
            if(ypos > SCR_HEIGHT / 4.0f + ui(20.0f) && ypos < SCR_HEIGHT / 4.0f + ui(40.0f) && xpos > SCR_WIDTH * 3.0f / 4.0f - ui(30.0f) && xpos < SCR_WIDTH * 3.0f / 4.0f - ui(10.0f)){
                hoverElement(8); highlightingUI = true;
            }
            else if(ypos > SCR_HEIGHT * 29.0f / 36.0f || ypos < SCR_HEIGHT / 4.0f || xpos < SCR_WIDTH / 4.0f || xpos > SCR_WIDTH * 3.0f / 4.0f){
                hoverElement(-1); currentElement = 8; highlightingUI = true;
            }
            else if(ypos < SCR_HEIGHT / 4.0f + ui(95.0f) && ypos > SCR_HEIGHT / 4.0f + ui(45.0f) && xpos > SCR_WIDTH * 3.0f / 4.0f - ui(60.0f) && xpos < SCR_WIDTH * 3.0f / 4.0f - ui(10.0f)){
                hoverElement(14); highlightingUI = true; tooltip = "Upload .zip";
            }
            else if(ypos < SCR_HEIGHT / 4.0f + ui(150.0f) && ypos > SCR_HEIGHT / 4.0f + ui(100.0f) && xpos > SCR_WIDTH * 3.0f / 4.0f - ui(60.0f) && xpos < SCR_WIDTH * 3.0f / 4.0f - ui(10.0f)){
                hoverElement(15); highlightingUI = true; tooltip = "Save .mat";
            }
            else if(ypos < SCR_HEIGHT / 4.0f + ui(205.0f) && ypos > SCR_HEIGHT / 4.0f + ui(155.0f) && xpos > SCR_WIDTH * 3.0f / 4.0f - ui(60.0f) && xpos < SCR_WIDTH * 3.0f / 4.0f - ui(10.0f)){
                hoverElement(16); highlightingUI = true; tooltip = "Upload .mat";
            }
            else if(ypos < SCR_HEIGHT / 4.0f + ui(260.0f) && ypos > SCR_HEIGHT / 4.0f + ui(210.0f) && xpos > SCR_WIDTH * 3.0f / 4.0f - ui(60.0f) && xpos < SCR_WIDTH * 3.0f / 4.0f - ui(10.0f)){
                hoverElement(18); highlightingUI = true; tooltip = "Download textures";
            }
            else if(ypos > SCR_HEIGHT * 3.5f / 4.0f - ui(75.0f) && ypos < SCR_HEIGHT * 3.5 / 4.0f - ui(25.0f) && xpos > SCR_WIDTH * 2.25f / 4.0f - ui(50.0f) && xpos < SCR_WIDTH * 2.25f / 4.0f + ui(50.0f)){
                hoverElement(19); highlightingUI = true; tooltip = "Use metallic workflow";
            }
            else if(ypos > SCR_HEIGHT * 3.5f / 4.0f - ui(75.0f) && ypos < SCR_HEIGHT * 3.5 / 4.0f - ui(25.0f) && xpos > SCR_WIDTH * 2.25f / 4.0f + ui(50.0f) && xpos < SCR_WIDTH * 2.25f / 4.0f + ui(150.0f)){
                hoverElement(20); highlightingUI = true; tooltip = "Use specular workflow";
            }
            else if(xpos > SCR_WIDTH / 4.0f + ui(20.0f) && xpos < SCR_WIDTH / 4.0f + ui(70.0f)){
                if(ypos > SCR_HEIGHT / 4.0f + ui(20.0f) && ypos < SCR_HEIGHT / 4.0f + ui(70.0f)) {
                    hoverElement(9); highlightingUI = true;
                }
                else if(ypos > SCR_HEIGHT / 4.0f + ui(80.0f) && ypos < SCR_HEIGHT / 4.0f + ui(130.0f)) {
                    hoverElement(10); highlightingUI = true;
                }
                else if(ypos > SCR_HEIGHT / 4.0f + ui(140.0f) && ypos < SCR_HEIGHT / 4.0f + ui(190.0f)) {
                    hoverElement(11); highlightingUI = true;
                }
                else if(ypos > SCR_HEIGHT / 4.0f + ui(200.0f) && ypos < SCR_HEIGHT / 4.0f + ui(250.0f)) {
                    hoverElement(12); highlightingUI = true;
                }
                else if(ypos > SCR_HEIGHT / 4.0f + ui(260.0f) && ypos < SCR_HEIGHT / 4.0f + ui(310.0f)) {
                    hoverElement(13); highlightingUI = true;
                }
                else if(highlightingUI) {hoverElement(-1); highlightingUI = false;}
            }
            else if(highlightingUI) {hoverElement(-1); highlightingUI = false;}
        }
        else if(ypos > SCR_HEIGHT - ui(60.0f) && ypos < SCR_HEIGHT - ui(10.0f)){
            if(xpos > ui(10.0f) && xpos < ui(60.0f)) {hoverElement(0); highlightingUI = true;}
            else if(xpos > ui(60.0f) && xpos < ui(110.0f)) {hoverElement(1); highlightingUI = true;}
            else if(xpos > ui(110.0f) && xpos < ui(160.0f)) {hoverElement(2); highlightingUI = true;}
            else if(xpos > ui(160.0f) && xpos < ui(210.0f)) {hoverElement(3); highlightingUI = true;}
            else if(xpos > ui(210.0f) && xpos < ui(260.0f)) {hoverElement(6); highlightingUI = true; tooltip = "Upload HDRI environment";}
            else if(xpos > SCR_WIDTH - ui(60.0f) && xpos < SCR_WIDTH - ui(10.0f)) {hoverElement(4); highlightingUI = true;}
            else if(xpos > SCR_WIDTH - ui(110.0f) && xpos < SCR_WIDTH - ui(60.0f)) {hoverElement(5); highlightingUI = true;}
            else if(xpos > SCR_WIDTH - ui(160.0f) && xpos < SCR_WIDTH - ui(110.0f)) {hoverElement(17); highlightingUI = true;}
            else if(xpos > SCR_WIDTH - ui(210.0f) && xpos < SCR_WIDTH - ui(160.0f)) {hoverElement(7); highlightingUI = true; tooltip = "Change material";}
            else if(highlightingUI) {hoverElement(-1); highlightingUI = false;}
        }
        else if(highlightingUI) {hoverElement(-1); highlightingUI = false;}
        return;
    }
    float xposf = static_cast<float>(xpos);
    float yposf = static_cast<float>(ypos);
    if (firstMouse){
        lastX = xposf;
        lastY = yposf;
        firstMouse = false;
    }
    float xOffset = xposf - lastX;
    float yOffset = lastY - yposf;
    lastX = xposf;
    lastY = yposf;
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
