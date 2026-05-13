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
#include <array>
#include <cstdlib>
#include <filesystem>
#include "embedded_resources.h"

#if defined(__APPLE__)
#include <mach-o/dyld.h>
#include <limits.h>
#endif

#if defined(_WIN32) || defined(__CYGWIN__)
#include <windows.h>
#include <commdlg.h>
#include <direct.h>
char* OpenFileDialogImpl(const char* filter){
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
    ofn.lpstrFilter = filter;
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    if (GetOpenFileNameA(&ofn)) {
        _chdir(currentDir);
        return filePath;
    }
    else {
        _chdir(currentDir);
        return nullptr;
    }
}
char* OpenFileDialog(){ return OpenFileDialogImpl("HDR Files\0*.hdr\0""All Image Files\0*.hdr;*.png;*.jpg;*.jpeg;*.bmp\0"); }
char* OpenFileDialogTex(){ return OpenFileDialogImpl("All Image Files\0*.png;*.jpg;*.jpeg;*.bmp\0"); }
char* OpenFileDialogZip(){ return OpenFileDialogImpl("Zip Files (*.zip)\0*.zip\0All Files (*.*)\0*.*\0"); }
char* SaveFileDialogImpl(const char* filter, const char* defaultName, const char* extension){
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
    ofn.lpstrFilter = filter;
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = nullptr;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = nullptr;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;
    snprintf(filePath, sizeof(filePath), "%s", defaultName);
    if (GetSaveFileNameA(&ofn)){
        std::string fileStr(filePath);
        if (fileStr.find(extension) == std::string::npos)
            fileStr += extension;
        _chdir(currentDir);
        return _strdup(fileStr.c_str());
    }
    else {
        _chdir(currentDir);
        return nullptr;
    }
}
char* SaveMatFileDialog(){ return SaveFileDialogImpl("Material Files (*.mat)\0*.mat\0All Files (*.*)\0*.*\0", "newMat.mat", ".mat"); }
char* SaveZipFileDialog(){ return SaveFileDialogImpl("Zip Files (*.zip)\0*.zip\0All Files (*.*)\0*.*\0", "textures.zip", ".zip"); }
char* OpenFileDialogMaterial(){ return OpenFileDialogImpl("Material Files (*.mat)\0*.mat\0All Files (*.*)\0*.*\0"); }
#else
#include <QFileDialog>
#include <QString>
#include <QApplication>
static QApplication* getQApp(){
    static int argc = 0;
    static char* argv[] = {nullptr};
#if defined(__linux__)
    // Force XCB platform on Linux
    qputenv("QT_QPA_PLATFORM", "xcb");
#endif
    static QApplication app(argc, argv);
    QApplication::setApplicationName("Material Viewer");
    return &app;
}
char* OpenFileDialogImpl(const char* title, const char* filter){
    static char filePath[256];
    getQApp();
    QString filename = QFileDialog::getOpenFileName(nullptr, title, "", filter);
    if (!filename.isEmpty()){
        snprintf(filePath, sizeof(filePath), "%s", filename.toStdString().c_str());
        return filePath;
    }
    else return nullptr;
}
char* SaveFileDialogImpl(const char* title, const char* defaultName, const char* filter, const char* extension){
    static char filePath[256];
    getQApp();
    QString filename = QFileDialog::getSaveFileName(nullptr, title, defaultName, filter);
    if (!filename.isEmpty()){
        std::string fileStr = filename.toStdString();
        if (fileStr.find(extension) == std::string::npos)
            fileStr += extension;
        snprintf(filePath, sizeof(filePath), "%s", fileStr.c_str());
        return filePath;
    }
    else return nullptr;
}
char* OpenFileDialog(){ return OpenFileDialogImpl("Open File", "HDR Files (*.hdr);;All Image Files (*.hdr *.png *.jpg *.jpeg *.bmp)"); }
char* OpenFileDialogTex(){ return OpenFileDialogImpl("Open File", "All Image Files (*.png *.jpg *.jpeg *.bmp)"); }
char* OpenFileDialogZip(){ return OpenFileDialogImpl("Open Zip File", "Zip Files (*.zip);;All Files (*.*)"); }
char* OpenFileDialogMaterial(){ return OpenFileDialogImpl("Open Material File", "Material Files (*.mat);;All Files (*.*)"); }
char* SaveMatFileDialog(){ return SaveFileDialogImpl("Save Material File", "newMat.mat", "Material Files (*.mat);;All Files (*.*)", ".mat"); }
char* SaveZipFileDialog(){ return SaveFileDialogImpl("Save Zip File", "textures.zip", "Zip Files (*.zip);;All Files (*.*)", ".zip"); }
#endif
const float pi = 3.14159265359;

enum UIElement {
    UI_ENV_1 = 0, UI_ENV_2, UI_ENV_3, UI_ENV_4,
    UI_SHAPE_CUBE, UI_SHAPE_SPHERE,
    UI_UPLOAD_HDRI, UI_OPEN_MATERIAL_PANEL,
    UI_CLOSE_MATERIAL_PANEL, UI_TEX_ALBEDO,
    UI_TEX_METALLIC, UI_TEX_NORMAL,
    UI_TEX_ROUGHNESS, UI_TEX_AO,
    UI_UPLOAD_ZIP, UI_SAVE_MAT,
    UI_UPLOAD_MAT, UI_SHAPE_TEAPOT,
    UI_DOWNLOAD_TEXTURES, UI_WORKFLOW_METALLIC,
    UI_WORKFLOW_SPECULAR, UI_ELEMENT_COUNT
};

struct Camera {
    float radius = 5.0f;
    float yaw = pi / 8.0f;
    float pitch = pi / 12.0f;
    float fov = 45.0f;
    glm::vec3 pos = glm::vec3(sin(pi / 8.0f) * 5.0f, sin(pi / 12.0f) * 5.0f, cos(pi / 8.0f) * 5.0f);

    void updatePos() {
        pos = glm::vec3(sin(yaw) * radius, sin(pitch) * radius, cos(yaw) * radius);
    }
};

struct MaterialTextures {
    unsigned int albedo = 0;
    unsigned int metallic = 0;
    unsigned int normal = 0;
    unsigned int roughness = 0;
    unsigned int ao = 0;
};

struct AppState {
    unsigned int scrWidth = 800;
    unsigned int scrHeight = 600;
    float contentScale = 1.0f;
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    bool isMetallic = true;

    float lastX, lastY;
    bool firstMouse = true;

    std::string error;
    float errorTime = 0.0f;
    std::string tooltip;

    bool highlightingUI = false;
    bool selectingEnv = false;
    bool selectingShape = false;
    bool uploadingEnv = false;
    bool showMaterialUI = false;
    int currentElement = 0;
    int shapeNum = 0;
    char* uploadedEnv = nullptr;

    glm::vec3 extraColors[UI_ELEMENT_COUNT];

    inline float ui(float value) const { return value * contentScale; }

    void setError(const std::string& msg) {
        std::cerr << msg << std::endl;
        error = msg;
        errorTime = 0.0f;
    }
};

Camera cam;
AppState app;
MaterialTextures mat;

struct ScreenRect {
    float x, y, w, h;
    bool contains(float mx, float my) const {
        return mx >= x && mx < x + w && my >= y && my < y + h;
    }
};

ScreenRect boundsFromModel(const glm::mat4& model, float u0 = 0, float v0 = 0, float u1 = 1, float v1 = 1) {
    glm::vec4 corners[4] = {
        model * glm::vec4(u0, v0, 0, 1),
        model * glm::vec4(u1, v0, 0, 1),
        model * glm::vec4(u0, v1, 0, 1),
        model * glm::vec4(u1, v1, 0, 1),
    };
    float minX = corners[0].x, maxX = corners[0].x;
    float minY = corners[0].y, maxY = corners[0].y;
    for (int i = 1; i < 4; ++i) {
        minX = std::min(minX, corners[i].x);
        maxX = std::max(maxX, corners[i].x);
        minY = std::min(minY, corners[i].y);
        maxY = std::max(maxY, corners[i].y);
    }
    return {minX, minY, maxX - minX, maxY - minY};
}

struct UIButton {
    UIElement id;
    std::string tooltip;
    float x, y, w, h;
    unsigned int texture = 0;
    bool visible = true;
    bool flipY = false;

    bool contains(float mx, float my) const {
        return visible && mx >= x && mx < x + w && my >= y && my < y + h;
    }
};

std::vector<UIButton> buttons;
std::vector<UIButton> materialButtons;
ScreenRect materialPanelBounds;
glm::mat4 materialPanelModel(1.0f);

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

std::string getShaders(std::string file){
    std::string resourcePath = toResourcePath(file);
    const auto* resource = EmbeddedResources::getResource(resourcePath);
    if (resource) {
        return std::string(reinterpret_cast<const char*>(resource->data), *resource->size);
    }
    std::ifstream shaderFile(file);
    if (!shaderFile.is_open()) {
        std::cerr<<"Error opening shader file at "<<file<<"\n";
        app.setError("Error opening shader file.");
        return "";
    }
    return std::string((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
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
    if (data){
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
        app.setError("HDR image failed to load.");
    }
    return hdrTexture;
}
static GLenum channelsToFormat(int nrComponents) {
    if (nrComponents == 1) return GL_RED;
    if (nrComponents == 4) return GL_RGBA;
    return GL_RGB;
}

static unsigned int uploadTextureToGL(unsigned char* data, int width, int height, int channels) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    GLenum format = channelsToFormat(channels);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    return textureID;
}

unsigned int loadTexture(std::string file){
    int width, height, nrComponents;
    unsigned char* data = nullptr;

    std::string resourcePath = toResourcePath(file);
    const auto* resource = EmbeddedResources::getResource(resourcePath);
    if (resource) {
        data = stbi_load_from_memory(resource->data, static_cast<int>(*resource->size), &width, &height, &nrComponents, 0);
    } else {
        data = stbi_load(file.c_str(), &width, &height, &nrComponents, 0);
    }

    if (!data){
        std::cerr<<"Texture image failed to load at path "<<file<<std::endl;
        app.setError("Texture image failed to load.");
        return 0;
    }
    unsigned int textureID = uploadTextureToGL(data, width, height, nrComponents);
    stbi_image_free(data);
    return textureID;
}
struct ImageData{
    unsigned char* data = nullptr;
    int width;
    int height;
    int channels;
    size_t dataSize;
    ~ImageData() { delete[] data; }
};
unsigned int loadTexture(ImageData* imageData){
    return uploadTextureToGL(imageData->data, imageData->width, imageData->height, imageData->channels);
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
namespace fs = std::filesystem;
std::vector<std::string> listFilesInDir(const std::string& dirPath, const std::string& extension) {
    std::vector<std::string> files;
    if (!fs::exists(dirPath) || !fs::is_directory(dirPath)) {
        return files;
    }
    for (const auto& entry : fs::directory_iterator(dirPath)) {
        if (entry.is_regular_file()) {
            std::string path = entry.path().string();
            if (extension.empty() || (path.size() >= extension.size() &&
                path.compare(path.size() - extension.size(), extension.size(), extension) == 0)) {
                files.push_back(path);
            }
        }
    }
    std::sort(files.begin(), files.end());
    return files;
}
std::vector<std::string> listSubdirsWithFile(const std::string& dirPath, const std::string& filename) {
    std::vector<std::string> results;
    if (!fs::exists(dirPath) || !fs::is_directory(dirPath)) {
        return results;
    }
    for (const auto& entry : fs::directory_iterator(dirPath)) {
        if (entry.is_directory()) {
            fs::path filePath = entry.path() / filename;
            if (fs::exists(filePath)) {
                results.push_back(filePath.string());
            }
        }
    }
    std::sort(results.begin(), results.end());
    return results;
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
    if (!result.empty()) result.pop_back();
    return result;
}
std::vector<std::string> generateNgrams(const std::string &str, int n){
    std::vector<std::string> ngrams;
    if (str.size()<n) {
        ngrams.push_back(str);
        return ngrams;
    }
    for (size_t i = 0; i <= str.size() - n; ++i){
        ngrams.push_back(str.substr(i,n));
    }
    return ngrams;
}
double computeCosineSimilarity(const std::vector<std::string> &vec1, const std::vector<std::string> &vec2){
    std::unordered_map<std::string, int> freq1, freq2;
    for (const auto &gram : vec1) freq1[gram]++;
    for (const auto &gram : vec2) freq2[gram]++;
    double dot = 0.0, mag1 = 0.0, mag2 = 0.0;
    for (const auto &[gram, count1] : freq1){
        if (freq2.count(gram)) dot += count1 * freq2[gram];
        mag1 += count1 * count1;
    }
    for (const auto &[gram, count2] : freq2){
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
    while (iss >> word){
        if (word == normalizedKeyword) return 1.0;
    }
    if (normalizedKeyword.length() >= 3 && normalizedFilename.find(normalizedKeyword) != std::string::npos){
        return 0.9;
    }
    auto filenameNgrams = generateNgrams(normalizedFilename, 3);
    auto keywordNgrams = generateNgrams(normalizedKeyword, 3);
    return computeCosineSimilarity(filenameNgrams, keywordNgrams) * 0.8;
}
void matchTextures(const std::vector<std::string> &filenames, const std::unordered_map<std::string, int> &textureMap, zip* archive, int numTextures, std::vector<int> &textures){
    int numFiles = filenames.size();
    std::vector<std::vector<double>> probabilities(numFiles, std::vector<double>(numTextures, 0.0));
    for (int i = 0; i < numFiles; ++i) {
        std::string format = filenames[i].substr(filenames[i].size()-3, 3);
        if (format != "png" && format != "jpg" && format != "peg" && format != "gif" && format != "ebp" && format != "bmp" && format != "eif") continue;
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

        for (int i = 0; i < numFiles; ++i) {
            if (assignedFiles[i]) continue;
            for (int j = 0; j < numTextures; ++j) {
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
        if (!zfile){
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
        for (int i = 0; i < numFiles; ++i) {
            if (assignedFiles[i]) continue;
            double sum = 0.0;
            for (int j = 0; j < numTextures; ++j) {
                if (assignedSlots[j]) continue;
                sum += probabilities[i][j];
            }
            if (sum > 0) {
                for (int j = 0; j < numTextures; ++j) {
                    if (!assignedSlots[j]) probabilities[i][j] /= sum;
                }
            }
        }
    }
}
std::pair<std::array<unsigned int, 5>, bool> OpenZipFile(const char* path){
    const int numTextures = 5;
    std::array<unsigned int, 5> textures;
    textures.fill(static_cast<unsigned int>(-1));
    int err = 0;
    zip* archive = zip_open(path, 0, &err);
    bool metallic = true;
    if (!archive) {
        app.setError("Failed to open archive");
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
        for (zip_int64_t i = 0; i < numFiles; ++i){
            const char* filename = zip_get_name(archive, i, 0);
            if (!filename){
                app.setError("Error opening file in zip");
                continue;
            }
            filenames.emplace_back(filename);
        }
        std::vector<int> matchedTextures(numTextures+2, -1);

        matchTextures(filenames, textureMap, archive, numTextures+2, matchedTextures);
        textures[0] = matchedTextures[0];
        textures[2] = matchedTextures[2];
        textures[4] = matchedTextures[4];
        if (matchedTextures[5]!=-1&&matchedTextures[6]!=-1){
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
    if (!success){
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr<<"A vertex shader compilation failed.\n"<<infoLog<<std::endl;
        app.setError("A vertex shader compilation failed.");
    }
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragShaderCStr = fragSource.c_str();
    glShaderSource(fragmentShader, 1, &fragShaderCStr, nullptr);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success){
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr<<"A fragment shader compilation failed.\n"<<infoLog<<std::endl;
        app.setError("A fragment shader compilation failed.");
    }
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success){
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "Linking shader program failed. \n"<<infoLog<<std::endl;
        app.setError("Linking shader program failed.");
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return shaderProgram;
}
unsigned int createCubemap(int size, GLenum minFilter, bool genMipmaps = false) {
    unsigned int cubemap;
    glGenTextures(1, &cubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
    for (unsigned int i = 0; i < 6; ++i){
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB32F, size, size, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if (genMipmaps) glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    return cubemap;
}
void renderCubemapFaces(unsigned int program, unsigned int cubemap, unsigned int VAO, const glm::mat4* views, int mipLevel = 0) {
    for (unsigned int i = 0; i < 6; ++i){
        glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, &views[i][0][0]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubemap, mipLevel);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
    }
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
    unsigned int envCubemap = createCubemap(512, GL_LINEAR_MIPMAP_LINEAR);
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
    renderCubemapFaces(skyProgram, envCubemap, VAO, captureViews);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    unsigned int irradianceMap = createCubemap(32, GL_LINEAR);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);
    glUseProgram(irradianceProgram);
    glUniform1i(glGetUniformLocation(irradianceProgram, "skybox"), 0); 
    glUniformMatrix4fv(glGetUniformLocation(irradianceProgram, "projection"), 1, GL_FALSE, &captureProj[0][0]);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    glViewport(0, 0, 32, 32);
    renderCubemapFaces(irradianceProgram, irradianceMap, VAO, captureViews);
    unsigned int prefilterMap = createCubemap(128, GL_LINEAR_MIPMAP_LINEAR, true);
    glUseProgram(prefilterProgram);
    glUniform1i(glGetUniformLocation(prefilterProgram, "skybox"), 0); 
    glUniformMatrix4fv(glGetUniformLocation(prefilterProgram, "projection"), 1, GL_FALSE, &captureProj[0][0]);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    unsigned int maxMipLevels = 5;
    for (unsigned int mip = 0; mip < maxMipLevels; ++mip){
        unsigned int mipWidth = static_cast<unsigned int>(128 * std::pow(0.5, mip));
        unsigned int mipHeight = static_cast<unsigned int>(128 * std::pow(0.5, mip));
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);
        float roughness = (float)mip / (float)(maxMipLevels - 1);
        glUniform1f(glGetUniformLocation(prefilterProgram, "roughness"), roughness); 
        renderCubemapFaces(prefilterProgram, prefilterMap, VAO, captureViews, mip);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, app.scrWidth, app.scrHeight);
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
    glViewport(0, 0, app.scrWidth, app.scrHeight);
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
            app.setError("Unsupported texture format.");
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
    if (!zip){
        app.setError("Couldn't save zip file");
        return;
    }
    std::unique_ptr<ImageData> albedoData(loadTextureData(albedo));
    std::unique_ptr<ImageData> roughnessData(loadTextureData(roughness));
    std::unique_ptr<ImageData> normalData(loadTextureData(normal));
    std::unique_ptr<ImageData> metalnessData(loadTextureData(metallic));
    std::unique_ptr<ImageData> aoData(loadTextureData(ao));
    if (!albedoData || !roughnessData || !normalData || !metalnessData || !aoData) {
        app.setError("Failed to load one or more texture data.");
        zip_discard(zip);
        return;
    }
    std::vector<std::pair<std::string, std::vector<unsigned char>>> dataBuffer;
    auto processTexture = [&](ImageData* texture, const std::string& filename) {
        int pngSize;
        unsigned char* pngData = stbi_write_png_to_mem(texture->data, texture->width * texture->channels, texture->width, texture->height, texture->channels, &pngSize);
        if (!pngData) {
            app.setError("Couldn't save png to memory");
            return;
        }
        std::lock_guard<std::mutex> lock(zipMutex);
        dataBuffer.emplace_back(filename, std::vector<unsigned char>(pngData, pngData + pngSize));
        stbi_image_free(pngData);
    };
    std::thread t1(processTexture, albedoData.get(), "albedo.png");
    std::thread t2(processTexture, roughnessData.get(), "roughness.png");
    std::thread t3(processTexture, normalData.get(), "normal.png");
    std::thread t4(processTexture, metalnessData.get(), "metallic.png");
    std::thread t5(processTexture, aoData.get(), "ao.png");
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    for (const auto& [filename, pngData] : dataBuffer) {
        int pngSize = sizeof(pngData);
        zip_source_t* source = zip_source_buffer(zip, pngData.data(), pngData.size(), 0);
        if (!source || zip_file_add(zip, filename.c_str(), source, ZIP_FL_OVERWRITE) < 0) {
            zip_source_free(source);
            zip_discard(zip);
            app.setError("Couldn't add PNG to zip file");
            return;
        }
    }
    zip_close(zip);
    return;
}
void writeCustomTextureFile(const char* outputPath, unsigned int albedo, unsigned int roughness, unsigned int normal, unsigned int metalness, unsigned int ao){
    std::unique_ptr<ImageData> albedoData(loadTextureData(albedo));
    std::unique_ptr<ImageData> roughnessData(loadTextureData(roughness));
    std::unique_ptr<ImageData> normalData(loadTextureData(normal));
    std::unique_ptr<ImageData> metalnessData(loadTextureData(metalness));
    std::unique_ptr<ImageData> aoData(loadTextureData(ao));
    if (!albedoData || !roughnessData || !normalData || !metalnessData || !aoData) {
        app.setError("Failed to load one or more texture data.");
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
        app.setError("Failed to open output file.");
        return;
    }
    long long int magicNumber;
    if (app.isMetallic) magicNumber = 0x4D4154455249414C;
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
        if (ZSTD_isError(compressedSize)){
            std::cerr<<"Compression error: "<<ZSTD_getErrorName(compressedSize)<<std::endl;
            app.setError("Error compressing textures");
            return;
        }
        outputFile.write(reinterpret_cast<const char*>(&compressedSize), sizeof(compressedSize));
        outputFile.write(reinterpret_cast<const char*>(compressedData.data()), compressedSize);
    };
    compressWriteTexture(albedoData.get());
    compressWriteTexture(roughnessData.get());
    compressWriteTexture(normalData.get());
    compressWriteTexture(metalnessData.get());
    compressWriteTexture(aoData.get());
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
        if (!inputFile.is_open()){
            app.setError("Failed to load material file.");
            return;
        }
        inputStream = std::make_unique<std::istream>(inputFile.rdbuf());
    }
    
    std::istream& input = *inputStream;
    
    long long int magicNumber;
    input.read(reinterpret_cast<char*>(&magicNumber), sizeof(magicNumber));
    if (magicNumber == 0x4D4154455249414C) app.isMetallic = true;
    else if (magicNumber == 0x53504543554C4152) app.isMetallic = false;
    else{
        app.setError("Invalid file format.");
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
            app.setError("Error decompressing textures");
            delete[] imageData->data;
            delete imageData;
            return nullptr;
        }
        return imageData;
    };
    std::unique_ptr<ImageData> albedoData(loadTextureFromStream(albedoMeta));
    if (!albedoData) return;
    std::unique_ptr<ImageData> roughnessData(loadTextureFromStream(roughnessMeta));
    if (!roughnessData) return;
    std::unique_ptr<ImageData> normalData(loadTextureFromStream(normalMeta));
    if (!normalData) return;
    std::unique_ptr<ImageData> metalnessData(loadTextureFromStream(metalnessMeta));
    if (!metalnessData) return;
    std::unique_ptr<ImageData> aoData(loadTextureFromStream(aoMeta));
    if (!aoData) return;
    albedoID = loadTexture(albedoData.get());
    roughnessID = loadTexture(roughnessData.get());
    normalID = loadTexture(normalData.get());
    metallicID = loadTexture(metalnessData.get());
    aoID = loadTexture(aoData.get());
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
    if (FT_Init_FreeType(&ft)) {
        app.setError("Could not initialize FreeType.");
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
    
    if (ftError){
        app.setError("Could not load font.");
        FT_Done_Face(face);
        FT_Done_FreeType(ft);
        return;
    }
    FT_Set_Pixel_Sizes(face, 0, 48);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    for (unsigned char c = 0; c < 128; ++c){
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)){
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
    glm::mat4 textProj = glm::ortho(0.0f, (float)app.scrWidth, 0.0f, (float)app.scrHeight);
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, &textProj[0][0]);
    glUniform3f(glGetUniformLocation(shader, "textColor"), color.x, color.y, color.z);
    glm::mat4 spriteModel = glm::mat4(1.0f);
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, &spriteModel[0][0]);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); ++c){
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
    
    if (!loaded){
        app.setError("Failed to load OBJ file");
        return;
    }
    objl::Mesh mesh = loader.LoadedMeshes[0];
    glm::vec3 minBound(FLT_MAX, FLT_MAX, FLT_MAX);
    glm::vec3 maxBound(-FLT_MAX, -FLT_MAX, -FLT_MAX);
    for (size_t i = 0; i < mesh.Vertices.size(); ++i){
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
    if (aspectRatio > 1.0f) scaleFactor = 1.8f / maxDistance;
    else scaleFactor = 1.25f / (maxDistance);
    std::vector<float> vertices;
    std::vector<unsigned int> indices = mesh.Indices;
    for (size_t i = 0; i < mesh.Vertices.size(); ++i){
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

std::unordered_map<std::string, std::string> shaders;
void loadShaders() {
    for (const auto& path : listFilesInDir(getAppPath("/shaders"), "")) {
        std::string name = fs::path(path).filename().string();
        shaders[name] = getShaders(path);
    }
}
std::string defaultMatLoc = getAppPath("/material/stainless_steel.mat");
std::string albedoLoc = getAppPath("/material/albedoNot.png");
std::string aoLoc = getAppPath("/material/aoNot.png");
std::string metallicLoc = getAppPath("/material/metallicNot.png");
std::string normalLoc = getAppPath("/material/normalNot.png");
std::string roughnessLoc = getAppPath("/material/roughnessNot.png");
std::vector<std::string> environmentLocs = listSubdirsWithFile(getAppPath("/environments"), "environment.hdr");
std::vector<std::string> uiElementLocs = listFilesInDir(getAppPath("/ui"), ".png");
std::string cubeLoc = getAppPath("/models/cube.obj");
std::string sphereLoc = getAppPath("/models/sphere.obj");
std::string teapotLoc = getAppPath("/models/teapot.obj");
std::string backgroundLoc = getAppPath("/resources/background.png");
void drawSprite(unsigned int program, const glm::mat4& model, const glm::vec3& color, unsigned int texture) {
    glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, &model[0][0]);
    glUniform3fv(glGetUniformLocation(program, "extraColor"), 1, &color[0]);
    glBindTexture(GL_TEXTURE_2D, texture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void layoutButtons(const unsigned int* uiElements) {
    float sw = (float)app.scrWidth;
    float sh = (float)app.scrHeight;

    buttons = {
        {UI_ENV_1, "", app.ui(10.0f), sh - app.ui(60.0f), app.ui(50.0f), app.ui(50.0f), uiElements[UI_ENV_1], true, true},
        {UI_ENV_2, "", app.ui(60.0f), sh - app.ui(60.0f), app.ui(50.0f), app.ui(50.0f), uiElements[UI_ENV_2], true, true},
        {UI_ENV_3, "", app.ui(110.0f), sh - app.ui(60.0f), app.ui(50.0f), app.ui(50.0f), uiElements[UI_ENV_3], true, true},
        {UI_ENV_4, "", app.ui(160.0f), sh - app.ui(60.0f), app.ui(50.0f), app.ui(50.0f), uiElements[UI_ENV_4], true, true},
        {UI_UPLOAD_HDRI, "Upload HDRI environment", app.ui(210.0f), sh - app.ui(60.0f), app.ui(50.0f), app.ui(50.0f), uiElements[UI_UPLOAD_HDRI], true, true},
        {UI_SHAPE_CUBE, "", sw - app.ui(60.0f), sh - app.ui(60.0f), app.ui(50.0f), app.ui(50.0f), uiElements[UI_SHAPE_CUBE], true, true},
        {UI_SHAPE_SPHERE, "", sw - app.ui(110.0f), sh - app.ui(60.0f), app.ui(50.0f), app.ui(50.0f), uiElements[UI_SHAPE_SPHERE], true, true},
        {UI_SHAPE_TEAPOT, "", sw - app.ui(160.0f), sh - app.ui(60.0f), app.ui(50.0f), app.ui(50.0f), uiElements[UI_SHAPE_TEAPOT], true, true},
        {UI_OPEN_MATERIAL_PANEL, "Change material", sw - app.ui(210.0f), sh - app.ui(60.0f), app.ui(50.0f), app.ui(50.0f), uiElements[UI_OPEN_MATERIAL_PANEL], true, true},
    };

    materialButtons.clear();
    if (app.showMaterialUI) {
        materialPanelModel = glm::mat4(1.0f);
        materialPanelModel = glm::translate(materialPanelModel, glm::vec3(sw * 0.085f, sh * 0.95f, 0.0f));
        materialPanelModel = glm::scale(materialPanelModel, glm::vec3(sw / 1.2f, -sh / 1.2f, 1.0f));
        materialPanelBounds = boundsFromModel(materialPanelModel,
            244.0f / 1280.0f, 112.0f / 720.0f, 1039.0f / 1280.0f, 618.0f / 720.0f
        );

        float pl = materialPanelBounds.x;
        float pt = materialPanelBounds.y;
        float pw = materialPanelBounds.w;
        float ph = materialPanelBounds.h;
        float pr = pl + pw;
        float pb = pt + ph;
        float pcx = pl + pw / 2.0f;

        materialButtons = {
            {UI_CLOSE_MATERIAL_PANEL, "", pr - app.ui(30.0f), pt + app.ui(40.0f), app.ui(20.0f), app.ui(20.0f), uiElements[UI_CLOSE_MATERIAL_PANEL], true, false},
            {UI_TEX_ALBEDO, "", pl + app.ui(20.0f), pt + app.ui(40.0f), app.ui(40.0f), app.ui(40.0f), mat.albedo, true, false},
            {UI_TEX_METALLIC, "", pl + app.ui(20.0f), pt + app.ui(90.0f), app.ui(40.0f), app.ui(40.0f), mat.metallic, true, false},
            {UI_TEX_NORMAL, "", pl + app.ui(20.0f), pt + app.ui(140.0f), app.ui(40.0f), app.ui(40.0f), mat.normal, true, false},
            {UI_TEX_ROUGHNESS, "", pl + app.ui(20.0f), pt + app.ui(190.0f), app.ui(40.0f), app.ui(40.0f), mat.roughness, true, false},
            {UI_TEX_AO, "", pl + app.ui(20.0f), pt + app.ui(240.0f), app.ui(40.0f), app.ui(40.0f), mat.ao, true, false},
            {UI_UPLOAD_ZIP, "Upload .zip", pr - app.ui(60.0f), pt + app.ui(65.0f), app.ui(50.0f), app.ui(50.0f), uiElements[UI_UPLOAD_ZIP], true, true},
            {UI_SAVE_MAT, "Save .mat", pr - app.ui(60.0f), pt + app.ui(120.0f), app.ui(50.0f), app.ui(50.0f), uiElements[UI_SAVE_MAT], true, true},
            {UI_UPLOAD_MAT, "Upload .mat", pr - app.ui(60.0f), pt + app.ui(175.0f), app.ui(50.0f), app.ui(50.0f), uiElements[UI_UPLOAD_MAT], true, true},
            {UI_DOWNLOAD_TEXTURES, "Download textures", pr - app.ui(60.0f), pt + app.ui(230.0f), app.ui(50.0f), app.ui(50.0f), uiElements[UI_DOWNLOAD_TEXTURES], true, true},
            {UI_WORKFLOW_METALLIC, "Use metallic workflow", pcx - app.ui(50.0f), pb - app.ui(55.0f), app.ui(100.0f), app.ui(40.0f),
                app.isMetallic ? uiElements[UI_WORKFLOW_SPECULAR] : uiElements[UI_WORKFLOW_METALLIC], true, true},
            {UI_WORKFLOW_SPECULAR, "Use specular workflow", pcx + app.ui(50.0f), pb - app.ui(55.0f), app.ui(100.0f), app.ui(40.0f),
                !app.isMetallic ? uiElements[UI_WORKFLOW_SPECULAR] : uiElements[UI_WORKFLOW_METALLIC], true, true},
        };
    }
}

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
    window = glfwCreateWindow(app.scrWidth, app.scrHeight, "Material Viewer", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Failed to open GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouseCallback);
    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    app.scrWidth = fbWidth;
    app.scrHeight = fbHeight;
    int winWidth, winHeight;
    glfwGetWindowSize(window, &winWidth, &winHeight);
    app.contentScale = (float)fbWidth / (float)winWidth; // for Retina or high-DPI displays

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
    
    loadShaders();
    unsigned int shaderProgram = createShader(shaders["main.vert"], shaders["main.frag"]);

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
    
    unsigned int skyShaderProgram = createShader(shaders["cubemap.vert"], shaders["sky.frag"]);
    unsigned int cubemapShaderProgram = createShader(shaders["cubemap.vert"], shaders["cubemap.frag"]);
    unsigned int irradianceShaderProgram = createShader(shaders["cubemap.vert"], shaders["irradiance.frag"]);
    unsigned int prefilterShaderProgram = createShader(shaders["cubemap.vert"], shaders["prefilter.frag"]);
    unsigned int brdfShaderProgram = createShader(shaders["brdf.vert"], shaders["brdf.frag"]);

    unsigned int envCubemap;
    unsigned int irradianceMap;
    unsigned int prefilterMap;
    unsigned int brdfMap;
    HDRItoCubemap(environmentLocs[app.currentElement], cubemapShaderProgram, irradianceShaderProgram, prefilterShaderProgram, skyVAO, envCubemap, irradianceMap, prefilterMap);
    GetBRDFLUTTexture(brdfShaderProgram, brdfMap);

    readCustomTextureFile(defaultMatLoc, mat.albedo, mat.roughness, mat.normal, mat.metallic, mat.ao);
    if (argc > 1) {
        std::cout << "Opening file: " << argv[1] << std::endl;
        readCustomTextureFile(argv[1], mat.albedo, mat.roughness, mat.normal, mat.metallic, mat.ao);
    } 
    else std::cout << "No file provided." << std::endl;

    for (unsigned int i = 0; i < UI_ELEMENT_COUNT; ++i){
        app.extraColors[i] = glm::vec3(1.0f);
    }
    unsigned int uiElements[UI_ELEMENT_COUNT] = {};
    for (unsigned int i = 0; i < UI_ELEMENT_COUNT; ++i){
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
    unsigned int spriteProgram = createShader(shaders["ui.vert"], shaders["ui.frag"]);
    unsigned int textProgram = createShader(shaders["ui.vert"], shaders["text.frag"]);
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

    float tooltipTime = 0.0f;

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        app.deltaTime = currentFrame - app.lastFrame;
        app.lastFrame = currentFrame;
        processInput(window);

        if (app.selectingEnv){
            app.selectingEnv = false;
            HDRItoCubemap(environmentLocs[app.currentElement], cubemapShaderProgram, irradianceShaderProgram, prefilterShaderProgram, skyVAO, envCubemap, irradianceMap, prefilterMap);
            app.uploadedEnv = nullptr;
        }
        else if (app.selectingShape){
            app.selectingShape = false;
            if (app.currentElement == UI_SHAPE_CUBE) app.shapeNum = 1;
            else if (app.currentElement == UI_SHAPE_SPHERE) app.shapeNum = 0;
            else if (app.currentElement == UI_SHAPE_TEAPOT) app.shapeNum = 2;
        }
        else if (app.uploadingEnv){
            app.uploadingEnv = false;
            HDRItoCubemap(app.uploadedEnv, cubemapShaderProgram, irradianceShaderProgram, prefilterShaderProgram, skyVAO, envCubemap, irradianceMap, prefilterMap);
        }
        
        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        glUseProgram(shaderProgram);

        glm::mat4 projection = glm::perspective(glm::radians(cam.fov), (float)app.scrWidth / (float)app.scrHeight, 0.1f, 100.0f);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);

        glm::mat4 view = glm::lookAt(cam.pos, glm::vec3(0.0,0.0,0.0), glm::vec3(0.0,1.0,0.0));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, &view[0][0]);

        glUniform3fv(glGetUniformLocation(shaderProgram, "camPos"), 1, &cam.pos[0]);

        if (app.shapeNum == 1)
            glBindVertexArray(cubeVAO);
        else if (app.shapeNum == 0)
            glBindVertexArray(sphereVAO);
        else if (app.shapeNum == 2)
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
        glBindTexture(GL_TEXTURE_2D, mat.albedo);
        glUniform1i(glGetUniformLocation(shaderProgram, "albedoMap"), 4);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, mat.metallic);
        glUniform1i(glGetUniformLocation(shaderProgram, "metallicMap"), 5);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, mat.normal);
        glUniform1i(glGetUniformLocation(shaderProgram, "normalMap"), 6);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, mat.roughness);
        glUniform1i(glGetUniformLocation(shaderProgram, "roughnessMap"), 7);
        glActiveTexture(GL_TEXTURE8);
        glBindTexture(GL_TEXTURE_2D, mat.ao);
        glUniform1i(glGetUniformLocation(shaderProgram, "aoMap"), 8);

        if (app.isMetallic) glUniform1i(glGetUniformLocation(shaderProgram, "isMetallic"), 1);
        else glUniform1i(glGetUniformLocation(shaderProgram, "isMetallic"), 0);

        if (app.shapeNum == 1)
            glDrawElements(GL_TRIANGLES, cubeIndexCount, GL_UNSIGNED_INT, 0);
        else if (app.shapeNum == 0)
            glDrawElements(GL_TRIANGLES, sphereIndexCount, GL_UNSIGNED_INT, 0);
        else if (app.shapeNum == 2)
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
        glDisable(GL_DEPTH_TEST);
        
        if (app.error != ""){
            RenderText(textProgram, textVAO, textVBO, app.error, app.ui(10.0f), (float)app.scrHeight - app.ui(20.0f), 0.35f * app.contentScale, glm::vec3(0.8f, 0.2f, 0.2f));
            app.errorTime += app.deltaTime;
            if (app.errorTime >= 7.0f) app.error = "";
        }
        else app.errorTime = 0.0f;
        if (app.tooltip != ""){
            tooltipTime += app.deltaTime;
        }
        else tooltipTime = 0.0f;
        layoutButtons(uiElements);
        glm::mat4 orthoProj = glm::ortho(0.0f, (float)app.scrWidth, (float)app.scrHeight, 0.0f, -1.0f, 1.0f);
        glUseProgram(spriteProgram);
        glUniformMatrix4fv(glGetUniformLocation(spriteProgram, "projection"), 1, GL_FALSE, &orthoProj[0][0]);
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(spriteVAO);
        for (const auto& btn : buttons) {
            if (!btn.visible) continue;
            glm::mat4 model = glm::mat4(1.0f);
            if (btn.flipY) {
                model = glm::translate(model, glm::vec3(btn.x, btn.y + btn.h, 0.0f));
                model = glm::scale(model, glm::vec3(btn.w, -btn.h, 1.0f));
            } else {
                model = glm::translate(model, glm::vec3(btn.x, btn.y, 0.0f));
                model = glm::scale(model, glm::vec3(btn.w, btn.h, 1.0f));
            }
            drawSprite(spriteProgram, model, app.extraColors[btn.id], btn.texture);
        }

        if (app.showMaterialUI){
            drawSprite(spriteProgram, materialPanelModel, glm::vec3(1.0f, 1.0f, 1.0f), uiBackground);
            for (const auto& btn : materialButtons) {
                if (!btn.visible) continue;
                glm::mat4 model = glm::mat4(1.0f);
                if (btn.flipY) {
                    model = glm::translate(model, glm::vec3(btn.x, btn.y + btn.h, 0.0f));
                    model = glm::scale(model, glm::vec3(btn.w, -btn.h, 1.0f));
                } else {
                    model = glm::translate(model, glm::vec3(btn.x, btn.y, 0.0f));
                    model = glm::scale(model, glm::vec3(btn.w, btn.h, 1.0f));
                }
                drawSprite(spriteProgram, model, app.extraColors[btn.id], btn.texture);
            }
            float tpl = materialPanelBounds.x;
            float tpt = materialPanelBounds.y;
            float tpb = tpt + materialPanelBounds.h;
            float tpcx = tpl + materialPanelBounds.w / 2.0f;
            float tsh = (float)app.scrHeight;
            float tx = tpl + app.ui(80.0f);
            RenderText(textProgram, textVAO, textVBO, "Base Color", tx, tsh - (tpt + app.ui(65.0f)), 0.35f * app.contentScale, glm::vec3(0.8f, 0.8f, 0.8f) * app.extraColors[UI_TEX_ALBEDO]);
            if (app.isMetallic){
                RenderText(textProgram, textVAO, textVBO, "Metalness", tx, tsh - (tpt + app.ui(115.0f)), 0.35f * app.contentScale, glm::vec3(0.8f, 0.8f, 0.8f) * app.extraColors[UI_TEX_METALLIC]);
                RenderText(textProgram, textVAO, textVBO, "Roughness", tx, tsh - (tpt + app.ui(215.0f)), 0.35f * app.contentScale, glm::vec3(0.8f, 0.8f, 0.8f) * app.extraColors[UI_TEX_ROUGHNESS]);
            }
            else{
                RenderText(textProgram, textVAO, textVBO, "Specular", tx, tsh - (tpt + app.ui(115.0f)), 0.35f * app.contentScale, glm::vec3(0.8f, 0.8f, 0.8f) * app.extraColors[UI_TEX_METALLIC]);
                RenderText(textProgram, textVAO, textVBO, "Glossiness", tx, tsh - (tpt + app.ui(215.0f)), 0.35f * app.contentScale, glm::vec3(0.8f, 0.8f, 0.8f) * app.extraColors[UI_TEX_ROUGHNESS]);
            }
            RenderText(textProgram, textVAO, textVBO, "Normal Map", tx, tsh - (tpt + app.ui(165.0f)), 0.35f * app.contentScale, glm::vec3(0.8f, 0.8f, 0.8f) * app.extraColors[UI_TEX_NORMAL]);
            RenderText(textProgram, textVAO, textVBO, "Ambient Occlusion", tx, tsh - (tpt + app.ui(265.0f)), 0.35f * app.contentScale, glm::vec3(0.8f, 0.8f, 0.8f) * app.extraColors[UI_TEX_AO]);
            RenderText(textProgram, textVAO, textVBO, "Metallic", tpcx - app.ui(25.0f), tsh - (tpb - app.ui(30.0f)), 0.3f * app.contentScale, glm::vec3(0.8f, 0.8f, 0.8f) * app.extraColors[UI_WORKFLOW_METALLIC]);
            RenderText(textProgram, textVAO, textVBO, "Specular", tpcx + app.ui(75.0f), tsh - (tpb - app.ui(30.0f)), 0.3f * app.contentScale, glm::vec3(0.8f, 0.8f, 0.8f) * app.extraColors[UI_WORKFLOW_SPECULAR]);
            glUseProgram(spriteProgram);
            glBindVertexArray(spriteVAO);
            glUniformMatrix4fv(glGetUniformLocation(spriteProgram, "projection"), 1, GL_FALSE, &orthoProj[0][0]);
            glActiveTexture(GL_TEXTURE0);
        }

        if (app.tooltip != "" && tooltipTime >= 0.5f){
            glUseProgram(spriteProgram);
            glm::mat4 spriteModel = glm::mat4(1.0f);
            spriteModel = glm::translate(spriteModel, glm::vec3(app.lastX - app.ui(2.5f), (float)app.scrHeight - app.lastY - app.ui(2.5f), 0.0f));
            spriteModel = glm::scale(spriteModel, glm::vec3(app.ui(8.5f) * app.tooltip.size(), app.ui(20.0f), 1.0f));
            glm::mat4 textProj = glm::ortho(0.0f, (float)app.scrWidth, 0.0f, (float)app.scrHeight);
            glUniformMatrix4fv(glGetUniformLocation(spriteProgram, "projection"), 1, GL_FALSE, &textProj[0][0]);
            glActiveTexture(GL_TEXTURE0);
            glBindVertexArray(spriteVAO);
            drawSprite(spriteProgram, spriteModel, glm::vec3(0.5f, 0.5f, 0.5f), 0);
            RenderText(textProgram, textVAO, textVBO, app.tooltip, app.lastX, (float)app.scrHeight - app.lastY, 0.35f * app.contentScale, glm::vec3(0.8f, 0.8f, 0.8f));
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
    app.scrWidth = width;
    app.scrHeight = height;
}
void uploadHDRI(){
    char* newEnvironment = OpenFileDialog();
    if (newEnvironment) {
        app.uploadedEnv = newEnvironment;
        app.uploadingEnv = true;
    }
}
void uploadTexture(int tex){
    const char* newTexture = OpenFileDialogTex();
    if (!newTexture) return;
    if (tex==0) mat.albedo = loadTexture(newTexture);
    else if (tex==1) mat.metallic = loadTexture(newTexture);
    else if (tex==2) mat.normal = loadTexture(newTexture);
    else if (tex==3) mat.roughness = loadTexture(newTexture);
    else if (tex==4) mat.ao = loadTexture(newTexture);
}
void uploadZip(){
    char* newZip = OpenFileDialogZip();
    if (newZip){
        std::pair<std::array<unsigned int, 5>, bool> import = OpenZipFile(newZip);
        std::array<unsigned int, 5>& textures = import.first;
        app.isMetallic = import.second;
        if (textures[0] != -1)
            mat.albedo = textures[0];
        else mat.albedo = loadTexture(albedoLoc);
        if (textures[1] != -1)
            mat.metallic = textures[1];
        else mat.metallic = loadTexture(metallicLoc);
        if (textures[2] != -1)
            mat.normal = textures[2];
        else mat.normal = loadTexture(normalLoc);
        if (textures[3] != -1)
            mat.roughness = textures[3];
        else mat.roughness = loadTexture(roughnessLoc);
        if (textures[4] != -1)
            mat.ao = textures[4];
        else mat.ao = loadTexture(aoLoc);
    }
}
void saveToFile(){
    char* matPath = SaveMatFileDialog();
    if (matPath) writeCustomTextureFile(matPath, mat.albedo, mat.roughness, mat.normal, mat.metallic, mat.ao);
}
void uploadMat(){
    char* matPath = OpenFileDialogMaterial();
    if (matPath) readCustomTextureFile(matPath, mat.albedo, mat.roughness, mat.normal, mat.metallic, mat.ao);
}
void downloadTextures(){
    char* zipPath = SaveZipFileDialog();
    if (zipPath){
        saveTexturesToZip(zipPath, mat.albedo, mat.roughness, mat.normal, mat.metallic, mat.ao);
    }
}
void processInput(GLFWwindow *window){
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && app.highlightingUI){
        if (app.currentElement >= 0 && app.currentElement < (int)environmentLocs.size()) {
            app.selectingEnv = true;
        }
        else switch (static_cast<UIElement>(app.currentElement)) {
            case UI_SHAPE_CUBE: case UI_SHAPE_SPHERE: case UI_SHAPE_TEAPOT:
                app.selectingShape = true; break;
            case UI_UPLOAD_HDRI: uploadHDRI(); break;
            case UI_OPEN_MATERIAL_PANEL: app.showMaterialUI = true; break;
            case UI_CLOSE_MATERIAL_PANEL: app.showMaterialUI = false; break;
            case UI_TEX_ALBEDO: case UI_TEX_METALLIC: case UI_TEX_NORMAL:
            case UI_TEX_ROUGHNESS: case UI_TEX_AO:
                uploadTexture(app.currentElement - UI_TEX_ALBEDO); break;
            case UI_UPLOAD_ZIP: uploadZip(); break;
            case UI_SAVE_MAT: saveToFile(); break;
            case UI_UPLOAD_MAT: uploadMat(); break;
            case UI_DOWNLOAD_TEXTURES: downloadTextures(); break;
            case UI_WORKFLOW_METALLIC: app.isMetallic = true; break;
            case UI_WORKFLOW_SPECULAR: app.isMetallic = false; break;
            default: break;
        }
    }
}
void hoverElement(int elementNum){
    for (unsigned int i = 0; i < UI_ELEMENT_COUNT; ++i){
        app.extraColors[i] = glm::vec3(1.0f);
    }
    if (elementNum != -1){
        app.extraColors[elementNum] = glm::vec3(1.3f, 1.3f, 1.6f);
        app.currentElement = elementNum;
    }
    else app.tooltip = "";
    return;
}
void mouseCallback(GLFWwindow* window, double xposIn, double yposIn){
    double xpos = xposIn * app.contentScale;
    double ypos = yposIn * app.contentScale;
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
        app.lastX = xpos;
        app.lastY = ypos;
        app.firstMouse = true;
        app.highlightingUI = false;
        if (app.showMaterialUI){
            if (!materialPanelBounds.contains(xpos, ypos)){
                hoverElement(-1);
                app.currentElement = UI_CLOSE_MATERIAL_PANEL;
                app.highlightingUI = true;
            } else {
                for (const auto& btn : materialButtons) {
                    if (btn.contains(xpos, ypos)) {
                        hoverElement(btn.id);
                        app.highlightingUI = true;
                        if (!btn.tooltip.empty()) app.tooltip = btn.tooltip;
                        break;
                    }
                }
            }
        } else {
            for (const auto& btn : buttons) {
                if (btn.contains(xpos, ypos)) {
                    hoverElement(btn.id);
                    app.highlightingUI = true;
                    if (!btn.tooltip.empty()) app.tooltip = btn.tooltip;
                    break;
                }
            }
        }
        if (!app.highlightingUI) hoverElement(-1);
        return;
    }
    float xposf = static_cast<float>(xpos);
    float yposf = static_cast<float>(ypos);
    if (app.firstMouse){
        app.lastX = xposf;
        app.lastY = yposf;
        app.firstMouse = false;
    }
    float xOffset = xposf - app.lastX;
    float yOffset = app.lastY - yposf;
    app.lastX = xposf;
    app.lastY = yposf;
    float sensitivity = 0.007f;
    xOffset *= sensitivity;
    yOffset *= sensitivity;
    cam.yaw += xOffset;
    cam.pitch += yOffset;
    cam.pitch = std::clamp(cam.pitch, -1.57f, 1.57f);
    float camX = sin(cam.yaw) * cam.radius;
    float camY = sin(cam.pitch) * cam.radius;
    float camZ = cos(cam.yaw) * cam.radius;
    cam.pos = glm::vec3(camX, camY, camZ);
}
