#include "MatrixStack.h"

MatrixStack::MatrixStack() {
	DirectX::XMFLOAT4X4 identity;
	DirectX::XMStoreFloat4x4(&identity, DirectX::XMMatrixIdentity());
	m_MatrixStack.push_back(identity);
}

// Adds a matrix to the stack.
// This method increments the count of items on the stack by 1, duplicating the current matrix.
// The stack will grow dynamically as more items are added.
void MatrixStack::Push() {
	m_MatrixStack.push_back(m_MatrixStack.back());
}

// Removes the current matrix from the top of the stack.
// Note that this method decrements the count of items on the stack by 1, effectively
// removing the current matrix from the top of the stack and promoting a matrix to the top of the
// stack. If the current count of items on the stack is 0, then this method returns without performing
// any action. If the current count of items on the stack is 1, then this method empties the stack.
void MatrixStack::Pop() {
	if (!m_MatrixStack.empty()) {
		m_MatrixStack.pop_back();
	}
}

// Determines the product of the current matrixand the given matrix.
// This method right-multiplies the given matrix to the current matrix (transformation is about the current world origin
// m_pstack[m_currentPos] = m_pstack[m_currentPos] * (*pMat);
// This method does not add an item to the stack, it replaces the current matrix with the product of the current matrix and the given matrix.
void MatrixStack::MultMatrix(DirectX::CXMMATRIX other) {
	DirectX::XMMATRIX currentMatrix = DirectX::XMLoadFloat4x4(&m_MatrixStack.back());
	DirectX::XMStoreFloat4x4(&m_MatrixStack.back(), DirectX::XMMatrixMultiply(currentMatrix, other));
}

// Determines the product of the current matrixand the given matrix.
// This method right-multiplies the given matrix to the current matrix (transformation is about the current world origin
// m_pstack[m_currentPos] = m_pstack[m_currentPos] * (*pMat);
// This method does not add an item to the stack, it replaces the current matrix with the product of the current matrix and the given matrix.
void MatrixStack::MultMatrix(const DirectX::XMFLOAT4X4& other) {
	DirectX::XMMATRIX currentMatrix = DirectX::XMLoadFloat4x4(&m_MatrixStack.back());
	DirectX::XMMATRIX otherMatrix = DirectX::XMLoadFloat4x4(&other);
	DirectX::XMStoreFloat4x4(&m_MatrixStack.back(), DirectX::XMMatrixMultiply(currentMatrix, otherMatrix));
}

// Determines the product of the given matrix and the current matrix.
// This method left-multiplies the given matrix to the current matrix (transformation is about the local origin of the object).
// m_pstack[m_currentPos] = (*pMat) * m_pstack[m_currentPos];
// This method does not add an item to the stack, it replaces the current matrix with the product of the given matrix and the current matrix.
void MatrixStack::MultMatrixLocal(DirectX::CXMMATRIX other) {
	DirectX::XMMATRIX currentMatrix = DirectX::XMLoadFloat4x4(&m_MatrixStack.back());
	DirectX::XMStoreFloat4x4(&m_MatrixStack.back(), DirectX::XMMatrixMultiply(other, currentMatrix));
}

// Determines the product of the given matrix and the current matrix.
// This method left-multiplies the given matrix to the current matrix (transformation is about the local origin of the object).
// m_pstack[m_currentPos] = (*pMat) * m_pstack[m_currentPos];
// This method does not add an item to the stack, it replaces the current matrix with the product of the given matrix and the current matrix.
void MatrixStack::MultMatrixLocal(const DirectX::XMFLOAT4X4& other) {
	DirectX::XMMATRIX currentMatrix = DirectX::XMLoadFloat4x4(&m_MatrixStack.back());
	DirectX::XMMATRIX otherMatrix = DirectX::XMLoadFloat4x4(&other);
	DirectX::XMStoreFloat4x4(&m_MatrixStack.back(), DirectX::XMMatrixMultiply(otherMatrix, currentMatrix));
}

// Loads identity in the current matrix.
// The identity matrix is a matrix in which all coefficients are 0.0 except the [1,1][2,2][3,3][4,4] coefficients,
// which are set to 1.0. The identity matrix is special in that when it is applied to vertices, they are unchanged.
// The identity matrix is used as the starting point for matrices that will modify vertex values to create rotations,
// translations, and any other transformations that can be represented by a 4x4 matrix.
void MatrixStack::LoadIdentity() {
	DirectX::XMFLOAT4X4 identity;
	DirectX::XMStoreFloat4x4(&m_MatrixStack.back(), DirectX::XMMatrixIdentity());
}

// Loads the given matrix into the current matrix.
void MatrixStack::LoadMatrix(const DirectX::XMFLOAT4X4& other) {
	m_MatrixStack.back() = other;
}

// Loads the given matrix into the current matrix.
void MatrixStack::LoadMatrix(DirectX::CXMMATRIX other) {
	DirectX::XMStoreFloat4x4(&m_MatrixStack.back(), other);
}

// Retrieves the current matrix at the top of the stack.
// Note that this method does not remove the current matrix from the top of the stack; rather, it just returns the current matrix.
DirectX::XMFLOAT4X4& MatrixStack::GetTop() {
	return m_MatrixStack.back();
}

// Rotates (relative to world coordinate space) around an arbitrary axis.
// axis - arbitrary axis of rotation
// angle - Rotation angle about the arbitrary axis, in radians. Angles are measured counterclockwise when looking along the arbitrary axis toward the origin.
// This method adds the rotation to the matrix stack with the computed rotation matrix similar to the following:
// D3DXMATRIX tmp;
// D3DXMatrixRotationAxis(&tmp, pV, angle);
// m_stack[m_currentPos] = m_stack[m_currentPos] * tmp;
// Because the rotation is right-multiplied to the matrix stack, the rotation is relative to world coordinate space.
void MatrixStack::RotateAxis(const DirectX::XMFLOAT3& axis, float angle) {
	DirectX::XMMATRIX currentMatrix = DirectX::XMLoadFloat4x4(&m_MatrixStack.back());
	DirectX::XMVECTOR axis3f = DirectX::XMLoadFloat3(&axis);
	DirectX::XMStoreFloat4x4(
		&m_MatrixStack.back(),
		DirectX::XMMatrixMultiply(
			currentMatrix,
			DirectX::XMMatrixRotationAxis(axis3f, angle)
		)
	);
}

// Rotates (relative to world coordinate space) around an arbitrary axis.
// axis - arbitrary axis of rotation
// angle - Rotation angle about the arbitrary axis, in radians. Angles are measured counterclockwise when looking along the arbitrary axis toward the origin.
// This method adds the rotation to the matrix stack with the computed rotation matrix similar to the following:
// D3DXMATRIX tmp;
// D3DXMatrixRotationAxis(&tmp, pV, angle);
// m_stack[m_currentPos] = m_stack[m_currentPos] * tmp;
// Because the rotation is right-multiplied to the matrix stack, the rotation is relative to world coordinate space.
void MatrixStack::RotateAxis(DirectX::FXMVECTOR axis, float angle) {
	DirectX::XMMATRIX currentMatrix = DirectX::XMLoadFloat4x4(&m_MatrixStack.back());
	DirectX::XMStoreFloat4x4(
		&m_MatrixStack.back(),
		DirectX::XMMatrixMultiply(
			currentMatrix,
			DirectX::XMMatrixRotationAxis(axis, angle)
		)
	);
}

// Rotates (relative to the object's local coordinate space) around an arbitrary axis.
// axis - arbitrary axis of rotation
// angle - Rotation angle about the arbitrary axis, in radians. Angles are measured counterclockwise when looking along the arbitrary axis toward the origin.
// This method adds the rotation to the matrix stack with the computed rotation matrix similar to the following:
// D3DXMATRIX tmp;
// D3DXMatrixRotationAxis(&tmp, pV, angle);
// m_stack[m_currentPos] = tmp * m_stack[m_currentPos];
// Because the rotation is left-multiplied to the matrix stack, the rotation is relative to the object's local coordinate space.
void MatrixStack::RotateAxisLocal(const DirectX::XMFLOAT3& axis, float angle) {
	DirectX::XMMATRIX currentMatrix = DirectX::XMLoadFloat4x4(&m_MatrixStack.back());
	DirectX::XMVECTOR axis3f = DirectX::XMLoadFloat3(&axis);
	DirectX::XMStoreFloat4x4(
		&m_MatrixStack.back(),
		DirectX::XMMatrixMultiply(
			DirectX::XMMatrixRotationAxis(axis3f, angle),
			currentMatrix
		)
	);
}

// Rotates (relative to the object's local coordinate space) around an arbitrary axis.
// axis - arbitrary axis of rotation
// angle - Rotation angle about the arbitrary axis, in radians. Angles are measured counterclockwise when looking along the arbitrary axis toward the origin.
// This method adds the rotation to the matrix stack with the computed rotation matrix similar to the following:
// D3DXMATRIX tmp;
// D3DXMatrixRotationAxis(&tmp, pV, angle);
// m_stack[m_currentPos] = tmp * m_stack[m_currentPos];
// Because the rotation is left-multiplied to the matrix stack, the rotation is relative to the object's local coordinate space.
void MatrixStack::RotateAxisLocal(const DirectX::FXMVECTOR& axis, float angle) {
	DirectX::XMMATRIX currentMatrix = DirectX::XMLoadFloat4x4(&m_MatrixStack.back());
	DirectX::XMStoreFloat4x4(
		&m_MatrixStack.back(),
		DirectX::XMMatrixMultiply(
			DirectX::XMMatrixRotationAxis(axis, angle),
			currentMatrix
		)
	);
}

// Rotates around (relative to world coordinate space).
// The yaw around the y-axis in radians.
// The pitch around the x-axis in radians.
// The roll around the z-axis in radians.
// This method adds the rotation to the matrix stack with the computed rotation matrix similar to the following:
// D3DXMATRIX tmp;
// D3DXMatrixRotationYawPitchRoll(&tmp, yaw, pitch, roll);
// m_stack[m_currentPos] = m_stack[m_currentPos] * tmp;
// Because the rotation is right-multiplied to the matrix stack, the rotation is relative to world coordinate space.
void MatrixStack::RotateYawPitchRoll(float yaw, float pitch, float roll) {
	DirectX::XMMATRIX currentMatrix = DirectX::XMLoadFloat4x4(&m_MatrixStack.back());
	DirectX::XMStoreFloat4x4(
		&m_MatrixStack.back(),
		DirectX::XMMatrixMultiply(
			currentMatrix,
			DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll)
		)
	);
}

// Rotates around (relative to world coordinate space).
// The yaw around the y-axis in radians.
// The pitch around the x-axis in radians.
// The roll around the z-axis in radians.
// This method adds the rotation to the matrix stack with the computed rotation matrix similar to the following:
// D3DXMATRIX tmp;
// D3DXMatrixRotationYawPitchRoll(&tmp, yaw, pitch, roll);
// m_stack[m_currentPos] = tmp * m_stack[m_currentPos];
// Because the rotation is left-multiplied to the matrix stack, the rotation is relative to the object's local coordinate space.
void MatrixStack::RotateYawPitchRollLocal(float yaw, float pitch, float roll) {
	DirectX::XMMATRIX currentMatrix = DirectX::XMLoadFloat4x4(&m_MatrixStack.back());
	DirectX::XMStoreFloat4x4(
		&m_MatrixStack.back(),
		DirectX::XMMatrixMultiply(
			DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll),
			currentMatrix
		)
	);
}

// Scale the current matrix about the world coordinate origin.
// This method right-multiplies the current matrix with the computed scale matrix. The transformation is about the current world origin.
// D3DXMATRIX tmp;
// D3DXMatrixScaling(&tmp, x, y, z);
// m_stack[m_currentPos] = m_stack[m_currentPos] * tmp;
void MatrixStack::Scale(float x, float y, float z) {
	DirectX::XMMATRIX currentMatrix = DirectX::XMLoadFloat4x4(&m_MatrixStack.back());
	DirectX::XMStoreFloat4x4(
		&m_MatrixStack.back(),
		DirectX::XMMatrixMultiply(
			currentMatrix,
			DirectX::XMMatrixScaling(x, y, z)
		)
	);
}

// Scale the current matrix about the object origin.
// This method left-multiplies the current matrix with the computed scale matrix. The transformation is about the local origin of the object.
// D3DXMATRIX tmp;
// D3DXMatrixScaling(&tmp, x, y, z);
// m_stack[m_currentPos] = tmp * m_stack[m_currentPos];
void MatrixStack::ScaleLocal(float x, float y, float z) {
	DirectX::XMMATRIX currentMatrix = DirectX::XMLoadFloat4x4(&m_MatrixStack.back());
	DirectX::XMStoreFloat4x4(
		&m_MatrixStack.back(),
		DirectX::XMMatrixMultiply(
			DirectX::XMMatrixScaling(x, y, z),
			currentMatrix
		)
	);
}

// Determines the product of the current matrix and the computed translation matrix determined by the given factors (x, y, and z)
// This method right-multiplies the current matrix with the computed translation matrix (transformation is about the current world origin).
// D3DXMATRIX tmp;
// D3DXMatrixTranslation(&tmp, x, y, z);
// m_stack[m_currentPos] = m_stack[m_currentPos] * tmp;
void MatrixStack::Translate(float x, float y, float z) {
	DirectX::XMMATRIX currentMatrix = DirectX::XMLoadFloat4x4(&m_MatrixStack.back());
	DirectX::XMStoreFloat4x4(
		&m_MatrixStack.back(),
		DirectX::XMMatrixMultiply(
			currentMatrix,
			DirectX::XMMatrixTranslation(x, y, z)
		)
	);
}

// Determines the product of the computed translation matrix determined by the given factors (x, y, and z) and the current matrix.
// This method left-multiplies the current matrix with the computed translation matrix (transformation is about the local origin of the object).
// D3DXMATRIX tmp;
// D3DXMatrixTranslation(&tmp, x, y, z);
// m_stack[m_currentPos] = tmp * m_stack[m_currentPos];
void MatrixStack::TranslateLocal(float x, float y, float z) {
	DirectX::XMMATRIX currentMatrix = DirectX::XMLoadFloat4x4(&m_MatrixStack.back());
	DirectX::XMStoreFloat4x4(
		&m_MatrixStack.back(),
		DirectX::XMMatrixMultiply(
			DirectX::XMMatrixTranslation(x, y, z),
			currentMatrix
		)
	);
}

std::ostream& operator<<(std::ostream& os, const MatrixStack& mstack) {
	std::ios::fmtflags oldFlag = os.flags();
	int counter = 0;
	for (const auto& m : mstack.m_MatrixStack) {
		os << ++counter << ") Matrix number" << std::endl;
		os << m;
	}
	os.flags(oldFlag);
	return os;
}