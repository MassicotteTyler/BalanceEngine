#include <SpriteNode.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

SpriteNode::SpriteNode(const sf::Texture& texture)
: _Sprite(texture)
{
}

SpriteNode::SpriteNode(const sf::Texture& texture, sf::IntRect& textureRect)
: _Sprite(texture, textureRect)
{
}

void SpriteNode::drawCurrent(sf::RenderTarget& target,
    sf::RenderStates states) const
{
  target.draw(_Sprite, states);
}
