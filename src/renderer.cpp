#include "renderer/renderer.hpp"
#include "world/world.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <external/stb_image.h>

int MCRFT::Renderer::init()
{
    m_screen = new Screen();
    m_screen->init_window();
    m_camera = new Camera(m_screen->m_window);
    setup_shaders();
    init_textures();
    m_shader->use();
    m_world = new World();
    m_world->init();
    m_world->generate_all_chunk_meshes();
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
    unsigned char *data = stbi_load(std::filesystem::path("../rsrc/64x64_sheet.png").c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    data = stbi_load(std::filesystem::path("../rsrc/grass-top.png").c_str(), &width, &height, &nrChannels, 0);
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
    data = stbi_load(std::filesystem::path("../rsrc/crosshair.png").c_str(), &m_crosshairWidth, &m_crosshairHeight, &m_crosshairChannels, 0);
    if (data)
    {
        glGenTextures(1, &m_crosshairTextureID);
        glBindTexture(GL_TEXTURE_2D, m_crosshairTextureID);
        GLenum format = (m_crosshairChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, m_crosshairWidth, m_crosshairHeight, 0, format, GL_UNSIGNED_BYTE, data);

        stbi_image_free(data);
    }
    else
    {
        std::cerr << "Failed to load crosshair texture" << std::endl;
    }
    return 0;
}
int MCRFT::Renderer::_render_crosshair()
{
    int error = 0;
    try
    {
    }
    catch (const std::exception &e)
    {
        std::cout << "Renderer _render_crosshair(): Exception: " << e.what() << std::endl;
    }
    return error;
}
int MCRFT::Renderer::render_map_meshes()
{
    int error = 0;
    try
    {
        for (unsigned int i = 0; i < 16; i++)
        {
            for (unsigned int j = 0; j < 16; j++)
            {

                glBindVertexArray(p_VAO);
                Chunk *chunk = m_world->get_chunk(i, j);
                if (chunk == nullptr)
                {
                    continue;
                }
                std::vector<float> *vertices = &(chunk->m_mesh_vertices);
                glm::mat4 model = glm::mat4(1.0f);
                m_shader->setMat4("model", model);
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
        std::cout << "Renderer render_map_meshes(): Exception: " << e.what() << std::endl;
    }
    return error;
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
    glGenVertexArrays(1, &p_VAO);
    glGenBuffers(1, &p_VBO);
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(m_screen->m_window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
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
        m_camera->update_frame();
        glm::vec3 copyOfOldCameraPos = m_camera->m_camera_pos;
        glm::vec3 copyOfOldPlayerPos = m_camera->m_curr_player->m_current_pos;
        // input
        // -----
        m_camera->process_input(m_screen->m_window);
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
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);

        // activate shader
        m_shader->use();
        m_camera->update_shaders_projection_mat(m_shader);
        if (glfwGetMouseButton(m_screen->m_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        {
            this->m_world->cast_ray(m_camera, m_camera->m_curr_player->m_current_pos);
        }
        // render boxes
        this->render_map_meshes();
        ImGui::Begin("FPS");
        // Text that appears in the window
        if (fps_string == nullptr)
        {
            ImGui::Text("LOADING");
        }
        else
        {
            ImGui::Text(fps_string);
        }
        // Ends the window
        ImGui::End();
        // Renders the ImGUI elements
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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
    glDeleteVertexArrays(1, &p_VAO);
    glDeleteBuffers(1, &p_VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}