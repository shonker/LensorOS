#include "keyboard.h"

uVector2 gTextPosition {0, 0};

bool isCAPS;
bool isLSHIFT;
bool isRSHIFT;

void handle_keyboard(uint8_t scancode) {
	uVector2 cachedPos = gRend.DrawPos;
	switch (scancode) {
	case LSHIFT:
		isLSHIFT = true;
		return;
	case LSHIFT + 0x80:
		isLSHIFT = false;
		return;
	case RSHIFT:
		isRSHIFT = true;
		return;
	case RSHIFT + 0x80:
		isRSHIFT = false;
		return;
	case ENTER:
		gRend.DrawPos = gTextPosition;
		gRend.crlf();
		gTextPosition = gRend.DrawPos;
		gRend.DrawPos = cachedPos;
		return;
	case ENTER + 0x80:
		return;
	case BACKSPACE:
		gRend.DrawPos = gTextPosition;
	 	gRend.clearchar();
		gTextPosition = gRend.DrawPos;
		gRend.swap(gTextPosition, {8, 24});
		gRend.DrawPos = cachedPos;
	 	return;
	case BACKSPACE + 0x80:
	 	return;
	case SPACE:
		gRend.DrawPos = gTextPosition;
		gRend.putchar(' ');
		gRend.swap(gTextPosition, {8, 24});
		gTextPosition = gRend.DrawPos;
		gRend.DrawPos = cachedPos;
		return;
	case SPACE + 0x80:
		return;
	case CAPSLOCK:
		isCAPS = !isCAPS;
		return;
	case CAPSLOCK + 0x80:
		return;
	}

	char ascii = QWERTY::Translate(scancode, isLSHIFT | isRSHIFT | isCAPS);
	if (ascii != 0) {
		gRend.DrawPos = gTextPosition;
		gRend.putchar(ascii);
		gRend.swap(gTextPosition, {8, 24});
		gTextPosition = gRend.DrawPos;
		gRend.DrawPos = cachedPos;
	}
}
