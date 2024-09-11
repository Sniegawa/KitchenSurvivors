#include "TextRenderer.h"

TextRenderer::TextRenderer(Shader& _shader)
{
	this->shader = _shader;
}

void TextRenderer::RenderText(std::string text, float x, float y, float scale, glm::vec3 color)
{
	unsigned int VBO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	this->shader.Use();
	this->shader.SetVector3f("textColor", color);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(quadVAO);

	

	std::string::const_iterator c;
	//Iterating through string
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Common::Characters[*c];

		float xpos = x + ch.Bearing.x * scale;
		float ypos = y + (Common::Characters['H'].Bearing.y - ch.Bearing.y) * scale;

		float w = ch.Size.x * scale;
		float h = ch.Size.y * scale;

		float vertices[6][4] =
		{
			{ xpos,     ypos + h,   0.0f, 1.0f },
			{ xpos + w, ypos,       1.0f, 0.0f },
			{ xpos,     ypos,       0.0f, 0.0f },

			{ xpos,     ypos + h,   0.0f, 1.0f },
			{ xpos + w, ypos + h,   1.0f, 1.0f },
			{ xpos + w, ypos,       1.0f, 0.0f }
		};
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		x += (ch.Advance >> 6) * scale;
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

}