#include <Windows.h>
#include "Input.hpp"

Input::Input(int code)
	: mCode(code)
	, mCount(0)
{
	mState[0] = mState[1] = false;
}

bool Input::isPress()
{
	return isDown() && mState[mCount] == false;
}

bool Input::isDown()
{
	return mState[1 - mCount] == true;
}

bool Input::isUp()
{
	return mState[1 - mCount] == false;
}

void Input::Update()
{
	mState[mCount] = GetAsyncKeyState(mCode) == 0 ? false : true;
	mCount = 1 - mCount;
}
