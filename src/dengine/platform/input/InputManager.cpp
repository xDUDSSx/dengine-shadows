#include "InputManager.h"

#include <GLFW/glfw3.h>

#include <utility>

#include "dengine/platform/Common.h"
#include "dengine/platform/Logger.h"

#include "imgui.h"

#define IM_MOUSE_KEYS_COUNT 3

constexpr Keys::Code imGuiMouseKeys[] = {Keys::mouseLeft, Keys::mouseRight, Keys::mouseMiddle};

std::map<Keys::Code, InputManager::KeyState> InputManager::m_keyMap;

float InputManager::m_mouseX = 0;
float InputManager::m_mouseY = 0;
float InputManager::m_mouseXPrev = 0;
float InputManager::m_mouseYPrev = 0;
float InputManager::m_mouseXDelta = 0;
float InputManager::m_mouseYDelta = 0;
float InputManager::m_mouseXDragDelta = 0;
float InputManager::m_mouseYDragDelta = 0;
float InputManager::m_mouseWheelOffset = 0;

std::unordered_map<std::string, InputManager::Action> InputManager::m_actions;

void InputManager::init()
{
	bindKey("pan", Keys::mouseMiddle, {});
	bindKey("rotate", Keys::mouseRight, {});
	bindKey("zoom", Keys::mouseScrlUp, {}, +1.0f);
	bindKey("zoom", Keys::mouseScrlDown, {}, -1.0f);
}

/**
 * If you change order of modifiers, change it also in
 * InputManager::areModifiersActive function.
 */
Modifiers InputManager::createModifiers(ModifiersList& list)
{
	Modifiers mods = {false, false, false, false, false, false};
	for (auto mod : list)
	{
		switch (mod)
		{
		case Keys::Code::ctrll:
			mods[0] = true;
			break;
		case Keys::Code::altl:
			mods[1] = true;
			break;
		case Keys::Code::shiftl:
			mods[2] = true;
			break;
		case Keys::Code::ctrlr:
			mods[3] = true;
			break;
		case Keys::Code::altr:
			mods[4] = true;
			break;
		case Keys::Code::shiftr:
			mods[5] = true;
			break;
		default:
			break;
		}
	}

	return mods;
}

bool InputManager::areModifiersActive(Modifiers mods)
{
	bool active = mods[0] == isKeyPressed(Keys::Code::ctrll);
	active &= mods[1] == isKeyPressed(Keys::Code::altl);
	active &= mods[2] == isKeyPressed(Keys::Code::shiftl);
	active &= mods[3] == isKeyPressed(Keys::Code::ctrlr);
	active &= mods[4] == isKeyPressed(Keys::Code::altr);
	active &= mods[5] == isKeyPressed(Keys::Code::shiftr);
	return active;
}

bool InputManager::isMouseClicked()
{
	for (int i = 0; i < IM_MOUSE_KEYS_COUNT; i++)
	{
		if (ImGui::IsMouseClicked(i))
		{
			return true;
		}
	}
	return false;
}

bool InputManager::isMouseDown()
{
	for (int i = 0; i < IM_MOUSE_KEYS_COUNT; i++)
	{
		if (ImGui::IsMouseDown(i))
		{
			return true;
		}
	}
	return false;
}

void InputManager::beginFrame()
{
	ImGuiIO& io = ImGui::GetIO();

	// Update mouse position
	if (ImGui::IsMousePosValid())
	{
		m_mouseX = io.MousePos.x;
		m_mouseY = io.MousePos.y;
	}

	m_mouseXDelta = io.MouseDelta.x;
	m_mouseYDelta = io.MouseDelta.y;

	m_mouseXPrev = io.MousePosPrev.x;
	m_mouseYPrev = io.MousePosPrev.y;

	// TODO: (DR) drag delta is unused, haven't tested if it works properly after changes, should be
	/* \todo JH \todo MH probably very naive */
	if (isMouseDown())
	{
		m_mouseXDragDelta += m_mouseXDelta;
		m_mouseYDragDelta += m_mouseYDelta;
	}
	else
	{
		m_mouseXDragDelta = m_mouseYDragDelta = 0;
	}

	// Update left, right and middle button.
	for (int i = 0; i < IM_MOUSE_KEYS_COUNT; i++)
	{
		if (ImGui::IsMouseClicked(i))
			setPressed(imGuiMouseKeys[i]);
		if (ImGui::IsMouseReleased(i))
			setUnpressed(imGuiMouseKeys[i]);
	}

	// Update mouse wheel
	if (abs(io.MouseWheel) < 0.0001)
	{
		InputManager::setUnpressed(Keys::mouseScrlDown);
		InputManager::setUnpressed(Keys::mouseScrlUp);
	}
	else if (io.MouseWheel > 0.0)
	{
		InputManager::setPressed(Keys::mouseScrlUp);
		InputManager::setUnpressed(Keys::mouseScrlDown);
	}
	else if (io.MouseWheel < 0.0)
	{
		InputManager::setPressed(Keys::mouseScrlDown);
		InputManager::setUnpressed(Keys::mouseScrlUp);
	}
	InputManager::m_mouseWheelOffset = io.MouseWheel;
}

void InputManager::endFrame()
{
	processEvents();

	// Process keys
	for (std::map<Keys::Code, KeyState>::const_iterator it = m_keyMap.begin(); it != m_keyMap.end(); ++it)
	{
		if (it->second == JUST_UP)
		{
			m_keyMap[it->first] = UP;
		}
		else if (it->second == JUST_DOWN)
		{
			m_keyMap[it->first] = DOWN;
		}
	}

	m_keyMap[Keys::Code::mouseScrlUp] = KeyState::UP;
	m_keyMap[Keys::Code::mouseScrlDown] = KeyState::UP;
	m_mouseWheelOffset = 0;
}

void InputManager::processEvents()
{
	for (auto& [actionName, action] : m_actions)
	{
		for (const auto& keyBind : action.keyBinds)
		{
			if (!areModifiersActive(keyBind.modifiers))
				continue;

			KeyState keyState = m_keyMap[keyBind.code];

			for (const auto& actionBind : action.actionBinds)
			{
				bool execute = false;
				switch (actionBind.keyState)
				{
				case DOWN:
					execute |= keyState == DOWN;
				case JUST_DOWN:
					execute |= keyState == JUST_DOWN;
					break;
				case UP:
					execute |= keyState == UP;
				case JUST_UP:
					execute |= keyState == JUST_UP;
					break;
				}
				if (execute)
					actionBind.fn(keyBind.value);
			}
		}
	}
}

InputManager::Action* InputManager::createAction(const std::string& name)
{
	Action* action = getAction(name);
	if (action == nullptr)
	{
		return &m_actions.emplace(name, name).first->second;
	}
	return action;
}

void InputManager::triggerAction(std::string name, bool keyPress, float value)
{
	Action* action = getAction(name);
	if (action == nullptr)
	{
		LOG_ERROR("InputManager: Cannot trigger action! Action '{}' does not exist.", name);
		return;
	}

	KeyState targetKeyState = keyPress ? DOWN : UP;
	for (const auto& bind : action->actionBinds)
	{
		if (bind.keyState == targetKeyState)
		{
			bind.fn(value);
		}
	}
}

InputManager::Action* InputManager::getAction(const std::string& name)
{
	auto it = m_actions.find(name);
	if (it != m_actions.end())
	{
		return &it->second;
	}
	return nullptr;
}

InputManager::Action* InputManager::getOrCreateAction(const std::string& name)
{
	auto it = m_actions.find(name);
	if (it != m_actions.end())
	{
		return &it->second;
	}
	return createAction(name);
}

bool InputManager::isActionCreated(const std::string& name)
{
	return getAction(name) != nullptr;
}

bool InputManager::isActionTriggered(const std::string& name, bool keyPress)
{
	Action* action = getAction(name);
	if (action == nullptr)
		return false;

	for (const auto& keyBind : action->keyBinds)
	{
		if (keyPress)
		{
			return isKeyJustPressed(keyBind.code) && areModifiersActive(keyBind.modifiers);
		}
		else
		{
			return isKeyJustUp(keyBind.code) && areModifiersActive(keyBind.modifiers);
		}
	}
}

bool InputManager::isActionActive(const std::string& name, bool keyPress)
{
	Action* action = getAction(name);
	if (action == nullptr)
		return false;

	for (const auto& keyBind : action->keyBinds)
	{
		if (keyPress)
		{
			return isKeyPressed(keyBind.code) && areModifiersActive(keyBind.modifiers);
		}
		else
		{
			return !isKeyPressed(keyBind.code) && areModifiersActive(keyBind.modifiers);
		}
	}
}

void InputManager::bindKey(const std::string& name, Keys::Code code, ModifiersList mods, float value)
{
	Action* action = getOrCreateAction(name);
	action->keyBinds.emplace_back(code, createModifiers(mods), value);
}

void InputManager::bindAction(const std::string& name, const ActionCallback& fn, bool pressed, bool longAction)
{
	Action* action = getOrCreateAction(name);
	action->actionBinds.emplace_back(pressed ? (longAction ? DOWN : JUST_DOWN) : (longAction ? UP : JUST_UP), fn);
}

static ImGuiKey keyCodeToImGui(const Keys::Code code)
{
	switch (code)
	{
	case Keys::a:
		return ImGuiKey_A;
	case Keys::b:
		return ImGuiKey_B;
	case Keys::c:
		return ImGuiKey_C;
	case Keys::d:
		return ImGuiKey_D;
	case Keys::e:
		return ImGuiKey_E;
	case Keys::f:
		return ImGuiKey_F;
	case Keys::g:
		return ImGuiKey_G;
	case Keys::h:
		return ImGuiKey_H;
	case Keys::i:
		return ImGuiKey_I;
	case Keys::j:
		return ImGuiKey_J;
	case Keys::k:
		return ImGuiKey_K;
	case Keys::l:
		return ImGuiKey_L;
	case Keys::m:
		return ImGuiKey_M;
	case Keys::n:
		return ImGuiKey_N;
	case Keys::o:
		return ImGuiKey_O;
	case Keys::p:
		return ImGuiKey_P;
	case Keys::q:
		return ImGuiKey_Q;
	case Keys::r:
		return ImGuiKey_R;
	case Keys::s:
		return ImGuiKey_S;
	case Keys::t:
		return ImGuiKey_T;
	case Keys::u:
		return ImGuiKey_U;
	case Keys::v:
		return ImGuiKey_V;
	case Keys::w:
		return ImGuiKey_W;
	case Keys::x:
		return ImGuiKey_X;
	case Keys::y:
		return ImGuiKey_Y;
	case Keys::z:
		return ImGuiKey_Z;
	case Keys::n1:
		return ImGuiKey_1;
	case Keys::n2:
		return ImGuiKey_2;
	case Keys::n3:
		return ImGuiKey_3;
	case Keys::n4:
		return ImGuiKey_4;
	case Keys::n5:
		return ImGuiKey_5;
	case Keys::n6:
		return ImGuiKey_6;
	case Keys::n7:
		return ImGuiKey_7;
	case Keys::n8:
		return ImGuiKey_8;
	case Keys::n9:
		return ImGuiKey_9;
	case Keys::n0:
		return ImGuiKey_0;
	case Keys::altr:
		return ImGuiKey_RightAlt;
	case Keys::altl:
		return ImGuiKey_LeftAlt;
	case Keys::ctrll:
		return ImGuiKey_LeftCtrl;
	case Keys::ctrlr:
		return ImGuiKey_RightCtrl;
	case Keys::shiftl:
		return ImGuiKey_LeftShift;
	case Keys::shiftr:
		return ImGuiKey_RightShift;
	case Keys::left:
		return ImGuiKey_LeftArrow;
	case Keys::right:
		return ImGuiKey_RightArrow;
	case Keys::up:
		return ImGuiKey_UpArrow;
	case Keys::down:
		return ImGuiKey_DownArrow;
	case Keys::f1:
		return ImGuiKey_F1;
	case Keys::f2:
		return ImGuiKey_F2;
	case Keys::f3:
		return ImGuiKey_F3;
	case Keys::f4:
		return ImGuiKey_F4;
	case Keys::f5:
		return ImGuiKey_F5;
	case Keys::f6:
		return ImGuiKey_F6;
	case Keys::f7:
		return ImGuiKey_F7;
	case Keys::f8:
		return ImGuiKey_F8;
	case Keys::f9:
		return ImGuiKey_F9;
	case Keys::f10:
		return ImGuiKey_F10;
	case Keys::f11:
		return ImGuiKey_F11;
	case Keys::f12:
		return ImGuiKey_F12;
	case Keys::home:
		return ImGuiKey_Home;
	case Keys::end:
		return ImGuiKey_End;
	case Keys::insert:
		return ImGuiKey_Insert;
	case Keys::del:
		return ImGuiKey_Delete;
	case Keys::pageUp:
		return ImGuiKey_PageUp;
	case Keys::pageDown:
		return ImGuiKey_PageDown;
	case Keys::esc:
		return ImGuiKey_Escape;
	default:
		return ImGuiKey_None;
	}
}

void InputManager::keyDown(int keyPressed)
{
	switch (keyPressed)
	{
		// alphabet a-z
	case GLFW_KEY_A:
		setPressed(Keys::a);
		break;
	case GLFW_KEY_B:
		setPressed(Keys::b);
		break;
	case GLFW_KEY_C:
		setPressed(Keys::c);
		break;
	case GLFW_KEY_D:
		setPressed(Keys::d);
		break;
	case GLFW_KEY_E:
		setPressed(Keys::e);
		break;
	case GLFW_KEY_F:
		setPressed(Keys::f);
		break;
	case GLFW_KEY_G:
		setPressed(Keys::g);
		break;
	case GLFW_KEY_H:
		setPressed(Keys::h);
		break;
	case GLFW_KEY_I:
		setPressed(Keys::i);
		break;
	case GLFW_KEY_J:
		setPressed(Keys::j);
		break;
	case GLFW_KEY_K:
		setPressed(Keys::k);
		break;
	case GLFW_KEY_L:
		setPressed(Keys::l);
		break;
	case GLFW_KEY_M:
		setPressed(Keys::m);
		break;
	case GLFW_KEY_N:
		setPressed(Keys::n);
		break;
	case GLFW_KEY_O:
		setPressed(Keys::o);
		break;
	case GLFW_KEY_P:
		setPressed(Keys::p);
		break;
	case GLFW_KEY_Q:
		setPressed(Keys::q);
		break;
	case GLFW_KEY_R:
		setPressed(Keys::r);
		break;
	case GLFW_KEY_S:
		setPressed(Keys::s);
		break;
	case GLFW_KEY_T:
		setPressed(Keys::t);
		break;
	case GLFW_KEY_U:
		setPressed(Keys::u);
		break;
	case GLFW_KEY_V:
		setPressed(Keys::v);
		break;
	case GLFW_KEY_W:
		setPressed(Keys::w);
		break;
	case GLFW_KEY_X:
		setPressed(Keys::x);
		break;
	case GLFW_KEY_Y:
		setPressed(Keys::y);
		break;
	case GLFW_KEY_Z:
		setPressed(Keys::z);
		break;

		// numbers
	case GLFW_KEY_KP_0:
		setPressed(Keys::n0);
		break;
	case GLFW_KEY_KP_1:
		setPressed(Keys::n1);
		break;
	case GLFW_KEY_KP_2:
		setPressed(Keys::n2);
		break;
	case GLFW_KEY_KP_3:
		setPressed(Keys::n3);
		break;
	case GLFW_KEY_KP_4:
		setPressed(Keys::n4);
		break;
	case GLFW_KEY_KP_5:
		setPressed(Keys::n5);
		break;
	case GLFW_KEY_KP_6:
		setPressed(Keys::n6);
		break;
	case GLFW_KEY_KP_7:
		setPressed(Keys::n7);
		break;
	case GLFW_KEY_KP_8:
		setPressed(Keys::n8);
		break;
	case GLFW_KEY_KP_9:
		setPressed(Keys::n9);
		break;

		// specials
	case GLFW_KEY_ESCAPE:
		setPressed(Keys::esc);
		break;

	case GLFW_KEY_LEFT_ALT:
		setPressed(Keys::altl);
		break;
	case GLFW_KEY_RIGHT_ALT:
		setPressed(Keys::altr);
		break;
	case GLFW_KEY_LEFT_CONTROL:
		setPressed(Keys::ctrll);
		break;
	case GLFW_KEY_RIGHT_CONTROL:
		setPressed(Keys::ctrlr);
		break;
	case GLFW_KEY_LEFT_SHIFT:
		setPressed(Keys::shiftl);
		break;
	case GLFW_KEY_RIGHT_SHIFT:
		setPressed(Keys::shiftr);
		break;

	case GLFW_KEY_UP:
		setPressed(Keys::up);
		break;
	case GLFW_KEY_DOWN:
		setPressed(Keys::down);
		break;
	case GLFW_KEY_LEFT:
		setPressed(Keys::left);
		break;
	case GLFW_KEY_RIGHT:
		setPressed(Keys::right);
		break;

	case GLFW_KEY_F1:
		setPressed(Keys::f1);
		break;
	case GLFW_KEY_F2:
		setPressed(Keys::f2);
		break;
	case GLFW_KEY_F3:
		setPressed(Keys::f3);
		break;
	case GLFW_KEY_F4:
		setPressed(Keys::f4);
		break;
	case GLFW_KEY_F5:
		setPressed(Keys::f5);
		break;
	case GLFW_KEY_F6:
		setPressed(Keys::f6);
		break;
	case GLFW_KEY_F7:
		setPressed(Keys::f7);
		break;
	case GLFW_KEY_F8:
		setPressed(Keys::f8);
		break;
	case GLFW_KEY_F9:
		setPressed(Keys::f9);
		break;
	case GLFW_KEY_F10:
		setPressed(Keys::f10);
		break;
	case GLFW_KEY_F11:
		setPressed(Keys::f11);
		break;
	case GLFW_KEY_F12:
		setPressed(Keys::f12);
		break;

	case GLFW_KEY_HOME:
		setPressed(Keys::home);
		break;
	case GLFW_KEY_INSERT:
		setPressed(Keys::insert);
		break;
	case GLFW_KEY_DELETE:
		setPressed(Keys::del);
		break;
	case GLFW_KEY_END:
		setPressed(Keys::end);
		break;
	case GLFW_KEY_PAGE_DOWN:
		setPressed(Keys::pageDown);
		break;
	case GLFW_KEY_PAGE_UP:
		setPressed(Keys::pageUp);
		break;

	default:; // printf("Unrecognized key pressed\n");
	}
}

void InputManager::keyUp(int keyReleased)
{
	// LOG_DEBUG("keyup: {}", keyReleased);

	switch (keyReleased)
	{
		// alphabet a-z
	case GLFW_KEY_A:
		setUnpressed(Keys::a);
		break;
	case GLFW_KEY_B:
		setUnpressed(Keys::b);
		break;
	case GLFW_KEY_C:
		setUnpressed(Keys::c);
		break;
	case GLFW_KEY_D:
		setUnpressed(Keys::d);
		break;
	case GLFW_KEY_E:
		setUnpressed(Keys::e);
		break;
	case GLFW_KEY_F:
		setUnpressed(Keys::f);
		break;
	case GLFW_KEY_G:
		setUnpressed(Keys::g);
		break;
	case GLFW_KEY_H:
		setUnpressed(Keys::h);
		break;
	case GLFW_KEY_I:
		setUnpressed(Keys::i);
		break;
	case GLFW_KEY_J:
		setUnpressed(Keys::j);
		break;
	case GLFW_KEY_K:
		setUnpressed(Keys::k);
		break;
	case GLFW_KEY_L:
		setUnpressed(Keys::l);
		break;
	case GLFW_KEY_M:
		setUnpressed(Keys::m);
		break;
	case GLFW_KEY_N:
		setUnpressed(Keys::n);
		break;
	case GLFW_KEY_O:
		setUnpressed(Keys::o);
		break;
	case GLFW_KEY_P:
		setUnpressed(Keys::p);
		break;
	case GLFW_KEY_Q:
		setUnpressed(Keys::q);
		break;
	case GLFW_KEY_R:
		setUnpressed(Keys::r);
		break;
	case GLFW_KEY_S:
		setUnpressed(Keys::s);
		break;
	case GLFW_KEY_T:
		setUnpressed(Keys::t);
		break;
	case GLFW_KEY_U:
		setUnpressed(Keys::u);
		break;
	case GLFW_KEY_V:
		setUnpressed(Keys::v);
		break;
	case GLFW_KEY_W:
		setUnpressed(Keys::w);
		break;
	case GLFW_KEY_X:
		setUnpressed(Keys::x);
		break;
	case GLFW_KEY_Y:
		setUnpressed(Keys::y);
		break;
	case GLFW_KEY_Z:
		setUnpressed(Keys::z);
		break;
		/// -------------------------

		// numbers
	case GLFW_KEY_KP_0:
		setUnpressed(Keys::n0);
		break;
	case GLFW_KEY_KP_1:
		setUnpressed(Keys::n1);
		break;
	case GLFW_KEY_KP_2:
		setUnpressed(Keys::n2);
		break;
	case GLFW_KEY_KP_3:
		setUnpressed(Keys::n3);
		break;
	case GLFW_KEY_KP_4:
		setUnpressed(Keys::n4);
		break;
	case GLFW_KEY_KP_5:
		setUnpressed(Keys::n5);
		break;
	case GLFW_KEY_KP_6:
		setUnpressed(Keys::n6);
		break;
	case GLFW_KEY_KP_7:
		setUnpressed(Keys::n7);
		break;
	case GLFW_KEY_KP_8:
		setUnpressed(Keys::n8);
		break;
	case GLFW_KEY_KP_9:
		setUnpressed(Keys::n9);
		break;

		// specials
	case GLFW_KEY_ESCAPE:
		setUnpressed(Keys::esc);
		break;

	case GLFW_KEY_LEFT_ALT:
		setUnpressed(Keys::altl);
		break;
	case GLFW_KEY_RIGHT_ALT:
		setUnpressed(Keys::altr);
		break;
	case GLFW_KEY_LEFT_CONTROL:
		setUnpressed(Keys::ctrll);
		break;
	case GLFW_KEY_RIGHT_CONTROL:
		setUnpressed(Keys::ctrlr);
		break;
	case GLFW_KEY_LEFT_SHIFT:
		setUnpressed(Keys::shiftl);
		break;
	case GLFW_KEY_RIGHT_SHIFT:
		setUnpressed(Keys::shiftr);
		break;

	case GLFW_KEY_UP:
		setUnpressed(Keys::up);
		break;
	case GLFW_KEY_DOWN:
		setUnpressed(Keys::down);
		break;
	case GLFW_KEY_LEFT:
		setUnpressed(Keys::left);
		break;
	case GLFW_KEY_RIGHT:
		setUnpressed(Keys::right);
		break;

	case GLFW_KEY_F1:
		setUnpressed(Keys::f1);
		break;
	case GLFW_KEY_F2:
		setUnpressed(Keys::f2);
		break;
	case GLFW_KEY_F3:
		setUnpressed(Keys::f3);
		break;
	case GLFW_KEY_F4:
		setUnpressed(Keys::f4);
		break;
	case GLFW_KEY_F5:
		setUnpressed(Keys::f5);
		break;
	case GLFW_KEY_F6:
		setUnpressed(Keys::f6);
		break;
	case GLFW_KEY_F7:
		setUnpressed(Keys::f7);
		break;
	case GLFW_KEY_F8:
		setUnpressed(Keys::f8);
		break;
	case GLFW_KEY_F9:
		setUnpressed(Keys::f9);
		break;
	case GLFW_KEY_F10:
		setUnpressed(Keys::f10);
		break;
	case GLFW_KEY_F11:
		setUnpressed(Keys::f11);
		break;
	case GLFW_KEY_F12:
		setUnpressed(Keys::f12);
		break;

	case GLFW_KEY_HOME:
		setUnpressed(Keys::home);
		break;
	case GLFW_KEY_INSERT:
		setUnpressed(Keys::insert);
		break;
	case GLFW_KEY_DELETE:
		setUnpressed(Keys::del);
		break;
	case GLFW_KEY_END:
		setUnpressed(Keys::end);
		break;
	case GLFW_KEY_PAGE_DOWN:
		setUnpressed(Keys::pageDown);
		break;
	case GLFW_KEY_PAGE_UP:
		setUnpressed(Keys::pageUp);
		break;

	default:
		LOG_ERROR("Unrecognized key pressed!");
	}
}

InputManager::KeyBinding::KeyBinding(Keys::Code code, const Modifiers& modifiers, float value)
    : code(code), modifiers(modifiers), value(value)
{
}
