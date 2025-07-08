# Зависимости и настройка проект

Движок написан на Visual C++ под платформу Windows, использовалась среда разработки Visual Studio 2022.


##Используемые библиотеки:
1) glfw: https://www.glfw.org/
2) glm: https://github.com/g-truc/glm
3) glew: https://glew.sourceforge.net/
4) audiere: https://audiere.sourceforge.net/audiere-1.9.4-users-doxygen/
5) stb: https://github.com/nothings/stb
6) freetype: https://freetype.org/
7) assimp: https://assimp.org/

По поводу assimp: вам помимо статической библиотеки (.lib) нужна будет и динамическая (.dll).

Все библиотеки поместите в папку Libraries.

Свойства конфигурации->Общие
1) Стандарт языка С++: Стандарт ISO C++ 20 (/std:c++20)
2) Стандарт языка С: Стандарт ISO C17 (2018) (/std:c17)
   
Свойства конфигурации->Каталоги VC++
Включаемые каталоги: 
1) include
2) Libraries\Assimp.3.0.0\build\native\include
3) Libraries\audiere-1.9.4\include
4) Libraries\freetype2133\include
5) Libraries\glew220\include
6) Libraries\glfw34WIN64\include
7) Libraries\glm.1.0.1\build\native\include
8) Libraries\stb-master\
9) include\InnerLibraries\

Свойства конфигурации->С/С++->Препроцессор
Определения препроцессора:
1)GLEW_STATIC;

