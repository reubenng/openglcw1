--
-- COMP3214 Coursework 1 configuration
--
-- Name: Reuben Ng (rdcn1g14)
--
print("Config.lua")
GLFW = 3 -- use 2 (GLFW 2), 3 (glfw 3) or 1 (freeglut, not tested yet)
GLew = true -- must use this.
IMAGE = 'stb_image'
STD = 'c++11'
BOOST = false -- nil, false or true.
BULLET = false -- nil, false or true.
MODEL = false -- nil, false or true.
GUI = 'ImGui' -- ImGui or  nil
FILES = {'./src/*.cpp'} -- nil or list of files. nil is equivalent to {'.cpp'}.