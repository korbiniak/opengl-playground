#ifndef COMPONENT_H
#define COMPONENT_H

#include <string>

class GameObject;

class Component {
 protected:
  GameObject* gameObject;
  bool enabled;

 public:
  Component() : gameObject(nullptr), enabled(true) {}
  virtual ~Component() = default;

  virtual void onAttach(GameObject* obj) { gameObject = obj; }

  virtual void update(float deltaTime) = 0;

  virtual void onDetach() { gameObject = nullptr; }

  virtual std::string getTypeName() const = 0;

  bool isEnabled() const { return enabled; }
  void setEnabled(bool e) { enabled = e; }

  GameObject* getGameObject() const { return gameObject; }
};

#endif /* COMPONENT_H */
