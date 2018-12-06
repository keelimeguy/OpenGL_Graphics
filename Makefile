CUR_DIR=./
TOP_DIR=${CUR_DIR}
SRC_DIR=${TOP_DIR}OpenGL_Graphics/src/
OBJ_DIR=${CUR_DIR}obj/
BUILD_DIR=${CUR_DIR}build/

SOURCES = $(shell find ${SRC_DIR} -name '*.cpp' | sort -k 1nr | cut -f2- | sed -e 's|${SRC_DIR}||')
OBJECTS = $(subst .o ,.o ${OBJ_DIR},${OBJ_DIR}$(SOURCES:%.cpp=%.o))

INCLUDE=-I${SRC_DIR} -isystem ${SRC_DIR}vendor/ -isystem ${TOP_DIR}Dependencies/GLEW/include/ -isystem ${TOP_DIR}Dependencies/GLFW/include/
LINCLUDE=-L${TOP_DIR}Dependencies/GLEW/mingw64/ -L${TOP_DIR}Dependencies/GLFW/lib-mingw-w64/
LIB=-lglew32 -lglfw3 -lopengl32 -luser32 -lGdi32 -lShell32
FLAGS=${INCLUDE} -std=c++17 -DIMGUI_IMPL_OPENGL_LOADER_GLEW -DGLEW_STATIC

app: $(OBJECTS)
	mkdir -p ${BUILD_DIR}
	g++ $(OBJECTS) ${FLAGS} ${LINCLUDE} ${LIB} -o ${BUILD_DIR}run
	cp ${TOP_DIR}Dependencies/GLEW/mingw64/glew32.dll ${BUILD_DIR}
	cp -R ${TOP_DIR}OpenGL_Graphics/res ${BUILD_DIR}

${OBJ_DIR}%.o: ${SRC_DIR}%.cpp
	mkdir -p $(dir $@)
	g++ $(FLAGS) -c $< -o $@

clean:
	find ${OBJ_DIR} -path ${OBJ_DIR}vendor -prune -o -name \*.o -type f -exec rm -f {} +
	rm -f ${BUILD_DIR}run
