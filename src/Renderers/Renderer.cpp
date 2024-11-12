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

void Renderer::Render(const std::vector<std::shared_ptr<GameObject>>& gameObjects)
{
    std::map<RenderLayer,std::map<Shader* ,std::map<Texture2D*, std::vector<glm::mat4>>>> RenderBatches;
    for (const auto& obj : gameObjects) {

        glm::mat4 model = glm::mat4(1.0f);

        

        model = glm::translate(model, glm::vec3(obj->Position - this->PlayerPos, 0.0f));

        //Rotation
        model = glm::translate(model, glm::vec3(0.5f * obj->Size.x, 0.5f * obj->Size.y, 0.0f)); //We transform model to be centered at center of object
        model = glm::rotate(model, obj->Rotation, glm::vec3(0.0f, 0.0f, 1.0f)); //Rotate
        model = glm::translate(model, glm::vec3(-0.5f * obj->Size.x, -0.5f * obj->Size.y, 0.0f)); //transform model so it's center is back at top left corner
        model = glm::scale(model, glm::vec3(obj->Size, 1.0f));

        RenderBatches[obj->GetRenderLayer()][obj->shader][obj->Sprite].push_back(model); //Wyszukujemy vector macierzy po kluczu i przypisujemy do niego now¹ macierz (NIE DZIA£A)
        
    }

    glActiveTexture(GL_TEXTURE0);

    for (auto& layer : { RenderLayer::BACKGROUND, RenderLayer::PICKUPS, RenderLayer::PROJECTILES, RenderLayer::ENEMY, RenderLayer::PLAYER })
    {
        for (auto& Shaderbatch : RenderBatches[layer])
        {
            Shader* shader = Shaderbatch.first;
            shader->Use();
            shader->SetUniform("image", 0);
            for (auto& renderBatch : Shaderbatch.second)
            {
                Texture2D* texture = renderBatch.first;
                const auto& modelMatrices = renderBatch.second;
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

    playershader->SetMatrix4("model", model);
    playershader->SetVector3f("spriteColor", player->Color);
    glActiveTexture(GL_TEXTURE0);
    playershader->SetInteger("image", 0);
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
    glActiveTexture(GL_TEXTURE0);
    backgroundShader->SetInteger("image", 0);
    background->Sprite->Bind();

    glBindVertexArray(this->BackgroundVAO);
    Common::debuginfo.DrawCalls++;
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}