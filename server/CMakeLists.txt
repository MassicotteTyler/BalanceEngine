project(game VERSION 0.1 LANGUAGES CXX)

include_directories(Include)

add_executable(bServer
  src/Aircraft.cpp
  src/Player.cpp
  src/CommandQueue.cpp
  src/Command.cpp
  src/Entity.cpp
  src/Game.cpp
  src/GameServer.cpp
  src/SceneNode.cpp
  src/SpriteNode.cpp
  src/World.cpp
  src/Main.cpp)

target_link_libraries(bServer
  ${SFML_LIBRARIES}
  )
