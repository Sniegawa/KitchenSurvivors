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
	this->InnitScreenGBuffer();
	this->PrepareLightmap();
	this->InnitUIvao();
	this->InnitLineVao();
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
    //glBindFramebuffer(GL_FRAMEBUFFER, this->gBuffer);
	std::map<RenderLayer,std::map<Shader* ,std::map<Texture2D*, std::vector<glm::mat4>>>> RenderBatches;
	for (const auto& obj : gameObjects) {

		glm::mat4 model = glm::mat4(1.0f);

		//model = glm::translate(model, glm::vec3(obj->Position - this->PlayerPos, 0.0f));
		model = glm::translate(model, glm::vec3(obj->Position, 0.0f));

		//Rotation
		model = glm::translate(model, glm::vec3(0.5f * obj->Size.x, 0.5f * obj->Size.y, 0.0f)); //We transform model to be centered at center of object
		model = glm::rotate(model, obj->Rotation, glm::vec3(0.0f, 0.0f, 1.0f)); //Rotate
		model = glm::translate(model, glm::vec3(-0.5f * obj->Size.x, -0.5f * obj->Size.y, 0.0f)); //transform model so it's center is back at top left corner
		model = glm::scale(model, glm::vec3(obj->Size, 1.0f));

		RenderBatches[obj->GetRenderLayer()][obj->shader][obj->Sprite].push_back(model); //Wyszukujemy vector macierzy po kluczu i przypisujemy do niego nowa macierz
		
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
	glActiveTexture(GL_TEXTURE4);
	player->NormalMap->Bind();
	playershader->SetInteger("NormalMap", 4);
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
	//model = glm::translate(model, glm::vec3(background->Position - this->PlayerPos, 0.0f));
	model = glm::translate(model, glm::vec3(background->Position, 0.0f));
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

	// Normal buffer (vec2)
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

//Renders given sprite based of given position
void Renderer::RenderSprite(Texture2D& sprite, glm::vec2 position, float rotation, glm::vec2 scale, glm::vec3 color)
{
	glm::mat4 model = glm::mat4(1.0f);

	model = glm::translate(model, glm::vec3(position, 0.0f));
	color = glm::vec3(1.0f);
	//Rotation
	model = glm::translate(model, glm::vec3(0.5f * sprite.Width, 0.5f * sprite.Height, 0.0f)); //We transform model to be centered at center of object
	model = glm::rotate(model, rotation, glm::vec3(0.0f, 0.0f, 1.0f)); //Rotate
	model = glm::translate(model, glm::vec3(-0.5f * sprite.Width, -0.5f * sprite.Height, 0.0f)); //transform model so it's center is back at top left corner
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

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;
const float MENU_RADIUS = 300.0f; // Radius of the radial menu
const float SLOT_RADIUS = 30.0f; // Radius of each ingredient slot


CircleMenuInformation createCircleInformation(float innerRadius,float outerRadius, int edges)
{
	CircleMenuInformation inf;

	std::vector<std::vector<glm::vec2>> quads;

	float angleStep = 2.0f * glm::pi<float>() / edges;

	for (int i = 0; i < edges; ++i)
	{
		float angle1 = i * angleStep;
		float angle2 = (i + 1) * angleStep;

		glm::vec2 inner1(innerRadius * cos(angle1), innerRadius * sin(angle1));
		glm::vec2 inner2(innerRadius * cos(angle2), innerRadius * sin(angle2));
		glm::vec2 outer1(outerRadius * cos(angle1), outerRadius * sin(angle1));
		glm::vec2 outer2(outerRadius * cos(angle2), outerRadius * sin(angle2));

		quads.push_back({ inner1,outer1,outer2,inner2 });
		glm::vec2 center = (inner1 + outer1 + outer2 + inner2) / 4.0f;
		inf.slotCenters.push_back(center);
	}
	
	inf.quads = quads;

	GLuint VAO, VBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Allocate buffer size (enough for all quads)
	glBufferData(GL_ARRAY_BUFFER, edges * 4 * sizeof(glm::vec2), nullptr, GL_DYNAMIC_DRAW);

	// Set vertex attribute pointers
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	inf.VAO = VAO;
	inf.VBO = VBO;
	return inf;
}

void Renderer::UpdateInventoryMenu(const Inventory* inv)
{
	this->info = createCircleInformation(100, 200, inv->inventorySize());
}

void Renderer::RenderCookingMenu(const Inventory* inv) 
{
	CircleMenuInformation info = this->info;
	auto& shader = ResourceManager::GetShader("CookingMenu");
	shader.Use();

	const float centerX = Common::ScreenSize.x / 2;       // Center of the menu
	const float centerY = Common::ScreenSize.y / 2;       // Center of the menu
	const float angleStep = 360.0f / inv->inventorySize(); // Angle between slots


	int slotIndex = 0;
	for (const auto& [ingredient, quantity] : inv->stock) {
		if (quantity <= 0) continue; // Skip empty inventory slots

		// Update VBO with the current slot's quad vertices
		glBindBuffer(GL_ARRAY_BUFFER, info.VBO);
		//glBufferData(GL_ARRAY_BUFFER, info.quads[slotIndex].size() * sizeof(glm::vec2), info.quads[slotIndex].data(), GL_DYNAMIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, info.quads[slotIndex].size() * sizeof(glm::vec2), info.quads[slotIndex].data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Set per-slot uniforms (e.g., color)
		glm::vec3 color = glm::vec3((float)slotIndex / info.quads.size(), 0.5f, 1.0f);
		shader.Use();
		shader.SetUniform("center", glm::vec2(centerX, centerY));
		shader.SetUniform("uColor", glm::vec4(color.x, color.y, color.z, 0.6f));

		glBindVertexArray(info.VAO);
		GLint currentVAO;
		glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &currentVAO);
		std::cout << "Currently bound VAO: " << currentVAO << std::endl;
		// Draw the quad
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		// Render additional elements (e.g., icons) at the slot's center
		glm::vec2 center = info.slotCenters[slotIndex];
		RenderSprite(ResourceManager::GetTexture("tomato"), center + glm::vec2(centerX, centerY), 0.0f, glm::vec2(16.0f));

		slotIndex++;
	}
	glBindVertexArray(0);
	glUseProgram(0);
}