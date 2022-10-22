#pragma once

class Input
{
public:
	Input(int code);

	bool isPress();
	bool isDown();
	bool isUp();

	void Update();

private:
	int mCode;
	bool mState[2] = { 0 };
	int mCount;
};
