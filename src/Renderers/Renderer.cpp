#include "Renderer.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <vector>

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
	this->InnitScreenGBuffer();
	this->PrepareLightmap();
	this->InnitUIvao();
	this->InnitLineVao();
}

void Renderer::Innit()
{
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
	for (int i = 0; i < 4; i++) 
	{
		glEnableVertexAttribArray(2 + i);
		glVertexAttribPointer(2 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4) * i));
		glVertexAttribDivisor(2 + i, 1);  
	}
	glBindVertexArray(0);
}

void Renderer::Render(const std::vector<GameObject*>& gameObjects)
{
    //glBindFramebuffer(GL_FRAMEBUFFER, this->gBuffer);

	//Maybe use Structs for future refactor
	std::map<RenderLayer, std::map<Shader*, std::map<Texture2D*, std::vector<glm::mat4>>>> RenderBatches;
	for (const auto& obj : gameObjects) {

		glm::mat4 model = glm::mat4(1.0f);
		//Position
		model = glm::translate(model, glm::vec3(obj->GetPosition(), 0.0f));

		//Rotation
		model = glm::translate(model, glm::vec3(0.5f * obj->GetSize().x, 0.5f * obj->GetSize().y, 0.0f)); //We transform model to be centered at center of object
		model = glm::rotate(model, obj->GetRotation(), glm::vec3(0.0f, 0.0f, 1.0f)); //Rotate
		model = glm::translate(model, glm::vec3(-0.5f * obj->GetSize().x, -0.5f * obj->GetSize().y, 0.0f)); //transform model so it's center is back at top left corner
		//Scale
		model = glm::scale(model, glm::vec3(obj->GetSize(), 1.0f));

		RenderBatches[obj->GetRenderLayer()][obj->GetShader()][obj->GetSprite()].push_back(model); //We search vector of matrices by all our parameters and add new matrice to it

	}

	//We want to render back to front based on below layer order
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
				//Update instance data
				glBindBuffer(GL_ARRAY_BUFFER, this->InstanceVBO);
				glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4), modelMatrices.data(), GL_DYNAMIC_DRAW);
				this->DrawInstances(modelMatrices.size());
			}
		}
	}
	RenderBatches.clear();
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
	Shader* playershader = player->GetShader();
	playershader->Use();

	glm::mat4 model = glm::mat4(1.0f);
	glm::vec2 PlayerSize = player->GetSize();
	//Position
	model = glm::translate(model, glm::vec3(player->GetPosition(), 0.0f));
	//Rotation
	model = glm::translate(model, glm::vec3(0.5f * PlayerSize.x, 0.5f * PlayerSize.y, 0.0f));
	model = glm::rotate(model, player->GetRotation(), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::translate(model, glm::vec3(-0.5f * PlayerSize.x, -0.5f * PlayerSize.y, 0.0f));
	//Scale
	model = glm::scale(model, glm::vec3(PlayerSize, 1.0f));

	glm::mat4 invmodel = glm::mat4(1.0f);
	invmodel = glm::inverse(model);
	
	playershader->SetMatrix4("model", model);
	playershader->SetMatrix4("InverseModel", invmodel);
	playershader->SetVector3f("spriteColor", player->GetColor());
	glActiveTexture(GL_TEXTURE4);
	player->GetNormalMap()->Bind();
	playershader->SetInteger("NormalMap", 4);
	glActiveTexture(GL_TEXTURE3);
	playershader->SetInteger("image", 3);
	player->GetSprite()->Bind();

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
	//As it's persumably first object to be drawn bind g buffer and clear everything
	glBindFramebuffer(GL_FRAMEBUFFER, this->gBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Shader* backgroundShader = background->GetShader();
	backgroundShader->Use();

	glm::vec2 BackgroundSize = background->GetSize();

	glm::mat4 model = glm::mat4(1.0f);
	//Position
	model = glm::translate(model, glm::vec3(background->GetPosition(), 0.0f));
	//Rotation
	model = glm::translate(model, glm::vec3(0.5f * BackgroundSize.x, 0.5f * BackgroundSize.y, 0.0f));
	model = glm::rotate(model, background->GetRotation(), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::translate(model, glm::vec3(-0.5f * BackgroundSize.x, -0.5f * BackgroundSize.y, 0.0f));
	//Scale
	model = glm::scale(model, glm::vec3(BackgroundSize, 1.0f));

	backgroundShader->SetMatrix4("model", model);
	backgroundShader->SetVector3f("spriteColor", background->GetColor());
	glActiveTexture(GL_TEXTURE3);
	backgroundShader->SetInteger("image", 3);
	background->GetSprite()->Bind();

	glBindVertexArray(this->BackgroundVAO);
	Common::debuginfo.DrawCalls++;
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void Renderer::InnitScreenGBuffer()
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

	// Normal buffer (vec3)
	glGenTextures(1, &this->gNormal);
	glBindTexture(GL_TEXTURE_2D, this->gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, this->gNormal, 0);

	// Color buffer(vec3)
	glGenTextures(1, &this->gAlbedo);
	glBindTexture(GL_TEXTURE_2D, this->gAlbedo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
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

}

void Renderer::RenderLight()
{
	//this->RenderLightmap(); Not implemented right yet
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
	shader.SetInteger("NormalsTest", 4);
	shader.SetVector2f("ScreenSize", Common::ScreenSize);
	shader.SetInteger("pixelSize", this->pixelSize);
	shader.SetInteger("LightPixelize", this->LightPixelize);
	shader.SetInteger("RenderMode", this->RenderMode);
	shader.SetVector2f("PlayerPosition", this->PlayerPos);
	glBindVertexArray(ScreenQuadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void UpdateDownscaledTexture(GLuint& texture, int factor)
{
	if(texture != -1)
		glDeleteTextures(1, &texture);
	int downscaledWidth = Common::ScreenSize.x / (1+factor);
	int downscaledHeight = Common::ScreenSize.y / (1+factor);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, downscaledWidth, downscaledHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

}

void Renderer::RenderLightmap()
{
	ComputeShader& shader = ResourceManager::GetComputeShader("Lightmap");
	shader.Use();
	glBindImageTexture(0, this->Lightmap, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	shader.SetUniform("ScreenSize", Common::ScreenSize);
	shader.SetUniform("PlayerPosition", this->PlayerPos);
	glDispatchCompute(Common::ScreenSize.x / 16 + 1, Common::ScreenSize.y / 16 + 1, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
	/*
	int factor = this->LightPixelize;
	int lastFactor = this->lastPixelSize;
	if (factor != lastFactor)
	{
		std::cout << "Changing the resolution of lightmap" << std::endl;
		this->lastPixelSize = factor;
		UpdateDownscaledTexture(this->DownscaledLightmap, factor);
	}

	DownscaleTexture(this->Lightmap, this->DownscaledLightmap, factor);
	*/

}

//Implementation of downscaling texture using compute shaders, better to use buffers
void Renderer::DownscaleTexture(GLuint inputTex, GLuint outputTex, int factor)
{ 
	int downscaledWidth = Common::ScreenSize.x / factor;
	int downscaledHeight = Common::ScreenSize.y / factor;

	// Workgroup size (matches local_size_x and local_size_y in the shader)
	int localSizeX = 16;
	int localSizeY = 16;

	// Calculate the number of workgroups
	int workgroupsX = (downscaledWidth + localSizeX - 1) / localSizeX;
	int workgroupsY = (downscaledHeight + localSizeY - 1) / localSizeY;
	ComputeShader& shader = ResourceManager::GetComputeShader("Downscaling");
	shader.Use();
	glBindImageTexture(0, inputTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	glBindImageTexture(1, outputTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	shader.SetUniform("downscaleFactor", factor);
	glDispatchCompute(workgroupsX, workgroupsY, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

}

void Renderer::InnitLineVao()
{
	glGenVertexArrays(1, &LineVAO);
	glGenBuffers(1, &LineVBO);

	glBindVertexArray(LineVAO);

	glBindBuffer(GL_ARRAY_BUFFER, LineVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 2, nullptr, GL_DYNAMIC_DRAW);
	
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

//Doesn't work
void Renderer::RenderLine(glm::vec2 p1, glm::vec2 p2, glm::vec4 color = glm::vec4(1.0f))
{
	auto& shader = ResourceManager::GetShader("Line");
	shader.Use();
	std::vector<glm::vec2> vertices = { p1, p2 };
	glBindBuffer(GL_ARRAY_BUFFER, this->LineVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec2) * vertices.size(), vertices.data());

	shader.SetUniform("color", color);

	glBindVertexArray(this->LineVAO);
	glDrawArrays(GL_LINES, 0, 2);

	glBindVertexArray(0);
	glUseProgram(0);
	
}

void Renderer::InnitUIvao()
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


	glGenVertexArrays(1, &UIspriteVAO);
	glGenBuffers(1, &UIspriteVBO);

	glBindVertexArray(UIspriteVAO);

	glBindBuffer(GL_ARRAY_BUFFER, UIspriteVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
}

//Renders given sprite based of given position purpose of this function is to render UI sprite elements
void Renderer::RenderSprite(const Texture2D& sprite, glm::vec2 position, float rotation, glm::vec2 scale, glm::vec3 color)
{
	glm::mat4 model = glm::mat4(1.0f);
	//Position
	model = glm::translate(model, glm::vec3(position, 0.0f));
	color = glm::vec3(1.0f);
	//Rotation
	model = glm::translate(model, glm::vec3(0.5f * sprite.Width, 0.5f * sprite.Height, 0.0f));
	model = glm::rotate(model, rotation, glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::translate(model, glm::vec3(-0.5f * sprite.Width, -0.5f * sprite.Height, 0.0f));
	//Scale
	model = glm::scale(model, glm::vec3(scale, 1.0f));

	auto& shader = ResourceManager::GetShader("UI").Use();
	shader.SetUniform("model", model);
	shader.SetUniform("spriteColor", color);

	glActiveTexture(GL_TEXTURE0);
	shader.SetUniform("image", 0);
	sprite.Bind();

	glBindVertexArray(this->UIspriteVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindVertexArray(0);
	glUseProgram(0);
}

float CalculateMouseAngle(glm::vec2 mousePos)
{
	glm::vec2 screenCenter = Common::ScreenSize * 0.5f;
	glm::vec2 delta = mousePos - screenCenter;

	float angle = atan2(delta.y, delta.x);

	if (angle < 0.0f)
	{
		angle += 2.0f * glm::pi<float>(); //Loop angle to be positive
	}

	return angle;
}
//Calculate angle of next slot until we get slot index if somehow we can't find slot returns -1
int DetermineHoveredSlot(float mouseAngle, int edges)
{
	float angleStep = 2.0f * glm::pi<float>() / edges;

	for (int i = 0; i < edges; ++i)
	{
		float startAngle = i * angleStep;
		float endAngle = (i + 1) * angleStep;

		if (mouseAngle >= startAngle && mouseAngle < endAngle)
		{
			return i;
		}
	}
	return -1;
}

void Renderer::RenderCookingMenu(Inventory* inv, CookingMenu& cookingMenu) 
{
	//We can have two different menu info struct so i just use pointer to that menu info
	CookingMenuInfo* info;
	auto& shader = ResourceManager::GetShader("CookingMenu");
	shader.Use();
	int slotAmount = (inv->inventorySize() >= 5 ? inv->inventorySize() : 5); //minimum 5 slots
	const float centerX = Common::ScreenSize.x / 2;
	const float centerY = Common::ScreenSize.y / 2;
	const float angleStep = 360.0f / slotAmount;


	if (inv->inventorySize() <= 5)
	{
		info = &cookingMenu.GetSmallestCookingMenu();
	}
	else
	{
		info = &cookingMenu.GetCookingMenuInfo();
	}

	//Currently hovered slot
	int HoveredSlot = DetermineHoveredSlot(CalculateMouseAngle(*this->MousePos), slotAmount);
	//We are looping using objects but need to keep track of current index
	int slotIndex = 0;

	for (const auto & [ingredient,quantity] : inv->stock)
	{

		if (quantity <= 0) continue;
		
		//Update VBO with the current slot's quad vertices
		glBindBuffer(GL_ARRAY_BUFFER, info->VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, info->quads[slotIndex].size() * sizeof(glm::vec2), info->quads[slotIndex].data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//Set per slot uniforms
		glm::vec3 color = glm::vec3((float)slotIndex / info->quads.size(), 0.5f, 1.0f);
		glm::mat4 model = glm::mat4(1.0f);
		glm::vec2 center = info->slotCenters[slotIndex];
		if (slotIndex == HoveredSlot)
		{
			//Assign Hovered slot 
			cookingMenu.HoveredSlot = Slot(slotIndex, ingredient->id);
			//Make hovered slot point out
			color = glm::vec3(1.0f);
		}
		//Visuals for selected slots, maybe use another shader in future for effects
		for (const auto& ss : cookingMenu.selectedSlots)
		{
			if (slotIndex == ss.slotID)
			{
				color = glm::vec3(0.75f);
			}
		}

		shader.Use();
		shader.SetUniform("model", model);
		shader.SetUniform("center", glm::vec2(centerX, centerY));
		shader.SetUniform("uColor", glm::vec4(color.x, color.y, color.z, 0.6f));

		// Draw the quad
		glBindVertexArray(info->VAO);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		
		//Render ingredient sprite at slot center
		RenderSprite(*ingredient->sprite, center + glm::vec2(centerX, centerY), 0.0f, glm::vec2(32.0f));

		//TODO Render outline of the slot

		slotIndex++;
	}
	//If we haven't drawn 5 slots draw empty ones it's pretty much same code as above but it just doesn't include slot variable stuff
	if (slotIndex < 5)
	{
		while (slotIndex < 5)
		{
			glBindBuffer(GL_ARRAY_BUFFER, info->VBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, info->quads[slotIndex].size() * sizeof(glm::vec2), info->quads[slotIndex].data());
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glm::vec3 color = glm::vec3((float)slotIndex / info->quads.size(), 0.5f, 1.0f);
			if (slotIndex == HoveredSlot)
			{
				cookingMenu.HoveredSlot = Slot(-1, -1);
				color = glm::vec3(1.0f);
			}
			shader.Use();
			shader.SetUniform("center", glm::vec2(centerX, centerY));
			shader.SetUniform("uColor", glm::vec4(color.x, color.y, color.z, 0.6f));

			glBindVertexArray(info->VAO);
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
			slotIndex++;
		}
	}
	glBindVertexArray(0);
	glUseProgram(0);
}

//Function that renders text on screen
//somehow need to render it in another pass
//maybe with maintaining a vector of text to render in textPass
void Renderer::RenderText(std::string text,glm::vec3 color, glm::vec2 position,float textScale)
{
	unsigned int VBO;
	unsigned int quadVAO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	Shader shader = ResourceManager::GetShader("text");
	shader.Use();
	shader.SetVector3f("textColor", color);
	shader.SetUniform("text", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(quadVAO);

	std::string::const_iterator c;

	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Common::Characters[*c];
		
		float xpos = position.x + ch.Bearing.x * textScale;
		float ypos = position.y + (Common::Characters['H'].Bearing.y - ch.Bearing.y) * textScale; \

		float w = ch.Size.x * textScale;
		float h = ch.Size.y * textScale;

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
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices),vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		position.x += (ch.Advance >> 6) * textScale;

	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}