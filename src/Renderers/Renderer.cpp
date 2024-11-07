#include "Renderer.h"

void Renderer::UpdatePlayerPos(glm::vec2 playerPos)
{
    this->PlayerPos = playerPos;
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
    std::map<Texture2D*, std::vector<glm::mat4>> RenderBatches;

    for (const auto& obj : gameObjects) {

        glm::mat4 model = glm::mat4(1.0f);
        if (obj->Rotation != 0.0f)
            printf("Rotation : %f\n", obj->Rotation);
        model = glm::translate(model, glm::vec3(obj->Position - this->PlayerPos, 0.0f));
        //Rotation
        model = glm::translate(model, glm::vec3(0.5f * obj->Size.x, 0.5f * obj->Size.y, 0.0f)); //We transform model to be centered at center of object
        model = glm::rotate(model, obj->Rotation, glm::vec3(0.0f, 0.0f, 1.0f)); //Rotate
        model = glm::translate(model, glm::vec3(-0.5f * obj->Size.x, -0.5f * obj->Size.y, 0.0f)); //transform model so it's center is back at top left corner
        model = glm::scale(model, glm::vec3(obj->Size, 1.0f));

        RenderBatches[obj->Sprite].push_back(model); //Wyszukujemy vector macierzy po kluczu i przypisujemy do niego now¹ macierz (NIE DZIA£A)

    }
    for (auto& batch : RenderBatches)
    {
        Texture2D* texture = batch.first;
        const auto& modelMatrices = batch.second;
        ResourceManager::GetShader("instancedSprite").Use();
        glActiveTexture(GL_TEXTURE0);
        ResourceManager::GetShader("instancedSprite").SetInteger("image",0);
        texture->Bind();
        this->UpdateInstanceData(modelMatrices);
        this->DrawInstances(modelMatrices.size());

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
