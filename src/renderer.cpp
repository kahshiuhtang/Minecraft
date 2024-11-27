#include "renderer/renderer.hpp"
#include "world/world.hpp"

MCRFT::Renderer::Renderer()
{
    m_screen = new Screen();
    m_screen->setupwindow();
    m_camera = new Camera(m_screen->m_window);
    setupshaders();
    m_curr_shader->use();
    m_world = new World();
    m_world->createnew();
    m_world->generateallchunkmeshes();
    m_texture_manager = new TextureManager();
    setuptextures();
    m_gui = new Gui(m_screen->m_window);
    m_curr_shader->setint("texture1", 0);
    m_curr_shader->setint("texture2", 1);
}
int MCRFT::Renderer::setupshaders()
{
    norm_shader = m_shader_mang.addshader("../rsrc/block.vs", "../rsrc/block.fs");
    m_curr_shader = m_shader_mang.getshader(norm_shader);
    crosshair_shader = m_shader_mang.addshader("../rsrc/line.vs", "../rsrc/line.fs");
    // m_curr_shader = new Shader("../rsrc/model_loading.vs", "../rsrc/model_loading.fs");
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
        glDisable(GL_DEPTH_TEST);
        glUseProgram(crosshair_shader);

        float halfcrosshairwidth = 30.0f / 2.0f;
        float xsize = halfcrosshairwidth / m_screen->getscreenwidth() * 2.0f; // Convert to NDC
        float ysize = halfcrosshairwidth / m_screen->getscreenheight() * 2.0f; // Convert to NDC
        float crosshair[] = {
            -xsize, 0.0f,  // Horizontal line
             xsize, 0.0f,
             0.0f, -ysize, // Vertical line
             0.0f,  ysize
        };

        GLuint vao_2d, vbo_2d;
        glGenVertexArrays(1, &vao_2d);
        glGenBuffers(1, &vbo_2d);

        glBindVertexArray(vao_2d);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_2d);
        glBufferData(GL_ARRAY_BUFFER, sizeof(crosshair), crosshair, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glBindVertexArray(0);

        glBindVertexArray(vao_2d);
        glDrawArrays(GL_LINES, 0, 4);
        glBindVertexArray(0);

        glDeleteVertexArrays(1, &vao_2d);
        glDeleteBuffers(1, &vbo_2d);

        glEnable(GL_DEPTH_TEST);
    }
    catch (const std::exception &e)
    {
        std::cout << "Renderer rendercrosshair(): Exception: " << e.what() << std::endl;
        error = -1;
    }
    return error;
}

int MCRFT::Renderer::rendermapmeshes()
{
    int error = 0;
    try
    {
        for (unsigned int i = 0; i < CHUNK_LENGTH; i++)
        {
            for (unsigned int j = 0; j < CHUNK_WIDTH; j++)
            {

                glBindVertexArray(p_VAO);
                std::shared_ptr<MCRFT::Chunk> chunk = m_world->getchunk(i, j);
                if (chunk == nullptr)
                {
                    continue;
                }
                std::vector<float> *vertices = &(chunk->m_mesh_vertices);
                glm::mat4 model = glm::mat4(1.0f);
                m_curr_shader->setmat4("model", model);
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
        if (m_world->isinsideblock(curr_x, curr_y, curr_z))
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
        m_curr_shader->use();
        m_camera->updateprojmatrix(m_curr_shader);
        if (glfwGetMouseButton(m_screen->m_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        {
            this->m_world->findhitblock(m_camera, m_camera->m_curr_player->m_current_pos);
        }

        // render boxes
        this->rendermapmeshes();
        if (fps_string != nullptr)
        {
            m_gui->renderframe(fps_string);
        }
        glEnable(GL_DEPTH_TEST);
        glUseProgram(crosshair_shader);
        this->rendercrosshair();
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