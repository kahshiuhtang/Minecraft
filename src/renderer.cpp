#include "renderer/renderer.hpp"
#include "world/world.hpp"

MCRFT::Renderer::Renderer()
{
    m_screen = new Screen();
    m_screen->setupwindow();
    m_camera = new Camera(m_screen->m_window);
    setupshaders();
    m_shader->use();
    m_world = new World();
    m_world->init();
    m_world->generate_all_chunk_meshes();
    m_texture_manager = new TextureManager();
    setuptextures();
    m_gui = new Gui(m_screen->m_window);
    m_shader->setint("texture1", 0);
    m_shader->setint("texture2", 1);
}
int MCRFT::Renderer::setupshaders()
{
    m_shader = new Shader("../rsrc/7.3.camera.vs", "../rsrc/7.3.camera.fs");
    // m_shader = new Shader("../rsrc/model_loading.vs", "../rsrc/model_loading.fs");
    return 0;
}
int MCRFT::Renderer::setuptextures()
{
    m_texture_manager->addtexture("../rsrc/64x64_sheet.png");
    m_texture_manager->addtexture("../rsrc/crosshair.png");
    return 0;
}
int MCRFT::Renderer::rendercrosshair()
{
    int error = 0;
    try
    {
    }
    catch (const std::exception &e)
    {
        std::cout << "Renderer rendercrosshair(): Exception: " << e.what() << std::endl;
    }
    return error;
}
int MCRFT::Renderer::rendermapmeshes()
{
    int error = 0;
    try
    {
        for (unsigned int i = 0; i < 16; i++)
        {
            for (unsigned int j = 0; j < 16; j++)
            {

                glBindVertexArray(p_VAO);
                std::shared_ptr<MCRFT::Chunk> chunk = m_world->get_chunk(i, j);
                if (chunk == nullptr)
                {
                    continue;
                }
                std::vector<float> *vertices = &(chunk->m_mesh_vertices);
                glm::mat4 model = glm::mat4(1.0f);
                m_shader->setmat4("model", model);
                glBindBuffer(GL_ARRAY_BUFFER, p_VBO);
                glBufferData(GL_ARRAY_BUFFER, vertices->size() * sizeof(float), vertices->data(), GL_STATIC_DRAW);

                // position attribute
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
                glEnableVertexAttribArray(0);
                // texture coord attribute
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
                glEnableVertexAttribArray(1);
                glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(5 * sizeof(float)));
                glEnableVertexAttribArray(2);
                glDrawArrays(GL_TRIANGLES, 0, vertices->size() / 8);
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cout << "Renderer rendermapmeshes(): Exception: " << e.what() << std::endl;
    }
    return error;
}

int MCRFT::Renderer::loop()
{
    // render loop
    // -----------
    double prevTime = 0.0;
    double crntTime = 0.0;
    double timeDiff;
    unsigned int counter = 0;
    glGenVertexArrays(1, &p_VAO);
    glGenBuffers(1, &p_VBO);
    const char *fps_string = nullptr;
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
            fps_string = new_title.c_str();
            glfwSetWindowTitle(m_screen->m_window, new_title.c_str());
            prevTime = crntTime;
            counter = 0;
        }
        m_camera->updateframe();
        glm::vec3 copyOfOldCameraPos = m_camera->m_camera_pos;
        glm::vec3 copyOfOldPlayerPos = m_camera->m_curr_player->m_current_pos;
        // input
        // -----
        m_camera->processinput(m_screen->m_window);
        int curr_x = floor(m_camera->m_curr_player->m_current_pos.x);
        int curr_y = floor(m_camera->m_curr_player->m_current_pos.y);
        int curr_z = floor(m_camera->m_curr_player->m_current_pos.z);
        if (m_world->isInsideBlock(curr_x, curr_y, curr_z))
        {
            m_camera->m_camera_pos = copyOfOldCameraPos;
            m_camera->m_curr_player->m_current_pos = copyOfOldPlayerPos;
            continue; // Todo, probably just want flag here
        }

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_gui->startnewframe();

        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        if (!m_texture_manager->gettexture(1))
        {
            continue;
        }
        glBindTexture(GL_TEXTURE_2D, m_texture_manager->gettexture(1)->m_texture_id);

        // activate shader
        m_shader->use();
        m_camera->updateprojmatrix(m_shader);
        if (glfwGetMouseButton(m_screen->m_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        {
            this->m_world->cast_ray(m_camera, m_camera->m_curr_player->m_current_pos);
        }

        // render boxes
        this->rendermapmeshes();
        if (fps_string != nullptr)
        {
            m_gui->renderframe(fps_string);
        }
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(m_screen->m_window);
        glfwPollEvents();
    }
    return 0;
}
int MCRFT::Renderer::destroyrenderer()
{
    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &p_VAO);
    glDeleteBuffers(1, &p_VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}