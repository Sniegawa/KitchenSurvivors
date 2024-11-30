#include "Renderer.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

void Renderer::UpdatePlayerPos(glm::vec2 playerPos)
{
    this->PlayerPos = playerPos;
}

Renderer::Renderer() {}

void Renderer::RendererSetup()
{
    this->Innit();
    this->InnitPlayerData();
    this->InnitBackgroundData();
    this->InnitScreenQuad();
    this->InnitScreenBuffers();
    this->PrepareLightmap();
}

void Renderer::Innit()
{
    float vertices[] = {
        // pos      // tex
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };

    glBindVertexArray(0);
    glGenVertexArrays(1, &this->VAO);
    glBindVertexArray(this->VAO);

    glGenBuffers(1, &this->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &this->InstanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, this->InstanceVBO);
    for (int i = 0; i < 4; i++) {
        glEnableVertexAttribArray(2 + i);
        glVertexAttribPointer(2 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4) * i));
        glVertexAttribDivisor(2 + i, 1);  // Tell OpenGL this is per-instance data
    }
    glBindVertexArray(0);
}

void Renderer::Render(const std::vector<GameObject*>& gameObjects)
{
   // glBindFramebuffer(GL_FRAMEBUFFER, this->gBuffer);
    std::map<RenderLayer,std::map<Shader* ,std::map<Texture2D*, std::vector<glm::mat4>>>> RenderBatches;
    for (const auto& obj : gameObjects) {

        glm::mat4 model = glm::mat4(1.0f);

        model = glm::translate(model, glm::vec3(obj->Position - this->PlayerPos, 0.0f));

        //Rotation
        model = glm::translate(model, glm::vec3(0.5f * obj->Size.x, 0.5f * obj->Size.y, 0.0f)); //We transform model to be centered at center of object
        model = glm::rotate(model, obj->Rotation, glm::vec3(0.0f, 0.0f, 1.0f)); //Rotate
        model = glm::translate(model, glm::vec3(-0.5f * obj->Size.x, -0.5f * obj->Size.y, 0.0f)); //transform model so it's center is back at top left corner
        model = glm::scale(model, glm::vec3(obj->Size, 1.0f));

        RenderBatches[obj->GetRenderLayer()][obj->shader][obj->Sprite].push_back(model); //Wyszukujemy vector macierzy po kluczu i przypisujemy do niego now� macierz (NIE DZIA�A)
        
    }

    for (auto& layer : { RenderLayer::BACKGROUND, RenderLayer::PICKUPS, RenderLayer::PROJECTILES, RenderLayer::ENEMY, RenderLayer::PLAYER })
    {
        for (auto& Shaderbatch : RenderBatches[layer])
        {
            Shader* shader = Shaderbatch.first;
            shader->Use();
            shader->SetUniform("image", 3);
            for (auto& renderBatch : Shaderbatch.second)
            {
                Texture2D* texture = renderBatch.first;
                const auto& modelMatrices = renderBatch.second;
                glActiveTexture(GL_TEXTURE3);
                texture->Bind();
                this->UpdateInstanceData(modelMatrices);
                this->DrawInstances(modelMatrices.size());
            }
        }
    }
}

void Renderer::UpdateInstanceData(const std::vector<glm::mat4>& modelMatrices)
{
    glBindBuffer(GL_ARRAY_BUFFER, this->InstanceVBO);
    glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4), modelMatrices.data(), GL_DYNAMIC_DRAW);
}

void Renderer::DrawInstances(GLsizei instanceCount)
{
    glBindVertexArray(this->VAO);
    Common::debuginfo.DrawCalls++;
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, instanceCount);

    glBindVertexArray(0);
}

void Renderer::InnitPlayerData()
{
    float vertices[] = {
        // pos      // tex
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };
    glGenVertexArrays(1, &this->PlayerVAO);
    glGenBuffers(1, &this->PlayerVBO);

    glBindBuffer(GL_ARRAY_BUFFER, this->PlayerVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(this->PlayerVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Renderer::RenderPlayer(Player* player)
{
    Shader* playershader = player->shader;
    playershader->Use();

    glm::mat4 model = glm::mat4(1.0f);
    //Position
    model = glm::translate(model, glm::vec3(player->Position, 0.0f));
    //Rotation
    model = glm::translate(model, glm::vec3(0.5f * player->Size.x, 0.5f * player->Size.y, 0.0f)); //We transform model to be centered at center of object
    model = glm::rotate(model, player->Rotation, glm::vec3(0.0f, 0.0f, 1.0f)); //Rotate
    model = glm::translate(model, glm::vec3(-0.5f * player->Size.x, -0.5f * player->Size.y, 0.0f)); //transform model so it's center is back at top left corner

    model = glm::scale(model, glm::vec3(player->Size, 1.0f));

    glm::mat4 invmodel = glm::mat4(1.0f);

    invmodel = glm::inverse(model);

    playershader->SetMatrix4("model", model);
    playershader->SetMatrix4("InverseModel", invmodel);
    playershader->SetVector3f("spriteColor", player->Color);
    glActiveTexture(GL_TEXTURE3);
    playershader->SetInteger("image", 3);
    player->Sprite->Bind();

    glBindVertexArray(this->PlayerVAO);
    Common::debuginfo.DrawCalls++;
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void Renderer::InnitBackgroundData()
{
    float vertices[] = {
        // pos      // tex
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };
    glGenVertexArrays(1, &this->BackgroundVAO);
    glGenBuffers(1, &this->BackgroundVBO);

    glBindBuffer(GL_ARRAY_BUFFER, this->BackgroundVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(this->BackgroundVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Renderer::RenderBackground(GameObject* background)
{
    glBindFramebuffer(GL_FRAMEBUFFER, this->gBuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    Shader* backgroundShader = background->shader;
    backgroundShader->Use();

    glm::mat4 model = glm::mat4(1.0f);
    //Position
    model = glm::translate(model, glm::vec3(background->Position - this->PlayerPos, 0.0f));
    //Rotation
    model = glm::translate(model, glm::vec3(0.5f * background->Size.x, 0.5f * background->Size.y, 0.0f)); //We transform model to be centered at center of object
    model = glm::rotate(model, background->Rotation, glm::vec3(0.0f, 0.0f, 1.0f)); //Rotate
    model = glm::translate(model, glm::vec3(-0.5f * background->Size.x, -0.5f * background->Size.y, 0.0f)); //transform model so it's center is back at top left corner

    model = glm::scale(model, glm::vec3(background->Size, 1.0f));

    backgroundShader->SetMatrix4("model", model);
    backgroundShader->SetVector3f("spriteColor", background->Color);
    glActiveTexture(GL_TEXTURE3);
    backgroundShader->SetInteger("image", 3);
    background->Sprite->Bind();

    glBindVertexArray(this->BackgroundVAO);
    Common::debuginfo.DrawCalls++;
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void Renderer::InnitScreenBuffers()
{
    int SCR_WIDTH = Common::ScreenSize.x;
    int SCR_HEIGHT = Common::ScreenSize.y;

    
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

    // Position buffer (vec2)
    glGenTextures(1, &this->gPosition);
    glBindTexture(GL_TEXTURE_2D, this->gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RG, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->gPosition, 0);

    // Normal buffer (vec2)
    glGenTextures(1, &this->gNormal);
    glBindTexture(GL_TEXTURE_2D, this->gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RG, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, this->gNormal, 0);

    // Color + specular buffer (vec4)
    glGenTextures(1, &this->gAlbedo);
    glBindTexture(GL_TEXTURE_2D, this->gAlbedo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, this->gAlbedo, 0);
	GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);
    // Attach depth buffer
    glGenRenderbuffers(1, &this->rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->rboDepth);

    // Check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::InnitScreenQuad()
{
    float quadVertices[] = {
        // positions   // texture coords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
        1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
        1.0f, -1.0f,  1.0f, 0.0f,
        1.0f,  1.0f,  1.0f, 1.0f
    };

   
    glGenVertexArrays(1, &ScreenQuadVAO);
    glGenBuffers(1, &ScreenQuadVBO);

    glBindVertexArray(ScreenQuadVAO);

    glBindBuffer(GL_ARRAY_BUFFER, ScreenQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void Renderer::PrepareLightmap()
{
    glCreateTextures(GL_TEXTURE_2D, 1, &this->Lightmap);
    glTextureParameteri(this->Lightmap, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(this->Lightmap, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(this->Lightmap, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(this->Lightmap, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureStorage2D(this->Lightmap, 1, GL_RGBA32F, Common::ScreenSize.x, Common::ScreenSize.y);
    glBindImageTexture(0, this->Lightmap, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
}

void Renderer::RenderLight()
{
    this->RenderLightmap();
    Shader& shader = ResourceManager::GetShader("light");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shader.Use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->gPosition);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, this->gNormal);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, this->gAlbedo);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, this->Lightmap);
    shader.SetInteger("gPosition", 0);
    shader.SetInteger("gNormal", 1);
    shader.SetInteger("gAlbedo", 2);
    shader.SetInteger("Lightmap", 3);
    shader.SetVector2f("ScreenSize", Common::ScreenSize);
    shader.SetInteger("pixelSize", this->pixelSize);
    shader.SetInteger("LightPixelize", this->LightPixelize);
    shader.SetInteger("RenderMode", this->RenderMode);
    shader.SetVector2f("PlayerPosition", this->PlayerPos);
    glBindVertexArray(ScreenQuadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

}

void Renderer::RenderLightmap()
{
    ComputeShader& shader = ResourceManager::GetComputeShader("Lightmap");
    shader.Use();
    shader.SetUniform("ScreenSize", Common::ScreenSize);
    shader.SetUniform("PlayerPosition", this->PlayerPos);
    glDispatchCompute(Common::ScreenSize.x/16+1, Common::ScreenSize.y/16+1, 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);

}