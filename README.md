<!-- Improved compatibility of back to top link: See: https://github.com/othneildrew/Best-README-Template/pull/73 -->
<a id="readme-top"></a>
<!--
*** Thanks for checking out the Best-README-Template. If you have a suggestion
*** that would make this better, please fork the repo and create a pull request
*** or simply open an issue with the tag "enhancement".
*** Don't forget to give the project a star!
*** Thanks again! Now go create something AMAZING! :D
-->



<!-- PROJECT SHIELDS -->
<!--
*** I'm using markdown "reference style" links for readability.
*** Reference links are enclosed in brackets [ ] instead of parentheses ( ).
*** See the bottom of this document for the declaration of the reference variables
*** for contributors-url, forks-url, etc. This is an optional, concise syntax you may use.
*** https://www.markdownguide.org/basic-syntax/#reference-style-links
-->
[![Contributors][contributors-shield]][contributors-url]
[![Forks][forks-shield]][forks-url]
[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]
[![MIT License][license-shield]][license-url]
[![LinkedIn][linkedin-shield]][linkedin-url]



<!-- PROJECT LOGO -->
<br />
<div align="center">

<a href="https://github.com/josephHelfenbein/material-viewer">
    <img src="src/resources/materialviewer-logo.svg" alt="Logo" width="80" height="80">
  </a>

<h3 align="center">Material Viewer</h3>

  <p align="center">
    A quick way to preview PBR materials
    <br />
    <br />
    <a href="https://github.com/josephHelfenbein/material-viewer/issues/new?labels=bug&template=bug-report---.md">Report Bug</a>
    ·
    <a href="https://github.com/josephHelfenbein/material-viewer/issues/new?labels=enhancement&template=feature-request---.md">Request Feature</a>
  </p>
</div>



<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li><a href="#about-the-project">About The Project</a></li>
    <li><a href="#installation">Installation</a></li>
    <li><a href="#prerequisites">Prerequisites</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#acknowledgments">Acknowledgments</a></li>
  </ol>
</details>



<!-- ABOUT THE PROJECT -->
## About The Project

A 3D desktop application to preview PBR materials made using OpenGL and C++. Featuring HDRI environments, image-based lighting, and PBR shading. You can upload individual image textures, upload a custom .mat file, or a .zip to preview the material. You can also save the material you're viewing to the custom .mat filetype for easy previewing and distribution of a PBR material.

## Screenshots
![screenshot](https://github.com/josephHelfenbein/material-viewer/blob/521a4a78ac07ad73d460f9166157c720a0adae96/src/resources/pbr.PNG)
![screenshot](https://github.com/josephHelfenbein/material-viewer/blob/521a4a78ac07ad73d460f9166157c720a0adae96/src/resources/pbr2.PNG)
![screenshot](https://github.com/josephHelfenbein/material-viewer/blob/521a4a78ac07ad73d460f9166157c720a0adae96/src/resources/pbr3.PNG)

### Built With

* [![C++][C++]][c++-url]
* [![OpenGL][OpenGL]][OpenGL-url]



See the [open issues](https://github.com/josephHelfenbein/material-viewer/issues) for a full list of proposed features (and known issues).

## Installation

To install **Material Viewer**, download the appropriate file from the [Releases](https://github.com/josephHelfenbein/material-viewer/releases) tab based on your operating system.

### Windows

1. **Binary**: Download `MaterialViewer.exe` and run it directly. No installation required.
2. **Installer**: Download `MaterialViewerInstaller.exe` and follow the setup wizard for a guided installation.

### Linux

1. **Binary (AppImage)**:
    - Download `MaterialViewer-x86_64.AppImage`.
    - Make it executable:
   ```bash
   chmod +x MaterialViewer-x86_64.AppImage
   ```
    - Run the AppImage directly:
   ```bash
   ./MaterialViewer-x86_64.AppImage
   ```
2. **Installer**:
    - Download `MaterialViewerLinuxInstall.zip` and extract it.
    - Run the installation script as root:
   ```bash
   unzip MaterialViewerLinuxInstall.zip
   cd MaterialViewerLinuxInstall
   sudo ./install.sh
   ```
     

## Prerequisites

### Windows

To compile the project on Windows, you'll need:
1. **MSYS2** - Install [MSYS2](https://www.msys2.org/) and set up the `ucrt64` environment for access to the necessary libraries and compiler.
    - Ensure `g++` is available at `C:\msys64\ucrt64\bin\g++.exe`.
    Add an envionment variable to your system: `C:\msys64\ucrt64\bin`
2. **Libraries** - Install the following libraries using the MSYS2 package manager:
    - `mingw-w64-ucrt-x86_64-libzip` (for handling zip archives)
    - `mingw-w64-ucrt-x86_64-zstd` (for Zstandard compression)
    - `mingw-w64-ucrt-x86_64-freetype` (for font rendering)
    - `mingw-w64-ucrt-x86_64-glfw` (for creating windows, OpenGL contexts)
    - `mingw-w64-ucrt-x86_64-cmake` (for building)
3. Run the `Debug MaterialViewer (Windows)` configuration in VSCode.

### Linux

To compile the project on Linux, you'll need:
1. **G++** - Install the `g++` compiler, version supporting at least C++ 17.
2. **Libraries** - Use your package manager to install:
    - `libzip` (for handling zip archives)
    - `libzstd-dev` (for Zstandard compression)
    - `libfreetype6-dev` (for font rendering)
    - `libglfw3-dev` (for creating windows, OpenGL contexts)
    - `qtbase5-dev` (for Qt core libraries)
    - `qt5-qmake` (for compiling Qt applications)
    - `libqt5widgets5`, `libqt5gui5`, `libqt5core5a` (for UI elements)
    - `cmake` (for building)
3. Run the `Debug MaterialViewer (Linux)` configuration in VSCode.

<!-- LICENSE -->
## License

Distributed under the MIT License. See `LICENSE.txt` for more information.


<!-- ACKNOWLEDGMENTS -->
## Acknowledgments

* [Best README Template](https://github.com/othneildrew/Best-README-Template)
* [Learn OpenGL](https://learnopengl.com/)


<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[contributors-shield]: https://img.shields.io/github/contributors/josephHelfenbein/material-viewer.svg?style=for-the-badge
[contributors-url]: https://github.com/josephHelfenbein/material-viewer/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/josephHelfenbein/material-viewer.svg?style=for-the-badge
[forks-url]: https://github.com/josephHelfenbein/material-viewer/network/members
[stars-shield]: https://img.shields.io/github/stars/josephHelfenbein/material-viewer.svg?style=for-the-badge
[stars-url]: https://github.com/josephHelfenbein/material-viewer/stargazers
[issues-shield]: https://img.shields.io/github/issues/josephHelfenbein/material-viewer.svg?style=for-the-badge
[issues-url]: https://github.com/josephHelfenbein/material-viewer/issues
[license-shield]: https://img.shields.io/github/license/josephHelfenbein/material-viewer.svg?style=for-the-badge
[license-url]: https://github.com/josephHelfenbein/material-viewer/blob/master/LICENSE.txt
[linkedin-shield]: https://img.shields.io/badge/-LinkedIn-black.svg?style=for-the-badge&logo=linkedin&colorB=555
[linkedin-url]: https://linkedin.com/in/joseph-j-helfenbein
[product-screenshot]: images/screenshot.png
[Next.js]: https://img.shields.io/badge/next.js-000000?style=for-the-badge&logo=nextdotjs&logoColor=white
[Next-url]: https://nextjs.org/
[React.js]: https://img.shields.io/badge/React-20232A?style=for-the-badge&logo=react&logoColor=61DAFB
[React-url]: https://reactjs.org/
[Vue.js]: https://img.shields.io/badge/Vue.js-35495E?style=for-the-badge&logo=vuedotjs&logoColor=4FC08D
[Vue-url]: https://vuejs.org/
[Angular.io]: https://img.shields.io/badge/Angular-DD0031?style=for-the-badge&logo=angular&logoColor=white
[Angular-url]: https://angular.io/
[Svelte.dev]: https://img.shields.io/badge/Svelte-4A4A55?style=for-the-badge&logo=svelte&logoColor=FF3E00
[Svelte-url]: https://svelte.dev/
[Laravel.com]: https://img.shields.io/badge/Laravel-FF2D20?style=for-the-badge&logo=laravel&logoColor=white
[Laravel-url]: https://laravel.com
[Bootstrap.com]: https://img.shields.io/badge/Bootstrap-563D7C?style=for-the-badge&logo=bootstrap&logoColor=white
[Bootstrap-url]: https://getbootstrap.com
[JQuery.com]: ?style=for-the-badge&logo=jquery&logoColor=whitehttps://img.shields.io/badge/jQuery-0769AD
[JQuery-url]: https://jquery.com 
[C++]: https://img.shields.io/badge/c++-00599C?logo=cplusplus&style=for-the-badge&logoColor=white
[c++-url]: https://developer.oracle.com/languages/javascript.html
[OpenGL]: https://img.shields.io/badge/opengl-5586A4?logo=opengl&style=for-the-badge&logoColor=white
[OpenGL-url]: https://www.khronos.org/webgl/
