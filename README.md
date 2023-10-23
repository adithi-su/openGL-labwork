# OpenGL-notes

### Objects
- represents a subset of openGL's state
- ex: object with settings of the drawing window -> we could then set its size/how many colors it supports etc.

```
// The State of OpenGL
struct OpenGL_Context {
  	...
  	object_name* object_Window_Target;
  	...  	
};
```
```
// create object
unsigned int objectId = 0;
glGenObject(1, &objectId);
// bind/assign object to context
glBindObject(GL_WINDOW_TARGET, objectId);
// set options of object currently bound to GL_WINDOW_TARGET
glSetObjectOption(GL_WINDOW_TARGET, GL_OPTION_WINDOW_WIDTH,  800);
glSetObjectOption(GL_WINDOW_TARGET, GL_OPTION_WINDOW_HEIGHT, 600);
// set context target back to default
glBindObject(GL_WINDOW_TARGET, 0);
```
- We first create an object and store a reference to it as an id (the real object's data is stored behind the scenes).
- Then we bind the object (using its id) to the target location of the context (the location of the example window object target is defined as GL_WINDOW_TARGET).
- Next we set the window options and finally we un-bind the object by setting the current object id of the window target to 0.
- The options we set are stored in the object referenced by objectId and restored as soon as we bind the object back to GL_WINDOW_TARGET.

### Creating a window
- create a window to draw in, define a context and handle user input by ourselves cause these operations are OS specific
- use libraries like GLUT, SDL, SFML and GLFW.

- link glfw3.lib in the linker settings for VS Code
  
```
#include <glad/glad.h>
#include <GLFW/glfw3.h>
``` 

**NOTE**: 
- install glfw and cmake
- include GLAD before GLFW. The include file for GLAD includes the required OpenGL headers behind the scenes (like GL/gl.h)

#### Hello Window
```
int main()
{
    glfwInit(); // intialize GLFW
    //setting context version to OpenGl 3.3 (major.minor)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // (option to configure (prefixed with GLFW_), option value)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  
    return 0;
}
```

- find options and corresponding values at <a href="http://www.glfw.org/docs/latest/window.html#window_hints">GLFW's window handling documentation</a>

- creating the window object
```
GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
if (window == NULL)
{
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
}
glfwMakeContextCurrent(window);
```

- Tell OpenGL the size of the rendering window so OpenGL knows how we want to display the data and coordinates with respect to the window.
```
glViewport(0, 0, 800, 600);
```
- first 2 parameters - set location of lower left corner of the window
- third and fourth - width and height of rendering window in pixels = GLFW's window size
- also check framebuffer_size_callback - for resizing

- for the application to keep rendering and handling user input until explicity specified to stop - create a while loop (render loop)
```
while(!glfwWindowShouldClose(window))
{
    glfwSwapBuffers(window); //swap the color buffer (a large 2D buffer that contains color values for each pixel in GLFW's window) that is used to render to during this render iteration and show it as output to the screen
    glfwPollEvents();  //checks if any events are triggered (like keyboard input or mouse movement events), updates the window state, and calls the corresponding functions (which we can register via callback methods)
}
```

- clean/delete resources - call at the end of the main function
```
glfwTerminate();
return 0;
```

#### Input
```
void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
```
- check whether the user has pressed the escape key (if it's not pressed, glfwGetKey returns GLFW_RELEASE).
- If the user did press the escape key, we close GLFW by setting its WindowShouldClose property to true using glfwSetwindowShouldClose.
- The next condition check of the main while loop will then fail and the application closes.
- call processInput every iteration of the render loop:
```
while (!glfwWindowShouldClose(window))
{
    processInput(window);

    glfwSwapBuffers(window);
    glfwPollEvents();
}  
```

#### Rendering
```
// render loop
while(!glfwWindowShouldClose(window))
{
    // input
    processInput(window);

    // rendering commands here
    ...

    // check and call events and swap the buffers
    glfwPollEvents();
    glfwSwapBuffers(window);
}
```
### Hello Triangle
- OpenGL works with 3D space but screen/window is a 2D array of pixels - transforming 3D->2D, 2D coordinates -> actual colored pixels - done by graphics pipeline
- Processing cores run small programs (**shaders**) on the GPU for each step of the pipeline.
- Stages of the graphics pipeline:
  
<img src="https://learnopengl.com/img/getting-started/pipeline.png">

*blue section represents sections where our own shaders can be injected
- vertex shader input <- single vertex, output -> to geometry shader
- The primitive assembly stage takes as input all the vertices (or vertex if GL_POINTS is chosen) from the vertex (or geometry) shader that form one or more primitives and assembles all the point(s) in the primitive shape given; in this case two triangles.
- geometry shader is optional and usually set to default.
- rasterization stage -  maps the resulting primitive(s) to the corresponding pixels on the final screen, resulting in fragments for the fragment shader to use.
- Before the fragment shaders run, **clipping** is performed. Clipping discards all fragments that are outside your view, increasing performance.
- fragment shader - to calculate the final color of a pixel and this is usually the stage where all the advanced OpenGL effects occur; contains data about the 3D scene that it can use to calculate the final pixel color (like lights, shadows, color of the light and so on).
- the final object will then pass through one more stage that we call the **alpha test** and **blending stage**. This stage checks the corresponding depth (and stencil) value of the fragment and uses those to check if the resulting fragment is in front or behind other objects and should be discarded accordingly. The stage also checks for alpha values (alpha values define the opacity of an object) and blends the objects accordingly. So even if a pixel output color is calculated in the fragment shader, the final pixel color could still be something entirely different when rendering multiple triangles.

**NOTE**:
- A fragment in OpenGL is all the data required for OpenGL to render a single pixel.
- In modern OpenGL we are required to define at least a vertex and fragment shader of our own (there are no default vertex/fragment shaders on the GPU).

#### Vertex input
- normalized device coordinates
```
float vertices[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
};  
```
- NDC coordinates will be transformed to screen-space coordinates via the viewport transform using the data you provided with glViewport. The resulting screen-space coordinates are then transformed to fragments as inputs to your fragment shader.
- send input to vertex shader -> by creating memory on the GPU to store vertex data, configure how OpenGL should interpret the memory and specify how to send the data to the graphics card.
- manage this memory via **vertex buffer objects (VBO)** - can send large batches of data all at once to the graphics card, and keep it there if there's enough memory left, without having to send data one vertex at a time.
```
unsigned int VBO;
glGenBuffers(1, &VBO);  
```
-  bind the newly created buffer to the GL_ARRAY_BUFFER target with the glBindBuffer function:
```
glBindBuffer(GL_ARRAY_BUFFER, VBO);  
```
- From that point on any buffer calls we make (on the GL_ARRAY_BUFFER target) will be used to configure the currently bound buffer, which is VBO.
- Then we can make a call to the glBufferData function that copies the previously defined vertex data into the buffer's memory:
```
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
```
  - GL_STREAM_DRAW: the data is set only once and used by the GPU at most a few times.
  - GL_STATIC_DRAW: the data is set only once and used many times.
  - GL_DYNAMIC_DRAW: the data is changed a lot and used many times.

#### Vertex shader
- basic vertex shader is GLSL:
```
#version 330 core
layout (location = 0) in vec3 aPos;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
```
- At the end of the main function, whatever we set gl_Position to will be used as the output of the vertex shader.
- Since our input is a vector of size 3 we have to cast this to a vector of size 4. We can do this by inserting the vec3 values inside the constructor of vec4 and set its w component to 1.0f

#### Compiling a shader
```

```
