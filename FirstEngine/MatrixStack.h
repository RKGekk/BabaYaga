#pragma once

#include <stack>
#include <iostream>
#include <DirectXMath.h>

#include "stringUtility.h"

// Supplies a mechanism to enable matrices to be pushed onto and popped off of a matrix stack.
// Implementing a matrix stack is an efficient way to track matrices while traversing a transform hierarchy.
// Matrix stack to store transformations as matrices.
// Implementing a Scene Hierarchy
// A matrix stack simplifies the construction of hierarchical models, in which complicated objects are
// constructed from a series of simpler objects.
// A scene, or transform, hierarchy is usually represented by a tree data structure.Each node in the tree
// data structure contains a matrix.A particular matrix represents the change in coordinate systems from
// the node's parent to the node. For example, if you are modeling a human arm, you might implement the following hierarchy.
// In this hierarchy, the Body matrix places the body in the world.The UpperArm matrix contains the rotation
// of the shoulder, the LowerArm matrix contains the rotation of the elbow, and the Hand matrix contains
// the rotation of the wrist.To determine where the hand is relative to the world, you simply multiply
// all the matrices from Body down through Hand together.
// The previous hierarchy is overly simplistic, because each node has only one child.If you begin to model
// the hand in more detail, you will probably add fingersand a thumb. Each digit can be added to the
// hierarchy as children of Hand.
// If you traverse the complete graph of the arm in depth - first order-traversing as far down one path
// as possible before moving on to the next path—to draw the scene, you perform a sequence of segmented
// rendering.For example, to render the handand fingers, you implement the following pattern.
// 1.  Push the Hand matrix onto the matrix stack.
// 2.  Draw the hand.
// 3.  Push the Thumb matrix onto the matrix stack.
// 4.  Draw the thumb.
// 5.  Pop the Thumb matrix off the stack.
// 6.  Push the Finger 1 matrix onto the matrix stack.
// 7.  Draw the first finger.
// 8.  Pop the Finger 1 matrix off the stack.
// 9.  Push the Finger 2 matrix onto the matrix stack.You continue in this manner until all the fingersand thumb are rendered.
// After you have completed rendering the fingers, you would pop the Hand matrix off the stack.
// You can follow this basic process in code with the following examples.When you encounter a node
// during the depth - first search of the tree data structure, push the matrix onto the top of the matrix stack.
// MatrixStack->Push();
// MatrixStack->MultMatrix(pNode->matrix);
// When you are finished with a node, pop the matrix off the top of the matrix stack.
// MatrixStack->Pop();
// In this way, the matrix on the top of the stack always represents the world - transform of the current
// node.Therefore, before drawing each node, you should set matrix.
class MatrixStack {

	std::deque<DirectX::XMFLOAT4X4> m_MatrixStack;

public:

	MatrixStack();

	// Adds a matrix to the stack.
	// This method increments the count of items on the stack by 1, duplicating the current matrix.
	// The stack will grow dynamically as more items are added.
	void Push();

	// Removes the current matrix from the top of the stack.
	// Note that this method decrements the count of items on the stack by 1, effectively
	// removing the current matrix from the top of the stack and promoting a matrix to the top of the
	// stack. If the current count of items on the stack is 0, then this method returns without performing
	// any action. If the current count of items on the stack is 1, then this method empties the stack.
	void Pop();

	// Determines the product of the current matrixand the given matrix.
	// This method right-multiplies the given matrix to the current matrix (transformation is about the current world origin
	// m_pstack[m_currentPos] = m_pstack[m_currentPos] * (*pMat);
	// This method does not add an item to the stack, it replaces the current matrix with the product of the current matrix and the given matrix.
	void MultMatrix(DirectX::CXMMATRIX other);

	// Determines the product of the current matrixand the given matrix.
	// This method right-multiplies the given matrix to the current matrix (transformation is about the current world origin
	// m_pstack[m_currentPos] = m_pstack[m_currentPos] * (*pMat);
	// This method does not add an item to the stack, it replaces the current matrix with the product of the current matrix and the given matrix.
	void MultMatrix(const DirectX::XMFLOAT4X4& other);

	// Determines the product of the given matrix and the current matrix.
	// This method left-multiplies the given matrix to the current matrix (transformation is about the local origin of the object).
	// m_pstack[m_currentPos] = (*pMat) * m_pstack[m_currentPos];
	// This method does not add an item to the stack, it replaces the current matrix with the product of the given matrix and the current matrix.
	void MultMatrixLocal(DirectX::CXMMATRIX other);

	// Determines the product of the given matrix and the current matrix.
	// This method left-multiplies the given matrix to the current matrix (transformation is about the local origin of the object).
	// m_pstack[m_currentPos] = (*pMat) * m_pstack[m_currentPos];
	// This method does not add an item to the stack, it replaces the current matrix with the product of the given matrix and the current matrix.
	void MultMatrixLocal(const DirectX::XMFLOAT4X4& other);

	// Loads identity in the current matrix.
	// The identity matrix is a matrix in which all coefficients are 0.0 except the [1,1][2,2][3,3][4,4] coefficients,
	// which are set to 1.0. The identity matrix is special in that when it is applied to vertices, they are unchanged.
	// The identity matrix is used as the starting point for matrices that will modify vertex values to create rotations,
	// translations, and any other transformations that can be represented by a 4x4 matrix.
	void LoadIdentity();

	// Loads the given matrix into the current matrix.
	void LoadMatrix(const DirectX::XMFLOAT4X4& other);

	// Loads the given matrix into the current matrix.
	void LoadMatrix(DirectX::CXMMATRIX other);

	// Retrieves the current matrix at the top of the stack.
	// Note that this method does not remove the current matrix from the top of the stack; rather, it just returns the current matrix.
	DirectX::XMFLOAT4X4& GetTop();

	// Rotates (relative to world coordinate space) around an arbitrary axis.
	// axis - arbitrary axis of rotation
	// angle - Rotation angle about the arbitrary axis, in radians. Angles are measured counterclockwise when looking along the arbitrary axis toward the origin.
	// This method adds the rotation to the matrix stack with the computed rotation matrix similar to the following:
	// D3DXMATRIX tmp;
	// D3DXMatrixRotationAxis(&tmp, pV, angle);
	// m_stack[m_currentPos] = m_stack[m_currentPos] * tmp;
	// Because the rotation is right-multiplied to the matrix stack, the rotation is relative to world coordinate space.
	void RotateAxis(const DirectX::XMFLOAT3& axis, float angle);

	// Rotates (relative to world coordinate space) around an arbitrary axis.
	// axis - arbitrary axis of rotation
	// angle - Rotation angle about the arbitrary axis, in radians. Angles are measured counterclockwise when looking along the arbitrary axis toward the origin.
	// This method adds the rotation to the matrix stack with the computed rotation matrix similar to the following:
	// D3DXMATRIX tmp;
	// D3DXMatrixRotationAxis(&tmp, pV, angle);
	// m_stack[m_currentPos] = m_stack[m_currentPos] * tmp;
	// Because the rotation is right-multiplied to the matrix stack, the rotation is relative to world coordinate space.
	void RotateAxis(DirectX::FXMVECTOR axis, float angle);

	// Rotates (relative to the object's local coordinate space) around an arbitrary axis.
	// axis - arbitrary axis of rotation
	// angle - Rotation angle about the arbitrary axis, in radians. Angles are measured counterclockwise when looking along the arbitrary axis toward the origin.
	// This method adds the rotation to the matrix stack with the computed rotation matrix similar to the following:
	// D3DXMATRIX tmp;
	// D3DXMatrixRotationAxis(&tmp, pV, angle);
	// m_stack[m_currentPos] = tmp * m_stack[m_currentPos];
	// Because the rotation is left-multiplied to the matrix stack, the rotation is relative to the object's local coordinate space.
	void RotateAxisLocal(const DirectX::XMFLOAT3& axis, float angle);

	// Rotates (relative to the object's local coordinate space) around an arbitrary axis.
	// axis - arbitrary axis of rotation
	// angle - Rotation angle about the arbitrary axis, in radians. Angles are measured counterclockwise when looking along the arbitrary axis toward the origin.
	// This method adds the rotation to the matrix stack with the computed rotation matrix similar to the following:
	// D3DXMATRIX tmp;
	// D3DXMatrixRotationAxis(&tmp, pV, angle);
	// m_stack[m_currentPos] = tmp * m_stack[m_currentPos];
	// Because the rotation is left-multiplied to the matrix stack, the rotation is relative to the object's local coordinate space.
	void RotateAxisLocal(const DirectX::FXMVECTOR& axis, float angle);

	// Rotates around (relative to world coordinate space).
	// The yaw around the y-axis in radians.
	// The pitch around the x-axis in radians.
	// The roll around the z-axis in radians.
	// This method adds the rotation to the matrix stack with the computed rotation matrix similar to the following:
	// D3DXMATRIX tmp;
	// D3DXMatrixRotationYawPitchRoll(&tmp, yaw, pitch, roll);
	// m_stack[m_currentPos] = m_stack[m_currentPos] * tmp;
	// Because the rotation is right-multiplied to the matrix stack, the rotation is relative to world coordinate space.
	void RotateYawPitchRoll(float yaw, float pitch, float roll);

	// Rotates around (relative to world coordinate space).
	// The yaw around the y-axis in radians.
	// The pitch around the x-axis in radians.
	// The roll around the z-axis in radians.
	// This method adds the rotation to the matrix stack with the computed rotation matrix similar to the following:
	// D3DXMATRIX tmp;
	// D3DXMatrixRotationYawPitchRoll(&tmp, yaw, pitch, roll);
	// m_stack[m_currentPos] = tmp * m_stack[m_currentPos];
	// Because the rotation is left-multiplied to the matrix stack, the rotation is relative to the object's local coordinate space.
	void RotateYawPitchRollLocal(float yaw, float pitch, float roll);

	// Scale the current matrix about the world coordinate origin.
	// This method right-multiplies the current matrix with the computed scale matrix. The transformation is about the current world origin.
	// D3DXMATRIX tmp;
	// D3DXMatrixScaling(&tmp, x, y, z);
	// m_stack[m_currentPos] = m_stack[m_currentPos] * tmp;
	void Scale(float x, float y, float z);

	// Scale the current matrix about the object origin.
	// This method left-multiplies the current matrix with the computed scale matrix. The transformation is about the local origin of the object.
	// D3DXMATRIX tmp;
	// D3DXMatrixScaling(&tmp, x, y, z);
	// m_stack[m_currentPos] = tmp * m_stack[m_currentPos];
	void ScaleLocal(float x, float y, float z);

	// Determines the product of the current matrix and the computed translation matrix determined by the given factors (x, y, and z)
	// This method right-multiplies the current matrix with the computed translation matrix (transformation is about the current world origin).
	// D3DXMATRIX tmp;
	// D3DXMatrixTranslation(&tmp, x, y, z);
	// m_stack[m_currentPos] = m_stack[m_currentPos] * tmp;
	void Translate(float x, float y, float z);

	// Determines the product of the computed translation matrix determined by the given factors (x, y, and z) and the current matrix.
	// This method left-multiplies the current matrix with the computed translation matrix (transformation is about the local origin of the object).
	// D3DXMATRIX tmp;
	// D3DXMatrixTranslation(&tmp, x, y, z);
	// m_stack[m_currentPos] = tmp * m_stack[m_currentPos];
	void TranslateLocal(float x, float y, float z);

	friend std::ostream& operator<<(std::ostream& os, const MatrixStack& mstack);
};