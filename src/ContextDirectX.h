// Implementation of Canvas that supports Windows Store 
// and Windows Phone


#include "Style.h"
#include "Context.h"
#include <string>
#include "System\Windows8.1\DeviceResources.h"
#include "System\Windows8.1\StepTimer.h"
#include "System\Windows8.1\pch.h"
#include <memory>

namespace canvas {
	class ContextDirectX : public Context {
	public:
		ContextDirectX(ID2D1DeviceContext *_devicecontext)  {
			CreateDeviceDependentResources();
			devicecontext = _devicecontext;
		}
		
		~ContextDirectX() {

		}

		void clearRect(float x, float y, float w, float h) = 0;
		void fillRect(float x, float y, float w, float h) = 0;
		void moveTo(float x, float y) = 0;
		void lineTo(float x, float y) = 0;
		void stroke() = 0;
		void fill() = 0;

		float lineWidth;

		Style fillStyle;
		Style strokeStyle;

	private:
		void CreateDeviceDependentResources();
		void ReleaseDeviceDependentResources();
		void Update(DX::StepTimer const& timer);
		void Render();

		// Cached pointer to device resources.

		// Resources related to text rendering.
		std::wstring                                    m_text;
		DWRITE_TEXT_METRICS	                            m_textMetrics;
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>    m_whiteBrush;
		Microsoft::WRL::ComPtr<ID2D1DrawingStateBlock>  m_stateBlock;
		Microsoft::WRL::ComPtr<IDWriteTextLayout>       m_textLayout;
		Microsoft::WRL::ComPtr<IDWriteTextFormat>		m_textFormat;

		ID2D1DeviceContext* devicecontext;
	};
};