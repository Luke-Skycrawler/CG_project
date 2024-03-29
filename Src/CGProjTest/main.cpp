#include <iostream>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"
#include "mesh.h"
#include "model.h"
#include "light.h"

#include "utils.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

bool postrender=false, edge = false, skybox=false,model_draw=false;


//windows
GLFWwindow* window = NULL;

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;


int init();
void TestDEMO();

int main()
{
    if (!init())
        exit(1);

    TestDEMO();

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}


int init()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return 0;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return 0;
    }


    return 1;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        postrender=!postrender;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        edge=!edge;
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
        skybox=!skybox;
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
        model_draw=!model_draw;
    // if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

void TestDEMO()
{

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    /*Shader lightingShader("shaders/1.color.vs", "shaders/1.color.fs");
    Shader lightCubeShader("shaders/1.light_cube.vs", "shaders/1.light_cube.fs");*/
    //Shader lightingShader("shaders/multi_light.vs", "shaders/multi_light.fs");
    //Shader lightCubeShader("shaders/light_cube.vs", "shaders/light_cube.fs");
    Shader simpleShader("shaders/1.color.vs", "shaders/simple.fs");
    Shader screenShader("shaders/view.vs", "shaders/core.fs");
    Shader skyboxShader("shaders/skycube.vs", "shaders/skycube.fs");
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.3f,  0.2f,  2.0f),
        glm::vec3(2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -1.0f),
        glm::vec3(0.0f,  0.0f, -3.0f),
        glm::vec3(2.0f,  3.0f, 1.0f)
    };

    Light mutilLight = Light(pointLightPositions, 5);


    // first, configure the cube's VAO (and VBO)
    unsigned int VBO, cubeVAO, quadVBO, quadVAO;
    float quad[] = {
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);


    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), &quad, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);


    // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    // we only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need (it's already bound, but we do it again for educational purposes)
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);



    // load models 
    Model temple("nanosuit/nanosuit.obj");
    // load textures (we now use a utility function to keep the code more organized)
    // -----------------------------------------------------------------------------
    unsigned int diffuseMap = loadTexture("container2.png");
    unsigned int specularMap = loadTexture("container2_specular.png");
    //--------cube texture
    std::vector<std::string> faces
    {
        "skybox/right.jpg",
        "skybox/left.jpg",
        "skybox/top.jpg",
        "skybox/bottom.jpg",
        "skybox/front.jpg",
        "skybox/back.jpg"
    };
    unsigned int cubemapTexture = loadCubemap(faces);

    screenShader.use();
    screenShader.setInt("screenTexture", 0);
    unsigned int framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    unsigned int texColorBuffer;
    glGenTextures(1, &texColorBuffer);
    glBindTexture(GL_TEXTURE_2D, texColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texColorBuffer, 0);

    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "error: framebuffer\n";
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    // shader configuration
    // --------------------
    mutilLight.lightingShader.use();
    mutilLight.lightingShader.setInt("material.diffuse", 0);
    mutilLight.lightingShader.setInt("material.specular", 1);

    mutilLight.lightingShader.setFloat("material.shininess", 64);
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----

        processInput(window);
        // glBindFramebuffer(GL_FRAMEBUFFER,framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, postrender ? framebuffer : 0);
        // glBindFramebuffer(GL_FRAMEBUFFER,0);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_STENCIL_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        glStencilFunc(GL_ALWAYS, 1, 0XFF);
        glStencilMask(0XFF);
        mutilLight.lightingShader.use();

        // render
        // ------
        // glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

         // bind diffuse map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        // bind specular map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        // render the cube
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);


        glm::mat4 model = glm::mat4(1.0f);
        ///*
        //   Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index
        //   the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
        //   by defining light types as classes and set their values in there, or by using a more efficient uniform approach
        //   by using 'Uniform buffer objects', but that is something we'll discuss in the 'Advanced GLSL' tutorial.
        //*/
        //// directional light
        //lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        //lightingShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        //lightingShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        //lightingShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
        //// point light 1
        //lightingShader.setVec3("pointLights[0].position", pointLightPositions[0]);
        //lightingShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
        //lightingShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
        //lightingShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        //lightingShader.setFloat("pointLights[0].constant", 1.0f);
        //lightingShader.setFloat("pointLights[0].linear", 0.09);
        //lightingShader.setFloat("pointLights[0].quadratic", 0.032);
        //// point light 2
        //lightingShader.setVec3("pointLights[1].position", pointLightPositions[1]);
        //lightingShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
        //lightingShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
        //lightingShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
        //lightingShader.setFloat("pointLights[1].constant", 1.0f);
        //lightingShader.setFloat("pointLights[1].linear", 0.09);
        //lightingShader.setFloat("pointLights[1].quadratic", 0.032);
        //// point light 3
        //lightingShader.setVec3("pointLights[2].position", pointLightPositions[2]);
        //lightingShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
        //lightingShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
        //lightingShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
        //lightingShader.setFloat("pointLights[2].constant", 1.0f);
        //lightingShader.setFloat("pointLights[2].linear", 0.09);
        //lightingShader.setFloat("pointLights[2].quadratic", 0.032);
        //// point light 4
        //lightingShader.setVec3("pointLights[3].position", pointLightPositions[3]);
        //lightingShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
        //lightingShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
        //lightingShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
        //lightingShader.setFloat("pointLights[3].constant", 1.0f);
        //lightingShader.setFloat("pointLights[3].linear", 0.09);
        //lightingShader.setFloat("pointLights[3].quadratic", 0.032);
        //// spotLight
        //lightingShader.setVec3("spotLight.position", camera.Position);
        //lightingShader.setVec3("spotLight.direction", camera.Front);
        //lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        //lightingShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        //lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        //lightingShader.setFloat("spotLight.constant", 1.0f);
        //lightingShader.setFloat("spotLight.linear", 0.09);
        //lightingShader.setFloat("spotLight.quadratic", 0.032);
        //lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        //lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

        //// view/projection transformations
        //glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        //glm::mat4 view = camera.GetViewMatrix();
        //lightingShader.setMat4("projection", projection);
        //lightingShader.setMat4("view", view);

        //// world transformation
        //model = glm::mat4(1.0f);
        //lightingShader.setMat4("model", model);


        //glm::vec3 box2Pos(0.3, 0.0, 1.2);
        //lightingShader.use();
        //model = glm::translate(model, box2Pos);
        //lightingShader.setMat4("model", model);
        //glDrawArrays(GL_TRIANGLES, 0, 36);
        //model = glm::translate(model, box2Pos);

        // // also draw the lamp object(s)
        //lightCubeShader.use();
        //lightCubeShader.setMat4("projection", projection);
        //lightCubeShader.setMat4("view", view);

        //// we now draw as many light bulbs as we have point lights.
        //glBindVertexArray(lightCubeVAO);
        //for (unsigned int i = 0; i < 4; i++)
        //{
        //    model = glm::mat4(1.0f);
        //    model = glm::translate(model, pointLightPositions[i]);
        //    model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
        //    lightCubeShader.setMat4("model", model);
        //    glDrawArrays(GL_TRIANGLES, 0, 36);
        //}

        mutilLight.SetShaderValue(camera, lightCubeVAO, model);

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        float scale = 1.02;
        glm::mat4 tmpmodel = glm::scale(model, glm::vec3(scale, scale, scale));

        if (model_draw) {
            mutilLight.lightingShader.use();
            temple.Draw(mutilLight.lightingShader);
        }
        if (skybox) {
            glStencilMask(0x00);
            // skybox
            // glDepthMask(GL_FALSE);
            glm::mat4 skyview = glm::mat4(glm::mat3(view));
            glDepthFunc(GL_LEQUAL);
            skyboxShader.use();
            skyboxShader.setMat4("projection", projection);
            skyboxShader.setMat4("view", skyview);
            glBindVertexArray(cubeVAO);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            // glDepthMask(GL_TRUE);
            glDepthFunc(GL_LESS);
        }
        if (edge) {
            glStencilFunc(GL_NOTEQUAL, 1, 0XFF);
            // glStencilMask(0x00);
            glDisable(GL_DEPTH_TEST);
            simpleShader.use();
            simpleShader.setMat4("projection", projection);
            simpleShader.setMat4("view", view);
            simpleShader.setMat4("model", tmpmodel);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glStencilMask(0xFF);
            glEnable(GL_DEPTH_TEST);
            glStencilFunc(GL_ALWAYS, 1, 0XFF);
        }


        if (postrender) {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glDisable(GL_DEPTH_TEST);
            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            screenShader.use();
            glBindVertexArray(quadVAO);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texColorBuffer);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
    glDeleteBuffers(1, &VBO);
}