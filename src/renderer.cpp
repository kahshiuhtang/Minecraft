#include "renderer.hpp"
#include "world.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <external/stb_image.h>

int MCRFT::Renderer::init()
{
    m_screen = new Screen();
    m_screen->init_window();
    m_camera = new Camera();
    m_camera->init_camera(m_screen->m_window);
    setup_shaders();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    float vertices[] = {
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f};

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    init_textures();
    m_shader->use();
    m_shader->setInt("texture1", 0);
    m_shader->setInt("texture2", 1);
    return 0;
}
int MCRFT::Renderer::setup_shaders()
{
    m_shader = new Shader("../rsrc/7.3.camera.vs", "../rsrc/7.3.camera.fs");
    // m_shader = new Shader("../rsrc/model_loading.vs", "../rsrc/model_loading.fs");
    return 0;
}
int MCRFT::Renderer::init_textures()
{
    // texture 1
    // ---------
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char *data = stbi_load(std::filesystem::path("../rsrc/container.jpg").c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    // texture 2
    // ---------
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    data = stbi_load(std::filesystem::path("../rsrc/awesomeface.png").c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        // note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    return 0;
}
int MCRFT::Renderer::loop()
{
    // render loop
    // -----------
    const siv::PerlinNoise::seed_type seed = 123456u;

    const siv::PerlinNoise perlin{seed};
    double prevTime = 0.0;
    double crntTime = 0.0;
    double timeDiff;
    unsigned int counter = 0;
    World *world = new World();
    world->init();
    while (!glfwWindowShouldClose(m_screen->m_window))
    {
        crntTime = glfwGetTime();
        timeDiff = crntTime - prevTime;
        counter++;
        // If a second has passed.
        if (timeDiff >= 1.0 / 30.0)
        {
            std::string FPS = std::to_string((1.0 / timeDiff) * counter);
            std::string new_title = FPS + " FPS";
            glfwSetWindowTitle(m_screen->m_window, new_title.c_str());
            prevTime = crntTime;
            counter = 0;
        }
        m_camera->update_frame();

        // input
        // -----
        m_camera->process_input(m_screen->m_window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        // activate shader
        m_shader->use();
        m_camera->update_shaders_projection_mat(m_shader);

        // render boxes
        glBindVertexArray(VAO);
        for (unsigned int i = 0; i <= 100; i++)
        {
            for (unsigned int j = 0; j <= 100; j++)
            {
                for (unsigned int k = 0; k <= 100; k++)
                {
                    if (world->is_block_occupied(i, j, k) == false)
                    {
                        continue;
                    }
                    glm::vec3 position = glm::vec3(i * 1.0f, j * 1.0f, k * 1.0f);
                    glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
                    model = glm::translate(model, position);
                    m_shader->setMat4("model", model);
                    if (!world->is_block_occupied(i + 1, j, k))
                    {
                        glDrawArrays(GL_TRIANGLES, 18, 6);
                    }
                    if (!world->is_block_occupied(i - 1, j, k))
                    {
                        glDrawArrays(GL_TRIANGLES, 12, 6);
                    }
                    if (!world->is_block_occupied(i, j + 1, k))
                    {
                        glDrawArrays(GL_TRIANGLES, 30, 6);
                    }
                    if (!world->is_block_occupied(i, j - 1, k))
                    {
                        glDrawArrays(GL_TRIANGLES, 24, 6);
                    }
                    if (!world->is_block_occupied(i, j, k + 1))
                    {
                        glDrawArrays(GL_TRIANGLES, 6, 6);
                    }
                    if (!world->is_block_occupied(i, j, k - 1))
                    {
                        glDrawArrays(GL_TRIANGLES, 0, 6);
                    }
                }
                // double noise = perlin.octave2D_01((i * 0.01), (j * 0.01), 4);
                // noise *= 100;
                // const int i_noise = static_cast<int>(noise);
                // for (unsigned int k = 0; k < i_noise; k++)
                // {
                //     glm::vec3 position = glm::vec3(i * 1.0f, k * 1.0f, j * 1.0f);
                //     glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
                //     model = glm::translate(model, position);
                //     m_shader->setMat4("model", model);
                //     glDrawArrays(GL_TRIANGLES, 6, 6);
                //     glDrawArrays(GL_TRIANGLES, 12, 6);
                //     glDrawArrays(GL_TRIANGLES, 18, 6);
                //     glDrawArrays(GL_TRIANGLES, 24, 6);
                // }
            }
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(m_screen->m_window);
        glfwPollEvents();
    }
    return 0;
}
int MCRFT::Renderer::destroy_renderer()
{
    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}