#pragma once

struct LightProperties {
	float	m_Attenuation[3];  /* Attenuation coefficients */
	float	m_Range;
	float	m_Falloff;
	float	m_Theta;
	float	m_Phi;
};