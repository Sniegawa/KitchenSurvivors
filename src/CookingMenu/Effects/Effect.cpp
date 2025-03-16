#include "Effect.h"

#include "../../Objects/Player.h"
#include "../../ResourceHandlers/ResourceManager.h"

Effect::Effect(std::string _name, std::string _desc, const Texture2D& _sprite, Player* _playerPointer) : Name(_name),Description(_desc),Sprite(_sprite),m_playerptr(_playerPointer){}