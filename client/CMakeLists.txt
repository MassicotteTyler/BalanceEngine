project(game VERSION 0.1 LANGUAGES CXX)

include_directories(Include)

add_executable(balance
  src/Aircraft.cpp
  src/Player.cpp
  src/CommandQueue.cpp
  src/Command.cpp
  src/Entity.cpp
  src/SceneNode.cpp
  src/SpriteNode.cpp
  src/Utility.cpp
  src/NetworkNode.cpp
  src/World.cpp
  src/GameClient.cpp
  src/Main.cpp)

target_link_libraries(balance
  ${SFML_LIBRARIES}
  )
