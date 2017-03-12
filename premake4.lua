solution "Coursework1"
    configurations "Release"
        links "glfw3"
        links "glew32"
        links "opengl32"
        links{'rt', 'm', 'dl'}
        project "Coursework1"
        	location "./build"
            kind "ConsoleApp"
            language "C++"
            files {"./src/*.cpp"}
            configuration "Release"