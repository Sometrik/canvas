// Implementation of Canvas that supports Windows Store 
// and Windows Phone

#include "Style.h"

namespace canvas {
	class ContextDirectX {
	public:
		ContextDirectX() { }
		virtual ~ContextDirectX() { }

		virtual void clearRect(float x, float y, float w, float h) = 0;
		virtual void fillRect(float x, float y, float w, float h) = 0;
		virtual void moveTo(float x, float y) = 0;
		virtual void lineTo(float x, float y) = 0;
		virtual void stroke() = 0;
		virtual void fill() = 0;

		float lineWidth;
		Style fillStyle;
		Style strokeStyle;

	private:
	};
};