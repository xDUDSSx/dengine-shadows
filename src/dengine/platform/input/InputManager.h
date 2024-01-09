#pragma once

#include <map>
#include <utility>
#include <vector>
#include <array>
#include <unordered_map>
#include <functional>
#include <string>

#include "glm/vec2.hpp"

#include "KeyCodes.h"

using Modifiers = std::array<bool, 6L>;
using ModifiersList = std::vector<Keys::Code>;
using ActionCallback = std::function<void(float)>;

/**
 * \brief Class for providing input state. Collects state from GLFW and ImGui.
 *
 * This class stores state of all keys and mouse buttons in a map \a keyMap -
 * used for controlling all interaction. It also updates the stored statuses of
 * the keys from JUST pressed/released to pressed/release.
 */
class InputManager
{
  public:
	enum KeyState
	{
		UP,
		JUST_UP,
		DOWN,
		JUST_DOWN
	}; ///< state of the key used in keyMap - enum { 0 - up, 1 - just up, 2 -
	   ///< down, 3 - just down}

	/// States of all keys, modifiers, and mouse buttons - enum value KeyState.
	/// changed in update()
	static std::map<Keys::Code, KeyState> m_keyMap;

	static float m_mouseX; ///< Current mouse cursor X position in screen coordinates (0 is corner of the monitor)
	static float m_mouseY; ///< Current mouse cursor Y position in screen coordinates (0 is corner of the monitor)
	static float m_mouseXPrev, m_mouseYPrev;   ///< Previous frame's cursor position
	static float m_mouseXDelta, m_mouseYDelta; ///< Change of the cursor position across last two frames
	static float m_mouseXDragDelta;            ///< Same as mouse X delta but only non-zero during some mouse press
	static float m_mouseYDragDelta;            ///< Same as mouse Y delta but only non-zero during some mouse press
	static float m_mouseWheelOffset;           ///< Immediate mouse scroll change

	struct ActionBinding
	{
		// Keys::Code code;
		// Modifiers modifiers;
		KeyState keyState;
		ActionCallback fn;
		// float value;

		ActionBinding(KeyState keyState, ActionCallback callback) : keyState(keyState), fn(std::move(callback)) {}
	};

	struct KeyBinding
	{
		Keys::Code code;
		Modifiers modifiers;
		float value{0.0f};

		KeyBinding(Keys::Code code, const Modifiers& modifiers, float value);
	};

	struct Action
	{
		std::string name;

		std::vector<KeyBinding> keyBinds;
		std::vector<ActionBinding> actionBinds;

		explicit Action(std::string actionName) : name(std::move(actionName)) {}
	};

	static std::unordered_map<std::string, Action> m_actions;

	static Action* createAction(const std::string& name);
	static Action* getAction(const std::string& name);
	static Action* getOrCreateAction(const std::string& name);
	static bool isActionCreated(const std::string& name);

	static void triggerAction(std::string name, bool keyPress = true, float value = 0.0f);
	/// Whether the action has JUST been triggered by a key
	static bool isActionTriggered(const std::string& name, bool keyPress = true);
	/// Whether the action key binds are being held down
	static bool isActionActive(const std::string& name, bool keyPress = true);

	static void bindKey(const std::string& name, Keys::Code code, ModifiersList mods = ModifiersList(), float value = 0.0f);
	static void bindAction(const std::string& name, const ActionCallback& fn, bool pressed = true, bool longAction = false);
	static inline void bindLongAction(std::string name, ActionCallback fn, bool pressed = true)
	{
		bindAction(std::move(name), std::move(fn), pressed, true);
	}

	static Modifiers createModifiers(ModifiersList& list);
	static bool areModifiersActive(Modifiers mods);

  public:
	static void init();

	/**
	 * Updates mouse position and buttons
	 */
	static void beginFrame();

	/**
	 * Updates the stored statuses of the keys in the \a keyMap array (JUST_UP -> UP, JUST_DOWN -> DOWN).
	 * Processes events.
	 */
	static void endFrame();

	static inline glm::vec2 getMouseDelta()
	{
		return {m_mouseXDelta, m_mouseYDelta};
	}
	static inline glm::vec2 getMousePos()
	{
		return {m_mouseX, m_mouseY};
	}
	static inline void setPressed(const Keys::Code code)
	{
		m_keyMap[code] = JUST_DOWN;
	}
	static inline void setUnpressed(const Keys::Code code)
	{
		m_keyMap[code] = JUST_UP;
	}
	static inline bool isKeyPressed(const Keys::Code code)
	{
		return (m_keyMap[code] == DOWN || m_keyMap[code] == JUST_DOWN);
	}
	static inline bool isKeyJustPressed(const Keys::Code code)
	{
		return (m_keyMap[code] == JUST_DOWN);
	}
	static inline bool isKeyJustUp(const Keys::Code code)
	{
		return (m_keyMap[code] == JUST_UP);
	}

	/**
	 * @return Whether any of the mouse buttons was clicked
	 */
	static bool isMouseClicked();

	/**
	 * @return Whether any of the mouse buttons is pressed
	 */
	static bool isMouseDown();

	static void keyDown(int keyPressed);
	static void keyUp(int keyReleased);

  private:
	static void processEvents();
};
