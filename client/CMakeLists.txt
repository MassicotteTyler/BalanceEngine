project(game VERSION 0.1 LANGUAGES CXX)

include_directories(Include)

add_executable(balance
  src/Aircraft.cpp
  src/Entity.cpp
  src/Game.cpp
  src/SceneNode.cpp
  src/SpriteNode.cpp
  src/World.cpp
  src/Main.cpp)

target_link_libraries(balance
  ${SFML_LIBRARIES}
  )
