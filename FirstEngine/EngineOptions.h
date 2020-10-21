#pragma once

#include <string>
#include <Windows.h>

#include "Renderer.h"

struct EngineOptions {

	// Rendering options
	Renderer	m_Renderer;
	bool		m_runFullSpeed; //V-Sync
	bool		m_fullScreen;
	int			m_screenWidth;
	int			m_screenHeight;
	float		m_screenDepth;
	float		m_screenNear;
	float		m_fov;			// in radians
	float		m_aspectRatio;

	// Sound options
	float m_soundEffectsVolume;
	float m_musicVolume;

	EngineOptions();
	~EngineOptions();

	void Init(const char* xmlFilePath, LPWSTR lpCmdLine);
};
