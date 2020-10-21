#pragma once

#include "Bindable.h"
#include "ConstantBuffer.h"
#include "cbPerFrame.h"
#include "memoryUtility.h"

template <typename T>
class VertexConstantBuffer : public ConstantBuffer<T> {
	using ConstantBuffer<T>::pConstantBuffer;

private:
	unsigned int m_slot;

public:

	VertexConstantBuffer(ID3D11Device* device, const T& consts) : ConstantBuffer<T>::ConstantBuffer(device, consts) {
		m_slot = 0;
	}

	VertexConstantBuffer(ID3D11Device* device, const T& consts, unsigned int slot) : ConstantBuffer<T>::ConstantBuffer(device, consts) {
		m_slot = slot;
	}

	void Bind(ID3D11DeviceContext* deviceContext) override {
		deviceContext->VSSetConstantBuffers(m_slot, 1u, pConstantBuffer.GetAddressOf());
	}
};