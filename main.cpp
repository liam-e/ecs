// To compile and run:
// clang++ -std=c++11 main.cpp -o ecs && ./ecs
#include <vector>
#include <stdio.h>
#include <typeindex>
#include <unordered_map>
#include <map>
#include <cassert>
#include <algorithm>


#define assertm(exp, msg) assert(((void)msg, exp))

class Entity;
class Component;
class Game;
void testComponents();

class Component {
    public:
        Entity* entity;

        virtual void init(){}

        virtual void update(float deltaTime){}
};

class Entity {
    public:
        std::unordered_map<std::type_index, Component*> components;

        template <typename T>
        bool hasComponent() {
            return components.find(typeid(T)) != components.end();
        }
};

template <typename T>
class System {
    public:
        std::vector<T> components;

        T* addComponentToEntity(Entity* entity) {
            if (entity->components.find(typeid(T)) != entity->components.end()){
                printf("Entity already has component of type %s\n", typeid(T).name());
                return static_cast<T*>(entity->components[typeid(T)]);
            }

            components.emplace_back();
            T *newComponent = &components.back();

            newComponent->entity = entity;
            entity->components[typeid(T)] = newComponent;

            return newComponent;
        }

        void removeComponentFromEntity(Entity* entity) {
            if (entity->components.find(typeid(T)) == entity->components.end()){
                return;
            }

            for (size_t i = 0; i < components.size(); i++) {
                if (&components[i] == entity->components[typeid(T)]) {
                    components.erase(components.begin() + i);
                    break;
                }
            }
            entity->components.erase(typeid(T));
        }

        void update(float deltaTime) {
            for (size_t i = 0; i < components.size(); i++) {
                components[i].update(deltaTime);
            }
        }
};

class PositionComponent : public Component {
    public:
        float x, y;

        PositionComponent() : x(0), y(0) {}

        PositionComponent(float _x, float _y) : x(_x), y(_y) {}
};

class RenderComponent : public Component {};

using PositionSystem = System<PositionComponent>;
using RenderSystem = System<RenderComponent>;

class Game {
    public:
        std::vector<Entity> entities;
        std::map<std::type_index, void*> systemMap;
        
        template<typename T>
        T* createEntity() {
            T *entity = new T();
            entities.push_back(*entity);
            return entity;
        }

        template<typename T>
        T* addSystem() {
            T* system = new T();
            systemMap[typeid(T)] = system;
            return system;
        }

        template<typename T>
        bool hasSystem() {
            return systemMap.find(typeid(T)) != systemMap.end();
        }

        template<typename T>
        T* getSystem() {
            if (!hasSystem<T>()) {
                printf("System of type %s does not exist\n", typeid(T).name());
                return nullptr;
            }
            return static_cast<T*>(systemMap[typeid(T)]);
        }

        void update(float deltaTime) {
            for (const auto &pair : systemMap){
                static_cast<System<Component>*>(pair.second)->update(deltaTime);
            }
        }
};

class PlayerEntity : public Entity {};


int main(){
    testComponents();
    
    printf("All tests passed!\n");

    return 0;
}

void testComponents(){
    Game *game = new Game();

    // INITIALISE SYSTEMS
    PositionSystem *positionSystem = game->addSystem<PositionSystem>();
    RenderSystem *renderSystem = game->addSystem<RenderSystem>();

    PlayerEntity *player = game->createEntity<PlayerEntity>();

    game->getSystem<PositionSystem>()->addComponentToEntity(player);

    assertm(player->hasComponent<PositionComponent>(), "Player does not have PositionComponent!\n");
    assertm(player->components.size() == 1, "The size of components is not 1!\n");

    game->getSystem<RenderSystem>()->addComponentToEntity(player);

    assertm(player->hasComponent<RenderComponent>(), "Player does not have RenderComponent!\n");
    assertm(player->components.size() == 2, "The size of components is not 2!\n");

    game->update(0.1f);

    game->getSystem<PositionSystem>()->removeComponentFromEntity(player);

    assertm(!player->hasComponent<PositionComponent>(), "Player still has PositionComponent!\n");
    assertm(player->components.size() == 1, "The size of components is not 1!\n");

    game->getSystem<RenderSystem>()->removeComponentFromEntity(player);

    assertm(!player->hasComponent<RenderComponent>(), "Player still has RenderComponent!\n");
    assertm(player->components.size() == 0, "The size of components is not 0!\n");
}
