// To compile and run:
// clang++ -std=c++11 main.cpp -o ecs && ./ecs
#include <vector>
#include <stdio.h>
#include <typeindex>
#include <unordered_map>
#include <cassert>

#define assertm(exp, msg) assert(((void)msg, exp))

class Entity;
class Component;
class System;
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

        Entity() {
            printf("%s: Initialised\n", typeid(this).name());
        }
        
        template <typename T, typename... Args>
        T* addComponent(Args&&... args) {
            if (components.find(typeid(T)) != components.end()) { // FOUND EXISTING
                printf("%s: Already has '%s'!\n", typeid(this).name(), typeid(T).name());
                return (T*)components[typeid(T)];
            }
            T* newComponent = new T(std::forward<Args>(args)...);
            newComponent->entity = this;
            components[typeid(T)] = newComponent;
            newComponent->init();
            return newComponent;
        }

        template <typename T>
        T* getComponent() {
            if (components.find(typeid(T)) == components.end()) { // NOT FOUND
                printf("%s: Cannot find Component of type '%s'!\n", typeid(this).name(), typeid(T).name());
                return nullptr;
            }
            return (T*)components[typeid(T)];
        }

        template <typename T>
        void removeComponent() {
            components.erase(typeid(T));
            printf("%s: Removed '%s'\n", typeid(this).name(), typeid(T).name());
        }
};

class System {
    public:
        std::vector<Component> components;

        void update(float deltaTime) {
            for (size_t i = 0; i < components.size(); i++) {
                components[i].update(deltaTime);
            }
        }
};

class PositionComponent : public Component {
    public:
        float x, y;

        PositionComponent() : x(0), y(0) {
            printf("%s: Initialised at (%f, %f)\n", typeid(this).name(), x, y);
        }

        PositionComponent(float _x, float _y) : x(_x), y(_y) {
            printf("%s: Initialised at (%f, %f)\n", typeid(this).name(), x, y);
        }

        void update(float deltaTime) {
            printf("%s: Updated\n", typeid(this).name());
        }
};

class RenderComponent : public Component {
    public:
        RenderComponent() {
            printf("%s: Initialised\n", typeid(this).name());
        }

        void update(float deltaTime) {
            printf("%s: Updated\n", typeid(this).name());
        }
};

class PositionSystem : public System {
    public:
        std::vector<PositionComponent> components;

        PositionSystem() {
            printf("%s: Initialised\n", typeid(this).name());
        };
};

class RenderSystem : public System {
    public:
        std::vector<RenderComponent> components;

        RenderSystem() {
            printf("%s: Initialised\n", typeid(this).name());
        };

        void update(float deltaTime) {
            printf("%s: Updated\n", typeid(this).name());
        }
};

class GameEngine {
    public:
        std::vector<Entity> entities;
        std::vector<System> systems;

        void update(float deltaTime) {
            for (size_t i = 0; i < systems.size(); i++) {
                systems[i].update(deltaTime);
            }
        }
        
        template<typename T>
        T* createEntity() {
            T *entity = new T();
            entities.push_back(*entity);
            return entity;
        }

        template<typename T>
        T* addSystem() {
            T* system = new T();
            systems.push_back(*system);
            return system;
        }
};

class PlayerEntity : public Entity {
};


int main(){
    // TODO: Have entities 
    // TODO: Use variadic function template for createEntity
    // TODO: have component added to system
    testComponents();
    return 0;
}

void testComponents(){
    GameEngine *gameEngine = new GameEngine();

    // INITIALISE SYSTEMS
    gameEngine->addSystem<PositionSystem>();
    gameEngine->addSystem<RenderSystem>();

    PlayerEntity *player = gameEngine->createEntity<PlayerEntity>();

    assertm(player->components.size() == 0, "components is not empty!\n");

    PositionComponent *pos = player->addComponent<PositionComponent>(0.1, 0.2);

    assertm(pos, "Getting existing Component returned null!\n");
    assertm(player->components.size() == 1, "The size of components is not 1!\n");
    
    PositionComponent *pos_b = player->getComponent<PositionComponent>();

    assertm(pos_b, "Getting existing Component returned null!\n");
    assertm(player->components.size() == 1, "The size of components is not 1!\n");

    RenderComponent *ren = player->getComponent<RenderComponent>();

    assertm(!ren, "Getting non-existent Component did not return null!\n");
    assertm(player->components.size() == 1, "The size of components is not 1!\n");

    RenderComponent *ren_b = player->addComponent<RenderComponent>();

    assertm(ren_b, "Getting existing Component returned null!\n");
    assertm(player->components.size() == 2, "The size of components is not 2!\n");

    player->removeComponent<PositionComponent>();

    assertm(player->components.size() == 1, "The size of components is not 1!\n");
    
    PositionComponent *pos_c = player->getComponent<PositionComponent>();

    assertm(!pos_c, "Getting non-existent Component did not return null!\n");

    player->removeComponent<RenderComponent>();

    assertm(player->components.size() == 0, "The size of components is not 0!\n");

    printf("Finished!\n");
}
