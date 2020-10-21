#pragma once

#include <stack>
#include <DirectXMath.h>

class MatrixStack {

	std::stack<DirectX::XMFLOAT4X4> m_MatrixStack;

public:

	MatrixStack() {
		DirectX::XMFLOAT4X4 identity;
		DirectX::XMStoreFloat4x4(&identity, DirectX::XMMatrixIdentity());
		m_MatrixStack.push(identity);
	}

	void Push() {
		m_MatrixStack.push(m_MatrixStack.top());
	}

	void Pop() {
		m_MatrixStack.pop();
	}

	void MultMatrixLocal(const DirectX::XMFLOAT4X4& other) {
		DirectX::XMMATRIX currentMatrix = DirectX::XMLoadFloat4x4(&m_MatrixStack.top());
		DirectX::XMMATRIX otherMatrix = DirectX::XMLoadFloat4x4(&other);
		DirectX::XMStoreFloat4x4(&m_MatrixStack.top(), DirectX::XMMatrixMultiply(currentMatrix, otherMatrix));
	}

	DirectX::XMFLOAT4X4 GetTop() {
		return m_MatrixStack.top();
	}
};