#include "PlayerRenderer.h"

PlayerRenderer::PlayerRenderer(const SpriteRenderer& sr) : SpriteRenderer(sr) 
{
	this->initRenderData();
};

void PlayerRenderer::DrawSprite(Texture2D texture, glm::vec2 position, glm::vec2 size, float rotate, glm::vec3 color)
{
    
    this->shader.Use();
    glm::mat4 model = glm::mat4(1.0f);
    //Position
    model = glm::translate(model, glm::vec3(position, 0.0f));
    //Rotation
    model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f)); //We transform model to be centered at center of object
    model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f)); //Rotate
    model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f)); //transform model so it's center is back at top left corner

    model = glm::scale(model, glm::vec3(size, 1.0f));

    this->shader.SetMatrix4("model", model);
    this->shader.SetVector3f("spriteColor", color);

    glActiveTexture(GL_TEXTURE0);
    texture.Bind();

    glBindVertexArray(this->quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}